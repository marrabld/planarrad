
// visualisationconfigdialog.h

#ifndef _WLTOOL_GUI_VISUALISATIONCONFIGDIALOG_H
#define _WLTOOL_GUI_VISUALISATIONCONFIGDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QCheckBox>
#include <QLabel>

#include "base/jparser.h"

using namespace jude::base;




class VisualisationConfigDialog : public QDialog {

  Q_OBJECT

  QCheckBox* acs_spectra_cb;
  QCheckBox* acs_a_time_cb;
  QCheckBox* acs_c_time_cb;

  QCheckBox* ctd_salin_cb; 
  QCheckBox* ctd_pressure_cb; 
  QCheckBox* ctd_temp_cb; 

  QCheckBox* ecobb3_time_cb;

  QCheckBox* hocr_ed_raw_spectra_cb;
  QCheckBox* hocr_ed_raw_time_cb;
  QCheckBox* hocr_ed_dark_time_cb;
  QCheckBox* hocr_lu_raw_spectra_cb;
  QCheckBox* hocr_lu_raw_time_cb;
  QCheckBox* hocr_lu_dark_time_cb;
  QCheckBox* hocr_ed_spectra_cb;
  QCheckBox* hocr_ed_time_cb;
  QCheckBox* hocr_lu_spectra_cb;
  QCheckBox* hocr_lu_time_cb;
  QCheckBox* hocr_es_spectra_cb;
  QCheckBox* hocr_es_time_cb;

public:

  VisualisationConfigDialog(QWidget* p=0);

  void parserToGui(const JParser& prm);
  void guiToParser(JParser& prm) const;
 
  //public slots:
    //  void radioButtonClicked(QAbstractButton* b);
    //void acsDeviceLocatePressed();
};



#endif // _WLTOOL_GUI_VISUALISATIONCONFIGDIALOG_H

