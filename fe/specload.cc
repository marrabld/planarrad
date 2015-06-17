
// specload.cc

#include "specload.h"
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include "rad/bandspec.h"
#include "fe/bandspeccombobox.h"
#include "rad/sbanddata.h"
#include "math/ipol.h"
#include "base/jlog.h"

using namespace jude::math;
using namespace jude::rad;
using namespace jude::fe;



// SpecLoad::(constructor)

SpecLoad::SpecLoad(const JString& text, QWidget* parent, QVBoxLayout* vbox, bool stretch_hbox, const JString& ct) :
  FileLoad(text, parent, vbox, false, ct, false) {
 
  bandspec_cm = new BandSpecComboBox(false, FileLoad::w);
  FileLoad::hbox->addWidget(bandspec_cm);
 
  if (stretch_hbox) FileLoad::hbox->addStretch(100);
}



// SpecLoad::(constructor)

SpecLoad::SpecLoad(const JString& text, QWidget* parent, QGridLayout* grid, int row, int start_col, const JString& ct) :
  FileLoad(text, parent, grid, row, start_col, ct, false) {

  bandspec_cm = new BandSpecComboBox(false, parent);
  grid->addWidget(bandspec_cm, row, start_col+3);
}



void SpecLoad::rebuild() {
  bandspec_cm->rebuild();
}



// SpecLoad::setEnabled()

void SpecLoad::setEnabled(bool f) {
  FileLoad::setEnabled(f);
  bandspec_cm->setEnabled(f);
}



// SpecLoad::sourceFilepath()

const JString SpecLoad::sourceFilepath() { 
  return FileLoad::filepath(); 
}



// SpecLoad::setSourceFilepath()

void SpecLoad::setSourceFilepath(const JString& fp) { 
  FileLoad::setFilepath(fp); 
}



// SpecLoad::setBandSpec()

void SpecLoad::setBandSpecFilepath(const JString& fp) {
  bandspec_cm->setCurrentBandSpecFromFilepath(fp);
}



// SpecLoad::compatibleFilepath()
// todo - put resample reoutines into common place
#ifdef XXX
const JString SpecLoad::compatibleFilepath(BandSpec& bs) {

    // if the file is in the right bands then just use directly
  if (bandSpecFilepath().isEmpty() || bs.filepath() == bandSpecFilepath() || sourceFilepath().isEmpty()) return sourceFilepath();

    // get the source file band spec
  bool dummy;
  BandSpec* src_bs = BandSpec::createFromFile(bandSpecFilepath(), dummy);
  if (src_bs==0) { /* FIXME */ }

  const vector<double>& src_x = src_bs->bandCentres();
  const vector<double>& targ_x = bs.bandCentres();

    // read the source data
  SBandData src_sd;
  if (!FileUtils::readFactoryBuiltFromFile(src_sd, FileDesc(sourceFilepath(), true))) {
    /* FIXME */
  }

  if (jlog::vbLevel(5)) {
    jlog::ls << "targ_x:\n" << targ_x << "\nsrc_x:\n" << src_x << "\n";
    jlog::ls << "src_y:\n" << src_sd.vectorData() << "\n";
  }

    // generated interpolated data at required bands
  SBandData targ_sd(Ipol::linearInterpolate(targ_x, src_sd.vectorData(), src_x));

  if (jlog::vbLevel(5)) {
    jlog::ls << "targ_y:\n" << targ_sd.vectorData() << "\n";
  }

  JString fp = temp_filepath + "_" + bs.code();

    // write to temporary file
  jlog::ls << "Writing temporary file: " << fp << "\n";
  if (!FileUtils::writeFactoryBuiltToFile(targ_sd, FileDesc(fp, true))) {
    /* FIXME */
  }
  delete src_bs;

  return fp;
}
#endif


// SpecLoad::bandSpec()

const JString SpecLoad::bandSpecFilepath() {
  BandSpec* bs = bandspec_cm->currentBandSpec();
  if (!bs) return JString::null;
  return bs->filepath();
}

