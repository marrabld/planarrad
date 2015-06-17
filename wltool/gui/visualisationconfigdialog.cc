
// visualisationconfigdialog.cc

#include "visualisationconfigdialog.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QButtonGroup>
#include <QPushButton>
#include <QFileDialog>




VisualisationConfigDialog::VisualisationConfigDialog(QWidget* p) : 
  QDialog(p) {

  setCaption("Visualisation Configuration");

  QVBoxLayout* vl_top = new QVBoxLayout(this);

  QGroupBox *gb = new QGroupBox(tr("ACS")); 
  QVBoxLayout *vl = new QVBoxLayout;
  acs_spectra_cb = new QCheckBox(tr("ACS spectra"));
  vl->addWidget(acs_spectra_cb);
  acs_a_time_cb = new QCheckBox(tr("ACS absorption time series"));
  vl->addWidget(acs_a_time_cb);
  acs_c_time_cb = new QCheckBox(tr("ACS attenuation time series"));
  vl->addWidget(acs_c_time_cb);
  gb->setLayout(vl);
  vl_top->addWidget(gb);

  gb = new QGroupBox(tr("CTD")); 
  vl = new QVBoxLayout;
  ctd_pressure_cb = new QCheckBox(tr("CTD pressure time series"));
  vl->addWidget(ctd_pressure_cb); 
  ctd_temp_cb = new QCheckBox(tr("CTD temperature time series"));
  vl->addWidget(ctd_temp_cb); 
  ctd_salin_cb = new QCheckBox(tr("CTD salinity time series"));
  vl->addWidget(ctd_salin_cb); 
  gb->setLayout(vl);
  vl_top->addWidget(gb);

  gb = new QGroupBox(tr("ECOBB3")); 
  vl = new QVBoxLayout;
  ecobb3_time_cb = new QCheckBox(tr("ECOBB3 time series"));
  vl->addWidget(ecobb3_time_cb); 
  gb->setLayout(vl);
  vl_top->addWidget(gb);

  gb = new QGroupBox(tr("HOCR")); 
  vl = new QVBoxLayout;

  hocr_ed_raw_time_cb = new QCheckBox(tr("HOCR Ed raw time series"));
  vl->addWidget(hocr_ed_raw_time_cb); 
  //hocr_ed_dark_time_cb = new QCheckBox(tr("HOCR Ed raw dark time series"));
  //vl->addWidget(hocr_ed_dark_time_cb); 
  hocr_ed_time_cb = new QCheckBox(tr("HOCR Ed processed time series"));
  vl->addWidget(hocr_ed_time_cb); 
  hocr_lu_raw_time_cb = new QCheckBox(tr("HOCR Lu raw time series"));
  vl->addWidget(hocr_lu_raw_time_cb); 
  //hocr_lu_dark_time_cb = new QCheckBox(tr("HOCR Lu dark time series"));
  //vl->addWidget(hocr_lu_dark_time_cb); 
  hocr_lu_time_cb = new QCheckBox(tr("HOCR Lu processed time series"));
  vl->addWidget(hocr_lu_time_cb);
  vl->addSpacing(10);

  hocr_ed_raw_spectra_cb = new QCheckBox(tr("HOCR Ed raw spectra"));
  vl->addWidget(hocr_ed_raw_spectra_cb); 
  hocr_ed_spectra_cb = new QCheckBox(tr("HOCR Ed processed spectra"));
  vl->addWidget(hocr_ed_spectra_cb); 
  hocr_lu_raw_spectra_cb = new QCheckBox(tr("HOCR Lu raw spectra"));
  vl->addWidget(hocr_lu_raw_spectra_cb); 
  hocr_lu_spectra_cb = new QCheckBox(tr("HOCR Lu processed spectra"));
  vl->addWidget(hocr_lu_spectra_cb);
  vl->addSpacing(10);

  hocr_es_time_cb = new QCheckBox(tr("HOCR Es processed time series"));
  vl->addWidget(hocr_es_time_cb); 
  hocr_es_spectra_cb = new QCheckBox(tr("HOCR Es processed spectra"));
  vl->addWidget(hocr_es_spectra_cb); 

  gb->setLayout(vl);
  vl_top->addWidget(gb);

  QPushButton* cancel_pb = new QPushButton(tr("Cancel"));
  QPushButton* ok_pb = new QPushButton(tr("OK"));

  QHBoxLayout* hl = new QHBoxLayout;
  vl_top->insertLayout(-1,hl);
  hl->insertStretch(100);
  hl->addWidget(cancel_pb);
  hl->addWidget(ok_pb);

  connect(cancel_pb, SIGNAL(pressed()), this, SLOT(reject()));
  connect(ok_pb, SIGNAL(pressed()), this, SLOT(accept()));

  //fsf_rb->setChecked(true);

  //connect(&button_group, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(radioButtonClicked(QAbstractButton*)));
  //connect(acs_device_pb, SIGNAL(pressed()), this, SLOT(acsDeviceLocatePressed()));
}



//  SensorConfigDialog::radioButtonClicked()

//void SensorConfigDialog::radioButtonClicked(QAbstractButton* b) {
//  updateItemsEnabled();
//}

/*
void SensorConfigDialog::updateItemsEnabled() {

  QAbstractButton* b = button_group.checkedButton();
 
  bool f(b == fsf_rb || b == fsf_no_hocr_rb); 
  acs_device_label->setEnabled(f);
  acs_device_le->setEnabled(f);
  acs_device_pb->setEnabled(f);

  grid_widget->setEnabled(b == custom_rb);
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) sens[i]->updateItemsEnabled(); 
}
*/


void VisualisationConfigDialog::parserToGui(const JParser& prm) {

  acs_spectra_cb->setChecked((prm.isDefinedAndTrueBool("vis_acs_spectra")));
  acs_a_time_cb->setChecked((prm.isDefinedAndTrueBool("vis_acs_a_time")));
  acs_c_time_cb->setChecked((prm.isDefinedAndTrueBool("vis_acs_c_time")));

  ctd_salin_cb->setChecked((prm.isDefinedAndTrueBool("vis_ctd_salinity")));
  ctd_pressure_cb->setChecked((prm.isDefinedAndTrueBool("vis_ctd_pressure")));
  ctd_temp_cb->setChecked((prm.isDefinedAndTrueBool("vis_ctd_temperature")));

  ecobb3_time_cb->setChecked((prm.isDefinedAndTrueBool("vis_ecobb3_time")));

  hocr_ed_time_cb->setChecked((prm.isDefinedAndTrueBool("vis_hocr_ed_time")));
  hocr_ed_raw_time_cb->setChecked((prm.isDefinedAndTrueBool("vis_hocr_ed_raw_time")));
  //hocr_ed_dark_time_cb->setChecked((prm.isDefinedAndTrueBool("vis_hocr_ed_dark_time")));
  hocr_lu_time_cb->setChecked((prm.isDefinedAndTrueBool("vis_hocr_lu_time")));
  hocr_lu_raw_time_cb->setChecked((prm.isDefinedAndTrueBool("vis_hocr_lu_raw_time")));
  //hocr_lu_dark_time_cb->setChecked((prm.isDefinedAndTrueBool("vis_hocr_lu_dark_time")));

  hocr_ed_raw_spectra_cb->setChecked((prm.isDefinedAndTrueBool("vis_hocr_ed_raw_spectra")));
  hocr_lu_raw_spectra_cb->setChecked((prm.isDefinedAndTrueBool("vis_hocr_lu_raw_spectra")));
  hocr_ed_spectra_cb->setChecked((prm.isDefinedAndTrueBool("vis_hocr_ed_spectra")));
  hocr_lu_spectra_cb->setChecked((prm.isDefinedAndTrueBool("vis_hocr_lu_spectra")));

  hocr_es_time_cb->setChecked((prm.isDefinedAndTrueBool("vis_hocr_es_time")));
  hocr_es_spectra_cb->setChecked((prm.isDefinedAndTrueBool("vis_hocr_es_spectra")));
}



// SensorConfigDialog::getParams()

void VisualisationConfigDialog::guiToParser(JParser& prm) const {

  prm.setLabelAndValue("vis_acs_spectra", (acs_spectra_cb->isChecked()) ? "yes" : "no"); 
  prm.setLabelAndValue("vis_acs_a_time", (acs_a_time_cb->isChecked()) ? "yes" : "no"); 
  prm.setLabelAndValue("vis_acs_c_time", (acs_c_time_cb->isChecked()) ? "yes" : "no"); 

  prm.setLabelAndValue("vis_ctd_salinity", (ctd_salin_cb->isChecked()) ? "yes" : "no"); 
  prm.setLabelAndValue("vis_ctd_pressure", (ctd_pressure_cb->isChecked()) ? "yes" : "no"); 
  prm.setLabelAndValue("vis_ctd_temperature", (ctd_temp_cb->isChecked()) ? "yes" : "no"); 

  prm.setLabelAndValue("vis_ecobb3_time", (ecobb3_time_cb->isChecked()) ? "yes" : "no"); 

  prm.setLabelAndValue("vis_hocr_ed_raw_time", (hocr_ed_raw_time_cb->isChecked()) ? "yes" : "no"); 
  //prm.setLabelAndValue("vis_hocr_ed_dark_time", (hocr_ed_dark_time_cb->isChecked()) ? "yes" : "no"); 
  prm.setLabelAndValue("vis_hocr_ed_time", (hocr_ed_time_cb->isChecked()) ? "yes" : "no"); 
  prm.setLabelAndValue("vis_hocr_lu_raw_time", (hocr_lu_raw_time_cb->isChecked()) ? "yes" : "no"); 
  //prm.setLabelAndValue("vis_hocr_lu_dark_time", (hocr_lu_dark_time_cb->isChecked()) ? "yes" : "no"); 
  prm.setLabelAndValue("vis_hocr_lu_time", (hocr_lu_time_cb->isChecked()) ? "yes" : "no"); 

  prm.setLabelAndValue("vis_hocr_ed_raw_spectra", (hocr_ed_raw_spectra_cb->isChecked()) ? "yes" : "no"); 
  prm.setLabelAndValue("vis_hocr_ed_spectra", (hocr_ed_spectra_cb->isChecked()) ? "yes" : "no"); 
  prm.setLabelAndValue("vis_hocr_lu_raw_spectra", (hocr_lu_raw_spectra_cb->isChecked()) ? "yes" : "no"); 
  prm.setLabelAndValue("vis_hocr_lu_spectra", (hocr_lu_spectra_cb->isChecked()) ? "yes" : "no"); 

  prm.setLabelAndValue("vis_hocr_es_time", (hocr_es_time_cb->isChecked()) ? "yes" : "no"); 
  prm.setLabelAndValue("vis_hocr_es_spectra", (hocr_es_spectra_cb->isChecked()) ? "yes" : "no"); 
}




/*
void VisualisationConfigDialog::acsDeviceLocatePressed() {

  QString s = QFileDialog::getOpenFileName(QString::null, QString::null, 0, 0, "Locate ACS Device File");
 
  if (s.isEmpty()) return;

  acs_device_le->setText(s);
}
*/

