
// specwidget.cc

#include "specwidget.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QApplication>
#include "base/jlog.h"
#include "visualisationoptionsdialog.h"



SpecWidget::SpecWidget(const VisualisationOptionsDialog& vod, const DataListModel& data_model, QWidget* parent) :
  BasePanel(parent),
  vod(vod),
  data_model(data_model),
  left_axis_count(0),
  right_axis_count(0) {

  //QLabel* label;

  //setPalette(QApplication::palette());

  QVBoxLayout* vl = new QVBoxLayout(this);
  vl->setMargin(2);
  vl->setSpacing(4);

  spec_chart = new SpecChart(this);
  vl->addWidget(spec_chart);
  vl->setStretchFactor(spec_chart, 100);

  sd_vl = new QVBoxLayout(vl);
  //vl->addSpacing(4);

  QHBoxLayout* hl = new QHBoxLayout(vl);
  hl->setMargin(0);
  hl->setSpacing(4);

  add_pb = new QPushButton(" Add Line ", this);
  hl->addWidget(add_pb);

  hl->addStretch(100);

  log_left_cb = new QCheckBox("LgL", this);
  hl->addWidget(log_left_cb);
  log_right_cb = new QCheckBox("LgR", this);
  hl->addWidget(log_right_cb);

  setMinimumWidth(50);

  curr_depth = 0;

  setAspectRatio(1.1);
  slotAddSpecData();

  //setBackgroundColor(QColor(255,255,255));
 
  left_label.setRotation(270);
  right_label.setRotation(270);

  connect(log_left_cb, SIGNAL(stateChanged(int)), this, SLOT(slotLogCheckBoxChanged()));
  connect(log_right_cb, SIGNAL(stateChanged(int)), this, SLOT(slotLogCheckBoxChanged()));

  connect(add_pb, SIGNAL(clicked()), this, SLOT(slotAddSpecData()));
}



void SpecWidget::slotLogCheckBoxChanged() {
  //jlog::ls << "isLog() " << isLog() << "\n";
  SpecData* sd = sd_list.first();
  while (sd) {
    //jlog::ls << "pd " << (int64) pd << "\n";
    if (sd->isLeftAxis()) sd->setIsLog(isLogLeft());
    else sd->setIsLog(isLogRight());
    sd->rebuildLine();
    sd = sd_list.next();
  }
  spec_chart->setLeftAxisIsLog(isLogLeft());
  spec_chart->setRightAxisIsLog(isLogRight());
  slotSetValueAxesRange();
  jlog::ls << "UPDATE\n";
  spec_chart->update();
}



// SpecWidget::slotDataSetsChanged()

void SpecWidget::slotDataSetsChanged() {

  SpecData* sd = sd_list.first();
  while (sd) {
    sd->slotDataSetListChanged();
    sd = sd_list.next();
  }
  slotSetValueAxesRange();
  spec_chart->update();
}



void SpecWidget::slotUpdateWavelengthRange() {

  //if (!vod.isFixedWavelengthRange()) {

  double min_waveln = 400;
  double max_waveln = 740;

  if (data_model.dataSetCount() != 0) {
    min_waveln = data_model.minWavelength();
    max_waveln = data_model.maxWavelength();
  }

  if (min_waveln == max_waveln) {
    min_waveln -= 0.5; 
    max_waveln += 0.5;
    spec_chart->chartObject().xBottomAxis().setTickSubdivision(1, 10);
    spec_chart->chartObject().xTopAxis().setTickSubdivision(1, 10); 
  } else {
    spec_chart->chartObject().xBottomAxis().setTickSubdivision(0.001, 10);
    spec_chart->chartObject().xTopAxis().setTickSubdivision(0.001, 10); 
  }
  spec_chart->chartObject().xBottomAxis().setValueRange(min_waveln, max_waveln);
  spec_chart->chartObject().xTopAxis().setValueRange(min_waveln, max_waveln);
  spec_chart->setNeedRedraw();
}



void SpecWidget::slotCurrentDepthChanged(double d) {

  //jlog::ls << "SpecWidget::slotCurrentDepthChanged() " << d << "\n";

  curr_depth = d;
  spec_chart->setCurrentDepth(d);

  SpecData* sd = sd_list.first();
  while (sd) {
    sd->setCurrentDepth(d);
    sd->rebuildLine();
    sd = sd_list.next();
  }

  spec_chart->setNeedRedraw();
  spec_chart->update();
}



void SpecWidget::slotAddSpecData() {

  SpecData* sd = new SpecData(data_model, *spec_chart, this);
  sd->setCurrentDepth(curr_depth);

  if (sd->isLeftAxis()) {
    sd->setIsLog(isLogLeft());
    left_axis_count++;
  } else {
    sd->setIsLog(isLogRight());
    right_axis_count++;
  }

  connect(sd, SIGNAL(signalClose(SpecData*)), this, SLOT(slotRemoveSpecData(SpecData*)));
  connect(sd, SIGNAL(signalSetValueAxesRange()), this, SLOT(slotSetValueAxesRange()));
  connect(sd, SIGNAL(signalAxisChanged(SpecData*)), this, SLOT(slotAxisChanged(SpecData*)));

  sd_vl->addWidget(sd);
  sd_list.append(sd);

  int h = 0;
  SpecData* sd2 = sd_list.first();
  while (sd2) {
    h += sd2->height();
    sd2 = sd_list.next();
  }


  //setFixedAdditionalHeight(sd->height()*sd_list.count());
  setFixedAdditionalHeight(h);
  setFixedSize(width(), ((int) (width() * aspectRatio())) + fixedAdditionalHeight());

  //jlog::ls << "HERE\n";
  sd->rebuildLine();
  slotSetValueAxesRange();
  spec_chart->update();
  //update();
}



void SpecWidget::slotRemoveSpecData(SpecData* sd) {
  sd_vl->remove(sd);
  sd_list.remove(sd);
  if (sd->isLeftAxis()) left_axis_count--;
  else right_axis_count--;
  sd->deleteLater();
  slotSetValueAxesRange();
  spec_chart->update();
}



void SpecWidget::slotAxisChanged(SpecData* sd) {
  if (sd->isLeftAxis()) {
    left_axis_count++;
    right_axis_count--;
    sd->setIsLog(isLogLeft());
  } else {
    left_axis_count--;
    right_axis_count++;
    sd->setIsLog(isLogRight());
  }
  if (sd) sd->rebuildLine();
  slotSetValueAxesRange();
  spec_chart->update();
}



// ProfileChart::slotSetValueAxesRange()

void SpecWidget::slotSetValueAxesRange() {

  if (hasLeftAxis()) {
    spec_chart->chartObject().yLeftAxis().setLabelItem(&left_label);
    spec_chart->chartObject().yLeftAxis().setDisplayNumbers(true);
  } else {
    spec_chart->chartObject().yLeftAxis().setLabelItem(0);
    spec_chart->chartObject().yLeftAxis().setDisplayNumbers(false);
  }

  if (hasRightAxis()) {
    spec_chart->chartObject().yRightAxis().setLabelItem(&right_label);
    spec_chart->chartObject().yRightAxis().setDisplayNumbers(true);
  } else {
    spec_chart->chartObject().yRightAxis().setLabelItem(0);
    spec_chart->chartObject().yRightAxis().setDisplayNumbers(false);
  }

  double max_value_left = 0;
  double min_value_left = 0;

  if (isLogLeft()) {
    max_value_left = -100;
    min_value_left = 100;
  }

  double max_value_right = 0;
  double min_value_right = 0;

  if (isLogRight()) {
    max_value_right = -100;
    min_value_right = 100;
  }

  int left_code = 0;
  int right_code = 0;

  SpecData* sd = sd_list.first();
  while (sd) {
    
    if (sd->isLeftAxis()) {
      sd->expandValueRange(min_value_left,  max_value_left);
      left_code |= sd->unitsCode();
    } else { 
      sd->expandValueRange(min_value_right, max_value_right);
      right_code |= sd->unitsCode();
    }

    sd = sd_list.next();
  }
  
  JString sl = SpecData::unitsString(left_code);
  JString sr = SpecData::unitsString(right_code);

  //jlog::ls << "top: " << st << "\n";
  //jlog::ls << "bot: " << sb << "\n";

  if (hasLeftAxis()) left_label.setText(sl.isEmpty() ? "n/a" : sl);
  if (hasRightAxis()) right_label.setText(sr.isEmpty() ? "n/a" : sr);

  left_label.setRotation(270);
  right_label.setRotation(270);
  
  double range = max_value_left - min_value_left;
  if (isLogLeft() || min_value_left != 0) min_value_left -= range / 10;
  max_value_left += range / 10;

  range = max_value_right - min_value_right;
  if (isLogRight() || min_value_right != 0) min_value_right -= range / 10;
  max_value_right += range / 10;

  jlog::ls << " Left " << min_value_left << " " << max_value_left << "\n";
  jlog::ls << " Right " << min_value_right << " " << max_value_right << "\n";

  spec_chart->chartObject().yRightAxis().setLinkSubdivisions(0);
  spec_chart->chartObject().yLeftAxis().setLinkSubdivisions(0);

  if (hasLeftAxis() && min_value_left < max_value_left) {
    spec_chart->setLeftAxisRange(min_value_left, max_value_left);
  } 

  if (hasRightAxis() && min_value_right < max_value_right) {
    spec_chart->setRightAxisRange(min_value_right, max_value_right);
  } else {
    if (hasLeftAxis()) {
      spec_chart->chartObject().yRightAxis().setLinkSubdivisions(&spec_chart->chartObject().yLeftAxis());
      spec_chart->setRightAxisRange(spec_chart->chartObject().yLeftAxis().minValue(), spec_chart->chartObject().yLeftAxis().maxValue());
    } else {
      if (!isLogRight()) spec_chart->setRightAxisRange(0, 10);
      else spec_chart->setRightAxisRange(-2, 1);
    }
  }

  if (!hasLeftAxis() || min_value_left >= max_value_left) {
    if (hasRightAxis()) {
      spec_chart->chartObject().yLeftAxis().setLinkSubdivisions(&spec_chart->chartObject().yRightAxis());
      spec_chart->setLeftAxisRange(spec_chart->chartObject().yRightAxis().minValue(), spec_chart->chartObject().yRightAxis().maxValue());
    } else {
      if (!isLogLeft()) spec_chart->setLeftAxisRange(0, 10);
      else spec_chart->setLeftAxisRange(-2, 1);
    }
  }

    // mark that redraw will be needed
  spec_chart->setNeedRedraw();
}

