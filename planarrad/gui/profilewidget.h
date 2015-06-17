
// profilewidget.h

#ifndef _PLANARRAD_PROFILEWIDGET_H
#define _PLANARRAD_PROFILEWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
#include <QVBoxLayout>
#include "profilechart.h"
#include "profiledata.h"
#include "datalistmodel.h"
#include "fe/textdrawitem2d.h"

//using namespace jude::fe;

class VisualisationOptionsDialog;



class ProfileWidget : public QWidget {

  Q_OBJECT

  const VisualisationOptionsDialog& vod;

  const DataListModel& data_model;

  QWidget* top_slider_padding;
  QWidget* bottom_slider_padding;

  QSlider* depth_sl;
  ProfileChart* profile_chart;

  TextDrawItem2D top_label;
  TextDrawItem2D bottom_label;

  QPushButton* add_pb;
  QCheckBox* log_top_cb;
  QCheckBox* log_bot_cb;
  QLineEdit* depth_le;

  Button* logtop_bt;
  Button* logbottom_bt;

  QVBoxLayout* pd_vl;

  double max_depth;
  double curr_depth;

  int top_axis_count;
  int bottom_axis_count;

  JList<ProfileData> pd_list;

  double maxDepthFromVisOptions();

public:

  ProfileWidget(const VisualisationOptionsDialog& vod, const DataListModel& data_model, QWidget* parent);

  //QSize sizeHint() const { return QSize(212,300); }

  bool hasTopAxis() const { return top_axis_count > 0; }
  bool hasBottomAxis() const { return bottom_axis_count > 0; }

  bool isLogTop() const { return log_top_cb->isChecked(); }
  bool isLogBottom() const { return log_bot_cb->isChecked(); }

  //bool isLogTop() const { return logtop_bt->isDown(); }
  //bool isLogBottom() const { return logbottom_bt->isDown(); }

signals:

  void signalCurrentDepthChanged(double d);

public slots:
    
  void slotDepthSliderMoved(int v);
  void slotDepthReturnPressed();
  void slotLogCheckBoxChanged();

  void slotDataSetsChanged();
  void slotUpdateMaxDepth();
  void slotSetValueAxesRange();

  void slotAddProfileData();
  void slotRemoveProfileData(ProfileData* pd);
  void slotAxisChanged(ProfileData* pd);

  void slotNewCurrentWavelength(ProfileData* src_pd);
};



#endif // _PLANARRAD_PROFILEWIDGET_H

