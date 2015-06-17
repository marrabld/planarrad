
#include "specdata.h"
#include <QLayout>
#include <QLabel>
#include <QPixmap>
#include <QIcon>



// SpecData::(constructor)

SpecData::SpecData(const DataListModel& data_model, SpecChart& sc, QWidget* p) :
  QWidget(p),
  data_model(data_model),
  spec_chart(sc),
  is_log(false),
  curr_depth(0) {

  QVBoxLayout* vl = new QVBoxLayout(this);
  vl->setMargin(0);
  vl->setSpacing(2);

  QHBoxLayout* hl = new QHBoxLayout(vl);

  hl->setMargin(0);
  hl->setSpacing(2);

  colour_cb = new ColourCombo(this);
  hl->addWidget(colour_cb);
  connect(colour_cb, SIGNAL(currentIndexChanged(int)), this, SLOT(slotColourCombo()));

  data_set_cb = new DataSetCombo(data_model, this);
  hl->addWidget(data_set_cb);
  connect(data_set_cb, SIGNAL(signalUserChange()), this, SLOT(slotDataSetCombo()));

  data_type_cb = new DataTypeCombo(data_model, this);
  hl->addWidget(data_type_cb);
  connect(data_type_cb, SIGNAL(signalUserChange()), this, SLOT(slotDataTypeCombo()));

  hl->addSpacing(1);
  hl->addStretch(100);

  leftaxis_bt = new Button(QPixmap(":/resources/leftaxis_bt_16x16.png"), &axis_grp, this);
  leftaxis_bt->setIsTogglable(false);
  hl->addWidget(leftaxis_bt);
  rightaxis_bt = new Button(QPixmap(":/resources/rightaxis_bt_16x16.png"), &axis_grp, this);
  rightaxis_bt->setIsTogglable(false);
  hl->addWidget(rightaxis_bt);
  leftaxis_bt->setDown(true);
  connect(leftaxis_bt, SIGNAL(clicked(Button*)), this, SLOT(slotAxisClicked(Button*)));
  connect(rightaxis_bt, SIGNAL(clicked(Button*)), this, SLOT(slotAxisClicked(Button*)));

  hl->addSpacing(3);

  close_bt = new Button(QPixmap(":/resources/close_bt_16x16.png"), this);
  hl->addWidget(close_bt);
  connect(close_bt, SIGNAL(clicked(Button*)), this, SLOT(slotCloseClicked()));

  slotDataSetListChanged();
}



// SpecData::(destcructor)

SpecData::~SpecData() {
  spec_chart.chartObject().removeData00(line_data);
  spec_chart.chartObject().removeData01(line_data);
}

 

// SpecData::slotDataSetListChanged()
// called when a data set is added or removed

void SpecData::slotDataSetListChanged() {

  //jlog::ls << "SpecData::slotDataSetListChanged()\n";

  data_set_cb->rebuild();
  data_type_cb->rebuild(data_set_cb->currentDataSet());

  //jlog::ls << (int64) data_set_cb->currentDataSet() << " " << (int64) data_type_cb->currentSpecSet() << "\n";
  
  setEnabled(data_set_cb->currentDataSet());

  if (!data_type_cb->specSetChanged()) return;

  //jlog::ls << "SpecSet changed.\n";

   // maybe need to rescale the axes depending on the mode
  emit signalSetValueAxesRange();
    // rebuild this line as data source will have changed
  rebuildLine();
    // this was from user interaction with SPecData so have to trigger a screen update here
  spec_chart.update();
}



// SpecData::slotColourCombo()
// called when the colour combo is changed by the user

void SpecData::slotColourCombo() {
    // just rebuild line to change its colour - no axis change
  rebuildLine();
    // trigger a screen update
  spec_chart.update();
}



// SpecData::slotDataSetCombo()
// called when the data set combo is changed by the user

void SpecData::slotDataSetCombo() {

  data_type_cb->rebuild(data_set_cb->currentDataSet());

  if (!data_type_cb->specSetChanged()) return;

    // maybe need to rescale the axes depending on the mode
  emit signalSetValueAxesRange();
    // rebuild this line as data source will have changed
  rebuildLine();
    // this was from user interaction with SPecData so have to trigger a screen update here
  spec_chart.update();

}



// SpecData::slotDataSetCombo()
// called when the data type combo is changed directly by the user
// but not when a rebuild of the data type combo changes the selected

void SpecData::slotDataTypeCombo() {

  if (!data_type_cb->specSetChanged()) return;

    // maybe need to rescale the axes depending on the mode
  emit signalSetValueAxesRange();
    // rebuild this line as data source will have changed
  rebuildLine();
    // this was from user interaction with SPecData so have to trigger a screen update here
  spec_chart.update();
}



void SpecData::slotAxisClicked(Button* b) {
  emit signalAxisChanged(this);
}



void SpecData::slotCloseClicked() {
  spec_chart.chartObject().removeData00(line_data);
  spec_chart.chartObject().removeData01(line_data);
  rebuildLine();
  emit signalClose(this);
}



// SpecData::rebuildLine()

void SpecData::rebuildLine() {
  
  line_data.clearData();

  const SpecSet* spec_set = data_type_cb->currentSpecSet();
  if (!spec_set) return;

  vector<double> waveln_data;
  vector<double> val_data;


  spec_set->sampleInterpolated(waveln_data, val_data, curr_depth);

  if (is_log) {
    for (unsigned int i=0; i<val_data.size(); i++) { 
      val_data(i) = log10(val_data(i) > 0 ? val_data(i) : 1E-100);
      // jlog::ls << "v " << val_data(i) << "\n";
    }
  }

  //jlog::ls << "depth:\n" << depth_data << "\n";
  //jlog::ls << "val:\n" << val_data << "\n";

  line_data.setColour(colour_cb->lineColour());
  line_data.setData(waveln_data, val_data);

  if (waveln_data.size() == 1) {
    waveln_data.resize(2);
    waveln_data(1) = waveln_data(0) + 0.5;
    waveln_data(0) -= 0.5;
    val_data.resize(2);
    val_data(1) = val_data(0);
  }

  spec_chart.chartObject().removeData00(line_data);
  spec_chart.chartObject().removeData01(line_data);

  if (isLeftAxis()) {
    //jlog::ls << "REG0\n";
    spec_chart.chartObject().registerData00(line_data);
  } else {
    //jlog::ls << "REG1\n";
    spec_chart.chartObject().registerData01(line_data);
  }

    // the chart will need an internal redraw before it is updated on the screen
  spec_chart.setNeedRedraw();
}



// SpecData::expandValueRange()

void SpecData::expandValueRange(double& min_v, double& max_v) const {

  const SpecSet* curr_ss = data_type_cb->currentSpecSet();
  if (!curr_ss) return;

  double mn, mx;

  if (!is_log) {
    mn = curr_ss->minSampleValue();
    mx = curr_ss->maxSampleValue();
  } else {
    mn = curr_ss->minLog10SampleValue();
    mx = curr_ss->maxLog10SampleValue();
  }

  if (mx > max_v) max_v = mx; 
  if (mn < min_v) min_v = mn; 
}



// SpecData::sizePolicy()

QSizePolicy SpecData::sizePolicy() const {
  return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}



