
#ifndef _PLANARRAD_SPECDATA_H
#define _PLANARRAD_SPECDATA_H

#include <QWidget>
#include <QComboBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QImage>
#include <QCheckBox>
#include "datalistmodel.h"
#include "specchart.h"
#include "base/jcolour.h"
#include "fe/button.h"
#include "fe/buttongroup.h"
#include "colourcombo.h"
#include "datasetcombo.h"
#include "datatypecombo.h"
#include "chartS2/chartlinedata2d.h"

using namespace jude::base;
using namespace jude::fe;


class SpecData : public QWidget {

  Q_OBJECT

  const DataListModel& data_model;

  SpecChart& spec_chart;

  const DataSet* curr_ds;

  ChartLineData2D line_data;
  
  ColourCombo* colour_cb;
  DataSetCombo* data_set_cb;
  DataTypeCombo* data_type_cb;
  
  ButtonGroup axis_grp;
  
  Button* leftaxis_bt;
  Button* rightaxis_bt;
  
  Button* close_bt;
  
  bool is_log;

  double curr_depth;

public:

  SpecData(const DataListModel& data_model, SpecChart& pc, QWidget *p=0);
  ~SpecData();

  QSizePolicy sizePolicy() const;

  bool isLeftAxis() const { return leftaxis_bt->isDown(); }
  bool isRightAxis() const { return rightaxis_bt->isDown(); }

  void setIsLog(bool f) { is_log = f; }

  void setCurrentDepth(double d) { curr_depth = d; }

  void rebuildLine();

  void expandValueRange(double& min_v, double& max_v) const;

  int unitsCode() const { return data_type_cb->unitsCode(); }
  static const JString unitsString(int code) { return DataTypeCombo::unitsString(code); }

public slots:
    
  void slotDataSetListChanged();

  void slotColourCombo();
  void slotDataSetCombo();
  void slotDataTypeCombo();

  void slotAxisClicked(Button*);
  void slotCloseClicked();
  
signals:

  void signalSetValueAxesRange();
  void signalNewCurrentWavelength(SpecData*);
  void signalAxisChanged(SpecData*);
  void signalClose(SpecData*);
};



#endif // _PLANARRAD_SPECDATA_H

