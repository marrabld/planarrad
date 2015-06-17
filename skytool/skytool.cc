
// skytool.cc

#include "skytool.h"
#include "rad/raddirectable.h"
#include "base/jparser.h"
#include "base/jgenericfactory.h"
#include "rad/bandspec.h"
#include "ds/direcspec.h"
#include "ds/dsquad.h"
#include "ds/quadintegrator.h"
#include "base/jlog.h"
#include "ds/spherepartition.h"
#include "ds/hemispherepartvxh.h"
#include "file/filedesc.h"
#include "file/fileutils.h"
#include "rad/radunits.h"
#include "math/mathutil.h"
#include "rad/hemispheredraw.h"
#include "base/jbuffer.h"
#include "base/jimagesave.h"
#include "rad/radutils.h"
#include "config.h"


using namespace jude::file;
using namespace jude::math;



// NOTE: skys like all emission data are stored as *quad averaged radiances*


// SkyTool::(constructor)

SkyTool::SkyTool() : 
  Tool(), 
  master_bs(0),
  master_ds_spec(0),
  emis(0) { init(); }



// SkyTool::(destructor)

SkyTool::~SkyTool() {
  delete master_bs;
  delete master_ds_spec;
  delete emis;
}



// SkyTool::init()

void SkyTool::init() {

  static bool done_init = false;
  if (done_init) return;

    // somewhere in all programs there must occur a list of all the static polymorphic types that can occur
  JFactory< RadDirecImp< SBandData>, RadDirecTable< SBandData, HemiSpherePartVxH > >::instance().reg();
  JFactory< RadDirecImp< SBandData>, RadDirecTable< SBandData, SpherePartVxH > >::instance().reg();
  JFactory< RadDirecDirecImp< SBandData>, RadDirecDirecTable< SBandData, HemiSpherePartVxH > >::instance().reg();
  JFactory< RadDirecDirecImp< SBandData>, RadDirecDirecTable< SBandData, SpherePartVxH > >::instance().reg();


  done_init = true;
}



// SkyTool::run()

bool SkyTool::run(const JParser& prm) {

    // basic initialisation common to all tools
  if (Tool::init(prm)) return true;
  Tool::initRandom(prm);

  if (jlog::vbLevel(5)) prm.displayList(jlog::ls);

  uncancelProcessing();

    // delete any previous allocation
  delete master_bs;
  delete master_ds_spec;
  delete emis;
  emis = 0;
  master_bs = 0;
  master_ds_spec = 0;

    // get output band specification or quit with error
  master_bs = BandSpec::create(prm, "");
  if (!master_bs) { jlog::es << "No output band specification given or it is incomplete.\n"; return false; }

    // get output directional specification or quit with error
  master_ds_spec = DirecSpec::create(prm, "");
  if (!master_ds_spec) { jlog::es << "No output directional specification given or it is incomplete.\n"; return false; }
    // can optionally build and save a full sphere structure
  bool do_full = false;
  if (prm.isDefinedAndTrueBool("do_full_sphere")) do_full = true;
    // try to build a full directional spectral data table
  emis = buildRadDirec<SBandData>(*master_ds_spec, master_bs->bandCount(), do_full);
    // if failed an error will have been streamed to jlog::es
  if (!emis) return false;

    // try to build a full directional spectral data table
  //emis_pol = buildRadDirec<SBandDataPol>(*master_ds_spec, master_bs->bandCount(), do_full);
    // if failed an error will have been streamed to jlog::es
  //if (!emis_pol) return false;

    // initialise completion count
  Tool::pct_complete = 0;

    // any quads that don't have values written to them will be zero (esp. upward looking quads)
  emis->makeAll(SBandData(master_bs->bandCount(),0));

  bool ret_val;

    // get the type
  JString type = prm.getStringOrEmpty("type");
  if (type.isEmpty()) { jlog::es << "Sky model parameter 'type' not specified.\n"; return false; }

    // these calls set up the emis structure to be mean radiance over solid angle in each quad
    // in the z-plus half of the directional structure (i.e the whole thing for hemi-partitions, z-plus half for full) 
  if (type == "diffuse") ret_val = diffuseSky(*emis, prm);
  else if (type == "thetazero") ret_val = thetaZeroSky(*emis, prm);
  else if (type == "specular") ret_val = specularSky(*emis, prm);
  else if (type == "hlideal") ret_val = hlidealSky(*emis, prm);
  else if (type == "grant") ret_val = grantSky(*emis, prm);
  else if (type == "fileload") ret_val = loadFile(emis, prm);
  // else if (type == "cie") ret_val = cieSky(*emis, prm);
  // else if (type == "libradtran") ret_val = libradtranSky(*emis, prm);
  else {
    jlog::es << "'type = " << prm.getString("type") << "' is not a supported sky type.\n";
    ret_val = false;
  }

    // quit if something went wrong
  if (!ret_val) return false;

    // if requested to azimuthally average
  if (prm.isDefinedAndTrueBool("azimuthally_average")) {
  
    RadUtils::makeAzimuthallyAveraged(*emis);
    jlog::ls << "Azimuthally averaged.\n";
  
    //if (!emis->azimuthallyAverage()) {
    //  jlog::ls << "Azimuthal averaging requested but directional structure is not of the right type, so ignoring.\n";
    //} else {
    //  jlog::ls << "Azimuthally averaged.\n";
    // }
  }

    // finalise completion count
  Tool::pct_complete = 100;

    // standard is to set this up like an emission file (i.e. will be quad-averaged radiance)
    // if this parameter is true then will be transformed to like a received file 
    // (i.e. will be integral of cos theta x radiance over solid angle)
    // but note that the directions will not be changed
  bool recv = (prm.isDefined("output_planar_irrad")) ? prm.getBool("output_planar_irrad") : false;
  if (recv) emis->multiplyAllBySACT();
 
  emis->lazyInvalidate();
  SBandData sd(emis->planarIrradiance());
 
  if (jlog::vbLevel(1)) {
    jlog::ls << "Planar irradiance (" << spectralIrradianceUnits() << ") ";
    sd.display(jlog::ls);
    jlog::ls << "\n";
    //jlog::ls.printf("Integral: %f Wm-2",sd.integrateOverWavelength(*master_bs));
  }

    // if file to save to is specified 
  if (prm.isDefined("sky_save_fp")) {
      // get file path
    JString out_filepath = prm.getString("sky_save_fp");
      // build notes using function in Tool
    JTextDevice nd;
    buildNotes(nd, prm);
      // generic writer that includes information for reconstruction of dynamic types on reload
    FileUtils::writeFactoryBuiltToFile(*emis, FileDesc(out_filepath, false), nd.text()); 
      // for info
    if (jlog::vbLevel(1)) {
      jlog::ls << "Written file '" << out_filepath << "' as ";
      jlog::ls << (emis->dataTypeIsMeanRad() ? "quad-averaged radiance" : "quad-averaged planar irradiance") << ".\n";
    }
  }
  
    // maybe save a hemisphere image
  if (!RadUtils::saveHemisphereImage("sky", "", *emis, *master_bs, prm)) return false;

  return true;
}



// SkyTool::getSunPosition() [private]
// (sun_direc) is vector which points in the direction the sun shines in
// returns false if something went wrong

bool SkyTool::getSunPosition(const JParser& prm, double& zenith, double& azimuth, Vec3D& sun_direc) {

    // get sun zenith angle
  zenith = (prm.isDefined("zenith")) ? prm.getDouble("zenith") : 0;
  if (zenith < 0 || zenith > 90) { jlog::es.printf("Bad zenith angle %.0f degrees. Valid range is 0 to 90.\n",zenith); return false; }

    // get sun azimuth angle
  azimuth = (prm.isDefined("azimuth")) ? prm.getDouble("azimuth") : 0;
  if (azimuth < -360 || azimuth > 360) { jlog::es.printf("Bad azimuth angle %.0f degrees. Valid range is -360 to 360.\n",azimuth); return false; }

    // build vector - this is actually the required outgoing vector for the sky hemisphere
  sun_direc.makeUnit((M_PI*zenith)/180, (M_PI*azimuth)/180);   

    // output values
  if (jlog::vbLevel(2)) {
    jlog::ls.printf("Zenith: %f degrees\n",zenith);
    jlog::ls.printf("Azimuth: %f degrees\n",azimuth);
    jlog::ls.printf("Sun light direction vector: [%f, %f, %f]\n",-sun_direc.x(),-sun_direc.y(),sun_direc.z());
  }

  return true;
}



// SkyTool::getTotalAndDiffuseIrrad() [private]
// returns false if something went wrong

bool SkyTool::getTotalAndDiffuseIrrad(const JParser& prm, SBandData& total_irrad, SBandData& diffuse_irrad, SBandData& direct_irrad) {

    // attempt to load total irradiance, false return means specification attempted but something wrong
  if (!RadUtils::loadSBandData(total_irrad, prm, "total_irrad", "total irradiance", *master_bs)) return false;
    // attempt to load diffuse irradiance, false return means specification attempted but something wrong
  if (!RadUtils::loadSBandData(diffuse_irrad, prm, "diffuse_irrad", "diffuse irradiance", *master_bs)) return false;
    // attempt to load direct irradiance, false return means specification attempted but something wrong
  if (!RadUtils::loadSBandData(direct_irrad, prm, "direct_irrad", "direct irradiance", *master_bs)) return false;

  if (total_irrad.bandCount()!=0 && diffuse_irrad.bandCount()!=0 && direct_irrad.bandCount()!=0 && jlog::ls.vbLevel(1)) {
    jlog::ls << "WARNING: It is recommended to only specify two of the three total, direct and diffuse irradiances. ";
    jlog::ls << "They may not be consistent and in that case the program behviour is undefined.\n";
  }

    // if nothing defined at all then use default
    // diffuse irradiance onto a horizontal plane is unity in all bands
  if (total_irrad.bandCount() == 0 && diffuse_irrad.bandCount() == 0 && direct_irrad.bandCount() == 0) {

    if (jlog::vbLevel(3)) jlog::ls << "No specification of sky irradiances so using default.\n";

    total_irrad = SBandData(master_bs->bandCount(), 1);
    diffuse_irrad = SBandData(master_bs->bandCount(), 0);
    direct_irrad = total_irrad - diffuse_irrad;

  } else if (total_irrad.bandCount()==0 && diffuse_irrad.bandCount()!=0 && direct_irrad.bandCount()!=0) {
    total_irrad = direct_irrad + diffuse_irrad;

  } else if (total_irrad.bandCount()!=0 && diffuse_irrad.bandCount()==0 && direct_irrad.bandCount()!=0) {
    diffuse_irrad = total_irrad - direct_irrad;

  } else if (total_irrad.bandCount()!=0 && diffuse_irrad.bandCount()!=0 && direct_irrad.bandCount()==0) {
    direct_irrad = total_irrad - diffuse_irrad;
  
  } else if (total_irrad.bandCount()==0) {
    total_irrad = SBandData(master_bs->bandCount(), 0);

  } else if (diffuse_irrad.bandCount()==0) {
    diffuse_irrad = SBandData(master_bs->bandCount(), 0);

  } else if (direct_irrad.bandCount()==0) {
    direct_irrad = SBandData(master_bs->bandCount(), 0);
  }

  if (jlog::vbLevel(3) && master_bs->hasWavelengthData()) {
    jlog::ls.printf("Total irradiance (%f Wm-2):\n",master_bs->integrateOverWavelength(total_irrad));
    jlog::ls << total_irrad.vectorData() << " (Wm-2nm-1)\n";
    jlog::ls.printf("Diffuse irradiance (%f Wm-2):\n",master_bs->integrateOverWavelength(diffuse_irrad));
    jlog::ls << diffuse_irrad.vectorData() << " (Wm-2nm-1)\n";
    jlog::ls.printf("Direct irradiance (%f Wm-2):\n",master_bs->integrateOverWavelength(direct_irrad));
    jlog::ls << direct_irrad.vectorData() << " (Wm-2nm-1)\n";
  }

  return true;
}



// SkyTool::loadFile()

bool SkyTool::loadFile(RadDirecImp<SBandData>*& emis, const JParser& prm) {
 
    // try to get a filepath to load
  JString fp = prm.getStringOrEmpty("file_load_fp");
  if (fp.isEmpty()) { jlog::es << "SKy type 'fileload' specified but no 'file_load_fp' provided.\n"; return false; }
 
    // try to load the full directional function
  RadDirecImp<SBandData>* new_emis = FileUtils::createFactoryBuiltFromFile< RadDirecImp<SBandData> >(FileDesc(fp, false));

  if (!new_emis) {
    jlog::es << "Failed to load input radiance file '" << fp << "'.\n";
    return false; 
  }

  // if (!FileUtils::readFactoryBuiltFromFile(*func, FileDesc(fp, false))) return false;
      // info
  if (jlog::ls.vbLevel(3)) jlog::ls << "Loaded '" << fp << "'.\n";
    // check number of bands
  if (new_emis->bandCount() != master_bs->bandCount()) {
    jlog::es << "File '" << fp << "' is in " << new_emis->bandCount() << " bands but should be " << master_bs->bandCount() << ".\n";
    delete new_emis;
    return false; 
  }

    // check directional structure   
  if (!new_emis->direcStruct().isEqual(emis->direcStruct())) { 
    jlog::es << "File '" << fp << "' has incorrect directional structure.\n";
    delete new_emis; 
    return false;
  }

  delete emis;
  emis = new_emis;
  
  return true;
}



// SkyTool::diffuseSky() [private]
// diffuse sky is a radiance of 1/PI Wm-2sr-1nm-1 in all directions
// so that is a planar irradiance of 1.0 Wm-2nm-1

bool SkyTool::diffuseSky(RadDirecImp<SBandData>& emis, const JParser& prm) {

  if (jlog::vbLevel(2)) jlog::ls.printf("Sky: diffuse\n");

    // initialise all radiances to zero
  emis.makeAll(SBandData(master_bs->bandCount(), 0));
    // initialise all z-plus direction radiances to 1/pi 
  emis.makeAllZPlus(SBandData(master_bs->bandCount(), 1/M_PI));
    // set the data type
  emis.setDataTypeMeanRad();

  return true;
}



// SkyTool::thetaZeroSky() [private]
// set all radiances to zero excpet in the quad closest to theta = 0, i.e. directly up
// equivalent to a planar irradiance of 1 Wm-2nm-1

bool SkyTool::thetaZeroSky(RadDirecImp<SBandData>& emis, const JParser& prm) {

  if (jlog::vbLevel(2)) jlog::ls.printf("Sky: thetazero\n");

    // initialise all radiances to zero
  emis.makeAll(SBandData(master_bs->bandCount(), 0));
    // the quad index corresponding to the direction (0,0,1) 
  int i = emis.direcStruct().direcToIndex(Vec3D(0,0,1));
    // quad-averaged radiance required to produce a planar irradiance of one
  double v = 1.0 / emis.direcStruct().solidAngleAbsCosTheta(i);
    // set the radiance in the quad
  emis.atIndex(i) = SBandData(master_bs->bandCount(), v);
    // set the data type
  emis.setDataTypeMeanRad();

  return true;
}



// SkyTool::specSky() [private]
// direct radiance in one quad the remainder of the sky diffuse (equal radiance in all quads).

bool SkyTool::specularSky(RadDirecImp<SBandData>& emis, const JParser& prm) {

  if (jlog::vbLevel(2)) jlog::ls.printf("Sky: specular\n");
 
    // sun position
  double zenith, azimuth;
  Vec3D sun_direc;
  if (!getSunPosition(prm, zenith, azimuth, sun_direc)) return false;

    // get total and diffuse irradiances
  SBandData total_irrad; 
  SBandData diffuse_irrad;
  SBandData direct_irrad;
  if (!getTotalAndDiffuseIrrad(prm, total_irrad, diffuse_irrad, direct_irrad)) return false;
  
    // this converts diffuse irradiance to radiance
  diffuse_irrad.divide(M_PI);
    // loop through all directions
  for (int i=0;i<emis.direcStruct().units();i++) { 
       // only defined on the top side (which will actually be downwards)
    if (emis.direcStruct().isZPlus(i)) emis.atIndex(i) = diffuse_irrad;
  }

    // get which quad the direct irradiance going out of 
  int i = emis.direcStruct().direcToIndex(sun_direc);
    // convert direct irradiance to radiance
  direct_irrad.divide(emis.direcStruct().solidAngleAbsCosTheta(i));
    // add on the direct radiance to the indirect radiance
  emis.atIndex(i) += direct_irrad;
    // set the data type
  emis.setDataTypeMeanRad();

  return true;
}



// SkyTool::hlidealSky() [private]
// parameters:
// type=hlideal
// C=<double> 0=uniform background sky 2=cardoidal sky 1.25=approx heavy overcast
// rdif=<double> ratio of diffuse to direct (between 0 and 1 inclusive) ame as Rsky in HydroLight output

bool SkyTool::hlidealSky(RadDirecImp<SBandData>& emis, const JParser& prm) {
 
  if (jlog::vbLevel(2)) jlog::ls.printf("Sky: hlideal\n");
 
  const DirecStruct& ds = emis.direcStruct();

    // sun position
  double zenith, azimuth;
  Vec3D sun_direc;
  getSunPosition(prm, zenith, azimuth, sun_direc);

    // get C value
  double C = prm.getDouble("C");
  if (C < 0 || C > 2) { jlog::es << "Bad C value, should be between 0 and 2 inclusive\n"; return false; }

    // get proportion diffuse irradiance in total irradiance
  double r = prm.getDouble("rdif");
  if (r < 0 || r > 1) { jlog::es << "Bad rdif ratio, should be between 0 and 1 inclusive\n"; return false; }

    // zero all the entries of the exitance to zero   
  emis.makeAll(SBandData(master_bs->bandCount(), 0));
 
    // to match other sky types arrange so that each (1 nm) band will integrate to PI diffuse irradance 
  double total_irradiance = M_PI;

    // calculate L0
  double L0 = (total_irradiance * r) / (M_PI * (1 + (2*C)/3));
  double sum = 0;

  if (jlog::vbLevel(2)) jlog::ls.printf("C = %f, r_dif = %f, L0 = %f\n",C,r,L0);

    // loop through all directions
  for (int i=0;i<ds.units();i++) { 

      // only defined on the top side (which will actually be downwards)
    if (ds.isZPlus(i) > 0) {

        // calculate radiance
      double L = L0 * (1 + C * ds.meanCosTheta(i));
      sum += L * emis.direcStruct().solidAngleAbsCosTheta(i);
      emis.atIndex(i) = SBandData(master_bs->bandCount(), L / M_PI);
    }
  }

  // now add on direct irradiance

    // for cube partitions and zero zenith angle need to set all four top quads
  if (ds.isCubePartition() && zenith==0) {

    int index0 = ds.direcToIndex(Vec3D(0.001,0.001,1));
    int index1 = ds.direcToIndex(Vec3D(-0.001,0.001,1));
    int index2 = ds.direcToIndex(Vec3D(0.001,-0.001,1));
    int index3 = ds.direcToIndex(Vec3D(-0.001,-0.001,1));
  
    if (jlog::vbLevel(2)) jlog::ls.printf("%d %d %d %d\n",index0,index1,index2,index3);
 
      // these four quads have the same solid angle and cos theta
    double v = (1 - r) / (4 * ds.solidAngleAbsCosTheta(index0));

    if (jlog::vbLevel(2)) jlog::ls.printf("v %f\n",v);
 
    SBandData sd(master_bs->bandCount(), v);

    emis.atIndex(index0) += sd;
    emis.atIndex(index1) += sd;
    emis.atIndex(index2) += sd;
    emis.atIndex(index3) += sd;
 
    if (jlog::vbLevel(2)) {
      jlog::ls.printf("Direct quad averaged radiance (sa %f, one of four): ",ds.solidAngle(index0));
      emis.atIndex(index0).display(jlog::ls);
      jlog::ls << "\n";
    }

    // otherwise just put the sun in one quad
  } else {

    int i = ds.direcToIndex(sun_direc);
    emis.atIndex(i) += SBandData(master_bs->bandCount(), (1 - r) / ds.solidAngleAbsCosTheta(i));

    if (jlog::vbLevel(2)) {
      jlog::ls.printf("Direct quad averaged radiance (sa %f): ",ds.solidAngle(i));
      emis.atIndex(i).display(jlog::ls);
      jlog::ls << "\n";
    }
  }

    // set the data type
  emis.setDataTypeMeanRad();

  return true;
}



// SkyTool::grantSky() [private]
// based on Grant et al 1996.
// the normalized radiance distribution in that paper does not in fact integrate to unity 
// over solid angle cos theta but goes up and up as scattering angle gets smaller
// so we limit the scattering angle to > sun angular width

bool SkyTool::grantSky(RadDirecImp<SBandData>& emis, const JParser& prm) {

  if (jlog::vbLevel(2)) jlog::ls.printf("Sky: grant\n");
 
  const double sun_angular_diameter = 0.51;

    // sun position
  double zenith_deg, azimuth_deg;
  Vec3D sun_direc;
  getSunPosition(prm, zenith_deg, azimuth_deg, sun_direc);
  double zenith_rad = degToRad(zenith_deg);
  double azimuth_rad = degToRad(azimuth_deg);

  double sun_cos_psi = cos((M_PI*sun_angular_diameter)/360);

   // get total and diffuse irradiances
  SBandData total_irrad; 
  SBandData diffuse_irrad;
  SBandData direct_irrad;
  if (!getTotalAndDiffuseIrrad(prm, total_irrad, diffuse_irrad, direct_irrad)) return false;

    // grant provides three equations, default is clear sky
  int model_eqn = 0;
  if (prm.isDefined("model_eqn")) {
    if (prm.valueCheck("model_eqn", "clear")) model_eqn = 0;
    else if (prm.valueCheck("model_eqn", "high_cloud")) model_eqn = 1;
    else if (prm.valueCheck("model_eqn", "low_cloud")) model_eqn = 2;
    else if (prm.valueCheck("model_eqn", "overcast")) model_eqn = 3;
    else {
      jlog::es << "'model_eqn = " << prm.getString("model_eqn") << "' is not a supported Grant sky model equation\n.";
      return false;
    }
  }

  if (jlog::vbLevel(2)) {
    jlog::ls.printf("model equation: ");
    switch (model_eqn) {
      case 0 : jlog::ls.printf("clear\n"); break;
      case 1 : jlog::ls.printf("high cloud\n"); break;
      case 2 : jlog::ls.printf("low cloud\n"); break;
      case 3 : jlog::ls.printf("overcast\n"); break;
    }
  }

    // size of sub-quads to split sphere into
  double sub_quad_sa = (4*M_PI) / 1000000; // 1.25e-5
  if (prm.isDefined("sub_quad_count")) sub_quad_sa = (4*M_PI) / prm.getInt("sub_quad_count");
  else if (prm.isDefined("sub_quad_solid_angle")) sub_quad_sa = prm.getDouble("sub_quad_solid_angle");
 
  const DirecStruct& ds = emis.direcStruct();

  basic_radtype sum(0);
  double sun_sa = 0;
  int sun_quad_count = 0;
  int quads_done = 0;
 
  if (jlog::vbLevel(2)) jlog::ls << "Processing...\n";
 
    // loop through all directions
  for (int i=0;i<ds.units();i++) { 

      // only defined on the top side (which will actually be downwards)
    if (ds.meanCosTheta(i) > 0) {

      QuadIntegrator* qi = ds.quadAtIndex(i)->createIntegrator(sub_quad_sa);

      if (jlog::vbLevel(2)) {
	jlog::ls.printf("quad %d/%d sub count: %d   \n", i, (ds.isFull() ? ds.units()/2 : ds.units())-1, qi->subQuadCount());
	jlog::ls.device().cursorUp(1);
      }

      bool qf = false;

      while (!qi->atEnd()) {

          // get the outward direction of the emitance quad
	Vec3D v(qi->direction());

	double theta = v.thetaFromUnit();
	double phi = v.phiFromUnit();
	phi = phi - azimuth_rad;

          // scattering angle
	double cos_psi = v.z()*cos(zenith_rad) + sin(theta)*sin(zenith_rad)*cos(phi);
	  // normalized sky radiance - only if outside sun diameter
	double N;

	switch (model_eqn) {
	  case 0 :
	      // clear sky
	    N = (cos_psi < sun_cos_psi) ? 0.0361 * (6.3 + (1 + cos_psi*cos_psi)/(1 - cos_psi)) * (1 - exp(-0.31/v.z())) : 0;
	    break;
  	  case 1:
              // high translucent cloud
	      // by trial and error deduced that zenith_rad and the scattering angle are in radians here
	    N = 0.149 + 0.084 * zenith_rad + 1.305 * exp(-2.5 * acos(cos_psi));
	    break;
	  case 2:
	      // low translucent cloud
              // by trial and error deduced that zenith_rad and the scattering angle are in radians here
	    N = 0.080 + 0.058 * zenith_rad + 0.652 * exp(-2.1 * acos(cos_psi));
	    break;
	  default:
	      // sun totally obscured
            N = 0.441 * (1 + 4.6 * v.z()) / 5.6;
	}

	  // sum solid angle of sub quads in sun direction
	if (cos_psi >= sun_cos_psi) {
	  sun_sa += qi->solidAngle();
	  qf = true;
	}

	//if (cos_psi > sun_cos_psi) printf("HERE\n");

	//printf("direc (scat angle %f) ",180*acos(cos_psi)/M_PI);
	//v.display(ls);

	  // this one does sum to 1
	//double N = 0.441 * (1 + 4.6 * v.z()) / 5.6;
	
	// harrison and coombes
	//N = (A + B*exp(-m*psi) + C*cos_psi*cos_psi*cos(zenith_rad)) * (1-exp(-rho/v.z())) * (1-exp(-gamma/sun_direc.z())

          // keep track of the N irradiance integral to calc correction factor (should be 1)
	sum += N * qi->solidAngleAbsCosTheta();

	  // add to this quad the scaled diffuse irradiance
        emis.atIndex(i) += (diffuse_irrad * N) * (qi->solidAngle() / ds.solidAngle(i));

	  // next sub quad
	qi->next();
      }

      if (qf) sun_quad_count++;
     
        // delete the integrator
      delete qi;

        // current completion
      quads_done++;
      Tool::pct_complete = (200.0 * quads_done) / ds.units();
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "\n";
  
    // N does not really sum to 1 as it should 
    // because peak near sun is like an asymptote
    // is discussed in paper - use correction factor
  if (jlog::vbLevel(3)) jlog::ls.printf("N integral = %f\n",sum);
  emis.divide(sum);
  
  emis.lazyInvalidate();
  SBandData sd(emis.sumBySolidAngleAbsCosTheta());
  if (jlog::vbLevel(3)) jlog::ls << "Calculated diffuse:\n" << sd.vectorData() << "\n";

    // fix diffuse irradiance
  SBandData correc(diffuse_irrad);
  correc.divideExceptZeros(sd);
  emis.multiplyAll(correc);
  emis.lazyInvalidate();

  if (jlog::vbLevel(3)) {
    jlog::ls << "Corrected diffuse:\n" << emis.sumBySolidAngleAbsCosTheta().vectorData() << "\n";
    jlog::ls.printf("sun_sa: %f\n",sun_sa);
    jlog::ls.printf("sun_quad_count: %d\n",sun_quad_count);
  }

    // get which quad the direct irradiance is coming in 
  int i = emis.direcStruct().direcToIndex(sun_direc);
    // convert direct irradiance to radiance
  direct_irrad.divide(emis.direcStruct().solidAngleAbsCosTheta(i));
    // add on the direct radiance to the indirect radiance
  emis.atIndex(i) += direct_irrad;
    // set the data type
  emis.setDataTypeMeanRad();

  return true;
}



/*

// SkyTool::cieSky() [private]

bool SkyTool::cieSky(SpecImp<SpectralData>& emis, const JParser& prm) {

  jlog::es << "cie sky is currently disabled\n";
  return false;

  const bool debug_flag = false;

  static const struct {

    double a;
    double b;
    double c;
    double d;
    double e;

  } params[15] = { 

    {  4.0, -0.70,  0, -1.0, 0.00 },  // 1
    {  4.0, -0.70,  2, -1.5, 0.15 },  // 2
    {  1.1, -0.80,  0, -1.0, 0.00 },  // 3
    {  1.1, -0.80,  2, -1.5, 0.15 },  // 4
    {  0.0, -1.00,  0, -1.0, 0.00 },  // 5
    {  0.0, -1.00,  2, -1.5, 0.15 },  // 6
    {  0.0, -1.00,  5, -2.5, 0.30 },  // 7
    {  0.0, -1.00, 10, -3.0, 0.45 },  // 8
    { -1.0, -0.55,  2, -1.5, 0.15 },  // 9
    { -1.0, -0.55,  5, -2.5, 0.30 },  // 10
    { -1.0, -0.55, 10, -3.0, 0.45 },  // 11
    { -1.0, -0.32, 10, -3.0, 0.45 },  // 12
    { -1.0, -0.32, 16, -3.0, 0.30 },  // 13
    { -1.0, -0.15, 16, -3.0, 0.30 },  // 14
    { -1.0, -0.15, 24, -2.8, 0.15 }   // 15
  };
 
    // get cie sky type
  int type = prm.getInt("cietype");
  if (type < 1 || type > 15) dataError("Bad cietype in cie sky.");

  ls.printf("Sky: cie type %d\n",type);
 
    // sun position
  double zenith, azimuth;
  Vec3D sun_direc;
  getSunPosition(prm, zenith, azimuth, sun_direc);

  SpectralData sd(1);

  const double a = params[type-1].a;
  const double b = params[type-1].b;
  const double c = params[type-1].c;
  const double d = params[type-1].d;
  const double e = params[type-1].e;

  if (debug_flag) {
    ls.printf("sun light direction vector:\n");
    ls << sun_direc << "\n";
    ls.printf("params: %.2f %.2f %.2f %.2f %.2f\n",a,b,c,d,e);
  }

    // zero all the entries of the exitance to zero   
  emis.makeAll(SpectralData::null());

    // Zs is the sun zenith angle equivalent to the eleavation angle or theta
    // basically the angle between the sun outward direction and the z=1 vector
  double Zs = sun_direc.thetaFromUnit();
 
    // this is the multiplier which is constant for all directions
    // assumes zenith luminance as 1
  //  double mult_fac = 1 / (f_func(c,d,e,Zs)*phi_func(a,b,0));

    // maximum luminance is at sundirection (chi=0) and at the sun elevation Zs
    // should ensure don't get luminance > 1 (which will saturate the emitance)
  double mult_fac = 8 / (f_func(c,d,e,0)*phi_func(a,b,Zs));

    // loop through all directions
  for (int i=0;i<emis.direcStruct().units();i++) { 

      // get the outward direction of the emitance quad
    Vec3D v = emis.direcStruct().indexToPointDirec(i);
   
      // only defined on the top side (which will actually be downwards)
    if (v.z() > 0) {

        // chi is the angle between the quad emitance direction and the sun outward direction
      double chi = acos(Vec3D::dotProduct(v,sun_direc));
  
        // Z is the emitance zenith angle equivalent to the elevation angle or theta
        // basically the angle between the quad emitance direction and the z=1 vector
      double Z = v.thetaFromUnit();

      double lum = mult_fac * f_func(c,d,e,chi) * phi_func(a,b,Z);

      if (debug_flag) {
	ls << v << "\n";
	ls.printf("chi %.3f\n",chi); 
	ls.printf("Z %.2f\n",Z);
	ls.printf("phi func %.3f\n", phi_func(a,b,Z));
	ls.printf("f func %.3f\n", f_func(c,d,e,chi));
	ls.printf("luminance %.3f\n",lum); 
      }

      SpectralData sd_correc(sd);
      sd_correc.multiply(lum);
     
        // if this is received energy model then convert to integral of radiance in times cos theta 
      //if (recv) sd_correc.multiply(emis.direcStruct().solidAngleAbsCosTheta(i));
      emis.atIndex(i) = sd_correc;
    }
  }
  
  //if (!recv) {
     // the integral of radinace over sphere onto a a horizontal surface
  SpectralData sd_correc = emis.sumBySolidAngleAbsCosTheta();

  ls.printf("integral: ");
  sd_correc.display(ls);
    // to make equivalent to 'unit' sky distribution this integral should be M_PI
  emis.multiplyAll(M_PI/sd_correc[0]);
    //}

  return true;
}



// SkyTool::phi_func() [private]
// used by cieSky()

double SkyTool::phi_func(double a, double b, double z) {
  return 1 + a * exp(b / cos(z));
}



// SkyTool::f_func() [private]
// used by cieSky()

double SkyTool::f_func(double c, double d, double e, double chi) {
  return 1 + c * (exp(d*chi) - exp(d*M_PI/2)) + e * cos(chi*chi);
}




// SkyTool::libradtranSky()
// invokes external program "uvspec"

bool SkyTool::libradtranSky(SpecImp<SpectralData>& emis, const JParser& prm) {

  if (vbLevel(2)) ls.printf("Sky: libradtran\n");

  if (!prm.isDefined("libradtran_dir")) { 
    jlog::es << "** libRadtran directory is not specified so rquired files cannot be located **\n";
    jlog::es << "** if libRadtran is installed please specify where by using libradtran_dir=<path> or setting AQ_LIBRADTRAN_DIR **\n"; 
    jlog::es << "** or recompiling with --with-libradtran=<path> as an option to ./configure **\n"; 
    return false;
  }

  JString libradtran_dir = prm.getString("libradtran_dir");

    // wavelength limits need to do
  double wave_min = SpectralData::bandMin()[0];
  double wave_max = SpectralData::bandMax()[SpectralData::bands()-1];

  const DirecStruct& ds = emis.direcStruct();

  vector<double> phi_position;
  vector<double> theta_position;

  //int phi_count;
  //int theta_count;

  int res = 2;

    // if creating a spherical partition
  if (ds.isSpherePartition()) {

      // cast to a spherical partition - should never fail 
    const SpherePartition* sp = dynamic_cast<const SpherePartition*>(&ds);
    //if (!sp) return internalError(__FILE__,__LINE__); 

    int phi_count = sp->horizSize() * res;
    int theta_count = sp->vertSize() * res + 1;

    phi_position.resize(phi_count);
    theta_position.resize(theta_count);

    for (int i=0; i<phi_count; i++) {
      phi_position(i) = (i*360.0) / phi_count;
      printf("%d %f\n", i, phi_position(i));
    }
   
    for (int i=0; i<theta_count-1; i++) {
      int j = i / res;
      double frac = ((double) i - (j * res)) / res;
      theta_position(i) = sp->thetaPointsDeg()[j] * (1-frac) + sp->thetaPointsDeg()[j+1] * frac;
      printf("%d %f\n", i, theta_position(i));
    }
    theta_position(theta_count-1) = sp->thetaPointsDeg()[sp->vertSize()];

  } else {
    jlog::es << "libRadtran not currently supported for cuvbe partitions\n";
    return false;
  }

  JFile f(prm.getString("temp_dir")+"/uvspec_input");
  if (!f.open(IO_WriteOnly)) {
    jlog::es << "** Failed to write uvspec input file to temporary directory **\n";
    jlog::es << "** Ensure directory is specified correctly by temp_dir=<path> or environment variable AQ_TEMP_DIR **\n";
    return false;
  }

  if (vbLevel(2)) ls << "libRadtran directory: \"" << libradtran_dir << "\"\n";

    // text stream to write to the file
  JTextStream str(&f);

    // where to find libRadtran data files
  str << "data_files_path " << libradtran_dir << "/data\n";
    // tropical atmosphere
  str << "atmosphere_file " << libradtran_dir << "/data/atmmod/afglus.dat\n";
    // extraterrestrial spectrum
  str << "solar_file " << libradtran_dir << "/data/solar_flux/atlas_plus_modtran\n";
    // scale ozone to this many DU
  str << "ozone_column 300.0\n";
    // wavelength range to calculate
  str << "wavelength " << wave_min << " " << wave_max << "\n";
    // basically says we want 1 nm intervals
  str << "spline " << wave_min << " " << wave_max << " 1\n";

  str << "day_of_year 170\n";
  str << "albedo 0.2\n";
  str << "rte_solver disort2\n";
  str << "nstr 16\n";
  str << "deltam on\n";
  str << "sza 32.0\n";
  str << "phi0 40.0\n";

  str << "umu  -1.0 -0.5 -0.2 -0.1\n";    
  str << "phi 0.0 45.0 90.0 135.0 180.0 225.0 270.0 315.0\n";
  str << "quiet\n";

  f.close();

  return true;
}


*/

// SkyTool::radtranSky() [private]
// this is the old incomplete radtran model sky - will probably never be completed
/*

bool SkyTool::radtranSky(SpecImp<SpectralData>& emis, const JParser& prm) {

    // this is "standard pressure" in mb note: looks like 1013.25 mb = 101.325 KPa
  const double standard_pressure = 1013.25;

    // ozone scale height in cm (set to 5km) - need to find out how to get this
  double ozone_scale_height = 500000;

     // get sun position
  double zenith_deg, azimuth_deg;
  Vec3D sun_direc;
  getSunPosition(prm, zenith_deg, azimuth_deg, sun_direc);
  double zenith_rad = GlobalFuncs::degreesToRadians(zenith_deg);

  double ozone_optical_mass = 1 / (sun_direc.z() + 268.45 * sqrt(zenith_deg) * pow(115.420 - zenith_deg, -3.2922));
  ls.printf("ozone optical mass %f\n",ozone_optical_mass);

  JString waveln_filepath(prm.getString("waveln_filepath"));
  JString H0_filepath(prm.getString("et_filepath"));
  JString ozone_filepath(prm.getString("ozone_filepath"));
  JString oxygen_filepath(prm.getString("oxygen_filepath"));
  JString water_filepath(prm.getString("water_filepath"));

    // day of the year counting from first of January ( = 1 presumably)
  int D = prm.getInt("day");

    // if pressure is not defined use "standard pressure" which just 
    // means no extra corrections for pressure
  double pressure = (prm.isDefined("pressure")) ? prm.getDouble("pressure") : standard_pressure;
 
    // read in the wavelengths to use
  Vector waveln;
  waveln.readFromTextFile(waveln_filepath);

    // read in the mean extraterrestrial irradiance
  Vector H0;
  H0.readFromTextFile(H0_filepath);
    // read in the ozone absorption co-efficients
  Vector ozone_abs;
  ozone_abs.readFromTextFile(ozone_filepath);
    // read in the oxygen absorption co-efficients
  Vector oxygen_abs;
  oxygen_abs.readFromTextFile(oxygen_filepath);
    // read in the water vapour absorption co-efficients
  Vector water_abs;
  water_abs.readFromTextFile(water_filepath);

  ls.printf("original wavelengths: %d\n",waveln.size());

  if (waveln.size() != H0.size() || 
      waveln.size() != oxygen_abs.size() || 
      waveln.size() != water_abs.size() || 
      waveln.size() != ozone_abs.size()) dataError("mismatch in length of atmosphere data vectors\n");

  double v = 1 + 0.0167 * cos((2*M_PI*(D-3))/365);
  v = v * v; 
  ls.printf("Day %d, orbit eccentricity correction: %f\n",D,v);

    // correct extra terrestrial irradiance for earth-sun distance
  Vector F0(H0);
  F0.multiply(v);

    // atmospheric path length
  double M = 1 / (sun_direc.z() + 0.15 * pow((93.885 - zenith_deg), -1.253));
    // pressure corrected atmospheric path langth
  double Ma = (M * pressure) / standard_pressure;

  ls.printf("Atmospheric path length: %f (%f)\n",M,Ma);

    // ozone path length
  double Moz = 1.0035 / sqrt(sun_direc.z()*sun_direc.z() + 0.007);

  ls.printf("Ozone path length: %f\n",Moz);

    // Rayleigh scattering (transmittance)
  Vector Tr(waveln.size());
  for (int i=0;i<waveln.size();i++) {
      // convert wavelength from nm to to um
    double lambda = waveln[i] / 1000;
    double l2 = lambda * lambda;
    double l4 = l2 * l2;
    Tr[i] = exp(-Ma/(115.6406*l4 - 1.335*l2));
  }

  //printf("Tr:\n");
  //Tr.display(ls);

    // ozone transmittance
  Vector Toz(waveln.size());
  for (int i=0;i<waveln.size();i++) {
    Toz[i] = exp(-ozone_abs[i] * ozone_optical_mass * Moz);
  }

  //printf("Toz:\n");
  //Toz.display(ls);

    // oxygen transmittance
  Vector To(waveln.size());
  for (int i=0;i<waveln.size();i++) {
    double v1 = -1.41 * oxygen_abs[i] * Ma;
    double v2 = pow((1 + 118.3 * oxygen_abs[i] * Ma), 0.45);
    To[i] = exp(v1/v2);
  }

  ls.printf("To:\n");
  ls << To << "\n";

  //printf("H0:\n");
  //H0.display(ls);

  //printf("F0:\n");
  //F0.display(ls);

  return true;
}
*/


void SkyTool::helpText(JTextStream& str) const {

  str << "\n";
  str << "Sky Model Specification\n";
  str << "-----------------------\n";
  str << "type = diffuse | thetazero | specular | hlideal | grant\n\n";

  str << "Output File Specification\n";
  str << "-------------------------\n";
  str << "sky_save_fp = <filepath>  (file to save the sky direction quad-averaged radiance to)\n\n";

  str << "Output Band And Directional Structure Specification From Files\n";
  str << "--------------------------------------------------------------\n";
  str << "band_spec_fp   = <filepath>  (filepath of output band specification)\n";
  str << "direc_spec_fp  = <filepath>  (filepath of output directional structure specification)\n";
  str << "do_full_sphere = yes | no  (save sky as a full spherical directional structure, default 'no')\n\n";
  
  str << "Model 'diffuse'\n";
  str << "---------------\n";
  str << "(no additional parameters)\n\n";

  str << "Model 'thetazero'\n";
  str << "-----------------\n";
  str << "(no additional parameters)\n\n";

  str << "Model 'specular'\n";
  str << "----------------\n";
  str << "zenith  = <real>  (solar zenith angle in degrees)\n";
  str << "azimuth = <real>  (solar azimuth angle in degrees)\n\n";
  str << "And see below for specification of sky irradiances.\n";
  str << "\n";

  str << "Model 'hlideal'\n";
  str << "---------------\n";
  str << "zenith  = <real>  (solar zenith angle in degrees)\n";
  str << "azimuth = <real>  (solar azimuth angle in degrees)\n";
  str << "C       = <real>  (C value, valid range 0 - 2)\n";
  str << "rdif    = <real>  (rdif ratio, valid range 0 - 1)\n";
  str << "\n";

  str << "Model 'grant'\n";
  str << "-------------\n";
  str << "zenith               = <real>  (solar zenith angle in degrees)\n";
  str << "azimuth              = <real>  (solar azimuth angle in degrees)\n";
  str << "model_eqn            = clear | high_cloud | low_cloud | overcast  (model form to use, default 'clear')\n\n";
  str << "And see below for specification of sky irradiances.\n";
  str << "\n";

  str << "Sky Irradiances For Models 'specular' And 'grant'\n";
  str << "-------------------------------------------------\n";
  str << "total_irrad_data           = <r1,r2,r3, ... rn>  (directly specified total spectral irradiance in Wm-2nm-1, optional)\n";
  str << "diffuse_irrad_data         = <r1,r2,r3, ... rn>  (directly specified diffuse spectral irradiance in Wm-2nm-1, optional)\n";
  str << "direct_irrad_data          = <r1,r2,r3, ... rn>  (directly specified direct spectral irradiance in Wm-2nm-1, optional)\n";
  str << "total_irrad_fp             = <filepath>  (filepath to total spectral irradiance in Wm-2nm-1, optional)\n";
  str << "diffuse_irrad_fp           = <filepath>  (filepath to diffuse spectral irradiance in Wm-2nm-1, optional)\n";
  str << "direct_irrad_fp            = <filepath>  (filepath to direct spectral irradiance in Wm-2nm-1, optional)\n";
  str << "total_irrad_band_spec_fp   = <filepath>  (band specification by file for total irradiance, optional)\n";
  str << "diffuse_irrad_band_spec_fp = <filepath>  (band specification by file for diffuse irradiance, optional)\n";
  str << "direct_irrad_band_spec_fp  = <filepath>  (band specification by file for direct irradiance, optional)\n\n";
  str << "Note: any two of total, direct and diffuse irradiances can be specified.\n";
  str << "See below for how to directly specify bands and wavelengths.\n";
  str << "\n";

  str << "Additonal Options\n";
  str << "-----------------\n";
  str << "sub_quad_count        = <integer> (approximate number of divisions of the sphere to perform integrations over, default 1E6)\n";
  str << "sub_quad_solid_angle  = <real>    (solid angle of divisions of the sphere to perform integrations over, default 1.25E-5)\n";
  str << "azimuthally_average   = yes | no  (for spherical partitions only, will make output uniform for azimuth, optional)\n";
  str << "\n";

  str << "Output Hemisphere Image Specification\n";
  str << "-------------------------------------\n";
  str << "sky_image_save_fp  = <filepath>  (filepath to save hemisphere PPM image to, optional)\n";
  str << "sky_image_size     = <integer>  (size of hemisphere PPM image in pixels)\n";
  str << "sky_image_sens_k   = <real>  (sensitivity of radinace to RGB conversion, optional)\n\n";

  Tool::helpTextBandAndDirecSpec(str);
  str << "Output: (no prefix)\n";
  str << "Total irradiance:   'total_irrad_'\n";
  str << "Diffuse irradiance: 'diffuse_irrad_'\n";
  str << "Direct irradiance:  'direct_irrad_'\n\n";

  str << "Parameters For Direct Band Specification\n";
  str << "----------------------------------------\n";
  BandSpec::helpText(str);
  str << "\n";

  str << "Parameters For Direct Directional Structure Specification\n";
  str << "---------------------------------------------------------\n";
  DirecSpec::helpText(str);
  str << "\n";

  Tool::helpText(str);
}

