
// tool.cc

#include "tool.h"
#include "base/jlog.h"
#include "base/jutil.h"
#include "base/jversion.h"
#include "base/jdir.h"
#include "math/mathutil.h"
#include "math/rand.h"
#include "rad/raddirectable.h"
#include "rad/sbanddata.h"
#include "ds/spherepartvxh.h"
#include "ds/hemispherepartvxh.h"

//#include "spectraldata.h"
//#include "spectraldata_util.h"
//#include "specadap.h"
//#include "reflectable.h"
//#include "reflecsparse.h"
//#include "reflecsparsesym.h"
//#include "spherepartvxh.h"
//#include "parser.h"
//#include "error.h"
//#include "direcspec.h"
//#include "bandspec.h"
//#include "jconsole.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
//#include "boost/filesystem.hpp" 

//#ifdef USE_CUBLAS
//#include "cublas.h"
//#endif

//#ifdef NONFREE
//#include "cubepartn.h"
//#endif

using namespace jude::base;
using namespace jude::math;
using namespace jude::ds;
using namespace jude::rad;
using namespace jude::tool;



// Tool::(constructor) 

Tool::Tool() : 
  log_file(0),
  pct_complete(0) { }



// Tool::(destructor) 

Tool::~Tool() {
  if (log_file) { log_file->close(); delete log_file; }
}



// Tool::init()
// gets the verbose level or sets default = 2
// displays some initial info
// returns true if nothing else to do

bool Tool::init(const JParser& prm) {

    // set up a log file that can be written to with jlog::lsl
  JString log_fp = prm.getStringOrEmpty("log_save_fp");
  if (!log_fp.isEmpty()) {
    log_file = new JFile(log_fp);
    if (log_file->open(JIO_WriteOnly)) JLog::setLogFile(log_file);
    else {
      delete log_file; 
      log_file = 0;
      jlog::es << "Couldn't open log file '" << log_fp << "' for writing.\n"; 
    } 
  }

  if (jlog::vbLevel(1)) jlog::ls.printf("%s %s\n", name().latin1(), JUDE_RELEASE_STRING);
  if (jlog::vbLevel(4)) prm.displayList(jlog::ls);
  else prm.displayList(jlog::f);

    // display help text
  if (prm.hyphenParamCount() > 0) { helpText(jlog::ls); return true; }

  return false;
} 



// Tool::buildNotes()

void Tool::buildNotes(JTextDevice& nd, const JParser& prm) {
  JTextStream ns(&nd);
  if (prm.isDefined("notes_include_params")) if (prm.getBool("notes_include_params") == false) return;
  prm.writeParams(ns);
}



/*

// Tool::displayRadianceByTheta()

void Tool::displayRadianceByTheta(const JString& text, double phi_deg, int theta_points, const SpecImp<SpectralData>& s, bool div_sa) {

  JTextStream str(&JConsole::standard);

  str << text << ":\n";

  writeThetaPoints(str, phi_deg, theta_points, s, div_sa);
}



void Tool::writeThetaPoints(JTextStream& str, double phi_deg, int theta_points, const SpecImp<SpectralData>& spec, bool div_sact) {

  const DirecStruct& ds(spec.direcStruct());

  double theta_range = ds.isFull() ? 2*M_PI : M_PI;
  double theta_start = -M_PI/2 + theta_range / (2*theta_points);

  double phi_rad = GlobalFuncs::degreesToRadians(phi_deg);

  for (int i=0; i<theta_points; i++) {

    double theta = theta_start + (i*theta_range) / theta_points;

    int index = ds.direcToIndex(Vector3D(theta,phi_rad));

    SpectralData sd(spec.atIndex(index));
    if (div_sact) sd.divide(ds.solidAngleAbsCosTheta(index));

    str << GlobalFuncs::radiansToDegrees(theta) << " ";
    str << phi_deg << " ";
    str << index << " ";
    str << ds.meanCosTheta(index) << " ";  
    str << sd << "\n";
  }
}



*/


// Tool::writeAngularData()

void Tool::writeAngularData(JTextStream& str, const std::vector<double>& phi_deg, const std::vector<double>& theta_deg, const RadDirecImp<SBandData>& spec, 
			    bool div_sact, bool mult_sact) {

  const DirecStruct& ds(spec.direcStruct());

    // write header line
  str << "theta(deg) phi(deg) quad_index quad_mean_cos_theta";
  for (int i=0; i<spec.bandCount(); i++) str << JString(" b")+JString::number(i);
  str << "\n";

  for (unsigned int i=0; i<phi_deg.size(); i++) {

    for (unsigned int j=0; j<theta_deg.size(); j++) {

      double phi_rad = degToRad(phi_deg[i]);
      double theta_rad = degToRad(theta_deg[j]);

      int index = ds.direcToIndex(Vec3D(theta_rad, phi_rad));

      SBandData sd(spec.atIndex(index));

        // turn irradiance integral (used in aqrad) into quad averaged radiance
      if (div_sact) sd.divide(ds.solidAngleAbsCosTheta(index));
        // turn quad averaged radiance into irradiance integral
      if (mult_sact) sd.multiply(ds.solidAngleAbsCosTheta(index));

      str << theta_deg[j] << " ";
      str << phi_deg[i] << " ";
      str << index << " ";
      str << ds.meanCosTheta(index) << " ";
        // write spectral data with space for seperator between bands and sub-band info
      for (int i=0; i<spec.bandCount()-1; i++) sd.write(str, " ", " ");
        // end of entry
      str << "\n";
    }

    str << "\n\n";
  }
}



/*

// Tool::initBands() [protected]

bool Tool::initBands(const JParser& prm) {

     // old way of specifying bands - depreceated
  if (!prm.isDefined("band_spec")) {

      // set the number of spectral bands
    SpectralData::setBands(prm.getInt("bands")); 
      // centres
    if (prm.isDefined("band_centres")) {
      SpectralData::setBandCentres(SpectralData(FileDesc(prm.getString("band_centres"), true)));
    }
      // widths
    if (prm.isDefined("band_widths")) {
      SpectralData::setBandWidths(SpectralData(FileDesc(prm.getString("band_widths"), true)));
    }

    // band_spec definition
  } else {

    BandSpec* bs = BandSpec::createFromFile(prm.getString("band_spec"));
    if (!bs) return false;

    SpectralData::setBands(bs->bandCount()); 
    SpectralData::setBandCentres(SpectralData(bs->bandCentres()));
    SpectralData::setBandWidths(SpectralData(bs->bandWidths()));
  }

  SpectralData::buildTristimFuncs();

  SpectralData::displaySetup(ls);

  return true;
}


*/


// Tool::initRandom()

void Tool::initRandom(const JParser& prm) {

  if (prm.isDefined("random_seed")) {
    if (prm.valueCheck("random_seed","time")) random_seed(time(0));
    else if (prm.valueCheck("random_seed","pid")) random_seed(getpid());
    else if (prm.valueCheck("random_seed","pidtime")) random_seed(getpid() + time(0));
    else random_seed(prm.getInt("random_seed"));
  } else random_seed(getpid() + time(0));
}



/*! Copies common tool parameters from \a src_prm to \a targ_prm.

*/

void Tool::copyOverParams(JParser& targ_prm, const JParser& src_prm) const {

  targ_prm.copyLabelAndValue(src_prm, "log_save_fp");
  targ_prm.copyLabelAndValue(src_prm, "verbose");
  targ_prm.copyLabelAndValue(src_prm, "random_seed");
  targ_prm.copyLabelAndValue(src_prm, "notes_include_params");
}



/*! Ensures that a nested directory structure exists by creating directories as required. 

The base path is \a base e.g. "/home/jude/test" and \a sub_dirs is the directory structure below 
the base with preceding "/"s, i.e. "/dir1/dir2"

*/

void Tool::makePath(const JString& base, const JString& sub_dirs) const {

  for (int i=0; i<=sub_dirs.contains('/'); i++) {
    JDir dir(base + sub_dirs.section('/', 0, i));
    if (!dir.exists()) dir.mkdir();
  }
}



/*! \brief Creates a directional partition structure from parameters specified in a JParser object.

If the specified parameters are incomplete or inconsistent an error message is streamed to jlog::es and the return is null.
It is the callers responsibility to ensure the returned object is ultimately deleted.

\todo Put cube partition back in.
*/

DirecStruct* Tool::buildDirecStruct(const DirecSpec& ds_spec, bool do_full) {

  const JParser& prm = ds_spec.parser();

    // get and check partition description
  JString partition(prm.getString("partition"));
  if (partition.isEmpty()) return 0;

  // DIST1_EXCLUDE_START

    // cube partition
  if (partition == "cube") {
      // get partition size
    int n = prm.getInt("n");
    if (n <= 0) { jlog::es << "Bad cube partition size " << n << ".\n"; return 0; }

    //if (do_full) return new CubePartN(n);
    //else return new HemiCubePartN(n);
    return 0;
  }

  // DIST1_EXCLUDE_END

    // sphere partition
  if (partition == "sphere") {
 
    int v = prm.getInt("vn");
    int h = prm.getInt("hn");
    //if (v <= 0 || h <= 0 || (v&1) || (h&1)) { jlog::es << "Bad sphere partition size " << v << " x " << h << ".\n"; return 0; }
    // bodge to allow single azimuth quad
    if (v <= 0 || h <= 0 || (v&1)) { jlog::es << "Bad sphere partition size " << v << " x " << h << ".\n"; return 0; }
    
      // get theta points or not if not defined
    std::vector<double> tv;
    if (prm.isDefined("theta_points") || prm.isDefined("theta_points_fp")) {
      if (prm.isDefined("theta_points")) tv = prm.getVectorOfDoubles("theta_points");
      else { if (!readFromTextFile(tv, prm.getString("theta_points_fp"))) return 0; }
      if (((int)tv.size()) != v+3) { 
	jlog::es << "Wrong number of theta points (" << (int) tv.size() << " should be " << (v+3) << ").\n"; 
	return 0;
      } 
    }
      // full sphere
    if (do_full) {
      if (tv.size() == 0) return new SpherePartVxH(v,h);
      else return new SpherePartVxH(v,h,tv);
      // hemisphere
    } else {
      v = v /2;
      if (tv.size() == 0) return new HemiSpherePartVxH(v,h);
      else {
	tv.resize((tv.size()+1)/2);
	return new HemiSpherePartVxH(v,h,tv);
      }
    }
  }

  jlog::es << "Unsupported partition type '" << partition << "'.";
  return 0;
}



// Tool::saveRadiance()

bool Tool::saveRadDirecData(const JParser& prm, const JString& prefix, const JString& sub_dirs, 
			    const JString& filename, const RadDirecImp<SBandData>& Ldata, bool mult_sact) {

  if (!prm.isDefined(prefix+"_output_dir")) return true;

    // tree base directory for this data
  JString path(prm.getString(prefix+"_output_dir"));
    // make sure directory exists
  makePath(path, sub_dirs);
    // full file path
  path += sub_dirs + "/" + filename;
  
    // arrays of theta and phi angles (out directions) to output radiance for
  std::vector<double> phi_deg;
  std::vector<double> theta_deg;

    // phi is given as a list of angles in degrees
  if (prm.isDefined(prefix+"_output_phi_deg")) {

    phi_deg = prm.getVectorOfDoubles(prefix+"_output_phi_deg");

    // or phi is set up as a number of points at regular intervals
  } else {

    int n = prm.getInt(prefix+"_output_phi_points");
    double phi_range = Ldata.direcStruct().isFull() ? 360 : 180;
    double phi_start = -90 + phi_range / (2*n);

    phi_deg.resize(n);
    for (int i=0; i<n; i++) phi_deg[i] = phi_start + (i*phi_range) / n;
  }

    // theta is given as a list of angles in degrees
  if (prm.isDefined(prefix+"_output_theta_deg")) {

    theta_deg = prm.getVectorOfDoubles(prefix+"_output_theta_deg");

    // or theta is set up as a number of points at regular intervals
  } else {

    int n = prm.getInt(prefix+"_output_theta_points");
    double theta_range = Ldata.direcStruct().isFull() ? 360 : 180;
    double theta_start = -90 + theta_range / (2*n);

    theta_deg.resize(n);
    for (int i=0; i<n; i++) theta_deg[i] = theta_start + (i*theta_range) / n;
  }

    // get phi and number of theta points
  //double phi_deg = prm.isDefined(prefix+"_output_phi_deg") ? prm.getDouble(prefix+"_output_phi_deg") : 0;
  //int theta_points = prm.isDefined(prefix+"_output_theta_points") ? prm.getInt(prefix+"_output_theta_points") : 20;

  jlog::ls << "Saving: '" << path.latin1() << "'\n"; 

  JFile f(path);
  if (!f.open(JIO_WriteOnly)) return false;

  JTextStream str(&f);

  writeAngularData(str, phi_deg, theta_deg, Ldata, false, mult_sact);

  f.close();

  return true;
}



// Tool::saveIrradiance()

bool Tool::saveIrradiance(const JParser& prm, const JString& prefix, const JString& sub_dirs, 
			  const JString& filename, const RadDirecImp<SBandData>& Ldata) {

  if (!prm.isDefined(prefix+"_output_dir")) return true;

    // tree base directory for this data
  JString path(prm.getString(prefix+"_output_dir"));
    // make sure directory exists
  makePath(path, sub_dirs);
    // full file path
  path += sub_dirs + "/" + filename;

    // calculate plane irradiance
  Ldata.lazyInvalidate();
  SBandData sd(Ldata.sumBySolidAngleAbsCosTheta());
  
  if (jlog::vbLevel(1)) jlog::ls << "Saving: '" << path << "'\n";

  FileDesc fd(path, true);
  FileUtils::writeFactoryBuiltToFile(sd, fd);

  return true;
}



// Tool::saveReflectance()

bool Tool::saveReflectance(const JParser& prm, const JString& prefix, const JString& sub_dirs, 
			   const JString& filename, const SBandData& Rdata) {

  if (!prm.isDefined(prefix+"_output_dir")) return true;

    // tree base directory for this data
  JString path(prm.getString(prefix+"_output_dir"));
    // make sure directory exists
  makePath(path, sub_dirs);
    // full file path
  path += sub_dirs + "/" + filename;

  if (jlog::vbLevel(1)) jlog::ls << "Saving: '" << path << "'\n";

  FileDesc fd(path);
  FileUtils::writeFactoryBuiltToFile(Rdata, fd);

    // quit if no list file to build
  if (!prm.isDefined(prefix+"_list_out")) return true;

    // open list file
  JFile file(prm.getString(prefix+"_list_out").stripWhiteSpace());
  if (!file.open(JIO_AppendOnly)) {
    jlog::es << "File '" << path.latin1() << "' could not be written.\n"; 
    internalError();
  }
    // append this reflectance
  JTextStream str(&file);   
  str << path << "\n";
    // close file
  file.close();

  return true;
}



// Tool::helpTextBandAnDirecSpec()
// is the same in many tools help text

void Tool::helpTextBandAndDirecSpec(JTextStream& str) const {

  str << "Band And Directional Structure Direct Specification\n";
  str << "---------------------------------------------------\n";
  str << "All band and directional structure specifications can be given directly\n";
  str << "in the supplied parameters rather than by reference to a file. In this case\n";
  str << "the band specification and directional structure specification parameters\n";
  str << "are given with an appropriate prefix to indicate to what they refer.\n\n";
}



// Tool::helpText()

void Tool::helpText(JTextStream& str) const {
 
  str << "General Options\n";
  str << "---------------\n";
  str << "verbose     = <integer>  (verbosity level 0-9, default '2')\n";  
  str << "log_save_fp = <filepath>  (file to save log to, optional)\n";  
  if (usesRand()) str << "random_seed = time | pid | pidtime | <int>  (initialisation of random number generation, optional)\n";
  str << "\n";
  str << "notes_include_params = yes | no   (include run parameters in notes of saved data files, default yes)\n";  
}



/*! \brief Creates a full directional partition structure from parameters specified in a Parser object.

If the specified parameters are incomplete or inconsistent an error message is streamed to jlog::es and the return is null.
It is the callers responsibility to ensure the returned object is ultimately deleted.

\todo Put cube partition back in.
*/
  /*
DirecStruct* Tool::fullDirecStruct(const JParser& prm) {

  if (!prm.isDefined("partition")) {
    jlog::es << "No partition specified.";
    return 0;
  }

  // DIST1_EXCLUDE_START

    // cube partition
  if (prm.valueCheck("partition", "cube")) {

      // get partition size
    int n = prm.getInt("n");
    if (n <= 0) { jlog::es << "Bad cube partition size " << n << ".\n"; return 0; }

    //return new CubePartN(n);
    return 0;
  }

  // DIST1_EXCLUDE_END

    // sphere partition
  if (prm.valueCheck("partition", "sphere")) {

    int v = prm.getInt("vn");
    int h = prm.getInt("hn");
    if (v <= 0 || h <= 0) { jlog::es << "Bad sphere partition size " << v << " x " << h << ".\n"; return 0; }
    
      // regular partition
    if (!prm.isDefined("theta_points")) return new SpherePartVxH(v,h);
    
    std::vector<double> tv;
    if (!readFromTextFile(tv, prm.getString("theta_points"))) {
      jlog::es << "Couldn't read theta points file "; 
      return 0;
    }

    if (tv.size() != v+3) {
      jlog::es << "Wrong number of theta points (" << (int) tv.size() << " should be " << (v+3) << ").\n"; 
      return 0; 
    }

    return new SpherePartVxH(v,h,tv);
  }

  jlog::es << "Unknown partition type.";
  return 0;
}
  */


/*! \brief Creates a half directional partition structure from parameters specified in a JParser object.

If the specified parameters are incomplete or inconsistent an error message is streamed to jlog::es and the return is null.
It is the callers responsibility to ensure the returned object is ultimately deleted.

\todo Put cube partition back in.
*/
  /*
DirecStruct* Tool::halfDirecStruct(const JParser& prm) {

  if (!prm.isDefined("partition")) {
    jlog::es << "No partition specified.";
    return 0;
  }

  // DIST1_EXCLUDE_START

    // cube partition
  if (prm.valueCheck("partition", "cube")) {

      // get partition size
    int n = prm.getInt("n");
    if (n <= 0) { jlog::es << "Bad cube partition size " << n << ".\n"; return 0; }

    //return new HemiCubePartN(n);
    return 0;
  }  

  // DIST1_EXCLUDE_END

    // sphere partition
  if (prm.valueCheck("partition", "sphere")) {

    int hv = prm.getInt("hvn");
    int h = prm.getInt("hn");
    if (hv <= 0 || h <= 0) { jlog::es << "Bad sphere partition size " << hv << " x " << h << ".\n"; return 0; }
   
      // regular partition 
    if (!prm.isDefined("half_theta_points")) return new HemiSpherePartVxH(hv,h);

    std::vector<double> tv;
    if (!readFromTextFile(tv, prm.getString("half_theta_points"))) {
      jlog::es << "Couldn't read half theta points file "; 
      return 0;
    }

    if (tv.size() != hv+2) {
      jlog::es << "Wrong number of theta points (" << (int) tv.size() << " should be " << (hv+2) << ").\n"; 
      return 0; 
    }
    
    return new HemiSpherePartVxH(hv,h,tv);
  }

  jlog::es << "Unknown partition type.";
  return 0;
}
  */

/*

// Tool::buildFullSpec()

SpecImp<SpectralData>* Tool::buildFullSpec(const JParser& prm) {

  SpecImp<SpectralData>* emis;

    // get if table or adap partition
  bool is_table = prm.valueCheck("storage","table");
  if (!is_table) dataError("Unknown partition storage type.");

#ifdef NONFREE

    // cube partition
  if (prm.valueCheck("partition", "cube")) {

    int n = prm.getInt("n");
    if (n <= 0) dataError("Bad cube partition size.");

    if (is_table) emis = new SpecTable<SpectralData, CubePartN>(CubePartN(n));
    else emis = new SpecAdap<SpectralData, CubePartN>(CubePartN(n));

    return emis;
  }

#endif

    // sphere partition
  if (prm.valueCheck("partition", "sphere")) {

    int v = prm.getInt("vn");
    int h = prm.getInt("hn");
    if (v <= 0 || h <= 0) dataError("Bad sphere partition size.");

    if (!prm.isDefined("theta_points")) {
      if (is_table) emis = new SpecTable<SpectralData, SpherePartVxH>(SpherePartVxH(v,h));
      else emis = new SpecAdap<SpectralData, SpherePartVxH>(SpherePartVxH(v,h));
    } else {

      Vector tv;
      tv.readFromTextFile(prm.getString("theta_points"));
      if (tv.size() != v+3) dataError("Wrong number of theta points"); 

      if (is_table) emis = new SpecTable<SpectralData, SpherePartVxH>(SpherePartVxH(v,h,tv));
      else emis = new SpecAdap<SpectralData, SpherePartVxH>(SpherePartVxH(v,h,tv));
    }

    return emis;
  }

  dataError("Unknown partition type.");
  return 0;
}



// Tool::buildFullSpec()

SpecImp<SpectralData>* Tool::buildFullSpec(const DirecSpec& ds_spec) {

  SpecImp<SpectralData>* emis;

#ifdef NONFREE

    // cube partition
  if (ds_spec.isCubePartition()) {

    if (ds_spec.isTableStorage()) emis = new SpecTable<SpectralData, CubePartN>(CubePartN(ds_spec.cubeN()));
    else emis = new SpecAdap<SpectralData, CubePartN>(CubePartN(ds_spec.cubeN()));

    return emis;
  } 

#endif

  if (ds_spec.isSpherePartition()) {

    int v = ds_spec.sphereVN();
    int h = ds_spec.sphereHN();

    if (ds_spec.thetaPointsFilePath().isEmpty()) {
      if (ds_spec.isTableStorage()) emis = new SpecTable<SpectralData, SpherePartVxH>(SpherePartVxH(v,h));
      else emis = new SpecAdap<SpectralData, SpherePartVxH>(SpherePartVxH(v,h));
    } else {

      Vector tv;
      tv.readFromTextFile(ds_spec.thetaPointsFilePath());
      if (tv.size() != v+3) dataError("Wrong number of theta points"); 

      if (ds_spec.isTableStorage()) emis = new SpecTable<SpectralData, SpherePartVxH>(SpherePartVxH(v,h,tv));
      else emis = new SpecAdap<SpectralData, SpherePartVxH>(SpherePartVxH(v,h,tv));
    }

    return emis;
  }

  dataError("Unknown partition type.");
  return 0;
}

*/


// Tool::buildHalfSpec()
// for sphere partitions (vn) refers to the full partition, i.e. use vn/2 for a hemi partition


/*

// Tool::buildFullReflec()

ReflecImp<SpectralData>* Tool::buildFullReflec(const JParser& prm) {

  ReflecImp<SpectralData>* pf;

   // get partition type
  //bool is_cube = prm.valueCheck("partition", "cube");
  //if (!is_cube && !prm.valueCheck("partition", "sphere")) dataError("Unknown partition type.");

    // get storage method
  bool is_table = prm.valueCheck("storage","table");
  bool is_sparse = prm.valueCheck("storage","sparse");
  bool is_sparsesym = prm.valueCheck("storage","sparsesym");
  if (!is_table && !is_sparse && !is_sparsesym) dataError("Unknown partition storage type.");

#ifdef NONFREE
  
    // get partition size
  if (prm.valueCheck("partition", "cube")) {

    int n = prm.getInt("n");
    if (n <= 0) dataError("Bad cube partition size.");

    if (is_table) pf = new ReflecTable<SpectralData, CubePartN>(CubePartN(n));
    else if (is_sparse) pf = new ReflecSparse<SpectralData, CubePartN>(CubePartN(n));
    else pf = new ReflecSparseSym<SpectralData, CubePartN>(CubePartN(n));

    return pf;
  }

#endif
 
  if (prm.valueCheck("partition", "sphere")) {

    int v = prm.getInt("vn");
    int h = prm.getInt("hn");
    if (v <= 0 || h <= 0) dataError("Bad sphere partition size.");

    if (!prm.isDefined("theta_points")) {

      if (is_table) pf = new ReflecTable<SpectralData, SpherePartVxH>(SpherePartVxH(v,h));
      else if (is_sparse) pf = new ReflecSparse<SpectralData, SpherePartVxH>(SpherePartVxH(v,h));
      else pf = new ReflecSparseSym<SpectralData, SpherePartVxH>(SpherePartVxH(v,h));

    } else {

      Vector tv;
      tv.readFromTextFile(prm.getString("theta_points"));
      if (tv.size() != v+3) dataError("Wrong number of theta points"); 

      if (is_table) pf = new ReflecTable<SpectralData, SpherePartVxH>(SpherePartVxH(v,h,tv));
      else if (is_sparse) pf = new ReflecSparse<SpectralData, SpherePartVxH>(SpherePartVxH(v,h,tv));
      else pf = new ReflecSparseSym<SpectralData, SpherePartVxH>(SpherePartVxH(v,h,tv));
    }

    return pf;
  }

  dataError("Unknown partition type.");
  return 0;
}



// Tool::buildFullReflec()

ReflecImp<SpectralData>* Tool::buildFullReflec(const DirecSpec& ds_spec, int storage_type) {

  ReflecImp<SpectralData>* pf;

    // get storage method
  //bool is_table = prm.valueCheck("storage","table");
  //bool is_sparse = prm.valueCheck("storage","sparse");
  //bool is_sparsesym = prm.valueCheck("storage","sparsesym");
  //if (!is_table && !is_sparse && !is_sparsesym) dataError("Unknown partition storage type.");

#ifdef NONFREE

    // get partition size
  if (ds_spec.isCubePartition()) {

    int n = ds_spec.cubeN();

    if (storage_type == Reflec::TABLE) pf = new ReflecTable<SpectralData, CubePartN>(CubePartN(n));
    else if (storage_type == Reflec::SPARSE) pf = new ReflecSparse<SpectralData, CubePartN>(CubePartN(n));
    else pf = new ReflecSparseSym<SpectralData, CubePartN>(CubePartN(n));

       // or sphere partition
    return pf;
  } 

#endif

  if (ds_spec.isSpherePartition()) {

    int v = ds_spec.sphereVN();
    int h = ds_spec.sphereHN();

    if (ds_spec.thetaPointsFilePath().isEmpty()) {
       
       if (storage_type == Reflec::TABLE) pf = new ReflecTable<SpectralData, SpherePartVxH>(SpherePartVxH(v,h));
       else if (storage_type == Reflec::SPARSE) pf = new ReflecSparse<SpectralData, SpherePartVxH>(SpherePartVxH(v,h));
       else pf = new ReflecSparseSym<SpectralData, SpherePartVxH>(SpherePartVxH(v,h));

    } else {

      Vector tv;
      tv.readFromTextFile(ds_spec.thetaPointsFilePath());
      if (tv.size() != v+3) dataError("Wrong number of theta points"); 

      if (storage_type == Reflec::TABLE) pf = new ReflecTable<SpectralData, SpherePartVxH>(SpherePartVxH(v,h,tv));
      else if (storage_type == Reflec::SPARSE) pf = new ReflecSparse<SpectralData, SpherePartVxH>(SpherePartVxH(v,h,tv));
      else pf = new ReflecSparseSym<SpectralData, SpherePartVxH>(SpherePartVxH(v,h,tv));
    }

    return pf;
  }

  dataError("Unknown partition type.");
  return 0;
}



*/

