
// tool.h

#ifndef _JUDE_TOOL_TOOL_H
#define _JUDE_TOOL_TOOL_H

//#include "spectraldata.h" 
#include "base/jclock.h" 
#include "base/jtextstream.h" 
#include "ds/direcstruct.h" 
#include "ds/direcspec.h" 
#include "rad/raddirectable.h" 
#include "rad/raddirecdirecimp.h" 
#include "rad/raddirecdirectable.h" 
#include "ds/spherepartvxh.h" 
#include "ds/hemispherepartvxh.h" 
#include "base/jlog.h" 
#include "base/jparser.h" 
#include "base/jtextdevice.h" 
#include "rad/bandspec.h" 
#include "file/fileutils.h" 
//#include "bmatrix.h"

//template <typename SD> class SpecImp;
//template <typename SD> class ReflecImp;

//class JParser;
//class DirecSpec;

using namespace jude::base;
using namespace jude::ds;
using namespace jude::rad;

namespace jude {
namespace tool {


/*! Base class for tools.

  \todo There needs to be a more basic base class for tools in jude::base - JTool, then this needs to be RadTool or such like.
*/

class Tool {

  JClock total_time;

  JFile* log_file;

  bool processing_canceled;

  void writeAngularData(JTextStream& str, const std::vector<double>& phi_deg, const std::vector<double>& theta_deg, const RadDirecImp<SBandData>& spec, 
			bool div_sact, bool mult_sact);

protected:
 
  double pct_complete;

  void initRandom(const JParser& prm);

  void makePath(const JString& base, const JString& sub_dirs) const;

  DirecStruct* buildDirecStruct(const DirecSpec& ds_spec, bool do_full);

  template <typename A>
  RadDirecImp<A>* buildRadDirec(const DirecSpec& ds_spec, int band_count, bool do_full);

  template <typename A>
  RadDirecDirecImp<A>* buildRadDirecDirec(const DirecSpec& ds_spec, int band_count, bool do_full);

  bool saveRadDirecData(const JParser& prm, const JString& prefix, const JString& sub_dirs, 
			const JString& filename, const RadDirecImp<SBandData>& Ldata, bool mult_sact);

  bool saveIrradiance(const JParser& prm, const JString& prefix, const JString& sub_dirs, 
		      const JString& filename, const RadDirecImp<SBandData>& Ldata);

  bool saveReflectance(const JParser& prm, const JString& prefix, const JString& sub_dirs, 
		       const JString& filename, const SBandData& Rdata);

  void helpTextBandAndDirecSpec(JTextStream& str) const;

public:

  Tool();
  virtual ~Tool();

  virtual const JString name() const = 0;

  virtual bool init(const JParser& prm);
  virtual bool run(const JParser& prm) = 0;

  virtual void copyOverParams(JParser& targ_prm, const JParser& src_prm) const;

    /*! Derived class should return true if random number generation is used. */
  virtual bool usesRand() const { return false; }

  virtual void clearPctComplete() { pct_complete = 0; }
  virtual double pctComplete() const { return pct_complete; }

  void startTiming() { total_time.start(); }
  int runningTimeMsec() const { return total_time.msecElapsed(); }

  virtual void cancelProcessing() { processing_canceled = true; }
  virtual void uncancelProcessing() { processing_canceled = false; }
  bool processingCanceled() const { return processing_canceled; }

  void buildNotes(JTextDevice& nd, const JParser& prm);

    /*! Derived classes can override this to display brief help text but should also call this. */
  virtual void helpText(JTextStream& str) const; 
};



/*! \brief Creates directional function storage with partition structure and storage type from parameters specified in a JParser object.

If the specified parameters are incomplete or inconsistent an error message is streamed to jlog::es and the return is null.
It is the callers responsibility to ensure the returned object is ultimately deleted.

\todo Put cube partition back in.
*/

template <typename A>
RadDirecImp<A>* Tool::buildRadDirec(const DirecSpec& ds_spec, int band_count, bool do_full) {

  const JParser& prm = ds_spec.parser();

  RadDirecImp<A>* emis = 0;

    // get if table or adap partition
  bool is_table = prm.isDefined("storage") ? prm.valueCheck("storage", "table") : true;
  if (!is_table) { jlog::es << "Unknown partition storage type.\n"; return 0; }

    // get and check partition description
  JString partition(prm.getStringOrEmpty("partition"));
  if (partition.isEmpty()) partition = "sphere";

 
  if (partition == "sphere") {

      // get parameters
    int v = prm.getInt("vn");
    int h = prm.getInt("hn");
    if (v <= 0 || h <= 0 || (v&1) || ((h&1) && h!=1)) { jlog::es << "Bad sphere partition size " << v << " x " << h << ".\n"; return 0; }

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
      if (tv.size() == 0) { 
	if (is_table) emis = new RadDirecTable<A, SpherePartVxH>(SpherePartVxH(v, h), band_count);
	//else emis = new SpecAdap<SpectralData, HemiSpherePartVxH>(HemiSpherePartVxH(v/2, h));
      } else {
	if (is_table) emis = new RadDirecTable<A, SpherePartVxH>(SpherePartVxH(v, h, tv), band_count);
	//else emis = new SpecAdap<SpectralData, SpherePartVxH>(SpherePartVxH(v, h, tv));
      }

      // hemi sphere
    } else {
        // half the number of vertical quads
      v = v/2;
        // regular
      if (tv.size() == 0) { 
	if (is_table) emis = new RadDirecTable<A, HemiSpherePartVxH>(HemiSpherePartVxH(v, h), band_count);
	//else emis = new SpecAdap<SpectralData, HemiSpherePartVxH>(HemiSpherePartVxH(v, h));
      } else {
	  // cut down theta points to first half
	tv.resize((tv.size()+1)/2);
	if (is_table) emis = new RadDirecTable<A, HemiSpherePartVxH>(HemiSpherePartVxH(v, h, tv), band_count);
	//else emis = new SpecAdap<SpectralData, HemiSpherePartVxH>(HemiSpherePartVxH(v, h, tv));
      }
    }
    return emis;
  }

  jlog::es << "Unsupported partition type '" << partition << "'.";
  return 0;
}



/*! \brief Creates direction-to-direction function storage with partition structure and storage type from parameters specified in a JParser object.

If the specified parameters are incomplete or inconsistent an error message is streamed to jlog::es and the return is null.
It is the callers responsibility to ensure the returned object is ultimately deleted.

\todo Put cube partition back in.
*/

template <typename A>
RadDirecDirecImp<A>* Tool::buildRadDirecDirec(const DirecSpec& ds_spec, int band_count, bool do_full) {

  const JParser& prm = ds_spec.parser();

  RadDirecDirecImp<A>* r_func = 0;

    // get if table or adap partition
  bool is_table = prm.isDefined("storage") ? prm.valueCheck("storage", "table") : true;
  if (!is_table) { jlog::es << "Unknown partition storage type.\n"; return 0; }

    // get and check partition description
  JString partition(prm.getStringOrEmpty("partition"));
  if (partition.isEmpty()) partition = "sphere";


  if (partition == "sphere") {
 
      // get parameters
    int v = prm.getInt("vn");
    int h = prm.getInt("hn");
    if (v <= 0 || h <= 0 || (v&1) || ((h&1) && h!=1)) { jlog::es << "Bad sphere partition size " << v << " x " << h << ".\n"; return 0; }
   
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
      if (tv.size() == 0) { 
	if (is_table) r_func = new RadDirecDirecTable<A, SpherePartVxH>(SpherePartVxH(v, h), band_count);
	//else f_func = new SpecAdap<SpectralData, HemiSpherePartVxH>(HemiSpherePartVxH(v/2, h));
      } else {
	if (is_table) r_func = new RadDirecDirecTable<A, SpherePartVxH>(SpherePartVxH(v, h, tv), band_count);
	//else emis = new SpecAdap<SpectralData, SpherePartVxH>(SpherePartVxH(v, h, tv));
      }

      // hemi sphere
    } else {
        // half the number of vertical quads
      v = v/2;
        // regular
      if (tv.size() == 0) { 
	if (is_table) r_func = new RadDirecDirecTable<A, HemiSpherePartVxH>(HemiSpherePartVxH(v, h), band_count);
	//else emis = new SpecAdap<SpectralData, HemiSpherePartVxH>(HemiSpherePartVxH(v, h));
      } else {
	  // cut down theta points to first half
	tv.resize((tv.size()+1)/2);
	if (is_table) r_func = new RadDirecDirecTable<A, HemiSpherePartVxH>(HemiSpherePartVxH(v, h, tv), band_count);
	//else emis = new SpecAdap<SpectralData, HemiSpherePartVxH>(HemiSpherePartVxH(v, h, tv));
      }
    }
    return r_func;
  }

  jlog::es << "Unsupported partition type '" << partition << "'.";
  return 0;
}



} } // namespace::tool

#endif // _JUDE_TOOL_TOOL_H

