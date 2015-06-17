
#include "hemiwidget.h"
#include <QApplication>
#include <QLabel>



HemiWidget::HemiWidget(const VisualisationOptionsDialog& vod, const DataListModel& data_model, QWidget* parent) :
  BasePanel(parent),
  vod(vod),
  data_model(data_model),
  hsd_data(0),
  need_auto_sens(true) {

  //setPalette(QApplication::palette());

  QVBoxLayout* vl = new QVBoxLayout(this);
  vl->setMargin(2);
  vl->setSpacing(4);

  hsd_view = new HSDView(this);
  vl->addWidget(hsd_view);
  vl->setStretchFactor(hsd_view, 100);

  QHBoxLayout* hl = new QHBoxLayout(vl);
  hl->setMargin(0);
  hl->setSpacing(2);

  QLabel* label = new QLabel("Sens.",this);
  hl->addWidget(label);

  sens_sl = new QSlider(Qt::Horizontal,this);
  sens_sl->setMinimum(0);
  sens_sl->setMaximum(1000);
  sens_sl->setSliderPosition(500);
  curr_rel_sens = 0.5;
  hl->addWidget(sens_sl);
  connect(sens_sl, SIGNAL(sliderMoved(int)), this, SLOT(slotSensSliderMoved(int)));

  hl->addSpacing(2);

  hl = new QHBoxLayout(vl);
  hl->setMargin(0);
  hl->setSpacing(2);

  data_set_cb = new DataSetCombo(data_model, this);
  data_set_cb->setIncludeNames(true);
  hl->addWidget(data_set_cb);
  connect(data_set_cb, SIGNAL(signalUserChange()), this, SLOT(slotDataSetCombo()));

  data_type_cb = new QComboBox(this);
  data_type_cb->addItem("Ld");
  data_type_cb->addItem("Lu");
  hl->addWidget(data_type_cb);
  connect(data_type_cb, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDataTypeCombo()));

  hl->addStretch(100);

  // hl->setStretchFactor(data_set_cb, 100);
  //hl->setStretchFactor(data_type_cb, 10);
}



// called when a data set is added or removed

void HemiWidget::slotDataSetsChanged() {

  jlog::ls << "HemiWidget::slotDataSetListChanged()\n";

  const DataSet* prev_ds = data_set_cb->currentDataSet();

  data_set_cb->rebuild();
  
  //setEnabled(data_set_cb->currentDataSet() != 0);

  if (data_set_cb->currentDataSet() != prev_ds) need_auto_sens = true;
    // rebuild this line as data source will have changed
  rebuildImage();
    // this was from user interaction with SPecData so have to trigger a screen update here
  update();
}



// SpecData::slotDataSetCombo()
// called when the data set combo is changed by the user

void HemiWidget::slotDataSetCombo() {

  need_auto_sens = true;
    // rebuild this line as data source will have changed
  rebuildImage();
    // this was from user interaction with SPecData so have to trigger a screen update here
  update();
}



// SpecData::slotDataSetCombo()
// called when the data type combo is changed directly by the user
// but not when a rebuild of the data type combo changes the selected

void HemiWidget::slotDataTypeCombo() {

  jlog::ls << "HERE " << data_type_cb->currentIndex() << "\n";

  hsd_view->setViewZPlusHemisphere(data_type_cb->currentIndex() == 0);

  need_auto_sens = true;
    // rebuild this line as data source will have changed
  rebuildImage();
    // this was from user interaction with SPecData so have to trigger a screen update here
  update();
}



void HemiWidget::slotCurrentDepthChanged(double d) {
  curr_depth = d;
    // rebuild this line as data source will have changed
  rebuildImage();
    // this was from user interaction with SPecData so have to trigger a screen update here
  update();
}



// HemiWidget::slotSensSliderMoved()

void HemiWidget::slotSensSliderMoved(int v) {

  curr_rel_sens = v / 1000.0;
    // rebuild
  rebuildImage();
    // this was from user interaction with SPecData so have to trigger a screen update here
  update();
}



void HemiWidget::rebuildImage() {

  // jlog::ls << " HemiWidget::rebuildImage()\n";

  const DataSet* ds = data_set_cb->currentDataSet();
  if (!ds) { hsd_view->setSourceData(0); return; }

  const RadDirecSet& rsd(ds->LDirecSet());

  delete hsd_data;
  hsd_data = rsd.sampleInterpolated(curr_depth);

  if (hsd_data == 0) { hsd_view->setSourceData(0); return; }

  hsd_view->setBandSpec(rsd.bandSpec());
  hsd_view->setSourceData(hsd_data);
  if (need_auto_sens) { 
    double s = hsd_view->autoSetSensK(); 
    need_auto_sens = false; 
    curr_rel_sens = s / hsd_view->maxSensK();
    sens_sl->setSliderPosition((int)(1000 * curr_rel_sens));
  } else hsd_view->setRelativeSensK(curr_rel_sens);
}


