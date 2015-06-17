
// sensorselect.cc

#include "sensorselect.h"
#include <QHBoxLayout>
#include <QFileDialog>



/*
SensorSelect::SensorSelect(QWidget* p) : 
  QWidget(p) {

  QHBoxLayout* hl = new QHBoxLayout(this);

  sensor_cb = new QComboBox(this);
  hl->addWidget(sensor_cb);

  sensor_cb->addItem("RawData");
  sensor_cb->addItem("ACS");
  sensor_cb->addItem("CTD");
  sensor_cb->addItem("ECOBB3");
  sensor_cb->addItem("HOCR");

  device_file_le = new QLineEdit(this);
  hl->addWidget(device_file_le);

  QPushButton* b = new QPushButton("Load");
  hl->addWidget(b);
}
*/


SensorSelect::SensorSelect(QGridLayout* grid, int row, int col, QWidget* p) {

  sensor_cb = new QComboBox;
  grid->addWidget(sensor_cb, row, col++);

  sensor_cb->addItem("RawData");
  sensor_cb->addItem("ACS");
  sensor_cb->addItem("CTD");
  sensor_cb->addItem("ECOBB3");
  sensor_cb->addItem("HOCR");

  device_file_le = new QLineEdit;
  grid->addWidget(device_file_le, row, col++);

  locate_pb = new QPushButton("Locate");
  grid->addWidget(locate_pb, row, col++);

  connect(sensor_cb, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(sensorChanged(const QString&)));
  connect(locate_pb, SIGNAL(pressed()), this, SLOT(locateButtonPressed()));
}



void SensorSelect::setSensorType(const JString& s) {
  int index = sensor_cb->findText(J2Q(s));
  if (index < 0) index = 0;
  sensor_cb->setCurrentIndex(index);
}



void SensorSelect::sensorChanged(const QString& text) {
  updateItemsEnabled();
}



void SensorSelect::updateItemsEnabled() {

  bool f = (sensor_cb->isEnabled() && sensor_cb->currentText()=="ACS");

  device_file_le->setEnabled(f);
  locate_pb->setEnabled(f);
}




void SensorSelect::locateButtonPressed() {

  QString s = QFileDialog::getOpenFileName(QString::null, QString::null, 0, 0, "Locate Device File");
 
  if (s.isEmpty()) return;

  device_file_le->setText(s);
}

