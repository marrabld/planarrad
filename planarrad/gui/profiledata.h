
#ifndef _PLANARRAD_PROFILEDATA_H
#define _PLANARRAD_PROFILEDATA_H

#include <QWidget>
#include <QComboBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QImage>
#include <QCheckBox>
#include "datalistmodel.h"
#include "profilechart.h"
#include "base/jcolour.h"
#include "fe/button.h"
#include "fe/buttongroup.h"
#include "colourcombo.h"
#include "datasetcombo.h"
#include "datatypecombo.h"
#include "chartS2/chartlinedata2d.h"

using namespace jude::base;
using namespace jude::fe;


class ProfileData : public QWidget {

  Q_OBJECT

  const DataListModel& data_model;

  ProfileChart& profile_chart;

  const DataSet* curr_ds;

  ChartLineData2D line_data;
  
  ColourCombo* colour_cb;
  DataSetCombo* data_set_cb;
  DataTypeCombo* data_type_cb;
  
  QLineEdit* waveln_le;
  QSlider* waveln_sl;
  
  QCheckBox* waveln_locked_cb;

  ButtonGroup axis_grp;
  
  Button* topaxis_bt;
  Button* bottomaxis_bt;
  
  Button* close_bt;

  bool is_log;
  
  double curr_waveln;
  double min_waveln;
  double max_waveln;

public:

  ProfileData(const DataListModel& data_model, ProfileChart& pc, QWidget *p=0);
  ~ProfileData();

  QSizePolicy sizePolicy() const;

  void setCurrentWavelength(double w);
  double currentWavelength() const { return curr_waveln; }

  bool isWavelengthLocked() { return waveln_locked_cb->isChecked(); }

  bool isTopAxis() const { return topaxis_bt->isDown(); }
  bool isBottomAxis() const { return bottomaxis_bt->isDown(); }

  void setIsLog(bool f) { is_log = f; }

  void setWavelengthSliderPosition();
  void setWavelengthText();

  void rebuildLine();

  void expandValueRange(double& min_v, double& max_v) const;

  int unitsCode() const { return data_type_cb->unitsCode(); }
  static const JString unitsString(int code) { return DataTypeCombo::unitsString(code); }

public slots:
    
  void slotDataSetListChanged();

  void slotColourCombo();
  void slotDataSetCombo();
  void slotDataTypeCombo();

  void slotWavelnSliderMoved(int v);
  void slotWavelnReturnPressed();

  void slotAxisClicked(Button*);
  void slotCloseClicked();
  
signals:

  void signalSetValueAxesRange();
  void signalNewCurrentWavelength(ProfileData*);
  void signalAxisChanged(ProfileData*);
  void signalClose(ProfileData*);
};



#endif // _PLANARRAD_PROFILEDATA_H

