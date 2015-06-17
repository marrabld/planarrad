
// sbanddata.h

#ifndef _JUDE_RAD_SBANDDATA_H
#define _JUDE_RAD_SBANDDATA_H

#include "rad/radtypes.h"
#include "rad/bandspec.h"
#include "math/ublas.h"
#include "math/mathutil.h"
#include "base/jtextstream.h"
#include "rad/policys.h"


using namespace jude::math;
using namespace jude::base;



namespace jude {
namespace rad {



class SBandData_Base { 

public:

  virtual ~SBandData_Base() { }

  virtual const JString factoryID() const = 0;

  //virtual void display(JTextStream& str) const = 0;
};



//JTextStream& operator<<(JTextStream& str, const SBandData_Base& sd) { 
// sd.display(str);
//  return str; 
//}



/*! \brief Radiance or irradiance data in a series of bands.

\todo either don't write vector lengths or read them in when reading FixedSize.
*/

template <typename DT>
class SBandData_Imp : public SBandData_Base {

public:

  typedef typename DT::elem_type elem_type;
  typedef DT policy_type;

private:

  vector<typename DT::elem_type> data;

public:

  static const SBandData_Imp<DT> null_ret;

  SBandData_Imp() { }
  SBandData_Imp(int n) { data = scalar_vector<elem_type>(n, 0); }
  SBandData_Imp(int n, elem_type d) { data = scalar_vector<elem_type>(n, d); }
  SBandData_Imp(const vector<elem_type>& d) : data(d) { } 

  SBandData_Imp(const SBandData_Imp& sd, int use_band_start, int use_band_count) { 
    data = boost::numeric::ublas::project(sd.data, range(use_band_start, use_band_start + use_band_count)); }

  ~SBandData_Imp() { }

    // future versions will require factory built spectral data
  //const JString& factoryID() const { return JFactory< SBandData_Base, SBandData_Imp<DT> >::instance().id(); }

    // the factory ID needed to reconstruct this object on loading
  virtual const JString factoryID() const { return staticFactoryID(); }
    // safe to assume DT is not a virtual type
  static const JString staticFactoryID() { return "sbd_"+DT::staticFactoryID(); }
 
    /*! \brief Returns a reference to a zero size spectral band data object. Useful for null reference function returns. */ 
  static const SBandData_Imp<DT>& null() { return null_ret; }

  int bandCount() const { return data.size(); }

  elem_type& bandValue(int i) { return data(i); }
  const elem_type& bandValue(int i) const { return data(i); }

  elem_type maximumBandValue() const { return norm_max(data); }
  elem_type sumOfBandValues() const { return norm_1(data); }
 
  const vector<elem_type>& vectorData() const { return data; }
 
  elem_type project(const SBandData_Imp& f) const { return dotProduct(data, f.data); }
  
  void expandBandCount(int bc) { (*this) = SBandData(bc, data(0)); }

  void resample(const BandSpec& targ_bs, const BandSpec& src_bs) { (*this) = targ_bs.resampleToThis(*this, src_bs); }
  
  bool allZero() const { 
    for (unsigned int i=0; i<data.size(); i++) if (data(i)!=0) return false;
    return true;
  }

  void copy(const SBandData_Imp<DT>& sd) { 
    data.resize(sd.data.size()); 
    data = sd.data; 
  }

  void copyRange(const SBandData_Imp<DT>& sd, int band_start, int band_end) { 
    range r(band_start, band_end);
    boost::numeric::ublas::project(data, r) = boost::numeric::ublas::project(sd.data, r); 
  }

  void add(const SBandData_Imp<DT>& sd) { 
    if (data.size() != sd.data.size()) { data.resize(sd.data.size()); data = scalar_vector<elem_type>(data.size(), 0); }
    data += sd.data; 
  }

  void subtract(const SBandData_Imp<DT>& sd) { data -= sd.data; }
  void multiply(const SBandData_Imp<DT>& sd) { data = element_prod(data, sd.data); }
  void divide(const SBandData_Imp<DT>& sd) { data = element_div(data, sd.data); }
  void divideExceptZeros(const SBandData_Imp<DT>& sd) { for (unsigned int i=0; i<data.size(); i++) if (sd.data(i)!=0) data(i) /= sd.data(i); }
  void multiply(double f) { data *= f; }
  void divide(double f) { data /= f; }
  void negate() { data = -data; }

  elem_type& operator[](int i) { return data(i); }
  const elem_type& operator[](int i) const { return data(i); }

  const SBandData_Imp<DT>& operator=(const SBandData_Imp<DT>& sd) { copy(sd); return *this; }

  const SBandData_Imp<DT>& operator+=(const SBandData_Imp<DT>& sd) { add(sd); return *this; }
  const SBandData_Imp<DT>& operator-=(const SBandData_Imp<DT>& sd) { subtract(sd); return *this; }

  const SBandData_Imp<DT>& operator*=(const SBandData_Imp<DT>& rhs) { multiply(rhs); return *this; }
  const SBandData_Imp<DT>& operator/=(const SBandData_Imp<DT>& rhs) { divide(rhs); return *this; }

  const SBandData_Imp<DT>& operator*=(double f) { multiply(f); return *this; }
  const SBandData_Imp<DT>& operator/=(double f) { divide(f); return *this; }
 
  const SBandData_Imp<DT> operator*(double f) const { 
    SBandData_Imp<DT> sd(*this); 
    sd.multiply(f);
    return sd; 
  }

  const SBandData_Imp<DT> operator/(double f) const { 
    SBandData_Imp<DT> sd(*this); 
    sd.divide(f);
    return sd; 
  }
 
  template <typename DT2>
  friend const SBandData_Imp<DT2> operator+(const SBandData_Imp<DT2>& lhs, const SBandData_Imp<DT2>& rhs);
  template <typename DT2>
  friend const SBandData_Imp<DT2> operator-(const SBandData_Imp<DT2>& lhs, const SBandData_Imp<DT2>& rhs);
  template <typename DT2>
  friend const SBandData_Imp<DT2> operator*(const SBandData_Imp<DT2>& lhs, const SBandData_Imp<DT2>& rhs);
  template <typename DT2>
  friend const SBandData_Imp<DT2> operator/(const SBandData_Imp<DT2>& lhs, const SBandData_Imp<DT2>& rhs);

  template <typename DT2>
  friend const SBandData_Imp<DT2> sqrt(const SBandData_Imp<DT2>& lhs);

  template <typename DT2>
  friend JTextStream& jude::base::operator<<(JTextStream& str, const SBandData_Imp<DT2>& rhs);

    /*! Reads the spectral data from a text stream. Returns true no error occured. */
  bool read(JTextStream& str) { return jude::math::readFromTextStream(data, str); }
    /*! Writes the spectral data to a text stream. Returns true no error occured. */
  bool write(JTextStream& str) const { 
    return jude::math::writeToTextStream(data, str, ", ", ","); 
  }
    /*! Writes the spectral data to a text stream suing sepetators \a band_sep and \a sub_sep. Returns true no error occured. */
  bool write(JTextStream& str, const JString& band_sep, const JString& sub_sep) const { 
    return jude::math::writeToTextStream(data, str, band_sep, sub_sep); 
  }

    /*! Reads the spectral data from a data stream. Note the spectral data must have previously been sized to the correct number of bands.
      Returns true if no error occured. */
  bool read(JDataStream& str, uns32 saved_version) { 
    if (saved_version < VERSION_CODE(2, 1, 0)) {
      int32 n;
      str >> n;
      data.resize(n);
    }
    for (unsigned int i=0; i<data.size(); i++) str >> data(i);
    return true; 
  }

    /*! Writes the spectral data to a data stream. Note does not include the number of bands. Returns true if no error occured. */
  bool write(JDataStream& str) const {
    for (unsigned int i=0; i<data.size(); i++) str << data(i);
    return true;
  }

  void display(JTextStream& str) const {
    str << data;
  }
};



  // for null return
template <typename DT>
const SBandData_Imp<DT> SBandData_Imp<DT>::null_ret(0);



template <typename DT>
inline const SBandData_Imp<DT> operator+(const SBandData_Imp<DT>& lhs, const SBandData_Imp<DT>& rhs) { 
  SBandData_Imp<DT> sd(lhs);
  sd.add(rhs);
  return sd;
}



template <typename DT>
inline const SBandData_Imp<DT> operator-(const SBandData_Imp<DT>& lhs, const SBandData_Imp<DT>& rhs) { 
  SBandData_Imp<DT> sd(lhs);
  sd.subtract(rhs);
  return sd;
}



template <typename DT>
inline const SBandData_Imp<DT> operator*(const SBandData_Imp<DT>& lhs, const SBandData_Imp<DT>& rhs) { 
  SBandData_Imp<DT> sd(lhs);
  sd.multiply(rhs);
  return sd;
}



template <typename DT>
inline const SBandData_Imp<DT> operator/(const SBandData_Imp<DT>& lhs, const SBandData_Imp<DT>& rhs) { 
  SBandData_Imp<DT> sd(lhs);
  sd.divide(rhs);
  return sd;
}



template <typename DT>
inline const SBandData_Imp<DT> sqrt(const SBandData_Imp<DT>& lhs) { 
  SBandData_Imp<DT> sd(lhs);
  for (int i=0; i<sd.bandCount(); i++) {
    typename SBandData_Imp<DT>::elem_type v(sd.data(i));
    sd.data(i) = std::sqrt(v);
  }
  return sd;
}



/*
class SBandData : public SBandData_Imp<datatype_s> {
public:
  const JString factoryID() const { return "sbanddata_s"; }
}



*/



typedef SBandData_Imp<PolicyS> SBandData;




//inline void writeToTextStream(const datatype_s& d, JTextStream& str, const JString& sub_sep) {
// str << d;
//}



} } // namespace jude::rad




namespace jude {
namespace base {

template <typename DT>
inline JTextStream& operator<<(JTextStream& str, const jude::rad::SBandData_Imp<DT>& rhs) { 
  rhs.write(str);
  return str;
}

} }

#endif // _JUDE_RAD_SBANDDATA_H

