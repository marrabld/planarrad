
// hemiwidget.h

#ifndef _PLANARRAD_HEMIWIDGET_H
#define _PLANARRAD_HEMIWIDGET_H

#include "fe/basepanel.h"
#include <QTableView>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
#include <QVBoxLayout>
#include "fe/hsdview.h"
#include "specdata.h"
#include "datalistmodel.h"
#include "fe/textdrawitem2d.h"

//using namespace jude::fe;

class VisualisationOptionsDialog;



class HemiWidget : public BasePanel {

  Q_OBJECT

  const VisualisationOptionsDialog& vod;

  const DataListModel& data_model;

  RadDirecImp<SBandData>* hsd_data;

  HSDView* hsd_view;
 
  DataSetCombo* data_set_cb;
  QComboBox* data_type_cb;

  QSlider* sens_sl;

  //HemiChart* spec_chart;

  //TextDrawItem2D left_label;
  //TextDrawItem2D right_label;

  //QPushButton* add_pb;
  //QCheckBox* log_left_cb;
  //QCheckBox* log_right_cb;

  //QVBoxLayout* sd_vl;

  double curr_depth;

  double curr_rel_sens;

  bool need_auto_sens;

  //int left_axis_count;
  //int right_axis_count;

  //JList<SpecData> sd_list;

  void rebuildImage();

public:

  HemiWidget(const VisualisationOptionsDialog& vod, const DataListModel& data_model, QWidget* parent);

  //bool hasLeftAxis() const { return left_axis_count > 0; }
  //bool hasRightAxis() const { return right_axis_count > 0; }

  //bool isLogLeft() const { return log_left_cb->isChecked(); }
  //bool isLogRight() const { return log_right_cb->isChecked(); }

public slots:
    
  void slotDataSetsChanged();
  void slotDataSetCombo();
  void slotDataTypeCombo();

  void slotCurrentDepthChanged(double d);
  void slotSensSliderMoved(int v);
};



#endif // _PLANARRAD_HEMIWIDGET_H

