
// skywidget.h

#ifndef _PLANARRAD_SKYWIDGET_H
#define _PLANARRAD_SKYWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QStackedWidget>
#include "fe/fileload.h"
#include "fe/specload.h"
#include "fe/toolthread.h"
#include "base/jparser.h"
#include "skytool/skytool.h"

using namespace jude::base;
using namespace jude::fe;



class SkyWidget : public QWidget {

  Q_OBJECT

  SkyTool skytool;

  JParser tool_prm;
  ToolThread tool_thread;
 
  QComboBox* sky_type_cm;
  QStackedWidget* input_stack;
 
  QLineEdit* spec_azimuth_le;
  QLineEdit* spec_zenith_le;
  SpecLoad* spec_total_sl;
  SpecLoad* spec_diffuse_sl;

  QLineEdit* grant_azimuth_le;
  QLineEdit* grant_zenith_le;
  SpecLoad* grant_total_sl;
  SpecLoad* grant_diffuse_sl;
 
  QRadioButton* grant_clear_rb;
  QRadioButton* grant_high_cloud_rb;
  QRadioButton* grant_low_cloud_rb;
  QRadioButton* grant_overcast_rb;

  QLineEdit* hlideal_azimuth_le;
  QLineEdit* hlideal_zenith_le;
  QLineEdit* hlideal_C_le;
  QLineEdit* hlideal_r_le;

  SpecLoad* file_load_sl;

  JString sky_fp;

  /*
  int diffuse_index;
  int thetazero_index;
  int specular_index;
  int hlideal_index;
  int grant_index;
  int libradtran_index;
  */

  JString tempDir() { return ""; }

  enum { DIFFUSE_CODE=0, THETAZERO_CODE, SPECULAR_CODE, HLIDEAL_CODE, GRANT_CODE, FILE_CODE };

  QWidget* buildAzimuthAndZenith(QWidget* parent, QLineEdit*& azimuth_le,  QLineEdit*& zenith_le);  
  QWidget* buildTotalAndDiffuse(QWidget* parent, SpecLoad*& total_sl,  SpecLoad*& diffuse_sl);
  QWidget* buildGrantCloudState(QWidget* parent);
  QWidget* buildHLIdealParams(QWidget* parent);

public:

  typedef enum { SAVE, SKYTOOL, SLABTOOL } prm_context;

  SkyWidget(bool enable_file, QWidget* parent);

  void parserToGui(const JString& prefix, const JParser& prm, prm_context pc);
  void guiToParser(const JString& prefix, JParser& prm, prm_context pc) const;

  bool buildSkyThread(const BandSpec& bs_spec, const DirecSpec& ds_spec, int sub_quad_count, const JString& output_dir, bool fp_option);

  bool isProcessing() const { return tool_thread.isRunning(); }

  double pctComplete() const { return skytool.pctComplete(); }
  void clearPctComplete() { skytool.clearPctComplete(); }

  void cancelProcessing() { skytool.cancelProcessing(); }

  const JString& builtFilepath() const { return sky_fp; }

  void rebuildSpecLoads();

public slots:

  void typeActivated(int code);
  void slotProcessingFinished(bool ret_ok);

signals:

  void signalProcessingFinished(bool ret_ok);
};



#endif // _PLANARRAD_SKYWIDGET_H

