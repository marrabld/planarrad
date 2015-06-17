
// interfacewidget.h

#ifndef _PLANARRAD_INTERFACEWIDGET_H
#define _PLANARRAD_INTERFACEWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QStackedWidget>
#include "base/jstring.h"
#include "fe/specload.h"
#include "fe/toolthread.h"
#include "surftool/surftool.h"

using namespace jude::base;
using namespace jude::fe;



class InterfaceWidget : public QWidget {

  Q_OBJECT

  SurfTool surftool;

  JParser tool_prm;
  ToolThread tool_thread;

  QRadioButton* iface_none_rb;
  QRadioButton* iface_flat_rb;
  QRadioButton* iface_wb_rb;
  QRadioButton* iface_file_rb;

  QWidget* refrac_widget;
  QLineEdit* iface_flat_refrac0_le;
  QLineEdit* iface_flat_refrac1_le;
 
  QLabel* wb_label1;
  QLabel* wb_label2;
  QLabel* wb_label3;
 
  QLineEdit* wind_speed_le;
  QLineEdit* wind_direc_le;
  QCheckBox* azavg_cb;

  FileLoad* file_load_fl;

  JString iface_fp;

  QButtonGroup button_grp;

public:

  typedef enum { SAVE, SURFTOOL, SLABTOOL } prm_context;

  InterfaceWidget(QWidget* parent);

  void parserToGui(const JString& prefix, const JParser& prm, prm_context pc);
  void guiToParser(const JString& prefix, JParser& prm, prm_context pc) const;

  bool buildInterfaceThread(const DirecSpec& ds_spec, int sub_quad_count, const JString& output_dir, bool fp_option);

  bool isProcessing() const { return tool_thread.isRunning(); }

  double pctComplete() const { return surftool.pctComplete(); }
  void clearPctComplete() { surftool.clearPctComplete(); }

  void cancelProcessing() { surftool.cancelProcessing(); }

public slots:

  void slotProcessingFinished(bool ret_ok);
  void slotUpdateEnabled();

signals:

  void signalProcessingFinished(bool ret_ok);
};



#endif // _PLANARRAD_INTERFACEWIDGET_H

