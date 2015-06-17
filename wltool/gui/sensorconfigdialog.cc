
// sensorconfigdialog.cc

#include "sensorconfigdialog.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QButtonGroup>
#include <QFileDialog>




SensorConfigDialog::SensorConfigDialog(const QString& dir, QWidget* p) :
  QDialog(p),
  current_dir_qs(dir) {

  setCaption("Sensor Configuration");

  QVBoxLayout* vl = new QVBoxLayout(this);
  QGroupBox *gb_preset = new QGroupBox(tr("Preset")); 
  QVBoxLayout *vl_preset = new QVBoxLayout;

  //QButtonGroup* button_group = new QButtonGroup;

  raw_rb = new QRadioButton(tr("Raw Data"));
  button_group.addButton(raw_rb);
  vl_preset->addWidget(raw_rb);

  fsf_rb = new QRadioButton(tr("FSF Standard"));
  button_group.addButton(fsf_rb);
  vl_preset->addWidget(fsf_rb);
 
  fsf_no_hocr_rb = new QRadioButton(tr("FSF No HyperOCRs"));
  button_group.addButton(fsf_no_hocr_rb);
  vl_preset->addWidget(fsf_no_hocr_rb);
 
  QHBoxLayout* hl = new QHBoxLayout;
  vl_preset->insertLayout(-1,hl);

  acs_device_label = new QLabel(tr("ACS Device File:"));
  hl->addWidget(acs_device_label);
  acs_device_le = new QLineEdit;
  hl->addWidget(acs_device_le);
  acs_device_pb = new QPushButton(tr("Locate"));
  hl->addWidget(acs_device_pb);

  gb_preset->setLayout(vl_preset);
  vl->addWidget(gb_preset);
 
  QGroupBox *gb_custom = new QGroupBox(tr("Custom")); 
  QVBoxLayout *vl_custom = new QVBoxLayout;

  custom_rb = new QRadioButton("Use Custom Configuration");
  button_group.addButton(custom_rb);
  vl_custom->addWidget(custom_rb);
 
  grid_widget = new QWidget;

  QGridLayout* grid = new QGridLayout(grid_widget);
  grid->setVerticalSpacing(2);

  grid->addWidget(new QLabel(tr("Chan.")), 0, 0);
  grid->addWidget(new QLabel(tr("Sensor:")), 0, 1);
  grid->addWidget(new QLabel(tr("Device File:")), 0, 2);
  
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    grid->addWidget(new QLabel(QString::number(i+1)+"."), i+1, 0, Qt::AlignHCenter);
    sens[i] = new SensorSelect(grid, i+1, 1, grid_widget);
  }
  vl_custom->addWidget(grid_widget);

  gb_custom->setLayout(vl_custom);
  vl->addWidget(gb_custom);

  QPushButton* cancel_pb = new QPushButton(tr("Cancel"));
  QPushButton* ok_pb = new QPushButton(tr("OK"));

  hl = new QHBoxLayout;
  vl->insertLayout(-1,hl);
  hl->insertStretch(100);
  hl->addWidget(cancel_pb);
  hl->addWidget(ok_pb);

  connect(cancel_pb, SIGNAL(pressed()), this, SLOT(reject()));
  connect(ok_pb, SIGNAL(pressed()), this, SLOT(accept()));

  fsf_rb->setChecked(true);

  connect(&button_group, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(radioButtonClicked(QAbstractButton*)));
  connect(acs_device_pb, SIGNAL(pressed()), this, SLOT(acsDeviceLocatePressed()));
}



//  SensorConfigDialog::radioButtonClicked()

void SensorConfigDialog::radioButtonClicked(QAbstractButton* b) {
  updateItemsEnabled();
}


void SensorConfigDialog::updateItemsEnabled() {

  QAbstractButton* b = button_group.checkedButton();
 
  bool f(b == fsf_rb || b == fsf_no_hocr_rb); 
  acs_device_label->setEnabled(f);
  acs_device_le->setEnabled(f);
  acs_device_pb->setEnabled(f);

  grid_widget->setEnabled(b == custom_rb);
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) sens[i]->updateItemsEnabled(); 
}



void SensorConfigDialog::parserToGui(const JParser& prm) {

  JString sensor_setup(prm.getStringOrEmpty("sensor_setup"));
  if (sensor_setup == "raw") raw_rb->setChecked(true);
  else if (sensor_setup == "fsf_no_hocr") fsf_no_hocr_rb->setChecked(true);
  else if (sensor_setup == "custom") custom_rb->setChecked(true);
  else fsf_rb->setChecked(true);

  if (fsf_rb->isChecked() || fsf_no_hocr_rb->isChecked()) {
    acs_device_le->setText(J2Q(prm.getStringOrEmpty("chan1_device_fp")));
  }

  if (custom_rb->isChecked()) {
    for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
      JString chanX = JString("chan" + JString::number(i+1));
      if (prm.isDefined(chanX+"_sensor")) sens[i]->setSensorType(prm.getString(chanX+"_sensor"));
      if (prm.isDefined(chanX+"_device_fp")) sens[i]->setDeviceFilePath(prm.getString(chanX+"_device_fp"));
    }
  }

  updateItemsEnabled();
}



// SensorConfigDialog::getParams()

void SensorConfigDialog::guiToParser(JParser& prm) const {
 
  JString sensor_setup("fsf");
  if (raw_rb->isChecked()) sensor_setup = "raw";
  else if (fsf_no_hocr_rb->isChecked()) sensor_setup = "fsf_no_hocr";
  else if (custom_rb->isChecked()) sensor_setup = "custom";
  prm.setLabelAndValue("sensor_setup", sensor_setup);
  
    // remove any previous channel specs
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    JString chanX = JString("chan" + JString::number(i+1));
    prm.removeLabel(chanX+"_sensor");
    prm.removeLabel(chanX+"_device_fp");
  }
  
    // if an fsf preset then only one channel spec needed
  if (fsf_rb->isChecked() || fsf_no_hocr_rb->isChecked()) {
    if (!acs_device_le->text().isEmpty()) prm.setLabelAndValue("chan1_device_fp", Q2J(acs_device_le->text()));
  }

    // custom specifies all channels
  if (custom_rb->isChecked()) {
    for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
      if (sens[i]->sensorType() == "RawData") continue;
      JString chanX = JString("chan" + JString::number(i+1));
      prm.setLabelAndValue(chanX+"_sensor", sens[i]->sensorType());
      if (!sens[i]->deviceFilePath().isEmpty()) prm.setLabelAndValue(chanX+"_device_fp", sens[i]->deviceFilePath());
    }
  }

}



void SensorConfigDialog::acsDeviceLocatePressed() {

  QString s = QFileDialog::getOpenFileName(this, "Locate ACS Device File", current_dir_qs);
  //QString s = QFileDialog::getOpenFileName(QString::null, QString::null, 0, 0, "Locate ACS Device File");
 
  if (s.isEmpty()) return;

  JString js(Q2J(s));
  emit signalFilePathOpened(js); 

  acs_device_le->setText(s);
}

