
// planarrad.h

#include "planarrad.h"
#include "base/jparser.h"
#include <QLayout>
#include <QDir>
#include <QMenuBar>
#include <QAction>
#include <QCoreApplication>
#include <QFileDialog>
#include "aboutdialog.h"
#include "additionaloptionsdialog.h"
#include "visualisationoptionsdialog.h"



// PlanarRad::(constructor)

PlanarRad::PlanarRad(JParser& prm, QWidget* parent) :
  Tool_Gui(prm, parent),
  skytool_thread(skytool, skytool_prm),
  slabtool_thread(skytool, slabtool_prm),
  is_processing(false) {

  setCaption("PlanarRad ("+current_dir_qs+")");
  setMinimumWidth(400);

    // this tool uses BandSpecs so intialise list and try and find any from file
  Tool_Gui::initBandSpecList();
    // add any BandSpecs specified in the parameters to the available list making the first the default
  BandSpec::addToAvailableList(outer_prm, "", true);
  BandSpec::addToAvailableList(outer_prm, "total_", false);
  BandSpec::addToAvailableList(outer_prm, "diffuse_", false);
    // this will add a default band spec in the event failed to find any
  BandSpec::addDefaultToAvailableList();

    // this tool uses DirecSpecs so initialise list and try and find any from file
  Tool_Gui::initDirecSpecList();
    // add a DirecSpec specified in the parameters to the available list making it the default
  DirecSpec::addToAvailableList(outer_prm, "", true);
    // this will add a default directional spec in the event faild to find any
  DirecSpec::addDefaultToAvailableList();

    // first create the additional options dialog
  additional_options = new AdditionalOptionsDialog(this);
    // first create the additional options dialog
  vis_options = new VisualisationOptionsDialog(this);

    // split horzointally with controls on the left and chart panels on the right
  h_split = new QSplitter(this);
  h_split->setOrientation(Qt::Horizontal);
  setCentralWidget(h_split);

    // container widget for control area
  QWidget* w = new QWidget();
  //w->setMinimumWidth(620);
  h_split->addWidget(w);
    // vertical layout
  QVBoxLayout* vl = new QVBoxLayout(w);
    // tabs for model versus data
  tabs = new QTabWidget(w);
  vl->addWidget(tabs);
    // model tab
  model_widget = new ModelWidget(*additional_options, tabs);
  tabs->addTab(model_widget, "Model");
    // data tab
  data_widget = new DataWidget(data_model, tabs);
  tabs->addTab(data_widget, "Data");

  h_split2 = new QSplitter(h_split);
  h_split2->setOrientation(Qt::Horizontal);
  h_split->addWidget(h_split2);

    // container widget vertical profile
  w = new QWidget();
  h_split2->addWidget(w);
    // vertical layout
  vl = new QVBoxLayout(w);
    // tabs for model versus data
  profile_widget = new ProfileWidget(*vis_options, data_model, w);
  vl->addWidget(profile_widget);

    // container widget for charts
  w = new QWidget();
  h_split2->addWidget(w);
    // vertical layout
  vl = new QVBoxLayout(w);
    // tabs for model versus data
  panel_scroll = new PlotPanelScroll(w);
  vl->addWidget(panel_scroll);

  spec_widget = new SpecWidget(*vis_options, data_model, w);
  //spec_widget->setAspectRatio(1.1);
  //spec_widget->setFixedAdditionalHeight(25);
  panel_scroll->insertPanel(spec_widget, 0);

  hemi_widget = new HemiWidget(*vis_options, data_model, w);
  hemi_widget->setAspectRatio(1);
  hemi_widget->setFixedAdditionalHeight(30);
  panel_scroll->insertPanel(hemi_widget, 0);
  //panel_scroll->resize(212, panel_scroll->height());
 
    // menu
  createMenus();

  connect(model_widget, SIGNAL(signalReportError()), this, SLOT(slotReportError()));
  connect(model_widget, SIGNAL(signalIsProcessing(bool)), this, SLOT(slotIsProcessing(bool)));
  connect(model_widget, SIGNAL(signalProcessingFinishedOk()), this, SLOT(slotProcessingFinishedOk()));

  connect(profile_widget, SIGNAL(signalCurrentDepthChanged(double)), this, SLOT(slotCurrentDepthChanged(double)));
  connect(profile_widget, SIGNAL(signalCurrentDepthChanged(double)), spec_widget, SLOT(slotCurrentDepthChanged(double)));
  connect(profile_widget, SIGNAL(signalCurrentDepthChanged(double)), hemi_widget, SLOT(slotCurrentDepthChanged(double)));

  connect(data_widget, SIGNAL(signalDataSetsChanged()), this, SLOT(slotDataSetsChanged()));
  connect(data_widget, SIGNAL(signalDataSetsChanged()), spec_widget, SLOT(slotDataSetsChanged()));
  connect(data_widget, SIGNAL(signalDataSetsChanged()), hemi_widget, SLOT(slotDataSetsChanged()));

  connect(data_widget, SIGNAL(signalReportError()), this, SLOT(slotReportError()));
}



// PlanarRad::saveSettings()

void PlanarRad::saveSettings(QSettings& settings) const {

  Tool_Gui::saveSettings(settings);

  //jlog::ls << "save\n";
 
    // state of the horizontal splitter
  settings.setValue("main_window/h_split", h_split->saveState());
  settings.setValue("main_window/h_split2", h_split2->saveState());
    // current tab 
  settings.setValue("main_window/tabs", tabs->currentIndex());
    // current directory opening and saving files in
  settings.setValue("gui/current_dir",  current_dir_qs);

  model_widget->saveSettings("", settings);
  additional_options->saveSettings("", settings);
  vis_options->saveSettings("vod", settings);
}



// PlanarRad::restoreSettings()

void PlanarRad::restoreSettings(const QSettings& settings) {

  Tool_Gui::restoreSettings(settings);

  if (!settings.contains("main_window/h_split")) {

    //jlog::ls << "no restore\n";
    resize(1180, 680);
    QList<int> size_list;
    size_list.append(580);
    size_list.append(500);
    h_split->setSizes(size_list);
    size_list.clear();
    size_list.append(250);
    size_list.append(220);
    h_split2->setSizes(size_list);
    current_dir_qs = QDir::currentPath();
  } else {  

    //jlog::ls << "restore\n";
      // state of the horizontal splitter
    h_split->restoreState(settings.value("main_window/h_split").toByteArray());
    h_split2->restoreState(settings.value("main_window/h_split").toByteArray());
      // selected tab
    tabs->setCurrentIndex(settings.value("main_window/tabs").toInt());
      // current directory opening and saving files in
    current_dir_qs = settings.value("gui/current_dir").toString();
  }

  updateCurrentDir(Q2J(current_dir_qs)+"/dummy");

  model_widget->restoreSettings("", settings);
  additional_options->restoreSettings("", settings);
  vis_options->restoreSettings("vod", settings);

  profile_widget->slotUpdateMaxDepth();
}



// PlanarRad::init()

void PlanarRad::init() {
  // nothing to do
}



// PlanarRad::createMenus()

void PlanarRad::createMenus() {

  QAction* act;

  file_menu = menuBar()->addMenu("File");
  proc_menu = menuBar()->addMenu("Processing");
  vis_menu = menuBar()->addMenu("Visualisation");
  help_menu = menuBar()->addMenu("Help");
 
  // file menu

  act = new QAction(tr("&Load Model Parameters"), this);
  act->setShortcut(tr("Ctrl+L"));
  act->setStatusTip(tr("Load model parameters file"));
  connect(act, SIGNAL(triggered()), this, SLOT(slotLoadModelParameters()));
  file_menu->addAction(act);

  act = new QAction(tr("&Save Model Parameters"), this);
  act->setShortcut(tr("Ctrl+S"));
  act->setStatusTip(tr("Save model parameters file"));
  connect(act, SIGNAL(triggered()), this, SLOT(slotSaveModelParameters()));
  file_menu->addAction(act);

  file_menu->addSeparator();

  act = new QAction(tr("&Change Current Working Directory"), this);
  act->setShortcut(tr("Ctrl+W"));
  act->setStatusTip(tr("Change current working directory"));
  connect(act, SIGNAL(triggered()), this, SLOT(slotChangeCurrentDir()));
  file_menu->addAction(act);

  file_menu->addSeparator();

  act = new QAction(tr("&Load Data File"), this);
  act->setShortcut(tr("Ctrl+D"));
  act->setStatusTip(tr("Load data file"));
  connect(act, SIGNAL(triggered()), data_widget, SLOT(slotLoad()));
  file_menu->addAction(act);

  file_menu->addSeparator();

  act = new QAction(tr("&Quit"), this);
  act->setStatusTip(tr("Close the application"));
  connect(act, SIGNAL(triggered()), this, SLOT(slotQuit()));
  file_menu->addAction(act);

  // processing menu
  
  act = new QAction(tr("Build Input Radiance File"), this);
  //act->setShortcut(tr("Ctrl+R"));
  act->setStatusTip(tr("Process the input radiance parameters and build as a file"));
  connect(act, SIGNAL(triggered()), model_widget, SLOT(slotBuildInputRadianceFileThread()));
  proc_menu->addAction(act);
  
  act = new QAction(tr("Build Interface Function File"), this);
  //act->setShortcut(tr("Ctrl+I"));
  act->setStatusTip(tr("Process the interface function parameters and build as a file"));
  connect(act, SIGNAL(triggered()), model_widget, SLOT(slotBuildInterfaceFileThread()));
  proc_menu->addAction(act);

  act = new QAction(tr("Build Phase Function File"), this);
  //act->setShortcut(tr("Ctrl+P"));
  act->setStatusTip(tr("Process the phase function parameters and build as a file"));
  connect(act, SIGNAL(triggered()), model_widget, SLOT(slotBuildPhaseFunctionFileThread()));
  proc_menu->addAction(act);
  
  proc_menu->addSeparator();

  act = new QAction(tr("&Additional Options"), this);
  act->setStatusTip(tr("Change additional options"));
  connect(act, SIGNAL(triggered()), this, SLOT(slotAdditionalOptionsDialog()));
  proc_menu->addAction(act);

  // processing menu

  act = new QAction(tr("&Options"), this);
  act->setStatusTip(tr("Change visualisation options"));
  connect(act, SIGNAL(triggered()), this, SLOT(slotVisualisationOptionsDialog()));
  vis_menu->addAction(act);

  // help menu

  act = new QAction(tr("&About"), this);
  act->setStatusTip(tr("Display information about the program"));
  connect(act, SIGNAL(triggered()), this, SLOT(slotAboutDialog()));
  help_menu->addAction(act);
}



// PlanarRad::updateCurrentDir() [private]

void PlanarRad::updateCurrentDir(const JString& fp) { 
  if (!fp.contains('/')) return;
  JString js = fp.section('/', 0, fp.contains('/')-1);
  //jlog::ls << "Current dir: " << js << "\n";
  current_dir_qs = J2Q(js);
  QDir::setCurrent(current_dir_qs);
  Tool_Gui::initBandSpecList();
  BandSpec::addDefaultToAvailableList();
  Tool_Gui::initDirecSpecList();
  DirecSpec::addDefaultToAvailableList();
  data_widget->setCurrentDir(current_dir_qs);
  model_widget->setCurrentDir(current_dir_qs);
  model_widget->slotCurrentDirChanged();
  setCaption("PlanarRad ("+current_dir_qs+")");
}



// PlanarRad::slotLoadModelParameters()

void PlanarRad::slotLoadModelParameters() { 

  if (jlog::vb(5)) jlog::ls << "PlanarRad::slotLoadModelParameters()\n";

  QString s = QFileDialog::getOpenFileName(this, tr("Load Model Parameters"), current_dir_qs);
  JString fp(Q2J(s));

  if (fp.isEmpty()) return;

  updateCurrentDir(fp);

  JParser prm;
  prm.readFile(fp);
  model_widget->parserToGui("", prm);
  additional_options->parserToGui("", prm, AdditionalOptionsDialog::SAVE);
}



// PlanarRad::slotSaveModelParameters()

void PlanarRad::slotSaveModelParameters() { 

  if (jlog::vb(5)) jlog::ls << "PlanarRad::slotSaveModelParameters()\n";

  QString s = "params.txt";
  if (!current_dir_qs.isEmpty()) s = current_dir_qs+"/"+s;

  s = QFileDialog::getSaveFileName(this, tr("Save Model Parameters"), s);
  JString fp(Q2J(s));

  if (fp.isEmpty()) return;

  JParser prm;
  model_widget->guiToParser("", prm);
  additional_options->guiToParser("", prm, AdditionalOptionsDialog::SAVE);

  prm.writeFile(fp);

  updateCurrentDir(fp);
}



void PlanarRad::slotChangeCurrentDir() { 

  if (jlog::vb(5)) jlog::ls << "PlanarRad::slotLoadModelParameters()\n";

  QString s = QFileDialog::getExistingDirectory(this, tr("Select Working Directory"), current_dir_qs);
  JString fp(Q2J(s));

  if (fp.isEmpty()) return;

  updateCurrentDir(fp+"/dummy");
}



// here enable and disable stuff

void PlanarRad::slotIsProcessing(bool f) {
  proc_menu->setEnabled(!f);
}



// here some processing got successfully completed (not canceled or error)

void PlanarRad::slotProcessingFinishedOk() {

  if (jlog::vb(5)) jlog::ls << "PlanarRad::slotProcessingCompleted()\n";

  data_widget->autoLoad(model_widget->lastOutputFile());
}



// PlanarRad::slotDataSetsChanged()

void PlanarRad::slotDataSetsChanged() { 
  profile_widget->slotDataSetsChanged();
  profile_widget->slotUpdateMaxDepth();
  spec_widget->slotUpdateWavelengthRange();
  //panel_scroll->slotDataSetsChanged();
}



// PlanarRad::slotCurrentDepthChanged()

void PlanarRad::slotCurrentDepthChanged(double d) { 
  //panel_scroll->slotCurrentDepthChanged(d);
}



// PlanarRad::slotReportError()

void PlanarRad::slotReportError() { 
  reportError();
}



// PlanarRad::slotAdditionalOptionsDialog()

void PlanarRad::slotAdditionalOptionsDialog() { 
    // store the current state of the window
  JParser store_prm; 
  additional_options->guiToParser("", store_prm, AdditionalOptionsDialog::SAVE);
    // if accpeted then quit keeping new states 
  if (additional_options->exec() == QDialog::Accepted) return;
    // othewise write back the original state
  additional_options->parserToGui("", store_prm, AdditionalOptionsDialog::SAVE);
}



// PlanarRad::slotVisualisationOptionsDialog()

void PlanarRad::slotVisualisationOptionsDialog() { 
    // store the current state of the window
  JParser store_prm; 
  vis_options->guiToParser("", store_prm, VisualisationOptionsDialog::SAVE);
    // if not accepted then write back the original state
  if (vis_options->exec() != QDialog::Accepted) {
    vis_options->parserToGui("", store_prm, VisualisationOptionsDialog::SAVE);
    return;
  }
    // might have changed max depth
  profile_widget->slotUpdateMaxDepth();
}



// PlanarRad::slotAboutDialog()

void PlanarRad::slotAboutDialog() { 
    // create the dialog
  AboutDialog ad(this);
    // if canceled do nothing 
  ad.exec();
}



// PlanarRad::slotQuit()

void PlanarRad::slotQuit() { 
  QCoreApplication::quit();
}

