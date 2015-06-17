
#include "datatypecombo.h"



// DataTypeCombo::(constructor)

DataTypeCombo::DataTypeCombo(const DataListModel& data_model, QWidget *p) :
  data_model(data_model),
  curr_ds(0),
  prev_ss(0),
  curr_ss(0) { 

  setSizeAdjustPolicy(AdjustToContents);

  rebuild(0);
}



// DataTypeCombo::rebuild()

void DataTypeCombo::rebuild(const DataSet* ds) {

  disconnect(this, SIGNAL(currentIndexChanged(int)), this, SIGNAL(slotCurrentIndexChanged()));

    // store what was before
  prev_ss = curr_ss;

  curr_ds = ds;

    // get the currently selected string, will try to select the same again
  JString prev_type = Q2J(currentText());

  clear();

    // if no data sets then just add a dummy "Ed"
  if (!curr_ds) {
    addItem("Ed");
  } else {
    if (curr_ds->EdSet().hasSampleData()) addItem("Ed");
    if (curr_ds->EuSet().hasSampleData()) addItem("Eu");
    if (curr_ds->LuSet().hasSampleData()) addItem("Lu");
    if (curr_ds->LdSet().hasSampleData()) addItem("Ld");
    if (curr_ds->EoSet().hasSampleData()) addItem("Eo");
    if (curr_ds->EouSet().hasSampleData()) addItem("Eou");
    if (curr_ds->EodSet().hasSampleData()) addItem("Eod");
    if (curr_ds->KuSet().hasSampleData()) addItem("Ku");
    if (curr_ds->KdSet().hasSampleData()) addItem("Kd");
    if (curr_ds->RSet().hasSampleData()) addItem("R");
    if (curr_ds->RtotSet().hasSampleData()) addItem("Rtot");
  }

    // try and find the old entry to select
  for (int i=0; i<count(); i++) {
    if (Q2J(itemText(i)) == prev_type) { setCurrentIndex(i); break; }
  }

  setFixedWidth(54);

  curr_ss = getCurrentSpecSet();

  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCurrentIndexChanged()));
}



// DataTypeCombo::slotCurrentIndexChanged()

void DataTypeCombo::slotCurrentIndexChanged() {

    // store what was before the user change
  prev_ss = curr_ss;

  curr_ss = getCurrentSpecSet();
 
  emit signalUserChange();
}



// DataTypeCombo::getCurrentSpecSet()

const SpecSet* DataTypeCombo::getCurrentSpecSet() const {

    // new currently selected
  if (!curr_ds) return 0;

  if (currentText()=="Ed") return &curr_ds->EdSet();
  if (currentText()=="Eu") return &curr_ds->EuSet();
  if (currentText()=="Ld") return &curr_ds->LdSet();
  if (currentText()=="Lu") return &curr_ds->LuSet();
  if (currentText()=="Kd") return &curr_ds->KdSet();
  if (currentText()=="Ku") return &curr_ds->KuSet();
  if (currentText()=="Eo") return &curr_ds->EoSet();
  if (currentText()=="Eou") return &curr_ds->EouSet();
  if (currentText()=="Eod") return &curr_ds->EodSet();
  if (currentText()=="R") return &curr_ds->RSet();
  if (currentText()=="Rtot") return &curr_ds->RtotSet();
  
  return 0;
}




int DataTypeCombo::unitsCode() const {

  if (!curr_ds) return 0;

  if (currentText()=="Ed" || currentText()=="Eu") return IRRAD;
  if (currentText()=="Ld" || currentText()=="Lu") return RAD;
  if (currentText()=="Kd" || currentText()=="Ku") return MM1;
  if (currentText()=="Eo" || currentText()=="Eou" || currentText()=="Eod") return IRRAD;
  if (currentText()=="R") return PROP;
  if (currentText()=="Rtot") return SRM1;

  return 0;
}



const JString DataTypeCombo::unitsString(int code) {

  JString s("");

  if (code & IRRAD) s += "Irrad. (Wm-2nm-1)";
  if (code & RAD) {
    if (!s.isEmpty()) s += " | ";
    s += "Rad. (Wm-2nm-1sr-1)";
  }
  if (code & MM1) {
    if (!s.isEmpty()) s += " | ";
    s += "(m-1)";
  }
  if (code & SRM1) {
    if (!s.isEmpty()) s += " | ";
    s += "(sr-1)";
  }
  if (code & PROP) {
    if (!s.isEmpty()) s += " | ";
    s += "(prop.)";
  }

  return s;
}

