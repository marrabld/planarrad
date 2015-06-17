
// visualisationoptionsdialog.cc

#include "visualisationoptionsdialog.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QButtonGroup>
#include <QPushButton>
#include <QFileDialog>
#include "base/jlog.h"
#include "fe/tool_gui.h"

using namespace jude::fe;



VisualisationOptionsDialog::VisualisationOptionsDialog(QWidget* p) : 
  QDialog(p) {

  setCaption("Visualisation Options");

  //QLabel* label;
  QGridLayout* grid;
  QGroupBox *gb;

  QVBoxLayout* vl_top = new QVBoxLayout(this);

  gb = new QGroupBox(tr("Profile Plot"));
  vl_top->addWidget(gb);
  grid = new QGridLayout(gb);
  grid->setSpacing(4);
  grid->setMargin(8);
  grid->setColStretch(3, 100);
 
  auto_depth_range_rb = new QRadioButton(tr("Automatic depth range"));
  button_grp.addButton(auto_depth_range_rb);
  grid->addWidget(auto_depth_range_rb, 0, 0);

  fixed_depth_range_rb = new QRadioButton(tr("Fixed depth range"));
  button_grp.addButton(fixed_depth_range_rb);
  grid->addWidget(fixed_depth_range_rb, 1, 0);

  fixed_max_depth_le = new QLineEdit();
  grid->addWidget(fixed_max_depth_le, 1, 1);

  fixed_max_depth_label = new QLabel("(m)");
  grid->addWidget(fixed_max_depth_label, 1, 2);


  QPushButton* cancel_pb = new QPushButton(tr("Cancel"));
  QPushButton* ok_pb = new QPushButton(tr("OK"));

  QHBoxLayout* hl = new QHBoxLayout;
  vl_top->insertLayout(-1,hl);
  hl->insertStretch(100);
  hl->addWidget(cancel_pb);
  hl->addWidget(ok_pb);

  connect(cancel_pb, SIGNAL(pressed()), this, SLOT(reject()));
  connect(ok_pb, SIGNAL(pressed()), this, SLOT(accept()));
 
  connect(fixed_max_depth_le, SIGNAL(returnPressed()), this, SLOT(accept()));

  connect(&button_grp, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slotUpdateEnabled()));
}



// VisualisationOptionsDialog::saveSettings()

void VisualisationOptionsDialog::saveSettings(const JString& prefix, QSettings& settings) const {

    // will collect parameters for this widget
  JParser prm;
    // add sky widget parameters
  guiToParser(prefix, prm, SAVE);
    // save to settings - uses the prefix here as well so the parser doesn't read unnecessary parameters
  Tool_Gui::parserAllToSettings(prm, prefix+"params", settings);
    // log for debugging
  if (jlog::vb(5)) prm.displayList(jlog::ls);
}



// VisualisationOptionsDialog::restoreSettings()

void VisualisationOptionsDialog::restoreSettings(const JString& prefix, const QSettings& settings) {

    // parameters for this widget - uses the prefix here as well so the parser doesn't read unnecessary parameters
  JParser prm;
  Tool_Gui::settingsToParserAll(prm, prefix+"params", settings);
    // set parameters from the parser
  parserToGui(prefix, prm, SAVE);
     // log for debugging
  if (jlog::vb(5)) prm.displayList(jlog::ls);
}



// VisualisationOptionsDialog::parserToGui()

void VisualisationOptionsDialog::parserToGui(const JString& prefix, const JParser& prm, prm_context pc) {

  JString s;

  if (prm.isDefinedAndTrueBool(prefix+"fixed_depth_range")) fixed_depth_range_rb->setChecked(true);
  else auto_depth_range_rb->setChecked(true);

  s = prm.getStringOrEmpty(prefix+"fixed_max_depth");
  fixed_max_depth_le->setText(J2Q(s));

  slotUpdateEnabled();
}



// VisualisationOptionsDialog::guiToParser()

void VisualisationOptionsDialog::guiToParser(const JString& prefix, JParser& prm, prm_context pc) const {

  JString s;

  if (fixed_depth_range_rb->isChecked()) prm.setLabelAndValue(prefix+"fixed_depth_range", "yes");
  s = fixed_max_depth_le->text();
  if (!s.isEmpty()) prm.setLabelAndValue(prefix+"fixed_max_depth", Q2J(s));
}



// VisualisationOptionsDialog::slotUpdateEnabled()

void VisualisationOptionsDialog::slotUpdateEnabled() {
 
  bool f = fixed_depth_range_rb->isChecked();
  fixed_max_depth_le->setEnabled(f);
  fixed_max_depth_label->setEnabled(f);
}


