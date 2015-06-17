
// dsquad.h

#ifndef _JUDE_DS_DSQUAD_H
#define _JUDE_DS_DSQUAD_H

#include "S3/vec3d.h"
#include "base/jtextstream.h"

namespace jude {
namespace ds {

using namespace jude::base;
using namespace jude::S3;

class QuadIntegrator;



/*! \brief Base class for quads used in directional partitions. 

Each directional partition quad has four corners, numbered 0 to 3, which are expressed as directional unit vectors starting at the origin. Each quad also has two other ways of expressing the direction it represents. \n

The below is not correct\n

First the function pointDirec() returns the unit vector which starts at the origin and passes through a point in the quad intended to best represent the direction the quad represents, the 'quad direction point'. 
Secondly the function meanDirec() returns a vector which actually represents the solid angle centroid of the quad. Note that meanDirec() and pointDirec() may or may not be the same, dependent on the partitioning scheme used. 
The reason for the two schemes is that for directional partitioning based on constant zenth and azimuth angles the natural direction to ascribe to the quad is the mid-zenith and mid-azimuth angle. 
However this is not the same as the solid angle centroid of the quad. In other situations, when dealing with functions integrated over solid angle for example, it is more desirable to consider the mean direction over the solid angle of a quad. 
These considerations become even more important in the 3D model decribed in Hedley (2008) where the partitioning is not based on constant azimuth and zenith angles and subsampling of functions within quads is required.
*/

class DsQuad {

protected:

  Vec3D corn0;
  Vec3D corn1;
  Vec3D corn2;
  Vec3D corn3;

  Vec3D cubeProject(const Vec3D& v) const;

public:

    // things that can be displayed by display() function as bits
  enum { SA=1, SACT=(1<<1), CT=(1<<2), INDEX=(1<<3), POINTDIREC=(1<<4), MEANDIREC=(1<<5), MEANTHETA=(1<<6), MEANPHI=(1<<7), POINTTHETA=(1<<8), POINTPHI=(1<<9) };

  virtual ~DsQuad() { }
 
    /*! \brief Unit vector that goes through the solid angle centroid of the quad. */
  virtual const Vec3D& meanDirec() const = 0;
    /*! \brief Unit vector that expresses the approximate direction represented by the quad. */
  virtual const Vec3D& pointDirec() const = 0;

  virtual double meanTheta() const = 0;
  virtual double meanPhi() const = 0;

  virtual double pointTheta() const = 0;
  virtual double pointPhi() const = 0;

    /*! \brief Unit vector that goes through corner 0 of the quad. */
  const Vec3D& corner0() const { return corn0; }
    /*! \brief Unit vector that goes through corner 1 of the quad. */
  const Vec3D& corner1() const { return corn1; }
    /*! \brief Unit vector that goes through corner 2 of the quad. */
  const Vec3D& corner2() const { return corn2; }
    /*! \brief Unit vector that goes through corner 3 of the quad. */
  const Vec3D& corner3() const { return corn3; }

    /*! \brief Vector that goes through corner 0 of the quad projected onto the origin centred cube of side length 2. */
  const Vec3D cubeProjectCorner0() const { return cubeProject(corn0); }
    /*! \brief Vector that goes through corner 1 of the quad projected onto the origin centred cube of side length 2. */
  const Vec3D cubeProjectCorner1() const { return cubeProject(corn1); }
    /*! \brief Vector that goes through corner 2 of the quad projected onto the origin centred cube of side length 2. */
  const Vec3D cubeProjectCorner2() const { return cubeProject(corn2); }
    /*! \brief Vector that goes through corner 3 of the quad projected onto the origin centred cube of side length 2. */
  const Vec3D cubeProjectCorner3() const { return cubeProject(corn3); }

    /*! \brief Returns a vector that goes through solid angle centroid of the quad projected onto the origin centred cube of side length 2. */
  const Vec3D cubeProjectMeanDirecX() const { return cubeProject(meanDirec()); }
    /*! \brief Returns a vector that goes through quad direction point projected onto the origin centred cube of side length 2. */
  const Vec3D cubeProjectPointDirecX() const { return cubeProject(pointDirec()); }
 
  virtual double meanCosTheta() const = 0;
  virtual double solidAngle() const = 0;
  virtual double solidAngleAbsCosTheta() const = 0;
  virtual double meanCubeDistance() const = 0;
  virtual double pointCubeDistance() const = 0;

    /*! \brief The subclass should return a random vector direction within the quad with a flat distribution over solid angle. */
  virtual Vec3D randomDirec() const = 0; 

  virtual QuadIntegrator* createIntegrator(double sa=0) const = 0;

  void read(JDataStream& str) {
    str >> corn0;
    str >> corn1;
    str >> corn2;
    str >> corn3;
  }

  void write(JDataStream& str) const {
    str << corn0;
    str << corn1;
    str << corn2;
    str << corn3;
  }

  virtual bool display(JTextStream& str, int index, int code) const;
};



} } // namespace jude::ds

#endif // _JUDE_DS_DSQUAD_H

