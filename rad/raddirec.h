
// raddirec.h

#ifndef _JUDE_RAD_RADDIREC_H
#define _JUDE_RAD_RADDIREC_H

#include "base/jtypes.h"
#include "rad/radtypes.h"
#include "rad/bandspec.h"
#include "ds/direcstruct.h"
#include "base/jlog.h"
#include "base/jerror.h"

using namespace jude::base;
using namespace jude::ds;
using namespace jude::rad;

//class jude::ds::DirecStruct;
//class jude::base::JDataStream;

namespace jude {
namespace rad {



// formerly Spec

/*! Interface for all classes that store directional tables of derived classes of SBandData_Base. */

class RadDirec {

public:

  typedef enum { UNSPECIFIED=0, MEAN_RAD=1, PLANAR_IRRAD=2, SCALAR_IRRAD=3 } datatype;

protected:

  enum { TYPE_BITS=8 };

  int band_count;

  datatype data_type;

  void setBandCount(int bc) { band_count = bc; } 

  void changeDataType(datatype p, datatype n) { data_type = (data_type==p) ? n : UNSPECIFIED; }
  
public:

  RadDirec() : band_count(0), data_type(UNSPECIFIED) { }
  RadDirec(int bc) : band_count(bc), data_type(UNSPECIFIED) { }
  RadDirec(int bc, datatype d) : band_count(bc), data_type(d) { }
  RadDirec(const RadDirec& rd) : band_count(rd.band_count), data_type(rd.data_type) { }

  virtual ~RadDirec() { }

  virtual const JString factoryID() const = 0;

  virtual const DirecStruct& direcStruct() const = 0;

  virtual RadDirec* duplicate() const = 0;
  virtual RadDirec* duplicateStructure() const = 0;

    /*! \brief Returns the number of spectral bands of the data.*/
  int bandCount() const { return band_count; } 

    /*! \brief Interface to force recalculation of any internally stored derived values when they are next requested. */
  virtual void lazyInvalidate() const = 0; 

  virtual void expandBandCount(int bc) = 0;

  virtual void resample(const BandSpec& targ_bs, const BandSpec& src_bs) = 0;
 
    /* \brief Interface to multiply all stored data by a single value. */
  virtual void multiply(basic_radtype f) = 0;
    /* \brief Interface to divide all stored data by a single value. */
  virtual void divide(basic_radtype f) = 0;

  /* \brief Interface to copy the directional partition structure and stored data from another RadDirec type.
    Derived classes should verify that the supplied object is of an appropriate polymorphic 
    type and stream a message to jlog::es and call internalError() if not. 
    Derived classes should also call this base implementation of this function since it performs
    operations such as tracking the stored data type. */
  virtual void copy(const RadDirec& r) {
    data_type = r.data_type; 
  }

  /* \brief Interface to add the stored data from another RadDirec type onto this one.
    Derived classes should verify that the supplied object is of an appropriate polymorphic 
    type and stream a message to jlog::es and call internalError() if not. 
    Derived classes should also call this base implementation of this function since it performs
    operations such as tracking the stored data type. */
  virtual void add(const RadDirec& r) { 
    if (!sameDataType(*this, r)) data_type = UNSPECIFIED; 
  }

  virtual void addMultiple(const RadDirec& r, double m) { 
    if (!sameDataType(*this, r)) data_type = UNSPECIFIED; 
  }

  /* \brief Interface to subtract the stored data from another RadDirec type onto this one.
    Derived classes should verify that the supplied object is of an appropriate polymorphic 
    type and stream a message to jlog::es and call internalError() if not. 
    Derived classes should also call this base implementation of this function since it performs
    operations such as tracking the stored data type. */
  virtual void subtract(const RadDirec& r) {
    if (!sameDataType(*this, r)) data_type = UNSPECIFIED; 
  }
 
  virtual void multiplyAllBySA() = 0;
  virtual void multiplyAllByCT() = 0;
  virtual void multiplyAllBySACT() = 0;

  //virtual void divideAllBySA() = 0;
  virtual void divideAllByCT() = 0;
  virtual void divideAllBySACT() = 0;

  void setDataTypeUnspecified() { data_type = UNSPECIFIED; }
  void setDataTypeMeanRad() { data_type = MEAN_RAD; }
  void setDataTypePlanarIrrad() { data_type = PLANAR_IRRAD; }
  void setDataTypeScalarIrrad() { data_type = SCALAR_IRRAD; }
 
  void copyDataType(const RadDirec& rd) { data_type = rd.data_type; }

  bool dataTypeIsUnspecified() const { return data_type == UNSPECIFIED; }
  bool dataTypeIsMeanRad() const { return data_type == MEAN_RAD; }
  bool dataTypeIsScalarIrrad() const { return data_type == SCALAR_IRRAD; }
  bool dataTypeIsPlanarIrrad() const { return data_type == PLANAR_IRRAD; }

    /*! \brief Returns true if the data types of the two RadDirec objects are the same. */
  static bool sameDataType(const RadDirec& r1, const RadDirec& r2) { return (r1.data_type == r2.data_type); }
 
    /*! \brief Interface for checking if this object and \a rhs have the same polymorphic type and directional partition structure. */ 
  virtual bool checkCompatible(const RadDirec& rhs) const = 0;
  
  const JString dataString() const { 
    switch (data_type) {
      case MEAN_RAD : return "quad-averaged radiance";
      case SCALAR_IRRAD : return "scalar irradiance";
      case PLANAR_IRRAD : return "planar irradiance";
      default : return "unspecified";
    }
  }

  virtual bool read(JDataStream& str, uns32 saved_version);
  virtual bool write(JDataStream& str) const;

  bool read(JTextStream& str, uns32 saved_version) { 
    jlog::ls << "RadDirec::read(JTextStream& str, uns32 saved_version) not implemented.\n";
    internalError();
    return true; 
  }

  bool write(JTextStream& str) const { 
    jlog::ls << "RadDirec::write(JTextStream& str) not implemented.\n";
    internalError();
    return true; 
  }
};



} } // namespace jude::rad

#endif // _JUDE_RAD_RADDIREC_H

