
#include "direcstruct.h"
#include "base/jlog.h"

using namespace jude::base;
using namespace jude::ds;



/*! \todo Generalise output to JTextStream with jlog::ls as default */

void DirecStruct::display() const {

  for (int i=0;i<units();i++) {

    double sa = solidAngle(i);
    double ct = meanCosTheta(i);
    double sact = solidAngleAbsCosTheta(i);
    double mcd = meanCubeDistance(i);
    double pcd = pointCubeDistance(i);
    Vec3D v(indexToPointDirec(i));

    jlog::ls.printf("%i: sa %f ct %f sact %f (calc %f) mcd %f pcd %f ",i,sa,ct,sact,fabs(sa*ct),mcd,pcd);
    jlog::ls.printf("pd: [ x %f y %f z %f ]\n",v.x(),v.y(),v.z());
  }
}

