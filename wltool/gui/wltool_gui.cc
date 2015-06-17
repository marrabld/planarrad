
// wltool_gui.h

#include "wltool_gui.h"
#include "base/jparser.h"
#include "rad/bandspec.h"
#include "packettab.h"
#include "base/jlog.h"
#include "aboutdialog.h"
#include "sensorconfigdialog.h"
#include "wl/record_merge.h"
#include "visualisationconfigdialog.h"
#include "wlpanel.h"
#include "wl/sensor_hocr.h"
#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qprogressbar.h>
#include <qfiledialog.h>
#include <qgroupbox.h>

#include <QLayout>
#include <QTabWidget>
#include <QTableView>
#include <QLabel>
#include <QMenuBar>
#include <QItemSelection>
#include <QCoreApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>



WLTool_Gui::WLTool_Gui(JParser& prm, QWidget* parent) :
  Tool_Gui(prm, parent),
  tool_thread(wltool, WLTool_Gui::inner_prm),
  is_processing(false) {

  setCaption("WLTool");

    // menu
  createMenus();

   // the central widget holds the full layout
  QWidget* w = new QWidget();
  setCentralWidget(w);
  QHBoxLayout* vl = new QHBoxLayout(w);

    // split the top section horizontally for the chart panel on the right
  h_split = new QSplitter(w);
  h_split->setOrientation(Qt::Horizontal);
  vl->addWidget(h_split);

    // split the space vertically for the raw data at the bottom
  v_split = new QSplitter(w);
  v_split->setOrientation(Qt::Vertical);

  QWidget* w_left = new QWidget(w);
  QVBoxLayout* vl_left = new QVBoxLayout(w_left);
  vl_left->setContentsMargins(0,0,0,0);
  //h_split->addWidget(v_split);
  h_split->addWidget(w_left);

  vl_left->addWidget(v_split);

  top = new QWidget;
  QVBoxLayout* vl_top = new QVBoxLayout(top);

  tabs = new QTabWidget(top);
  vl_top->addWidget(tabs);

    // packet tab
  packet_tab = new PacketTab(tabs);
  tabs->addTab(packet_tab, "Packets");
  packet_tab->tableView()->setModel(&packet_list_model);
    // record tab
  record_tab = new RecordTab(tabs);
  tabs->addTab(record_tab, "Records");
  record_tab->tableView()->setModel(&record_list_model);
    // record tab
  merge_tab = new MergeTab(tabs);
  tabs->addTab(merge_tab, "Merge");
  merge_tab->tableView()->setModel(&merge_list_model);

    // add the top left area
  v_split->addWidget(top);
  v_split->setCollapsible(0,false);

    // panel scroll - initially empty 
  QWidget* right = new QWidget;
  QVBoxLayout* vl3 = new QVBoxLayout(right);
  h_split->addWidget(right);
  panel_scroll = new WLPanelScroll(right);
  vl3->addWidget(panel_scroll);

  bottom = new QWidget;
  QVBoxLayout* vl2 = new QVBoxLayout(bottom);
    // raw data area
  QLabel* label = new QLabel("Record Data:", bottom);
  vl2->addWidget(label);
  record_text = new RecordText(bottom);
  vl2->addWidget(record_text);
 
  QWidget* bw = new QWidget;
  vl_left->addWidget(bw);

  QHBoxLayout* hl = new QHBoxLayout(bw);
 
  label = new QLabel("Process: ");
  hl->addWidget(label);

  progress_bar = new QProgressBar(this);
  progress_bar->setMinimum(0);
  progress_bar->setMaximum(100);
  hl->addWidget(progress_bar);
  cancel_pb = new QPushButton(" Cancel ", this);
  connect(cancel_pb, SIGNAL(clicked()), this, SLOT(slotCancelProcessing()));
  hl->addWidget(cancel_pb);

    // add the raw data to the bottom of the horizontal split
  v_split->addWidget(bottom);
 
  h_split->setCollapsible(0,false);
  v_split->setCollapsible(0,false);
 
 
  connect(&tool_thread, SIGNAL(processingFinished(bool)), this, SLOT(processingFinished(bool)));

    // change of selected record from record list
  connect(record_tab->tableView()->selectionModel(), 
	  SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), 
	  this,
	  SLOT(slotRecordTableSelectionChanged(const QItemSelection&, const QItemSelection&))); 

    // change of selected record from merge list
  connect(merge_tab->tableView()->selectionModel(), 
	  SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), 
	  this,
	  SLOT(slotMergeTableSelectionChanged(const QItemSelection&, const QItemSelection&))); 

  connect(this, SIGNAL(signalNewRecordSelected(const Record*)), record_text, SLOT(slotNewRecordSelected(const Record*)));

  connect(this, SIGNAL(signalNewRecordList(const RecordList*)), panel_scroll, SLOT(slotNewRecordList(const RecordList*)));
  connect(this, SIGNAL(signalNewRecordSelected(const Record*)), panel_scroll, SLOT(slotNewRecordSelected(const Record*)));

  connect(&progress_timer, SIGNAL(timeout()), this, SLOT(slotUpdateProgress()));

  connect(merge_menu, SIGNAL(signalDoMerge(int)), this, SLOT(slotDoMerge(int)));
  
  connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)));
    
  //parserToGui();

  setMinimumSize(400,400);
}



void WLTool_Gui::createMenus() {

  QAction* act;

  QMenu* file_menu = menuBar()->addMenu("File");
  merge_menu = new MergeMenu(wltool);
  menuBar()->addMenu(merge_menu);
  QMenu* config_menu = menuBar()->addMenu("Config");
  QMenu* help_menu = menuBar()->addMenu("Help");
 
  act = new QAction(tr("Load &Archive File"), this);
  act->setShortcut(tr("Ctrl+A"));
  act->setStatusTip(tr("Load an archive file"));
  connect(act, SIGNAL(triggered()), this, SLOT(slotLoadArchiveFile()));
  file_menu->addAction(act);

  act_close_af = new QAction(tr("Close &Archive File"), this);
  act_close_af->setShortcut(tr("Ctrl+X"));
  act_close_af->setStatusTip(tr("Close the currently loaded archive file"));
  connect(act_close_af, SIGNAL(triggered()), this, SLOT(slotCloseArchiveFile()));
  file_menu->addAction(act_close_af);

  file_menu->addSeparator();

  menu_save_cf = file_menu->addMenu(tr("Save &Channel File"));

  file_menu->addSeparator();

  menu_load_ps = file_menu->addMenu(tr("Load &Processed File"));

  file_menu->addSeparator();

  act_save_merged = new QAction(tr("Save Merged File"), this);
  act_save_merged->setShortcut(tr("Ctrl+M"));
  act_save_merged->setStatusTip(tr("Save merged file"));
  connect(act_save_merged, SIGNAL(triggered()), this, SLOT(slotSaveMergedFile()));
  file_menu->addAction(act_save_merged);

  file_menu->addSeparator();

  act = new QAction(tr("&Quit"), this);
  act->setStatusTip(tr("Close the application"));
  connect(act, SIGNAL(triggered()), this, SLOT(slotQuit()));
  file_menu->addAction(act);

  act = new QAction(tr("&Sensor Config"), this);
  act->setShortcut(tr("Ctrl+S"));
  act->setStatusTip(tr("Set the sensor configuration"));
  connect(act, SIGNAL(triggered()), this, SLOT(slotSensorConfigDialog()));
  config_menu->addAction(act);

  act = new QAction(tr("&Visualisation Config"), this);
  act->setShortcut(tr("Ctrl+I"));
  act->setStatusTip(tr("Set the visualisation configuration"));
  connect(act, SIGNAL(triggered()), this, SLOT(slotVisualisationConfigDialog()));
  config_menu->addAction(act);

  act_show_keys = new QAction(tr("&Show Chart Keys"), this);
  act_show_keys->setShortcut(tr("Ctrl+K"));
  act_show_keys->setStatusTip(tr("Toggle chart key display on and off"));
  act_show_keys->setCheckable(true);
  connect(act_show_keys, SIGNAL(triggered(bool)), this, SLOT(slotChartKeysToggled(bool)));
  config_menu->addAction(act_show_keys);

  act = new QAction(tr("&About"), this);
  act->setStatusTip(tr("Display information avout the program"));
  connect(act, SIGNAL(triggered()), this, SLOT(slotAboutDialog()));
  help_menu->addAction(act);

}



void WLTool_Gui::rebuildMenus() {
  rebuildChannelSaveMenu();
  rebuildLoadProcessedMenu();
  merge_menu->rebuild();
}



void WLTool_Gui::rebuildChannelSaveMenu() {

  menu_save_cf->clear();

  if (!archiveFileLoaded()) { menu_save_cf->setEnabled(false); return; }

  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    if (archiveFile()->channel(i).recordCount() > 0) {

      JString s("Save Channel "+JString::number(i+1)+" File (");
      s += wltool.sensorConfig().sensor(i).sensorType() + ")";

      QAction* act = new QAction(tr(J2Q(s)), this);
      act->setStatusTip(tr("Save the channel records file"));
      act->setData(i);
      connect(act, SIGNAL(triggered()), this, SLOT(slotSaveChannelFile()));
      menu_save_cf->addAction(act);
    }
  }

  menu_save_cf->setEnabled(true);  
}



void WLTool_Gui::rebuildLoadProcessedMenu() {
 
  menu_load_ps->clear();
 
  if (!archiveFileLoaded()) { menu_load_ps->setEnabled(false); return; }

  bool enab = false;

    // this is specific for the HOCR, will neeed to change of any other sensor data needs loading
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    if (archiveFile()->channel(i).recordCount() > 0 && wltool.sensorConfig().sensor(i).sensorType() == "HOCR") {

      Sensor_HOCR* sens_hocr = dynamic_cast<Sensor_HOCR*>(&wltool.sensorConfig().sensor(i));
      if (!sens_hocr) continue;

      JString s("Load Channel "+JString::number(i+1)+" Processed " + wltool.sensorConfig().sensor(i).sensorType() + " Ed and Lu Data");
      QAction* act = new QAction(tr(J2Q(s)), this);
      act->setStatusTip(tr("Load processed Ed and Lu HOCR data"));
      act->setData(i);
      connect(act, SIGNAL(triggered()), this, SLOT(slotLoadProcessedEdAndLuFile()));
      menu_load_ps->addAction(act);

      s = JString("Load Channel "+JString::number(i+1)+" Processed " + wltool.sensorConfig().sensor(i).sensorType() + " Es Data");
      act = new QAction(tr(J2Q(s)), this);
      act->setStatusTip(tr("Load processed Es HOCR data"));
      act->setData(i);
      connect(act, SIGNAL(triggered()), this, SLOT(slotLoadProcessedEsFile()));
      menu_load_ps->addAction(act);
    
      if (!sens_hocr->hasProcessedEdAndLu()) act->setEnabled(false);

        // can enable this whole menu now
      enab = true;
    }
  }

  menu_load_ps->setEnabled(enab);  
}



void WLTool_Gui::updateStates() {
  menuBar()->setEnabled(!is_processing);
  top->setEnabled(!is_processing);
  bottom->setEnabled(!is_processing && tabs->currentIndex()==1);
  cancel_pb->setEnabled(is_processing);
  act_close_af->setEnabled(archiveFileLoaded());
  act_save_merged->setEnabled(wltool.mergeDone());
}



// WLTool_Gui::parserToGui()

void WLTool_Gui::parserToGui() {
 
  //JString s;
 
    // target band spec
  //archive_fp = outer_prm.getStringOrEmpty("archive_fp");
  //chan_device_fp[0] = outer_prm.getStringOrEmpty("chan1_device_fp");
}



// WLTool_Gui::guiToParser()

void WLTool_Gui::guiToParser() {

  // JString s;

  //inner_prm.clear();

    // gets locations of various directories - probably shuld not be in general Tool_GUI
  //Tool_Gui::guiToParser();

  //inner_prm.setLabelAndValue("archive_fp", archive_fp);
  //inner_prm.setLabelAndValue("operation", "extract");
  //if (!chan_device_fp[0].isEmpty()) inner_prm.setLabelAndValue("chan1_device_fp", chan_device_fp[0]);

}



// WLTool_Gui::init()

void WLTool_Gui::init() {
 
  inner_prm.copyLabelAndValue(outer_prm, "verbose");
   
  if (!wltool.buildSensorConfig(inner_prm)) reportError();

  panel_scroll->rebuild(wltool.sensorConfig(), outer_prm);

  act_show_keys->setChecked(!outer_prm.isDefinedAndTrueBool("vis_hide_keys"));
  panel_scroll->slotSetShowKey(act_show_keys->isChecked());
 
  rebuildMenus();
  updateStates();
}



// WLTool_Gui::slotRecordTableSelectionChanged()

void WLTool_Gui::slotRecordTableSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

  if (jlog::vb(5)) jlog::ls << "WLTool_Gui::slotRecordTableSelectionChanged()\n";

  ArchiveFile* af = wltool.archiveFile();
 
  Record* rec(0);
  if (af && !selected.indexes().isEmpty()) rec = af->record(selected.indexes().first().row());

  emit signalNewRecordSelected(rec);
}



// WLTool_Gui::slotMergeTableSelectionChanged()

void WLTool_Gui::slotMergeTableSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

  if (jlog::vb(5)) jlog::ls << "WLTool_Gui::slotMergeTableSelectionChanged()\n";

  ArchiveFile* af = wltool.archiveFile();
  if (!af || selected.indexes().isEmpty()) { emit signalNewRecordSelected(0); return; }

  if (selected.indexes().first().row() >= af->channelMerge().recordCount()) { emit signalNewRecordSelected(0); return; }

  const Record_MERGE* rec = dynamic_cast<const Record_MERGE*>(af->channelMerge().record(selected.indexes().first().row()));
  
  if (!rec) { emit signalNewRecordSelected(0); return; }

  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {   
    if (rec->record(i)) {
      emit signalNewRecordSelected(rec->record(i));
    }
  }
}



// WLTool_Gui::slotLoadArchiveFile()

void WLTool_Gui::slotLoadArchiveFile() {
 
  if (jlog::vb(5)) jlog::ls << "slotLoadArchiveFile()\n";

  QString s = QFileDialog::getOpenFileName(this, tr("Load Archive File"), current_dir_qs);
  //QString s = QFileDialog::getOpenFileName(QString::null, QString::null, 0, 0, "Load Archive File");
  JString fp(Q2J(s));

  if (fp.isEmpty()) return;

  slotFilePathOpened(fp);

  startProcessingExtract(fp);
}



void WLTool_Gui::startProcessingExtract(const JString& fp) {

    // clear the old lists
  packet_list_model.setArchiveFile(0);
  record_list_model.setArchiveFile(0);
  merge_list_model.setArchiveFile(0);
  emit signalNewRecordList(0);
  emit signalNewRecordSelected(0);

  is_processing = true;
  updateStates();

  int sep_count = fp.contains('/');
  JString fn = (sep_count==0) ? fp : fp.section('/',sep_count,sep_count);
  setCaption(J2Q("WLTool: "+fn));

  inner_prm.clear();
  inner_prm.setLabelAndValue("preloaded_sensor_setup", "yes");
  inner_prm.setLabelAndValue("operation", "extract");
  inner_prm.setLabelAndValue("archive_fp", fp);

    // make sure this is zero before starting the timer
  wltool.clearPctComplete();
    // a progress update every 100 ms
  processing_canceled = false;
  progress_timer.start(100);
    // start the tool processing
  tool_thread.start();

  //loadArchiveFile(fp);

  //updateStates();
}



void WLTool_Gui::finishedProcessingExtract(bool ret_ok) {

  if (!ret_ok) {
    wltool.deleteArchiveFile();
    if (!processing_canceled) reportError();
    rebuildMenus();
    return;
  }

    // update the packet list
  packet_list_model.setArchiveFile(wltool.archiveFile());
  packet_tab->tableView()->resizeColumnsToContents();
    // update the record list
  record_list_model.setArchiveFile(wltool.archiveFile());
  record_tab->tableView()->resizeColumnsToContents();
    // update the merge list
  merge_list_model.setArchiveFile(wltool.archiveFile());
  //merge_tab->tableView()->resizeColumnsToContents();

  rebuildMenus();
    // record list
  tabs->setCurrentIndex(1);

  emit signalNewRecordList(wltool.archiveFile() ? &wltool.archiveFile()->recordList() : 0);
  emit signalNewRecordSelected(0);
}



// WLTool_Gui::processingFinished() [slot]

void WLTool_Gui::processingFinished(bool ret_ok) {

  if (jlog::vbLevel(5)) jlog::ls << "processingFinished() " << ret_ok << ".\n";

  progress_timer.stop();
  progress_bar->setValue(ret_ok ? 100 : 0);

  is_processing = false;
  updateStates();

  if (inner_prm.valueCheck("operation","extract")) finishedProcessingExtract(ret_ok);

  else if (inner_prm.valueCheck("operation","load_processed")) {
    if (inner_prm.valueCheck("dummy_flag","ed_and_lu")) finishedLoadProcessedEdAndLu(ret_ok);
    else finishedLoadProcessedEs(ret_ok);

  } else if (inner_prm.valueCheck("operation","merge")) finishedDoMerge(ret_ok);
}



// WLTool_Gui::slotCancelProcessing()

void WLTool_Gui::slotCancelProcessing() {

  if (jlog::vb(5)) jlog::ls << "slotCancelProcessing()\n";

  processing_canceled = true;
  wltool.stopProcessing();
}



// WLTool_Gui::updateProgress() [slot]

void WLTool_Gui::slotUpdateProgress() {

  progress_bar->setValue((int) wltool.pctComplete());
}



// WLTool_Gui::slotCloseArchiveFile()

void WLTool_Gui::slotCloseArchiveFile() {
 
  if (jlog::vb(5)) jlog::ls << "slotCloseArchiveFile()\n";

  setCaption("WLTool");

  packet_list_model.setArchiveFile(0);
  record_list_model.setArchiveFile(0);
  merge_list_model.setArchiveFile(0);

  wltool.deleteArchiveFile();

  emit signalNewRecordSelected(0);
  emit signalNewRecordList(0);

  progress_bar->setValue(0);

  rebuildMenus();
  updateStates();
}



// WLTool_Gui::slotDoMerge()

void WLTool_Gui::slotDoMerge(int channel_index) {

  if (jlog::vb(5)) jlog::ls << "slotDoMerge()\n";

  jlog::ls << "merge on channel " << channel_index+1 << "\n";

  inner_prm.clear();
  inner_prm.setLabelAndValue("operation","merge");
  inner_prm.setLabelAndValue("merge_reference_channel",JString::number(channel_index+1));
  merge_menu->guiToParser(inner_prm);

  inner_prm.displayList(jlog::ls);

    // make sure this is zero before starting the timer
  wltool.clearPctComplete();
    // a progress update every 100 ms
  processing_canceled = false;
  progress_timer.start(100);
    // start the tool processing
  tool_thread.start();
}



void WLTool_Gui::finishedDoMerge(bool ret_ok) {

  if (!ret_ok) {
    if (!processing_canceled) reportError();
    return;
  }

    // update the packet list
  merge_list_model.setArchiveFile(wltool.archiveFile());
  //merge_tab->tableView()->resizeColumnsToContents();

  //rebuildMenus();
    // record list
  tabs->setCurrentIndex(2);

  //emit signalNewRecordList(wltool.archiveFile() ? &wltool.archiveFile()->recordList() : 0);
  //emit signalNewRecordSelected(0);
}




void WLTool_Gui::slotCurrentTabChanged(int index) {
  bottom->setEnabled(index == 1);
}




// WLTool_Gui::slotChartKeysToggled()

void WLTool_Gui::slotChartKeysToggled(bool f) {

  if (jlog::vb(5)) jlog::ls << "slotChartKeysToggled() " << f << "\n";

  if (!f) outer_prm.setLabelAndValue("vis_hide_keys", "yes");
  else outer_prm.removeLabel("vis_hide_keys");

  panel_scroll->slotSetShowKey(f);
}



// WLTool_Gui::slotSensorConfigDialog()

void WLTool_Gui::slotSensorConfigDialog() {
 
  if (jlog::vb(5)) jlog::ls << "slotSensorConfigDialog()\n";

  JParser prm;

    // create the dialog
  SensorConfigDialog scd(current_dir_qs, this);
    // set up the gui elements from the current inner_prm parameters
  wltool.sensorConfig().writeToParser(prm);
  scd.parserToGui(prm);
  connect(&scd, SIGNAL(signalFilePathOpened(const JString&)), this, SLOT(slotFilePathOpened(const JString&)));
    // if canceled do nothing 
  if (scd.exec() == QDialog::Rejected) return;
    // log info
  if (jlog::vb(5)) jlog::ls << "Change sensor config.\n";
    // set the inner_prm parameters from the final state of the gui elements
  scd.guiToParser(prm);
    // log info
  if (jlog::vb(5)) prm.displayList(jlog::ls);
    // make a new config
  SensorConfig* new_config = new SensorConfig(prm);
    // if went wrong then quit with error
  if (!new_config->statusOk()) { delete new_config; return reportError(); }

  bool reprocess = false;

    // if there is an archive file loaded either clear everything or reload it
  if (!archiveFilepath().isEmpty()) {
    QMessageBox mb(this);
    mb.setText("Reprocess loaded archive file or close it?");
    mb.setIcon(QMessageBox::Question);
    QPushButton* b = mb.addButton("Reprocess", QMessageBox::AcceptRole);
    mb.addButton("Close", QMessageBox::RejectRole);
    mb.exec();
    reprocess = (mb.clickedButton() == b); 
  }

  if (reprocess) {
    wltool.setSensorConfig(new_config);
    startProcessingExtract(archiveFilepath());
    return;
  }

  packet_list_model.setArchiveFile(0);
  record_list_model.setArchiveFile(0);
  merge_list_model.setArchiveFile(0);
  wltool.deleteArchiveFile();
  emit signalNewRecordSelected(0);
  emit signalNewRecordList(0);
  
  wltool.setSensorConfig(new_config);
  panel_scroll->deleteAllPanels();
  panel_scroll->rebuild(wltool.sensorConfig(), outer_prm);
  progress_bar->setValue(0);
  
  rebuildMenus();
  updateStates();
}



// changes the visualisation options

void WLTool_Gui::slotVisualisationConfigDialog() {
 
  if (jlog::vb(5)) jlog::ls << "slotVisualisationConfigDialog()\n";

    // create the dialog
  VisualisationConfigDialog vcd(this);
    // set up the gui elements from the current outer_prm parameters
  vcd.parserToGui(outer_prm);
    // if canceled do nothing 
  if (vcd.exec() == QDialog::Rejected) return;
    // set the outer_prm parameters from the final state of the gui elements
  vcd.guiToParser(outer_prm);
    // visualisation opitons only changed in outer_prm so panels can be added or removed
  panel_scroll->rebuild(wltool.sensorConfig(), outer_prm);

  if (wltool.archiveFile()) emit signalNewRecordList(&wltool.archiveFile()->recordList());
 
  updateStates();
}



void WLTool_Gui::slotAboutDialog() {
 
  if (jlog::vb(5)) jlog::ls << "slotAboutDialog()\n";

    // create the dialog
  AboutDialog ad(this);
    // if canceled do nothing 
  ad.exec();
}



void WLTool_Gui::slotLoadProcessedEdAndLuFile() {

  if (!archiveFile()) return;

  QAction* act(dynamic_cast<QAction*>(QObject::sender()));
  if (!act) return;

  int channel_index = act->data().toInt();

  if (jlog::vb(5)) jlog::ls << "slotLoadProcessedFileEdAndLu() " << channel_index << "\n";

  QString s = QFileDialog::getOpenFileName(this, "Load Processed Ed And Lu File For Channel " + QString::number(channel_index+1), current_dir_qs);
  JString fp(Q2J(s));

  if (fp.isEmpty()) return;

  slotFilePathOpened(fp);

    // clear these
  clearLoadedEdAndLu(channel_index);
  clearLoadedEs(channel_index);
 
  is_processing = true;
  updateStates();

  JString label(JString("chan")+JString::number(channel_index+1)+"_hocr_l2s_ed_and_lu");

  inner_prm.clear();
  inner_prm.setLabelAndValue("operation", "load_processed");
  inner_prm.setLabelAndValue("dummy_flag", "ed_and_lu"); // just so that afterwards we know which load was done
  inner_prm.setLabelAndValue("channel_index", JString::number(channel_index));
  inner_prm.setLabelAndValue(label+"_fp", fp);
  if (jlog::vb(5)) inner_prm.displayList(jlog::ls);
 
    // make sure this is zero before starting the timer
  progress_bar->setValue(0);
  wltool.clearPctComplete();
    // a progress update every 100 ms
  processing_canceled = false;
  progress_timer.start(100);
    // start the tool processing
  tool_thread.start();
}



// WLTool_Gui::slotLoadProcessedEsFile()

void WLTool_Gui::slotLoadProcessedEsFile() {

  if (!archiveFile()) return;

  QAction* act(dynamic_cast<QAction*>(QObject::sender()));
  if (!act) return;

  int channel_index = act->data().toInt();

  if (jlog::vb(5)) jlog::ls << "slotLoadProcessedFileEs() " << channel_index << "\n";

  QString s = QFileDialog::getOpenFileName(this, "Load Processed Es File For Channel " + QString::number(channel_index+1), current_dir_qs);
  JString fp(Q2J(s));

  if (fp.isEmpty()) return;

  slotFilePathOpened(fp);

    // clear any previous
  clearLoadedEs(channel_index);
 
  is_processing = true;
  updateStates();

  JString label(JString("chan")+JString::number(channel_index+1)+"_hocr_l2s_es");

  inner_prm.clear();
  inner_prm.setLabelAndValue("operation", "load_processed");
  inner_prm.setLabelAndValue("dummy_flag", "es"); // just so that afterwards we know which load was done
  inner_prm.setLabelAndValue("channel_index", JString::number(channel_index));
  inner_prm.setLabelAndValue(label+"_fp", fp);
  if (jlog::vb(5)) inner_prm.displayList(jlog::ls);
 
    // make sure this is zero before starting the timer
  progress_bar->setValue(0);
  wltool.clearPctComplete();
    // a progress update every 100 ms
  processing_canceled = false;
  progress_timer.start(100);
    // start the tool processing
  tool_thread.start();
}



void WLTool_Gui::finishedLoadProcessedEdAndLu(bool ret_ok) {

  if (!ret_ok) {
    if (!processing_canceled) reportError();
    clearLoadedEdAndLu(inner_prm.getInt("channel_index"));
    return;
  } 

 if (jlog::vb(5)) jlog::ls << "Processed Ed and Lu data loaded ok.\n";

    // can now load Es data - so enable in menu
  rebuildLoadProcessedMenu();

    // force update of the merge list
  merge_list_model.setArchiveFile(wltool.archiveFile());
 
    // force a redraw of the plots
  emit signalNewRecordList(wltool.archiveFile() ? &wltool.archiveFile()->recordList() : 0);
  //emit signalNewRecordSelected(0);
}



void WLTool_Gui::finishedLoadProcessedEs(bool ret_ok) {

 if (!ret_ok) {
    if (!processing_canceled) reportError();
    clearLoadedEs(inner_prm.getInt("channel_index"));
    return;
  } 

  if (jlog::vb(5)) jlog::ls << "Processed Es loaded ok.\n";

    // force update of the merge list
  merge_list_model.setArchiveFile(wltool.archiveFile());
 
    // force a redraw of the plots
  emit signalNewRecordList(wltool.archiveFile() ? &wltool.archiveFile()->recordList() : 0);
  //emit signalNewRecordSelected(0);
}



// WLTool_Gui::clearLoadedEdAndLu()

void WLTool_Gui::clearLoadedEdAndLu(int ci) {
 
  JListIterator<Record> iter(archiveFile()->channel(ci).recordList());
  Record* rec = iter.first();
  while (rec) {
    Record_HOCR* rec_hocr = dynamic_cast<Record_HOCR*>(rec);
    if (rec_hocr) rec_hocr->clearProcessedEdAndLu();
    rec = iter.next();
  }

  Sensor_HOCR* sens_hocr = dynamic_cast<Sensor_HOCR*>(&wltool.sensorConfig().sensor(ci));
  if (sens_hocr) sens_hocr->clearProcessedEdAndLu();
}



// WLTool_Gui::clearLoadedEs()

void WLTool_Gui::clearLoadedEs(int ci) {
 
  JListIterator<Record> iter(archiveFile()->channel(ci).recordList());
  Record* rec = iter.first();
  while (rec) {
    Record_HOCR* rec_hocr = dynamic_cast<Record_HOCR*>(rec);
    if (rec_hocr) rec_hocr->clearProcessedEs();
    rec = iter.next();
  }

  Sensor_HOCR* sens_hocr = dynamic_cast<Sensor_HOCR*>(&wltool.sensorConfig().sensor(ci));
  if (sens_hocr) sens_hocr->clearProcessedEs();
}



void WLTool_Gui::slotSaveChannelFile() {

  if (jlog::vb(5)) jlog::ls << "slotSaveChannelFile(()\n";

  QAction* act(dynamic_cast<QAction*>(QObject::sender()));
  if (!act) return;

  int channel_index = act->data().toInt();

  JString fp = wltool.channelFilepath("extracted", channel_index);
  if (!current_dir_qs.isEmpty()) fp = Q2J(current_dir_qs)+"/"+fp;

  QString s = QFileDialog::getSaveFileName(this, "Save Channel File", J2Q(fp));
  fp = Q2J(s);

  if (fp.isEmpty()) return;

  if (jlog::vb(5)) jlog::ls << "saving channel " << channel_index << " file " << fp << "\n";

  if (!wltool.saveChannelFile(fp, channel_index)) reportError();
}



void WLTool_Gui::slotSaveMergedFile() {
  if (jlog::vb(5)) jlog::ls << "slotSaveMergedFile()\n";

  QString s = "merged.txt";
  if (!current_dir_qs.isEmpty()) s = current_dir_qs+"/"+s;

  s = QFileDialog::getSaveFileName(this, "Save Channel File", s);
  JString fp = Q2J(s);

  if (fp.isEmpty()) return;

  if (jlog::vb(5)) jlog::ls << "saving merged file " << fp << "\n";

  if (!wltool.saveMergedFile(fp)) reportError();
}



void WLTool_Gui::slotFilePathOpened(const JString& fp) {

  if (!fp.contains('/')) return;

  JString js = fp.section('/', 0, fp.contains('/')-1);
  jlog::ls << "Current dir: " << js << "\n";
  current_dir_qs = J2Q(js);
}



void WLTool_Gui::slotQuit() {
  QCoreApplication::quit();
}



// WLTool_Gui::saveSettings()
// save all persistent settings in the outer_prm and inner_prm

void WLTool_Gui::saveSettings(QSettings& settings) const {

  Tool_Gui::saveSettings(settings);
  settings.setValue("main_window/h_split", h_split->saveState());
  settings.setValue("main_window/v_split", v_split->saveState());

  settings.setValue("gui/current_dir",  current_dir_qs);

  JParser prm;
  wltool.sensorConfig().writeToParser(prm);

  parserToSettings(prm, "sensor_setup", settings);

  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    JString chanX = JString("chan" + JString::number(i+1));
    parserToSettings(prm, chanX+"_sensor", settings);
    parserToSettings(prm, chanX+"_device_fp", settings);
  }

  parserToSettings(outer_prm, "vis_acs_spectra", settings);
  parserToSettings(outer_prm, "vis_acs_a_time", settings);
  parserToSettings(outer_prm, "vis_acs_c_time", settings);
  parserToSettings(outer_prm, "vis_ctd_salinity", settings);
  parserToSettings(outer_prm, "vis_ctd_pressure", settings);
  parserToSettings(outer_prm, "vis_ctd_temperature", settings);
  parserToSettings(outer_prm, "vis_ecobb3_time", settings);
  parserToSettings(outer_prm, "vis_hocr_ed_raw_spectra", settings);
  parserToSettings(outer_prm, "vis_hocr_ed_raw_time", settings);
  parserToSettings(outer_prm, "vis_hocr_ed_dark_time", settings);
  parserToSettings(outer_prm, "vis_hocr_lu_raw_spectra", settings);
  parserToSettings(outer_prm, "vis_hocr_lu_raw_time", settings);
  parserToSettings(outer_prm, "vis_hocr_lu_dark_time", settings);
  parserToSettings(outer_prm, "vis_hocr_ed_spectra", settings);
  parserToSettings(outer_prm, "vis_hocr_ed_time", settings);
  parserToSettings(outer_prm, "vis_hocr_lu_spectra", settings);
  parserToSettings(outer_prm, "vis_hocr_lu_time", settings);
  parserToSettings(outer_prm, "vis_hocr_es_spectra", settings);
  parserToSettings(outer_prm, "vis_hocr_es_time", settings);
  parserToSettings(outer_prm, "vis_hide_keys", settings);
}



// WLTool_Gui::restoreSettings()
// restore all persistent settings to the outer_prm and inner_prm

void WLTool_Gui::restoreSettings(const QSettings& settings) {
  Tool_Gui::restoreSettings(settings);
  h_split->restoreState(settings.value("main_window/h_split").toByteArray());
  v_split->restoreState(settings.value("main_window/v_split").toByteArray());
 
  settingsToParser(inner_prm, "sensor_setup", settings);

  current_dir_qs = settings.value("gui/current_dir").toString();

  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    JString chanX = JString("chan" + JString::number(i+1));
    settingsToParser(inner_prm, chanX+"_sensor", settings);
    settingsToParser(inner_prm, chanX+"_device_fp", settings);
  }

    // use this a test for the settings being empty (i.e. first run)
  if (!settings.contains("main_window/geometry")) {

      // default visualisations
    outer_prm.setLabelAndValue("vis_acs_spectra", "yes");
    //outer_prm.setLabelAndValue("vis_ctd_pressure", "yes");
    outer_prm.setLabelAndValue("vis_ecobb3_time", "yes");

    resize(700,500);
    QList<int> sz_list;
    sz_list.append(450);
    sz_list.append(250);
    h_split->setSizes(sz_list);
    sz_list.clear();
    sz_list.append(350);
    sz_list.append(150);
    v_split->setSizes(sz_list);

  } else {
    settingsToParser(outer_prm, "vis_acs_spectra", settings);
    settingsToParser(outer_prm, "vis_acs_a_time", settings);
    settingsToParser(outer_prm, "vis_acs_c_time", settings);
    settingsToParser(outer_prm, "vis_ctd_salinity", settings);
    settingsToParser(outer_prm, "vis_ctd_pressure", settings);
    settingsToParser(outer_prm, "vis_ctd_temperature", settings);
    settingsToParser(outer_prm, "vis_ecobb3_time", settings);
    settingsToParser(outer_prm, "vis_hocr_ed_raw_spectra", settings);
    settingsToParser(outer_prm, "vis_hocr_ed_raw_time", settings);
    settingsToParser(outer_prm, "vis_hocr_ed_dark_time", settings);
    settingsToParser(outer_prm, "vis_hocr_lu_raw_spectra", settings);
    settingsToParser(outer_prm, "vis_hocr_lu_raw_time", settings);
    settingsToParser(outer_prm, "vis_hocr_lu_dark_time", settings);
    settingsToParser(outer_prm, "vis_hocr_ed_spectra", settings);
    settingsToParser(outer_prm, "vis_hocr_ed_time", settings);
    settingsToParser(outer_prm, "vis_hocr_lu_spectra", settings);
    settingsToParser(outer_prm, "vis_hocr_lu_time", settings);
    settingsToParser(outer_prm, "vis_hocr_es_spectra", settings);
    settingsToParser(outer_prm, "vis_hocr_es_time", settings);
    settingsToParser(outer_prm, "vis_hide_keys", settings);
  }
}

