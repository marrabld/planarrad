 
// phasetool.cc

#include "phasetool.h"
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
#include "rad/radutils.h"
#include "math/mathutil.h"
#include "rad/hemispheredraw.h"
#include "config.h"


using namespace jude::file;
using namespace jude::math;



// NOTE:
// used to be vsfs were stored times 4*PI
// not anymore



// PhaseTool::(constructor)

PhaseTool::PhaseTool() : 
  Tool(),
  master_bs(0),
  master_ds_spec(0),
  phase_func_tab(0) { init(); }



// PhaseTool::(destructor)

PhaseTool::~PhaseTool() {
  delete master_bs;
  delete master_ds_spec;
  delete phase_func_tab;
}



// PhaseTool::init()

void PhaseTool::init() {

  static bool done_init = false;
  if (done_init) return;

    // somewhere in all programs there must occur a list of all the static polymorphic types that can occur
  JFactory< RadDirecImp< SBandData>, RadDirecTable< SBandData, HemiSpherePartVxH > >::instance().reg();
  JFactory< RadDirecImp< SBandData>, RadDirecTable< SBandData, SpherePartVxH > >::instance().reg();
  JFactory< RadDirecDirecImp< SBandData>, RadDirecDirecTable< SBandData, HemiSpherePartVxH > >::instance().reg();
  JFactory< RadDirecDirecImp< SBandData>, RadDirecDirecTable< SBandData, SpherePartVxH > >::instance().reg();


  done_init = true;
}



// PhaseTool::run()

bool PhaseTool::run(const JParser& prm) {

    // basic initialisation common to all tools
  if (Tool::init(prm)) return true;
  Tool::initRandom(prm);

  if (jlog::vbLevel(5)) prm.displayList(jlog::ls);

  uncancelProcessing();

    // delete any previous allocation
  delete master_bs;
  delete master_ds_spec;
  delete phase_func_tab;

    // get output band specification or quit with error
  master_bs = BandSpec::create(prm, "");
  if (!master_bs) { jlog::es << "No output band specification given or it is incomplete.\n"; return false; }
    // pull out band count into ist own vaiable for ease of access
  band_count = master_bs->bandCount();

    // get output directional specification or quit with error
  master_ds_spec = DirecSpec::create(prm, "");
  if (!master_ds_spec) { jlog::es << "No output directional specification given or it is incomplete.\n"; return false; }
    // try to build a full directional spectral data table
  phase_func_tab = buildRadDirecDirec<SBandData>(*master_ds_spec, master_bs->bandCount(), true);
    // if failed an error will have been streamed to jlog::es
  if (!phase_func_tab) return false;
    // this is the output data type, tabulated values are L_out / (L_in*sa)
  phase_func_tab->setInDataTypeScalarIrrad();
  phase_func_tab->setExDataTypeMeanRad();
 
    // initialise completion count
  Tool::pct_complete = 0;

    // size of sub-quads to split sphere into
  if (prm.isDefined("sub_quad_count")) sub_quad_sa = (4*M_PI) / prm.getInt("sub_quad_count");
  else if (prm.isDefined("sub_quad_solid_angle")) sub_quad_sa = prm.getDouble("sub_quad_solid_angle");
  else sub_quad_sa = (4*M_PI) / 20000;

  wl.clear();
  if (prm.isDefined("wavelength_list")) wl = prm.getVectorOfDoubles("wavelength_list");

  JString build_type(prm.getString("type"));
 
  bool ret_val = true;

  if (build_type == "isotropic") {

    if (jlog::vbLevel(2)) jlog::ls << "Isotropic phase function.\n";
      // the incident scalar irradiance in any quad is distributed over the exitant quads in proportion to their solid angle
      // scalar_irrad_out = (out_sa / 4pi) x scalar_irrad_in
      // the desired function value is outgoing radiance divided by incident scalar irradiance
      // note that: scalar_irrad_out = L_out x out_sa
      // so required value at each quad pair is L_out/scalar_irrad_in = 1/4pi
    phase_func_tab->makeAll(SBandData(master_bs->bandCount(), 1.0/(4*M_PI)));

    // forward scattering only 
  } else if (build_type == "forward") {
 
    if (jlog::vbLevel(2)) jlog::ls << "Forward scattering only phase function.\n";
      // make everything zero
    phase_func_tab->makeAll(SBandData(master_bs->bandCount(), 0)); 
      // everything incident in a quad is exitant from the opposite quad, all other entries are zero 
    for (int i=0; i<phase_func_tab->direcStruct().units(); i++) {
      //int out_index = phase_func_tab->direcStruct().negatedIndex(i);
      int out_index = i;
        // the desired function value is outgoing radiance divided by incident scalar irradiance
        // unit of conservation is power - we want the same planar irrad to come out as goes in
        // so divide outgoing (numerator) by sact and incoming (denominator) by ct - result divide by sa  
      phase_func_tab->setData(i, out_index, SBandData(band_count, 1.0 / phase_func_tab->direcStruct().solidAngle(i)));
    }

    // forward and backward scattering only
  } else if (build_type == "back_and_forward") {
 
    if (jlog::vbLevel(2)) jlog::ls << "Back and forward scattering only phase function.\n";
      // make everything zero
    phase_func_tab->makeAll(SBandData(master_bs->bandCount(), 0)); 
      // everything scattered from an incident in a quad is exitant from the same quad and the opposite in equal proportion 
    for (int i=0; i<phase_func_tab->direcStruct().units(); i++) {
        // the desired function value is outgoing radiance divided by incident scalar irradiance
        // unit of conservation is power - we want the same planar irrad to come out as goes in
        // so divide outgoing (numerator) by sact and incoming (denominator) by ct - result divide by sa  
      SBandData sd(band_count, 0.5 / phase_func_tab->direcStruct().solidAngle(i));
      phase_func_tab->setData(i, i, sd);
      phase_func_tab->setData(i, phase_func_tab->direcStruct().negatedIndex(i), sd);
    }

    // test integration of isotropic
  } else if (build_type == "isotropic_integ") {

    if (jlog::vbLevel(2)) jlog::ls << "Isotropic (integrated test) phase function.\n";
    PhaseFunc func("SINGLE", FileSetDesc("__ISOTROPIC__"), wl, master_bs);
    ret_val = integratePhaseFunc(func);

  } else if (build_type == "petzold") {

    if (jlog::vbLevel(2)) jlog::ls << "Petzold phase function.\n";
    PhaseFunc func("SINGLE", FileSetDesc("__PETZOLD__"), wl, master_bs);
    ret_val = integratePhaseFunc(func);

  } else if (build_type == "water") {

    if (jlog::vbLevel(2)) jlog::ls << "Pure water phase function.\n";
    PhaseFunc func("SINGLE", FileSetDesc("__PUREWATER__"), wl, master_bs);
    ret_val = integratePhaseFunc(func);

  } else if (build_type == "file") {
  
    if (!prm.isDefined("file_list")) { jlog::ls << "Build type is 'file' but no file_list supplied.\n"; return false; }
  
      // comma seperated list of vector files for phase function at a series of wavelengths
      // or comma separated list of files with Bp for Fournier-Fourand pf (Bp = bbp/bp, particulate backscatter fraction)
      // will also need to add on weighted pure water phase func to get the whole thing (?)
      // can also be pseudo-files __ISOTROPIC__, __PETZOLD__, but these are undocumented
    FileSetDesc func_fsd(prm.getString("file_list"));
      // get how to build the function in each band from the specified files
    JString mode_str;
    if (!getBandMode(mode_str, func_fsd, prm)) return false;

    PhaseFunc func(mode_str, func_fsd, wl, master_bs);
    ret_val = integratePhaseFunc(func);
 
    // Fournier-Forand phase function
  } else if (build_type == "ff") {
 
    if (jlog::vbLevel(2)) jlog::ls << "Fournier-Forand phase function.\n";
 
      // read list of backscatter proportions
    std::vector<JString> v = prm.getVectorOfStrings("backscatter_proportion_list");
    if (v.size() == 0) { jlog::ls << "Build type is 'ff' but no backscatter_proportion_list supplied"; return false; }

    FileSetDesc func_fsd;
    for (unsigned int i=0; i<v.size(); i++) {
      func_fsd.appendFile(FileDesc("__FOURNIER__"+v[i]+"__", true));
    }

      // get how to build the function in each band from the specified files
    JString mode_str;
    if (!getBandMode(mode_str, func_fsd, prm)) return false;

    PhaseFunc func(mode_str, func_fsd, wl, master_bs);
      // initialisation can fail if a solution for mu could not be found
    if (!func.initOk()) return false;
    ret_val = integratePhaseFunc(func);

  } else if (build_type == "fileload") {

    if (!loadFile(prm)) return false;

  } else {

    jlog::es << "Unsupported build type '" << build_type << "'.\n";
    return false;
  }

    // quit if something went wrong
  if (!ret_val) return false;

  if (jlog::vbLevel(3)) {

    jlog::ls.printf("Diffuse scatter (Note: will not be the same as bb/B!):\n");
    phase_func_tab->diffuseScatter().display(jlog::ls);
    jlog::ls << "\n";

    RadUtils::displayHalfFunctionsAsScatter(*phase_func_tab, jlog::ls);
  }

  double fwd_sum = 0;
  double back_sum = 0;
  double sa_sum = 0;

    // this only works for spherical partitions
  for (int out_index=0; out_index<phase_func_tab->direcStruct().units(); out_index++) {

    double f = phase_func_tab->atIndices(0, out_index).bandValue(0) * phase_func_tab->direcStruct().solidAngle(out_index);
    sa_sum += phase_func_tab->direcStruct().solidAngle(out_index);
      // now the same in and out index is forward scattering, change of hemisphere is backscatter
    if (out_index >= phase_func_tab->direcStruct().units() / 2) back_sum += f;
    else fwd_sum += f;
  }
  //fwd_sum /= 4*M_PI;
  //back_sum /= 4*M_PI;
  if (jlog::ls.vbLevel(2)) jlog::ls.printf("Index 0 incident: fwd %f back %f back frac %f\n",fwd_sum,back_sum,back_sum/(fwd_sum+back_sum));
 
    // if file to save to is specified 
  if (prm.isDefined("phase_func_save_fp")) {
      // get file path
    JString out_filepath = prm.getString("phase_func_save_fp");
      // build notes using function in Tool
    JTextDevice nd;
    buildNotes(nd, prm);
       // generic writer that includes information for reconstruction of dynamic types on reload
    FileUtils::writeFactoryBuiltToFile(*phase_func_tab, FileDesc(out_filepath, false)); 
      // for info
    if (jlog::vbLevel(1)) {
      jlog::ls << "Written file '" << out_filepath << "' ";
      jlog::ls << "as a tabulated direction-to-direction function of " << phase_func_tab->inDataString() << " to " << phase_func_tab->exDataString() << ".\n";
    }
  }

  Tool::pct_complete = 100;

  return true;
}



// PhaseTool::buildFromFile()

bool PhaseTool::loadFile(const JParser& prm) {
  
    // try to get a filepath to load
  JString fp = prm.getStringOrEmpty("file_load_fp");
  if (fp.isEmpty()) { jlog::es << "Phase function type 'fileload' specified but no 'file_load_fp' provided.\n"; return false; }
 
    // try to load the full directional function
  RadDirecDirecImp<SBandData>* new_func = FileUtils::createFactoryBuiltFromFile< RadDirecDirecImp<SBandData> >(FileDesc(fp, false));

  if (!new_func) {
    jlog::es << "Failed to load phase function file '" << fp << "'.\n";
    return false; 
  }
      // info
  if (jlog::ls.vbLevel(3)) jlog::ls << "Loaded '" << fp << "'.\n";
    // check number of bands
  if (new_func->bandCount() != master_bs->bandCount()) {
    jlog::es << "File '" << fp << "' is in " << new_func->bandCount() << " bands but should be " << master_bs->bandCount() << ".\n";
    delete new_func;
    return false; 
  }

    // check directional structure   
  if (!new_func->direcStruct().isEqual(phase_func_tab->direcStruct())) { 
    jlog::es << "File '" << fp << "' has incorrect directional structure.\n";
    delete new_func; 
    return false;
  }

  delete phase_func_tab;
  phase_func_tab = new_func;
  
  return true;
}



//  PhaseTool::getBandMode()

bool PhaseTool::getBandMode(JString& mode_str, const FileSetDesc& fsd, const JParser& prm) {

  // phase func mode string - describes how to build a multi-band phase function from the files
  // can be one of:
  //   single - there is only one file so build a 1-band phase function
  //   band - each file is the phase function for each successive bands
  //   ipol - use the wavelength of the phase function to build wavelength-interpolated phase function for each band

    // if only one band we can assume band_mode=single if not specified
  if (!prm.isDefined("band_mode")) { 

    if (band_count == 1) mode_str = "SINGLE";
    else { jlog::ls << "Build type requires band_mode to be supplied, but it isn't.\n"; return false; }

  } else mode_str = prm.getString("band_mode").upper();
  
  if (mode_str=="BAND" && master_bs->bandCount()!=fsd.fileCount()) {
    jlog::es << "band_mode is 'band' but the number of specified files does not match the number of bands.\n";
    return false;
  }
  
  if (mode_str=="IPOL" && wl.size()==0) {
    jlog::es << "band_mode is 'ipol' but no wavelengths specified.\n";
    return false;
  }

  if (mode_str=="IPOL" && ((int)wl.size())!=fsd.fileCount()) {
    jlog::es << "band_mode is 'ipol' but the number of specified files does not match the number of wavelengths.\n";
    return false;
  }

  return true;  
} 



// PhaseTool::integratePhaseFunc() [private]
// builds the table

bool PhaseTool::integratePhaseFunc(const PhaseFunc& func) {

    // initialise tabulated phase func to zero
  phase_func_tab->makeAll(SBandData(master_bs->bandCount(), 0));
    // directional structure for convienience
  const DirecStruct& ds(phase_func_tab->direcStruct());

  if (jlog::ls.vbLevel(2)) jlog::ls << "Integration delta solid angle: " << sub_quad_sa << "\n";

  int quad_pairs_done = 0;
  Tool::pct_complete = 0;

    // loop through all in and out index pairs
  for (int in_index=0; in_index<ds.units(); in_index++) {

    SBandData total(master_bs->bandCount(), 0);

    for (int out_index=0; out_index<ds.units(); out_index++) {

        // don't try to integrate forward scattering
      //if (out_index != ds.negatedIndex(in_index)) {
      if (out_index != in_index) {

          // check if already calculated - i.e. for sparse symmetrical representations
	if (phase_func_tab->atIndices(in_index, out_index).allZero()) {

	  QuadIntegrator* qi_in = ds.quadAtIndex(in_index)->createIntegrator(sub_quad_sa); 
	  QuadIntegrator* qi_out = ds.quadAtIndex(out_index)->createIntegrator(sub_quad_sa); 

	  if (!qi_in || !qi_out) {
	    jlog::es << "Failed to create quad integrators\n";
	    return false;
	  }
	  
	  if (jlog::vbLevel(2)) {
	    jlog::ls.printf("quad %d %d : %d %d    \n", in_index, out_index, qi_in->subQuadCount(), qi_out->subQuadCount());
	    jlog::ls.device().cursorUp(1);
	  }

 	  SBandData sd(master_bs->bandCount(), 0);

	    // loop through all sub quad pairs
	  while (!qi_in->atEnd()) {

	    qi_out->reset();
	    while (!qi_out->atEnd()) {

	        // cosine of scattering angle - zero scattering angle means striaght on
	      double cos_angle = Vec3D::dotProduct(qi_in->direction(), qi_out->direction());

                // calculate angle in degrees
	      double ang_rad;
	        // small errors sometimes give out of bound values so trap those explicitly
	      if (cos_angle >= 1.0) ang_rad = 0;
	      else if (cos_angle <= -1.0) ang_rad = M_PI;
	      else ang_rad = acos(cos_angle);
	        // add on the value for this sub quad multiplied by the two sub-quad solid angles 
	      sd += func.spectralValuesFromAngle(ang_rad) * (qi_in->solidAngle() * qi_out->solidAngle());

	        // next out sub-quad
	      qi_out->next();
	    }
	      // next in sub-quad
	    qi_in->next();
	  }

	    // check if another thread cancelled the processing
	  if (processingCanceled()) return false;

	  sd.divide(ds.solidAngle(in_index) * ds.solidAngle(out_index)); 
	  phase_func_tab->setData(in_index, out_index, sd);
	}

  	  // fetch out what was just calculated or the previously calcualted result
	total += phase_func_tab->atIndices(in_index, out_index) * ds.solidAngle(out_index);	

      } // forward scattering test

      quad_pairs_done++;
      Tool::pct_complete = (100.0 * quad_pairs_done) / ds.unitsSquared();

    } // out_index loop

      // the total scattered for one in index should sum to 1 so calculate the difference
    SBandData sd(master_bs->bandCount(), 1);
    sd.subtract(total);
    sd.divide(ds.solidAngle(in_index));
      // now can set the forward scattering quad
    //phase_func_tab->setData(in_index, ds.negatedIndex(in_index), sd);
    phase_func_tab->setData(in_index, in_index, sd);
  }

  if (jlog::vbLevel(2)) jlog::ls.printf("\n");

  return true;
}



void PhaseTool::helpText(JTextStream& str) const {

  str << "\n";
  str << "Phase Function Type Specification\n";
  str << "---------------------------------\n";
  str << "type = isotropic | isotropic_integ | petzold | pure_water | file | ff\n\n";
  
  str << "Output File Specification\n";
  str << "-------------------------\n";
  str << "phase_func_save_fp = <filepath>  (file to save the scalar irradiance to quad-averaged radiance function to)\n\n";

  str << "Output Band And Directional Structure Specification From Files\n";
  str << "--------------------------------------------------------------\n";
  str << "band_spec_fp  = <filepath>  (filepath of output band specification)\n";
  str << "direc_spec_fp = <filepath>  (filepath of output directional structure specification)\n\n";

  str << "Integration Options\n";
  str << "-------------------\n";
  str << "sub_quad_count       = <integer> (approximate number of divisions of the sphere to perform integrations over, default 20000)\n";
  str << "sub_quad_solid_angle = <real>    (solid angle of divisions of the sphere to perform integrations over, default 6.3E-4)\n";
  str << "\n";

  str << "Type 'isotropic'\n";
  str << "----------------\n";
  str << "Isotropic scattering function which is directly calculated.\n";
  str << "(no additional parameters)\n\n";

  str << "Type 'isotropic_integ'\n";
  str << "----------------------\n";
  str << "Isotropic scattering function evaluated by numerical integration - for testing purposes only.\n";
  str << "(no additional parameters)\n\n";

  str << "Type 'petzold'\n";
  str << "--------------\n";
  str << "Petzolds phase function numerically integrated from built-in parameters\n";
  str << "(no additional parameters)\n\n";

  str << "Type 'water'\n";
  str << "------------\n";
  str << "Pure water phase function, numerically integrated.\n";
  str << "(no additional parameters)\n\n";
  
  str << "Type 'ff'\n";
  str << "---------\n";
  str << "Fournier-Forand phase function numerically integrated based on backscatter proportion.\n";
  str << "backscatter_proportion_list = <r1, r2, ... rn>  (backsckatter proportion for each phase function)\n";
  str << "wavelength_list             = <r1, r2, ... rn>  (wavelength of each phase function, optional dependent on band_mode)\n";
  str << "band_mode                   = single | band | ipol  (how to build the function in each output band from the specified functions, optional if only one band)\n";
  str << "\n";

  str << "Type 'file'\n";
  str << "-----------\n";
  str << "Angular phase function specified in one or more files.\n";
  str << "file_list       = <fp1, fp2, ... fpn>  (paths to the phase function files)\n";
  str << "wavelength_list = <r1, r2, ... rn>  (wavelength of each phase function, optional dependent on band_mode)\n";
  str << "band_mode       = single | band | ipol  (how to build the function in each output band from the specified functions, optional if only one band)\n";
  str << "\n";

  Tool::helpTextBandAndDirecSpec(str);
  str << "Output: (no prefix)\n";
  str << "\n";

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

