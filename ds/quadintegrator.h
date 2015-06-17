
// quadintegrator.h

#ifndef _JUDE_DS_QUADINTEGRATOR_H
#define _JUDE_DS_QUADINTEGRATOR_H

#include "S3/vec3d.h"

namespace jude {
namespace ds {

using namespace jude::S3;



/*! \brief Interface for classes that can perform integation over a quad. */
 
class QuadIntegrator {

public:

  virtual ~QuadIntegrator() { }

  virtual bool atEnd() const = 0;
  virtual void setSubSolidAngle(double sa) = 0;
  virtual int subQuadCount() const = 0;
  virtual void reset() = 0;
  virtual void next() = 0;

  virtual const Vec3D& direction() const = 0;

  virtual double cosTheta() const = 0;
  virtual double solidAngle() const = 0;
  virtual double solidAngleAbsCosTheta() const = 0;
};



} } // namespace jude::ds

#endif // _JUDE_DS_QUADINTEGRATOR_H

