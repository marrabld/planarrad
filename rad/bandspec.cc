
// bandspec.cc

#include "rad/bandspec.h"
#include "rad/sbanddata.h"
#include "base/jparser.h"
#include "base/jfile.h"
#include "base/jlog.h"
#include "base/jdir.h"
#include "base/jconstants.h"
#include "math/ipol.h"
#include "math/ublas.h"
#include "rad/report.h"
//#include "config.h"

using namespace jude::base;
using namespace jude::math;
using namespace jude::rad;



  // for initialising a default of 17 bands 400-740 nm
static const double default_centres[17] = { 410,430,450,470,490,510,530,550,570,590,610,630,650,670,690,710,730 };
static const double default_widths[17] = { 20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20 };



  // for building tristimulus functions
static const int TRISTIM_ELEMS = 43;

static const double tri_wave[TRISTIM_ELEMS] = { 
  360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470,
  480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 
  600, 610, 620, 630, 640, 650, 660, 670, 680, 690, 700, 710,
  720, 730, 740, 750, 760, 770, 780 };

static const double tri_red[TRISTIM_ELEMS] = { 
  0.0001, 0.0004, 0.0014, 0.0042, 0.0143, 0.0435, 0.1344, 0.2839, 0.3483, 0.3362,
  0.2908, 0.1954, 0.0956, 0.0320, 0.0049, 0.0093, 0.0633, 0.1655, 0.2904, 0.4334,
  0.5945, 0.7621, 0.9163, 1.0263, 1.0622, 1.0026, 0.8544, 0.6424, 0.4479, 0.2835, 
  0.1649, 0.0874, 0.0467, 0.0227, 0.0114, 0.0058, 0.0028, 0.0014, 0.0007, 0.0003,
  0.0002, 0.0001, 0.0000 };

static const double tri_green[TRISTIM_ELEMS] = {
  0.0000, 0.0000, 0.0000, 0.0001, 0.0004, 0.0012, 0.0040, 0.0116, 0.0230, 0.0380, 
  0.0600, 0.0910, 0.1390, 0.2080, 0.3230, 0.5030, 0.7100, 0.8620, 0.9540, 0.9950, 
  0.9950, 0.9520, 0.8700, 0.7570, 0.6310, 0.5030, 0.3810, 0.2650, 0.1750, 0.1070,
  0.0610, 0.0320, 0.0170, 0.0082, 0.0041, 0.0021, 0.0011, 0.0005, 0.0003, 0.0001,
  0.0000, 0.0000, 0.0000 };

static const double tri_blue[TRISTIM_ELEMS] = {
  0.0006, 0.0019, 0.0201, 0.0201, 0.0679, 0.2074, 0.6456, 1.3856, 1.7471, 1.7721, 
  1.6692, 1.2876, 0.8130, 0.4652, 0.2720, 0.1582, 0.0782, 0.0422, 0.0203, 0.0087,
  0.0039, 0.0021, 0.0017, 0.0011, 0.0008, 0.0003, 0.0002, 0.0001, 0.0000, 0.0000,
  0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
  0.0000, 0.0000, 0.0000 };



  // static list of pre-loaded BandSpec's used for GUI applications
JList<BandSpec> BandSpec::available_list;
  // the default in the list
BandSpec* BandSpec::default_bs = 0;



// BandSpec::initAvailableList()

void BandSpec::initAvailableList() {
    // will create BandSpec's here using new so will need to be deleted when app quits
  available_list.setAutoDelete(true);
    // no default 
  default_bs = 0;
    // clear list
  available_list.clear();
}



// BandSpec::addToAvailableList()

BandSpec* BandSpec::addToAvailableList(const JParser& prm, const JString& prefix, bool make_default) {

  bool preferred_default;

    // first try to see if there is one directly specified in the parser or via a file
  BandSpec* bs = create(prm, prefix, preferred_default);
    // quit if none found
  if (!bs) return 0;
    // log
  if (jlog::vbLevel(5)) jlog::ls << "Adding band specification '" << bs->name() << "' (" << bs->bandCount() << " bands) to list\n";
    // add to list
  available_list.append(bs);
    // and make default if requested from specification and no previous default
    // or always if demanded from caller
  if ((preferred_default && !default_bs) || make_default) default_bs = bs;
    // return pointer
  return bs;
}



/*! Populates the static list of available BandSpecs by reading files in the directory \a dir_path.
  Returns false if the directory does not exist.
  \todo Fix for fixed band compilations - should reject BandSpecs without the right number of bands. 
*/

bool BandSpec::addToAvailableList(const JString& dir_path) {

  if (jlog::vbLevel(5)) jlog::ls << "Looking for band specification \".bsf\" files in '" << dir_path << "'.\n";

    // get list of files in the directory
  JDir dir(dir_path);

  if (!dir.exists()) return false;

  JList<JString> entry_list;
  entry_list.setAutoDelete(true);
  dir.getEntryList(entry_list);

  int found_count = 0;

    // go through each file
  JString* fp = entry_list.first();
  while (fp) {
      // if ends in ".txt" then try to load it as a direc spec
    if (fp->right(4) == ".bsf") {

      bool preferred_default;
        // try to create from the file - returns null if failed
      BandSpec* bs = createFromFile(dir_path + "/" + *fp, preferred_default);
        // if went ok then add to avauilable list
      if (bs) { 
	  // add to list
	available_list.append(bs); 
	  // and make default if requested
	if (preferred_default && !default_bs) default_bs = bs;
	found_count++; 
      }
    }
      // next file in dir
    fp = entry_list.next();
  }

    // info to log
  if (jlog::vbLevel(5)) {
    if (found_count==0) jlog::ls << "None found!\n";
    else jlog::ls << "Found " << found_count << ".\n";
  }

  return true;
}



// BandSpec::addDefaultToAvailableList() [static]

bool BandSpec::addDefaultToAvailableList() {

    // if got some in the list then don't need to create a default
  if (available_list.count() > 0) return false;

  if (jlog::vbLevel(5)) jlog::ls << "Creating default 17 bands 400-740 nm.\n";

  BandSpec* bs = new BandSpec(17, default_centres, default_widths);
  bs->setName("17 Bands (400-740 nm)");
  bs->setCode("17");

    // add to list
  available_list.append(bs);
    // make default
  default_bs = bs;

  return true;
}



// BandSpec::create()
/*! Creates a band specification directly from the supplied parser data or via a referenced file. */
 
BandSpec* BandSpec::create(const JParser& prm, const JString& prefix) {
  bool dummy;
  return create(prm, prefix, dummy);
}



// BandSpec::create()
/*! Creates a band specification directly from the supplied parser data or via a referenced file. */
 
BandSpec* BandSpec::create(const JParser& prm, const JString& prefix, bool& preferred_default) {

    // if 'bands' is defined in prm then the specification is in prm
  if (prm.isDefined(prefix+"band_count")) {

      // create a band spec with no file path
    BandSpec* bs = new BandSpec("");
      // read it directly from the parser - return if went ok
    if (bs->readFromParser(prm, prefix, preferred_default)) return bs;
      // otherwise delete the band spec
    delete bs;
      // quit with failure
    return 0;

    // if 'band_spec' spec is defined then the supplied file is the parser specification
  } else if (prm.isDefined(prefix+"band_spec_fp")) {

      // make a file descriptor for the file to read
    return createFromFile(prm.getString(prefix+"band_spec_fp"), preferred_default);
  }

    // neither 'bands' nor 'band_spec' defined so quit with failure
  return 0;
}



// BandSpec::createFromFile()
/*! Creates a band specification directly from a file, specified by \a file_path. */

BandSpec* BandSpec::createFromFile(const JString& file_path, bool& preferred_default) {

  if (jlog::vbLevel(5)) jlog::ls.printf("Loading band specification file: '%s'.\n", file_path.latin1());

    // make a parser from the file
  JParser bs_prm;

  if (bs_prm.readFile(file_path, 5)) {
      // create a band spec with file path
    BandSpec* bs = new BandSpec(file_path);
      // read it from the new parser with no prefix - return if went ok
    if (bs->readFromParser(bs_prm, "", preferred_default)) return bs;
      // otherwise delete the band spec
    delete bs;
  }

    // couldn't load
  if (jlog::vbLevel(5)) jlog::ls.printf("File '%s' ignored.\n", file_path.latin1());

  return 0;
}  



BandSpec* BandSpec::createFromReport(const JTextTable& ttab) {

  int band_count = -1;
  int curr_row = 0;

    // go through rowws of table looking for band count
  while (curr_row < ttab.rowCount()) {
      // code label fo rthe data on this row or next
    const JString& code = ttab.getText(curr_row, 0).lower();
      // get if band count
    if (code == "band_count") { band_count = ttab.getText(curr_row, 2).toInt(); break; }
      // try next row
    curr_row++;
  }

  if (jlog::vb(5)) jlog::ls << "Band count: " << band_count << "\n"; 

  if (band_count < 0) {
    jlog::es << "Report text file does not contain 'band_count' entry.\n";
    return 0;
  }

  std::vector<double> band_centres;
  std::vector<double> band_widths;

    // go through rowws of table looking for band centres and widths
  curr_row = 0;
  while (curr_row < ttab.rowCount()) {

      // code label fo rthe data on this row or next
    const JString& code = ttab.getText(curr_row, 0).lower();

      // get if band centres
    if (code == "band_centres") { 
      band_centres.resize(band_count);
      for (int i=0; i<band_count; i++) band_centres[i] = ttab.getText(curr_row, 2 + i).toDouble();

      // get if band widths
    } else if (code == "band_widths") { 
      band_widths.resize(band_count);
      for (int i=0; i<band_count; i++) band_widths[i] = ttab.getText(curr_row, 2 + i).toDouble();
    }

      // quit if got both
    if ((int)band_centres.size() == band_count && (int)band_widths.size() == band_count) break;
 
      // try next row
    curr_row++;
  }

  if ((int)band_widths.size() == band_count) return new BandSpec(band_count, &band_centres[0], &band_widths[0]);
  return new BandSpec(band_count, &band_centres[0]);
}



// BandSpec::(constructor) [private]
// this is only used internally to BandSpec by factory functions

BandSpec::BandSpec(const JString& fp) :
  filepath_str(fp),
  name_str(""),
  code_str(""),
  band_count(0) { }



// BandSpec::(constructor)
/*! Constructs a band specification which is a contiguous subset of \a use_band_count bands from \a src_bs starting at index \a use_band_start. */ 

BandSpec::BandSpec(const BandSpec& src_bs, int use_band_start, int use_band_count) :
  filepath_str(""),
  name_str(""),
  code_str(""),
  band_count(use_band_count) {

  range r(use_band_start, use_band_start+use_band_count);
  
  band_centres = project(src_bs.band_centres, r);
  band_widths = project(src_bs.band_widths, r);
  band_lo = project(src_bs.band_lo, r);
  band_hi = project(src_bs.band_hi, r);
}



// BandSpec::writeToReport()

void BandSpec::writeToReport(Report& rep) const {

  rep.stream() << "# band specification\n";
  rep.stream() << "band_count" << rep.majorSep() << "-" << rep.majorSep() << band_count << "\n";

  rep.stream() << "band_centres" << rep.majorSep() << "-";
  for (int i=0; i<band_count; i++) rep.stream() << rep.majorSep() << band_centres(i);
  rep.stream() << "\n";

  rep.stream() << "band_widths" << rep.majorSep() << "-";
  for (int i=0; i<band_count; i++) rep.stream() << rep.majorSep() << band_widths(i);
  rep.stream() << "\n";
}



// BandSpec::(constructor)
/*! Constructs a band specification of \a n bands with band centres and widths in nm specified in the arrays \a bc \and \a bw. */

BandSpec::BandSpec(int n, const double* bc, const double *bw) :
  filepath_str(""),
  name_str(""),
  code_str(""),
  band_count(n) {
 
  if (!bc || !bw) { jlog::es << __FILE__ << ":" << __LINE__ << " null pointer supplied"; internalError(); }

  band_centres.resize(band_count);
  band_widths.resize(band_count);

  for (int i=0; i<band_count; i++) {
    band_centres(i) = *(bc++);
    band_widths(i) = *(bw++);
  }

  buildBandLoAndHi();
} 



// BandSpec::(constructor)
/*! Constructs a band specification of \a n bands with band centres and widths in nm specified in the arrays \a bc \and \a bw. */

BandSpec::BandSpec(int n, const double* bc) :
  filepath_str(""),
  name_str(""),
  code_str(""),
  band_count(n) {
 
  if (!bc) { jlog::es << __FILE__ << ":" << __LINE__ << " null pointer supplied"; internalError(); }
 
  band_centres.resize(band_count);
 
  for (int i=0; i<band_count; i++) {
    band_centres(i) = *(bc++);
  }

  buildBandWidthsFromCentres();

  buildBandLoAndHi();
} 



// BandSpec::read()

bool BandSpec::read(JDataStream& str, uns32 saved_version) {

  //is_default = false; 

  str >> filepath_str;
  str >> name_str;
  str >> code_str;

  str >> centres_fp;
  str >> widths_fp;

  str >> band_count;
 
  readFromDataStream(band_centres, str, saved_version);
  readFromDataStream(band_widths, str, saved_version);
  readFromDataStream(band_lo, str, saved_version);
  readFromDataStream(band_hi, str, saved_version);

  return true;
}



// BandSpec::write()

bool BandSpec::write(JDataStream& str) const {

  str << filepath_str;
  str << name_str;
  str << code_str;

  str << centres_fp;
  str << widths_fp;
 
  str << band_count;

  writeToDataStream(band_centres, str);
  writeToDataStream(band_widths, str);
  writeToDataStream(band_lo, str);
  writeToDataStream(band_hi, str);

  return true;
}



// BandSpec::readFromParser()
/*! Reads in a BandSpec configuration from the JParser \a prm, with value prefix \a prefix. */

bool BandSpec::readFromParser(const JParser& prm, const JString& prefix, bool& preferred_default) {

  if (jlog::vbLevel(5)) jlog::ls << "Direct band specification using prefix '" << prefix << "'.\n";

   // clear any previous data
  band_centres.resize(0);
  band_widths.resize(0);

    // try to get a base directory from the parser, i.e. the directory the parser file was in
  JString base_dir = prm.baseDir() + "/";

  if (jlog::vbLevel(5)) jlog::ls.printf("Band specification base directory '%s'.\n",base_dir.latin1());

    // only band_count must be defined
  if (!prm.isDefined(prefix+"band_count")) { 
    if (jlog::vbLevel(5)) jlog::ls << "Parameter '" << prefix << "band_count' not defined.\n"; 
    return false; 
  }
  
  band_count = prm.getInt(prefix+"band_count");

    // info
  if (jlog::vbLevel(5)) jlog::ls << "Band count: " << band_count << "\n";

    // name and code are optional
  name_str = prm.getStringOrEmpty(prefix+"bs_name");
  code_str = prm.getStringOrEmpty(prefix+"bs_code");

    // bands may be specified by filepaths, set filespths to empty if not
  centres_fp = prm.getStringOrEmpty(prefix+"band_centres_fp");
  widths_fp = prm.getStringOrEmpty(prefix+"band_widths_fp");
 
    // now try to get direct specification of bands from parameters
  if (prm.isDefined(prefix+"band_centres_data")) {
    copy(band_centres, prm.getVectorOfDoubles(prefix+"band_centres_data"));
    if (jlog::vbLevel(5)) jlog::ls << "Band centres read directly: (nm) " << band_centres << "\n";
  }
  if (prm.isDefined(prefix+"band_widths_data")) {
    copy(band_widths, prm.getVectorOfDoubles(prefix+"band_widths_data"));
    if (jlog::vbLevel(5)) jlog::ls << "Band widths read directly: (nm) " << band_widths << "\n";
  }

    // the specification can specify if it is the preferred default in a gui context
    // this may be ignored if another band spec or the tool sets a different default
  preferred_default = (prm.isDefinedAndTrueBool(prefix+"preferred_default"));

    // quit with error if something was not defined which needs to be
  //if (prm.somethingWasNotDefined()) return false;

    // if band centres specified in a file try to read
    // so note that file specification overrides dirrect specification
  if (!centres_fp.isEmpty()) {
    if (centres_fp.left(1) != "/") centres_fp.prepend(base_dir);
    if (jlog::vbLevel(5)) jlog::ls << "Reading band centres from: '" << centres_fp << "'\n";
    if (!readFromTextFile(band_centres, centres_fp)) return false;
  }
 
    // if band widths specified in a file try to read
    // so note that file specification overrides dirrect specification
  if (!widths_fp.isEmpty()) {
    if (widths_fp.left(1) != "/") widths_fp.prepend(base_dir);
    if (jlog::vbLevel(5)) jlog::ls << "Reading band widths from: '" << centres_fp << "'\n";
    if (!readFromTextFile(band_widths, widths_fp)) return false;
  }
  
    // if centres still not defined then no wavelength calculations can be done
  if (band_centres.size() == 0) return true;

    // check got right number of band centres
  if ((int)band_centres.size() != band_count) {
    jlog::es << "Read " << (int)band_centres.size() << " band centres for band specification (prefix '" << prefix <<"'), should be " << band_count << ".\n"; 
    return false;
  }
    // check got right number of band widths if got any
  if (band_widths.size() != 0 && (int)band_widths.size() != band_count) {
    jlog::es << "Read " << (int)band_centres.size() << " band widths for band specification (prefix '" << prefix <<"'), should be " << band_count << ".\n"; 
    return false;
  }

    // might still not have the widths specified so have to infer from centres
  if (band_widths.size() == 0) buildBandWidthsFromCentres();

    // can now calculated upper and lower limits
  buildBandLoAndHi();

  return true;
}



// BandSpec::buildBandWidthsFromCentres() [private]

void BandSpec::buildBandWidthsFromCentres() {

    // set the size of the vector
  band_widths.resize(band_count);
    // if there is only one band can't guess the widths so set to 1 nm
  if (band_count==1) band_widths(0) = 1;
  else {
      // make the end bands symmetrical about the centre
    band_widths(0) = (band_centres(1) - band_centres(0));
    band_widths(band_count-1) = (band_centres(band_count-1) - band_centres(band_count-2));
      // make all other band boundaries the mid-point between adjacent bands
      // if the band centres are irregular then this may lead to overlapping band box functions
    for (int i=1; i<band_count-1; i++) {
      band_widths(i) = (band_centres(i+1) - band_centres(i-1)) / 2;
    }
  }
}



// BandSpec::buildBandLoAndHi() [private]

void BandSpec::buildBandLoAndHi() {

  band_lo.resize(band_count);
  band_hi.resize(band_count);

  for (int i=0; i<band_count; i++) {
    double w = band_widths(i) / 2;
    band_lo(i) = band_centres(i) - w;
    band_hi(i) = band_centres(i) + w;
  }
}



// BandSpec::addToParser()

void BandSpec::addToParser(JParser& prm, const JString& prefix) const {

    // if was created from a file then just pass the file
    // since this is used when a gui front-end invokes a tool it will always be able to find the same file
  if (!filepath_str.isEmpty()) { prm.setLabelAndValue(prefix+"band_spec_fp", filepath_str); return; }

    // number of bands is always defined
  prm.setLabelAndValue(prefix+"band_count", band_count);
    // these may or may not be defined
  if (!name_str.isEmpty()) prm.setLabelAndValue(prefix+"bs_name", name_str);
  if (!code_str.isEmpty()) prm.setLabelAndValue(prefix+"bs_code", code_str);

  std::vector<double> std_v;

    // file path for band centres may also be set or not
  if (!centres_fp.isEmpty()) prm.setLabelAndValue(prefix+"band_centres_fp", centres_fp);
    // if it was not set then assume direct specification of band centres in nm
  else if (band_centres.size() > 0) {
    copy(std_v, band_centres);
    prm.setLabelAndValue(prefix+"band_centres_data", std_v);
  }

    // file path for band widths may also be set or not
  if (!widths_fp.isEmpty()) prm.setLabelAndValue(prefix+"band_widths_fp", widths_fp);
    // if it was not set then assume direct specification of band widths in nm
    // in reality the widths could have been inferred from centres but this will make no difference
  else if (band_widths.size() > 0) {
    copy(std_v, band_widths);
    prm.setLabelAndValue(prefix+"band_widths_data", std_v);
  }

  // note that preferred_default is a meta-level parameter not stored in the BandSpec
  // hence is not written here
}



// BandSpec::closestBand()

int BandSpec::closestBand(double lambda_nm) const {

  double diff = DOUBLE_MAX;
  int closest_band = 0;

  for (int i=0; i<band_count; i++) {
    double d = fabs(band_centres(i) - lambda_nm);
    if (d < diff) { diff = d; closest_band = i; }
  }

  return closest_band;
}



// BandSpec::EtoUmol()
// converts energy in watts to umol quanta per sec

vector<double> BandSpec::EtoUmol(const vector<double>& energy_watts) const {
    // check
  if (!hasWavelengthData()) { jlog::es << "BandSpec::EtoUmol() called but no wavelength data supplied.\n"; internalError(); }
    // umol_conv is lazy calculated
  if (umol_conv.size()==0) calcUmolConv();
    // multiply watts in each band
  return element_prod(energy_watts, umol_conv);
}



// BandSpec::EtoPAR()
// converts energy in watts to umol quanta per sec
// and then sums over the range 400 to 700 nm
// only sums across bands that are actually there

double BandSpec::EtoPAR(const vector<double>& energy_watts) const {
 
    // check
  if (!hasWavelengthData()) { jlog::es << "BandSpec::EtoPAR() called but no wavelength data available.\n"; internalError(); }
 
    // umol_conv is lazy calculated
  if (umol_conv.size()==0) calcUmolConv();

    // multiply watts in each band
  vector<double> u(element_prod(energy_watts, umol_conv));

  double par_sum = 0;
  //double w;

  for (int i=0; i<band_count; i++) {
    if (band_centres(i) > 400 && band_centres(i) < 700) par_sum += u(i) * band_widths(i);
  }

  return par_sum;
}



// BandSpec::integrateOverWavelength()

double BandSpec::integrateOverWavelength(const SBandData& power_per_waveln) const {

    // check
  if (!hasWavelengthData()) { jlog::es << "BandSpec::integrateOverWavelength() called but no wavelength data available.\n"; internalError(); }

  double power = 0;
  for (int i=0; i<band_count; i++) {
    power += power_per_waveln.vectorData()(i) * band_widths(i);
  }
    // this is in power_units_str
  return power;
}



// BandSpec::calcUmolConv() [private]
/*! \todo Could be more accurate */

void BandSpec::calcUmolConv() const { 

    // create vector
  umol_conv.resize(band_count);
    // do each band
  for (int i=0; i<band_count; i++) {
      // the band centre wavelength in metres
    double lambda_m = (band_centres(i) / 1E9);
      // energy of a photon at the band centre wavelength in Watts - q = hc/lambda
    double quanta_energy = JConstants::hc() / lambda_m;
      // the number of photons per sec that would give 1 Watt in this band 
    double quanta_per_sec = 1.0 / quanta_energy;
      // the number of photons in umol
    umol_conv(i) = (quanta_per_sec * 1E6) / JConstants::avagadrosConstant();
  }
}



// BandSpec::calcTristimFuncs() [private]
// Lazy evaluation of tristimulus functions for this band specification.
// The conversion of radiance to luminance in the red, green and blue follows
// Mobley L&W eqn. 2.10 (pg. 49)

void BandSpec::calcTristimFuncs() const {

  vector<double> all_x;
  vector<double> red_y;
  vector<double> green_y;
  vector<double> blue_y;

  copy(all_x, TRISTIM_ELEMS, tri_wave);
  copy(red_y, TRISTIM_ELEMS, tri_red);
  copy(green_y, TRISTIM_ELEMS, tri_green);
  copy(blue_y, TRISTIM_ELEMS, tri_blue);

  if (hasWavelengthData()) {

    tristim_red = Ipol::linearInterpolate(band_centres, red_y, all_x);
    tristim_green = Ipol::linearInterpolate(band_centres, green_y, all_x);
    tristim_blue = Ipol::linearInterpolate(band_centres, blue_y, all_x);

      // incorporate K_m and band widths so the integral for spectral vector data can be
      // be found by a dot product
    for (int i=0; i<band_count; i++) {
      double m = 683 * band_widths(i);
      tristim_red(i) *= m; 
      tristim_green(i) *= m; 
      tristim_blue(i) *= m; 
    }

    // if no wavelength data then set as equal for monochrome images
  } else {
    tristim_red = scalar_vector<double>(band_count, 6830.0/TRISTIM_ELEMS);
    tristim_green = scalar_vector<double>(band_count, 6830.0/TRISTIM_ELEMS);
    tristim_blue = scalar_vector<double>(band_count, 6830.0/TRISTIM_ELEMS);
  }

  if (jlog::vbLevel(5)) {
    jlog::ls << "Tristimulus functions:\n";
    jlog::ls << "red:   " << tristim_red << "\n";
    jlog::ls << "green: " << tristim_green << "\n";
    jlog::ls << "blue:  " << tristim_blue << "\n";
  }
}



// BandSpec::redLumens()

double BandSpec::redLumens(const SBandData& sd) const {
  if (tristim_red.size()==0) calcTristimFuncs();
  return dotProduct(sd.vectorData(), tristim_red);
}



// BandSpec::greenLumens()

double BandSpec::greenLumens(const SBandData& sd) const {
  if (tristim_red.size()==0) calcTristimFuncs();
  return dotProduct(sd.vectorData(), tristim_green);
}



// BandSpec::blueLumens()

double BandSpec::blueLumens(const SBandData& sd) const {
  if (tristim_red.size()==0) calcTristimFuncs();
  return dotProduct(sd.vectorData(), tristim_blue);
}



// BandSpec::checkBandCount()

bool BandSpec::checkBandCount(const JString& text, const SBandData& sd) const {

  if (sd.bandCount() == bandCount()) return true;

  jlog::es << "Number of bands of " << text << " (" << sd.bandCount() << ") does not ";
  jlog::es << "match band specification (" << bandCount() << ").\n";

  return false;     
}



// BandSpec::helpText()

void BandSpec::helpText(JTextStream& str) {
  str << "<prefix>bs_name           = <string>  (name of the band specification scheme)\n";
  str << "<prefix>bs_code           = <string>  (short code to use to id band specification scheme)\n";
  str << "<prefix>band_count        = <integer>  (number of bands)\n";
  str << "<prefix>band_centres_data = <r0,r1,r2, ... rn>  (direct specification of band centres in nm)\n";
  str << "<prefix>band_widths_data  = <r0,r1,r2, ... rn>  (direct specification of band widths in nm)\n";
  str << "<prefix>band_centres_fp   = <filepath>  (path of file specifying band centres in nm)\n";
  str << "<prefix>band_widths_fp    = <filepath>  (path of file specifying band widths in nm)\n";
  str << "<prefix>preferred_default = yes | no  (request this be the default band specification in a gui context)\n";
}



// BandSpec::display()

void BandSpec::display(JTextStream& str) const {
  str << "\"" << name_str << "\" code: " << code_str << " bands: " << band_count << "\n";
}






// BandSpec::readVectorFile()
/*
bool BandSpec::readVectorFile(Vector& v, const JString& filepath) {

  v.makeZeros(band_count);

    // open the file
  JFile f(filepath.latin1());
  if (!f.open(IO_ReadOnly)) {
    es << "File: '" << filepath << "' not found\n"; 
    return false; 
  }

    // get a data stream on the file
  JTextStream str(&f);
 
  for (int i=0; i<band_count; i++) {
    JString line = FileUtils::readTextLine(str).stripWhiteSpace();
    v.put(i, line.toDouble());
  }
 
    // close file
  f.close();

  return true;
}
*/


// Vector::writeToTextFile()
/*
bool BandSpec::writeVectorFile(const Vector& v, const JString& filepath) {

    // open the file
  JFile f(filepath.latin1());
  if (!f.open(IO_WriteOnly)) {
    es << "File: '" << filepath << "' not writable\n"; 
    return false;
  }

    // get a data stream on the file
  JTextStream str(&f);
  str.setScientific(8);

    // write data  
  for (int i=0; i<band_count; i++) str << v.get(i) << "\n";

    // close file
  f.close();

  return true;
}
*/

