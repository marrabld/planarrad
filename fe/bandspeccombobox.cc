
// bandspeccombobox.cc

#include "fe/bandspeccombobox.h"
#include "rad/bandspec.h"

using namespace jude::fe;



// (has_default) means the combo box should have a "<default>" option at the top
// which means use whatever the overall band spec is

BandSpecComboBox::BandSpecComboBox(bool has_default, QWidget* parent) :
  QComboBox(parent),
  has_default(has_default) { 

  rebuild();
}



void BandSpecComboBox::rebuild() {

  clear();

  int i = 0;

  if (has_default) { 
    insertItem("<default>", i);
    i++;
  }

    // this will record which one we want to appear by default
  int default_option = -1;

  BandSpec* bs = BandSpec::availableList().first();
  while (bs) {
    if (!bs->name().isEmpty()) insertItem(J2Q(bs->name()), i);
    else insertItem("<un-named>", i);
    if (bs == BandSpec::availableDefault()) default_option = i;
    bs = BandSpec::availableList().next();
    i++;
  }

    // if one wanted to be the default then set it
  if (default_option >= 0) setCurrentItem(default_option);
}



// BandSpecComboBox::currentItem()

BandSpec* BandSpecComboBox::currentBandSpec() {
  int i = currentItem();
  if (has_default) i--;
  if (i<0) return 0;
  return BandSpec::availableList().at(i);
}



// BandSpecComboBox::currentBandSpecFilepath()
 
const JString BandSpecComboBox::currentBandSpecFilepath() {
  BandSpec* bs = currentBandSpec();
  if (!bs) return JString::null;
  return bs->filepath();
}



// BandSpecComboBox::setCurrentItemFromFilepath()

void BandSpecComboBox::setCurrentBandSpecFromFilepath(const JString& fp) {

  if (fp.isEmpty()) return;

  BandSpec* bs = BandSpec::availableList().first();
  int i = (has_default) ? 1 : 0;
  while (bs) {
    jlog::ls << "Compare " << fp << " " << bs->filepath() << "\n";
    if (bs->filepath() == fp) { setCurrentItem(i); return; }
    else {
      JString fn1 = bs->filepath().section('/', bs->filepath().contains('/'), bs->filepath().contains('/'));
      JString fn2 = fp.section('/', fp.contains('/'), fp.contains('/'));
      if (fn1 == fn2) {
	setCurrentItem(i); 
	return;
      }
    }
    bs = BandSpec::availableList().next();
    i++;
  }
}

