
// tool_gui.cc

#include "fe/tool_gui.h"
#include "tool/envvars.h"
#include "rad/bandspec.h"
#include "ds/direcspec.h"

#include <QMessageBox>

using namespace jude::fe;
using namespace jude::rad;
using namespace jude::ds;



Tool_Gui::Tool_Gui(JParser& prm, QWidget* parent) :
  QMainWindow(parent),
  outer_prm(prm) {

    // these are set in the main() function and need to be propogated to the basic tool
  temp_dir = outer_prm.getStringOrEmpty(ENV_TEMP_DIR);
  files_dir = outer_prm.getStringOrEmpty(ENV_FILES_DIR);
  band_spec_dir = outer_prm.getStringOrEmpty(ENV_BANDSPEC_DIR);
  direc_spec_dir = outer_prm.getStringOrEmpty(ENV_DIRECSPEC_DIR);

    // error messages will be stored here for delivering in message boxes
  jlog::es.addDevice(&es_store);
}



// Tool_Gui::sizePolicy()

QSizePolicy Tool_Gui::sizePolicy() const {
  return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}



// Tool_Gui::saveAllSettings()

void Tool_Gui::saveAllSettings() {
  QSettings settings("jude", toolID());
  settings.clear();
  saveSettings(settings);
}



// Tool_Gui::restoreAllSettings()

void Tool_Gui::restoreAllSettings() {
  QSettings settings("jude", toolID());

  bool last_init_failed = settings.value("gui/init_failed").toBool();

  if (last_init_failed) {
    if (jlog::vb(1)) jlog::ls << "It seems the last run failed during initialisation, so settimngs have been cleared.\n";
    settings.clear();
    settings.sync();

  } else {
    settings.setValue("gui/init_failed", true);
    settings.sync();
    restoreSettings(settings);
  }
}



void Tool_Gui::clearInitialisationFlag() {
  QSettings settings("jude", toolID());
  settings.setValue("gui/init_failed", false);
  settings.sync();
}



// Tool_Gui::closeEvent()

//void Tool_Gui::closeEvent(QCloseEvent *event) {
//  jlog::ls << "Close Event\n";
//  QSettings settings("jude", toolID());
//  settings.clear();
//  saveSettings(settings);
//  QMainWindow::closeEvent(event);
//}



void Tool_Gui::saveSettings(QSettings& settings) const {
  settings.setValue("main_window/geometry", saveGeometry());
}



void Tool_Gui::restoreSettings(const QSettings& settings) {
  restoreGeometry(settings.value("main_window/geometry").toByteArray());
}



// Tool_Gui::parserAllToSettings() [static]

void Tool_Gui::parserAllToSettings(const JParser& prm, const JString& group, QSettings& settings) {
  for (int i=0; i<prm.paramCount(); i++) {
    settings.setValue((group+"/"+prm.getName(i)).latin1(), prm.getValue(i).latin1());
  }
}



// Tool_Gui::settingsToParserAll() [static]

void Tool_Gui::settingsToParserAll(JParser& prm, const JString& group, const QSettings& settings) {

  QStringList keys = settings.allKeys();

  QString group_qs(J2Q(group));
  if (!group_qs.isEmpty()) group_qs += "/";

  for (int i=0; i<keys.size(); i++) {
    QString label(keys.at(i));
    if (label.left(group_qs.length()) != group_qs) continue; 
    prm.setLabelAndValue(Q2J(label.right(label.length() - group_qs.length())), Q2J(settings.value(label.latin1()).toString()));
  }
}



// Tool_Gui::guiToParser()

void Tool_Gui::guiToParser() {

    // these may be passed from environmental variables or run parameters
  if (!temp_dir.isEmpty()) inner_prm.setLabelAndValue("temp_dir", temp_dir);
  if (!files_dir.isEmpty()) inner_prm.setLabelAndValue("files_dir", files_dir);
  if (!band_spec_dir.isEmpty()) inner_prm.setLabelAndValue("band_spec_dir", band_spec_dir);
  if (!direc_spec_dir.isEmpty()) inner_prm.setLabelAndValue("direc_spec_dir", direc_spec_dir);
}



void Tool_Gui::initBandSpecList() {
  BandSpec::initAvailableList();
  //BandSpec::addToAvailableList("");
  BandSpec::addToAvailableList(bandSpecDir());
  //BandSpec::addToAvailableList("./");
}



void Tool_Gui::initDirecSpecList() {
  DirecSpec::initAvailableList();
  //DirecSpec::addToAvailableList("");
  DirecSpec::addToAvailableList(direcSpecDir());
  //DirecSpec::addToAvailableList("./");
}
 


void Tool_Gui::reportError() {
  QMessageBox mb(this);
  mb.setText(J2Q(es_store.currentText()));
  mb.setIcon(QMessageBox::Critical);
  mb.exec();
  es_store.clear();
}

