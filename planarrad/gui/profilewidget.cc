
// profilewidget.cc

#include "profilewidget.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include "base/jlog.h"
#include "visualisationoptionsdialog.h"



ProfileWidget::ProfileWidget(const VisualisationOptionsDialog& vod, const DataListModel& data_model, QWidget* parent) :
  vod(vod),
  data_model(data_model),
  top_axis_count(0),
  bottom_axis_count(0) {

  QLabel* label;

  QVBoxLayout* vl = new QVBoxLayout(this);
  vl->setMargin(0);
  vl->setSpacing(4);

  QHBoxLayout* hl = new QHBoxLayout(vl);
  hl->setMargin(0);
  hl->setSpacing(4);

  QVBoxLayout* vl2 = new QVBoxLayout(hl);
  vl2->setMargin(0);
  vl2->setSpacing(0);

  top_slider_padding = new QWidget(this);
  top_slider_padding->setFixedHeight(4+20);
  vl2->addWidget(top_slider_padding);

  depth_sl = new QSlider(Qt::Vertical, this);
  depth_sl->setMinimum(0);
  depth_sl->setMaximum(1000);
  vl2->addWidget(depth_sl);

  bottom_slider_padding = new QWidget(this);
  bottom_slider_padding->setFixedHeight(40);
  vl2->addWidget(bottom_slider_padding);

  profile_chart = new ProfileChart(this);
  hl->addWidget(profile_chart);
  hl->setStretchFactor(profile_chart, 100);
  vl->setStretchFactor(hl, 100);

  pd_vl = new QVBoxLayout(vl);

  vl->addSpacing(4);

  hl = new QHBoxLayout(vl);
  hl->setMargin(0);
  hl->setSpacing(4);

  add_pb = new QPushButton(" Add Line ", this);
  hl->addWidget(add_pb);

  hl->addStretch(100);

  log_bot_cb = new QCheckBox("LgB", this);
  hl->addWidget(log_bot_cb);
  log_top_cb = new QCheckBox("LgT", this);
  hl->addWidget(log_top_cb);

  label = new QLabel("Depth:");
  hl->addWidget(label);

  depth_le = new QLineEdit(this);
  depth_le->setMinimumWidth(40);
  hl->addWidget(depth_le);
  hl->setStretchFactor(depth_le, 30);

  label = new QLabel("(m)");
  hl->addWidget(label);

  setMinimumWidth(50);

  depth_sl->setSliderPosition(1000);
  depth_le->setText("0");
  curr_depth = 0;
  max_depth = 10;

  slotAddProfileData();

  connect(depth_sl, SIGNAL(sliderMoved(int)), this, SLOT(slotDepthSliderMoved(int)));
  connect(depth_le, SIGNAL(returnPressed()), this, SLOT(slotDepthReturnPressed()));

  connect(log_top_cb, SIGNAL(stateChanged(int)), this, SLOT(slotLogCheckBoxChanged()));
  connect(log_bot_cb, SIGNAL(stateChanged(int)), this, SLOT(slotLogCheckBoxChanged()));

  connect(add_pb, SIGNAL(clicked()), this, SLOT(slotAddProfileData()));
}



// ProfileWidget::slotDepthSliderMoved()

void ProfileWidget::slotDepthSliderMoved(int v) {

  curr_depth = (max_depth * (1000 - v)) / 1000;

  JString s = JString::number(curr_depth, 'f', 1);
  depth_le->setText(J2Q(s));

  profile_chart->setCurrentDepth(curr_depth);
  profile_chart->update();
  emit signalCurrentDepthChanged(curr_depth);
}



// ProfileWidget::slotDepthReturnPressed()

void ProfileWidget::slotDepthReturnPressed() {

  JString s = Q2J(depth_le->text());
  curr_depth = s.toDouble();

  if (curr_depth < 0) curr_depth = 0;
  if (curr_depth > max_depth) curr_depth = max_depth;

  depth_sl->setSliderPosition((int)(1000 - (curr_depth * 1000) / max_depth));

  profile_chart->setCurrentDepth(curr_depth);
  profile_chart->update();
  emit signalCurrentDepthChanged(curr_depth);
}



void ProfileWidget::slotLogCheckBoxChanged() {
  //jlog::ls << "isLog() " << isLog() << "\n";
  ProfileData* pd = pd_list.first();
  while (pd) {
    //jlog::ls << "pd " << (int64) pd << "\n";
    if (pd->isBottomAxis()) pd->setIsLog(isLogBottom());
    else pd->setIsLog(isLogTop());
    pd->rebuildLine();
    pd = pd_list.next();
  }
  profile_chart->setTopAxisIsLog(isLogTop());
  profile_chart->setBottomAxisIsLog(isLogBottom());
  slotSetValueAxesRange();
  profile_chart->update();
}



// ProfileWidget::slotDataSetsChanged()

void ProfileWidget::slotDataSetsChanged() {

  ProfileData* pd = pd_list.first();
  while (pd) {
    pd->slotDataSetListChanged();
    pd = pd_list.next();
  }
  slotSetValueAxesRange();
  profile_chart->update();
}



// ProfileWidget::slotUpdateMaxDepth()

void ProfileWidget::slotUpdateMaxDepth() {

  double old_max_depth = max_depth;

  max_depth = maxDepthFromVisOptions();

  if (old_max_depth == max_depth) return;

  jlog::ls << "new max depth " << max_depth << " " << vod.isFixedDepthRange() << "\n";

  if (curr_depth > max_depth) {
    curr_depth = max_depth;
    profile_chart->setCurrentDepth(curr_depth);
    JString s = JString::number(curr_depth, 'f', 1);
    depth_le->setText(J2Q(s));
    emit signalCurrentDepthChanged(curr_depth);
  }

  depth_sl->setSliderPosition((int)(1000 - (curr_depth * 1000) / max_depth));

  profile_chart->setMaxDepth(max_depth); 
  profile_chart->update();
}



void ProfileWidget::slotAddProfileData() {

  ProfileData* pd = new ProfileData(data_model, *profile_chart, this);

  if (pd->isBottomAxis()) {
    pd->setIsLog(isLogBottom());
    bottom_axis_count++;
  } else {
    pd->setIsLog(isLogTop());
    top_axis_count++;
  }

  connect(pd, SIGNAL(signalClose(ProfileData*)), this, SLOT(slotRemoveProfileData(ProfileData*)));
  connect(pd, SIGNAL(signalSetValueAxesRange()), this, SLOT(slotSetValueAxesRange()));
  connect(pd, SIGNAL(signalNewCurrentWavelength(ProfileData*)), this, SLOT(slotNewCurrentWavelength(ProfileData*)));
  connect(pd, SIGNAL(signalAxisChanged(ProfileData*)), this, SLOT(slotAxisChanged(ProfileData*)));

  pd_vl->addWidget(pd);
  pd_list.append(pd);

  pd->rebuildLine();
  slotSetValueAxesRange();
  profile_chart->update();
}



void ProfileWidget::slotRemoveProfileData(ProfileData* pd) {
  pd_vl->remove(pd);
  pd_list.remove(pd);
  if (pd->isBottomAxis()) bottom_axis_count--;
  else top_axis_count--;
  pd->deleteLater();
  slotSetValueAxesRange();
  profile_chart->update();
}



void ProfileWidget::slotAxisChanged(ProfileData* pd) {
  if (pd->isBottomAxis()) {
    bottom_axis_count++;
    top_axis_count--;
    pd->setIsLog(isLogBottom());
  } else {
    bottom_axis_count--;
    top_axis_count++;
    pd->setIsLog(isLogTop());
  }
  if (pd) pd->rebuildLine();
  slotSetValueAxesRange();
  profile_chart->update();
}



void ProfileWidget::slotNewCurrentWavelength(ProfileData* src_pd) {

  if (!src_pd->isWavelengthLocked()) return;

  jlog::ls << "new waveln: " << src_pd->currentWavelength() << "\n";

  ProfileData* pd = pd_list.first();
  while (pd) {
    if (pd != src_pd && pd->isWavelengthLocked()) {
      pd->setCurrentWavelength(src_pd->currentWavelength());
      pd->setWavelengthSliderPosition();
      pd->setWavelengthText();
      pd->rebuildLine();
    }
    pd = pd_list.next();
  }
  profile_chart->update();
}



// ProfileWidget::maxDepthFromVisOptions() [private]

double ProfileWidget::maxDepthFromVisOptions() {
  if (!vod.isFixedDepthRange()) {
    return (data_model.dataSetCount() == 0) ? 10 : data_model.maxDepth();
  } else {
    return vod.fixedMaxDepth();
  }
}



// ProfileChart::slotSetValueAxesRange()

void ProfileWidget::slotSetValueAxesRange() {

  if (hasBottomAxis()) {
    profile_chart->chartObject().xBottomAxis().setLabelItem(&bottom_label);
    profile_chart->chartObject().xBottomAxis().setDisplayNumbers(true);
    bottom_slider_padding->setFixedHeight(40);
  } else {
    profile_chart->chartObject().xBottomAxis().setLabelItem(0);
    profile_chart->chartObject().xBottomAxis().setDisplayNumbers(false);
    bottom_slider_padding->setFixedHeight(4);
  }

  if (hasTopAxis()) {
    profile_chart->chartObject().xTopAxis().setLabelItem(&top_label);
    profile_chart->chartObject().xTopAxis().setDisplayNumbers(true);
    top_slider_padding->setFixedHeight(40+20);
  } else {
    profile_chart->chartObject().xTopAxis().setLabelItem(0);
    profile_chart->chartObject().xTopAxis().setDisplayNumbers(false);
    top_slider_padding->setFixedHeight(4+20);
  }

  double max_value_bottom = 0;
  double min_value_bottom = 0;

  if (isLogBottom()) {
    max_value_bottom = -100;
    min_value_bottom = 100;
  }

  double max_value_top = 0;
  double min_value_top = 0;

  if (isLogTop()) {
    max_value_top = -100;
    min_value_top = 100;
  }

  int bottom_code = 0;
  int top_code = 0;

  ProfileData* pd = pd_list.first();
  while (pd) {

    if (pd->isTopAxis()) {
      pd->expandValueRange(min_value_top,  max_value_top);
      top_code |= pd->unitsCode();
    } else { 
      pd->expandValueRange(min_value_bottom, max_value_bottom);
      bottom_code |= pd->unitsCode();
    }

    pd = pd_list.next();
  }

  JString st = ProfileData::unitsString(top_code);
  JString sb = ProfileData::unitsString(bottom_code);

  if (hasTopAxis()) top_label.setText(st.isEmpty() ? "n/a" : st);
  if (hasBottomAxis()) bottom_label.setText(sb.isEmpty() ? "n/a" : sb);
  
  double range = max_value_bottom - min_value_bottom;
  if (isLogBottom() || min_value_bottom != 0) min_value_bottom -= range / 10;
  max_value_bottom += range / 10;

  range = max_value_top - min_value_top;
  if (isLogTop() || min_value_top != 0) min_value_top -= range / 10;
  max_value_top += range / 10;

  jlog::ls << " Bottom " << min_value_bottom << " " << max_value_bottom << "\n";
  jlog::ls << " Top " << min_value_top << " " << max_value_top << "\n";
 
  profile_chart->chartObject().xTopAxis().setLinkSubdivisions(0);
  profile_chart->chartObject().xBottomAxis().setLinkSubdivisions(0);

  if (hasBottomAxis() && min_value_bottom < max_value_bottom) {
    profile_chart->setBottomAxisRange(min_value_bottom, max_value_bottom);
  } 

  if (hasTopAxis() && min_value_top < max_value_top) {
    profile_chart->setTopAxisRange(min_value_top, max_value_top);
  } else {
    if (hasBottomAxis()) {
      profile_chart->chartObject().xTopAxis().setLinkSubdivisions(&profile_chart->chartObject().xBottomAxis());
      profile_chart->setTopAxisRange(profile_chart->chartObject().xBottomAxis().minValue(), profile_chart->chartObject().xBottomAxis().maxValue());
    } else {
      if (!isLogTop()) profile_chart->setTopAxisRange(0, 10);
      else profile_chart->setTopAxisRange(-2, 1);
    }
  }

  if (!hasBottomAxis() || min_value_bottom >= max_value_bottom) {
    if (hasTopAxis()) {
      profile_chart->chartObject().xBottomAxis().setLinkSubdivisions(&profile_chart->chartObject().xTopAxis());
      profile_chart->setBottomAxisRange(profile_chart->chartObject().xTopAxis().minValue(), profile_chart->chartObject().xTopAxis().maxValue());
    } else {
      if (!isLogBottom()) profile_chart->setBottomAxisRange(0, 10);
      else profile_chart->setBottomAxisRange(-2, 1);
    }
  }

  /*

  if (max_value_bottom > min_value_bottom) {
    profile_chart->setBottomAxisRange(min_value_bottom, max_value_bottom);
  } else {
    if (!isLogBottom()) profile_chart->setBottomAxisRange(0, 10);
    else profile_chart->setBottomAxisRange(-2, 1);
  }

  if (max_value_top > min_value_top) {
    profile_chart->setTopAxisRange(min_value_top, max_value_top);
  } else {
    if (!isLogTop()) profile_chart->setTopAxisRange(0, 10);
    else profile_chart->setTopAxisRange(-2, 1);
  }
  */

  profile_chart->setNeedRedraw();
}

