
// direcspeccombobox.cc

#include "fe/direcspeccombobox.h"
#include "ds/direcspec.h"

using namespace jude::fe;



DirecSpecComboBox::DirecSpecComboBox(bool has_default, QWidget* parent) :
  QComboBox(parent) {

  rebuild();
}



void DirecSpecComboBox::rebuild() {

  clear();
    // this will record which one we want to appear by default
  int default_option = -1;

  int i = 0;
  DirecSpec* ds_spec = DirecSpec::availableList().first();
  while (ds_spec) {
    if (!ds_spec->name().isEmpty()) insertItem(J2Q(ds_spec->name()), i);
    else insertItem("<un-named>", i);
    if (ds_spec == DirecSpec::availableDefault()) default_option = i;
    ds_spec = DirecSpec::availableList().next();
    i++;
  }

    // if one wanted to be the default then set it
  if (default_option >= 0) setCurrentItem(default_option);
}



// DirecSpecComboBox::currentDirecSpec()

DirecSpec* DirecSpecComboBox::currentDirecSpec() {
  int i = currentItem();
  return DirecSpec::availableList().at(i);
}



// DirecSpecComboBox::currentDirecSpecFilepath()
 
const JString DirecSpecComboBox::currentDirecSpecFilepath() {
  DirecSpec* ds = currentDirecSpec();
  return ds->filepath();
}



// DirecSpecComboBox::setCurrentDirecSpecFromFilepath()

void DirecSpecComboBox::setCurrentDirecSpecFromFilepath(const JString& fp) {

  if (fp.isEmpty()) return;
 
  DirecSpec* ds = DirecSpec::availableList().first();
  int i = 0;
  while (ds) {
    if (ds->filepath() == fp) setCurrentItem(i);
    ds = DirecSpec::availableList().next();
    i++;
  }
}

