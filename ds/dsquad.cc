
// dsquad.cc

#include "dsquad.h"
#include "math/mathutil.h"

using namespace jude::ds;
using namespace jude::math;



/*! \brief Calculates the projected position of a vector starting at the origin onto the axis-aligned cube centred at the origin with sides of length 2. */

Vec3D DsQuad::cubeProject(const Vec3D& v) const {

  Vec3D pv;

  double isec_direc = fabs(v.x()) > fabs(v.y()) ? v.x() : v.y();
  if (fabs(isec_direc) < fabs(v.z())) isec_direc = v.z();

  pv = v / fabs(isec_direc);

  return pv;
}



bool DsQuad::display(JTextStream& str, int index, int code) const {

  switch (code) {

    case DsQuad::SA : str << solidAngle(); return true;
    case DsQuad::SACT : str << solidAngleAbsCosTheta(); return true;
    case DsQuad::CT : str << meanCosTheta(); return true;
    case DsQuad::INDEX : str << index; return true;
    case DsQuad::POINTDIREC : {
      Vec3D v(pointDirec()); 
      str << v.x() << "," << v.y() << "," << v.z(); 
      return true;
    }
    case DsQuad::MEANDIREC : {
      Vec3D v(meanDirec()); 
      str << v.x() << "," << v.y() << "," << v.z(); 
      return true;
    }
    case DsQuad::MEANTHETA : str << radToDeg(meanTheta()); return true;
    case DsQuad::MEANPHI : str << radToDeg(meanPhi()); return true;
    case DsQuad::POINTTHETA : str << radToDeg(pointTheta()); return true;
    case DsQuad::POINTPHI : str << radToDeg(pointPhi()); return true;
  }

  return false;
}

