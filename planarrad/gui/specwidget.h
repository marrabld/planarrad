
// specwidget.h

#ifndef _PLANARRAD_SPECWIDGET_H
#define _PLANARRAD_SPECWIDGET_H

#include "fe/basepanel.h"
#include <QTableView>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
#include <QVBoxLayout>
#include "specchart.h"
#include "specdata.h"
#include "datalistmodel.h"
#include "fe/textdrawitem2d.h"

//using namespace jude::fe;

class VisualisationOptionsDialog;



class SpecWidget : public BasePanel {

  Q_OBJECT

  const VisualisationOptionsDialog& vod;

  const DataListModel& data_model;

  SpecChart* spec_chart;

  TextDrawItem2D left_label;
  TextDrawItem2D right_label;

  QPushButton* add_pb;
  QCheckBox* log_left_cb;
  QCheckBox* log_right_cb;

  QVBoxLayout* sd_vl;

  double curr_depth;

  int left_axis_count;
  int right_axis_count;

  JList<SpecData> sd_list;

public:

  SpecWidget(const VisualisationOptionsDialog& vod, const DataListModel& data_model, QWidget* parent);

  bool hasLeftAxis() const { return left_axis_count > 0; }
  bool hasRightAxis() const { return right_axis_count > 0; }

  bool isLogLeft() const { return log_left_cb->isChecked(); }
  bool isLogRight() const { return log_right_cb->isChecked(); }

public slots:
    
  void slotLogCheckBoxChanged();

  void slotUpdateWavelengthRange();
  void slotDataSetsChanged();
  void slotSetValueAxesRange();

  void slotAddSpecData();
  void slotRemoveSpecData(SpecData* sd);
  void slotAxisChanged(SpecData* sd);

  void slotCurrentDepthChanged(double d);
};



#endif // _PLANARRAD_SPECWIDGET_H

