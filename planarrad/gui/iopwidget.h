
// iopwidget.h

#ifndef _PLANARRAD_IOPWIDGET_H
#define _PLANARRAD_IOPWIDGET_H

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
#include "phasetool/phasetool.h"

using namespace jude::base;
using namespace jude::fe;



class IOPWidget : public QWidget {

  Q_OBJECT

  PhaseTool phasetool;

  JParser tool_prm;
  ToolThread tool_thread;
 
  QButtonGroup atten_button_grp;
  QRadioButton* atten_uni_rb;
  QLineEdit* atten_uni_le;
  QRadioButton* atten_file_rb;
  QWidget* atten_uni_widget;

  QButtonGroup absorp_button_grp;
  QRadioButton* absorp_uni_rb;
  QLineEdit* absorp_uni_le; 
  QRadioButton* absorp_file_rb;
  QWidget* absorp_uni_widget;

  QButtonGroup pf_button_grp;
  QRadioButton* pf_isotropic_rb;
  QRadioButton* pf_petzold_rb;
  QRadioButton* pf_ff_rb;
  QRadioButton* pf_file_rb;

  QWidget* bb_frac_widget;
  QLineEdit* bb_frac_le;

  SpecLoad* atten_sl;
  SpecLoad* absorp_sl;
  FileLoad* phase_func_fl;

  JString pf_fp;

  //JString tempDir() { return ""; }

public:

  typedef enum { SAVE, PHASETOOL, SLABTOOL } prm_context;

  IOPWidget(QWidget* parent);

  void parserToGui(const JString& prefix, const JParser& prm, prm_context pc);
  void guiToParser(const JString& prefix, JParser& prm, prm_context pc) const;

  bool buildPhaseFuncThread(const DirecSpec& ds_spec, int sub_quad_count, const JString& output_dir, bool fp_option);

  bool isProcessing() const { return tool_thread.isRunning(); }

  double pctComplete() const { return phasetool.pctComplete(); }
  void clearPctComplete() { phasetool.clearPctComplete(); }

  void cancelProcessing() { phasetool.cancelProcessing(); }

  void rebuildSpecLoads();

public slots:

  void slotProcessingFinished(bool ret_ok);
  void slotUpdateEnabled();
 
signals:

  void signalProcessingFinished(bool ret_ok);
};



#endif // _PLANARRAD_IOPWIDGET_H

