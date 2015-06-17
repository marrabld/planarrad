
// sensorconfigdialog.h

#ifndef _WLTOOL_GUI_SENSORCONFIGDIALOG_H
#define _WLTOOL_GUI_SENSORCONFIGDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include "sensorselect.h"
#include "wl/sensorconfig.h"

using namespace jude::wl;



class SensorConfigDialog : public QDialog {

  Q_OBJECT

  SensorSelect* sens[HARDWARE_CHANNEL_COUNT];

  QButtonGroup button_group;

  QRadioButton* raw_rb;
  QRadioButton* fsf_rb;
  QRadioButton* fsf_no_hocr_rb;
  QRadioButton* custom_rb;

  QLabel* acs_device_label;
  QLineEdit* acs_device_le;
  QPushButton* acs_device_pb;

  QWidget* grid_widget;

  QString current_dir_qs;

  void updateItemsEnabled();

public:

  SensorConfigDialog(const QString& dir, QWidget* p=0);

  void parserToGui(const JParser& prm);
  void guiToParser(JParser& prm) const;
 
public slots:
  void radioButtonClicked(QAbstractButton* b);
  void acsDeviceLocatePressed();

signals:
  void signalFilePathOpened(const JString& fp); 
};



#endif // _WLTOOL_GUI_SENSORCONFIGDIALOG_H

