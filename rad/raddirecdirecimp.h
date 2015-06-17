
// raddirecdirecimp.h

#ifndef _JUDE_RAD_RADDIRECDIRECIMP_H
#define _JUDE_RAD_RADDIRECDIRECIMP_H

#include "rad/raddirecdirec.h"
#include "table/direcdirectablebase.h"
#include "S3/vec3d.h"
#include "base/jlog.h"
#include "base/jerror.h"
#include "rad/rdd_matrixdft.h"
#include "ds/quaddatasource.h"
#include "ds/spherepartition.h"

using namespace jude::base;
using namespace jude::S3;
using namespace jude::table;

namespace jude {
namespace rad {

template <typename A>
class RadDirecImp;



/*! \brief Base class for specific directional implementations of the RadImp Interface.

   Type A must always be a derived class of SBandData_Base.
*/

template <typename A>
class RadDirecDirecImp : public RadDirecDirec { 

public:

    /*! Constructor for unspecifed incident and exitant data types. */
  RadDirecDirecImp<A>() { }
    /*! Constructor for unspecifed data type. */
  RadDirecDirecImp<A>(int bc) : RadDirecDirec(bc) { }
    /*! Constructor which sets the specifed incident and exitant data types to \a idt and \a edt respectively. */
  RadDirecDirecImp<A>(int bc, datatype idt, datatype edt) : RadDirecDirec(bc, idt, edt) { }
    /*! Constructor which copies the data type of \a rdd. */
  RadDirecDirecImp<A>(const RadDirecDirecImp<A>& rdd) : RadDirecDirec(rdd) { }

    /*! \brief Implementations must provide a function which creates a copy of structure and data.*/
  virtual RadDirecDirecImp<A>* duplicate() const = 0;
    /*! \brief Implementations must provide a function which creates a copy of structure only, data undefined. */
  virtual RadDirecDirecImp<A>* duplicateStructure() const = 0;
    /*! \brief Implementations must provide a function which creates a copy of structure and the data type only, data undefined. */
  virtual RadDirecDirecImp<A>* duplicateStructureAndDataType() const = 0;
    /*! \brief Implementations must provide a function which creates a half version of structure only, data undefined. */
  virtual RadDirecDirecImp<A>* duplicateHalfStructure() const = 0;

    /*! \brief Implementations must provide a function which creates a table with this objects directional structure. */
  virtual RadDirecImp<A>* createDirec() const = 0;

  virtual const DirecDirecTableBase<A>& tableData() const = 0;

  //virtual void copyFromMatrixDFTSym(const RDD_MatrixDFT& rdd_mat) = 0;
  virtual void copyToMatrixDFT(RDD_MatrixDFT& rdd_mat) const = 0;
  virtual void copyToMatrixDFTSym(RDD_MatrixDFT& rdd_mat) const = 0;

  virtual void makeAll(const A& sd) = 0;
  virtual void makeZero() = 0;

  virtual void multiplyAll(const A& m) = 0;
  virtual void divideAll(const A& m) = 0;

  virtual void multiplyQuadrants(double zp_zp, double zp_zm, double zm_zp, double zm_zm) = 0;

  virtual void setData(int in_index, int ex_index, const A& sd) = 0;

  virtual void addToData(int in_index, int ex_index, const A& sd) = 0;
 
  void setData(const Vec3D& in_direc, const Vec3D& ex_direc, const A& sd) {
    setData(direcStruct().direcToIndex(in_direc), direcStruct().direcToIndex(ex_direc), sd);
  }

    /*! Returns the function value at the combination of incident and exitant directions \a in_direc and \a ex_direc (both expressed directed outwards). */
    // Note that in many cases this return by value will be optimized out by the return value optimisation
  virtual const A at(const Vec3D& in_direc, const Vec3D& ex_direc) const = 0;

    /*! Returns the function value at the combination of incident and exitant quad indices \a in_index and \a ex_index. */
  virtual const A atIndices(int in_index, int ex_index) const = 0;

    /*! Returns the sum of the stored values over all quad-quad pairs. */
  virtual const A& sumOfEntries() const = 0;

  const A diffuseReflectance() const;
  const A diffuseScatter() const;

  virtual void translateInToOut(const RadDirecImp<A>& rdin, RadDirecImp<A>& rdout) const = 0;

  virtual void extractOutData(int in_index, int rotate_pos, RadDirecImp<A>& out_data) const = 0;
  virtual void addToOutData(int in_index, int rotate_pos, const RadDirecImp<A>& out_data) = 0;
 
  void splitRotateAboutX(RadDirecDirecImp<A>* trans_zp, RadDirecDirecImp<A>* trans_zm, 
			 RadDirecDirecImp<A>* reflec_zp_zm, RadDirecDirecImp<A>* reflec_zm_zp) const;

  void splitNegate(RadDirecDirecImp<A>* trans_zp, RadDirecDirecImp<A>* trans_zm, 
		   RadDirecDirecImp<A>* reflec_zp_zm, RadDirecDirecImp<A>* reflec_zm_zp) const;

  void reverseExitantY();

  //bool azimuthallyAverage();

  void display(JTextStream& str) const;
  void displayInIndex(int in_index, JTextStream& str) const;
};



/*! Returns the diffuse reflectance that this function corresponds to under conditions of diffuse illumination. */

template <typename A>
const A RadDirecDirecImp<A>::diffuseReflectance() const {

  if (in_data_type==UNSPECIFIED || ex_data_type==UNSPECIFIED) {
    jlog::es << "Internal error " << __FILE__ << ":" << __LINE__ << " data type is unspecified.\n";
    internalError();
  }

  const DirecStruct& ds(direcStruct());

    // this will sum outgoing planar irradiance
  A irrad_sum(bandCount(), 0);

  for (int i=0; i<ds.units(); i++) {
    for (int j=0; j<ds.units(); j++) {
     
      const A& v(atIndices(i,j));

        // calculated form the concept of unity radiance from all directions
      switch (COMP(in_data_type, ex_data_type, 8)) {
        case COMP(MEAN_RAD, MEAN_RAD, TYPE_BITS) : irrad_sum += v * ds.solidAngleAbsCosTheta(j); break;
        case COMP(PLANAR_IRRAD, MEAN_RAD, TYPE_BITS) : irrad_sum += v * ds.solidAngleAbsCosTheta(i) * ds.solidAngleAbsCosTheta(j); break;
        case COMP(SCALAR_IRRAD, MEAN_RAD, TYPE_BITS) : irrad_sum += v * ds.solidAngle(i) * ds.solidAngleAbsCosTheta(j); break;
        case COMP(MEAN_RAD, PLANAR_IRRAD, TYPE_BITS) : irrad_sum += v; break;
        case COMP(PLANAR_IRRAD, PLANAR_IRRAD, TYPE_BITS) : irrad_sum += v * ds.solidAngleAbsCosTheta(i); break;
        case COMP(SCALAR_IRRAD, PLANAR_IRRAD, TYPE_BITS) : irrad_sum += v * ds.solidAngle(i); break;
        case COMP(MEAN_RAD, SCALAR_IRRAD, TYPE_BITS) : irrad_sum += v * fabs(ds.meanCosTheta(j)); break;
        case COMP(PLANAR_IRRAD, SCALAR_IRRAD, TYPE_BITS) : irrad_sum += v * ds.solidAngleAbsCosTheta(i) * fabs(ds.meanCosTheta(j)); break;
        case COMP(SCALAR_IRRAD, SCALAR_IRRAD, TYPE_BITS) : irrad_sum += v * ds.solidAngle(i) * fabs(ds.meanCosTheta(j)); break;
      }
    }
  }

    // unity radiance in all directions is pi planar irradiance, so divide to get diffuse reflectance
  irrad_sum /= ((ds.isFull()) ? M_PI * 2 : M_PI);

  return irrad_sum;
}



/*! \brief Returns the diffuse volumetric scatter that this function corresponds to under conditions of diffuse illumination. 

  This is calculated by assuming an incoming radiance that is equal in all directions. The diffuse scatter is then the
  total exitant (outgoing) scalar irradiance divided by the total incident scalar irradiance.
*/

template <typename A>
const A RadDirecDirecImp<A>::diffuseScatter() const {

  if (in_data_type==UNSPECIFIED || ex_data_type==UNSPECIFIED) {
    jlog::es << "Internal error " << __FILE__ << ":" << __LINE__ << " data type is unspecified.\n";
    internalError();
  }

  const DirecStruct& ds(direcStruct());

    // this will sum outgoing scalar irradiance
  A scat_sum(bandCount(), 0);

  for (int i=0; i<ds.units(); i++) {
    for (int j=0; j<ds.units(); j++) {
     
      const A& v(atIndices(i,j));

      //jlog::ls << in_data_type << " " << ex_data_type << " " << v.vectorData() << "\n";

        // calculated form the concept of unity radiance from all directions
        // COMP generates an integer code and is defined in jtypes.h
      switch (COMP(in_data_type, ex_data_type, TYPE_BITS)) {
        case COMP(MEAN_RAD, MEAN_RAD, TYPE_BITS) : scat_sum += v * ds.solidAngle(j); break;
        case COMP(PLANAR_IRRAD, MEAN_RAD, TYPE_BITS) : scat_sum += v * ds.solidAngleAbsCosTheta(i) * ds.solidAngle(j); break;
        case COMP(SCALAR_IRRAD, MEAN_RAD, TYPE_BITS) : scat_sum += v * ds.solidAngle(i) * ds.solidAngle(j); break;
        case COMP(MEAN_RAD, PLANAR_IRRAD, TYPE_BITS) : scat_sum += v / fabs(ds.meanCosTheta(j)); break;
        case COMP(PLANAR_IRRAD, PLANAR_IRRAD, TYPE_BITS) : scat_sum += v * ds.solidAngleAbsCosTheta(i) / fabs(ds.meanCosTheta(j)); break;
        case COMP(SCALAR_IRRAD, PLANAR_IRRAD, TYPE_BITS) : scat_sum += v * ds.solidAngle(i) / fabs(ds.meanCosTheta(j)); break;
        case COMP(MEAN_RAD, SCALAR_IRRAD, TYPE_BITS) : scat_sum += v; break;
        case COMP(PLANAR_IRRAD, SCALAR_IRRAD, TYPE_BITS) : scat_sum += v * ds.solidAngleAbsCosTheta(i); break;
        case COMP(SCALAR_IRRAD, SCALAR_IRRAD, TYPE_BITS) : scat_sum += v * ds.solidAngle(i); break;
      }
    }
  }

    // unity radiance in all directions is 4pi scalar irradiance for a sphere, 2pi for a hemisphere, so divide to get total scatter
  scat_sum /= ds.totalSolidAngle();

  return scat_sum;
}


/*


//  ReflecImp::compare() [static]

template <typename SD>
void ReflecImp<SD>::compare(const ReflecImp<SD>& r0, const ReflecImp<SD>& r1) {

  if (!r0.direcStruct().isEqual(r1.direcStruct())) {
    printf("ReflecImp::compare() r0 and r1 differ in directional structure");
    return;
  }


  const DirecStruct& ds = r0.direcStruct();

  double max_pct_diff = 0;

  for (int in_index=0; in_index<ds.units(); in_index++) {
    for (int out_index=0; out_index<ds.units(); out_index++) {

      SD sd0 = r0.atIndices(in_index, out_index);
      SD sd1 = r1.atIndices(in_index, out_index);

      printf("%d %d\n",in_index,out_index);
      sd0.display();
      sd1.display();
    }
  }

printf("max_pct_diff %f\n",max_pct_diff);
}

*/



template <typename A>
void RadDirecDirecImp<A>::splitNegate(RadDirecDirecImp<A>* f00, RadDirecDirecImp<A>* f01,
				      RadDirecDirecImp<A>* f10, RadDirecDirecImp<A>* f11) const {

  if (f00) { f00->makeAll(A(bandCount(),0)); f00->copyDataTypes(*this); }
  if (f01) { f01->makeAll(A(bandCount(),0)); f01->copyDataTypes(*this); }
  if (f10) { f10->makeAll(A(bandCount(),0)); f10->copyDataTypes(*this); }
  if (f11) { f11->makeAll(A(bandCount(),0)); f11->copyDataTypes(*this); }

     // loop through in directions
  for (int in_index=0; in_index<direcStruct().units(); in_index++) {

      // loop through out directions
    for (int out_index=0; out_index<direcStruct().units(); out_index++) {

      const A& sd = atIndices(in_index, out_index);

      const Vec3D dv_in(direcStruct().indexToMeanDirec(in_index));
      const Vec3D dv_out(direcStruct().indexToMeanDirec(out_index));

      // f00 is the function for photon travel in z- directon that ends up in z+ direction - ie water underside
  
     if (dv_in.z() >= 0) {
	if (dv_out.z() >= 0) { if (f10) f10->setData(dv_in, dv_out, sd); }
	else { if (f11) f11->setData(dv_in, -dv_out, sd); }
      } else {
	if (dv_out.z() >= 0) { if (f00) f00->setData(-dv_in, dv_out, sd); }
	else { if (f01) f01->setData(-dv_in, -dv_out, sd); }
      }
   
      /*
      if (dv_in.z() >= 0) {
	if (dv_out.z() >= 0) { if (f00) f00->setData(dv_in, dv_out, sd); }
	else { if (f01) f01->setData(dv_in, -dv_out, sd); }
      } else {
	if (dv_out.z() >= 0) { if (f10) f10->setData(-dv_in, dv_out, sd); }
	else { if (f11) f11->setData(-dv_in, -dv_out, sd); }
      }
      */
      
    }
  }
}



template <typename A>
void RadDirecDirecImp<A>::splitRotateAboutX(RadDirecDirecImp<A>* f00, RadDirecDirecImp<A>* f01,
				 RadDirecDirecImp<A>* f10, RadDirecDirecImp<A>* f11) const {

  if (f00) { f00->makeAll(A(bandCount(),0)); f00->copyDataTypes(*this); }
  if (f01) { f01->makeAll(A(bandCount(),0)); f01->copyDataTypes(*this); }
  if (f10) { f10->makeAll(A(bandCount(),0)); f10->copyDataTypes(*this); }
  if (f11) { f11->makeAll(A(bandCount(),0)); f11->copyDataTypes(*this); }

     // loop through in directions
  for (int in_index=0; in_index<direcStruct().units(); in_index++) {

      // loop through out directions
    for (int out_index=0; out_index<direcStruct().units(); out_index++) {

      const A& sd = atIndices(in_index, out_index);

      const Vec3D dv_in(direcStruct().indexToMeanDirec(in_index));
      const Vec3D dv_out(direcStruct().indexToMeanDirec(out_index));

      // f00 is the function for photon travel in z- directon that ends up in z+ direction - ie water underside
      
      if (dv_in.z() >= 0) {
	if (dv_out.z() >= 0) { if (f10) f10->setData(dv_in, dv_out, sd); }
	else { if (f11) f11->setData(dv_in, dv_out.rotate180AboutX(), sd); }
      } else {
	if (dv_out.z() >= 0) { if (f00) f00->setData(dv_in.rotate180AboutX(), dv_out, sd); }
	else { if (f01) f01->setData(dv_in.rotate180AboutX(), dv_out.rotate180AboutX(), sd); }
      }
    }
  }
}



template <typename A>
void RadDirecDirecImp<A>::reverseExitantY() {

     // loop through in directions
  for (int in_index=0; in_index<direcStruct().units(); in_index++) {

      // clear an array of done flags
    bool done_flag[direcStruct().units()];
    for (int i=0; i<direcStruct().units(); i++) done_flag[i] = false;

      // loop through out directions
    for (int out_index_1=0; out_index_1<direcStruct().units(); out_index_1++) {

      //if (in_index==1) setData(in_index, out_index_1, SBandData(bandCount(), 0));
     
        // only do if not swapped this one already
      if (done_flag[out_index_1] == false) {
 
	  // the direction vectors to swap
	const Vec3D dv_out_1(direcStruct().indexToPointDirec(out_index_1));
	const Vec3D dv_out_2(dv_out_1.x(), -dv_out_1.y(), dv_out_1.z());
  	  // the other index
	int out_index_2 = direcStruct().direcToIndex(dv_out_2);
	  // get the two data entries
	const A sd_1(atIndices(in_index, out_index_1));
	const A sd_2(atIndices(in_index, out_index_2));
	//jlog::ls << "sd1\n" << sd_1 << "\n";
	//jlog::ls << "sd2\n" << sd_2 << "\n";
          // swap data
	setData(in_index, out_index_1, sd_2);
	setData(in_index, out_index_2, sd_1);
	//setData(in_index, out_index_1, SBandData(bandCount(), 0));
	//setData(in_index, out_index_2, sd_1);
          // note that have done this other index
	done_flag[out_index_2] = true;
      }
    }
  }
}



/* \todo Implementation ought to be fixed a bit to ensure code is scalable - defer to DirecStruct derived class. */ 
/*
template <typename A>
bool RadDirecDirecImp<A>::azimuthallyAverage() {

  const SpherePartition* ds = dynamic_cast<const SpherePartition*>(&direcStruct());

  if (!ds) return false;

  RadDirecDirecImp<A>* rtemp(copy());

    // for each in row
  for (int v_in=0; v_in<ds->vertSize(); v_in++) {

      // for each out index
    for (int v_out=0; v_out<ds->vertSize(); v_out++) {
      for (int h_out=0; h_out<ds->horizSize(); h_out++) {

	A sd(bandCount(), 0);

	for (int h_in=0; h_in<ds->horizSize(); h_in++) {
	  h_in_equiv
	  sd += atIndices(v_in*ds->horizSize() + h_in + 1, out_index);
	}

      sd.divide(ds->horizSize());

      for (int h_in=0; h_in<ds->horizSize(); h_in++) setData(v_in*ds->horizSize() + h_in + 1, out_index, sd);
    }
  }

  delete rtemp;

  return true;
}
*/


/*

// ReflecImp<SD>::makeAxisSymmetrical() [private]
// (ax) is 0: x-axis 1: y-axis 2: z-axis

template <typename SD>
void ReflecImp<SD>::makeAxisSymmetrical(int ax) {

    // loop through in directions
  for (int in_index=0; in_index<direcStruct().units(); in_index++) {
      // loop through out directions
    for (int out_index=0; out_index<direcStruct().units(); out_index++) {
      
      Vector3D dv_in(direcStruct().indexToPointDirec(in_index));
      Vector3D dv_out(direcStruct().indexToPointDirec(out_index));
      
      switch (ax) {
        case 0 : dv_in.setX(-dv_in.x());
	         dv_out.setX(-dv_out.x()); 
	         break;
        case 1 : dv_in.setY(-dv_in.y());
	         dv_out.setY(-dv_out.y()); 
	         break;
        default : dv_in.setZ(-dv_in.z());
	          dv_out.setZ(-dv_out.z()); 
      }

      int in_index_opp = direcStruct().direcToIndex(dv_in);
      int out_index_opp = direcStruct().direcToIndex(dv_out);
      
      SD sd = atIndices(in_index, out_index);
      sd += atIndices(in_index_opp, out_index_opp);
      sd.divide(2);

      setReflectance(in_index, out_index, sd);
      setReflectance(in_index_opp, out_index_opp, sd);
    }
  }
}


 */



/*! \brief Interface for classes that retrieve info to display in text output of a directional data table. */
 
template <typename A> 
class RadDirecDirecQDS : public QuadDataSource {

  const RadDirecDirecImp<A>& rdd;

  int in_index;
  int band_index;

public:

  RadDirecDirecQDS(const RadDirecDirecImp<A>& rdd, int ii, int bi) : 
    rdd(rdd), 
    in_index(ii), 
    band_index(bi) { }

  void displayAtIndex(JTextStream& str, int out_index, const JString& sub_sep) const {
    A::policy_type::logWrite(str, rdd.atIndices(in_index, out_index).bandValue(band_index), sub_sep);
    //str << rd.atIndex(out_index).bandValue(band_index);
  }
};



template <typename A>
void RadDirecDirecImp<A>::display(JTextStream& str) const {
   
  for (int band_index=0; band_index<bandCount(); band_index++) {

    for (int in_index=0; in_index<direcStruct().units(); in_index++) {

      jlog::ls << "Incident index: " << in_index << "\n";

      RadDirecDirecQDS<A> qds((*this), in_index, band_index);
      direcStruct().displayTableValues(str, qds, ", ", ",");
    }
  }
}



template <typename A>
void RadDirecDirecImp<A>::displayInIndex(int in_index, JTextStream& str) const {
   
  for (int band_index=0; band_index<bandCount(); band_index++) {

    jlog::ls << "Incident index: " << in_index << "\n";
    
    RadDirecDirecQDS<A> qds((*this), in_index, band_index);
    direcStruct().displayTableValues(str, qds, ", ", ",");
  }
}



} } // namespace jude::rad

#endif // _JUDE_RAD_RADDIRECDIRECIMP_H




