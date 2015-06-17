
#include "datasetcombo.h"



// DataSetCombo::(constructor)

DataSetCombo::DataSetCombo(const DataListModel& data_model, QWidget *p) :
  data_model(data_model),
  prev_ds(0),
  curr_ds(0),
  include_names(false) { 

  setSizeAdjustPolicy(AdjustToContents);

  rebuild();
}



// DataSetCombo::rebuild()

void DataSetCombo::rebuild() {

  disconnect(this, SIGNAL(currentIndexChanged(int)), this, SIGNAL(slotCurrentIndexChanged()));

  clear();

    // how many to add - will add adummy "1" if no data sets
  int num_to_add = data_model.dataSetCount();
  if (num_to_add < 1) num_to_add = 1;

    // if a dataset was removed then may be renumbered
    // so tries to find the actual pointer
  bool found_ds = false;

    // add entries including dummy "1" if no data sets
  for (int i=0; i<num_to_add; i++) {
    JString s = JString::number(i+1);
    if (include_names) s += " " + data_model.dataSet(i)->name();
    addItem(J2Q(s));
    if (data_model.dataSet(i) == curr_ds) { setCurrentIndex(i); found_ds=true; }
  }

    // if the previous data set has gone then set to first or clear if none
  if (!found_ds) {
    setCurrentIndex(0);
    curr_ds = (data_model.dataSetCount() > 0) ? data_model.dataSet(0) : 0;
  }

  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCurrentIndexChanged()));
}



// DataSetCombo::slotCurrentIndexChanged()

void DataSetCombo::slotCurrentIndexChanged() {

    // store what was before the user change
  prev_ds = curr_ds;
    // new currently selected
  curr_ds = (data_model.dataSetCount() > currentIndex()) ? data_model.dataSet(currentIndex()) : 0;

  emit signalUserChange();
}

