
// wltool_gui.h

#ifndef _JUDETOOLS_WLTOOL_GUI_H
#define _JUDETOOLS_WLTOOL_GUI_H

#include "fe/tool_gui.h"
#include "wltool.h"
#include "wlpanel.h"
#include "fe/toolthread.h"
#include "fe/bandspeccombobox.h"
#include "fe/direcspeccombobox.h"
#include "fe/panelscroll.h"
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qprogressbar.h>
#include <qtimer.h>
#include "fe/widgetstack.h"
#include "wlpanelscroll.h"
#include "packettab.h"
#include "packetlistmodel.h"
#include "recordtab.h"
#include "recordlistmodel.h"
#include "mergelistmodel.h"
#include "recordtext.h"
#include "mergetab.h"
#include "mergemenu.h"

#include <QItemSelection>
#include <QItemSelectionModel>
#include <QSplitter>
#include <QProgressBar>
#include <QPushButton>

using namespace jude::fe;



// WLTool_Gui

class WLTool_Gui : public Tool_Gui {

  Q_OBJECT

  WLTool wltool;

  PacketListModel packet_list_model;
  RecordListModel record_list_model;
  MergeListModel merge_list_model;
  
  ToolThread tool_thread;
 
  QWidget* top;
  QWidget* bottom;

  QTabWidget* tabs;

  QSplitter* h_split;
  QSplitter* v_split;

  PacketTab* packet_tab;
  RecordTab* record_tab;
  MergeTab* merge_tab;

  WLPanelScroll* panel_scroll;

  RecordText* record_text;

  MergeMenu* merge_menu;
  QMenu* menu_save_cf;
  QMenu* menu_load_ps;

  QAction* act_show_keys;
  QAction* act_close_af;
  QAction* act_save_merged;
 
  QProgressBar* progress_bar;
  QPushButton* cancel_pb;

  QString current_dir_qs;
 
  //JString chan_device_fp[HARDWARE_CHANNEL_COUNT];

  QTimer progress_timer;

  bool is_processing;
  bool processing_canceled;

  void createMenus();

  void rebuildMenus();
  void rebuildChannelSaveMenu();
  void rebuildLoadProcessedMenu();
  void rebuildMergeMenu();

  void updateStates();

  WLPanel* createPanel(const JString& code, int channel_index);
  void checkPanel(const JString& code, int channel_index, int& curr_panel_index);
  void rebuildPanels();
  void clearPanels();

  void parserToGui();
  void guiToParser();

  void startProcessingExtract(const JString& fp);
  void finishedProcessingExtract(bool ret_ok);

  void clearLoadedEdAndLu(int ci);
  void clearLoadedEs(int ci); 

  //void loadArchiveFile(const JString& fp);
 
public:

  WLTool_Gui(JParser& prm, QWidget* parent=0);

  void init();

  const char* toolID() const { return "wltool_gui"; }

  void saveSettings(QSettings& settings) const;
  void restoreSettings(const QSettings& settings);

  bool archiveFileLoaded() const { return (wltool.archiveFile() != 0); }
  bool mergeDone() const { return (wltool.archiveFile()) ? wltool.archiveFile()->mergeDone() : false; }

  ArchiveFile* archiveFile() { return wltool.archiveFile(); }
  const ArchiveFile* archiveFile() const { return wltool.archiveFile(); }

  const JString archiveFilepath() const { return (wltool.archiveFile()) ? (wltool.archiveFile()->filepath()) : ""; }

public slots:

  void processingFinished(bool ret_val);
  void finishedLoadProcessedEdAndLu(bool ret_ok);
  void finishedLoadProcessedEs(bool ret_ok);
  void finishedDoMerge(bool ret_ok);

  void slotRecordTableSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);  
  void slotMergeTableSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

  void slotLoadArchiveFile();
  void slotCloseArchiveFile();
  void slotDoMerge(int channel_index);
  void slotCurrentTabChanged(int index);
  void slotChartKeysToggled(bool f);
  void slotLoadProcessedEdAndLuFile();
  void slotLoadProcessedEsFile();
  void slotSaveChannelFile();
  void slotSaveMergedFile();
  void slotQuit(); 

  void slotCancelProcessing();
  void slotUpdateProgress();

  void slotSensorConfigDialog();
  void slotVisualisationConfigDialog();
  void slotAboutDialog();

  void slotFilePathOpened(const JString& fp);

signals:

  void signalNewRecordList(const RecordList* rec_list);
  void signalNewRecordSelected(const Record* rec);
};



#endif // _JUDETOOLS_WLTOOL_GUI_H

