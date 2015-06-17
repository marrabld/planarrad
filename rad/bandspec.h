
// bandspec.h

#ifndef _JUDE_RAD_BANDSPEC_H_JDH
#define _JUDE_RAD_BANDSPEC_H_JDH

#include "rad/radtypes.h"
#include "base/jstring.h"
#include "base/jlist.h"
#include "base/jparser.h"
#include "math/ublas.h"
#include "rad/policys.h"
#include "base/jerror.h"
#include "math/ipol.h"


using namespace jude::math;

namespace jude {
namespace rad {

class Report;

template <typename DT> class SBandData_Imp;

typedef SBandData_Imp<PolicyS> SBandData;


/*!
Individual band data value is treated as being per nm
three ways of specifyinga band spec
 
1) band centres only: 
in this case when band width is needed (e.g. calculating actual power integrated over wavelength)
then a box function is assumed so that the transition between bands is ath the midpoint between centres
for the first and last band the width on both sides is of the centre is equal 
internally the widths are generated on construction so the only subsequent difference in behaviour between (1) and (2)
is that bandWidthsSpecified() return false here

2) band widths and centres
the widths are givem and bands are treated as a box function - than bands need not be contiguous and could even overlap

3) band sensitivity function is given - not yet implemneted

\todo - all of this
*/


class BandSpec {

    // if he band spec was read froma dedicated file this is its path
    // otherwise is empty
  JString filepath_str;

    // name and code for this BandSpec
    // optional but name at least is required for menus in GUI context
  JString name_str; 
  JString code_str;

    // number of bands
  int band_count;
 
    // if the centres and widths were supplied from a file these are the paths
    // otherwise they are empty
  JString centres_fp;
  JString widths_fp;
 
    // these define the bands as box functions
    // centres must be given and widths is always avalilabe, is calcuated from centres if not given
  vector<double> band_centres;
  vector<double> band_widths;

    // these are calculated for efficiency
  vector<double> band_lo;
  vector<double> band_hi;

    // multiplier that converts from band value in Watts nm-1 to umol nm-1 
    // this is lazy calculated on first use
  mutable vector<double> umol_conv;

    // the weighting functions for converting to red, green and blue
    // these are lazy calculated on first use
  mutable vector<double> tristim_red;
  mutable vector<double> tristim_green;
  mutable vector<double> tristim_blue;

    // list of available BandSpecs read in in GUI context programs
  static JList<BandSpec> available_list;
    // the default in the list
  static BandSpec* default_bs;

    // automatic width generation
  void buildBandWidthsFromCentres();
    // calc limits so they are available
  void buildBandLoAndHi();

    // lazy evaluation functions
  void calcUmolConv() const;
  void calcTristimFuncs() const;

    // default constructor unspecified
  BandSpec();
    // constructor which just sets filepath is private
  BandSpec(const JString& fp);
 
public:

    /*! \brief Initialises and clears the global list of available band specifications. */
  static void initAvailableList();
    /*! \brief If a band specification can be found in \a prm using \a prefix the create and add it. */
  static BandSpec* addToAvailableList(const JParser& prm, const JString& prefix, bool make_default);
    /*! \brief Populates the static list of available band specifications by reading files in the directory \a dir_path. */
  static bool addToAvailableList(const JString& dir_path);
  static bool addDefaultToAvailableList();
    /*! \brief Returns a reference to the global list of available band specifications. */
  static JList<BandSpec>& availableList() { return available_list; }
  static const BandSpec* availableDefault() { return default_bs; }

    /*! \brief Creates a band specification directly from the supplied parser data or via a referenced file. */
  static BandSpec* create(const JParser& prm, const JString& prefix);
    /*! \brief Creates a band specification directly from the supplied parser data or via a referenced file. */
  static BandSpec* create(const JParser& prm, const JString& prefix, bool& preferred_default);
    /*! \brief Creates a band specification directly from a file, specified by \a file_path. */
  static BandSpec* createFromFile(const JString& file_path, bool& preferred_default);
    /*! \brief Creates a band specification directly from a file, specified by \a file_path. */
  static BandSpec* createFromReport(const JTextTable& ttab);
 
    /*! \brief Constructs a band specification which is a contiguous subset of \a use_band_count bands from \a src_bs starting at index \a use_band_start. */ 
  BandSpec(const BandSpec& src_bs, int use_band_start, int use_band_count);
    /*! \brief Constructs a band specification of \a n bands with band centres and widths in nm specified in the arrays \a bc \and \a bw. */
  BandSpec(int n, const double* bc, const double *bw);
    /*! \brief Constructs a band specification of \a n bands with band centres in nm specified in the array \a bc, widths generated automatically. */
  BandSpec(int n, const double* bc);
 
  BandSpec(JDataStream& str, uns32 saved_version) { read(str, saved_version); }


    /*! \brief Reads in the band specification from the parser \a prm from parameter labels prefixed by \a prefix. */
  bool readFromParser(const JParser& prm, const JString& prefix, bool& preferred_default);
    /*! \brief Adds parameters to reconstruct the band specification to \a prm with labels prefixed with \a prefix. */ 
  void addToParser(JParser& prm, const JString& prefix) const;

    /*! \brief Returns the number of bands in the band specification. */
  int bandCount() const { return band_count; }
    /*! \brief Returns true if the centre wavelengths of the bands are specified (widths will also be available). */
  bool hasWavelengthData() const { return band_centres.size() != 0; }

    /*! \brief Returns the name of the band specification. */
  JString name() const { return name_str; }
    /*! \brief Sets the name of the band specification. */
  void setName(const JString& s) { name_str = s; }

    /*! \brief Returns the code of the band specification. */
  JString code() const { return code_str; }
    /*! \brief Sets the code of the band specification. */
  void setCode(const JString& s) { code_str = s; }


  JString filepath() const { return filepath_str; }

    /*! \brief Returns a vector of the band center locations in nm. */
  const vector<double>& bandCentres() const { return band_centres; }
    /*! \brief Returns a vector of the band widths in nm. */
  const vector<double>& bandWidths() const { return band_widths; }
    /*! \brief Returns a vector of the band box function lower bounds in nm. */
  const vector<double>& bandLow() const { return band_lo; }
    /*! \brief Returns a vector of the band box function upper bounds in nm. */
  const vector<double>& bandHigh() const { return band_hi; }

    /*! \brief Returns the index of the closest band to wavelnegth \a lambda_nm. */
  int closestBand(double lambda_nm) const;

  template <typename SD>
  SD resampleToThis(const SD& src_sd, const BandSpec& src_bs) const;

    // these should use SBandData as input
  vector<double> EtoUmol(const vector<double>& irrad_watts) const;
  double EtoPAR(const vector<double>& irrad_watts) const;
  double integrateOverWavelength(const SBandData& power_per_waveln) const;

    /*! \brief Returns the red lumens from radiance \a sd using the C.I.E. red tristimulus function. */
  double redLumens(const SBandData& sd) const;
    /*! \brief Returns the green lumens from radiance \a sd using the C.I.E. green tristimulus function. */
  double greenLumens(const SBandData& sd) const;
    /*! \brief Returns the blue lumens from radiance \a sd using the C.I.E. blue tristimulus function. */
  double blueLumens(const SBandData& sd) const;

    /*! \brief Comvienience function that writes an error to jlog::es and returns false if \a sd does not have the same number of bands as this. */
  bool checkBandCount(const JString& text, const SBandData& sd) const;

    /*! \brief Reads the band specification from a data file. The version number of the program that saved the file is \a saved version. */
  bool read(JDataStream& str, uns32 saved_version);
    /*! \brief Writes the band specification to a data file. */
  bool write(JDataStream& str) const;

  void writeToReport(Report& rep) const;

    /*! \brief Display brief help text on parameters required to build a band specification. */  
  static void helpText(JTextStream& str);

    /*! \brief Display info on the band specification on a text stream. */
  void display(JTextStream& str) const;
};



/*! \brief Resamples \a src_sd expressed in band specification \a src_bs to this band specification.
  This will result in an internal error if this band spec or src_bs has no wavelength data, so always check with
  hasWavelengthData() first. 
  \todo The checks could be removed in a non-debig compile.
*/

template <typename SD>
SD BandSpec::resampleToThis(const SD& src_sd, const BandSpec& src_bs) const {
    // checks
  if (!hasWavelengthData()) { jlog::es << "BandSpec::resampleToThis() called but no wavelength data supplied.\n"; internalError(); }
  if (!src_bs.hasWavelengthData()) { jlog::es << "BandSpec::resampleToThis() called but src_bs has no wavelength data.\n"; internalError(); }
    // linear interpolation
  return SD(Ipol::linearInterpolate(band_centres, src_sd.vectorData(), src_bs.bandCentres()));
}



} } // namespace jude::tool

#endif // _JUDE_RAD_BANDSPEC_H

