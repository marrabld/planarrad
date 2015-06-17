
// planarrad.h

#ifndef _PLANARRAD_MODELWIDGET_H
#define _PLANARRAD_MODELWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QSettings>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QToolBox>
#include "skywidget.h"
#include "iopwidget.h"
#include "interfacewidget.h"
#include "depthwidget.h"
#include "boundarywidget.h"
#include "fe/fileload.h"
#include "fe/specload.h"
#include "fe/bandspeccombobox.h"
#include "fe/direcspeccombobox.h"
#include "additionaloptionsdialog.h"
#include "../../slabtool/slabtool.h"

using namespace jude::fe;



class ModelWidget : public QWidget {

  Q_OBJECT

  const AdditionalOptionsDialog& aod;
 
  SlabTool slabtool;

  JParser tool_prm;
  ToolThread tool_thread;
 
  QLineEdit* name_le;

  BandSpecComboBox* target_bandspec_cm;
  DirecSpecComboBox* target_direcspec_cm;

  QToolBox* tb;

  SkyWidget* sky_widget;
  InterfaceWidget* interface_widget;
  IOPWidget* iop_widget;
  DepthWidget* depth_widget;
  BoundaryWidget* boundary_widget;
 
  QPushButton* process_pb;
  QLabel* activity_label;
  QProgressBar* progress_bar;
  QPushButton* cancel_pb;

  QTimer progress_timer;

  bool processing_canceled;

  bool slow_bit_message;

  QString current_dir_qs;

  enum { DONE, IFACE, PHASEFUNC, SLAB } processing_chain;

  const JString outputDir() const;
  const JString outputDirExistsOrEmpty() const;
  const JString outputDirMake() const;

  void setIsProcessing(bool f);

  void addImageSave(JParser& prm, const JString& outdir, const JString& prefix, int im_size, double sens_k);

public:

  ModelWidget(const AdditionalOptionsDialog& aod, QWidget* parent);

  void saveSettings(const JString& prefix, QSettings& settings) const;
  void restoreSettings(const JString& prefix, const QSettings& settings);

  void parserToGui(const JString& prefix, const JParser& prm);
  void guiToParser(const JString& prefix, JParser& prm) const;

  const JString lastOutputFile() const;

  void setCurrentDir(const QString& qs) { current_dir_qs = qs; }

public slots:

  void slotBuildInputRadianceFileThread();
  void slotBuildInterfaceFileThread();
  void slotBuildPhaseFunctionFileThread();
  void slotRunSlabToolThread();

  void slotStartProcessing();
  void slotCancelProcessing();
  void slotProcessingFinished(bool ret_ok);
  void slotUpdateProgress();
  void slotProcessingActivity(const JString& s);
  void slotCurrentDirChanged();

signals:

  void signalReportError();
  void signalIsProcessing(bool f) const;
  void signalProcessingFinishedOk();
};



#endif // _PLANARRAD_MODELWIDGET_H

