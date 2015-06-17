
// planarrad.h

#ifndef _PLANARRAD_PLANARRAD_H
#define _PLANARRAD_PLANARRAD_H

#include "fe/tool_gui.h"
#include "../skytool/skytool.h"
#include "../slabtool/slabtool.h"
#include "fe/toolthread.h"
#include "modelwidget.h"
#include "datawidget.h"
#include "profilewidget.h"
#include "datalistmodel.h"
#include "additionaloptionsdialog.h"
#include "visualisationoptionsdialog.h"
#include "plotpanelscroll.h"
#include "specwidget.h"
#include "hemiwidget.h"
#include <QSplitter>

/*
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
#include <QProgressBar>
#include <QPushButton>
*/

using namespace jude::fe;



class PlanarRad : public Tool_Gui {

  Q_OBJECT

  DataListModel data_model;

  SkyTool skytool;
  SlabTool slabtool;

  ToolThread skytool_thread;
  ToolThread slabtool_thread;
 
  JParser skytool_prm;
  JParser slabtool_prm;

  bool is_processing;

  QSplitter* h_split;
  QSplitter* h_split2;

  QTabWidget* tabs;

  ModelWidget* model_widget;
  DataWidget* data_widget;
  ProfileWidget* profile_widget;
  PlotPanelScroll* panel_scroll;

  SpecWidget* spec_widget;

  HemiWidget* hemi_widget;

  QMenu* file_menu;
  QMenu* proc_menu;
  QMenu* vis_menu;
  QMenu* help_menu;

  AdditionalOptionsDialog* additional_options;
  VisualisationOptionsDialog* vis_options;

  QString current_dir_qs;

  void createMenus();
  void updateCurrentDir(const JString& fp);

public:

  PlanarRad(JParser& prm, QWidget* parent=0);

  void init();

  const char* toolID() const { return "planarrad"; }

  void saveSettings(QSettings& settings) const;
  void restoreSettings(const QSettings& settings);

public slots:

  void slotLoadModelParameters();
  void slotSaveModelParameters();
  void slotChangeCurrentDir();
  void slotIsProcessing(bool f);
  void slotProcessingFinishedOk();

  void slotDataSetsChanged();
  void slotCurrentDepthChanged(double d);

  void slotReportError();
  void slotAdditionalOptionsDialog();
  void slotVisualisationOptionsDialog();
  void slotAboutDialog();

  void slotQuit();
};



#endif // _PLANARRAD_PLANARRAD_GUI_H

