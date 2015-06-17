
// slabtool.cc

#include "slabtool.h"
#include "base/jparser.h"
#include "rad/raddirecdirectable.h"
#include "rad/raddirecdirecdft.h"
#include "rad/raddirecdft.h"
#include "rad/report.h"
#include "base/jtexttable.h"
#include "base/jdatastream.h"
#include "config.h"sl




// SlabTool::(constructor)

SlabTool::SlabTool() : 
  Tool(), 
  master_bs(0),
  band_count(0),
  master_ds_spec(0),
  master_ds(0),
  slab_int_ptr(0),
  slab_soln(0),
  Ld_a(0),
  Lu_a(0),
  Ld_w(0),
  Lu_w(0),
  Ld_b(0),
  Lu_b(0),
  Ld_it(0),
  Lu_it(0),
  Ld_ir(0),
  Lu_ir(0),
  Ld_array(0),
  Lu_array(0),
  dLd_dz_array(0),
  dLu_dz_array(0),
  array_size(0),
  sample_point_depth(0),
  processing_done(false) { init(); }



// SlabTool::(destructor)

SlabTool::~SlabTool() {
  delete master_bs;
  delete master_ds_spec;
  delete master_ds;
  clearOutputAlloc();
}



// SlabTool::clearOutputAlloc() [private]

void SlabTool::clearOutputAlloc() {

  delete slab_soln;

  delete Ld_a;
  delete Lu_a;
  delete Ld_w;
  delete Lu_w;
  delete Ld_b;
  delete Lu_b;
  delete Ld_it;
  delete Lu_it;
  delete Ld_ir;
  delete Lu_ir;

  for (int i=0; i<array_size; i++) {
    delete Ld_array[i];
    delete Lu_array[i];
  }
  delete[] Ld_array;
  delete[] Lu_array;

  if (dLd_dz_array) {
    for (int i=0; i<array_size; i++) {
      delete dLd_dz_array[i];
      delete dLu_dz_array[i];
    }
    delete[] dLd_dz_array;
    delete[] dLu_dz_array;
  }

  slab_soln = 0;

  Ld_a = 0;
  Lu_a = 0;
  Ld_w = 0;
  Lu_w = 0;
  Ld_b = 0;
  Lu_b = 0;
  Ld_it = 0;
  Lu_it = 0;
  Ld_ir = 0;
  Lu_ir = 0;

  Ld_array = 0;
  Lu_array = 0;
  dLd_dz_array = 0;
  dLu_dz_array = 0;
  array_size = 0;
  sample_point_depth.resize(0);
}



// SlabTool::init()

void SlabTool::init() {

  static bool done_init = false;
  if (done_init) return;

    // somewhere in all programs there must occur a list of all the static polymorphic types that can occur
  JFactory< RadDirecImp< SBandData>, RadDirecTable< SBandData, HemiSpherePartVxH > >::instance().reg();
  JFactory< RadDirecImp< SBandData>, RadDirecTable< SBandData, SpherePartVxH > >::instance().reg();
  JFactory< RadDirecDirecImp< SBandData>, RadDirecDirecTable< SBandData, HemiSpherePartVxH > >::instance().reg();
  JFactory< RadDirecDirecImp< SBandData>, RadDirecDirecTable< SBandData, SpherePartVxH > >::instance().reg();


  done_init = true;
}



// SlabTool::run()

bool SlabTool::run(const JParser& prm) {

    // basic initialisation common to all tools
  if (Tool::init(prm)) return true;
  Tool::initRandom(prm);

  if (jlog::vbLevel(5)) prm.displayList(jlog::ls);

  uncancelProcessing();

  processing_done = false;

    // get output band specification or quit with error
  delete master_bs;
  master_bs = BandSpec::create(prm, "");
  if (!master_bs) { jlog::es << "No output band specification given or it is incomplete.\n"; return false; }
  band_count = master_bs->bandCount();
  
  if(!master_bs->hasWavelengthData()) {
    jlog::es << "Band specification has no wavelength data\n";
    return false;
  }

    // get output directional specification or quit with error
  delete master_ds_spec;
  master_ds_spec = DirecSpec::create(prm, "");
  if (!master_ds_spec) { jlog::es << "No output directional specification given or it is incomplete.\n"; return false; }

    // create a half-directional structure
  delete master_ds;
  master_ds = buildDirecStruct(*master_ds_spec, false);
    // check the type - only spherical partitions supported
  if (!master_ds->isSameType(HemiSpherePartVxH())) { jlog::es << "Unsupported directional structure."; return false; } 
    // now create a full directional structure as is how the outputs are done
  //master_full_ds = master_ds->createFull(); 
  master_full_ds = buildDirecStruct(*master_ds_spec, true);

    // start timing how long processing takes 
  startTiming();

  if (!calcOneSolution(prm)) return false;

    // report solution time
  if (jlog::ls.vbLevel(3)) jlog::ls.printf("Total Processing Time %.2f s\n",((double)runningTimeMsec())/1000);

  return true;
}



// SlabTool::calcOneSolution() [private]
// calculate solution for a single combination of slab and boundary coditions

bool SlabTool::calcOneSolution(const JParser& prm) {

    // get file path for the report
  JString filepath = prm.getStringOrEmpty("report_save_fp");
  if (filepath.isEmpty()) filepath = "report.txt";
     // start to write the report text file
  rep = new Report();
  if (!rep->openFile(filepath))  { jlog::es << "Error writing report file '" << filepath << "'.\n"; delete rep; rep=0; }
  
  if (rep) {
    JString s = prm.getStringOrEmpty("name");
    rep->stream() << "name" << rep->majorSep() << "-" << rep->majorSep() << s << "\n\n"; 
    master_bs->writeToReport(*rep);
    rep->stream() << "\n";

    const SpherePartition* sp = dynamic_cast<const SpherePartition*>(master_full_ds);

    rep->stream() << "# hemisphere directional specification\n";
    rep->stream() << "vn" << rep->majorSep() << "-" << rep->majorSep() << sp->vertSize() << "\n";
    rep->stream() << "hn" << rep->majorSep() << "-" << rep->majorSep() << sp->horizSize() << "\n";
    rep->stream() << "theta_points_deg" << rep->majorSep() << "-";
    for (unsigned int i=0; i<sp->thetaPointsDeg().size(); i++) rep->stream() << rep->majorSep() << sp->thetaPointsDeg()[i];
    rep->stream() << "\n";
  }


    // try to load sky boundary condition and quit if error
  if (!sky.load(prm, "sky", "sky", *master_ds, *master_bs)) return false;
    // info
  if (jlog::vbLevel(3)) jlog::ls << "Sky planar irradiance: [ " << sky.emis().planarIrradiance() << " ]\n";

    // try to load substrate boundary reflectance and quit if error
  if (!subs.load("bottom_reflec", prm, *master_ds, *master_bs)) return false;
    // info
  if (jlog::vbLevel(3)) jlog::ls << "Bottom diffuse reflectance: [ " << subs.diffuseReflectance() << " ]\n";
 
    // try to load water surface interface
  if (!water_surface.load("water_surface", prm, *master_ds, band_count)) return false;
    // info
  if (jlog::vbLevel(3)) {
    jlog::ls << "Surface diffuse reflec z+ to z-    : [ " << water_surface.reflecZpZm().diffuseReflectance() << " ]\n";
    jlog::ls << "Surface diffuse trans z+ direction : [ " << water_surface.transZp().diffuseReflectance() << " ]\n";
    jlog::ls << "Surface diffuse reflec z- to z+    : [ " << water_surface.reflecZmZp().diffuseReflectance() << " ]\n";
    jlog::ls << "Surface diffuse trans z- direction : [ " << water_surface.transZm().diffuseReflectance() << " ]\n";
  }

    // clear any previous allocation for outputs
  clearOutputAlloc();

    // Ld_a is sky downwelling radiance
    // and we take the radiance directional structure from the sky
  Ld_a = sky.emis().duplicate();
  Lu_a = sky.emis().duplicateStructureAndDataType();
  Ld_w = sky.emis().duplicateStructureAndDataType();
  Lu_w = sky.emis().duplicateStructureAndDataType();
  Ld_b = sky.emis().duplicateStructureAndDataType();
  Lu_b = sky.emis().duplicateStructureAndDataType();
  Ld_it = sky.emis().duplicateStructureAndDataType();
  Lu_it = sky.emis().duplicateStructureAndDataType();
  Ld_ir = sky.emis().duplicateStructureAndDataType();
  Lu_ir = sky.emis().duplicateStructureAndDataType();

    // create storage structure for outputs
  slab_soln = new SlabSoln<SBandData, SpherePartVxH>(*master_full_ds, band_count);
 
  int depth_code_prec = 1;
  if (prm.isDefined("depth_code_prec")) depth_code_prec = prm.getInt("depth_code_prec");
  slab_soln->setDepthCodePrecision(depth_code_prec);
  
    // get solution method - default is integrate
  JString soln(prm.getStringOrEmpty("solution"));
  if (soln.isEmpty()) soln = "integrate";

  if (soln == "basic_integrate") {

      // integrate substrate radiance up from the bottom
    if (!integrateSolutionBasic(prm)) return false;

  } else if (soln == "integrate") {

      // integrate substrate radiance up from the bottom
    if (!integrateSolutionDFT(prm)) return false;

  } else if (soln == "bounce") {

      // a slab must be loaded or calculated
    if (!prm.isDefined("slab_load_fp")) {    
        // try to build slab and quit if failed
      if (!buildSlab(prm)) return false;

      // otherwise try to load a slab
    } else {
      jlog::es << "Slab load not yet implemented\n"; 
      return false;
    }

      // now calculate for the current inputs
    if (!calcSolutionBounce()) return false;

  } else {

    jlog::es << "Solution method '" << soln << "' not supported.\n";
    return false;
  }

    // calculate reflectances etc. and possibly display
  slab_soln->calcOutputs();
    // build notes using function in Tool
  JTextDevice nd;
  buildNotes(nd, prm);
    // save any requested data to files
  slab_soln->saveOutputs(prm, *master_bs, nd.text());
    // write the report text file and close
  if (rep) {
    slab_soln->writeToReport(*rep);
    rep->closeFile();
  }

  return true;
}



// SlabTool::operationBuildSlab()

bool SlabTool::buildSlab(const JParser& prm) {

  if (jlog::ls.vbLevel(3)) jlog::ls << "Building slab...\n";
 
  SlabIntegrator_Base* slab_int(0);

    // check if want to build a slab from an inhomogenous column
  if (prm.isDefinedAndTrueBool("inhomogenous_column")) {
  
    // PlaneSlab<SBandData, IOPProfile<SBandData, HemiSpherePartVxH>, HemiSpherePartVxH>* ps = 
    //    new PlaneSlab<SpectralData, IOPProfile<SBandData,  HemiSpherePartVxH>, HemiSpherePartVxH>(*hds, master_bs->bandCount());
    
    // ps->iopData().loadFromParameters(prm);
    //slab = ps;
  
    jlog::es << "Inhomogenous columns not supported in this version."; 
    return false;
  
  } else {
    
    SlabIntegrator<SBandData, IOPSet<SBandData, HemiSpherePartVxH>, HemiSpherePartVxH>* si = \
      new SlabIntegrator<SBandData, IOPSet<SBandData,  HemiSpherePartVxH>, HemiSpherePartVxH>(*master_ds, *master_bs);

      // try to initialise from parameters, quit if something went wrong
    if (!si->iopData().loadFromParameters(prm)) return false;    
      // cast to base class PlaneSlab_Base
    slab_int = si;
  }
  
    // set number of sub-slabs to calculate - default is just one with sample points at the top and bottom
  double sample_distance = slab_int->physicalThickness();
  if (prm.isDefined("sample_point_distance")) sample_distance = prm.getDouble("sample_point_distance");
    // or get sample points from (i.e. dstances from 0 boundary ?)
  if (prm.isDefined("sample_point_list")) {
    std::vector<double> sd_list = prm.getVectorOfDoubles("sample_point_list");
    if (jlog::ls.vbLevel(3)) jlog::ls << "Sample point list: " << sd_list << "\n";
    slab_int->setSamplePoints(sd_list);
  } else {
    if (jlog::ls.vbLevel(3)) jlog::ls << "Sample point distance: " << sample_distance << "\n";
    slab_int->setRegularSamplePoints(sample_distance);
  }

    // the number of precision digits to append on the depth for file names
  int depth_code_prec = 1;
  if (prm.isDefined("depth_code_prec")) depth_code_prec = prm.getInt("depth_code_prec");

    // if this is set then don't bother saving the matrices for the zero thickness sample point
  bool ignore_zero_thickness = prm.isDefined("ignore_zero_thickness") ? prm.getBool("ignore_zero_thickness") : false;

  //RadDirecDirecDFT<SBandData, HemiSpherePartVxH> rtemp(*master_ds, master_bs->bandCount());
  //RadDirecDirecTable<SBandData, HemiSpherePartVxH> rtab(*master_ds, master_bs->bandCount());

    // integrator type
  if (!setIntegrator(*slab_int, prm)) return false;

    // if homogeneous column only need to do transmission and reflection in one direction
  slab_int->setCalcRA(true);
  slab_int->setCalcTA(true);
  slab_int->setCalcRB(!slab_int->slabIsHomogeneous());
  slab_int->setCalcTB(!slab_int->slabIsHomogeneous());

    // caclculate slab reflectances and transmissions
  if (jlog::ls.vbLevel(3)) jlog::ls << "Integrating slab...\n";
  slab_int->calcSolution();

  JString filebase(prm.getStringOrEmpty("subslab_save_base_fp"));

    // the zero thickness matrices may not be of interest
  int i_start = ignore_zero_thickness ? 1 : 0;

    // use each sample point in the water column to simulate water columns of different depths
    // this only works for homegenous water columns
    // loop for each sample point in the slab including the top and bottom boundaries
  for (int i=i_start; i<slab_int->samplePointCount(); i++) {

      // get the slab from the intergrator 
    slab_int->buildSlab(sb, i);

    //writeSlabFile(f, mRup, mTup, mRdown, mTdown);

    sb.mRup.display(jlog::ls);
    RadDirecDirecDFT<SBandData, HemiSpherePartVxH> rt_dft(*master_ds, band_count);
    rt_dft.copyFromMatrixDFT(sb.mRup);
    RadDirecDirecTable<SBandData, HemiSpherePartVxH> rt(*master_ds, band_count);
    rt.copy(rt_dft);
    rt.display(jlog::ls);

    if (!filebase.isEmpty()) {
      JString f = filebase + "_" + JString::number(slab_int->samplePointDistanceFromA(i),'f', depth_code_prec) + "m";
      //writeSlabFile(f, sb.mRup, sb.mTup);
    }

    if (jlog::vbLevel(4)) {

      RadDirecTable<SBandData, HemiSpherePartVxH> rad(*master_ds, master_bs->bandCount());
        // make isotropic (diffuse) radiance equal over the sphere
      rad.makeAll(SBandData(master_bs->bandCount(), 1.0/M_PI));
      jlog::ls << "Sample point distance from upper boundary: " << slab_int->samplePointDistanceFromA(i) << "\n";
      displaySlab("  top side (z- side) diffuse reflectance:    ", rad, sb.mRup);
      displaySlab("  upward (z- direc) diffuse transmission:    ", rad, sb.mTup);
      displaySlab("  bottom side (z+ side) diffuse reflectance: ", rad, sb.mRdown);
      displaySlab("  downward (z+ direc) diffuse transmission:  ", rad, sb.mTdown);
    }
  }

    // delete the slab integrator object
  delete slab_int;
 
  return true;
}



// SlabTool::calcSolution() [private]
// calculate one solution as set up

bool SlabTool::calcSolutionBounce() {

    // start Ld_w estimation as sky radiance transferred through water surface 
  Ld_w->makeAll(SBandData(master_bs->bandCount(), 0));
  water_surface.transZp().translateInToOut(*Ld_a, *Ld_w);
  
    // start Lu_b and Ld_b estimation as zero
  Lu_b->makeAll(SBandData(master_bs->bandCount(), 0));
  Ld_b->makeAll(SBandData(master_bs->bandCount(), 0));

    // zero these to make sure their spectral data is allocated in right number of bands
  Lu_w->makeAll(SBandData(master_bs->bandCount(), 0));
  Lu_a->makeAll(SBandData(master_bs->bandCount(), 0));
  
  int max_iters = 50;
  double iter_ppu_frac = 0.0001;

    // initial power per unit area
  double curr_ppu = Ld_b->planarIrradiance().sumOfBandValues();
  double prev1_ppu = 0;
  double prev2_ppu = 0;
  double dp;
 
  if (jlog::vbLevel(5)) jlog::ls << "Initial Ld_b ppu " << curr_ppu << "\n";

    // for convergence use the spectral irradiance at the top and bottom of the column
  //SBandData prev_Ed_b(master_bs->bandCount(), 0);
  //SBandData prev_Eu_w(master_bs->bandCount(), 0);
  //SBandData prev_delta_Ed_b;
  //SBandData prev_delta_Eu_w;
 
  int iters = 0;
    
  do {

    prev2_ppu = prev1_ppu;
    prev1_ppu = curr_ppu;
      
      // convert boundary radiances into dft matrix representation
    Ld_w->copyToMatrixDFT(Ld_w_ma);
    Lu_b->copyToMatrixDFT(Lu_b_ma);

      // upward radiance at water surface boundary is Ld_w reflected plus Lu_b transmitted
    sb.mRup.translateInToOut(Ld_w_ma, L_temp1_ma);
    sb.mTup.translateInToOut(Lu_b_ma, Lu_w_ma);
    Lu_w_ma.add(L_temp1_ma);
    Lu_w->copyFromMatrixDFT(Lu_w_ma);
      
      // downward radiance at substrate boundary is Lu_b reflected plus Ld_w transmitted
    sb.mRdown.translateInToOut(Lu_b_ma, L_temp1_ma);
    sb.mTdown.translateInToOut(Ld_w_ma, Ld_b_ma);
    Ld_b_ma.add(L_temp1_ma);
    Ld_b->copyFromMatrixDFT(Ld_b_ma);
  
      // recalculate Ld_w estimation with new reflection from underside of water surface
    Ld_w->makeAll(SBandData(master_bs->bandCount(), 0));
    water_surface.transZp().translateInToOut(*Ld_a, *Ld_w);
    water_surface.reflecZmZp().translateInToOut(*Lu_w, *Ld_w);
      
      // recalculate Lu_b estimation as substrate reflected Ld_b
    Lu_b->makeAll(SBandData(master_bs->bandCount(), 0));
    subs.directionalReflectance().translateInToOut(*Ld_b, *Lu_b);

      // new power per unit area under water surface
    //if (master_bs->hasWavelengthData()) curr_ppu = master_bs->integrateOverWavelength(Ld_b->planarIrradiance());
    //else

    //SD curr_Ed_b(Ld_b->planarIrradiance());
    //double curr_frac = norm_fabs_max((curr_Ed_b - prev_Ed_b) / curr_Ed_b);

    //if (jlog::vbLevel(5)) jlog::ls << "Iter " << iters+1 << " ";
 
    //SBandData curr_Ed_b(Ld_b->planarIrradiance());
    //SBandData curr_Eu_w(Lu_w->planarIrradiance());
    
      // on the first interation get the first calculated irradiance at the bottom
    //if (iters == 0) {
        // the first change from zero
    //  prev_delta_Ed_b = curr_Ed_b - prev_Ed_b;
    //  prev_delta_Eu_w = curr_Eu_w - prev_Eu_w;
    //    if (jlog::vbLevel(5)) jlog::ls << "(n/a)\n";
    // } else {
        // change this time
    // SBandData curr_delta_Ed_b(curr_Ed_b - prev_Ed_b);
    //  SBandData curr_delta_Eu_w(curr_Eu_w - prev_Eu_w);
  
    // double Ed_b_frac = norm_fabs_max(((curr_delta_Ed_b - prev_delta_Ed_b) / curr_Ed_b).vectorData());
    //  double Eu_w_frac = norm_fabs_max(((curr_delta_Eu_w - prev_delta_Eu_w) / curr_Eu_w).vectorData());
    //  double frac = max(Ed_b_frac, Eu_w_frac);
    //   if (jlog::vbLevel(5)) jlog::ls << "(" << frac << ")\n";
    // }
 
    //prev_Ed_b = curr_Ed_b;
    //prev_Eu_w = curr_Eu_w;
 
    //    dp = iter_ppu_frac;
    //  if (jlog::vbLevel(5)) jlog::ls << "(n/a)\n";
    // } else {
    //  dp = (curr_ppu-prev2_ppu) / prev2_ppu;
    //  if (jlog::vbLevel(5)) jlog::ls << "(" << dp << ")\n";
    // }
    
      // new power per unit area under water surface
    curr_ppu = Ld_b->planarIrradiance().sumOfBandValues();
    if (jlog::vbLevel(5)) jlog::ls << "Iter " << iters+1 << " Ld_b ppu " << curr_ppu << " ";
    if (prev2_ppu == 0) {
      dp = iter_ppu_frac;
      if (jlog::vbLevel(5)) jlog::ls << "(n/a)\n";
    } else {
      dp = (curr_ppu-prev2_ppu) / prev2_ppu;
      if (jlog::vbLevel(5)) jlog::ls << "(" << dp << ")\n";
    }
    
  } while (iters++ < max_iters && dp >= iter_ppu_frac);
  //} while (iters++ < max_iters && frac >= iter_ppu_frac);

    // transfer light upward through water surface and bounce sky off water surface
  Lu_a->makeAll(SBandData(master_bs->bandCount(), 0));
  water_surface.transZm().translateInToOut(*Lu_w, *Lu_a);
  water_surface.reflecZpZm().translateInToOut(*Ld_a, *Lu_a);
    // water leaving radiance
  Lu_it->makeAll(SBandData(band_count, 0));
  water_surface.transZm().translateInToOut(*Lu_w, *Lu_it);
    // water reflected radiance
  Lu_ir->makeAll(SBandData(band_count, 0));
  water_surface.reflecZpZm().translateInToOut(*Ld_a, *Lu_ir);
  
  return true;
}



// SlabTool::integrateSolution() [private]
// Calculates the light solution using a SlabIntegrator with supplied internal reflectance at boundary B

bool SlabTool::integrateSolutionBasic(const JParser& prm) {

    // cast the directional structure to the type it currently must be
    // to support other types this function could be templatized
  const HemiSpherePartVxH* ds = dynamic_cast<const HemiSpherePartVxH*>(master_ds);
  if (!ds) { jlog::es << "Wrong directional structure\n"; return false; }
 
    // set up slab integrator
  PlaneSolver<SBandData, IOPSet<SBandData, HemiSpherePartVxH>, HemiSpherePartVxH> slab_int(*master_ds, *master_bs);
    // try to initialise from parameters, quit if something went wrong
  if (!slab_int.iopData().loadFromParameters(prm)) return false;

    // iops for report
  if (rep) slab_int.iopData().writeToReport(*rep);

    // set integrator type
  JString s(prm.getStringOrEmpty("integrator"));
  if (s.isEmpty() || s == "euler") slab_int.setIntegrationMethod(PlaneSolver_Base::EULER);
  else if (s == "midpoint") slab_int.setIntegrationMethod(PlaneSolver_Base::MIDPOINT);
  else {
    jlog::es << "Unsupported integration method '" << s << "' for basic integration.\n";
    return false;
  }

  if (prm.isDefined("basic_usteps_per_optical_depth")) {
    int n = prm.getInt("basic_usteps_per_optical_depth");
    if (jlog::ls.vbLevel(3)) jlog::ls << "basic usteps per optical depth: " << n << "\n";
    slab_int.setUpwardStepsPerOpticalDepth(n);
  }

  if (prm.isDefined("basic_dsteps_per_optical_depth")) {
    int n = prm.getInt("basic_dsteps_per_optical_depth");
    if (jlog::ls.vbLevel(3)) jlog::ls << "basic dsteps per optical depth: " << n << "\n";
    slab_int.setDownwardStepsPerOpticalDepth(n);
  }

    // info
  if (jlog::ls.vbLevel(3)) jlog::ls << "Calculate solution...\n";
    // calculate slab reflectance
  slab_int.calcSolution(sky, water_surface, subs, *slab_soln);

  Ld_a->copy(slab_int.solnLdAir());
  Lu_a->copy(slab_int.solnLuAir());
  Ld_w->copy(slab_int.solnLdWater());
  Lu_w->copy(slab_int.solnLuWater());
  Ld_b->copy(slab_int.solnLdBottom());
  Lu_b->copy(slab_int.solnLuBottom());

    // calc water leaving radiance
  Lu_it->makeAll(SBandData(band_count, 0));
  water_surface.transZm().translateInToOut(*Lu_w, *Lu_it);
    // calc water reflected radiance
  Lu_ir->makeAll(SBandData(band_count, 0));
  water_surface.reflecZpZm().translateInToOut(*Ld_a, *Lu_ir);

  return true;
}



// SlabTool::integrateSolution() [private]
// Calculates the light solution using a SlabIntegrator with supplied internal reflectance at boundary B

bool SlabTool::integrateSolutionDFT(const JParser& prm) {

  //depth_points_todo = band_count * ((delta_interval > 0) ? slab_int.samplePointCount()*3 -2 : slab_int.samplePointCount());
  depth_points_done = 0;
  depth_points_todo = 1;
  inversions_done = 0;
  inversions_todo = 1;

  dft_pct1 = 0;
  dft_pct2 = 0;

    // cast the directional structure to the type it currently must be
    // to support other types this function could be templatized
  const HemiSpherePartVxH* ds = dynamic_cast<const HemiSpherePartVxH*>(master_ds);
  if (!ds) { jlog::es << "Wrong directional structure\n"; return false; }
 
    // set up slab integrator
  SlabIntegrator<SBandData, IOPSet<SBandData, HemiSpherePartVxH>, HemiSpherePartVxH> slab_int(*master_ds, *master_bs);
  slab_int_ptr = &slab_int;
    // try to initialise from parameters, quit if something went wrong
  if (!slab_int.iopData().loadFromParameters(prm)) { slab_int_ptr=0; return false; }    
    // iops for report
  if (rep) slab_int.iopData().writeToReport(*rep);
    // integrator type
  if (!setIntegrator(slab_int, prm)) { slab_int_ptr=0; return false; }
  
  //if (!slab_int_ptr) exit(1);
  if (!slab_int_ptr) return false;

    // load the bottom reflectance into the slab
    // must be a rotationally invariant function
  slab_int.loadInternalReflectanceB(subs.directionalReflectance());

    // set the sample points
  if (!setSamplePoints(slab_int, prm)) { slab_int_ptr=0; return false; }

  depth_points_todo = (slab_int.samplePointDeltaDistance() != 0) ? slab_int.samplePointCount()*3 -2 : slab_int.samplePointCount();

    // can disable calculations at sample points with this option - not much use at the moment
  bool do_sample_points = true;
  // if (prm.isDefined("do_sample_points")) do_sample_points = prm.getBool("do_sample_points");

    // only need RA and TA if looking at in-water radiances
    // FIXME - don't need to do RA and TA if not got multiple incident irradiances
    // faster to integrate the light
  slab_int.setCalcRA(do_sample_points);
  slab_int.setCalcTA(do_sample_points);
  slab_int.setCalcRB(true);
  slab_int.setCalcTB(false);

  if (!slab_int_ptr) exit(1);

    // info
  if (jlog::ls.vbLevel(3)) jlog::ls << "Calculate solution...\n";
    // calculate slab reflectance
  slab_int.calcSolution();

  //processing_slab_complete = true;

    // for holding reflectances
  mat* mR1 = new mat[band_count];
  mat* mR2 = new mat[band_count];
 
  mat* mR11 = new mat[band_count];
  mat* mR12 = new mat[band_count];
  mat* mR21 = new mat[band_count];
  mat* mR22 = new mat[band_count];
 
  mat* mT = new mat[band_count];
  mat* mR = new mat[band_count];

  mat* n1 = new mat[band_count];
  mat* n2 = new mat[band_count];

  mat* p1 = new mat[band_count];
  mat* p2 = new mat[band_count];

  mat* u1 = new mat[band_count];
  mat* u2 = new mat[band_count];

  mat* q1 = new mat[band_count];
  mat* q2 = new mat[band_count];

  mat* rn1 = new mat[band_count];
  mat* rn2 = new mat[band_count];

  mat* m1 = new mat[band_count];
  mat* m2 = new mat[band_count];
  
    // pre-size matrices so can use noalias() boost function
  for (int band=0; band<band_count; band++) {
    n1[band].resize(ds->rowsDFT()*ds->lmNumDFT(), ds->rowsDFT()*ds->lmNumDFT());
    n2[band].resize(ds->rowsDFT()*ds->lmNumDFT(), ds->rowsDFT()*ds->lmNumDFT());
  }

  if (!do_sample_points) {
    if (jlog::vbLevel(3)) jlog::ls << "Not doing sample points.\n";
    slab_int_ptr = 0;
    return true;
  }

  if (jlog::ls.vbLevel(3)) jlog::ls << "Doing sample points calculations...\n";
  
    // allocate array of pointers for sample point data
  array_size = slab_int.samplePointCount();
  sample_point_depth.resize(array_size);
  Ld_array = new RadDirecTable<SBandData, HemiSpherePartVxH>*[array_size];
  Lu_array = new RadDirecTable<SBandData, HemiSpherePartVxH>*[array_size];
    // and optionally for rate of change at sample points
  if (slab_int.samplePointDeltaDistance() != 0) {
    dLd_dz_array = new RadDirecTable<SBandData, HemiSpherePartVxH>*[array_size];
    dLu_dz_array = new RadDirecTable<SBandData, HemiSpherePartVxH>*[array_size];
  }
    // create structures
  for (int i=0; i<array_size; i++) {
    Ld_array[i] = new RadDirecTable<SBandData, HemiSpherePartVxH>(*ds, band_count);
    Ld_array[i]->setDataTypeMeanRad();
    Lu_array[i] = new RadDirecTable<SBandData, HemiSpherePartVxH>(*ds, band_count);
    Lu_array[i]->setDataTypeMeanRad();
    sample_point_depth[i] = slab_int.samplePointDistanceFromA(i);
    if (slab_int.samplePointDeltaDistance() != 0) {
      dLd_dz_array[i] = new RadDirecTable<SBandData, HemiSpherePartVxH>(*ds, band_count);
      dLd_dz_array[i]->setDataTypeMeanRad();
      dLu_dz_array[i] = new RadDirecTable<SBandData, HemiSpherePartVxH>(*ds, band_count);
      dLu_dz_array[i]->setDataTypeMeanRad();
    }
  }

  RadDirecDFT<SBandData, HemiSpherePartVxH> Ld_dft(*master_ds, band_count);
  RadDirecDFT<SBandData, HemiSpherePartVxH> Lu_dft(*master_ds, band_count);

  RadDirecTable<SBandData, HemiSpherePartVxH> L_temp1(*master_ds, band_count);
  RadDirecTable<SBandData, HemiSpherePartVxH> L_temp2(*master_ds, band_count);
  L_temp1.setDataTypeMeanRad();
  L_temp2.setDataTypeMeanRad();

  if (jlog::ls.vbLevel(3)) jlog::ls << "Building dft representation of transmission through surface from air to water...(SLOW)\n";
  
    // dft representation of transmission through surface from air to water
  RadDirecDirecDFT<SBandData, HemiSpherePartVxH> taw_dft(water_surface.transZp(), &dft_pct1, &dft_cancel);
  if (dft_cancel) { slab_int_ptr=0; return false; }
  taw_dft.buildWholeMatrices(mR11, mR12, mR21, mR22);

    // dft representation of downwelling sky radiance
  RadDirecDFT<SBandData, HemiSpherePartVxH> sky_dft(sky.emis());
  sky_dft.buildWholeMatrices(m1, m2);

    // multiply sky L by transmission through surface
  for (int band=0; band<band_count; band++) {
    p1[band] = prod(m1[band], mR11[0]) + prod(m2[band], mR21[0]);
    p2[band] = prod(m1[band], mR12[0]) + prod(m2[band], mR22[0]);
  }
   
    // display intial Ed_w below the surface from transission of the sky only 
  if (jlog::ls.vbLevel(3)) {
    Ld_dft.copyWholeMatrices(p1, p2);
    RadDirecTable<SBandData, HemiSpherePartVxH> Ld(Ld_dft);
    Ld.setDataTypeMeanRad();
    jlog::ls << "Ed_w from sky transission only: [ " << Ld.planarIrradiance() << " ]\n";
  }

    // dft representation of reflection of underside of water surface
  if (jlog::ls.vbLevel(3)) jlog::ls << "Building dft representation of reflection of underside of water surface...(SLOW)\n";
 
  RadDirecDirecDFT<SBandData, HemiSpherePartVxH> runder_dft(water_surface.reflecZmZp(), &dft_pct2, &dft_cancel); 
  if (dft_cancel) { slab_int_ptr=0; return false; }
  runder_dft.buildWholeMatrices(mR11, mR12, mR21, mR22);

    // identity matrix
  mat id_mat(identity_matrix<double>(ds->rowsDFT()*ds->lmNumDFT()));

  inversions_todo = slab_int.samplePointCount() * band_count;

    // for each sample point in the slab including the top and bottom boundaries
  for (int i=0; i<slab_int.samplePointCount(); i++) {
    
      // get transmission from water surface to this point
    slab_int.buildWholeMatricesTA(mT, i);
      // get reflectance of water column (and substrate) beneath this point
    slab_int.buildWholeMatricesRB1(mR1, i);
    slab_int.buildWholeMatricesRB2(mR2, i);
      // get reflectance of water column above this point
    slab_int.buildWholeMatricesRA(mR, i);
    
      // calculate multiple reflections between water column and surface underside
      // this is equivalent to equation 8.98 in L&W pg. 422
    for (int band=0; band<band_count; band++) {
 
        // info - shwo sample points at default verbosity
      if (jlog::ls.vbLevel(2)) jlog::ls.printf("Point %d (%s m) band %d\n", i, JString::number(slab_int.samplePointDistanceFromA(i)).latin1(), band);
      
        // multiply water column reflectance by underside of water surface
      if (i==0) {
	noalias(n1[band]) = prod(mR1[band], mR11[0]) + prod(mR2[band], mR21[0]);
	noalias(n2[band]) = prod(mR1[band], mR12[0]) + prod(mR2[band], mR22[0]);
      } else {
	noalias(n1[band]) = prod(mR1[band], mR[band]);
	noalias(n2[band]) = prod(mR2[band], mR[band]);
      }
        // then subtract from identity matrix and invert
      n1[band] = id_mat - n1[band];
      n2[band] = id_mat - n2[band];
        // do invertion
      if (!invert(n1[band], q1[band])) { 
	jlog::es << "Singular Matrix n1\n";
	//jlog::es << "R1\n " << mR1[band]; 
	//jlog::es << "R2\n " << mR2[band]; 
	slab_int_ptr=0; 
	return false; 
      }
      if (!invert(n2[band], q2[band])) { 
	jlog::es << "Singular Matrix n2\n";
	//jlog::es << "R1\n " << mR1[band]; 
	//jlog::es << "R2\n " << mR2[band]; 
	slab_int_ptr=0; 
	return false; 
      }
    
      if (i==0) {
	  // transmission through water surface multiplied by inverted matrix
	p1[band] = prod(p1[band], q1[band]);
	p2[band] = prod(p2[band], q2[band]);
	  // reflect by water column reflectance below to give upward distribution
	u1[band] = prod(p1[band], mR1[band]);
	u2[band] = prod(p2[band], mR2[band]);
	
      } else {
	  // transmit light from under water surface to this point
	rn1[band] = prod(p1[band], mT[band]);
	rn2[band] = prod(p2[band], mT[band]);
	rn1[band] = prod(rn1[band], q1[band]);
	rn2[band] = prod(rn2[band], q2[band]);
	u1[band] = prod(rn1[band], mR1[band]);
	u2[band] = prod(rn2[band], mR2[band]);
      }

      inversions_done++;
      //jlog::ls << "imversions " << inversions_done << " todo " << inversions_todo << "\n";

      if (processingCanceled()) { slab_int_ptr=0; return false; }
    }

      // get downwelling radiance at this point
    if (i==0) Ld_dft.copyWholeMatrices(p1, p2);
    else Ld_dft.copyWholeMatrices(rn1, rn2);
      // copy downwelling irradiance into stored output array
    Ld_array[i]->copy(Ld_dft);

      // get upwelling radiance at this point
    Lu_dft.copyWholeMatrices(u1, u2);
      // copy upwelling irradiance into stored output array
    Lu_array[i]->copy(Lu_dft);
  
      // show downwelling and upwelling irradiance this is useful to see at default verbosity
    if (jlog::ls.vbLevel(2)) {
      jlog::ls.printf("  Ed_%.1f: ", slab_int.samplePointDistanceFromA(i));
      jlog::ls << "[ " << Ld_array[i]->planarIrradiance() << " ]\n";
      jlog::ls.printf("  Eu_%.1f: ", slab_int.samplePointDistanceFromA(i));
      jlog::ls << "[ " << Lu_array[i]->planarIrradiance() << " ]\n";
    }
  
      // this means we have data points to calc Kd and Ku
      // this requires p1 and p2 to be set up as final total downwelling radiance below the water surface
    if (slab_int.samplePointDeltaDistance() != 0) {

      double dist;
      
        // at top sample point (water surface)
      if (i == 0) {
	  // can only do half the distance
	dist = slab_int.samplePointDeltaDistance() / 2;
	  // the ab_point of the lower radiance distribution 
	int dp2 = 1;
	  // upwelling and downwelling radiance at the sample point
	L_temp1.copy(*Lu_array[0]);
	L_temp2.copy(*Ld_array[0]);
   	  // upwelling and downwelling radiance just below the sample point
	calcLuAndLd(dp2, slab_int, mR11, mR12, mR21, mR22, p1, p2, dLu_dz_array[0], dLd_dz_array[0], *ds);
	depth_points_done += 1;

	// at bottom sample point (substrate)
      } else if (i == slab_int.samplePointCount()-1) {
	  // can only do half the distance
	dist = slab_int.samplePointDeltaDistance() / 2;
	  // the ab_point of the lower radiance distribution 
	int dp1 = i*3 - 1;
	  // upwelling and downwelling radiance just above the sample point
	calcLuAndLd(dp1, slab_int, mR11, mR12, mR21, mR22, p1, p2, &L_temp1, &L_temp2, *ds);
	  // upwelling and downwelling radiance at the substrate
	dLu_dz_array[i]->copy(*Lu_array[i]);
	dLd_dz_array[i]->copy(*Ld_array[i]);
	depth_points_done += 1;
	
	// not at end points
      } else {

	  // can do full distance
	dist = slab_int.samplePointDeltaDistance();
	  // the depth points above and below the sample points
	int dp1 = i*3 - 1;
	int dp2 = i*3 + 1;
	  // upwelling and downwelling radiance just above the sample point
	calcLuAndLd(dp1, slab_int, mR11, mR12, mR21, mR22, p1, p2, &L_temp1, &L_temp2, *ds);
	  // upwelling and downwelling radiance just below the sample point
	calcLuAndLd(dp2, slab_int, mR11, mR12, mR21, mR22, p1, p2, dLu_dz_array[i], dLd_dz_array[i], *ds);
	depth_points_done += 2;
      }
      
        // the change going downwards
      dLu_dz_array[i]->subtract(L_temp1);
      dLd_dz_array[i]->subtract(L_temp2);
        // the rate of change per m going downwards
      dLu_dz_array[i]->divideAllByScalar(dist);
      dLd_dz_array[i]->divideAllByScalar(dist);

        // show downwelling and upwelling delta irradiance this is useful to see at default verbosity
      if (jlog::ls.vbLevel(2)) {
	jlog::ls.printf("  dEd_%.1f: ", slab_int.samplePointDistanceFromA(i));
	jlog::ls << "[ " << dLd_dz_array[i]->planarIrradiance() << " ]\n";
	jlog::ls.printf("  dEu_%.1f: ", slab_int.samplePointDistanceFromA(i));
	jlog::ls << "[ " << dLu_dz_array[i]->planarIrradiance() << " ]\n";
      }
    }
  }

  // copy data into return soln structure

    // copy in boundary results
  slab_soln->setLWater(Ld_array[0], Lu_array[0]);
  slab_soln->setLBottom(Ld_array[slab_int.samplePointCount() - 1], Lu_array[slab_int.samplePointCount() - 1]);

    // transfer light upward through water surface and bounce sky off water surface
  Lu_a->makeAll(SBandData(master_bs->bandCount(), 0));
  water_surface.transZm().translateInToOut(*Lu_array[0], *Lu_a);
  water_surface.reflecZpZm().translateInToOut(*Ld_a, *Lu_a);
    // can now set air radiances
  slab_soln->setLAir(Ld_a, Lu_a);
 
    // water transmitted radiances
  Lu_it->makeZero();
  Ld_it->makeZero();
  water_surface.transZm().translateInToOut(*Lu_array[0], *Lu_it);
  water_surface.transZp().translateInToOut(*Ld_a, *Ld_it);
  slab_soln->setLAirWaterTransmitted(Ld_it, Lu_it);

    // water reflected radiances
  Lu_ir->makeZero();
  Ld_ir->makeZero();
  water_surface.reflecZpZm().translateInToOut(*Ld_a, *Lu_ir);
  water_surface.reflecZmZp().translateInToOut(*Lu_array[0], *Ld_ir);
  slab_soln->setLAirWaterReflected(Ld_ir, Lu_ir);

    // sample points
  slab_soln->setSamplePointCount(array_size);
  for (int i=0; i<array_size; i++) {
    slab_soln->setLSample(i, slab_int.samplePointDistanceFromA(i), Ld_array[i], Lu_array[i]);
    if (slab_int.samplePointDeltaDistance() != 0) {
      slab_soln->setDeltaLSample(i, dLd_dz_array[i], dLu_dz_array[i]);
    }
  }

  // old stuff

    // copy in boundary results
  Lu_w->copy(*Lu_array[0]);
  Ld_w->copy(*Ld_array[0]);
  Lu_b->copy(*Lu_array[slab_int.samplePointCount() - 1]);
  Ld_b->copy(*Ld_array[slab_int.samplePointCount() - 1]);

    // transfer light upward through water surface and bounce sky off water surface
  Lu_a->makeAll(SBandData(master_bs->bandCount(), 0));
  water_surface.transZm().translateInToOut(*Lu_w, *Lu_a);
  water_surface.reflecZpZm().translateInToOut(*Ld_a, *Lu_a);
    // water leaving radiance
  Lu_it->makeAll(SBandData(band_count, 0));
  water_surface.transZm().translateInToOut(*Lu_w, *Lu_it);
    // water reflected radiance
  Lu_ir->makeAll(SBandData(band_count, 0));
  water_surface.reflecZpZm().translateInToOut(*Ld_a, *Lu_ir);

  //Ld_b->subtract(*Ld_w);
  //jlog::ls << "t10:\n";
  //water_surface.t10().display(jlog::ls);
  //jlog::ls << "r_zpzm:\n";
  //water_surface.reflecZpZm().display(jlog::ls);
  //jlog::ls << "t_zp:\n";
  //water_surface.transZp().display(jlog::ls);

  delete[] mR1;
  delete[] mR2;
 
  delete[] mR11;
  delete[] mR12;
  delete[] mR21;
  delete[] mR22;
 
  delete[] mT;
  delete[] mR;

  delete[] n1;
  delete[] n2;

  delete[] p1;
  delete[] p2;

  delete[] u1;
  delete[] u2;

  delete[] q1;
  delete[] q2;

  delete[] rn1;
  delete[] rn2;
 
  delete[] m1;
  delete[] m2;
  
  slab_int_ptr = 0;
  return true;
}



// SlabTool::calcLuAndLd() [private]
// this duplicates a lot of the code SlabTool::integrateSolution()
// the redundacy should be removed but really rates of change should come directly from the integration
// so no point to fix this now

bool SlabTool::calcLuAndLd(int dp, SlabInt& slab_int, 
			   mat* mR11, mat* mR12, mat* mR21, mat* mR22,
			   const mat* p1, const mat* p2,
			   RadDirecTable<SBandData, HemiSpherePartVxH>* Lu, 
			   RadDirecTable<SBandData, HemiSpherePartVxH>* Ld, 
			   const HemiSpherePartVxH& ds) {

    // for holding reflectances
  mat* mR1 = new mat[band_count];
  mat* mR2 = new mat[band_count];

  mat* mT = new mat[band_count];
  mat* mR = new mat[band_count];

  mat* n1 = new mat[band_count];
  mat* n2 = new mat[band_count];

  mat* u1 = new mat[band_count];
  mat* u2 = new mat[band_count];

  mat* q1 = new mat[band_count];
  mat* q2 = new mat[band_count];

  mat* rn1 = new mat[band_count];
  mat* rn2 = new mat[band_count];

  RadDirecDFT<SBandData, HemiSpherePartVxH> Ld_dft(ds, band_count);
  RadDirecDFT<SBandData, HemiSpherePartVxH> Lu_dft(ds, band_count);

    // pre-size matrices so can use noalias() boost function
  for (int band=0; band<band_count; band++) {
    n1[band].resize(ds.rowsDFT()*ds.lmNumDFT(), ds.rowsDFT()*ds.lmNumDFT());
    n2[band].resize(ds.rowsDFT()*ds.lmNumDFT(), ds.rowsDFT()*ds.lmNumDFT());
  }

    // identity matrix
  mat id_mat(identity_matrix<double>(ds.rowsDFT()*ds.lmNumDFT()));
  
    // get transmission from water surface to this point
  slab_int.buildWholeMatricesTA(mT, dp, false);
    // get reflectance of water column (and substrate) beneath this point
  slab_int.buildWholeMatricesRB1(mR1, dp, false);
  slab_int.buildWholeMatricesRB2(mR2, dp, false);
    // get reflectance of water column above this point
  slab_int.buildWholeMatricesRA(mR, dp, false);
  
    // calculate multiple reflections between water column and surface underside
    // this is equivalent to equation 8.98 in L&W pg. 422
  for (int band=0; band<band_count; band++) {

    if (jlog::ls.vbLevel(5)) jlog::ls.printf("Delta sample point %d (%f m) band %d\n", slab_int.sampleDeltaPointDistanceFromA(dp), dp, band);

    noalias(n1[band]) = prod(mR1[band], mR[band]);
    noalias(n2[band]) = prod(mR2[band], mR[band]);
  
      // then subtract from identity matrix and invert
    n1[band] = id_mat - n1[band];
    n2[band] = id_mat - n2[band];
      // do invertion
    if (!invert(n1[band], q1[band])) { jlog::ls << "Singular Matrix"; return false; }
    if (!invert(n2[band], q2[band])) { jlog::ls << "Singular Matrix"; return false; }
      // transmit light from under water surface to this point
    rn1[band] = prod(p1[band], mT[band]);
    rn2[band] = prod(p2[band], mT[band]);
    rn1[band] = prod(rn1[band], q1[band]);
    rn2[band] = prod(rn2[band], q2[band]);
    u1[band] = prod(rn1[band], mR1[band]);
    u2[band] = prod(rn2[band], mR2[band]);

    //depth_points_done++;
  }
      
  Ld_dft.copyWholeMatrices(rn1, rn2);
  Ld->copy(Ld_dft);

  Lu_dft.copyWholeMatrices(u1, u2);  
  Lu->copy(Lu_dft);

  if (jlog::ls.vbLevel(5)) {
    jlog::ls.printf("  Ed_%.4f: ", slab_int.sampleDeltaPointDistanceFromA(dp));
    jlog::ls << "[ " << Ld->planarIrradiance() << " ]\n";
    jlog::ls.printf("  Eu_%.4f: ", slab_int.sampleDeltaPointDistanceFromA(dp));
    jlog::ls << "[ " << Lu->planarIrradiance() << " ]\n";
  }

  delete[] mR1;
  delete[] mR2;

  delete[] mT;
  delete[] mR;

  delete[] n1;
  delete[] n2;

  delete[] u1;
  delete[] u2;

  delete[] q1;
  delete[] q2;
 
  delete[] rn1;
  delete[] rn2;

  return true;
}







  // this part does a brute-force iterative bounce between the water
  // column and the underside of the water surface
  /*
  printf("Prepare bounce...\n");

    // Ld_a is sky downwelling radiance
  Spec::copy(*Ld_a, *sky_d);

    // start Ld_w as sky radiance transferred through water surface 
  Ld_w->makeZero();
  Reflec::translateInToOut(*surf_t01, *Ld_a, *Ld_w);
  displayPlaneIrradiance("Sky Component Ed_w: ", *Ld_w);

    // start sum of upward reflected light as zero
  Lu_w->makeZero();
 
    // start off downwelling light as sky component at underside of surface
  Spec::copy(*L_temp1, *Ld_w);

  if (!prm.isDefined("slab_load_reflectance")) {

      // get reflectance of whole water column
    slab.buildWholeMatricesRB1_A(mR1);
    slab.buildWholeMatricesRB2_A(mR2);

  } else {

    JString f(prm.getString("slab_load_reflectance"));

    JFile file(f.latin1());
    if (!file.open(IO_ReadOnly)) internalError("File '%s' could not be read\n",f.latin1()); 

    printf("Reading %s\n",f.latin1());
      
    JDataStream str(&file);   
    for (int band=0; band<band_count; band++) {
      readMatrix(mR1[band], str);
      readMatrix(mR2[band], str);
    }
    file.close();
  }

    // initial power per unit area
  double ppu = L_temp1->integrateSurfaceRadiantPPU().integrateOverWavelength();
  printf("Iter 0 ppu %f\n",ppu);

  int iters = 0;
  double p;

  do {

      // bounce downwelling radiance back up from the water
      // convert L_temp1 to DFT representation
    SpecDFT<SpectralData, DS> temp_dft(*L_temp1);
      // convert L_temp1 DFT to matrices
    temp_dft.buildWholeMatrices(m1, m2);
      // bounce L_temp1 off slab reflectance
    for (int band=0; band<band_count; band++) {
      n1[band] = prod(m1[band], mR1[band]);
      n2[band] = prod(m2[band], mR2[band]);
    }
      // copy back to DFT
    temp_dft.copyWholeMatrices(n1, n2);
      // and to table radiance representation
    ((SpecTable<SpectralData, DS>*)L_temp2)->copy(temp_dft);
      // add onto total coming up from water
    Spec::add(*Lu_w, *L_temp2);

      // bounce upwelling radiance back down off the surface underside
    L_temp1->makeZero();
    Reflec::translateInToOut(*surf_r1, *L_temp2, *L_temp1); 
      // add onto total coming down from surface
    Spec::add(*Ld_w, *L_temp1);

      // remaining power per unit area
    p = L_temp1->integrateSurfaceRadiantPPU().integrateOverWavelength();
    printf("Iter %d ppu %f\n",iters+1,p);

  } while (iters++ < max_bounce_iters && p/ppu > bounce_ppu_frac);

    // now can calculate Lu_a
  Lu_a->makeZero();
  Reflec::translateInToOut(*surf_t10, *Lu_w, *Lu_a);
  Reflec::translateInToOut(*surf_r0, *Ld_a, *Lu_a); 

  displayPlaneIrradiance("Ed_a: ", *Ld_a);
  displayPlaneIrradiance("Eu_a: ", *Lu_a);
  displayPlaneIrradiance("Ed_w: ", *Ld_w);
  displayPlaneIrradiance("Eu_w: ", *Lu_w);
  displayTopRadiance("Lu_w: ", *Lu_w); 
  displayTopRadiance("Lu_a: ", *Lu_a); 

  if (prm.isDefined("rrs_output_file")) {

    SpectralData sd_lu;
    SpectralData sd_ed;

    calcTopRadiance(sd_lu, *Lu_a);
    calcPlaneIrradiance(sd_ed, *Ld_a);
    sd_lu.divide(sd_ed);

    JString f(prm.getString("rrs_output_file").latin1());

    JFile file(f);
    if (!file.open(IO_WriteOnly)) internalError("File '%s' could not be written\n",f.latin1()); 

    printf("Writing %s\n",f.latin1());
      
    JTextStream str(&file);   
    str << sd_lu;

    file.close();
  }

  return true;
  */





// SlabTool::calcOutputs() [private]
/*
void SlabTool::calcOutputs() {

  SBandData sd_ed_a;
  SBandData sd_ed_w;
  SBandData sd_eu;
  
    // upward radiance including water surface reflection
  sd_Rtot = Lu_a->atZPlus(); 
    // upward radiance above water not including water surface reflection
  sd_Rrs = Lu_wt->atZPlus(); 
    // downward plane irradiance above water 
  sd_ed_a = Ld_a->planarIrradiance();
    // Rrs and Rtot are division
  sd_Rrs.divide(sd_ed_a);
  sd_Rtot.divide(sd_ed_a);
    // upward plane irradiance above water surface
  sd_R0p = Lu_a->planarIrradiance();
  sd_R0p.divide(sd_ed_a);
  
    // upward plane irradiance below water surface
  sd_R0m = Lu_w->planarIrradiance();
    // upward radiance below water surface
  sd_rrs = Lu_w->atZPlus();
    // downward plane irradiance below water surface
  sd_ed_w = Ld_w->planarIrradiance();
  sd_rrs.divide(sd_ed_w);
  sd_R0m.divide(sd_ed_w);
  
  if (jlog::vbLevel(2)) {
    jlog::ls << "Ed_a: [ " << sd_ed_a << " ]\n";
    jlog::ls << "Ed_w: [ " << sd_ed_w << " ]\n";
    jlog::ls << "Ed_b: [ " << Ld_b->planarIrradiance() << " ]\n";
    jlog::ls << "rrs: [ " << sd_rrs << " ]\n";
    jlog::ls << "Rrs: [ " << sd_Rrs << " ]\n";
    jlog::ls << "Rtot: [ " << sd_Rtot << " ]\n";
    jlog::ls << "R0m: [ " << sd_R0m << " ]\n";
    jlog::ls << "R0p: [ " << sd_R0p << " ]\n";
  }

}
*/


// SlabTool::saveOutputs() [private]

//void SlabTool::saveOutputs(const JParser& prm) const {

  /*
    // spectral data text files for planar irradiances
  RadUtils::savePlanarIrradiance("Ed_b", "", *Ld_b, prm);
  RadUtils::savePlanarIrradiance("Eu_b", "", *Lu_b, prm);
  RadUtils::savePlanarIrradiance("Ed_a", "", *Ld_a, prm);
  RadUtils::savePlanarIrradiance("Eu_a", "", *Lu_a, prm);
  RadUtils::savePlanarIrradiance("Ed_w", "", *Ld_w, prm);
  RadUtils::savePlanarIrradiance("Eu_w", "", *Lu_w, prm);
  RadUtils::savePlanarIrradiance("Eu_wl", "", *Lu_wt, prm);
  RadUtils::savePlanarIrradiance("Eu_wr", "", *Lu_wr, prm);

    // spectral data text files for scalar irradiances
  RadUtils::saveScalarIrradiance("Eod_b", "", *Ld_b, prm);
  RadUtils::saveScalarIrradiance("Eou_b", "", *Lu_b, prm);
  RadUtils::saveScalarIrradiance("Eod_a", "", *Ld_a, prm);
  RadUtils::saveScalarIrradiance("Eou_a", "", *Lu_a, prm);
  RadUtils::saveScalarIrradiance("Eod_w", "", *Ld_w, prm);
  RadUtils::saveScalarIrradiance("Eou_w", "", *Lu_w, prm);
  RadUtils::saveScalarIrradiance("Eou_wl", "", *Lu_wt, prm);
  RadUtils::saveScalarIrradiance("Eou_wr", "", *Lu_wr, prm);

    // directional data table files for quad-averaged radiance
  RadUtils::saveRadiance("Ld_b", "", *Ld_b, prm);
  RadUtils::saveRadiance("Lu_b", "", *Lu_b, prm);
  RadUtils::saveRadiance("Ld_a", "", *Ld_a, prm);
  RadUtils::saveRadiance("Lu_a", "", *Lu_a, prm);
  RadUtils::saveRadiance("Ld_w", "", *Ld_w, prm);
  RadUtils::saveRadiance("Lu_w", "", *Lu_w, prm);
  RadUtils::saveRadiance("Lu_wt", "", *Lu_wt, prm);
  RadUtils::saveRadiance("Lu_wr", "", *Lu_wr, prm);
	
    // hemispherical projection image files
  RadUtils::saveHemisphereImage("Ld_b", "", *Ld_b, *master_bs, prm);
  RadUtils::saveHemisphereImage("Lu_b", "", *Lu_b, *master_bs, prm);
  RadUtils::saveHemisphereImage("Ld_a", "", *Ld_a, *master_bs, prm);
  RadUtils::saveHemisphereImage("Lu_a", "", *Lu_a, *master_bs, prm);
  RadUtils::saveHemisphereImage("Ld_w", "", *Ld_w, *master_bs, prm);
  RadUtils::saveHemisphereImage("Lu_w", "", *Lu_w, *master_bs, prm);
  RadUtils::saveHemisphereImage("Lu_wt", "", *Lu_wt, *master_bs, prm);
  RadUtils::saveHemisphereImage("Lu_wr", "", *Lu_wr, *master_bs, prm);

    // the number of precision digits to append on the depth
  int depth_code_prec = 1;
  if (prm.isDefined("depth_code_prec")) depth_code_prec = prm.getInt("depth_code_prec");

  for (int i=0; i<array_size; i++) {

      // what to append on the end of the filenames
    JString depth_postfix(JString::number(samplePointDepth(i), 'f', depth_code_prec) + "m");

    RadUtils::saveRadiance("Ld_sample", depth_postfix, *Ld_array[i], prm);
    RadUtils::saveRadiance("Lu_sample", depth_postfix, *Lu_array[i], prm);

    RadUtils::saveHemisphereImage("Ld_sample", depth_postfix, *Ld_array[i], *master_bs, prm);
    RadUtils::saveHemisphereImage("Lu_sample", depth_postfix, *Lu_array[i], *master_bs, prm);
  }
  */

//}

/*

void SlabTool::saveSamplePointIrradiance(const RadDirecImp<SBandData>* Ldata_array) {

  JTextStream str(&f);

    // for each sample point
  for (int i=0; i<array_size; i++) {

      // what to append on the end of the filenames
    JString depth_postfix(JString::number(samplePointDepth[i],'f', depth_postfix_prec) + "m");

    Ldata.scalarIrradiance()

    RadUtils::saveRadiance("Ld_", *Ld_array[i], prm);
 
  }
}

*/


// SlabTool::setSamplePoints() [private]

bool SlabTool::setSamplePoints(SlabIntegrator_Base& slab_int, const JParser& prm) {

    // default is sample points at top and bottom
  double sample_distance = slab_int.physicalThickness();
    // default is no extra points
  double delta_interval = 0;

    // if regular distance si requested
  if (prm.isDefined("sample_point_distance")) {
      // get the distance
    sample_distance = prm.getDouble("sample_point_distance");
      // and get if a delta distance is requested
    if (prm.isDefined("sample_point_delta_distance")) delta_interval = prm.getDouble("sample_point_delta_distance");
  }

    // or get sample points from a list (i.e. distances from 0 boundary)
  if (prm.isDefined("sample_point_list")) {
      // get the list
    std::vector<double> sd_list = prm.getVectorOfDoubles("sample_point_list");
      // make sure starts at zero
    if (sd_list[0] != 0) sd_list.insert(sd_list.begin(), 0);
      // make sure last point is at bottom
    if (sd_list[sd_list.size()-1] != slab_int.physicalThickness()) sd_list.push_back(slab_int.physicalThickness());
      // info
    if (jlog::ls.vbLevel(3)) jlog::ls << "Sample point list: " << sd_list << "\n";
       // and get if a delta distance is requested
    if (prm.isDefined("sample_point_delta_distance")) {
      delta_interval = prm.getDouble("sample_point_delta_distance");
      slab_int.setSamplePoints(sd_list, delta_interval);
    } else {
      slab_int.setSamplePoints(sd_list);
    }
  
    // if no list set as regular sample points
  } else {
    if (jlog::ls.vbLevel(3)) {
      jlog::ls << "Sample point distance: " << sample_distance << "\n";
      jlog::ls << "Sample point delta interval: " << delta_interval << "\n";
    }
    slab_int.setRegularSamplePoints(sample_distance, delta_interval);
  }  

  return true;
}



// void SlabTool::setIntegrator() [private]

bool SlabTool::setIntegrator(SlabIntegrator_Base& slab_int, const JParser& prm) {

    // solution method 
  JString s(prm.getStringOrEmpty("integrator"));

  if (s.isEmpty()) {
    slab_int.setIntegrationMethod(SlabIntegrator_Base::RUNGA4);
    slab_int.setRunga4StepsPerOpticalDepth(20);
    if (jlog::vbLevel(3)) jlog::ls.printf("Using default integration of 'runga4' 20 steps per optical depth.\n");
    return true;
  }

  if (s == "euler") {
    int v;
    if (!prm.getInt(v, "euler_steps_per_optical_depth")) return false;
    slab_int.setIntegrationMethod(SlabIntegrator_Base::EULER);
    slab_int.setEulerStepsPerOpticalDepth(v);
    if (jlog::vbLevel(3)) jlog::ls.printf("Integration 'euler' %d steps per optical depth.\n",v);

  } else if (s == "midpoint") {
    int v;
    if (!prm.getInt(v, "midpoint_steps_per_optical_depth")) return false;
    slab_int.setIntegrationMethod(SlabIntegrator_Base::MIDPOINT);
    slab_int.setMidpointStepsPerOpticalDepth(v);
    if (jlog::vbLevel(3)) jlog::ls.printf("Integration 'midpoint' %d steps per optical depth.\n",v);
    
  } else if (s == "runga4") {
    int v;
    if (!prm.getInt(v, "runga4_steps_per_optical_depth")) return false;
    slab_int.setIntegrationMethod(SlabIntegrator_Base::RUNGA4);
    slab_int.setRunga4StepsPerOpticalDepth(v);
    if (jlog::vbLevel(3)) jlog::ls.printf("Integration 'runga4' %d steps per optical depth.\n",v);

  } else if (s == "runga4adap") {
    int min_steps; 
    int max_steps;
    double min_error;
    double max_error;
    if (!prm.getInt(min_steps, "runga4adap_min_steps_per_optical_depth")) return false;
    if (!prm.getInt(max_steps, "runga4adap_max_steps_per_optical_depth")) return false;
    if (!prm.getDouble(min_error, "runga4adap_min_error")) return false;
    if (!prm.getDouble(max_error, "runga4adap_max_error")) return false;
    slab_int.setIntegrationMethod(SlabIntegrator_Base::RUNGA4ADAP);
    slab_int.setRunga4AdapMinStepsPerOpticalDepth(min_steps);
    slab_int.setRunga4AdapMaxStepsPerOpticalDepth(max_steps);
    slab_int.setRunga4AdapMinError(min_error);
    slab_int.setRunga4AdapMaxError(max_error);
    if (jlog::vbLevel(3)) jlog::ls.printf("Integration 'runga4adap' min steps %d max steps %d min error %f max error %f.\n",min_steps,max_steps,min_error,max_error);
   
  } else { 
    jlog::es << "Integration method '" << s.latin1() << "' not supported."; 
    return false;
  }

  return true;
}



// SlabTool::displaySlab()

void SlabTool::displaySlab(const JString& text, const RadDirecImp<SBandData>& rad, const RDD_MatrixDFT& m) {

  RD_MatrixDFT sm;
  RD_MatrixDFT sm_res;
  RadDirecImp<SBandData>* rad_res = rad.duplicate();

  rad.copyToMatrixDFT(sm);
  m.translateInToOut(sm, sm_res);

  rad_res->copyFromMatrixDFT(sm_res);
  SBandData sd(rad_res->sumBySolidAngleAbsCosTheta());

  delete rad_res;

  jlog::ls << text << sd << "\n";
}



// SlabTool::pctComplete()

double SlabTool::pctComplete() const {

  if (!slab_int_ptr) return (processing_done) ? 100 : 0;

  const double pass1_frac = 0.4;
  const double pass2_frac = 0.2;
  const double dft1_frac = 0.2;
  const double dft2_frac = 0.2;

  //jlog::ls << "HERE imversions " << inversions_done << " todo " << inversions_todo << "\n";

  return (pass1_frac * slab_int_ptr->pctSolutionComplete()) \
    + dft1_frac * dft_pct1 + dft2_frac * dft_pct2 \
    + (100.0 * pass2_frac * inversions_done) / inversions_todo;

   //+ ((pass2_frac * depth_points_done) / depth_points_todo) \
 
}



// SlabTool::helpText()

void SlabTool::helpText(JTextStream& str) const {

  str << "Output Band And Directional Structure Specification From Files\n";
  str << "--------------------------------------------------------------\n";
  str << "band_spec_fp  = <filepath>  (filepath of output band specification)\n";
  str << "direc_spec_fp = <filepath>  (filepath of output directional structure specification)\n";
  str << "\n";

  //str << "Slab Build Options\n";
  //str << "------------------\n";
  //str << "slab_thickness  = <real>  (physical thickness of the slab in metres)\n";
  //str << "\n";

  //str << "Output File Specification\n";
  //str << "-------------------------\n";
  //str << "slab_save_fp         = <filepath>  (file to save the full slab file to, optional)\n\n";
  //str << "subslab_save_base_fp = <base filepath>  (base of file path to save the sub-slab files to, optional)\n";
  //str << "\n";

  Tool::helpTextBandAndDirecSpec(str);
  str << "Output: (no prefix)\n";
 
  str << "Parameters For Direct Band Specification\n";
  str << "----------------------------------------\n";
  BandSpec::helpText(str);
  str << "\n";

  str << "Parameters For Direct Directional Structure Specification\n";
  str << "---------------------------------------------------------\n";
  DirecSpec::helpText(str);
  str << "\n";

  str << "Input Specification\n";
  str << "-------------------\n";
  str << "sky_fp                         = <filepath>\n";
  str << "bottom_reflec_fp               = <filepath>   BRDF\n";
  str << "bottom_reflec_diffuse_data     = <r1,r2, ... rn>   Lambertian direct specification\n";
  str << "bottom_reflec_diffuse_fp       = <filepath>       Lambertian from file\n";
  str << "bottom_reflec_diffuse_band_spec_fp       = <filepath>\n";

  str << "depth = <real>\n";
  str << "sample_point_distance = <real>\n";
  str << "sample_point_list = <r1,r2, ... rn>\n";
  str << "sample_point_delta_distance = <real>\n";
 
  str << "atten_fp = <filepath>\n";
  str << "atten_band_spec_fp = <filepath>\n";
  str << "absorp_fp = <filepath>\n";
  str << "absorp_band_spec_fp = <filepath>\n";
  str << "scat_fp = <filepath>\n";
  str << "scat_band_spec_fp = <filepath>\n";

  str << "integrator   = euler | midpoint | runga4 | runga4adap";


  str << "\n";
  str << "Saving Quad-Averaged Directional Radiance Outputs\n";
  str << "-------------------------------------------------\n";
  str << "Ld_a_save_fp       = <filepath>  (save downwelling spectral directional quad-averaged radiance above the water surface)\n";
  str << "Lu_a_save_fp       = <filepath>  (save upwelling spectral directional quad-averaged radiance above the water surface)\n";
  str << "Ld_w_save_fp       = <filepath>  (save downwelling spectral directional quad-averaged radiance just below the water surface)\n";
  str << "Lu_w_save_fp       = <filepath>  (save upwelling spectral directional quad-averaged radiance just below the water surface)\n";
  str << "Ld_b_save_fp       = <filepath>  (save downwelling spectral directional quad-averaged radiance at the water column bottom)\n";
  str << "Lu_b_save_fp       = <filepath>  (save upwelling spectral directional quad-averaged radiance at the water column bottom)\n";
  str << "Ld_it_save_fp      = <filepath>  (save downwelling spectral directional quad-averaged transmitted into water radiance)\n";
  str << "Lu_it_save_fp      = <filepath>  (save upwelling spectral directional quad-averaged water-leaving radiance)\n";
  str << "Ld_ir_save_fp      = <filepath>  (save downwelling spectral directional quad-averaged water underside reflected radiance)\n";
  str << "Lu_ir_save_fp      = <filepath>  (save upwelling spectral directional quad-averaged water-reflected radiance)\n";
  str << "Ld_sample_save_fp  = <filepath>  (save downwelling spectral directional quad-averaged radiance at the sample points)\n";
  str << "Lu_sample_save_fp  = <filepath>  (save upwelling spectral directional quad-averaged radiance at the sample points)\n";
  str << "\n";
  str << "Note: Directional outputs are saved as data files which can be converted to text outputs using the filetool.\n";
  str << "The sample point outputs, eg. 'Ld_sample', will save data files for each sample point with the depth ";
  str << "postfixed onto the supplied file name.\n";
  str << "\n";

  str << "Output Hemisphere Image Specification\n";
  str << "-------------------------------------\n";
  str << "Images of the hemispherical radiance distribution can be saved\n";
  str << "using parameters strings of the following format:\n\n";
  str << "<code>_image_save_fp  = <filepath>  (filepath to save hemisphere PPM image to, optional)\n";
  str << "<code>_image_size     = <integer>   (size of hemisphere PPM image in pixels)\n";
  str << "<code>_image_sens_k   = <real>      (sensitivity of radinace to RGB conversion, optional)\n\n";
  str << "where <code> is any of:\n\n";
  str << "  Ld_a       downwelling quad-averaged radiance above the water surface\n";
  str << "  Lu_a       upwelling quad-averaged radiance above the water surface\n";
  str << "  Ld_w       downwelling quad-averaged radiance just below the water surface\n";
  str << "  Lu_w       upwelling quad-averaged radiance just below the water surface\n";
  str << "  Ld_b       downwelling quad-averaged radiance at the water column bottom\n";
  str << "  Lu_b       upwelling quad-averaged radiance at the water column bottom\n";
  str << "  Ld_it      downwelling quad-averaged transmitted into water radiance\n";
  str << "  Lu_it      upwelling quad-averaged water-leaving radiance\n";
  str << "  Ld_ir      downwelling quad-averaged water under side reflected radiance\n";
  str << "  Lu_ir      upwelling quad-averaged water top side reflected radiance\n";
  str << "  Ld_sample  downwelling quad-averaged radiance at the sample points\n";
  str << "  Lu_sample  upwelling quad-averaged radiance at the sample points\n";
  str << "Image files a saved as ppm format with a '.ppm' extension appended if it is not already present.\n";
  str << "If sample point saves are requested the filenames will have the depth in meters appended before the '.ppm' extension.\n";
  str << "The number of digits after the decimal point used to code the depth for the filename\n";
  str << "defaults to two but can be set in the parameters by:\n\n";
  str << "depth_code_prec = <integer>\n";

  /*

  str << "Saving Planar Irradiance Outputs\n";
  str << "--------------------------------\n";
  str << "Ed_a_save_fp       - downwelling spectral directional quad-averaged radiance above the water surface.\n";
  str << "Eu_a_save_fp       - upwelling spectral directional quad-averaged radiance above the water surface.\n";
  str << "Ed_w_save_fp       - downwelling spectral directional quad-averaged radiance just below the water surface.\n";
  str << "Eu_w_save_fp       - upwelling spectral directional quad-averaged radiance just below the water surface.\n";
  str << "Ed_b_save_fp       - downwelling spectral directional quad-averaged radiance at the water column bottom.\n";
  str << "Eu_b_save_fp       - upwelling spectral directional quad-averaged radiance at the water column bottom.\n";
  str << "Eu_wl_save_fp      - upwelling spectral directional quad-averaged water-leaving radiance.\n";
  str << "Eu_wr_save_fp      - upwelling spectral directional quad-averaged water-reflected radiance.\n";
  str << "Ed_sample_save_fp  - downwelling spectral directional quad-averaged radiance at the sample points.\n";
  str << "Eu_sample_save_fp  - upwelling spectral directional quad-averaged radiance at the sample points.\n";
  str << "\n";
  str << "Note: Irradiance outputs will be saved as comma delimted text files.\n";
  str << "The sample point outputs, eg. 'Ed_sample_save_fp', will save a single comma delimited text file where each line is ";
  str << "a seperate sample point and the first column is the sample point depth.\n";
  str << "\n";

  str << "Saving Scalar Irradiance Outputs\n";
  str << "--------------------------------\n";
  str << "Eod_a_save_fp       - downwelling spectral directional quad-averaged radiance above the water surface.\n";
  str << "Eou_a_save_fp       - upwelling spectral directional quad-averaged radiance above the water surface.\n";
  str << "Eo_a_save_fp        - upwelling spectral directional quad-averaged radiance above the water surface.\n";
  str << "Eod_w_save_fp       - downwelling spectral directional quad-averaged radiance just below the water surface.\n";
  str << "Eou_w_save_fp       - upwelling spectral directional quad-averaged radiance just below the water surface.\n";
  str << "Eo_w_save_fp        - upwelling spectral directional quad-averaged radiance just below the water surface.\n";
  str << "Eod_b_save_fp       - downwelling spectral directional quad-averaged radiance at the water column bottom.\n";
  str << "Eou_b_save_fp       - upwelling spectral directional quad-averaged radiance at the water column bottom.\n";
  str << "Eo_b_save_fp        - upwelling spectral directional quad-averaged radiance at the water column bottom.\n";
  str << "Eou_wl_save_fp      - upwelling spectral directional quad-averaged water-leaving radiance.\n";
  str << "Eou_wr_save_fp      - upwelling spectral directional quad-averaged water-reflected radiance.\n";
  str << "Eod_sample_save_fp  - downwelling spectral directional quad-averaged radiance at the sample points.\n";
  str << "Eou_sample_save_fp  - upwelling spectral directional quad-averaged radiance at the sample points.\n";
  str << "Eo_sample_save_fp   - total scalar spectral irradiance at the sample points.\n";
  str << "\n";
  str << "Note: Scalar irradiance outputs will be saved as comma delimted text files.\n";
  str << "The sample point outputs, eg. 'Ed_sample_save_fp', will save a single comma delimited text file where each line is ";
  str << "a seperate sample point and the first column is the sample point depth.\n";
  */
  /*
  str << "Saving PAR Outputs\n";
  str << "------------------\n";
  str << "Each of the above types can be saved as units of PAR, umol quanta s-1 over the range 400-700 nm.\n";
  str << "To do this insert 'par_' before 'save' in the code. For example: \n";
  str << "Ed_a_par_save_fp = <filepath>  Save a text file of downwelling planar PAR at the bottom of the water column.\n";
  str << "Ed_a_par_save_fp = <filepath>  Save a data file of directional quad-averaged PAR at the bottom of the water column (units umolq.s-1sr-1m-2).\n";
  str << "Note that the PAR calculation integrates over wavelnegth so all outputs will be in one band. ";
  str << "Irradiances will be a single number, PAR in umol quanta s-1 m-2.\n";
  */

  Tool::helpText(str);
}








// the remainder is to do with batch running - needs fixing but is not required in the OS version


// SlabTool::operationCalcSolution()
/*
void SlabTool::operationCalcSolutions(const JParser& prm) {

  jlog::ls << "Operation: calc_solutions\n";

    // list of slab file bases or just one directly given
  JTextTable sky_file(2);
  if (!readInputList("sky_list", prm, sky_file)) {
    sky_file.setText(0,0,prm.getString("sky"));
    if (prm.isDefined("sky_text")) sky_file.setText(0,1,prm.getString("sky_text"));
  }
  jlog::ls << "sky:\n";
  sky_file.display();

    // list of slab file bases or just one directly given
  JTextTable input_slab_file(2);
  if (!readInputList("input_slab_file_list", prm, input_slab_file)) {
    input_slab_file.setText(0,0,prm.getString("input_slab_file"));
    if (prm.isDefined("input_slab_file_text")) input_slab_file.setText(0,1,prm.getString("input_slab_file_text"));
  }
  jlog::ls << "input_slab_file:\n";
  input_slab_file.display();

    // list of slab file appendices (i.e. depths) or just one directly given
  JTextTable input_slab_file_appendix(2);
  if (!readInputList("input_slab_file_appendix_list", prm, input_slab_file_appendix)) {
    input_slab_file_appendix.setText(0,0,prm.getString("input_slab_file_appendix"));
    if (prm.isDefined("input_slab_file_appendix_text")) input_slab_file_appendix.setText(0,1,prm.getString("input_slab_file_appendix_text"));
  }
  jlog::ls << "input_slab_file_appendix:\n";
  input_slab_file_appendix.display();

    // list of substrate reflectances (or just one directly given)
  JTextTable subs_diffuse_r0(2);
  if (!readInputList("subs_diffuse_r0_list", prm, subs_diffuse_r0)) {
    subs_diffuse_r0.setText(0,0,prm.getString("subs_diffuse_r0"));
    if (prm.isDefined("subs_diffuse_r0_text")) subs_diffuse_r0.setText(0,1,prm.getString("subs_diffuse_r0_text"));
  }
  double subs_count = subs_diffuse_r0.rowCount();
  jlog::ls << "subs_diffuse_r0:\n";
  subs_diffuse_r0.display();
  
    // list of water surface transfer functions or just one directly given
  JTextTable surf(5);
  if (!readInputList("surf_list", prm, surf)) {
    surf.setText(0, 0, prm.getString("surf_r0"));
    surf.setText(0, 1, prm.getString("surf_r1"));
    surf.setText(0, 2, prm.getString("surf_t01"));
    surf.setText(0, 3, prm.getString("surf_t10"));
    if (prm.isDefined("surf_text")) surf.setText(0,4,prm.getString("surf_text"));
  }
  jlog::ls << "surf:\n";
  surf.display();

    // for sky files
  for (int s5=0; s5<sky_file.rowCount(); s5++) {

      // load sky radiance structure
    sky.loadFromFile(sky_file.getText(s5,0));

    clearOutputAlloc();

      // Ld_a is sky downwelling radiance
      // and we take the radiance directional structure from the sky
    Ld_a = sky.emis().duplicate();
    Lu_a = sky.emis().duplicateStructure();
    Ld_w = sky.emis().duplicateStructure();
    Lu_w = sky.emis().duplicateStructure();
    Ld_b = sky.emis().duplicateStructure();
    Lu_b = sky.emis().duplicateStructure();
 
      // for slab file base names
    for (int s1=0; s1<input_slab_file.rowCount(); s1++) {

        // slab base file name
      JString slab_file_base(input_slab_file.getText(s1,0));

        // for each slab file appendix (i.e. depths)
      for (int s2=0; s2<input_slab_file_appendix.rowCount(); s2++) {

          // add slab file appendix onto file name
	JString slab_file(slab_file_base);
	slab_file.append(input_slab_file_appendix.getText(s2,0));
          // load slab file
	readSlabFile(slab_file, sb.mRup, sb.mTup, sb.mRdown, sb.mTdown);

          // for each substrate reflectance file
	for (int s3=0; s3<subs_count; s3++) {

	    // load substrate reflectance
	  subs.loadFromFile(subs_diffuse_r0.getText(s3,0), sky.emis().direcStruct());

	    // for each water surface function file set
	  for (int s4=0; s4<surf.rowCount(); s4++) {

	      // load water surface
	    water_surface.loadFromFiles(surf.getText(s4,0),surf.getText(s4,1),surf.getText(s4,2),surf.getText(s4,3)); 

	      // now calculate for the current inputs
	    calcSolution();

	    calcOutputs();

	      // directory tree structure for any output data
	    JString sub_dirs("/" + sky_file.getText(s5,1));
	    sub_dirs += "/" + surf.getText(s4,4);
	    sub_dirs += "/" + input_slab_file.getText(s1,1);
	    sub_dirs += "/" + input_slab_file_appendix.getText(s2,1); 

	      // substrate type which gets appended on to file name prefix
	    JString subs_name = subs_diffuse_r0.getText(s3,1);

	      // save any plane irradiance data if requested
	    saveIrradiance(prm, "Ed_b", sub_dirs, "Ed_b_"+subs_name, *Ld_b);
	    saveIrradiance(prm, "Eu_b", sub_dirs, "Eu_b_"+subs_name, *Lu_b);
	    saveIrradiance(prm, "Ed_w", sub_dirs, "Ed_w_"+subs_name, *Ld_w);
	    saveIrradiance(prm, "Eu_w", sub_dirs, "Eu_w_"+subs_name, *Lu_w);
	    saveIrradiance(prm, "Ed_a", sub_dirs, "Ed_a_"+subs_name, *Ld_a);
	    saveIrradiance(prm, "Eu_a", sub_dirs, "Eu_a_"+subs_name, *Lu_a);

	      // save any angular radiance data if requested
	    saveRadDirecData(prm, "Ld_b", sub_dirs, "Ld_b_"+subs_name, *Ld_b, false);
	    saveRadDirecData(prm, "Lu_b", sub_dirs, "Lu_b_"+subs_name, *Lu_b, false);
	    saveRadDirecData(prm, "Ld_w", sub_dirs, "Ld_w_"+subs_name, *Ld_w, false);
	    saveRadDirecData(prm, "Lu_w", sub_dirs, "Lu_w_"+subs_name, *Lu_w, false);
	    saveRadDirecData(prm, "Ld_a", sub_dirs, "Ld_a_"+subs_name, *Ld_a, false);
	    saveRadDirecData(prm, "Lu_a", sub_dirs, "Lu_a_"+subs_name, *Lu_a, false);

	      // save any angular irradiance data (quad averaged L times quad solid angle cos theta) if requested
	    saveRadDirecData(prm, "Id_b", sub_dirs, "Id_b_"+subs_name, *Ld_b, true);
	    saveRadDirecData(prm, "Iu_b", sub_dirs, "Iu_b_"+subs_name, *Lu_b, true);
	    saveRadDirecData(prm, "Id_w", sub_dirs, "Id_w_"+subs_name, *Ld_w, true);
	    saveRadDirecData(prm, "Iu_w", sub_dirs, "Iu_w_"+subs_name, *Lu_w, true);
	    saveRadDirecData(prm, "Id_a", sub_dirs, "Id_a_"+subs_name, *Ld_a, true);
	    saveRadDirecData(prm, "Iu_a", sub_dirs, "Iu_a_"+subs_name, *Lu_a, true);

	      // save remote sensing reflectances Rrs and Rtot if requested
	    saveReflectance(prm, "rrs", sub_dirs, "rrs_"+subs_name, sd_rrs);
	    saveReflectance(prm, "Rrs", sub_dirs, "Rrs_"+subs_name, sd_Rrs);
	    saveReflectance(prm, "Rtot", sub_dirs, "Rtot_"+subs_name, sd_Rtot);
	    saveReflectance(prm, "R0m", sub_dirs, "R0m_"+subs_name, sd_R0m);
	    saveReflectance(prm, "R0p", sub_dirs, "R0p_"+subs_name, sd_R0p);
*/
	    /*  LIST FILE STUFF NEED RE-ENABLING

	    if (prm.isDefined("Rrs_output_dir")) {

       
	        // build directory structure
	      JString path(prm.getString("Rrs_output_dir"));
	      mkdir(path.latin1(),0750);

	      path += "/" + sky_file.getText(s5,1);
	      mkdir(path.latin1(),0750);
	      path += "/" + surf.getText(s4,4);
	      mkdir(path.latin1(),0750);
	      path += "/" + input_slab_file.getText(s1,1);
	      mkdir(path.latin1(),0750);
	      path += "/" + input_slab_file_appendix.getText(s2,1); 
	      mkdir(path.latin1(),0750);
 
	      path += "/rrs_" + subs_diffuse_r0.getText(s3,1);

	      saveSpectralData(sd_lu, path);
	      

	      appendToListFile(prm, path);

	      if (prm.isDefined("list_file_dir")) {

		JString lp(prm.getString("list_file_dir"));
		mkdir(lp.latin1(),0750);
		lp += "/" + sky_file.getText(s5,1);
		mkdir(lp.latin1(),0750);
		lp += "/" + surf.getText(s4,4);
		mkdir(lp.latin1(),0750);
		lp += "/" + input_slab_file.getText(s1,1);
		mkdir(lp.latin1(),0750);
		lp += "/" + input_slab_file_appendix.getText(s2,1); 
		mkdir(lp.latin1(),0750);
		lp += "/";
		lp += prm.getString("list_file");

		JFile file(lp);
		if (!file.open(IO_AppendOnly)) internalError("File '%s' could not be written\n",lp.latin1()); 

		JTextStream str(&file);   
		str << path << "\n";
		
		file.close();
	      }

	    }
	    */

	    /*
	    JString fb(prm.getString("rrs_output_filebase"));
	    fb.append(input_slab_file.getText(s1,1));
	    if (!subs_is_mixed) fb.append(subs_diffuse_r0.getText(s3,1));
	    else fb.append(mix_text+JString::number(s3));
	    fb.append(surf.getText(s4,4));
	    fb.append(sky_file.getText(s5,1));
	    fb.append(input_slab_file_appendix.getText(s2,1));
	  
	    saveSpectralData(sd_lu, fb);
	    appendToListFile(prm,fb);
	    */
/*
	  }
	}
      }
    } 
  }
}
*/

/*
// SlabTool::loadSky()
// load sky radiance structure

void SlabTool::loadSky(const JString& fp) {

  sky.loadFromFile(fp);

    // Ld_a is sky downwelling radiance
    // and we take the radiance directional structure from the sky
  Ld_a = sky.emis().duplicate();
  Lu_a = sky.emis().duplicateStructure();
  Ld_w = sky.emis().duplicateStructure();
  Lu_w = sky.emis().duplicateStructure();
  Ld_b = sky.emis().duplicateStructure();
  Lu_b = sky.emis().duplicateStructure();
}



// SlabTool::loadSky()
// load slab file
 
void SlabTool::loadSlabFile(const JString& fp) {
  readSlabFile(fp, sb.mRup, sb.mTup, sb.mRdown, sb.mTdown);
}



// SlabTool::loadAppendSlabFile()
 
void SlabTool::loadAppendSlabFile(const JString& fp) {
  appendSlabFile(fp, sb.mRup, sb.mTup, sb.mRdown, sb.mTdown);
}



// SlabTool::loadSubsDiffuseR0()

void SlabTool::loadSubsDiffuseR0(const JString& fp) {
  subs.loadFromFile(fp, sky.emis().direcStruct());
}



// SlabTool::loadSubsDiffuseR0()

void SlabTool::loadSubsDiffuseR0(const SBandData& sd) {
  subs.setDiffuseReflectance(sd, sky.emis().direcStruct());
}



//  SlabTool::loadWaterSurface()

void SlabTool::loadWaterSurface(const JString& fp_r0, const JString& fp_r1, const JString& fp_t01, const JString& fp_t10) {
  water_surface.loadFromFiles(fp_r0, fp_r1, fp_t01, fp_t10); 
}



// SlabTool::calcReflectances()

void SlabTool::calcReflectances() {   
  calcSolution();
  calcOutputs();
}





// SylabTool::calcSolution_Fast() [private]
// calculate one solution as set up
// redundant test code ?

void SlabTool::calcSolution_Fast() {

  jlog::ls << "HERE\n";

    // incorporate bottom reflectnace Rb to slab using Ru -> Ru + Td Rb (I - Rd Rb)^-1 Tu
    // Rd Rb
  RDD_MatrixDFT m_mid(sb.mRdown);
  m_mid.postMultiply(subs.reflecMatrixDFT());
    // (I - Rd Rb)^-1
  if (!m_mid.makeIdentityMinusInverse()) jlog::es << "Invert failed!\n";
    // temp -> Ru + Td Rb (I - Rd Rb)^-1 Tu
  RDD_MatrixDFT temp(sb.mTdown);
  temp.postMultiply(subs.reflecMatrixDFT()); 
  temp.postMultiply(m_mid);
  temp.postMultiply(sb.mTup);
  temp.add(sb.mRup);

  return;

    // incorporate underside or water reflectance Rs to slab using Ru -> Ru (I - Rs Ru)^-1
    // Rs Ru
  m_mid.copy(water_surface.r1_dft());
  m_mid.postMultiply(temp);
    // (I - Rs Ru)^-1
  if (!m_mid.makeIdentityMinusInverse()) jlog::es << "Invert failed!\n";
    // temp = Ru (I - Rs Ru)^-1
  temp.postMultiply(m_mid);
  printf("HERE2\n");
  
    // start Ld_w estimation as sky radiance transferred through water surface 
  Ld_a->copyToMatrixDFT(Ld_a_ma);
  water_surface.t01_dft().translateInToOut(Ld_a_ma, Ld_w_ma);
  temp.translateInToOut(Ld_w_ma, Lu_w_ma);
  water_surface.t10_dft().translateInToOut(Lu_w_ma, Lu_a_ma);
 
  Lu_w->copyFromMatrixDFT(Lu_w_ma);
  Ld_w->copyFromMatrixDFT(Ld_w_ma);
  Lu_a->copyFromMatrixDFT(Lu_a_ma);
  Ld_a->copyFromMatrixDFT(Ld_a_ma);
}




// SlabTool::operationCalcSolutions2()
 

void SlabTool::operationCalcSolutions2(const JParser& prm) {
  jlog::ls << "Operation: calc_solutions v2\n";

  SkySet sky_set;
  SlabSet slab_set;
  SubstrateSet subs_set;
  WaterSurfaceSet surf_set;

  sky_set.load(prm);
  //slab_set.load(prm);
  //subs_set.load(prm, sky_set.direcStruct());
  surf_set.load(prm);
}



// SlabTool::buildMixedSubstrate()

void SlabTool::buildMixedSubstrate(Substrate& s, const TextTable& tt0, const TextTable& tt1, double f0, double f1, const DirecStruct& ds) {

  subs.loadFromFile(tt0.getText(0,0), ds, verbose_level);

  int r0 = rand_int(tt0.rowCount());
  int r1 = rand_int(tt1.rowCount());

  printf("BUILD MIX %d (%d) %d (%d) %f %f\n",r0,tt0.rowCount(),r1,tt1.rowCount(),f0,f1);

  printf("%s\n",tt0.getText(r0,0).latin1());
  printf("%s\n",tt1.getText(r1,0).latin1());

  FileDesc fd0(tt0.getText(r0,0), true);
  SpectralData sd0(fd0);

  FileDesc fd1(tt1.getText(r1,0), true);
  SpectralData sd1(fd1);

  SpectralData sd(sd0 * f0);
  sd.add(sd1 * f1);

  printf("end0: ");
  sd0.display();
  printf("end1: ");
  sd1.display();

  subs.setDiffuseReflectance(sd,ds,verbose_level);
}

  */


// SlabTool::saveRadiance()
/*

bool SlabTool::saveRadiance(const Parser& prm, const JString& prefix, const JString& sub_dirs, 
			    const JString& filename, const SpecImp<SpectralData>& Ldata, bool mult_sact) {

  if (!prm.isDefined(prefix+"_output_dir")) return true;

    // tree base directory for this data
  JString path(prm.getString(prefix+"_output_dir"));
    // make sure directory exists
  makePath(path, sub_dirs);
    // full file path
  path += sub_dirs + "/" + filename;
  
    // arrays of theta and phi angles (out directions) to output radiance for
  vec phi_deg;
  vec theta_deg;

    // phi is given as a list of angles in degrees
  if (prm.isDefined(prefix+"_output_phi_deg")) {

    phi_deg = prm.getVectorOfDoubles(prefix+"_output_phi_deg");

    // or phi is set up as a number of points at regular intervals
  } else {

    int n = prm.getInt(prefix+"_output_phi_points");
    double phi_range = Ldata.direcStruct().isFull() ? 360 : 180;
    double phi_start = -90 + phi_range / (2*n);

    phi_deg.resize(n);
    for (int i=0; i<n; i++) phi_deg(i) = phi_start + (i*phi_range) / n;
  }

    // theta is given as a list of angles in degrees
  if (prm.isDefined(prefix+"_output_theta_deg")) {

    theta_deg = prm.getVectorOfDoubles(prefix+"_output_theta_deg");

    // or theta is set up as a number of points at regular intervals
  } else {

    int n = prm.getInt(prefix+"_output_theta_points");
    double theta_range = Ldata.direcStruct().isFull() ? 360 : 180;
    double theta_start = -90 + theta_range / (2*n);

    theta_deg.resize(n);
    for (int i=0; i<n; i++) theta_deg(i) = theta_start + (i*theta_range) / n;
  }

    // get phi and number of theta points
  //double phi_deg = prm.isDefined(prefix+"_output_phi_deg") ? prm.getDouble(prefix+"_output_phi_deg") : 0;
  //int theta_points = prm.isDefined(prefix+"_output_theta_points") ? prm.getInt(prefix+"_output_theta_points") : 20;

  printf("Saving: %s\n",path.latin1()); 

  JFile f(path);
  if (!f.open(IO_WriteOnly)) return false;

  JTextStream str(&f);

  writeAngularData(str, phi_deg, theta_deg, Ldata, false, mult_sact);

  f.close();

  return true;
}

*/




// SlabTool::saveSpectralData()

//void SlabTool::saveSpectralData(const SpectralData& sd, const JString& filepath) {
//  FileDesc fd(filepath, true);
//  sd.writeToTextFile(fd);
//  printf("File: written '%s'\n",fd.path().latin1());
//}



// SlabTool::appendToListFile() [private]

//void SlabTool::appendToListFile(const Parser& prm, const JString& fb) {

  /*
  if (!prm.isDefined("list_file_base")) return;

  JString path(prm.getString("list_file_base"));
  path += "_" + sky_file.getText(s5,1);
  path += "_" + surf.getText(s4,4);
  path += "_" + input_slab_file.getText(s1,1);
  path += "_" + input_slab_file_appendix.getText(s2,1);

  JFile file(path);
  if (!file.open(IO_AppendOnly)) internalError("File '%s' could not be written\n",path.latin1()); 
  
  JTextStream str(&file);   
  str << fb << "\n";

  file.close();
  */
//}


/*
// SlabTool::readInputList() [private]

bool SlabTool::readInputList(const JString& prm_file, const JParser& prm, JTextTable& ttab) {

  if (!prm.isDefined(prm_file)) return false;

  ttab.readFromFile(prm.getString(prm_file));

  return true;
}



// SlabTool::writeSlabFile()

bool SlabTool::writeSlabFile(const JString& f, 
			     const RDD_MatrixDFT& mRA, const RDD_MatrixDFT& mTA,
			     const RDD_MatrixDFT& mRB, const RDD_MatrixDFT& mTB) {

  JFile file(f.latin1());
  if (!file.open(JIO_WriteOnly)) { jlog::es << "File '" << f << "' could not be written.\n"; return false; }
  
  jlog::ls.printf("Writing %s\n",f.latin1());
  
  JDataStream str(&file);   

    // true means has reflectance and transmission 
    // defined in both directions through slab
  str << ((unsigned char) 1);
  
  mRA.writeToDataStream(str);
  mTA.writeToDataStream(str);
  mRB.writeToDataStream(str);
  mTB.writeToDataStream(str);
  
  file.close();

  return true;
}



// SlabTool::writeSlabFile()

bool SlabTool::writeSlabFile(const JString& f, 
			     const RDD_MatrixDFT& mRA, const RDD_MatrixDFT& mTA) {

  JFile file(f.latin1());
  if (!file.open(JIO_WriteOnly)) { jlog::es << "File '" << f << "' could not be written.\n"; return false; }
  
  jlog::ls.printf("Writing %s\n",f.latin1());
  
  JDataStream str(&file);   

    // true means has reflectance and transmission 
    // defined in both directions through slab
  str << ((unsigned char) 0);
  
  mRA.writeToDataStream(str);
  mTA.writeToDataStream(str);
  
  file.close();

  return true;
}



// SlabTool::readSlabFile()

bool SlabTool::readSlabFile(const JString& f, 
			    RDD_MatrixDFT& mRA, RDD_MatrixDFT& mTA,
			    RDD_MatrixDFT& mRB, RDD_MatrixDFT& mTB) {
 
  JFile file(f.latin1());
  if (!file.open(JIO_ReadOnly)) { jlog::es << "File '" << f << "' could not be read.\n"; return false; }

  jlog::ls.printf("Reading %s\n",f.latin1());

  JDataStream str(&file);   

  unsigned char both_direc;
  str >> both_direc;

  mRA.readFromDataStream(str);
  mTA.readFromDataStream(str);

  if (both_direc) {
    mRB.readFromDataStream(str);
    mTB.readFromDataStream(str);
  } else {
    mRB.copy(mRA);
    mTB.copy(mTA);
  }

  file.close();

  return true;
}



// SlabTool::appendSlabFile()

bool SlabTool::appendSlabFile(const JString& f, 
			      RDD_MatrixDFT& mRA, RDD_MatrixDFT& mTA,
			      RDD_MatrixDFT& mRB, RDD_MatrixDFT& mTB) {
 
  JFile file(f.latin1());
  if (!file.open(JIO_ReadOnly)) { jlog::es << "File '" << f << "' could not be read.\n"; return false; }

  jlog::ls.printf("Reading %s\n",f.latin1());

  JDataStream str(&file);
   
  RDD_MatrixDFT mRA_temp;
  RDD_MatrixDFT mTA_temp;
  RDD_MatrixDFT mRB_temp;
  RDD_MatrixDFT mTB_temp;

  unsigned char both_direc;
  str >> both_direc;

  mRA_temp.readFromDataStream(str);
  mTA_temp.readFromDataStream(str);
  if (both_direc) {
    mRB_temp.readFromDataStream(str);
    mTB_temp.readFromDataStream(str);
  } else {
    mRB_temp.copy(mRA_temp);
    mTB_temp.copy(mTA_temp);
  }

  combineSlabs(mTA, mRA, mTB, mRB, mTA_temp, mRA_temp);
  combineSlabs(mTB_temp, mRB_temp, mTA_temp, mRA_temp, mTB, mRB);
  mTB.copy(mTB_temp);
  mRB.copy(mRB_temp);

  file.close();

  return true;
}



// SlabTool::combineSlabs()
// this modifies slab1 mT1_f (forward transmission) and mR1_f (forward-looking reflectance) as 
// if appending slab2 on the far side

void SlabTool::combineSlabs(RDD_MatrixDFT& mT1_f, RDD_MatrixDFT& mR1_f, 
			    const RDD_MatrixDFT& mT1_b, const RDD_MatrixDFT& mR1_b, 
			    const RDD_MatrixDFT& mT2_f, const RDD_MatrixDFT& mR2_f) {

    // now need to consider path back transmitted and bounced back from second slab
  RDD_MatrixDFT m_mid(mR2_f);
  m_mid.postMultiply(mR1_b);

  if (!m_mid.makeIdentityMinusInverse()) jlog::es << "Invert failed!\n";

    // need T1(I-R1R2)^-1 for both
  mT1_f.postMultiply(m_mid);

    // update R1 first as depends on both R1 and T1
  RDD_MatrixDFT temp(mT1_f);
  temp.postMultiply(mR2_f);
  temp.postMultiply(mT1_b);
  mR1_f.add(temp);

    // now can finish T1
  mT1_f.postMultiply(mT2_f);
}

   



*/

