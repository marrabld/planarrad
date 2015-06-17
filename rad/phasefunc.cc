
#include "rad/phasefunc.h"
#include "base/jerror.h"
#include "base/jfile.h"
#include "base/jtexttable.h"
#include "file/fileutils.h"
#include "rad/sbanddata.h"
#include "math/ipol.h"
#include "math/rand.h"
#include "file/fileutils.h"

using namespace jude::rad;


// petzold phase function - mutiply by solid angle to give angular probability
// data is <angle>, <prob/solid angle>
// probability is considered to be the linear interpolation between the value points
// the solid angle at a value point is the range from half way to the the previous point and half way to the next point
// from Mobley L&W Table 3.10 pg. 111

static const int PETZOLD_ELEMS = 56;
static const double petzold_data[2*PETZOLD_ELEMS] = {

  0,     1767, // this is one is made up
  0.100, 1767,
  0.126, 1296, 
  0.158, 950.2,
  0.200, 699.1,
  0.251, 514.0,
  0.316, 376.4,
  0.398, 276.3,
  0.501, 218.8,
  0.631, 144.4,
  0.794, 102.2, 

  1.000, 71.61,
  1.259, 49.58,
  1.585, 33.95,
  1.995, 22.81,
  2.512, 15.16,
  3.162, 10.02,
  3.981, 6.58,
  5.012, 4.295,   
  6.310, 2.807,
  7.943, 1.819,
 
  10.0,  1.153,
  15.0,  0.4893,
  20.0,  0.2444,
  25.0,  0.1472,
  30.0,  0.08609,
  35.0,  0.05931,
  40.0,  0.04210,
  45.0,  0.03067,
  50.0,  0.02275,
  55.0,  0.01699,

  60.0,  0.01313,
  65.0,  0.01046,
  70.0,  0.008488,
  75.0,  0.006976,
  80.0,  0.005842,
  85.0,  0.004953,
  90.0,  0.004292,
  95.0,  0.003782,
  100.0, 0.003404,
  105.0, 0.003116,

  110.0, 0.002912,
  115.0, 0.002797,
  120.0, 0.002686,
  125.0, 0.002571,
  130.0, 0.002476,
  135.0, 0.002377,
  140.0, 0.002329,
  145.0, 0.002313,
  150.0, 0.002365,
  155.0, 0.002506,

  160.0, 0.002662,
  165.0, 0.002835,
  170.0, 0.003031,
  175.0, 0.003092,
  180.0, 0.003154 };



// example of the kind of file data that might be supplied
// retain as can be used for testing interpolation

static const double test_data[2*7] = {
  0, 0, 
  1, 0,  // prob over interval from prev to here i.e. 0 to 1
  1.5, 20,
  2, 20, // 1 to 2 
  3, 40,
  4, 10,
  5, 30 };



/*! Constructs a phase function based on creation mode \a mode_str and optionally a set of files \a fs and band specification \a bs. 
Potential values for \a mode_str are:
RANDOM 
SINGLE - a single phase function applies for all bands or wavelengths
BAND
IPOL

\todo This really ought to be refactored as a series of derived classes.
*/

PhaseFunc::PhaseFunc(const JString& mode_str, const FileSetDesc& fs, const std::vector<double>& wl, const BandSpec* bs) :
  bs(bs),
  value_sel_ipol(0),
  value_reg_ipol(0),
  reg_size(18001),
  init_ok(false) {

  if (mode_str == "RANDOM") mode = RANDOM_MODE;
  else if (mode_str == "SINGLE") mode = SINGLE_MODE;
  else if (mode_str == "BAND") mode = BAND_MODE;
  else if (mode_str == "IPOL") mode = IPOL_MODE;
  //else if (mode_str == "WAVE") mode = WAVE_MODE;
  else { 
    jlog::es << "PhaseFunc::(constructor) bad mode '" << mode_str << "'.\n";
    internalError();
  }

  //if ((mode==WAVE_MODE || mode==IPOL_MODE || mode==BAND_MODE) && !bs) {
  //  jlog::ls << "PhaseFunc::(constructor) mode is WAVE, IPOL or BAND but no band specification supplied.";
  //  internalError();
  // }

  if (!bs) {
    jlog::ls << "PhaseFunc::(constructor) no band specification supplied.";
    internalError();
  }

    // length of lookup vector
  size = 1000;

    // number of files or psuedo-files specified
  file_count = fs.fileCount();

  if (mode==RANDOM_MODE && file_count!=0) jlog::es << "PhaseFunc::(constructor) - creation mode is RANDOM but number of files != 0\n";
  if (mode==SINGLE_MODE && file_count!=1) jlog::es << "PhaseFunc::(constructor) - creation mode is SINGLE but number of files != 1\n";
  if (mode==BAND_MODE && file_count!=bs->bandCount()) jlog::es << "PhaseFunc::(constructor) - creation mode is BAND but not correct number of files\n";

  value_pdf = new vector<double>[file_count];
  value_sel = new vector<double>[file_count];
  value_reg = new vector<double>[file_count];
  wave_len = new double[file_count];

    // default is zero
  for (int i=0; i<file_count; i++) wave_len[i] = 0;
    // copy in any specified wavelengths
  for (unsigned int i=0; i<wl.size(); i++) wave_len[i] = wl[i];

  vector<double> angle;
  vector<double> value;

    // loop through specified files
  for (int i=0;i<file_count;i++) {

      // isotropic psuedo-file
    if (fs.file(i)->path() == JString("__ISOTROPIC__")) {
  
      wave_len[i] = 0;

      angle.resize(181);
      value.resize(181);
     
        // set up angle vector as 0 to 180 and value as constant probabilty
      for (unsigned int j=0; j<angle.size(); j++) { 
	  // steps of one degreee
	angle[j] = j;
  	  // the phase function value is constant, units (sr-1) and integrates to unity over solid angle
	value[j] = 1.0 / (4*M_PI);
      }

      // petzolds phase function psuedo-file
    } else if (fs.file(i)->path() == JString("__PETZOLD__")) {

      copy(angle, PETZOLD_ELEMS, petzold_data, 2);
      copy(value, PETZOLD_ELEMS, petzold_data+1, 2);

      // fournier-forand phase function psuedo-file, format is __FOURNIER__0.020__ (number can be any length)
    } else if (fs.file(i)->path().left(12) == JString("__FOURNIER__")) {

      if (jlog::vbLevel(6)) jlog::ls << "code: " << fs.file(i)->path() << "\n";

        // strip out backscatter fraction
      JString s(fs.file(i)->path());
      s = s.right(s.length()-12);
      s = s.left(s.length()-2);
      double Bfrac = s.toDouble();

        // this can fail so need to quit early
      if (!buildFournierForand(Bfrac, angle, value)) return;

      // pure sea water phase function psuedo-file
    } else if (fs.file(i)->path() == JString("__PUREWATER__")) {

      buildPureWater(angle, value);

      // or read data from text file
      // or build fornier-fourand from Bp values  
    } else {

      readFromTextFile(i, angle, value, *fs.file(i));
    }

    build(i, angle, value);
    buildAngleLookup(value_reg[i], angle, value);

    if (jlog::vbLevel(6)) {
      jlog::ls << "File no. " << (i+1) << " : regularised probability of scatter with angle (value_reg):\n";
      jlog::ls << value_reg[i] << "\n";
    }
  }

    // ipol mode assumes each file had a wavelength associated
  if (mode==IPOL_MODE) {

    value_sel_ipol = new vector<double>[bs->bandCount()];
    value_reg_ipol = new vector<double>[bs->bandCount()];

    for (int i=0;i<bs->bandCount();i++) {

        // the wavelength to build
      double w = bs->bandCentres()[i];

        // if wavelength is lower than first phase function then use first
      if (wave_len[0] >= w) {

	value_sel_ipol[i] = value_sel[0]; 
	value_reg_ipol[i] = value_reg[0]; 

        // if wavelength is larger than last phase function then use last
      } else if (wave_len[file_count-1] <= w) {

	value_sel_ipol[i] = value_sel[file_count-1];
	value_reg_ipol[i] = value_reg[file_count-1];

      } else {

          // loop until gone past lower wavelength
	int j;
	for (j=1;j<file_count-1 && wave_len[j] < w;j++);
 
          // if is exactly this wavelength then use it
	if (wave_len[j] == w) {
	  value_sel_ipol[i] = value_sel[j]; 
	  value_reg_ipol[i] = value_reg[j]; 

	} else {
            // the fraction to interpolate by
	  double frac = (w - wave_len[j-1]) / (wave_len[j] - wave_len[j-1]);
            // build interpolated functions
	  value_sel_ipol[i] = value_sel[j-1] * (1-frac) + value_sel[j] * frac;
	  value_reg_ipol[i] = value_reg[j-1] * (1-frac) + value_reg[j] * frac;
	}
	//printf("i = %d w = %f %f %f\n",i, w, frac, wave_len[0]);
	//value_sel_ipol[i].display();
      }
    }
  }

  init_ok = true;
}



/*! Destructor. */

PhaseFunc::~PhaseFunc() {
  delete[] value_pdf;
  delete[] value_sel;
  delete[] value_reg;
  delete[] wave_len;
  delete[] value_sel_ipol;
  delete[] value_reg_ipol;
}



// PhaseFunc::build() [private]
// integrates the phase function stored as angular positions (angle) and prob/sa (value) over solid angle
// and puts the result in the value_pdf vector for (band)

void PhaseFunc::build(int band, vector<double>& angle, vector<double>& value) {

    // multiply each phase function entry by the solid angle of the sphere band that it represents
    // the resulting values should sum to 1 and represent the angular probability distribution (I think)
  double sum = 0;
  double back_sum = 0;

  value_pdf[band] = zero_vector<double>(angle.size());

  for (unsigned int i=0;i<angle.size();i++) {

      // the angle boundaries for this point
    double lo_ang_deg = (i==0) ? 0 : 0.5 * (angle[i-1] + angle[i]);
    double hi_ang_deg = (i==angle.size()-1) ? 180 : 0.5 * (angle[i] + angle[i+1]);
      // the cosine of the angle boundaries for this point
    double lo_cos_ang = cos((M_PI*lo_ang_deg) / 180);
    double hi_cos_ang = cos((M_PI*hi_ang_deg) / 180);
      // solid angle associated with this point
    double sa = 2*M_PI*(lo_cos_ang - hi_cos_ang);
      // integrate the angular scattering pdf as the phase function x solid angle
    value_pdf[band][i] = value[i] * sa;
      // keep track of sum
    sum += value_pdf[band][i];
      // back scatter sum
    if (angle[i] == 90) back_sum += value_pdf[band][i] * 0.5;
    if (angle[i] > 90) back_sum += value_pdf[band][i];
  }

  if (jlog::vbLevel(3)) {
    jlog::ls.printf("Phase function sum = %f\n",sum);
    jlog::ls.printf("Phase function back_sum = %f\n",back_sum);
  }
  if (jlog::vbLevel(2)) jlog::ls.printf("Phase function backscatter ratio bb/b = %f\n",back_sum/sum);

    // get the running sum of the pdf 
  vector<double> value_sum = value_pdf[band];
  for (unsigned int i=1; i<value_sum.size(); i++) value_sum(i) += value_sum(i-1);
    // the final (highest) value should be 1 anyway, same as (sum) above, but normalise just to be sure
  value_sum /= value_sum(value_sum.size()-1);

    // we need to get a function so that we can look up an angle from a probability
    // this means we have to flip the function about the xy-line 

    // build the probability centre points 0-1 range at the pre-defined reolution
  vector<double> y_range(size);
  for (int i=0;i<size;i++) y_range(i) = (((double)i)+0.5)/size;

    // get the look-up values at the probability centres
    // note that isotropic scallterring does not mean all value_sel values will be equal - they will be lower in the forward and back directions
  value_sel[band] = Ipol::linearInterpolate(y_range, angle, value_sum);

  //if (jlog::vbLevel(5)) jlog::ls << "(value_sel):\n" << value_sel[band] << "\n";
}



// PhaseFunc::buildAngleLookup()
// (src_angle) may be at irregular intervals this builds a phase function vector
// where each element is at regular angle intervals define by the number of elements
// in (targ_value) minus 1 (reg_size, e.g. 1800 = 0.1 deg - vector is 1801 elements and includes both 0 and 180)
// values are linearly interpolated, although log-based interpolation may be more appropriate
// note for phase functions which are originally expressed this way (e.g. at 1801 regular points)
// this function produces an indentical copy of the original data
// note that the total sum does not add to 1 as it should because the forward scattering is hard to numerically
// integrate. However this gets fixed when constructing a tabulated phase function in PhaseTool since any slop gets
// put into the forward scattering quad anyway. Backscatter sums are fine.

void PhaseFunc::buildAngleLookup(vector<double>& targ_value, const vector<double>& src_angle, const vector<double>& src_value) {
  
  double sum = 0;
  double back_sum = 0;

  targ_value = zero_vector<double>(reg_size+1);

  for (int i=0;i<=reg_size;i++) {

      // centre angle at which value is required
    double ang_deg = (i*180.0) / reg_size;
    double ang_rad = (i*M_PI) / reg_size;
      // get the value
    targ_value(i) = Ipol::linearInterpolate(ang_deg, src_value, src_angle); 
      // the cosine of the angle boundaries for this point
    double lo_cos_ang = (i==0) ? 1 : cos((M_PI*(i-0.5)) / reg_size);
    double hi_cos_ang = (i==reg_size) ? -1 : cos((M_PI*(i+0.5)) / reg_size);
      // solid angle associated with this point
    double sa = 2*M_PI*(lo_cos_ang - hi_cos_ang);
      // keep track of sum and backscatter sum
    double integral = targ_value[i] * sa;
    sum += integral;
    if (ang_rad == M_PI/2) back_sum += integral * 0.5;
    if (ang_rad > M_PI/2) back_sum += integral;
  } 

    // these should be very close to the original sum and back_sum
    // and identical if the original phase function was also expressed at reg_size+1 regular points  
  if (jlog::vbLevel(3)) {
    jlog::ls.printf("Phase function regularised sum %f\n",sum);
    jlog::ls.printf("Phase function regularised back_sum %f\n",back_sum);
    jlog::ls.printf("Phase function regularised backscatter ratio bb/b = %f\n",back_sum/sum);
  }
}



// PhaseFunc::scatterDirec()
// note we already checked that (bs) was not null for the required modes

Vec3D PhaseFunc::scatterDirec(int band, const Vec3D& dv) const {

  switch (mode) {
    case RANDOM_MODE : return Vec3D::sphereRand();
    case SINGLE_MODE : return scatterDirecFromAngle(scatterAngleFromSingle(), dv);
    case BAND_MODE : return scatterDirecFromAngle(scatterAngleFromBand(band), dv);
    case IPOL_MODE : return scatterDirecFromAngle(scatterAngleFromIpol(band), dv);
    default : return scatterDirecFromAngle(scatterAngleFromWavelen(bs->bandCentres()[band]), dv);
  }
}



// PhaseFunc::scatterAngleFromSingle() [private]
// in single mode we assume there is a single phase function for all bands

double PhaseFunc::scatterAngleFromSingle() const {
  return value_sel[0]((int)uniform_dbn(0,size));
}



// PhaseFunc::scatterAngleFromBand() [private]
// return a scattering angle based on current direction (dv) and band (band)
// in band mode we assume there is a set of phase functions one for each band

double PhaseFunc::scatterAngleFromBand(int band) const {
  return value_sel[band]((int)uniform_dbn(0,size));
}



// PhaseFunc::scatterAngleFromBand() [private]
// return a scattering angle based on current direction (dv) and band (band)
// ipol mode is the same as band mode but used when the band functions had to be built from interpolating the input files

double PhaseFunc::scatterAngleFromIpol(int band) const {
  return value_sel_ipol[band]((int)uniform_dbn(0,size));
}



// PhaseFunc::scatterDirecFromWavelen() [private]
// return a scattering direction vector baded on current direction (dv) and wavelength (w)
// in wave mode we interpolate the scattering angle from the wavelength based on the set of phase functions

double PhaseFunc::scatterAngleFromWavelen(double w) const {

  //jlog::ls.printf(" w = %f %f %f\n",w,wave_len[0],wave_len[file_count-1]);

    // if wavelength is lower than first phase function then use first
  if (wave_len[0] >= w) return value_sel[0][(int)uniform_dbn(0,size)]; 
    // if wavelength is larger than last phase function then use last
  if (wave_len[file_count-1] <= w) return value_sel[file_count-1][(int)uniform_dbn(0,size)]; 

    // loop until gone past lower wavelength
  int i;
  for (i=1;i<file_count-1 && wave_len[i] < w;i++); 
    // if is exactly this wavelength then use it
  if (wave_len[i] == w) return value_sel[i][(int)uniform_dbn(0,size)]; 
 
    // the fraction to interpolate by
  double frac = (w - wave_len[i-1]) / (wave_len[i] - wave_len[i-1]);

    // get angle from each function
  int r = (int) uniform_dbn(0,size);
  double angle0 = value_sel[i-1](r);
  double angle1 = value_sel[i](r);

  jlog::ls.printf(" %f %f ang0 = %f ang1 = %f frac = %f\n",wave_len[i-1],wave_len[i],angle0,angle1,frac);

    // return scattered direction based on interpolated angle
  return angle1*frac + angle0*(1-frac);
}



// PhaseFunc::valueFromAngle()

double PhaseFunc::valueFromAngle(int band, double ang_rad) const {
 
  switch (mode) {
    case RANDOM_MODE : return 1.0 / (4*M_PI);
    case SINGLE_MODE : return valueFromAngleSingle(ang_rad);
    case BAND_MODE : return valueFromAngleBand(band, ang_rad);
    case IPOL_MODE : return valueFromAngleIpol(band, ang_rad);
    default : return valueFromAngleWavelen(scatterAngleFromWavelen(bs->bandCentres()[band]), ang_rad);
  }
}



// PhaseFunc::scatterAngleFromSingle() [private]
// in single mode we assume there is a single phase function for all bands

double PhaseFunc::valueFromAngleSingle(double ang_rad) const {
  return value_reg[0]((int)(0.5 + (ang_rad * reg_size)/M_PI));
}



// PhaseFunc::scatterAngleFromBand() [private]

double PhaseFunc::valueFromAngleBand(int band, double ang_rad) const {
  return value_reg[band]((int)(0.5 + (ang_rad * reg_size)/M_PI));
}



// PhaseFunc::scatterAngleFromBand() [private]

double PhaseFunc::valueFromAngleIpol(int band, double ang_rad) const {
  return value_reg_ipol[band]((int)(0.5 + (ang_rad * reg_size)/M_PI));
}



// PhaseFunc::valueFromAngleWavelen() [private]

double PhaseFunc::valueFromAngleWavelen(double w, double ang_rad) const {

  int index = (int) (0.5 + (ang_rad * reg_size)/M_PI);

    // if wavelength is lower than first phase function then use first
  if (wave_len[0] >= w) return value_reg[0][index]; 
    // if wavelength is larger than last phase function then use last
  if (wave_len[file_count-1] <= w) return value_reg[file_count-1][index]; 

    // loop until gone past lower wavelength
  int i;
  for (i=1;i<file_count-1 && wave_len[i] < w;i++); 
    // if is exactly this wavelength then use it
  if (wave_len[i] == w) return value_reg[i][index]; 
 
    // the fraction to interpolate by
  double frac = (w - wave_len[i-1]) / (wave_len[i] - wave_len[i-1]);

    // return scattered direction based on interpolated angle
  return value_reg[i][index]*frac + value_reg[i-1][index]*(1-frac);
}



// PhaseFunc::spectralValuesFromAngle()
// this could be an overloaded function supplied in derived classes

SBandData PhaseFunc::spectralValuesFromAngle(double ang_rad) const {

  switch (mode) {
    case RANDOM_MODE : return SBandData(bs->bandCount(), 1.0/(4*M_PI));
    case SINGLE_MODE : return spectraFromAngleSingle(ang_rad);
    case BAND_MODE : return spectraFromAngleBand(ang_rad);
    case IPOL_MODE : return spectraFromAngleIpol(ang_rad);
    default : return spectraFromAngleWavelen(ang_rad);
  }
}



// PhaseFunc::scatterAngleFromSingle() [private]
// in single mode we assume there is a single phase function for all bands

SBandData PhaseFunc::spectraFromAngleSingle(double ang_rad) const {
  return SBandData(bs->bandCount(), value_reg[0]((int)(0.5 + (ang_rad * reg_size)/M_PI)));
}



// PhaseFunc::scatterAngleFromBand() [private]

SBandData PhaseFunc::spectraFromAngleBand(double ang_rad) const {
  SBandData sd(bs->bandCount(), 0);
  int index = (int) (0.5 + (ang_rad * reg_size)/M_PI);
  for (int i=0;i<bs->bandCount();i++) sd.bandValue(i) = value_reg[i][index];
  return sd;
}



// PhaseFunc::spectraFromAngleIpol() [private]

SBandData PhaseFunc::spectraFromAngleIpol(double ang_rad) const {
  SBandData sd(bs->bandCount(), 0);
  int index = (int) (0.5 + (ang_rad * reg_size)/M_PI);
  for (int i=0;i<bs->bandCount();i++) sd.bandValue(i) = value_reg_ipol[i][index];
  return sd;
}



// PhaseFunc::spectraFromAngleWavelen() [private]

SBandData PhaseFunc::spectraFromAngleWavelen(double ang_rad) const {
  SBandData sd(bs->bandCount(), 0);
  for (int i=0;i<bs->bandCount();i++) sd.bandValue(i) = valueFromAngleWavelen(bs->bandCentres()(i), ang_rad);
  return sd;
}



// PhaseFunc::scatterDirecFromAngle() [private]
// return a scattering direction vector based on the scattering angle (angle) from direction vector (dv)

Vec3D PhaseFunc::scatterDirecFromAngle(double angle, const Vec3D& dv) const {

  Vec3D ra0(dv);
  ra0.setUnitLength(); 
  Vec3D ra1(ra0.unitVectorAtRightAngles());
  Vec3D ra2(Vec3D::crossProduct(ra0,ra1));
  ra2.setUnitLength(); 

  double ang = uniform_dbn(0, 2*M_PI);
  Vec3D n(ra1);
  n.rotateInPlane(ang, ra1, ra2);

  Vec3D sv(ra0);
  sv.rotateInPlane((2*M_PI*angle)/360, ra0, n);

  //double check = Vector3D::dotProduct(ra0,sv);
  //printf("check %f = %f\n",cos((2*M_PI*angle)/360),check); 
  //dv.display();
  //sv.display();
  return sv;
}



// PhaseFunc::readFromTextFile()

bool PhaseFunc::readFromTextFile(int file_num, vector<double>& angle, vector<double>& value, const FileDesc& fd) {

  JTextTable ttab;
  if (!ttab.readFromFile(fd.path())) {
    jlog::es << "PhaseFunc::readFromTextFile() file '" << fd.path().latin1() << "' not found\n"; 
    return false; 
  }

    // number of entries
  int n = ttab.rowCount();

  angle = zero_vector<double>(n);
  value = zero_vector<double>(n);

  for (int i=0; i<n; i++) {
    angle(i) = ttab.getText(i,0).toDouble();
    value(i) = ttab.getText(i,1).toDouble();
  }

  if (jlog::vbLevel(6)) {
    jlog::ls << "input angles:\n" << angle << "\n";
    jlog::ls << "input values:\n" << value << "\n";
  }

  return true;
}



// PhaseFunc::readFournierForand()
// untested and unused
/*
bool PhaseFunc::readFournierForand(int file_num, vector<double>& angle, vector<double>& value, const FileDesc& fd) {

    // open the file
  JFile f(fd.path().latin1());
  if (!f.open(JIO_ReadOnly)) { 
    jlog::es.printf("PhaseFunc::buildFournierForand() file '%s' not found\n", fd.path().latin1()); 
    return false; 
  }

    // get a text stream on the file
  JTextStream str(&f);

  JString line;

    // read in first uncommented and not empty text line
  FileUtils::readTextLine(line, str);
    // first line is wavelength
  wave_len[file_num] = line.toDouble();

    // read in next uncommented and not empty text line
  FileUtils::readTextLine(line, str);
    // next line is backscatter fraction
  double Bfrac = line.toDouble();

    // close the file
  f.close();

  buildFournierForand(Bfrac, angle, value);

  return true;
}
*/



//  PhaseFunc::buildFournierForand()

bool PhaseFunc::buildFournierForand(double Bp, vector<double>& angle, vector<double>& value) {
 
  if (jlog::vbLevel(3)) jlog::ls << "Fournier-Forand Bp = " << Bp << "\n";

  // frist we solve numerically for mu from Bp

  int Bp_iters = 500;
  double Bp_acc = 1e-15;

  double mu = 4;
  double mu_step = 2;

  int i;
  double Bp_est;

  for (i=0; i<Bp_iters; i++) {

    Bp_est = calcBp(mu);
    //printf("Bp_est %f mu %f\n",Bp_est,mu);
    
    if (Bp_est > Bp+Bp_acc) mu -= mu_step;
    else if (Bp_est < Bp-Bp_acc) mu += mu_step;
    else break;

    mu_step /= 1.5;
  }

  if (i == Bp_iters) { jlog::es << "mu solution failed\n"; return false; }

  double n = 1.01 + 0.1542 * (mu - 3);
  
  if (jlog::vbLevel(3)) {
    jlog::ls.printf("mu = %f Bp_est = %f (iters %d)\n",mu,Bp_est,i);
    jlog::ls.printf("n = %f\n",n);
  }

  int angle_points = reg_size+1;

  angle = vector<double>(angle_points);
  value = vector<double>(angle_points);

  angle[0] = 0;
  value[0] = 0;

  for (int i=1;i<angle_points;i++) {

    double ang_rad = (M_PI*i) / (angle_points-1);
    double ang_deg = (180.0*i) / (angle_points-1);

    angle[i] = ang_deg;
    value[i] = ffValue(ang_rad, mu);
  }

  return true;
}



double PhaseFunc::ffValue(double ang_rad, double ff_mu) {

  double v = (3 - ff_mu) / 2;
  double n = 1.01 + 0.1542 * (ff_mu - 3);

  double s = sin(ang_rad / 2);
  double delta = (4 * s * s)/ (3*(n-1)*(n-1));

  double delta180 = 4 / (3*(n-1)*(n-1));
  double omd = 1 - delta;

  double f1 = 1 - pow(delta, v);
  double f2 = v * omd - f1 + (delta * f1 - v * omd) / (s*s);
  double f3 = f2 / (4*M_PI * omd * omd * pow(delta,v));
  double f4 = (1 - pow(delta180, v)) / (16 * M_PI * (delta180 - 1) * pow(delta180,v));
  double c = cos(ang_rad);
  double f5 = f4 * (3 * c * c - 1);
 
  return f3 + f5;
}



double PhaseFunc::calcBp(double mu) {

    // estimate of n from Mobley et al 2002
  double n = 1.01 + 0.1542 * (mu - 3);

  double v = (3 - mu) / 2;

  double s = sin(M_PI / 4);
  double delta90 = (4 * s * s)/ (3*(n-1)*(n-1));

  double top = 1 - pow(delta90, v+1) - 0.5 * (1 - pow(delta90,v));
  double bottom = (1-delta90) * pow(delta90,v);

  return 1 - (top / bottom);
}



// PhaseFunc::buildPureWater()
// From Mobley pg 103. eq. 3.28
// the phase function is the same for pure water or pure seawater, but the amount 
// of scaterring is greater in sea water dues to CL- Na+ ions etc.

void PhaseFunc::buildPureWater(vector<double>& angle, vector<double>& value) {

  int angle_points = reg_size+1;

  angle.resize(angle_points);
  value.resize(angle_points);

  for (int i=0;i<angle_points;i++) {

    double ang_rad = (M_PI*i) / (angle_points-1);
    double ang_deg = (180.0*i) / (angle_points-1);

    angle[i] = ang_deg;

    double c = cos(ang_rad);
    value[i] = 0.06225*(1+0.835*c*c);
  }
}

