
// raddirecdirec.h

#ifndef _JUDE_RAD_RADDIRECDIREC_H
#define _JUDE_RAD_RADDIRECDIREC_H

#include "base/jtypes.h"
#include "rad/radtypes.h"
#include "rad/bandspec.h"
#include "ds/direcstruct.h"
#include "base/jlog.h"
#include "base/jerror.h"

using namespace jude::base;
using namespace jude::ds;

//class jude::base::JDataStream;

namespace jude {
namespace rad {



// formerly Spec

/*! Interface for all classes that store directional-to-directional functions as derived classes of SBandData_Base. */

class RadDirecDirec {

public:

  typedef enum { UNSPECIFIED=0, MEAN_RAD=1, PLANAR_IRRAD=2, SCALAR_IRRAD=3 } datatype;

protected:

  enum { TYPE_BITS=8 };

  int band_count;

  datatype in_data_type;
  datatype ex_data_type;

  void setBandCount(int bc) { band_count = bc; } 

  void changeInDataType(datatype p, datatype n) { in_data_type = (in_data_type==p) ? n : UNSPECIFIED; }
  void changeExDataType(datatype p, datatype n) { ex_data_type = (ex_data_type==p) ? n : UNSPECIFIED; }
 
public:

  RadDirecDirec() : band_count(0), in_data_type(UNSPECIFIED), ex_data_type(UNSPECIFIED) { }
  RadDirecDirec(int bc) : band_count(bc), in_data_type(UNSPECIFIED), ex_data_type(UNSPECIFIED) { }
  RadDirecDirec(int bc, datatype idt, datatype edt) : band_count(bc), in_data_type(idt), ex_data_type(edt) { }
  RadDirecDirec(const RadDirecDirec& rdd) : band_count(rdd.band_count), in_data_type(rdd.in_data_type), ex_data_type(rdd.ex_data_type) { }

  virtual ~RadDirecDirec() { }

  virtual const JString factoryID() const = 0;

  virtual const DirecStruct& direcStruct() const = 0;

  virtual RadDirecDirec* duplicate() const = 0;
  virtual RadDirecDirec* duplicateStructure() const = 0;

    /*! \brief Returns the number of spectral bands of the data.*/
  int bandCount() const { return band_count; } 

    /*! \brief Interface to force recalculation of any internally stored derived values when they are next requested. */
  virtual void lazyInvalidate() const = 0;

  virtual void expandBandCount(int bc) = 0;

  virtual void resample(const BandSpec& targ_bs, const BandSpec& src_bs) = 0;

  //virtual const SBandData_Base at(const Vec3D& in_direc, const Vec3D& ex_direc) const = 0;

    /* \brief Interface to multiply all stored data by a single value. */
  virtual void multiply(basic_radtype f) = 0;
    /* \brief Interface to divide all stored data by a single value. */
  virtual void divide(basic_radtype f) = 0;

  /* \brief Interface for a function which copies the directional partition structure and stored data from another RadDirecDirec type.
    Derived classes should verify that the supplied object is of an appropriate polymorphic 
    type and stream a message to jlog::es and call internalError() if not. 
    Derived classes should also call this base implementation of this function since it performs
    operations such as tracking the stored data type. */
  virtual void copy(const RadDirecDirec& r) { 
    in_data_type = r.in_data_type; 
    ex_data_type = r.ex_data_type; 
  }

  /* \brief Interface for a function which adds the stored data from another RadDirecDirec type onto this one.
    Derived classes should verify that the supplied object is of an appropriate polymorphic 
    type and stream a message to jlog::es and call internalError() if not. 
    Derived classes should also call this base implementation of this function since it performs
    operations such as tracking the stored data type. */
  virtual void add(const RadDirecDirec& r) { 
    if (!sameDataType(*this, r)) {
      in_data_type = UNSPECIFIED; 
      ex_data_type = UNSPECIFIED;
    }
  }

  /* \brief Interface for a function which squares the stored data from another RadDirecDirec type and adds it onto this one.
    This function is useful for accumulating sums of squares for calculating standard deviations and such like.
    Squaring is acheived by operator*() so the stored type must provide that function.
    Derived classes should verify that the supplied object is of an appropriate polymorphic 
    type and stream a message to jlog::es and call internalError() if not. 
    Derived classes should also call this base implementation of this function since it performs
    operations such as tracking the stored data type. */
  virtual void addSquared(const RadDirecDirec& r) { 
    in_data_type = UNSPECIFIED; 
    ex_data_type = UNSPECIFIED;
  }

  /* \brief Interface for a function which adds the stored data from another RadDirecDirec type onto this one.
    Derived classes should verify that the supplied object is of an appropriate polymorphic 
    type and stream a message to jlog::es and call internalError() if not. 
    Derived classes should also call this base implementation of this function since it performs
    operations such as tracking the stored data type. */
  virtual void subtract(const RadDirecDirec& r) {
    if (!sameDataType(*this, r)) {
      in_data_type = UNSPECIFIED; 
      ex_data_type = UNSPECIFIED;
    }
  }

  /* \brief Interface for a function which squares the stored data from another RadDirecDirec type and subtracts it from this one.
    This function is useful for calculating standard deviations and such like.
    Squaring is acheived by operator*() so the stored type must provide that function.
    Derived classes should verify that the supplied object is of an appropriate polymorphic 
    type and stream a message to jlog::es and call internalError() if not. 
    Derived classes should also call this base implementation of this function since it performs
    operations such as tracking the stored data type. */
  virtual void subtractSquared(const RadDirecDirec& r) { 
    in_data_type = UNSPECIFIED; 
    ex_data_type = UNSPECIFIED;
  }

  /* \brief Interface for a function which performs square root on the stored data.
    This function is useful for calculating standard deviations and such like.
    Square root is achieved by sqrt(obj) so the stored type obj, so sucha function must exist.
    Derived classes should call this base implementation of this function since it performs
    operations such as tracking the stored data type. */
  virtual void squareRoot() { 
    in_data_type = UNSPECIFIED; 
    ex_data_type = UNSPECIFIED;
  }

    /*! \brief Interface for a function which multiplies all data entries by the solid angle of their corresponding incident quad. */
  virtual void multiplyAllByInSA() = 0;
    /*! \brief Interface for a function which divides all data entries by the solid angle of their corresponding incident quad. */
  virtual void divideAllByInSA() = 0;

    /*! \brief Interface for a function which multiplies all data entries by the solid angle times cos(theta) of their corresponding incident quad. */
  virtual void multiplyAllByInSACT() = 0;
    /*! \brief Interface for a function which divides all data entries by the solid angle times cos(theta) of their corresponding incident quad. */
  virtual void divideAllByInSACT() = 0;

    /*! \brief Interface for a function which multiplies all data entries by the solid angle of their corresponding exitant quad. */
  virtual void multiplyAllByExSA() = 0;
    /*! \brief Interface for a function which multiplies all data entries by the solid angle of their corresponding exitant quad. */
  virtual void divideAllByExSA() = 0;

    /*! \brief Interface for a function which multiplies all data entries by the solid angle times cos(theta) of their corresponding exitant quad. */
  virtual void multiplyAllByExSACT() = 0;
    /*! \brief Interface for a function which divides all data entries by the solid angle times cos(theta) of their corresponding exitant quad. */
  virtual void divideAllByExSACT() = 0;

  // the below may be implemented one day

  // virtual void multiplyAllByInCT() = 0;
  // virtual void divideAllByInCT() = 0;
 
  // virtual void multiplyAllByExCT() = 0;
  // virtual void divideAllByExCT() = 0;
  // virtual void multiplyAllByExSACT() = 0;
  // virtual void divideAllByExSACT() = 0;

  void setInDataTypeUnspecified() { in_data_type = UNSPECIFIED; }
  void setInDataTypeMeanRad() { in_data_type = MEAN_RAD; }
  void setInDataTypePlanarIrrad() { in_data_type = PLANAR_IRRAD; }
  void setInDataTypeScalarIrrad() { in_data_type = SCALAR_IRRAD; }
 
  void setExDataTypeUnspecified() { ex_data_type = UNSPECIFIED; }
  void setExDataTypeMeanRad() { ex_data_type = MEAN_RAD; }
  void setExDataTypePlanarIrrad() { ex_data_type = PLANAR_IRRAD; }
  void setExDataTypeScalarIrrad() { ex_data_type = SCALAR_IRRAD; }
 
  bool inDataTypeIsUnspecified() const { return in_data_type == UNSPECIFIED; }
  bool inDataTypeIsMeanRad() const { return in_data_type == MEAN_RAD; }
  bool inDataTypeIsScalarIrrad() const { return in_data_type == SCALAR_IRRAD; }
  bool inDataTypeIsPlanarIrrad() const { return in_data_type == PLANAR_IRRAD; }
 
  bool exDataTypeIsUnspecified() const { return ex_data_type == UNSPECIFIED; }
  bool exDataTypeIsMeanRad() const { return ex_data_type == MEAN_RAD; }
  bool exDataTypeIsScalarIrrad() const { return ex_data_type == SCALAR_IRRAD; }
  bool exDataTypeIsPlanarIrrad() const { return ex_data_type == PLANAR_IRRAD; }

  // depreceated
  void copyDataTypes(const RadDirecDirec& rdd) { in_data_type=rdd.in_data_type; ex_data_type=rdd.ex_data_type; }

  void copyDataType(const RadDirecDirec& rdd) { in_data_type=rdd.in_data_type; ex_data_type=rdd.ex_data_type; }

    /*! \brief Returns true if the incident data types of the two RadDirecDirec objects are the same. */
  static bool sameInDataType(const RadDirecDirec& r1, const RadDirecDirec& r2) { return (r1.in_data_type == r2.in_data_type); }
    /*! \brief Returns true if the exititant data types of the two RadDirecDirec objects are the same. */
  static bool sameExDataType(const RadDirecDirec& r1, const RadDirecDirec& r2) { return (r1.ex_data_type == r2.ex_data_type); }
    /*! \brief Returns true if the incident and exititant data types of the two RadDirecDirec objects are the same. */
  static bool sameDataType(const RadDirecDirec& r1, const RadDirecDirec& r2) { return (sameInDataType(r1,r2) && sameExDataType(r1,r2)); }

    /*! \brief Interface for checking if this object and \a rhs have the same polymorphic type and directional partition structure. */ 
  virtual bool checkCompatible(const RadDirecDirec& rhs) const = 0;

  const JString inDataString() const { 
    switch (in_data_type) {
      case MEAN_RAD : return "quad-averaged radiance";
      case SCALAR_IRRAD : return "scalar irradiance";
      case PLANAR_IRRAD : return "planar irradiance";
      default : return "unspecified";
    }
  }

  const JString exDataString() const { 
    switch (ex_data_type) {
      case MEAN_RAD : return "quad-averaged radiance";
      case SCALAR_IRRAD : return "scalar irradiance";
      case PLANAR_IRRAD : return "planar irradiance";
      default : return "unspecified";
    }
  }

  virtual bool read(JDataStream& str, uns32 saved_version);
  virtual bool write(JDataStream& str) const;

  bool read(JTextStream& str, uns32 saved_version) { 
    jlog::ls << "RadDirecDirec::read(JTextStream& str, uns32 saved_version) not implemented.\n";
    internalError();
    return true; 
  }

  bool write(JTextStream& str) const { 
    jlog::ls << "RadDirecDirec::write(JTextStream& str) not implemented.\n";
    internalError();
    return true; 
  }

  virtual void display(JTextStream& str) const { }
};



} } // namespace jude::rad

#endif // _JUDE_RAD_RADDIRECDIREC_H

