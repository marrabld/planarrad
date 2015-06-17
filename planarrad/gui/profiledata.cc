
#include "profiledata.h"
#include <QLayout>
#include <QLabel>
#include <QPixmap>
#include <QIcon>



// ProfileData::(constructor)

ProfileData::ProfileData(const DataListModel& data_model, ProfileChart& pc, QWidget* p) :
  QWidget(p),
  data_model(data_model),
  profile_chart(pc),
  is_log(false),
  curr_waveln(0) {

  QLabel* label;

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

  waveln_le = new  QLineEdit(this);
  waveln_le->setMaxLength(10);
  hl->setStretchFactor(waveln_le, 100);
  hl->addWidget(waveln_le);

  label = new QLabel("(nm)",this);
  hl->addWidget(label);

  hl->addSpacing(1);

  bottomaxis_bt = new Button(QPixmap(":/resources/bottomaxis_bt_16x16.png"), &axis_grp, this);
  bottomaxis_bt->setIsTogglable(false);
  hl->addWidget(bottomaxis_bt);
  topaxis_bt = new Button(QPixmap(":/resources/topaxis_bt_16x16.png"), &axis_grp, this);
  topaxis_bt->setIsTogglable(false);
  hl->addWidget(topaxis_bt);
  bottomaxis_bt->setDown(true);
  connect(topaxis_bt, SIGNAL(clicked(Button*)), this, SLOT(slotAxisClicked(Button*)));
  connect(bottomaxis_bt, SIGNAL(clicked(Button*)), this, SLOT(slotAxisClicked(Button*)));

  hl->addSpacing(3);

  close_bt = new Button(QPixmap(":/resources/close_bt_16x16.png"), this);
  hl->addWidget(close_bt);
  connect(close_bt, SIGNAL(clicked(Button*)), this, SLOT(slotCloseClicked()));

  hl = new QHBoxLayout(vl);
  waveln_sl = new QSlider(Qt::Horizontal,this);
  waveln_sl->setMinimum(0);
  waveln_sl->setMaximum(1000);
  hl->addWidget(waveln_sl);

  waveln_locked_cb = new QCheckBox(this);
  hl->addWidget(waveln_locked_cb);

  connect(waveln_sl, SIGNAL(sliderMoved(int)), this, SLOT(slotWavelnSliderMoved(int)));
  connect(waveln_le, SIGNAL(returnPressed()), this, SLOT(slotWavelnReturnPressed()));
  
  slotDataSetListChanged();
  setWavelengthSliderPosition();
}



// ProfileData::(destcructor)

ProfileData::~ProfileData() {
  profile_chart.chartObject().removeData00(line_data);
  profile_chart.chartObject().removeData10(line_data);
}


 
// ProfileData::setCurrentWavelength()

void ProfileData::setCurrentWavelength(double w) {
  if (w < min_waveln) w = min_waveln;
  if (w > max_waveln) w = max_waveln;
  curr_waveln = w;
}

 

// ProfileData::slotDataSetListChanged()

void ProfileData::slotDataSetListChanged() {

  data_set_cb->rebuild();
  data_type_cb->rebuild(data_set_cb->currentDataSet());

  if (!data_set_cb->currentDataSet()) {

    waveln_le->setText("500");
    min_waveln = 500;
    max_waveln = 500;
    curr_waveln = 0;
    setEnabled(false);

  } else {

    min_waveln = data_set_cb->currentDataSet()->minWavelength();
    max_waveln = data_set_cb->currentDataSet()->maxWavelength();

    if (curr_waveln == 0) curr_waveln = (min_waveln + max_waveln) / 2;

    if (curr_waveln < min_waveln || curr_waveln > max_waveln) {
      curr_waveln = (curr_waveln < min_waveln) ? min_waveln : max_waveln;
    }

    setWavelengthText();
     
    setEnabled(true);
  }

  setWavelengthSliderPosition();

  if (!data_type_cb->specSetChanged()) return;

    // maybe need to rescale the axes depending on the mode
  emit signalSetValueAxesRange();
    // rebuild this line as data source will have changed
  rebuildLine();
    // this was from user interaction with SPecData so have to trigger a screen update here
  profile_chart.update();
}



// ProfileData::slotColourCombo()
// called when the colour combo is changed by the user

void ProfileData::slotColourCombo() {
    // just rebuild line to change its colour - no axis change
  rebuildLine();
    // trigger a screen update
  profile_chart.update();
}



// ProfileData::slotDataSetCombo()

void ProfileData::slotDataSetCombo() {

  data_type_cb->rebuild(data_set_cb->currentDataSet());

  if (!data_type_cb->specSetChanged()) return;

    // maybe need to rescale the axes depending on the mode
  emit signalSetValueAxesRange();
    // rebuild this line as data source will have changed
  rebuildLine();
    // this was from user interaction with SPecData so have to trigger a screen update here
  profile_chart.update();
}



// ProfileData::slotDataSetCombo()
// called when the data type combo is changed directly by the user
// but not when a rebuild of the data type combo changes the selected

void ProfileData::slotDataTypeCombo() {

  if (!data_type_cb->specSetChanged()) return;

    // maybe need to rescale the axes depending on the mode
  emit signalSetValueAxesRange();
    // rebuild this line as data source will have changed
  rebuildLine();
    // this was from user interaction so have to trigger a screen update here
  profile_chart.update();
}



// ProfileData::slotDepthSliderMoved()

void ProfileData::slotWavelnSliderMoved(int v) {

  curr_waveln = min_waveln + (((max_waveln - min_waveln) * v) / 1000);

  JString s = JString::number(curr_waveln, 'f', 0);
  waveln_le->setText(J2Q(s));
 
    // rebuild this line as data source will have changed
  rebuildLine();
    // this was from user interaction so have to trigger a screen update here
  profile_chart.update();

  emit signalNewCurrentWavelength(this);
}



// ProfileData::slotDepthReturnPressed()

void ProfileData::slotWavelnReturnPressed() {

  JString s = Q2J(waveln_le->text());
  curr_waveln = s.toDouble();

  if (curr_waveln < min_waveln) curr_waveln = min_waveln;
  if (curr_waveln > max_waveln) curr_waveln = max_waveln;

  waveln_sl->setSliderPosition((int)(((curr_waveln - min_waveln) * 1000) / (max_waveln - min_waveln)));
    
    // rebuild this line as data source will have changed
  rebuildLine();
    // this was from user interaction so have to trigger a screen update here
  profile_chart.update();

  emit signalNewCurrentWavelength(this);
}



void ProfileData::slotAxisClicked(Button* b) {
  emit signalAxisChanged(this);
}



void ProfileData::slotCloseClicked() {
  profile_chart.chartObject().removeData00(line_data);
  profile_chart.chartObject().removeData10(line_data);
  rebuildLine();
  emit signalClose(this);
}



// ProfileData::rebuildLine()

void ProfileData::rebuildLine() {
  
  line_data.clearData();

  const SpecSet* spec_set = data_type_cb->currentSpecSet();
  if (!spec_set) return;

  vector<double> depth_data;
  vector<double> val_data;

  spec_set->getProfileData(depth_data, val_data, curr_waveln);

  if (is_log) {
    for (unsigned int i=0; i<val_data.size(); i++) { 
      val_data(i) = log10(val_data(i) > 0 ? val_data(i) : 1E-100);
      // jlog::ls << "v " << val_data(i) << "\n";
    }
  }

  //jlog::ls << "depth:\n" << depth_data << "\n";
  //jlog::ls << "val:\n" << val_data << "\n";

  line_data.setColour(colour_cb->lineColour());
  line_data.setData(val_data, depth_data);

  profile_chart.chartObject().removeData00(line_data);
  profile_chart.chartObject().removeData10(line_data);

  if (isBottomAxis()) {
    //jlog::ls << "REG0\n";
    profile_chart.chartObject().registerData00(line_data);
  } else {
    //jlog::ls << "REG1\n";
    profile_chart.chartObject().registerData10(line_data);
  }

    // the chart will need an internal redraw before it is updated on the screen
  profile_chart.setNeedRedraw();
}



// ProfileData::expandValueRange()

void ProfileData::expandValueRange(double& min_v, double& max_v) const {

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



// ProfileData::sizePolicy()

QSizePolicy ProfileData::sizePolicy() const {
  return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}



// ProfileData::setWavelengthSliderPosition()

void ProfileData::setWavelengthSliderPosition() {
  if (max_waveln == min_waveln) {
    waveln_sl->setSliderPosition(500);
    waveln_sl->setEnabled(false);
    waveln_locked_cb->setEnabled(false);
  } else {
    waveln_sl->setSliderPosition((int)(((curr_waveln - min_waveln) * 1000) / (max_waveln - min_waveln)));
    waveln_sl->setEnabled(true);
    waveln_locked_cb->setEnabled(true);
  }
}



// ProfileData::setWavelengthText()

void ProfileData::setWavelengthText() {
  if (max_waveln == min_waveln) waveln_le->setText("500");
  else {
    JString s = JString::number(curr_waveln, 'f', 0);
    waveln_le->setText(J2Q(s));
  }
}

