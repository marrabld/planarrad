
// slabintegrator.h

#ifndef _JUDE_RAD_SLABINTEGRATOR_H
#define _JUDE_RAD_SLABINTEGRATOR_H

#include "math/ublas.h"
#include "base/jlog.h"
#include "rad/raddirecdirecdftsym.h"
#include "rad/rdd_matrixdft.h"
#include "rad/iopset.h"
#include "rad/slab.h"

using namespace jude::base;
using namespace jude::math;
using namespace jude::S3;

namespace jude {
namespace rad {



class SlabIntegrator_Base {

  //protected:

  // double pct_weight;
  // double pct_done;

public:

    // solution methods
  typedef enum { EULER=0, MIDPOINT, RUNGA4, RUNGA4ADAP } integration_method;

  virtual ~SlabIntegrator_Base() { }

  virtual void setCalcRA(bool f) = 0;
  virtual void setCalcRB(bool f) = 0;
  virtual void setCalcTA(bool f) = 0;
  virtual void setCalcTB(bool f) = 0;

  virtual double physicalThickness() const = 0;
  virtual double slabIsHomogeneous() const = 0;

  virtual void setRegularSamplePoints(double dist) = 0; 
  virtual void setRegularSamplePoints(double dist, double delta_dist) = 0; 
  virtual void setSamplePoints(const std::vector<double>& dist_list) = 0; 
  virtual void setSamplePoints(const std::vector<double>& dist_list, double delta_dist) = 0; 
  virtual int samplePointCount() = 0;
  virtual double samplePointDistanceFromA(int i) = 0;

  virtual void setIntegrationMethod(integration_method i) = 0;

  virtual void setEulerStepsPerOpticalDepth(int s) = 0;
  virtual void setMidpointStepsPerOpticalDepth(int s) = 0;
  virtual void setRunga4StepsPerOpticalDepth(int s) = 0;
  virtual void setRunga4AdapMinStepsPerOpticalDepth(int s) = 0;
  virtual void setRunga4AdapMaxStepsPerOpticalDepth(int s) = 0;
  virtual void setRunga4AdapMinError(double e) = 0;
  virtual void setRunga4AdapMaxError(double e) = 0;
 
  virtual void buildWholeMatricesRA(matrix<double>* m, int dp, bool dp_is_sp=true) = 0;
  virtual void buildWholeMatricesTA(matrix<double>* m, int dp, bool dp_is_sp=true) = 0;
  virtual void buildWholeMatricesRB(matrix<double>* m, int dp, bool dp_is_sp=true) = 0;
  virtual void buildWholeMatricesTB(matrix<double>* m, int dp, bool dp_is_sp=true) = 0;

  virtual void buildMatrixArrayRA(RDD_MatrixDFT& rm, int dp, bool dp_is_sp=true) = 0;
  virtual void buildMatrixArrayTA(RDD_MatrixDFT& rm, int dp, bool dp_is_sp=true) = 0;
  virtual void buildMatrixArrayRB(RDD_MatrixDFT& rm, int dp, bool dp_is_sp=true) = 0;
  virtual void buildMatrixArrayTB(RDD_MatrixDFT& rm, int dp, bool dp_is_sp=true) = 0;

  virtual bool buildSlab(Slab& slab, int dp) = 0;
  virtual bool buildSlab(Slab& slab) = 0;
 
  virtual void calcSolution() = 0;

  virtual double pctSolutionComplete() const = 0;
};




// SlabIntegrator

template <typename SD, typename IOPS, typename HDS> 
class SlabIntegrator : public SlabIntegrator_Base {

  typedef double mat_type;

  typedef matrix<mat_type> mat;
  typedef const matrix<mat_type> cmat;

  int band_count;

  // IOPS provides either homogenous or profiled a, b, c, vsf 
  // functions it should provide:
  // physicalThickness()
  // setCurrentBand(int band)
  // setCurrentLMode(int lm)
  // setCurrOpticalPosition(double curr_od)
  // currTau()
  // currRho()

  IOPS iops;

    // boundary A reflectance (low z end) - restricted to rotationally symmetrical function
  RadDirecDirecDFTSym<SD, HDS> subs_A;
    // boundary B reflectance (high z end) - restricted to rotationally symmetrical function
  RadDirecDirecDFTSym<SD, HDS> subs_B;

    // number of sample points to evaluate
  int sample_point_count;
    // if true this means each smaple point has extra points either side to calc Kd etc
  bool ab_delta_dist_points;
    // actual number of points to save matrices - not necesarily the same as the number of sample points
  int ab_point_count;
    // array of actual point locations
  double* ab_point;
    // distance between delta points either side of sample point
  double ab_delta_distance;

    // how to do the integration
  integration_method integ_method;
  int euler_steps_per_optical_depth;
  int midpoint_steps_per_optical_depth;
  int runga4_steps_per_optical_depth;
  int runga4adap_min_steps_per_optical_depth;
  int runga4adap_max_steps_per_optical_depth;
  double runga4adap_min_error;
  double runga4adap_max_error;

  bool calc_RA;   // true if need to integrate reflectance looking towards boundary A
  bool calc_RB;   // true if need to integrate reflectance looking towards boundary B
  bool calc_TA;   // true if need to integrate transmission from boundary A in direction towards B
  bool calc_TB;   // true if need to integrate transmission from boundary B in direction towards A

    // reflection looking towards boundary A integrated from A moving towards B 
    // (null = dont calculate)
  mat* RA1_table;
  mat* RA2_table;

    // reflection looking towards boundary B integrated from B moving towards A
    // (null = dont calculate)
  mat* RB1_table;
  mat* RB2_table;

    // transmission from boundary A integrated from A moving towards B 
    // (null = dont calculate)
  mat* TA1_table;
  mat* TA2_table;

    // transmission from boundary B integrated from B moving towards A
    // (null = dont calculate)
  mat* TB1_table;
  mat* TB2_table;

    // delta reflection looking towards boundary A integrated from A moving towards B 
    // (null = dont calculate)
  mat* dRA1_table;
  mat* dRA2_table;

    // delta reflection looking towards boundary B integrated from B moving towards A
    // (null = dont calculate)
  mat* dRB1_table;
  mat* dRB2_table;

    // delta transmission from boundary A integrated from A moving towards B 
    // (null = dont calculate)
  mat* dTA1_table;
  mat* dTA2_table;

    // delta transmission from boundary B integrated from B moving towards A
    // (null = dont calculate)
  mat* dTB1_table;
  mat* dTB2_table;

    // internal reflection of boundary A (or null to ignore)
  mat* reflecA1_table;
  mat* reflecA2_table;

    // internal reflection of boundary B (or null to ignore)
  mat* reflecB1_table;
  mat* reflecB2_table;
  
    // this is used in processing
  struct SI_Process {
    mat* curr_R1;
    mat* curr_R2;
    mat* curr_T1;
    mat* curr_T2;
    mat* curr_dR1;
    mat* curr_dR2;
    mat* curr_dT1;
    mat* curr_dT2;
    const mat* curr_reflec1;
    const mat* curr_reflec2;
  };

  int soln_bands_todo;
  int soln_bands_done;

  void allocateTables();
  void deleteTables();

  double kdelta(int i) { return (i==0) ? 1 : 0; }
 
  void buildTauAndRho();

  cmat eval_dR(cmat& R, cmat& tau, cmat& rho) const { return prod(R, (tau + prod(rho, R))) + rho + prod(tau, R); }
  cmat eval_dT(cmat& R, cmat& T, cmat& tau, cmat& rho) const { return prod(T, (tau + prod(rho, R))); }

  void integrate(SI_Process& ps, bool a_to_b);
  void integrate_Euler(mat& R, mat* T, double optical_depth_start, double optical_depth_end);
  void integrate_Midpoint(mat& R, mat* T, double optical_depth_start, double optical_depth_end);
  void integrate_Runga4(mat& R, mat* T, double optical_depth_start, double optical_depth_end);
  void integrate_Runga4Adap(mat& R, mat* T, double optical_depth_start, double optical_depth_end);

  void buildWholeMatrices(matrix<double>* m, const mat* curr, int dp, bool a_to_b, const char* mn, bool dp_is_sp=true);
  void buildMatrixArray(RDD_MatrixDFT& rm, const mat* curr, int dp, bool a_to_b, const char* mn, bool dp_is_sp=true);
  void buildMatrixArray12(RDD_MatrixDFT& rm, const mat* curr1, const mat* curr2, int dp, bool a_to_b, const char* mn1, const char* mn2, bool dp_is_sp=true);
 
public:

  SlabIntegrator(const HDS& hds, const BandSpec& bs);
  ~SlabIntegrator();

    /*! \brief Sets whether to calculate reflectance looking towards boundary A. */
  void setCalcRA(bool f) { calc_RA = f; }
    /*! \brief Sets whether to calculate reflectance looking towards boundary B. */
  void setCalcRB(bool f) { calc_RB = f; }
    /*! \brief Sets whether to calculate transmission from boundary A. */
  void setCalcTA(bool f) { calc_TA = f; }
    /*! \brief Sets whether to calculate transmission from boundary B. */
  void setCalcTB(bool f) { calc_TB = f; }

  IOPS& iopData() { return iops; }
  const IOPS& iopData() const { return iops; }

    /*! \brief Returns the thickness of the slab. */
  double physicalThickness() const { return iops.physicalThickness(); }

    /*! \brief Returns true if the slab is vertically homogeneous. */
  double slabIsHomogeneous() const { return iops.isHomogeneous(); }

    /*! \brief Loads a directional reflectance function for the internal reflectance at boundary A. Must be rotationally invariant. */
  void loadInternalReflectanceA(const RadDirecDirecImp<SD>& ri) { 
    subs_A.copy(ri);
    reflecA1_table = new mat[band_count * iops.halfDirecStruct().lmNumDFT()];
    reflecA2_table = new mat[band_count * iops.halfDirecStruct().lmNumDFT()];
    subs_A.buildIndividualMatrices(reflecA1_table, reflecA2_table);
  }

    /*! \brief Loads a directional reflectance function for the internal reflectance at boundary B. Must be rotationally invariant. */
  void loadInternalReflectanceB(const RadDirecDirecImp<SD>& ri) { 
    subs_B.copy(ri);
    reflecB1_table = new mat[band_count * iops.halfDirecStruct().lmNumDFT()];
    reflecB2_table = new mat[band_count * iops.halfDirecStruct().lmNumDFT()];
    subs_B.buildIndividualMatrices(reflecB1_table, reflecB2_table);
  }

    /*! \brief Sets regular sample points from boundary A seperated by physical distance \a dist.
      A final point is always placed at boundary B, so the last section may be shorter than \a dist. */
  void setRegularSamplePoints(double dist) {
    ab_delta_dist_points = false; 
    ab_delta_distance = 0;
    delete[] ab_point;
    sample_point_count = (int) ceil(iops.physicalThickness() / dist) + 1;
    ab_point_count = sample_point_count;
    ab_point = new double[ab_point_count];
    for (int i=0; i<ab_point_count-1; i++) ab_point[i] = dist * i;
    ab_point[ab_point_count-1] = iops.physicalThickness();
    if (jlog::ls.vbLevel(6)) jlog::ls.printf("sample_point_count = %d ab_point_count = %d\n",sample_point_count, ab_point_count);
  }

   /*! \brief Set regular sample points from boundary A at physical distance \a dist with additional points either side at distance half \a delta_dist.
     A final point is always placed at boundary B, so the last section may be shorter than \a dist. */
  void setRegularSamplePoints(double dist, double delta_dist) { 
    ab_delta_dist_points = true;
    ab_delta_distance = delta_dist;
    delta_dist /= 2;
    delete[] ab_point;
    sample_point_count = (int) ceil(iops.physicalThickness() / dist) + 1;
    ab_point_count = sample_point_count * 3 - 2;
    ab_point = new double[ab_point_count];
    ab_point[0] = 0;
    ab_point[1] = delta_dist;
    for (int i=1; i<sample_point_count-1; i++) {
      ab_point[3*i - 1] = dist * i - delta_dist;
      ab_point[3*i] = dist * i;
      ab_point[3*i + 1] = dist * i + delta_dist;
    }
    ab_point[ab_point_count-2] = iops.physicalThickness() - delta_dist;
    ab_point[ab_point_count-1] = iops.physicalThickness();
    if (jlog::ls.vbLevel(6)) jlog::ls.printf("sample_point_count = %d ab_point_count = %d\n",sample_point_count, ab_point_count);
  }

    // set sample points from boundary A at the physical distances supplied in (dist_vec)
    // note it is assumed and should be the case that the final point is at boundary B
    // if not the final supplied sample point is ignored
    // also is assumed to start at zero
  void setSamplePoints(const std::vector<double>& dist_vec) {
    ab_delta_dist_points = false;
    ab_delta_distance = 0;
    delete[] ab_point;
    sample_point_count = (int) dist_vec.size();
    ab_point_count = sample_point_count;
    ab_point = new double[ab_point_count];
    for (int i=0; i<ab_point_count-1; i++) ab_point[i] = dist_vec[i];
    ab_point[ab_point_count-1] = iops.physicalThickness();
    if (jlog::ls.vbLevel(6)) printf("sample_point_count = %d ab_point_count = %d\n", sample_point_count, ab_point_count);
  }

    // set sample points from boundary A at the physical distances supplied in (dist_vec)
    // note it is assumed and should be the case that the final point is at boundary B
    // if not the final supplied sample point is ignored
    // also is assumed to start at zero
    // also letting delta_dist > distance between sample pointa will be trouble
  void setSamplePoints(const std::vector<double>& dist_vec, double delta_dist) {
    ab_delta_dist_points = true;
    ab_delta_distance = delta_dist;
    delta_dist /= 2;
    delete[] ab_point;
    sample_point_count = (int) dist_vec.size();
    ab_point_count = sample_point_count * 3 - 2;
    ab_point = new double[ab_point_count];
    ab_point[0] = 0;
    ab_point[1] = delta_dist;
    for (int i=1; i<sample_point_count-1; i++) {
      ab_point[3*i - 1] = dist_vec[i] - delta_dist;
      ab_point[3*i] = dist_vec[i];
      ab_point[3*i + 1] = dist_vec[i] + delta_dist;
    }
    ab_point[ab_point_count-2] = iops.physicalThickness() - delta_dist;
    ab_point[ab_point_count-1] = iops.physicalThickness();
    if (jlog::ls.vbLevel(6)) printf("sample_point_count = %d ab_point_count = %d\n", sample_point_count, ab_point_count);
  }

    // note this includes the first and last points at boundaries A and B
  int samplePointCount() { return sample_point_count; } 
  double samplePointDistanceFromA(int i) { return ab_delta_dist_points ? ab_point[3*i] : ab_point[i]; }
  double sampleDeltaPointDistanceFromA(int i) { return ab_point[i]; }
  double samplePointDeltaDistance() const { return ab_delta_distance; }

    /*! \brief Sets which integration method to use. */
  void setIntegrationMethod(integration_method i) { integ_method = i; }

  void setEulerStepsPerOpticalDepth(int s) { euler_steps_per_optical_depth = s; }
  void setMidpointStepsPerOpticalDepth(int s) { midpoint_steps_per_optical_depth = s; }
  void setRunga4StepsPerOpticalDepth(int s) { runga4_steps_per_optical_depth = s; }
  void setRunga4AdapMinStepsPerOpticalDepth(int s) { runga4adap_min_steps_per_optical_depth = s; }
  void setRunga4AdapMaxStepsPerOpticalDepth(int s) { runga4adap_max_steps_per_optical_depth = s; }
  void setRunga4AdapMinError(double e) { runga4adap_min_error = e; }
  void setRunga4AdapMaxError(double e) { runga4adap_max_error = e; }

  // return matrix for sample point (dp) 
  // (dp == 0) is the boundary A
  // (dp == ab_point_count-1) is the boundary B

    // reflection at (dp) looking towards boundary A
  void buildWholeMatricesRA(matrix<double>* m, int dp, bool dp_is_sp=true) { buildWholeMatrices(m, RA1_table, dp, false, "RA", dp_is_sp); }
    // transmission from boundary A to (dp)
  void buildWholeMatricesTA(matrix<double>* m, int dp, bool dp_is_sp=true) { buildWholeMatrices(m, TA1_table, dp, false, "TA", dp_is_sp); }
    // two reflectance matrices used when internal boundary A reflectance is defined
  void buildWholeMatricesRA1(matrix<double>* m, int dp, bool dp_is_sp=true) { buildWholeMatrices(m, RA1_table, dp, false, "RA1", dp_is_sp); }
  void buildWholeMatricesRA2(matrix<double>* m, int dp, bool dp_is_sp=true) { buildWholeMatrices(m, RA2_table, dp, false, "RA2", dp_is_sp); }

    // reflection at (dp) looking towards boundary B
  void buildWholeMatricesRB(matrix<double>* m, int dp, bool dp_is_sp=true) { buildWholeMatrices(m, RB1_table, dp, true, "RB", dp_is_sp); }
    // transmission from boundary B to (dp)
  void buildWholeMatricesTB(matrix<double>* m, int dp, bool dp_is_sp=true) { buildWholeMatrices(m, TB1_table, dp, true, "TB", dp_is_sp); }
    // two reflectance matrices used when internal boundary B reflectance is defined
  void buildWholeMatricesRB1(matrix<double>* m, int dp, bool dp_is_sp=true) { buildWholeMatrices(m, RB1_table, dp, true, "RB1", dp_is_sp); }
  void buildWholeMatricesRB2(matrix<double>* m, int dp, bool dp_is_sp=true) { buildWholeMatrices(m, RB2_table, dp, true, "RB2", dp_is_sp); }
 
  // convienience functions for total slab reflection and transmission inside the slab

    // reflection at boundary B looking towards boundary A
  void buildWholeMatricesRA_B(matrix<double>* m) { buildWholeMatrices(m, RA1_table, ab_point_count-1, false, "RA"); }
    // transmission from boundary A to B
  void buildWholeMatricesTA_B(matrix<double>* m) { buildWholeMatrices(m, TA1_table, ab_point_count-1, false, "TA"); }
    // two reflectance matrices used when internal boundary A reflectance is defined
  void buildWholeMatricesRA1_B(matrix<double>* m) { buildWholeMatrices(m, RA1_table, ab_point_count-1, false, "RA1"); }
  void buildWholeMatricesRA2_B(matrix<double>* m) { buildWholeMatrices(m, RA2_table, ab_point_count-1, false, "RA2"); }

    // reflection at boundary A looking towards boundary B
  void buildWholeMatricesRB_A(matrix<double>* m) { buildWholeMatrices(m, RB1_table, 0, true, "RB"); }
    // transmission from boundary B to A
  void buildWholeMatricesTB_A(matrix<double>* m) { buildWholeMatrices(m, TB1_table, 0, true, "TB"); }
    // two reflectance matrices used when internal boundary B reflectance is defined
  void buildWholeMatricesRB1_A(matrix<double>* m) { buildWholeMatrices(m, RB1_table, 0, true, "RB1"); }
  void buildWholeMatricesRB2_A(matrix<double>* m) { buildWholeMatrices(m, RB2_table, 0, true, "RB2"); }

    // these are equivalent to the above functions
    // but use the RDD_MatrixDFT type to return the matrices
    // dp_is_sp is a bodge to extract the kd sample points when false

    /*! \brief Returns in \a rm the reflection at sample point \a dp looking towards boundary A.
      This version is for when there is no internal reflectance at A. */
  void buildMatrixArrayRA(RDD_MatrixDFT& rm, int dp, bool dp_is_sp=true) { buildMatrixArray(rm, RA1_table, dp, false, "RA", dp_is_sp); }
    /*! \brief Returns in \a rm the transmission from boundary A to sample point \a dp. */
  void buildMatrixArrayTA(RDD_MatrixDFT& rm, int dp, bool dp_is_sp=true) { buildMatrixArray(rm, TA1_table, dp, false, "TA", dp_is_sp); }
    /*! \brief Returns in \a rm the reflection at sample point \a dp looking towards boundary A.
      This version uses two matrices in \a rm for when internal rotationally invariant reflectance at boundary A is defined. */
  void buildMatrixArrayRA12(RDD_MatrixDFT& rm, int dp, bool dp_is_sp=true) { buildMatrixArray12(rm, RA1_table, RA2_table, dp, false, "RA1", "RA2", dp_is_sp); }

    /*! \brief Returns in \a rm the reflection at sample point \a dp looking towards boundary B.
      This version is for when there is no internal reflectance at B. */
  void buildMatrixArrayRB(RDD_MatrixDFT& rm, int dp, bool dp_is_sp=true) { buildMatrixArray(rm, RB1_table, ab_point_count-1, false, "RB", dp_is_sp); }
    /*! \brief Returns in \a rm the transmission from boundary B to sample point \a dp. */
  void buildMatrixArrayTB(RDD_MatrixDFT& rm, int dp, bool dp_is_sp=true) { buildMatrixArray(rm, TB1_table, ab_point_count-1, false, "TB", dp_is_sp); }
    /*! \brief Returns in \a rm the reflection at sample point \a dp looking towards boundary B.
      This version uses two matrices in \a rm for when internal rotationally invariant reflectance at boundary B is defined. */
  void buildMatrixArrayRB12(RDD_MatrixDFT& rm, int dp, bool dp_is_sp=true) { buildMatrixArray12(rm, RB1_table, RB2_table, ab_point_count-1, false, "RB1", "RB2", dp_is_sp); }

    /*! \brief Returns in a \rm the reflection of the whole slab looking towards boundary A from boundary B.
      This version is for when there is no internal reflectance at A. */
  void buildMatrixArrayRA_B(RDD_MatrixDFT& rm) { buildMatrixArray(rm, RA1_table, ab_point_count-1, false, "RA"); }
    // transmission from boundary A to B
  void buildMatrixArrayTA_B(RDD_MatrixDFT& rm) { buildMatrixArray(rm, TA1_table, ab_point_count-1, false, "TA"); }
    // two reflectance matrices used when internal boundary A reflectance is defined
  void buildMatrixArrayRA12_B(RDD_MatrixDFT& rm) { buildMatrixArray12(rm, RA1_table, RA2_table, ab_point_count-1, false, "RA1", "RA2"); }

    /*! \brief Returns in a \rm the reflection of the whole slab looking towards boundary B from boundary A.
      This version is for when there is no internal reflectance at B. */
  void buildMatrixArrayRB_A(RDD_MatrixDFT& rm) { buildMatrixArray(rm, RB1_table, ab_point_count-1, false, "RB"); }
    // transmission from boundary B to A
  void buildMatrixArrayTB_A(RDD_MatrixDFT& rm) { buildMatrixArray(rm, TB1_table, ab_point_count-1, false, "TB"); }
    // two reflectance matrices used when internal boundary B reflectance is defined
  void buildMatrixArrayRB12_A(RDD_MatrixDFT& rm) { buildMatrixArray12(rm, RB1_table, RB2_table, ab_point_count-1, false, "RB1", "RB2"); }
  
  bool buildSlab(Slab& slab, int dp);
 
  bool buildSlab(Slab& slab) { return buildSlab(slab, ab_point_count-1); }

    /*! \brief Perform the integration as current set up. */ 
  void calcSolution();

    /*! \brief Returns the current percentage completion of the solution. */
  double pctSolutionComplete() const { return 100 * ((double)soln_bands_done) / soln_bands_todo; }
};



// SlabIntegrator::(constructor)

template <typename SD, typename IOPS, typename HDS>
SlabIntegrator<SD,IOPS,HDS>::SlabIntegrator(const HDS& hds, const BandSpec& bs) :
  band_count(bs.bandCount()),
  iops(hds, bs),
  subs_A(hds, band_count),
  subs_B(hds, band_count),
  ab_point_count(2),
  ab_delta_distance(0),
  integ_method(EULER),
  euler_steps_per_optical_depth(200),
  calc_RA(true),
  calc_RB(true),
  calc_TA(true),
  calc_TB(true),
  RA1_table(0),
  RA2_table(0),
  RB1_table(0),
  RB2_table(0),
  TA1_table(0),
  TA2_table(0),
  TB1_table(0),
  TB2_table(0),
  reflecA1_table(0),
  reflecA2_table(0),
  reflecB1_table(0),
  reflecB2_table(0),
  soln_bands_todo(band_count * 2),
  soln_bands_done(0) {

  ab_point = new double[ab_point_count];
  ab_point[0] = 0;
  ab_point[1] = 1; // will be thickness
}



// SlabIntegrator::(destructor)

template <typename SD, typename IOPS, typename HDS>
SlabIntegrator<SD,IOPS,HDS>::~SlabIntegrator() {
  delete[] ab_point;
  delete[] reflecA1_table;
  delete[] reflecA2_table;
  delete[] reflecB1_table;
  delete[] reflecB2_table;
  deleteTables();
}



// SlabIntegrator::allocateTables() [private]

template <typename SD, typename IOPS, typename HDS>
void SlabIntegrator<SD,IOPS,HDS>::allocateTables() {

    // sanity checks
  if (!calc_RA && reflecA1_table) { jlog::ls << "SlabIntegrator boundary A reflectance set but calcRA not defined\n"; internalError(); } 
  if (!calc_RB && reflecB1_table) { jlog::ls << "SlabIntegrator boundary B reflectance set but calcRB not defined\n"; internalError(); } 

  int table_size = band_count * iops.halfDirecStruct().lmNumDFT() * ab_point_count;

    // need one reflectance to calc slab reflectance itself or transmittance
  RA1_table = (calc_RA) ? new mat[table_size] : 0;
    // only need second reflectance matrix if the internal boundary reflectance is defined
  RA2_table = (reflecA1_table) ? new mat[table_size] : 0;
    // only need to do transmittance if requested
  TA1_table = (calc_TA) ? new mat[table_size] : 0;

    // need one reflectance to calc slab reflectance itself or transmittance
  RB1_table = (calc_RB) ? new mat[table_size] : 0;
    // only need second reflectance matrix if the internal boundary reflectance is defined
  RB2_table = (reflecB1_table) ? new mat[table_size] : 0;
    // only need to do transmittance if requested
  TB1_table = (calc_TB) ? new mat[table_size] : 0;

  const HDS& hds(iops.halfDirecStruct());

    // fix matrix sizes
  for (int i=0; i<table_size; i++) {
    if (RA1_table) RA1_table[i].resize(hds.rowsDFT(), hds.rowsDFT());
    if (RA2_table) RA2_table[i].resize(hds.rowsDFT(), hds.rowsDFT());
    if (TA1_table) TA1_table[i].resize(hds.rowsDFT(), hds.rowsDFT());
    if (RB1_table) RB1_table[i].resize(hds.rowsDFT(), hds.rowsDFT());
    if (RB2_table) RB2_table[i].resize(hds.rowsDFT(), hds.rowsDFT());
    if (TB1_table) TB1_table[i].resize(hds.rowsDFT(), hds.rowsDFT());
  }
}



// SlabIntegrator::deleteTables() [private]

template <typename SD, typename IOPS, typename HDS>
void SlabIntegrator<SD,IOPS,HDS>::deleteTables() {

  delete[] RA1_table;
  delete[] RA2_table;
  delete[] TA1_table;
  delete[] RB1_table;
  delete[] RB2_table;
  delete[] TB1_table;
}





// SlabIntegrator::buildWholeMatrices()
// generic routine to return one of the spectral standard reflectance or 
// transmittance matrices as at the table of matrices at (*curr)
// (m) is an array of matrices one for each band
// each matrix contains the l-mode matrices diagonally (L&W pg. 419)

template <typename SD, typename IOPS, typename HDS> 
void SlabIntegrator<SD,IOPS,HDS>::buildWholeMatrices(matrix<double>* m, const mat* curr, int dp, bool a_to_b, const char* mn, bool dp_is_sp) {

  if (!m) { jlog::es << "SlabIntegrator matrix " << mn << " requested but not calculated"; internalError(); }
  
    // the actual stored point for this sample point
  int ab_point_for_dp = (ab_delta_dist_points && dp_is_sp) ? dp*3 : dp;

    // offset to this depth point
    // sample points are numbered from 0 at boundary A but ordered in reverse in the table for A to B 
  if (a_to_b) curr += (ab_point_count - ab_point_for_dp - 1);
  else curr += ab_point_for_dp;

  const HDS& hds(iops.halfDirecStruct());

  for (int band=0; band<band_count; band++) {

    // this didn't zero the other elements
    //m[band].resize(ds.rowsDFT()*ds.lmNumDFT(), ds.rowsDFT()*ds.lmNumDFT());

      // resize and zero all the matrix elements 
    m[band] = zero_matrix<mat_type>(hds.rowsDFT()*hds.lmNumDFT(), hds.rowsDFT()*hds.lmNumDFT());

    for (int lm=0; lm<hds.lmNumDFT(); lm++) {

      int offset = lm*hds.rowsDFT();
      
      for (int i=0; i<hds.rowsDFT(); i++) for (int j=0; j<hds.rowsDFT(); j++) m[band](offset+i, offset+j) = (*curr)(i,j);

      curr += ab_point_count;
    }
  }
}



// SlabIntegrator::buildMatrixArray() [private]

template <typename SD, typename IOPS, typename HDS> 
void SlabIntegrator<SD,IOPS,HDS>::buildMatrixArray(RDD_MatrixDFT& rm, const mat* curr, int dp, bool a_to_b, const char* mn, bool dp_is_sp) {
  const HDS& hds(iops.halfDirecStruct());
    // allocate a single matrix in the structure
  rm.setup1(band_count, hds.lmNumDFT(), hds.rowsDFT());
    // copy in both matrices
  buildWholeMatrices(rm.array11(), curr, dp, a_to_b, mn, dp_is_sp);
}



// SlabIntegrator::buildMatrixArray12() [private]

template <typename SD, typename IOPS, typename HDS>
void SlabIntegrator<SD,IOPS,HDS>::buildMatrixArray12(RDD_MatrixDFT& rm, const mat* curr1, const mat* curr2, 
					  int dp, bool a_to_b, const char* mn1, const char* mn2, bool dp_is_sp) {
  const HDS& hds(iops.halfDirecStruct());
    // allocate the w11 and w22 matrices
  rm.setup2(band_count, hds.lmNumDFT(), hds.rowsDFT());
    // copy in both matrices
  buildWholeMatrices(rm.array11(), curr1, dp, a_to_b, mn1, dp_is_sp);
  buildWholeMatrices(rm.array22(), curr2, dp, a_to_b, mn2, dp_is_sp);
}



/*! \todo fix for internal boundary reflecatnces. */

template <typename SD, typename IOPS, typename HDS>
bool SlabIntegrator<SD,IOPS,HDS>::buildSlab(Slab& slab, int dp) {

    // have to have one-way transmission and reflection at least
  if (!calc_RA || !calc_TA) return false;
    // copy one side
  buildMatrixArrayRA(slab.mRdown, dp);  // reflection looking upward from (dp) (B) toward boundary A
  buildMatrixArrayTA(slab.mTdown, dp);  // dowward transmission from (A) to point (dp)
  
    // if iops are homogenous other side is the same
  if (iops.isHomogeneous()) {
    buildMatrixArrayRA(slab.mRup, dp);  // reflection looking downward from (A) toward boundary (dp) (B)
    buildMatrixArrayTA(slab.mTup, dp);  // upward transmission from (dp) (B) toward A
  } else {
      // these have to have been done
    if (!calc_RB || !calc_TB) return false;
    buildMatrixArrayRB(slab.mRup, dp);  // reflection looking downward from (A) toward boundary (dp) (B)
    buildMatrixArrayTB(slab.mTup, dp);  // upward transmission from (dp) (B) toward A
  }

  return true;
}



// SlabIntegrator::calcSolution()

template <typename SD, typename IOPS, typename HDS> 
void SlabIntegrator<SD,IOPS,HDS>::calcSolution() {

  allocateTables();

  if (jlog::vbLevel(3)) jlog::ls << "Build tau and rho...\n";

    // will build tau and rho
  iops.init();

  soln_bands_todo = RA1_table ? band_count : 0;
  if (RB1_table) soln_bands_todo += band_count;
  soln_bands_done = 0;

  if (jlog::ls.vbLevel(3)) jlog::ls << "Integrate... " << ((RA1_table) ? "RA1 " : "") << ((RB1_table) ? "RB1" : "") << "\n"; 

  //pct_weight = (RA1_table && RB1_table) ? 0.5 : 1;

  if (RA1_table) {

    if (jlog::ls.vbLevel(3)) jlog::ls << "Boundary A to B\n";

    SI_Process psA;
    psA.curr_reflec1 = reflecA1_table;
    psA.curr_reflec2 = reflecA2_table;
    psA.curr_R1 = RA1_table;
    psA.curr_R2 = RA2_table;
    psA.curr_T1 = TA1_table;
    psA.curr_T2 = TA2_table;
      // these will store rate of change at sample points
    psA.curr_dR1 = dRA1_table;
    psA.curr_dR2 = dRA2_table;
    psA.curr_dT1 = dTA1_table;
    psA.curr_dT2 = dTA2_table;
      // downward sweep integral from boundary A to B
    integrate(psA, true);
  }

  if (RB1_table) {

    if (jlog::ls.vbLevel(3)) jlog::ls << "Boundary B to A\n";

    SI_Process psB;
    psB.curr_reflec1 = reflecB1_table;
    psB.curr_reflec2 = reflecB2_table;
    psB.curr_R1 = RB1_table;
    psB.curr_R2 = RB2_table;
    psB.curr_T1 = TB1_table;
    psB.curr_T2 = TB2_table; 
      // these will store rate of change at sample points
    psB.curr_dR1 = dRB1_table;
    psB.curr_dR2 = dRB2_table;
    psB.curr_dT1 = dTB1_table;
    psB.curr_dT2 = dTB2_table;
      // upward sweep integral from boundary B to A
    integrate(psB, false);
  }
}



// SlabIntegrator::integrate()

template <typename SD, typename IOPS, typename HDS> 
void SlabIntegrator<SD,IOPS,HDS>::integrate(SI_Process& ps, bool a_to_b) {

  //printf("%p %p %p %p\n", ps.curr_R1, ps.curr_R2, ps.curr_T1, ps.curr_reflec1); 

    // loop through all bands
  for (int band=0; band<band_count; band++) {

    if (jlog::ls.vbLevel(3)) jlog::ls << "Band " << band << "\n";

    iops.setCurrentBand(band);

      // loop through all l-modes
    for (int lm=0; lm<iops.halfDirecStruct().lmNumDFT(); lm++) {

      iops.setCurrentLMode(lm);

        // initialise reflection, note that on calling this we assume ps.curr_R1 != 0
        // and if ps.curr_reflec1 is defined then ps.curr_R2 is allocated 
      if (ps.curr_reflec1) {
	*ps.curr_R1 = *ps.curr_reflec1;
	*ps.curr_R2 = *ps.curr_reflec2;
      } else {
	*ps.curr_R1 = zero_matrix<double>(iops.halfDirecStruct().rowsDFT());
	//*ps.curr_R2 = zero_matrix<double>(ds.rowsDFT());
      }

        // initialise transmission
      if (ps.curr_T1) *ps.curr_T1 = identity_matrix<double>(iops.halfDirecStruct().rowsDFT()); 
    
      double optical_depth_start;

        // start at boundary A
      if (a_to_b) optical_depth_start = 0;
        // start at boundary B
      //else optical_depth_start = atten[band] * iops.physicalThickness();
      else optical_depth_start = iops.opticalThickness()[band];

        // for each optical depth point at which to sample up to boundary A
      for (int i=ab_point_count-2; i>=0; i--) {

  	  // move on to next slot for R1 matrix output
	ps.curr_R1++;
	  // if using gpu then R1 is still stored on gpu else must copy to start on in new slot 
	*ps.curr_R1 = *(ps.curr_R1 - 1);
 
          // move on to next slot for R2 matrix output, if calculating
	if (ps.curr_R2) {
	  ps.curr_R2++;
	  *ps.curr_R2 = *(ps.curr_R2 - 1);
	}

          // move on to next slot for T1 matrix output, if calculating
	if (ps.curr_T1) {
	  ps.curr_T1++;
	  *ps.curr_T1 = *(ps.curr_T1 - 1);
	}

	double optical_depth_end;
	if (a_to_b) optical_depth_end = iops.physicalDistanceToOptical(ab_point[ab_point_count-1-i], band);
	else optical_depth_end = iops.physicalDistanceToOptical(ab_point[i], band);

	//printf("a2b %d %f %f %d %d\n",a_to_b,ab_point[ab_point_count-1-i],ab_point[i],ab_point_count,i);
	//printf("optical start %f end %f ot %f\n",optical_depth_start, optical_depth_end, iops.opticalThickness()[band]);
	
	if (integ_method == MIDPOINT) {
	  integrate_Midpoint(*ps.curr_R1, ps.curr_T1, optical_depth_start, optical_depth_end);
	  if (ps.curr_R2) integrate_Midpoint(*ps.curr_R2, 0, optical_depth_start, optical_depth_end);

	} else if (integ_method == RUNGA4) {
	  integrate_Runga4(*ps.curr_R1, ps.curr_T1, optical_depth_start, optical_depth_end);
	  if (ps.curr_R2) integrate_Runga4(*ps.curr_R2, 0, optical_depth_start, optical_depth_end);

	} else if (integ_method == RUNGA4ADAP) {
	  integrate_Runga4Adap(*ps.curr_R1, ps.curr_T1, optical_depth_start, optical_depth_end);
	  if (ps.curr_R2) integrate_Runga4Adap(*ps.curr_R2, 0, optical_depth_start, optical_depth_end);

	} else {
	  integrate_Euler(*ps.curr_R1, ps.curr_T1, optical_depth_start, optical_depth_end);
	  if (ps.curr_R2) integrate_Euler(*ps.curr_R2, 0, optical_depth_start, optical_depth_end);
	}

	optical_depth_start = optical_depth_end;

	//pct_done = 100 * pct_weight * (band + (lm + ((double)(ab_point_count-2-i)) / (ab_point_count-2)) / iops.halfDirecStruct().lmNumDFT()) / band_count;
      }

      ps.curr_R1++;
      if (ps.curr_R2) ps.curr_R2++;
      if (ps.curr_T1) ps.curr_T1++;
      
      if (ps.curr_reflec1) {
	ps.curr_reflec1++;
	ps.curr_reflec2++;
      }

    }
    soln_bands_done++;
  }
}



// SlabIntegrator::integrate_Euler()
// T may be null

template <typename SD, typename IOPS, typename HDS> 
void SlabIntegrator<SD,IOPS,HDS>::integrate_Euler(mat& R, mat* T, double optical_depth_start, double optical_depth_end) {

    // the optical depth of the sub-slab
    // using fabs() means we can use the same Riccatti equations for up and down
  double optical_dist = fabs(optical_depth_end - optical_depth_start);
    // number of steps to do this sub-slab
  int steps = (int) ceil(euler_steps_per_optical_depth * optical_dist);
    // the optical depth step per step
  double optical_depth_step = optical_dist / steps;

  //printf("steps %d start %f end %f\n",steps,optical_depth_start,optical_depth_end);

  double curr_od = optical_depth_start;

    // integrate this sub slab
  for (int i=0; i<steps; i++, curr_od+=optical_depth_step) {

    iops.setCurrOpticalPosition(curr_od);

    if (T) (*T) += eval_dT(R, *T, iops.currTau(), iops.currRho()) * optical_depth_step;
    R += eval_dR(R, iops.currTau(), iops.currRho()) * optical_depth_step;
  }
}
 


// SlabIntegrator::integrate_Midpoint()
// T may be null

template <typename SD, typename IOPS, typename HDS> 
void SlabIntegrator<SD,IOPS,HDS>::integrate_Midpoint(mat& R, mat* T, double optical_depth_start, double optical_depth_end) {

    // the optical depth of the sub-slab
    // using fabs() means we can use the same Riccatti equations for up and down
  double optical_dist = fabs(optical_depth_end - optical_depth_start);
    // number of steps to do this sub-slab
  int steps = (int) ceil(midpoint_steps_per_optical_depth * optical_dist);
    // the optical depth step per step
  double optical_depth_step = optical_dist / steps;
    // the optical depth half-step
  double half_optical_depth_step = optical_dist / (2*steps);
  //half_optical_depth_step = 0;

  //printf("steps %d start %f end %f T %p\n",steps,optical_depth_start,optical_depth_end,T);

  mat R_mid;
  mat T_mid;

  double curr_od = optical_depth_start;

    // integrate this sub slab
  for (int i=0; i<steps; i++, curr_od+=optical_depth_step) {

    iops.setCurrOpticalPosition(curr_od);

      // evaluate R and T at halfway point
    R_mid = R + eval_dR(R, iops.currTau(), iops.currRho()) * half_optical_depth_step;
    if (T) T_mid = (*T) + eval_dT(R, *T, iops.currTau(), iops.currRho()) * half_optical_depth_step;

    iops.setCurrOpticalPosition(curr_od + half_optical_depth_step);

      // now update R and T based on slope at midpoint 
    R += eval_dR(R_mid, iops.currTau(), iops.currRho()) * optical_depth_step;
    if (T) (*T) += eval_dT(R_mid, T_mid, iops.currTau(), iops.currRho()) * optical_depth_step;
  }
}



// SlabIntegrator::integrate_Runga4()
// T may be null

template <typename SD, typename IOPS, typename HDS> 
void SlabIntegrator<SD,IOPS,HDS>::integrate_Runga4(mat& R, mat* T, double optical_depth_start, double optical_depth_end) {

    // the optical depth of the sub-slab
    // using fabs() means we can use the same Riccatti equations for up and down
  double optical_dist = fabs(optical_depth_end - optical_depth_start);
    // number of steps to do this sub-slab
  int steps = (int) ceil(runga4_steps_per_optical_depth * optical_dist);
    // the optical depth step per step
  double optical_depth_step = optical_dist / steps;
    // the optical depth half-step
  //double half_optical_depth_step = optical_dist / (2*steps);

  //printf("steps %d %d start %f end %f T %p\n",steps,runga4_steps_per_optical_depth,optical_depth_start,optical_depth_end,T);

  mat R_k1(R.size1(), R.size2());
  mat R_k2(R.size1(), R.size2());
  mat R_k3(R.size1(), R.size2());
  mat R_k4(R.size1(), R.size2());

  mat T_k1(R.size1(), R.size2());
  mat T_k2(R.size1(), R.size2());
  mat T_k3(R.size1(), R.size2());
  mat T_k4(R.size1(), R.size2());

  double curr_od = optical_depth_start;

    // integrate this sub slab
  for (int i=0; i<steps; i++, curr_od+=optical_depth_step) {

    iops.setCurrOpticalPosition(curr_od);

    noalias(R_k1) = eval_dR(R, iops.currTau(), iops.currRho()) * optical_depth_step;
    if (T) noalias(T_k1) = eval_dT(R, *T, iops.currTau(), iops.currRho()) * optical_depth_step;

    iops.setCurrOpticalPosition(curr_od + optical_depth_step/2);

      // mid point
    noalias(R_k2) = eval_dR(R+R_k1/2, iops.currTau(), iops.currRho()) * optical_depth_step;
    if (T) noalias(T_k2) = eval_dT(R+R_k1/2, (*T)+T_k1/2, iops.currTau(), iops.currRho()) * optical_depth_step;

      // mid point also
    noalias(R_k3) = eval_dR(R+R_k2/2, iops.currTau(), iops.currRho()) * optical_depth_step;
    if (T) noalias(T_k3) = eval_dT(R+R_k2/2, (*T)+T_k2/2, iops.currTau(), iops.currRho()) * optical_depth_step;

    iops.setCurrOpticalPosition(curr_od + optical_depth_step);

      // end point
    noalias(R_k4) = eval_dR(R+R_k3, iops.currTau(), iops.currRho()) * optical_depth_step;
    if (T) noalias(T_k4) = eval_dT(R+R_k3, (*T)+T_k3, iops.currTau(), iops.currRho()) * optical_depth_step;

    R += R_k1/6 + R_k2/3 + R_k3/3 + R_k4/6;
    if (T) (*T) += T_k1/6 + T_k2/3 + T_k3/3 + T_k4/6;
  }
}



// SlabIntegrator::integrate_Runga4Adap()
// T may be null

template <typename SD, typename IOPS, typename HDS> 
void SlabIntegrator<SD,IOPS,HDS>::integrate_Runga4Adap(mat& R, mat* T, double optical_depth_start, double optical_depth_end) {

    // the optical depth of the sub-slab
    // using fabs() means we can use the same Riccatti equations for up and down
  double optical_dist = fabs(optical_depth_end - optical_depth_start);
    // number of steps to do this sub-slab
  int max_steps = (int) ceil(runga4adap_max_steps_per_optical_depth * optical_dist);
  int min_steps = (int) ceil(runga4adap_min_steps_per_optical_depth * optical_dist);
  int curr_steps = max_steps;
 
  //printf("steps %d %d start %f end %f T %p\n",steps,runga4_steps_per_optical_depth,optical_depth_start,optical_depth_end,T);

  mat R_k1;
  mat R_k2;
  mat R_k3;
  mat R_k4;
  mat R_single;
  mat R_double;

  mat T_k1;
  mat T_k2;
  mat T_k3;
  mat T_k4;
  mat T_single;
  mat T_double;

  double total_max = 0;
  mat_type max_value;
  bool do_again;

  double single_step_size;
  double double_step_size;

  double optical_pos = 0;
  int total_steps = 0;
  int total_evals = 0;

  printf("FIX ME!\n");

  const mat& tau = iops.currTau();
  const mat& rho = iops.currRho();

    // integrate this sub slab
  while (optical_pos < optical_dist) {

    do {

      single_step_size = optical_dist / curr_steps;
      if (single_step_size > optical_dist - optical_pos) single_step_size = optical_dist - optical_pos;

      double_step_size = single_step_size / 2;

      // a single step

      R_k1 = eval_dR(R, tau, rho) * single_step_size;
      if (T) T_k1 = eval_dT(R, *T, tau, rho) * single_step_size;

      R_k2 = eval_dR(R+R_k1/2, tau, rho) * single_step_size;
      if (T) T_k2 = eval_dT(R+R_k1/2, (*T)+T_k1/2, tau, rho) * single_step_size;

      R_k3 = eval_dR(R+R_k2/2, tau, rho) * single_step_size;
      if (T) T_k3 = eval_dT(R+R_k2/2, (*T)+T_k2/2, tau, rho) * single_step_size;
      
      R_k4 = eval_dR(R+R_k3, tau, rho) * single_step_size;
      if (T) T_k4 = eval_dT(R+R_k3, (*T)+T_k3, tau, rho) * single_step_size;
      
      R_single = R + R_k1/6 + R_k2/3 + R_k3/3 + R_k4/6;
      if (T) T_single = (*T) + T_k1/6 + T_k2/3 + T_k3/3 + T_k4/6;

      // a double step
      
      //R_k1 = eval_dR(R, tau, rho) * double_step_size;
      //if (T) T_k1 = eval_dT(R, *T, tau, rho) * double_step_size;

      R_k1 /= 2;
      if (T) T_k1 /= 2;

      R_k2 = eval_dR(R+R_k1/2, tau, rho) * double_step_size;
      if (T) T_k2 = eval_dT(R+R_k1/2, (*T)+T_k1/2, tau, rho) * double_step_size;
      
      R_k3 = eval_dR(R+R_k2/2, tau, rho) * double_step_size;
      if (T) T_k3 = eval_dT(R+R_k2/2, (*T)+T_k2/2, tau, rho) * double_step_size;
      
      R_k4 = eval_dR(R+R_k3, tau, rho) * double_step_size;
      if (T) T_k4 = eval_dT(R+R_k3, (*T)+T_k3, tau, rho) * double_step_size;
      
      R_double = R + R_k1/6 + R_k2/3 + R_k3/3 + R_k4/6;
      if (T) T_double = (*T) + T_k1/6 + T_k2/3 + T_k3/3 + T_k4/6;
      
      R_k1 = eval_dR(R_double, tau, rho) * double_step_size;
      if (T) T_k1 = eval_dT(R_double, T_double, tau, rho) * double_step_size;
      
      R_k2 = eval_dR(R_double+R_k1/2, tau, rho) * double_step_size;
      if (T) T_k2 = eval_dT(R_double+R_k1/2, T_double+T_k1/2, tau, rho) * double_step_size; 

      R_k3 = eval_dR(R_double+R_k2/2, tau, rho) * double_step_size;
      if (T) T_k3 = eval_dT(R_double+R_k2/2, T_double+T_k2/2, tau, rho) * double_step_size;
      
      R_k4 = eval_dR(R_double+R_k3, tau, rho) * double_step_size;
      if (T) T_k4 = eval_dT(R_double+R_k3, T_double+T_k3, tau, rho) * double_step_size;
      
      R_double += R_k1/6 + R_k2/3 + R_k3/3 + R_k4/6;
      if (T) T_double += T_k1/6 + T_k2/3 + T_k3/3 + T_k4/6;

      max_value = 0;
      
      R_single -= R_double;
      max_value = norm_fabs_max(R_single);
      if (T) {
	T_single -= T_double;
	max_value = norm_fabs_max(T_single);
      }
      if (total_max < max_value) total_max = max_value;

      do_again = false;

      //int prev_steps = curr_steps;

      if (max_value > runga4adap_max_error && curr_steps < max_steps) {
	curr_steps = curr_steps*2;
	if (curr_steps > max_steps) curr_steps = max_steps;
	do_again = true;
      }

      if (max_value < runga4adap_min_error && curr_steps > min_steps) {
	curr_steps = curr_steps/2;
	if (curr_steps < min_steps) curr_steps = min_steps;
      }
  
      //printf("prev_steps %d new_steps %d (%d %d) error %e do_again %d\n",prev_steps,curr_steps,min_steps,max_steps,max_value,do_again);
  
      total_evals += 3;

    } while (do_again);

    R = R_double;
    if (T) (*T) = T_double;

    optical_pos += single_step_size;
    total_steps++;
  }

  //printf("total steps %d evals %d\n",total_steps, total_evals);
}



} } // namespace jude::rad

#endif // _JUDE_RAD_SLABINTEGRATOR_H

