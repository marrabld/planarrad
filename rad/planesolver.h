
// planesolver.h

#ifndef _PLANESOLVER_H_JDH
#define _PLANESOLVER_H_JDH

#include "S3/vec3d.h"
#include "math/ublas.h"
#include "rad/bandspec.h"
#include "rad/raddirecdirecimp.h"
#include "rad/planeinterface.h"
#include "rad/slabsoln.h"

using namespace jude::math;

namespace jude {
namespace rad {

class PlaneSolver_Base {

public:

    // solution methods
  typedef enum { EULER=0, MIDPOINT=1 } integration_method;
};



// PlaneSolver

template <typename SD, typename IOPS, typename DS> 
class PlaneSolver : public PlaneSolver_Base {

  typedef double mat_type;
  typedef matrix<mat_type> mat;

  const BandSpec& master_bs;
  int band_count;

  IOPS iops;

  double depth;

  bool auto_usteps;
  int usteps;
  int usteps_per_od;
  double depth_ustep;

  bool auto_dsteps;
  int dsteps;
  int dsteps_per_od;
  double depth_dstep;

  integration_method integ_method;

  int max_bounce_iters;
  double bounce_ppu_frac;

  RadDirecDirecTable<SD,DS> Suu;
  RadDirecDirecTable<SD,DS> Sud;
  RadDirecDirecTable<SD,DS> Sdu;
  RadDirecDirecTable<SD,DS> Sdd;

  RadDirecDirecTable<SD,DS> R;
  RadDirecDirecTable<SD,DS> R_hf;

  RadDirecDirecTable<SD,DS> R_cRC;
  RadDirecDirecTable<SD,DS> R_cCR;

    // only used in 'slow' evaluation function
  RadDirecDirecTable<SD,DS> C;

  RadDirecDirecTable<SD,DS> R_temp1;
  RadDirecDirecTable<SD,DS> R_temp2;
  RadDirecDirecTable<SD,DS> R_temp3;
  RadDirecDirecTable<SD,DS> R_temp4;
  RadDirecDirecTable<SD,DS> R_temp5;

  RadDirecTable<SD,DS> Ld_a;
  RadDirecTable<SD,DS> Lu_a;
  RadDirecTable<SD,DS> Ld_w;
  RadDirecTable<SD,DS> Lu_w;
  RadDirecTable<SD,DS> Ld_wt;
  RadDirecTable<SD,DS> Lu_wt;
  RadDirecTable<SD,DS> Ld_wr;
  RadDirecTable<SD,DS> Lu_wr;
  RadDirecTable<SD,DS> Ld;
  RadDirecTable<SD,DS> Lu;
  RadDirecTable<SD,DS> Ld_hf;
  RadDirecTable<SD,DS> Lu_hf;
  RadDirecTable<SD,DS> L_temp1;
  RadDirecTable<SD,DS> L_temp2;
  
  void bounceLight(const RadDirecDirecImp<SD>& water_reflec, const RadDirecDirecImp<SD>& surf_reflec);

  void upwardSweepBoost_Euler();

  void upwardSweep_Euler();
  void upwardSweep_Midpoint();
  void upwardEval_Slow(RadDirecDirecTable<SD,DS>& currR, double dz);
  void upwardEval(RadDirecDirecTable<SD,DS>& currR, double dz);

  void downwardSweep_Euler();
  void downwardSweep_Midpoint();
  void downwardEval(RadDirecTable<SD,DS>& currLd, RadDirecTable<SD,DS>& currLu, double dz);

  void calcDrDzStage1(RadDirecDirecTable<SD,DS>& R,
		      const RadDirecDirecTable<SD,DS>& currR,
		      const RadDirecDirecTable<SD,DS>& RSud);
  
public:

  PlaneSolver(const DS& ds, const BandSpec& bs);

  IOPS& iopData() { return iops; }
  const IOPS& iopData() const { return iops; }

    /*! \brief Returns the thickness of the slab. */
  double physicalThickness() const { return iops.physicalThickness(); }

    /*! \brief Returns true if the slab is vertically homogeneous. */
  double slabIsHomogeneous() const { return iops.isHomogeneous(); }

    /*! \brief Sets the number of steps to take when integrating upwards. Use zero to set to auto. */
  void setUpwardStepsPerOpticalDepth(int n) {  
    auto_usteps = (n<=0); 
    usteps_per_od = n;
  }

    /*! \brief Sets the number of steps to take when integrating downwards. Use zero to set to auto. */
  void setDownwardStepsPerOpticalDepth(int n) {  
    auto_dsteps = (n<=0); 
    dsteps_per_od = n;
  }

  /*
  void setDepth(double d, int us, int ds) {
    depth = d;
    auto_usteps = (us==0);
    usteps = us;
    auto_dsteps = (ds==0);
    dsteps = ds;
  }
  */

  void setIntegrationMethod(integration_method i) { integ_method = i; }

  void setBounceParams(int bi, double bf) {
    max_bounce_iters = bi;
    bounce_ppu_frac = bf;
  }

  void calcSolution(const PlaneBoundaryEmis<SD>& sky, \
		    const PlaneInterface<SD>& water_surface, \
		    const PlaneBoundaryReflec<SD>& subs, \
		    SlabSoln<SD, typename DS::full_type>& soln);

  const RadDirecTable<SD,DS>& solnLdAir() const { return Ld_a; }
  const RadDirecTable<SD,DS>& solnLuAir() const { return Lu_a; }
  const RadDirecTable<SD,DS>& solnLdWater() const { return Ld_w; }
  const RadDirecTable<SD,DS>& solnLuWater() const { return Lu_w; }
  const RadDirecTable<SD,DS>& solnLdBottom() const { return Ld; }
  const RadDirecTable<SD,DS>& solnLuBottom() const { return Lu; }
};



template <typename SD, typename IOPS, typename DS>
PlaneSolver<SD,IOPS,DS>::PlaneSolver(const DS& ds, const BandSpec& bs) :
  master_bs(bs),
  band_count(bs.bandCount()),
  iops(ds, bs),
  auto_usteps(true),
  auto_dsteps(true),
  integ_method(EULER),
  max_bounce_iters(100),
  bounce_ppu_frac(1E-8),
  Suu(ds, band_count),
  Sud(ds, band_count),
  Sdu(ds, band_count),
  Sdd(ds, band_count),
  R(ds, band_count, RadDirecDirec::MEAN_RAD, RadDirecDirec::MEAN_RAD),
  R_hf(ds, band_count, RadDirecDirec::MEAN_RAD, RadDirecDirec::MEAN_RAD),
  R_cRC(ds, band_count, RadDirecDirec::MEAN_RAD, RadDirecDirec::MEAN_RAD),
  R_cCR(ds, band_count, RadDirecDirec::MEAN_RAD, RadDirecDirec::MEAN_RAD),
  C(ds, band_count, RadDirecDirec::MEAN_RAD, RadDirecDirec::MEAN_RAD),
  R_temp1(ds, band_count, RadDirecDirec::MEAN_RAD, RadDirecDirec::MEAN_RAD),
  R_temp2(ds, band_count, RadDirecDirec::MEAN_RAD, RadDirecDirec::MEAN_RAD),
  R_temp3(ds, band_count, RadDirecDirec::MEAN_RAD, RadDirecDirec::MEAN_RAD),
  R_temp4(ds, band_count, RadDirecDirec::MEAN_RAD, RadDirecDirec::MEAN_RAD),
  R_temp5(ds, band_count, RadDirecDirec::MEAN_RAD, RadDirecDirec::MEAN_RAD),
  Ld_a(ds, band_count, RadDirec::MEAN_RAD),
  Lu_a(ds, band_count, RadDirec::MEAN_RAD),
  Ld_w(ds, band_count, RadDirec::MEAN_RAD),
  Lu_w(ds, band_count, RadDirec::MEAN_RAD),
  Ld_wt(ds, band_count, RadDirec::MEAN_RAD),
  Lu_wt(ds, band_count, RadDirec::MEAN_RAD),
  Ld_wr(ds, band_count, RadDirec::MEAN_RAD),
  Lu_wr(ds, band_count, RadDirec::MEAN_RAD),
  Ld(ds, band_count, RadDirec::MEAN_RAD),
  Lu(ds, band_count, RadDirec::MEAN_RAD),
  Ld_hf(ds, band_count, RadDirec::MEAN_RAD),
  Lu_hf(ds, band_count, RadDirec::MEAN_RAD),
  L_temp1(ds, band_count, RadDirec::MEAN_RAD),
  L_temp2(ds, band_count, RadDirec::MEAN_RAD) {
  
    // build C matrix
  C.makeAll(SD(band_count,0));
  for (int i=0;i<ds.units();i++) C.setData(i, i, SD(band_count, 1/ds.meanCosTheta(i)));
}



template <typename SD, typename IOPS, typename DS> 
void PlaneSolver<SD,IOPS,DS>::calcSolution(const PlaneBoundaryEmis<SD>& sky, const PlaneInterface<SD>& water_surface, 
					   const PlaneBoundaryReflec<SD>& subs, SlabSoln<SD, typename DS::full_type>& soln) {

  jlog::ls << "SOLVE\n";

  double depth = iops.physicalThickness();
 
  jlog::ls << "Total Depth: " << depth << "\n";
 
    // calc and display maximum optical depth
  //SD optical_depth(atten);
  //optical_depth.multiply(depth);

  double max_optical_depth = iops.opticalThickness().maximumBandValue();
  //double max_optical_depth = iops.opticalThickness().getMaximum();
  jlog::ls.printf("Optical Depth (max %f): ",max_optical_depth);
  jlog::ls << iops.opticalThickness() << "\n";

    // estimate steps required based on opitcal depth and solution method
    // is just a rough estimate as accuracy also depends 
    // on substrate reflectance and probably other factors
  if (auto_usteps) {
    switch (integ_method) {
      //case EULER : usteps = (int) (2000 * max_optical_depth); break;
      case EULER : usteps = (int) (200 * max_optical_depth); break;
	//case MIDPOINT : usteps = (int) (1000 * max_optical_depth); break;
      case MIDPOINT : usteps = (int) (100 * max_optical_depth); break;
	// this should never happen
      default: { jlog::es << "integ_method bad value (" << integ_method << ")\n"; internalError(); }
    }
  } else usteps = (int) (usteps_per_od * max_optical_depth); 

  if (auto_dsteps) {
    switch (integ_method) {
      //case EULER : dsteps = (int) (2000 * max_optical_depth); break;
      case EULER : dsteps = (int) (200 * max_optical_depth); break;
	//case MIDPOINT : dsteps = (int) (1000 * max_optical_depth); break;
      case MIDPOINT : dsteps = (int) (100 * max_optical_depth); break;
	// this should never happen
      default: { jlog::es << "integ_method bad value (" << integ_method << ")\n"; internalError(); }
    }
  } else dsteps = (int) (dsteps_per_od * max_optical_depth); 

    // the actual depth size of one step 
  depth_ustep = depth / usteps;
  depth_dstep = depth / dsteps;

    // start value of R
  //Reflec::copy(R, subs_r0);
  R.copy(subs.directionalReflectance());
  jlog::ls << "Substrate Diffuse Reflectance: [ " << R.diffuseReflectance() << " ]\n";

  Suu.copy(iops.Bplus());
  Sdd.copy(iops.Bplus());
  Sdu.copy(iops.Bminus());
  Sud.copy(iops.Bminus());

  //displayDiffuseReflectance("Substrate Diffuse Reflection: ",R);

    // integrate R upwards
  switch (integ_method) {
    case EULER : upwardSweep_Euler(); break;
      //case EULER : upwardSweepBoost_Euler(); break;
    case MIDPOINT : upwardSweep_Midpoint(); break;
  }

  //displayDiffuseReflectance("Water Column Diffuse Reflection:       ",R);
  jlog::ls << "Water Column Diffuse Reflectance: [ " << R.diffuseReflectance() << " ]\n";

  //displayPlaneIrradiance("Sky Ed (Ed_a): ",sky_d);

  //Spec::copy(Ld_a, sky_d);
  Ld_a.copy(sky.emis());
  jlog::ls << "Sky Ed [ " << Ld_a.planarIrradiance() << " ]\n";

    // calculate initial Ld_w based on sky radiance transferred through surface
  Ld_w.makeZero();
  water_surface.transZp().translateInToOut(Ld_a, Ld_w);

  //displayPlaneIrradiance("Sky Component Ed_w: ",Ld_w);
  jlog::ls << "Sky Component Ed_w: [ " << Ld_w.planarIrradiance() << " ]\n";

  jlog::ls << "Water surface under side diffuse reflectance: [ " << water_surface.reflecZmZp().diffuseReflectance() << " ]\n";

    // calculate Ld_w and Lu_w
  //bounceLight(R, surf_r1);
  bounceLight(R, water_surface.reflecZmZp());
  
    // now can calculate Lu_a
  Lu_a.makeZero();
  //Reflec::translateInToOut(surf_t10, Lu_w, Lu_a);
  water_surface.transZm().translateInToOut(Lu_w, Lu_a);
  //Reflec::translateInToOut(surf_r0, Ld_a, Lu_a); 
  water_surface.reflecZpZm().translateInToOut(Ld_a, Lu_a); 
 
  //displayPlaneIrradiance("Ed_a: ", Ld_a);
  //displayPlaneIrradiance("Eu_a: ", Lu_a);
  //displayPlaneIrradiance("Ed_w: ", Ld_w);
  //displayPlaneIrradiance("Eu_w: ", Lu_w);
  jlog::ls << "Ed_a: " << Ld_a.planarIrradiance() << "\n";
  jlog::ls << "Eu_a: " << Lu_a.planarIrradiance() << "\n";
  jlog::ls << "Ed_w: " << Ld_w.planarIrradiance() << "\n";
  jlog::ls << "Eu_w: " << Lu_w.planarIrradiance() << "\n";
 
  Ld.copy(Ld_w);
  Lu.copy(Lu_w);

   // integrate Lu and Ld downwards
  switch (integ_method) {
    case EULER : downwardSweep_Euler(); break;
    case MIDPOINT : downwardSweep_Midpoint(); break;
  }

  jlog::ls << "Ed_b: " << Ld.planarIrradiance() << "\n";
  jlog::ls << "Eu_b: " << Lu.planarIrradiance() << "\n";

  L_temp1.makeZero();
  subs.directionalReflectance().translateInToOut(Ld, L_temp1);
 
  jlog::ls << "Eu_b as reflected Ed_b check: " << L_temp1.planarIrradiance() << "\n";
  jlog::ls << "Lu_b: " << Lu.atZPlus() << "\n";
  jlog::ls << "Lu_w: " << Lu_w.atZPlus() << "\n";
  jlog::ls << "Lu_a: " << Lu_a.atZPlus() << "\n"; 

  // copy data into return soln structure

  soln.setLAir(&Ld_a, &Lu_a);
  soln.setLWater(&Ld_w, &Lu_w);
  soln.setLBottom(&Ld, &Lu);

  Lu_wt.makeZero();
  Ld_wt.makeZero();
  water_surface.transZm().translateInToOut(Lu_w, Lu_wt);
  water_surface.transZp().translateInToOut(Ld_a, Ld_wt);
  soln.setLAirWaterTransmitted(&Ld_wt, &Lu_wt);

  Lu_wr.makeZero();
  Ld_wr.makeZero();
  water_surface.reflecZpZm().translateInToOut(Ld_a, Lu_wr);
  water_surface.reflecZmZp().translateInToOut(Lu_w, Ld_wr);
  soln.setLAirWaterReflected(&Ld_wr, &Lu_wr);
}



// PlaneSolver::bounceLight() [private]
// given initial Ld_w (sky component) this calculates Lu_w
// and modifies Ld_w by multply bounces from the water body and surface underside

template <typename SD, typename IOPS, typename DS> 
void PlaneSolver<SD,IOPS,DS>::bounceLight(const RadDirecDirecImp<SD>& water_reflec, const RadDirecDirecImp<SD>& surf_reflec) {
 
  //RunOpts:log << "Surface to water bounce (limit " << bounce_ppu_frac << ", max steps " << max_bounce_iters << ")\n";

    // start sum of upward reflected light as zero
  Lu_w.makeAll(SD(band_count,0));
 
    // start off downwelling light as sky component at underside of surface
  L_temp1.copy(Ld_w);

    // initial power per unit area
  double ppu = L_temp1.planarIrradiance().sumOfBandValues();
  //double ppu = L_temp1.integrateSurfaceRadiantPPU().integrateOverWavelength();
  jlog::ls << "Iter 0 ppu " << ppu << "\n";

  int iters = 0;
  double p;

  do {

      // bounce downwelling radiance back up from the water
    L_temp2.makeZero();
    water_reflec.translateInToOut(L_temp1, L_temp2); 
      // add onto total coming up from water
    Lu_w.add(L_temp2);

      // bounce upwelling radiance back down off the surface underside
    L_temp1.makeZero();
    //jlog::ls << surf_reflec.diffuseReflectance() << "\n";
    surf_reflec.translateInToOut(L_temp2, L_temp1); 
      // add onto total coming down from surface
    Ld_w.add(L_temp1);

      // remaining power per unit area
    p = L_temp1.planarIrradiance().sumOfBandValues();
    jlog::ls << "Iter " << iters+1 << " ppu " << p << "\n";

  } while (iters++ < max_bounce_iters && p/ppu > bounce_ppu_frac);
}



// PlaneSolver::upwardSweepBoost_Euler() [private]

template <typename SD, typename IOPS, typename DS> 
void PlaneSolver<SD,IOPS,DS>::upwardSweepBoost_Euler() {

  int n = R.direcStruct().units();

  matrix<SD> mat_R(n,n);
  matrix<SD> mat_Sdu(n,n);
  matrix<SD> mat_Suu(n,n);
  matrix<SD> mat_Sud(n,n);
  matrix<SD> mat_Sdd(n,n);
  matrix<double> mat_C(n,n);
  matrix<SD> mat1(n,n);
  matrix<SD> mat2(n,n);

  for (int in_index=0;in_index<n;in_index++) {
    for (int out_index=0;out_index<n;out_index++) {
      mat_R(in_index, out_index) = R.atIndices(in_index, out_index);
      //mat_Sdu(in_index, out_index) = Sdu->atIndices(in_index, out_index);
      //mat_Suu(in_index, out_index) = Suu->atIndices(in_index, out_index);
      //mat_Sud(in_index, out_index) = Sud->atIndices(in_index, out_index);
      //mat_Sdd(in_index, out_index) = Sdd->atIndices(in_index, out_index);
      mat_Sdu(in_index, out_index) = iops.Bminus().atIndices(in_index, out_index);
      mat_Suu(in_index, out_index) = iops.Bplus().atIndices(in_index, out_index);
      mat_Sud(in_index, out_index) = iops.Bminus().atIndices(in_index, out_index);
      mat_Sdd(in_index, out_index) = iops.Bplus().atIndices(in_index, out_index);
      mat_C(in_index, out_index) = 0;
    }

    mat_C(in_index, in_index) = 1 / fabs(R.direcStruct().meanCosTheta(in_index));
  }

    // upward sweep
  for (int i=0;i<usteps;i++) {

      // cRC
    mat1 = prod(mat_R, mat_C);
    mat1 *= (iops.attenuation());

      // cCR
    mat2 = prod(mat_C, mat_R);
    mat2 *= (iops.attenuation());

      // cRC + cCR
    mat1 += mat2;

      // SduC
    mat2 = prod(mat_Sdu, mat_C);
      // cRC + cCR - sduC
    mat1 -= mat2;
    
      // RSuuC
    mat2 = prod(mat_R, mat_Suu);
    mat2 = prod(mat2, mat_C);
      // cRC + cCR - sduC - RSuuC
    mat1 -= mat2;  

      // SddCR
    mat2 = prod(mat_Sdd, mat_C);
    mat2 = prod(mat2, mat_R);
      // cRC + cCR - sduC - RSuuC - SddCR
    mat1 -= mat2;  

      // RSudCR
    mat2 = prod(mat_R, mat_Sud);
    mat2 = prod(mat2, mat_C);
    mat2 = prod(mat2, mat_R);
      // cRC + cCR - sduC - RSuuC - SddCR - RSudCR
    mat1 -= mat2;  
    
      // approx linear over small distance travelled
    mat1 *= -depth_ustep;

    mat_R += mat1;
  
    if (jlog::ls.vbLevel(2)) {
  
      for (int in_index=0;in_index<n;in_index++) {
	for (int out_index=0;out_index<n;out_index++) {
	  R->setData(in_index, out_index, mat_R(in_index, out_index));
	}
      }
      jlog::ls.printf("%d: depth %f ",i,depth-(i+1)*depth_ustep);
      jlog::ls << "diffuse reflec: " << R->diffuseReflectance() << "\n";
      jlog::ls.device().cursorUp(1);
    } 
  }


  if (jlog::ls.vbLevel(2)) jlog::ls << "\n";
  else {
    for (int in_index=0;in_index<n;in_index++) {
      for (int out_index=0;out_index<n;out_index++) {
	R->setData(in_index, out_index, mat_R(in_index, out_index));
      }
    }
  }

  //exit(1);
}



// PlaneSolver::upwardSweep_Euler()
// use Euler method to upwardly integrate R

template <typename SD, typename IOPS, typename DS> 
void PlaneSolver<SD,IOPS,DS>::upwardSweep_Euler() {
 
  if (jlog::vbLevel(2)) jlog::ls << "Upward R integral by Euler method " << usteps << " steps (" << usteps << " evals)\n";
 
    // upward sweep
  for (int i=0;i<usteps;i++) {
 
    upwardEval(R, depth_ustep);
    R.add(R_temp1);

    if (jlog::vbLevel(2)) {
      jlog::ls.printf("%d: depth %.3f ", i+1, depth-(i+1)*depth_ustep);
      jlog::ls << "Diffuse reflec: " << R.diffuseReflectance() << "\n";
      jlog::ls.device().cursorUp(1);
    } 
  }
  if (jlog::vbLevel(2)) jlog::ls << "\n";
}



// PlaneTool::upwardSweep_Midpoint()
// use Midpoint method to upwardly integrate R

template <typename SD, typename IOPS, typename DS> 
void PlaneSolver<SD,IOPS,DS>::upwardSweep_Midpoint() {
 
  if (jlog::vbLevel(2)) jlog::ls << "Upward R integral by Midpoint method " << usteps << " steps (" << (2*usteps) << " evals)\n";
 
    // upward sweep
  for (int i=0;i<usteps;i++) { 

    R_hf.copy(R);
    upwardEval(R_hf, depth_ustep/2);
    R_hf.add(R_temp1);

    upwardEval(R_hf, depth_ustep);
    R.add(R_temp1);

    if (jlog::vbLevel(2)) {
      jlog::ls.printf("%d: depth %f ", i+1, depth-(i+1)*depth_ustep);
      jlog::ls << "Diffuse reflec: " << R.diffuseReflectance() << "\n";
      jlog::ls.device().cursorUp(1);
    }  
  }
  if (jlog::vbLevel(2)) jlog::ls << "\n";
}



// upwardEval_Slow()
// calculate dz * dR/dz in R_temp1 for the current reflectance (currR) and distance (dz)
// this version does not utilise the calcDrDzStage1() which performs the calculation a bit faster by direct table access
// however in this version it is easier to see the equation form

template <typename SD, typename IOPS, typename DS> 
void PlaneSolver<SD,IOPS,DS>::upwardEval_Slow(RadDirecDirecTable<SD,DS>& currR, double dz) {

    // cRC
  R_temp1.makeMatrixMultiple(currR, C);
  R_temp1.multiplyAll(iops.attenuation());

    // cCR
  R_temp2.makeMatrixMultiple(C, currR);
  R_temp2.multiplyAll(iops.attenuation());

    // cRC + cCR
  R_temp1.add(R_temp2);  

    // SduC
  R_temp2.makeMatrixMultiple(Sdu, C);
    // cRC + cCR - sduC
  R_temp1.subtract(R_temp2);  

    // RSuuC
  R_temp2.makeMatrixMultiple(currR, Suu);
  R_temp3.makeMatrixMultiple(R_temp2, C);
    // cRC + cCR - sduC - RSuuC
  R_temp1.subtract(R_temp3);  

    // SddCR
  R_temp2.makeMatrixMultiple(Sdd, C);
  R_temp3.makeMatrixMultiple(R_temp2, currR);
    // cRC + cCR - sduC - RSuuC - SddCR
  R_temp1.subtract(R_temp3);  

    // RSudCR
  R_temp2.makeMatrixMultiple(currR, Sud);
  R_temp3.makeMatrixMultiple(R_temp2, C);
  R_temp4.makeMatrixMultiple(R_temp3, currR);
    // cRC + cCR - sduC - RSuuC - SddCR - RSudCR
  R_temp1.subtract(R_temp4);
   
    // approx linear over small distance travelled
  R_temp1.multiply(-dz);
}



// upwardEval()
// calculate dz * dR/dz in R_temp1 for the current reflectance (currR) and distance (dz)

template <typename SD, typename IOPS, typename DS> 
void PlaneSolver<SD,IOPS,DS>::upwardEval(RadDirecDirecTable<SD,DS>& currR, double dz) {

    // RSud
  R_temp2.makeMatrixMultiple(currR, Sud);

    // cRC + cCR
  calcDrDzStage1(R_temp1, currR, R_temp2);
   
    // approx linear over small distance travelled
  R_temp1.multiply(-dz);
}



// calcDrDzStage1()
// this is a slightly more efficient and convienient function for calculating dR/dz for use 
// in the upward reflection integtration in the invariant imbedded method

template <typename SD, typename IOPS, typename DS> 
void PlaneSolver<SD,IOPS,DS>::calcDrDzStage1(RadDirecDirecTable<SD,DS>& R,
					     const RadDirecDirecTable<SD,DS>& currR,
					     const RadDirecDirecTable<SD,DS>& RSud) {

  const DS& ds = currR.direcStruct();
  SD v;
 
  SD* ptr(R.tableData().dataArray());
  const SD* currR_ptr(currR.tableData().dataArray());
  const SD* Sdu_ptr(Sdu.tableData().dataArray());
 
    // go through the indices of the result
  for (int in_index=0; in_index<ds.units(); in_index++) {
    double sa_in_recip = 1 / ds.meanCosTheta(in_index);

    for (int out_index=0; out_index<ds.units(); out_index++, ptr++,currR_ptr++,Sdu_ptr++) {
      double sa_out_recip = 1 / ds.meanCosTheta(out_index);

      //v.makeMultiple(*currR_ptr, iops.attenuation(), (sa_in_recip + sa_out_recip));
      v = (*currR_ptr) * iops.attenuation() * (sa_in_recip + sa_out_recip);
      v.subtract(*Sdu_ptr * sa_out_recip);

        // the index to sum over (lhs out_index and rhs in_index)
      for (int sum_index=0; sum_index<ds.units(); sum_index++) {
	double sa_sum_recip = 1 / ds.meanCosTheta(sum_index);

	v -= currR.atIndices(in_index, sum_index) * Suu.atIndices(sum_index, out_index) * sa_out_recip;
	v -= (Sdd.atIndices(in_index, sum_index) + RSud.atIndices(in_index, sum_index)) * sa_sum_recip * currR.atIndices(sum_index, out_index);
	
      }

      *ptr = v;
    }
  }

}



// PlaneSolver::downwardSweep_Euler()

template <typename SD, typename IOPS, typename DS> 
void PlaneSolver<SD,IOPS,DS>::downwardSweep_Euler() {
 
  if (jlog::vbLevel(2)) {
    jlog::ls.printf("Downward Ld and Lu integral by Euler method %d steps (%d evals)\n",dsteps,dsteps);   
    jlog::ls << "Initial Ed: " << Ld.planarIrradiance() << "\n";
    jlog::ls << "Initial Eu: " << Lu.planarIrradiance() << "\n";
  }

    // downward sweep
  for (int i=0;i<dsteps;i++) { 
  L_temp1.copy(Ld);

    downwardEval(Ld, Lu, depth_dstep);

    Ld.add(L_temp1);
    Lu.add(L_temp2);

    if (jlog::vbLevel(2)) {

      double d = (i+1)*depth_dstep;
      double prev_d = i*depth_dstep;
        // display every meter
      bool disp = (floor(d) != floor(prev_d)); 

      jlog::ls.printf("%d: depth %f:\n",i,d);
      jlog::ls << "Ed: " << Ld.planarIrradiance() << "\n";
      jlog::ls << "Eu: " << Lu.planarIrradiance() << "\n";
      if (i<dsteps-1 && !disp) jlog::ls.device().cursorUp(3);
    }
  }
}



// PlaneSolver::downwardSweep_Midpoint()

template <typename SD, typename IOPS, typename DS> 
void PlaneSolver<SD,IOPS,DS>::downwardSweep_Midpoint() {

  if (jlog::vbLevel(2)) {
    jlog::ls.printf("Downward Ld and Lu integral by Midpoint method %d steps (%d evals)\n",dsteps,2*dsteps);
    jlog::ls << "Initial Ed: " << Ld.planarIrradiance() << "\n";
    jlog::ls << "Initial Eu: " << Lu.planarIrradiance() << "\n";
  }

    // downward sweep
  for (int i=0;i<dsteps;i++) { 

    Ld_hf.copy(Ld);
    Lu_hf.copy(Lu);
    downwardEval(Ld_hf, Lu_hf, depth_dstep/2);

    Ld_hf.add(L_temp1);
    Lu_hf.add(L_temp2);

    downwardEval(Ld_hf, Lu_hf, depth_dstep);

    Ld.add(L_temp1);
    Lu.add(L_temp2);

    if (jlog::vbLevel(2)) {

      double d = (i+1)*depth_dstep;
      double prev_d = i*depth_dstep;
        // display every meter
      bool disp = (floor(d) != floor(prev_d)); 

      jlog::ls.printf("%d: depth %f:\n",i,(i+1)*depth_dstep);
      jlog::ls << "Ed: " << Ld.planarIrradiance() << "\n";
      jlog::ls << "Eu: " << Lu.planarIrradiance() << "\n";
      if (i<dsteps-1 && !disp) jlog::ls.device().cursorUp(3);
    }
  }
}



// PlaneSolver::downwardEval()
// calculate dz * dLd/dz in L_temp1 
// calculate dz * dLu/dz in L_temp2 
// for the current radiance distribution (currLu) and (currLd) distance (dz)

template <typename SD, typename IOPS, typename DS> 
void PlaneSolver<SD,IOPS,DS>::downwardEval(RadDirecTable<SD,DS>& currLd, RadDirecTable<SD,DS>& currLu, double dz) {

  L_temp1.copy(currLd);
    // -cLd
  L_temp1.multiplyAll(iops.minusAttenuation());
    // -cLd + LdSdd
  Sdd.translateInToOut(currLd, L_temp1);
    // -cLd + LdSdd + LuSud
  Sud.translateInToOut(currLu, L_temp1);
    // equivalent to (-cLd + LdSdd + LuSud) * C
  L_temp1.divideAllByCT();
    // approx linear over small distance travelled
  L_temp1.multiply(dz);
    // copies back the original data type which is what this is
  L_temp1.copyDataType(currLd);

  L_temp2.copy(currLu);
    // -cLu
  L_temp2.multiplyAll(iops.minusAttenuation());
    // -cLu + LdSdu
  Sdu.translateInToOut(currLd, L_temp2);
    // -cLu + LdSdu + LuSuu
  Suu.translateInToOut(currLu, L_temp2);
    // equivalent to (-cLu + LdSdu + LuSuu) * C
  L_temp2.divideAllByCT();
    // approx linear over small distance travelled - and negate
  L_temp2.multiply(-dz);
    // copies back the original data type which is what this is
  L_temp2.copyDataType(currLu);
}



} } 

#endif // _PLANESOLVER_H_JDH

