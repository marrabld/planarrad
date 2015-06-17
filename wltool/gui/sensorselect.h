
// sensorselect.h

#ifndef _WLTOOL_GUI_SENSORSELECT_H
#define _WLTOOL_GUI_SENSORSELECT_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>

#include "base/jstring.h"

using namespace jude::base;



class SensorSelect : public QObject {

  Q_OBJECT

  QComboBox* sensor_cb;
  QLineEdit* device_file_le;
  QPushButton* locate_pb;

public:

  //SensorSelect(QWidget* p);
  SensorSelect(QGridLayout* grid, int row, int col, QWidget* p=0);

  void updateItemsEnabled();

  const JString sensorType() const { return Q2J(sensor_cb->currentText()); }  
  void setSensorType(const JString& s);

  const JString deviceFilePath() const { return Q2J(device_file_le->text()); }  
  void setDeviceFilePath(const JString& s) { return device_file_le->setText(J2Q(s)); }  

public slots:
  void sensorChanged(const QString& text);
  void locateButtonPressed();
};



#endif // _WLTOOL_GUI_SENSORSELECT_H

