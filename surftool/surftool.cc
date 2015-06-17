
// surftool.cc

#include "surftool.h"
#include "rad/raddirectable.h"
#include "base/jparser.h"
#include "base/jgenericfactory.h"
#include "rad/bandspec.h"
#include "ds/direcspec.h"
#include "ds/dsquad.h"
#include "ds/quadintegrator.h"
#include "base/jlog.h"
#include "file/filedesc.h"
#include "file/fileutils.h"
#include "rad/radunits.h"
#include "rad/radutils.h"
#include "math/mathutil.h"
#include "base/jstack.h"
#include "coxsurface.h"
#include "ray.h"



// SurfTool::(constructor)

SurfTool::SurfTool() : 
  Tool(), 
  master_bs(0),
  master_ds_spec(0),
  func(0) { }



// SurfTool::(destructor)

SurfTool::~SurfTool() {
  delete master_bs;
  delete master_ds_spec;
  delete func;
}



// SurfTool::run()

bool SurfTool::run(const JParser& prm) {

    // basic initialisation common to all tools
  if (Tool::init(prm)) return true;
  Tool::initRandom(prm);

  if (jlog::vbLevel(5)) prm.displayList(jlog::ls);

  uncancelProcessing();

    // delete any previous allocation
  delete master_bs;
  delete master_ds_spec;
  delete func;
 
    // get output band specification or quit with error
  master_bs = BandSpec::create(prm, "");
  if (!master_bs) { jlog::es << "No output band specification given or it is incomplete.\n"; return false; }
    // set up a local band count for ease of use
  band_count = master_bs->bandCount();

    // get output directional specification or quit with error
  master_ds_spec = DirecSpec::create(prm, "");
  if (!master_ds_spec) { jlog::es << "No output directional specification given or it is incomplete.\n"; return false; }

    // try to build a full directional spectral data table
  func = buildRadDirecDirec<SBandData>(*master_ds_spec, master_bs->bandCount(), true);
  if (!func) return false;
    // by defualt no standard error calc
  func_se = buildRadDirecDirec<SBandData>(*master_ds_spec, master_bs->bandCount(), true);
  if (!func_se) return false;

    // initialise completion count
  Tool::pct_complete = 0;

    // size of sub-quads to split sphere into
    // default value of 100,000,000 sub quads has been found required to remove aliasingg problems in upward transmitted radiance
  if (prm.isDefined("sub_quad_count")) sub_quad_sa = (4*M_PI) / prm.getInt("sub_quad_count");
  else if (prm.isDefined("sub_quad_solid_angle")) sub_quad_sa = prm.getDouble("sub_quad_solid_angle");
  else sub_quad_sa = (4*M_PI) / 100000000;

  bool ret_val;

    // get the type
  JString type = prm.getStringOrEmpty("type");
  if (type.isEmpty()) { jlog::es << "Surface parameter 'type' not specified.\n"; return false; }
 
    // intialising to zero is helpful for most build operations
  func->makeAll(SBandData(master_bs->bandCount(), 0));
  func_se->makeAll(SBandData(master_bs->bandCount(), 0));

     // these calls set up the functions as mean outgoing radiance over solid angle in each quad
    // divided by incoming planar irradiance
  if (type == "flat") ret_val = buildFlatInterface(prm);
  else if (type == "absent") ret_val = buildAbsentInterface();
  else if (type == "mirror") ret_val = buildMirrorInterface();
  else if (type == "coxmunk") ret_val = buildCoxMunkInterface(prm);
  else if (type == "black") ret_val = true;
  else if (type == "fileload") ret_val = buildFromFile(prm);
  else {
    jlog::es << "'type = " << prm.getString("type") << "' is not a supported surface type.\n";
    ret_val = false;
  }

    // quit if something went wrong
  if (!ret_val) return false;

    // the functions are BRDFs of outgoing radiance divided by incident planar irradiance
  func->setInDataTypePlanarIrrad();
  func->setExDataTypeMeanRad();

  if (func_se) {
    func_se->setInDataTypePlanarIrrad();
    func_se->setExDataTypeMeanRad();
  }

    // clear reflectance if only doing transmission
  if (prm.isDefinedAndTrueBool("transmission_only")) {
    //r0->makeAll(SBandData(master_bs->bandCount(), 0));
    //r1->makeAll(SBandData(master_bs->bandCount(), 0));
  }

    // if requested to azimuthally average
  if (prm.isDefinedAndTrueBool("azimuthally_average")) {
    RadUtils::makeAzimuthallyAveraged(*func);
    RadUtils::makeAzimuthallyAveraged(*func_se);
    jlog::ls << "Azimuthally averaged.\n";
  }

    // finalise completion count
  Tool::pct_complete = 100;

    // display half-function reflectances
  if (jlog::vbLevel(2)) RadUtils::displayHalfFunctionsAsReflectance(*func, jlog::ls);
  
    // if full file to save to is specified 
  if (prm.isDefined("surface_save_fp")) {
      // get file path
    JString out_filepath = prm.getString("surface_save_fp");
      // build notes using function in Tool
    JTextDevice nd;
    buildNotes(nd, prm);
       // generic writer that includes information for reconstruction of dynamic types on reload
    FileUtils::writeFactoryBuiltToFile(*func, FileDesc(out_filepath, false), nd.text()); 
      // info
    if (jlog::vbLevel(1)) {
      jlog::ls << "Written file '" << out_filepath << "' as ";
      jlog::ls << "tabulated direction-to-direction function of " << func->inDataString() << " to " << func->exDataString() << ".\n";
    }
  }

    // if full file to save to is specified 
  if (prm.isDefined("surface_se_save_fp") && func_se) {
      // get file path
    JString out_filepath = prm.getString("surface_se_save_fp");
      // build notes using function in Tool
    JTextDevice nd;
    buildNotes(nd, prm);
      // generic writer that includes information for reconstruction of dynamic types on reload
    FileUtils::writeFactoryBuiltToFile(*func_se, FileDesc(out_filepath, false)); 
      // info
    if (jlog::vbLevel(1)) {
      jlog::ls << "Written file '" << out_filepath << "' as ";
      jlog::ls << "tabulated direction-to-direction function of " << func_se->inDataString() << " to " << func_se->exDataString() << ".\n";
    }
  }

  return true;
}



// SurfTool::buildAbsentInterface()

bool SurfTool::buildAbsentInterface() {

  if (jlog::ls.vbLevel(2)) jlog::ls << "Building absent interface surface.\n";

  const DirecStruct& ds(func->direcStruct());

  SBandData sd(master_bs->bandCount(), 1);

    // loop through all in-indices
  for (int in_index=0; in_index<ds.units(); in_index++) {

      // index of outgoing quad
    int out_index = in_index;
      // the brdf relates the solid angle integral of incoming radiance * cos theta to outgoing radiance
      // this is notionally power - we want the same planar irrad to come out as goes in
      // so divide by the solid angular area of the exitant quad to convert outgoing planar irrad to mean radiance
    SBandData sd_correc(sd);
    sd_correc.divide(ds.solidAngleAbsCosTheta(out_index));
      // everything incident within this quad exits at the transmitted quad 
    func->setData(in_index, out_index, sd_correc);
   
      // check if another thread cancelled the processing
    if (processingCanceled()) return false;
      // progress
    Tool::pct_complete = (100.0 * in_index) / ds.units();
  }

  return true;
}




// SurfTool::buildAbsentInterface()

bool SurfTool::buildMirrorInterface() {

  if (jlog::ls.vbLevel(2)) jlog::ls << "Building mirror interface surface.\n";

  const DirecStruct& ds(func->direcStruct());

  SBandData sd(master_bs->bandCount(), 1);

    // loop through all in-indices
  for (int in_index=0; in_index<ds.units(); in_index++) {

      // get the direction of the solid angle mean point in the incidence quad
    Vec3D v = ds.indexToMeanDirec(in_index);
      // turn v into an outgoing vector on the same side of the surface
    //v = Vec3D(-v.x(), -v.y(), v.z()); // old
    v = Vec3D(v.x(), v.y(), -v.z());
      // index of outgoing quad
    int out_index = ds.direcToIndex(v);
      // the brdf relates the solid angle integral of incoming radiance * cos theta to outgoing radiance
      // this is notionally power - we want the same planar irrad to come out as goes in
      // so divide by the solid angular area of the exitant quad to conver ougoing planar irrad to mean radiance
    SBandData sd_correc(sd);
    sd_correc.divide(ds.solidAngleAbsCosTheta(out_index));
      // everything incident within this quad exits at the transmitted quad 
    func->setData(in_index, out_index, sd_correc);
   
      // check if another thread cancelled the processing
    if (processingCanceled()) return false;
      // progress
    Tool::pct_complete = (100.0 * in_index) / ds.units();
  }

  return true;
}



// Builds a reflection-transmission function for a flat interface between differing refractive indices.
// this uses its own simple single scattering ray-surface calculation - function rayCalc() defined below
// this is essentially redundant since the code in jude::ray can perform the same function
// however it is retained here as is use useful for testing and is simpler to understand

bool SurfTool::buildFlatInterface(const JParser& prm) {

  if (jlog::ls.vbLevel(2)) jlog::ls << "Building flat interface surface.\n";

  if (!prm.isDefined("refrac_index_0") || !prm.isDefined("refrac_index_1")) {
    jlog::es << "Both refrac_index_0 and refrac_index_1 should be defined for flat surface type.\n";
    return false;
  }

    // refractive index on either side of interface
  refrac_index_0 = prm.getDouble("refrac_index_0"); // z+ side, vectors travelling z- start here (underwater)
  refrac_index_1 = prm.getDouble("refrac_index_1"); // z- side, vectors travelling z+ start here (air)

  if (jlog::ls.vbLevel(2)) {
    jlog::ls << "Refractive index side 0 (z+ side): " << refrac_index_0 << "\n";
    jlog::ls << "Refractive index side 1 (z- side): " << refrac_index_1 << "\n";
    jlog::ls << "Integration delta solid angle: " << sub_quad_sa << "\n";
  }
			
    // directional structure
  const DirecStruct& ds(func->direcStruct());
  
  double trans_power;
  double reflec_power;

  Vec3D trans_direc;
  Vec3D reflec_direc;

    // loop through all in indices
  for (int in_index=0; in_index<ds.units(); in_index++) {

    QuadIntegrator* qi_in = ds.quadAtIndex(in_index)->createIntegrator(sub_quad_sa); 
  
    double qc_recip = 1.0 / qi_in->subQuadCount();
    int qc = 0;

    if (!qi_in) { jlog::es << "Failed to create quad integrator.\n"; return false; }
	 
    if (jlog::vbLevel(2)) {
      jlog::ls.printf("quad %d/%d sub count: %d   \n", in_index, ds.units()-1, qi_in->subQuadCount());
      jlog::ls.device().cursorUp(1);
    }

      // loop through all incident sub-quads 
    while (!qi_in->atEnd()) {

        // calculate the transmitted and reflected components of the ray from this sub-quad direction (negated to go towards the surface)
      rayCalc(refrac_index_0, refrac_index_1, qi_in->direction(), Vec3D(0,0,1), trans_direc, reflec_direc, trans_power, reflec_power);
        // the weight for this sub-quad result is based on its relative solid angle
      double frac = qi_in->solidAngle() / ds.solidAngle(in_index);
        // weight the result so the sum for this directional structure quad comes to unity
      reflec_power *= frac;
      trans_power *= frac;
        // add reflected power
      func->addToData(in_index, ds.direcToIndex(reflec_direc), SBandData(band_count, reflec_power));
        // add transmitted power if any
      if (trans_power != 0) func->addToData(in_index, ds.direcToIndex(trans_direc), SBandData(band_count, trans_power));
      
        // next sub-quad
      qi_in->next();

        // check if another thread cancelled the processing
      if (processingCanceled()) return false;
        // progress
      Tool::pct_complete = (100.0 * (in_index + (qc++) * qc_recip)) / ds.units();
    }
  }
 
  if (jlog::vbLevel(2)) jlog::ls << "\n";
  
    // divide by outging solid angle cos(theta) to turn outgoing planar irradiance to radiance
  func->divideAllByExSACT();

  return true;
}



// SurfTool::getRefractiveIndices() [private]

bool SurfTool::getRefractiveIndices(const JParser& prm) {

  if (!prm.isDefined("refrac_index_0") || !prm.isDefined("refrac_index_1")) {
    jlog::es << "Both refrac_index_0 and refrac_index_1 should be defined for flat surface type.\n";
    return false;
  }

    // refractive index on either side of interface
  refrac_index_0 = prm.getDouble("refrac_index_0"); // z+ side, vectors travelling z- start here
  refrac_index_1 = prm.getDouble("refrac_index_1"); // z- side, vectors travelling z+ start here

  if (jlog::ls.vbLevel(2)) {
    jlog::ls << "Refractive index side 0 (z+ side): " << refrac_index_0 << "\n";
    jlog::ls << "Refractive index side 1 (z- side): " << refrac_index_1 << "\n";
    jlog::ls << "Integration delta solid angle: " << sub_quad_sa << "\n";
  } 

  return true;
}



bool SurfTool::buildCoxMunkInterface(const JParser& prm) {

    // initialise the surface parameters and storage
  CoxSurface cs;
  if (!cs.init(prm)) return false;

    // get the refractive inidices
  if (!getRefractiveIndices(prm)) return false;

    // size of the target rectangle on the surafce centre
  if (!prm.isDefined("target_size")) { jlog::es << "Target area should be defined with target_size.\n"; return false; }
  targ_size = prm.getDouble("target_size");
  targ_size /= 2;

    // rays to do per quad
  if (!prm.isDefined("rays_per_quad")) { jlog::es << "Rays per quad to do should be defined with rays_per_quad.\n"; return false; }
  rays_per_quad = prm.getInt("rays_per_quad");

    // surfaces to do
  if (!prm.isDefined("surface_count")) { jlog::es << "Number of surface instantiations to do should be defined with surface_count.\n"; return false; }
  surface_count = prm.getInt("surface_count");

    // minimum power to propagate in a ray 			
  min_pwr_frac = prm.isDefined("min_pwr_frac") ? prm.getDouble("min_pwr_frac") : 1E-6;

  func_se = func->duplicateStructure();
    
  RadDirecDirecImp<SBandData>* func_temp = func->duplicateStructure();

    // intialising to zero is helpful for most build operations
  func_se->makeAll(SBandData(master_bs->bandCount(), 0));

    // for each surface
  for (int s=0; s<surface_count; s++) {

      // create the surface and report slopes
    cs.buildSurface();
    if (jlog::vbLevel(2)) cs.checkSlopes();

      // process the surface
    if (!doCoxSurface(cs, *func_temp, s, surface_count)) return false;

      // divide by outging solid angle cos(theta) to turn outgoing planar irradiance to radiance
    func_temp->divideAllByExSACT();
    func_temp->divide(rays_per_quad);

    func_temp->setInDataTypePlanarIrrad();
    func_temp->setExDataTypeMeanRad();
    if (jlog::vbLevel(2)) RadUtils::displayHalfFunctionsAsReflectance(*func_temp, jlog::ls);

    func->add(*func_temp);
    func_se->addSquared(*func_temp);
  }

  delete func_temp;

    // make function mean of surfaces
  func->divide(surface_count);
  
    // mean of sum of squares
  func_se->divide(surface_count);
    // subtract the mean squared
  func_se->subtractSquared(*func);
    // sometimes due to numerical errors values come out very slightly below zero which should be impossible
  for (int i=0; i<func_se->direcStruct().units(); i++) {
    for (int j=0; j<func_se->direcStruct().units(); j++) {
      double v = func_se->atIndices(i,j).bandValue(0);
      if (v < 0) func_se->setData(i, j, SBandData(band_count,0));
    }
  }

    // is now the standard deviation
  func_se->squareRoot();
    // is now the standard error
  func_se->divide(sqrt(surface_count));

  return true;
}



// SurfTool::doCoxSurface() [private]

bool SurfTool::doCoxSurface(CoxSurface& cs, RadDirecDirecImp<SBandData>& func_targ, int surf_num, int surf_count) {

    // directional structure
  const DirecStruct& ds(func->direcStruct());

    // initialise energy conservation checks
  doubleLONG pwr_in = 0;
  doubleLONG pwr_out = 0;
  doubleLONG pwr_zp = 0;
  doubleLONG pwr_zm = 0;
  doubleLONG leak_pwr = 0;
  
  double trans_power;
  double reflec_power;

  Vec3D trans_direc;
  Vec3D reflec_direc;

    // intialise to zero
  func_targ.makeAll(SBandData(master_bs->bandCount(), 0));

    // loop through all in indices
  for (int in_index=0; in_index<ds.units(); in_index++) {

      // for each ray to do
    for (int i=0; i<rays_per_quad; i++) {

        // random inward direction in this quad
      Vec3D in_direc = ds.quadAtIndex(in_index)->randomDirec();
        // point on the surface headed for
      Vec3D targ_point = Vec3D(uniform_dbn(-targ_size,targ_size), uniform_dbn(-targ_size,targ_size), 0);

      if (jlog::vbLevel(6)) jlog::ls << "targ_point: " << targ_point << " in_direc: " << in_direc << "\n";
 
        // if the ray is on side 0 (z+ side, water underside)
      bool on_side0 = (in_direc.z() < 0);
        // pull back to ray start position
      if (!on_side0) targ_point += in_direc * (cs.minZ() - 0.1) / in_direc.z();
      else targ_point += in_direc * (cs.maxZ() + 0.1) / in_direc.z();
      
      if (jlog::vbLevel(6)) jlog::ls << "ray start: " << targ_point << "\n";
 
        // stack of rays to process
      JStack<Ray*> ray_stack;
        // construct an initial ray
      Ray* first_ray = new Ray(targ_point, in_direc, 1, on_side0, 0);
      pwr_in += 1;
        // push the initial ray on the stack
      ray_stack.push(first_ray);
     
        // continue while there are rays on the stack
      while (!ray_stack.isEmpty()) {

  	  // pull a ray off the stack
	Ray* ray = ray_stack.pop();

	if (jlog::vbLevel(6)) {
	  jlog::ls << "ray v: " << ray->start() << " dv: " << ray->direc() << " pwr: " << ray->power();
	  jlog::ls.printf(" f: %p\n", ray->origFacet());
	}
   
	  // find the first intersecting facet or null if none
	double dist;
	const Facet* isec_facet = cs.findIsecFacet(ray->start(), ray->direc(), ray->origFacet(), dist);
      
	if (jlog::vbLevel(6)) jlog::ls.printf("isec facet %p\n", isec_facet);
	
	if (isec_facet) {
      
	  Vec3D isec_point(ray->start() + ray->direc() * dist);

            // calculate the transmitted and reflected components of the ray from this sub-quad direction
	  rayCalc(refrac_index_0, refrac_index_1, in_direc, isec_facet->surfaceNormal(), trans_direc, reflec_direc, trans_power, reflec_power);
	  
	  if (jlog::vbLevel(6)) {
	    jlog::ls << "surface normal: " << isec_facet->surfaceNormal() << "\n";
	    jlog::ls << "reflec_direc: " << reflec_direc << " power: " << reflec_power << "\n";
	    jlog::ls << "trans_direc: " << trans_direc << " power: " << trans_power << "\n";
	  }

	  reflec_power *= ray->power();
	  trans_power *= ray->power();

	    // add new rays
	  if (reflec_power > min_pwr_frac) ray_stack.push(new Ray(isec_point, reflec_direc, reflec_power, ray->onSide0(), isec_facet));
	  if (trans_power > min_pwr_frac) ray_stack.push(new Ray(isec_point, trans_direc, trans_power, !ray->onSide0(), isec_facet));

	} else {

	    // sum total power out to check energy conservation
	  pwr_out += ray->power();
	    // check if ray should really be hitting the surface
	  if ((ray->direc().z() < 0) == ray->onSide0()) leak_pwr += ray->power(); 
  	    // otherwise add exitant power
	  else {
	    func_targ.addToData(in_index, ds.direcToIndex(ray->direc()), SBandData(band_count, ray->power()));
	    if (in_direc.z() < 0) pwr_zm += ray->power();
	    else pwr_zp += ray->power();
	  }
	}

	  // delete the ray
	delete ray;

      } // next ray on the stack

        // check if another thread cancelled the processing
      if (processingCanceled()) return false;
        // progress
      Tool::pct_complete = (100.0 * (surf_num + (in_index + i / rays_per_quad) / ds.units()) / surf_count);
  
    } // next initial ray in quad

    if (jlog::vbLevel(2)) {
      jlog::ls.printf("surface: %d/%d quad: %d/%d    \n", surf_num+1, surf_count, in_index+1, ds.units());
      jlog::ls.device().cursorUp(1);
    }

  } // next quad

  doubleLONG half_pwr_in = pwr_in / 2;

  if (jlog::vbLevel(2)) {
    jlog::ls << "Power in: " << (double) pwr_in << " power out: " << (double) pwr_out << "\n";
    jlog::ls << "Power z-plus incident frac: " << (double) (pwr_zp/half_pwr_in) << "\n";
    jlog::ls << "Power z-minus incident frac: " << (double) (pwr_zm/half_pwr_in) << "\n";
    jlog::ls << "Leak power: " << (double) leak_pwr << " frac: " << (double) (leak_pwr/pwr_out) << "\n";
  }

  double zp_scale = half_pwr_in / pwr_zp;
  double zm_scale = half_pwr_in / pwr_zm;
  func_targ.multiplyQuadrants(zp_scale, zp_scale, zm_scale, zm_scale);    

  return true;
}



// SurfTool::rayCalc()
// this calculates a simple single-scattering ray reflection and transmission at an interface between refractive indices (n0) amd (n1)
// in_direc is treated as an incoming vector (i.e. direction of photon travel towards the surface)
// on exit reflec_power and trans_power are the relative powers reflected and transmitted, i.e. actual power if assuming incident power of 1.0
// and reflec_direc and trans_direc are their directions (of photon travel)

void SurfTool::rayCalc(double n0, double n1, const Vec3D& in_direc, const Vec3D& norm, 
                       Vec3D& trans_direc, Vec3D& reflec_direc, double& trans_power, double& reflec_power) {

    // dot product between the ray direction and surface normal at point of intersection
  double dp = Vec3D::dotProduct(in_direc, norm);
    // unit vector in reflectance direction
  reflec_direc = in_direc - norm * 2 * dp;

  // the reflectance defaults to 1 in case there is no transmitted ray 
    // note transmittance = 1-r;
  reflec_power = 1;

    // if the cosine is negative then the ray is incident on side0 of the surface 
  bool on_side0 = (dp < 0);

    // calculating ratio of incident side index to transmitted side index
  double it_ratio = (on_side0) ? n0/n1 : n1/n0;
  double ti_ratio = 1/it_ratio;

    // start to calculate transmitted ray direction
  double c = dp*dp + ti_ratio*ti_ratio - 1;
    // c < 0 means total reflection so no transmission
  if (c >= 0) {

       // transmission direction unit vector
    c = (dp<0) ? dp + sqrt(c) : dp - sqrt(c);
    trans_direc = (in_direc - norm * c) * it_ratio; 

      // gives incident theta from 0 - 90 degrees regardless of which side incident on
    double incident_theta = acos(fabs(dp));

      // special case
    if (incident_theta==0) {
   
      reflec_power = (1-it_ratio) / (1+it_ratio);
      reflec_power = reflec_power*reflec_power;

    } else {

        // transmitted theta 0 - 90 degrees regardless of which side
      double transmitted_theta = acos(fabs(Vec3D::dotProduct(trans_direc, norm)));
        // can now calculate reflectance based on Fresnels formula
      double theta_diff = incident_theta - transmitted_theta;
      double theta_sum = incident_theta + transmitted_theta;

        // power-to-power reflectance (i.e. fraction of reflected photons)
      double p1 = sin(theta_diff) / sin(theta_sum);
      double p2 = tan(theta_diff) / tan(theta_sum);

      reflec_power = (p1*p1 + p2*p2) / 2;
    }
  }

  trans_power = (1 - reflec_power);
}



// SurfTool::buildFromFile()

bool SurfTool::buildFromFile(const JParser& prm) {
  
    // try to get a filepath to load
  JString fp = prm.getStringOrEmpty("file_load_fp");
  if (fp.isEmpty()) { jlog::es << "Surface type 'fileload' specified but no 'file_load_fp' provided.\n"; return false; }
 
    // try to load the full directional function
  RadDirecDirecImp<SBandData>* new_func = FileUtils::createFactoryBuiltFromFile< RadDirecDirecImp<SBandData> >(FileDesc(fp, false));

  if (!new_func) {
    jlog::es << "Failed to load interface file '" << fp << "'.\n";
    return false; 
  }

  // if (!FileUtils::readFactoryBuiltFromFile(*func, FileDesc(fp, false))) return false;
      // info
  if (jlog::ls.vbLevel(3)) jlog::ls << "Loaded '" << fp << "'.\n";
    // check number of bands
  if (new_func->bandCount() != band_count) {
    jlog::es << "File '" << fp << "' is in " << new_func->bandCount() << " bands but should be " << band_count << ".\n";
    delete new_func;
    return false; 
  }

    // check directional structure   
  if (!new_func->direcStruct().isEqual(func->direcStruct())) { 
    jlog::es << "File '" << fp << "' has incorrect directional structure.\n";
    delete new_func; 
    return false;
  }

  delete func;
  func = new_func;
  
  return true;
}



void SurfTool::helpText(JTextStream& str) const {

  str << "\n";
  str << "Surface Type Specification\n";
  str << "--------------------------\n";
  str << "type = absent | black | flat\n";
  str << "\n";
 
  str << "Integration Options\n";
  str << "-------------------\n";
  str << "sub_quad_count       = <integer> (approximate number of divisions of the sphere to perform integrations over, default 1E8).\n";
  str << "sub_quad_solid_angle = <real>    (solid angle of divisions of the sphere to perform integrations over, default 1.25E-7).\n";
  str << "\n";

  Tool::helpText(str);
}

