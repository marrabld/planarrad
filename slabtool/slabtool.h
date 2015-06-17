
// slabtool.h

#ifndef _JUDETOOLS_SLABTOOL_H
#define _JUDETOOLS_SLABTOOL_H

#include "tool/tool.h"
#include "rad/raddirecimp.h"
#include "rad/raddirecdirecimp.h"
#include "rad/rdd_matrixdft.h"
#include "rad/rd_matrixdft.h"
#include "rad/slabintegrator.h"
#include "rad/planeinterface.h"
#include "rad/planeboundaryemis.h"
#include "rad/planeboundaryreflec.h"
#include "rad/sbanddata.h"
#include "rad/bandspec.h"
#include "rad/slab.h"
#include "rad/slabintegrator.h"
#include "rad/planesolver.h"
#include "rad/slabsoln.h"
#include "rad/report.h"
#include "ds/direcspec.h"

using namespace jude::tool;
using namespace jude::rad;
using namespace jude::ds;



class SlabTool : public Tool {

  typedef SlabIntegrator<SBandData, IOPSet<SBandData, HemiSpherePartVxH>, HemiSpherePartVxH> SlabInt;
  typedef matrix<double> mat;

  BandSpec* master_bs;
  int band_count;

  DirecSpec* master_ds_spec;
  DirecStruct* master_ds;
  DirecStruct* master_full_ds;

  Report* rep;

  PlaneBoundaryEmis<SBandData> sky;
  PlaneBoundaryReflec<SBandData> subs;
  PlaneInterface<SBandData> water_surface;

  SlabIntegrator_Base* slab_int_ptr;

  Slab sb;

    // will be used in the solution calculation  
  RD_MatrixDFT Ld_a_ma;
  RD_MatrixDFT Lu_a_ma;
  RD_MatrixDFT Ld_w_ma;
  RD_MatrixDFT Lu_w_ma;
  RD_MatrixDFT Ld_b_ma;
  RD_MatrixDFT Lu_b_ma;
  RD_MatrixDFT L_temp1_ma;

  SlabSoln<SBandData, SpherePartVxH>* slab_soln;

    // allocated in the solution calculation
  RadDirecImp<SBandData>* Ld_a;
  RadDirecImp<SBandData>* Lu_a;
  RadDirecImp<SBandData>* Ld_w;
  RadDirecImp<SBandData>* Lu_w;
  RadDirecImp<SBandData>* Ld_b;
  RadDirecImp<SBandData>* Lu_b;
  RadDirecImp<SBandData>* Ld_it;
  RadDirecImp<SBandData>* Lu_it;
  RadDirecImp<SBandData>* Ld_ir;
  RadDirecImp<SBandData>* Lu_ir;

    // these hold the last one calculated
  SBandData sd_rrs;   // Lu(w) / Ed(w) Lee. 1999
  SBandData sd_Rrs;   // Lu(wa) / Ed(a) Mobley L&W page 70 "returned through the surface".
  SBandData sd_Rtot;  // Lu(a) / Ed(a)
  SBandData sd_R0m;   // Eu(w) / Ed(w)
  SBandData sd_R0p;   // Eu(a) / Ed(a)

    // these arrays are allocated if sample points are done
  RadDirecTable<SBandData, HemiSpherePartVxH>** Ld_array;
  RadDirecTable<SBandData, HemiSpherePartVxH>** Lu_array;
  RadDirecTable<SBandData, HemiSpherePartVxH>** dLd_dz_array;
  RadDirecTable<SBandData, HemiSpherePartVxH>** dLu_dz_array;
  int array_size;

    // depths of the sample points
  std::vector<double> sample_point_depth;

  bool processing_done;

  int depth_points_todo;
  int depth_points_done;

  int inversions_todo;
  int inversions_done;

  double dft_pct1;
  double dft_pct2;

  bool dft_cancel;

  void init();

  void clearOutputAlloc();

  bool buildSlab(const JParser& prm);
  bool calcOneSolution(const JParser& prm);

  bool integrateSolutionBasic(const JParser& prm);
  bool integrateSolutionDFT(const JParser& prm);

  bool calcSolutionBounce();

  bool setSamplePoints(SlabIntegrator_Base& slab_int, const JParser& prm);
  bool setIntegrator(SlabIntegrator_Base& slab, const JParser& prm); 
  
  void calcOutputs();
  void saveOutputs(const JParser& prm) const;

  bool calcLuAndLd(int dp, SlabInt& slab_int, 
		   mat* mR11, mat* mR12, mat* mR21, mat* mR22,
		   const mat* p1, const mat* p2,
		   RadDirecTable<SBandData, HemiSpherePartVxH>* Lu, 
		   RadDirecTable<SBandData, HemiSpherePartVxH>* Ld, 
		   const HemiSpherePartVxH& ds);

  void displaySlab(const JString& text, const RadDirecImp<SBandData>& rad, const RDD_MatrixDFT& m);

  const RadDirecImp<SBandData>& checkNotNull(const RadDirecImp<SBandData>* ptr, const char *str) const {
    if (!ptr) { jlog::es << str << " called but this ouput has not been calculated.\n"; internalError(); }
    return *ptr;
  }

public:

  SlabTool();
  ~SlabTool();

  const JString name() const { return "slabtool"; }

  bool run(const JParser& prm);
 
  void cancelProcessing() { Tool::cancelProcessing(); dft_cancel = true; }
  void uncancelProcessing() { Tool::uncancelProcessing(); dft_cancel = false; }
 
  const SBandData& last_Rrs() const { return sd_Rrs; }
  const SBandData& last_rrs() const { return sd_rrs; }
  const SBandData& last_Rtot() const { return sd_Rtot; }
  const SBandData& last_R0m() const { return sd_R0m; }
  const SBandData& last_R0p() const { return sd_R0p; }

  const RadDirecImp<SBandData>& Ld_Air() const { return checkNotNull(Ld_a, __FUNCTION__); }
  const RadDirecImp<SBandData>& Lu_Air() const { return checkNotNull(Lu_a, __FUNCTION__); }
  const RadDirecImp<SBandData>& Ld_Water() const { return checkNotNull(Ld_w, __FUNCTION__); }
  const RadDirecImp<SBandData>& Lu_Water() const { return checkNotNull(Lu_w, __FUNCTION__); }
  const RadDirecImp<SBandData>& Ld_Bottom() const { return checkNotNull(Ld_b, __FUNCTION__); }
  const RadDirecImp<SBandData>& Lu_Bottom() const { return checkNotNull(Lu_b, __FUNCTION__); }
  const RadDirecImp<SBandData>& Lu_WaterLeaving() const { return checkNotNull(Lu_it, __FUNCTION__); }
  const RadDirecImp<SBandData>& Lu_WaterReflected() const { return checkNotNull(Lu_ir, __FUNCTION__); }

    /*! \brief Returns the number of sample points at which output data is available. 
      Must be checked before accessing sample point data, it might be zero. */
  int samplePoints() const { return array_size; }
    /*! \brief Returns the depth of sample point \a i. No bounds checking so be sure to call samplePoints() first. */
  double samplePointDepth(int i) const { return sample_point_depth[i]; }
 
    /*! \brief Returns directional quad averaged downwelling radiance, Ld, at sample point \a i. */
  const RadDirecImp<SBandData>& Ld_SamplePoint(int i) const {
    if (Ld_array == 0) { jlog::es << __FUNCTION__ << " called but this output has not been calculated.\n"; internalError(); }
    if (i<0 || i>array_size)  { jlog::es << __FUNCTION__ << " called with out-of-bounds index.\n"; internalError(); }
    return checkNotNull(Ld_array[i], __FUNCTION__);
  }

    /*! \brief Returns directional quad averaged upwelling radiance, Lu, at sample point \a i. */
  const RadDirecImp<SBandData>& Lu_SamplePoint(int i) const {
    if (Lu_array == 0) { jlog::es << __FUNCTION__ << " called but this output has not been calculated.\n"; internalError(); }
    if (i<0 || i>array_size)  { jlog::es << __FUNCTION__ << " called with out-of-bounds index.\n"; internalError(); }
    return checkNotNull(Lu_array[i], __FUNCTION__);
  }

    /*! \brief Returns rate of increase of Ld with increasing distance at sample point \a i. Converted to planar irradiance this is negative kd. */
  const RadDirecImp<SBandData>& dLd_SamplePoint(int i) const {
    if (dLd_dz_array == 0) { jlog::es << __FUNCTION__ << " called but this output has not been calculated.\n"; internalError(); }
    if (i<0 || i>array_size)  { jlog::es << __FUNCTION__ << " called with out-of-bounds index.\n"; internalError(); }
    return checkNotNull(dLd_dz_array[i], __FUNCTION__);
  }

    /*! \brief Returns rate of increase of Lu with increasing distance at sample point \a i. Converted to planar irradiance this is ku. */
  const RadDirecImp<SBandData>& dLu_SamplePoint(int i) const {
    if (dLu_dz_array == 0) { jlog::es << __FUNCTION__ << " called but this output has not been calculated.\n"; internalError(); }
    if (i<0 || i>array_size)  { jlog::es << __FUNCTION__ << " called with out-of-bounds index.\n"; internalError(); }
    return checkNotNull(dLu_dz_array[i], __FUNCTION__);
  }

  double pctComplete() const;

    /*! \brief Streams the help text for this tool to \a str. */
  void helpText(JTextStream& str) const;


  // one day these may be used again
  // void loadSky(const JString& fp); 
  // void loadSlabFile(const JString& fp);
  // void loadAppendSlabFile(const JString& fp);
  // void loadSubsDiffuseR0(const JString& fp);
  // void loadSubsDiffuseR0(const SBandData& sd);
  // void loadWaterSurface(const JString& fp_r0, const JString& fp_r1, const JString& fp_t01, const JString& fp_t10);
  // void calcReflectances();


  // these were private

  // bool readInputList(const JString& prm_file, const JParser& prm, JTextTable& ttab);

  // bool readSlabFile(const JString& f, RDD_MatrixDFT& mRA, RDD_MatrixDFT& mTA, RDD_MatrixDFT& mRB, RDD_MatrixDFT& mTB);
  // bool writeSlabFile(const JString& f, const RDD_MatrixDFT& mRA, const RDD_MatrixDFT& mTA,
  //		     const RDD_MatrixDFT& mRB, const RDD_MatrixDFT& mTB);
  // bool writeSlabFile(const JString& f, const RDD_MatrixDFT& mRA, const RDD_MatrixDFT& mTA);
  // bool appendSlabFile(const JString& f, 
  //		      RDD_MatrixDFT& mRA, RDD_MatrixDFT& mTA,
  //		      RDD_MatrixDFT& mRB, RDD_MatrixDFT& mTB);
  // void combineSlabs(RDD_MatrixDFT& mT1_f, RDD_MatrixDFT& mR1_f, 
  //		    const RDD_MatrixDFT& mT1_b, const RDD_MatrixDFT& mR1_b, 
  //		    const RDD_MatrixDFT& mT2_f, const RDD_MatrixDFT& mR2_f);
  // void displaySlab(const JString& text, const RadDirecImp<SBandData>& rad, const RDD_MatrixDFT& m);

  // void buildMixedSubstrate(PlaneBoundaryReflec<SBandData>& s, const JTextTable& tt0, const JTextTable& tt1, double f0, double f1, const DirecStruct& ds);
  // void calcSolution_Fast();

};



#endif // _JUDETOOLS_SLABTOOL_H

