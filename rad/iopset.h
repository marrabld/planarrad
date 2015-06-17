
// iopset.h

#ifndef _JUDE_RAD_IOPSET_H
#define _JUDE_RAD_IOPSET_H

#include "rad/iopdata.h"
#include "rad/sbanddata.h"
#include "file/fileutils.h"
#include "rad/radutils.h"
#include "rad/report.h"

namespace jude {
namespace rad {



template <typename SD, typename HDS>
class IOPSet : public IOPData<SD, HDS> {

  const BandSpec& master_bs;
    // copied from master_bs for convienence
  int band_count;

  double physical_thickness;
  SD optical_thickness;

    // (c) beam attenuation coefficients (m-1)
  SD atten;
  SD minus_atten;
    // (b) scattering coefficients (m-1)
  SD scat;
    // (a) absorption coefficients (m-1)
  SD absorp;

    // directional scattering expressed as an L->L vsf function
  RadDirecDirecImp<SD>* vsf_func;

    // the VSF function split into four upward/downward components
    // note Suu = Sdd and Sud = Sdu
  RadDirecDirecImp<SD>* trans_zp;
  RadDirecDirecImp<SD>* trans_zm;
  RadDirecDirecImp<SD>* reflec_zp_zm;
  RadDirecDirecImp<SD>* reflec_zm_zp;

    // forward and backward vsf divided by single scattering albedo
  RadDirecDirecImp<SD>* Bplus_func;
  RadDirecDirecImp<SD>* Bminus_func;

  RadDirecDirecDFTSym<SD, HDS> Bplus_dft;
  RadDirecDirecDFTSym<SD, HDS> Bminus_dft;

  matrix<double>* tau_table;
  matrix<double>* rho_table;

  int curr_band;
 
  const matrix<double>* curr_tau;
  const matrix<double>* curr_rho;

  void clearAlloc();

  void calcOpticalThickness() {
    optical_thickness = SBandData(band_count, physical_thickness);
    optical_thickness.multiply(atten);
  }

  void buildTauAndRhoTable();
 
public:

  IOPSet(const HDS& hds, const BandSpec& bs);
  ~IOPSet();

  static bool isHomogeneous() { return true; }

  double physicalThickness() const { return physical_thickness; }

  void setPhysicalThickness(double t) { 
    physical_thickness = t;
    calcOpticalThickness(); 
  }

  const SD& opticalThickness() const { return optical_thickness; }

  double physicalDistanceToOptical(double d, int band) const { return atten[band] * d; }

  const SD& attenuation() const { return atten; }
  const SD& minusAttenuation() const { return minus_atten; }
  void setAttenuation(const SD& sd);

  bool loadFromParameters(const JParser& prm);

  void init() { buildTauAndRhoTable(); }

  void setCurrentBand(int b) { curr_band = b; }

  void setCurrentLMode(int lm) { 
    curr_tau = tau_table + curr_band * IOPData<SD,HDS>::hds.lmNumDFT() + lm;
    curr_rho = rho_table + curr_band * IOPData<SD,HDS>::hds.lmNumDFT() + lm;
  }

  void setCurrOpticalPosition(double) { /* since vertically homegenous nothing to do */ }

  const matrix<double>& currTau() { return *curr_tau; }
  const matrix<double>& currRho() { return *curr_rho; }

  const RadDirecDirecImp<SD>& Bplus() const {
    if (!Bplus_func) { jlog::es << "IOPSet::Bplus() called but vsf or attenuation not defined\n"; internalError(); }
    return *Bplus_func;
  }

  const RadDirecDirecImp<SD>& Bminus() const {
    if (!Bminus_func) { jlog::es << "IOPSet::Bminus() called but vsf or attenuation not defined\n"; internalError(); }
    return *Bminus_func;
  }

  void writeToReport(Report& rep) const;

  /*
  
  bool attenuationDefined() const { return (atten != 0); }
  bool scatteringDefined() const { return (scat != 0); }
  bool absorptionDefined() const { return (absorp != 0); }
  bool vsfDefined() const { return (vsf_func != 0); }
  bool BplusDefined() const { return (Bplus_func != 0); }

  const SD& scattering() const { 
    if (!scat) { jlog::es << "IOPSet::scattering() called but scattering not defined\n"; internalError(); }
    return *scat;
  }

  const SD& absorption() const { 
    if (!absorp) { jlog::es << "IOPSet::absorption() called but absorbtion not defined\n"; internalError(); }
    return *absorp;
  }

  const RadDirecDirecImp<SD>& vsfFunc() const {
    if (!vsf_func) { jlog::es << "IOPSet::vsfFunc() called but vsf not defined"; internalError(); }
    return *vsf_func;
  }

  const RadDirecDirecImp<SD>& Bplus2() const {
    if (!Bplus_func) { jlog::es << "IOPSet::Bplus() called but vsf or attenuation not defined\n"; internalError(); }
    return *Bplus_func;
  }

  const RadDirecDirecImp<SD>& Bminus2() const {
    if (!Bminus_func) { jlog::es << "IOPSet::Bminus() called but vsf or attenuation not defined\n"; internalError(); }
    return *Bminus_func;
  }
  */
};



// IOPSet::(constructor)

template <typename SD, typename HDS>
IOPSet<SD,HDS>::IOPSet(const HDS& hds, const BandSpec& bs) :
  IOPData<SD,HDS>(hds),
  master_bs(bs),
  band_count(bs.bandCount()),
  vsf_func(0),
  trans_zp(0),
  trans_zm(0),
  reflec_zp_zm(0),
  reflec_zm_zp(0),
  Bplus_func(0),
  Bminus_func(0),
  Bplus_dft(hds, bs.bandCount()),
  Bminus_dft(hds, bs.bandCount()),
  tau_table(0),
  rho_table(0) { }



// IOPSet::(destructor)

template <typename SD, typename HDS>
IOPSet<SD,HDS>::~IOPSet() {
  clearAlloc();
}



// IOPSet::clearAlloc() [private]

template <typename SD, typename HDS>
void IOPSet<SD,HDS>::clearAlloc() {  

  delete vsf_func;
  delete trans_zp;
  delete trans_zm;
  delete reflec_zp_zm;
  delete reflec_zm_zp;
  delete Bplus_func;
  delete Bminus_func;
  delete[] tau_table;
  delete[] rho_table;

  vsf_func = 0;
  trans_zp = 0;
  trans_zm = 0;
  reflec_zp_zm = 0;
  reflec_zm_zp = 0;
  Bplus_func = 0;
  Bminus_func = 0;
  tau_table = 0;
  rho_table = 0;
}



// IOPSet::setAttenuation()

template <typename SD, typename HDS>
void IOPSet<SD,HDS>::setAttenuation(const SD& sd) {  
  atten = sd;
  minus_atten = atten;
  minus_atten.negate();
}



// IOPSet::loadFromParameters()

template <typename SD, typename HDS>
bool IOPSet<SD,HDS>::loadFromParameters(const JParser& prm) {
  
  clearAlloc();

    // try to get slab thickness
  if (!prm.getDouble(physical_thickness, "depth")) return false;
    // info
  if (jlog::vbLevel(3)) jlog::ls << "Slab physical thickness (depth): " << physical_thickness << " (m)\n";

    // attempt to load spectral attenuation, false return means specification attempted but something wrong
  if (!RadUtils::loadSBandData(atten, prm, "atten", "attenuation", master_bs)) return false;
    // if got attenuation then display, if not might be able to calculate it from the other inputs
  if (atten.bandCount() != 0 && jlog::vbLevel(3)) jlog::ls << "Attenuation (c) loaded: [ " << atten << " ]\n";

    // attempt to load spectral scattering, false return means specification attempted but something wrong
  if (!RadUtils::loadSBandData(scat, prm, "scat", "scattering", master_bs)) return false;
    // if got scattering display, if not might be able to calculate it from the other inputs
  if (scat.bandCount() != 0 && jlog::vbLevel(3)) jlog::ls << "Scattering (b) loaded: [ " << scat << " ]\n";

    // attempt to load spectral absorption, false return means specification attempted but something wrong
  if (!RadUtils::loadSBandData(absorp, prm, "absorp", "absorption", master_bs)) return false;
    // if got scattering display, if nbot might be able to get later
  if (absorp.bandCount() != 0 && jlog::vbLevel(3)) jlog::ls << "Absorption (a) loaded: [ " << absorp << " ]\n";

    // maybe can calculate attenuation
  if (atten.bandCount()==0 && scat.bandCount()!=0 && absorp.bandCount()!=0) {
    atten = scat + absorp;
    if (jlog::ls.vbLevel(3)) jlog::ls << "Attenuation (c) calculated: [ " << atten << "] \n";

    // or maybe can calculate scattering
  } else if (atten.bandCount()!=0 && scat.bandCount()==0 && absorp.bandCount()!=0) {
    scat = atten - absorp;
    if (jlog::ls.vbLevel(3)) jlog::ls << "Scattering (b) calculated: [ " << scat << " ]\n";
 
    // or maybe can calculate absorption
  } else if (atten.bandCount()!=0 && scat.bandCount()!=0 && absorp.bandCount()==0) {
    absorp = atten - scat;
    if (jlog::ls.vbLevel(3)) jlog::ls << "Absorption (a) calculated: [ " << absorp << " ]\n";
  }

    // now get directional scattering
  FileDesc fd;
  bool is_vsf;

    // directional scattering can be defined either as vsf function
  if (prm.isDefined("vsf_fp")) {
    fd = FileDesc(prm.getString("vsf_fp"), false);
    is_vsf = true;

    // or a phase function
  } else if (prm.isDefined("pf_fp")) {
    fd = FileDesc(prm.getString("pf_fp"), false);
    is_vsf = false;
    
    // one must be specified
  } else {
    jlog::es << "Either a phase function or a VSF must be specified.\n";
    return false;
  }

    // load
  if (jlog::ls.vbLevel(3)) jlog::ls << "Loading: '" << fd.path() << "'\n";    
  vsf_func = FileUtils::createFactoryBuiltFromFile< RadDirecDirecImp<SBandData> >(fd);
    // quit if data types are wrong
  if (!vsf_func->inDataTypeIsScalarIrrad() || !vsf_func->exDataTypeIsMeanRad()) {
    jlog::es << ((is_vsf) ? "VSF" : "Phase function") << " is not of correct units.\n";
    return false;
  }

    // can either be in the right number of bands or only one
  if (vsf_func->bandCount() != band_count && vsf_func->bandCount() != 1) {
    jlog::es <<  ((is_vsf) ? "VSF" : "Phase function") << " is in " << vsf_func->bandCount() << " bands but should be 1 or " << band_count << ".\n";
    return false;
  }

    // if is mono and we need more than one band then expand
  if (vsf_func->bandCount() == 1 && band_count != 1) {
      // info
    if (jlog::vbLevel(3)) jlog::ls << "Expanding " << ((is_vsf) ? "VSF" : "phase function") << " from one band to " << band_count << " bands by repeating the data.\n";
      // do it
    vsf_func->expandBandCount(band_count);
  }

    // if was expressed as a phase function and got scattering then can make it a vsf function
  if (!is_vsf && scat.bandCount()!=0) {
    if (jlog::ls.vbLevel(3)) jlog::ls << "Converting phase function to VSF by multiplying by scattering.\n";
    vsf_func->multiplyAll(scat);

  } else {
    if (jlog::ls.vbLevel(3)) jlog::ls << "Loaded VSF.\n";
      // set scattering if can - FIXME - correct ?
    if (scat.bandCount() == 0) scat = vsf_func->diffuseScatter();
      // warn if scattering is double specified 
    else if (jlog::ls.vbLevel(1)) { 
      jlog::ls << "WARNING: A VSF function has been supplied but scattering is also supplied or ineferable from ";
      jlog::ls << "attenuation and absorption. It is not recommended to double specify a parameter as the specifications ";
      jlog::ls << "may differ and its not clear which is the right one to use.\n";
      jlog::ls << "It is recommended to remove the specification of one of the IOP coefficents or use a phase function.\n";
    }
  }

    // finally check everything was specified or calculable
  if (atten.bandCount()==0 || scat.bandCount()==0 || absorp.bandCount()==0) {
    jlog::es << "IOPs are incompletely specified. Specify any two of attenuation, scattering or absorption. ";
    jlog::es << "Scattering can also be specified via a VSF function, in which case only one other IOP parameter is required.\n";
    return false;
  }
  
    // display basic info of VSF function
  if (jlog::ls.vbLevel(3)) jlog::ls << "VSF diffuse scatter: [ " << vsf_func->diffuseScatter() << " ]\n";
  
  //RadUtils::displayInfo(*vsf_func, 0, jlog::ls);

    // convert to L -> L function
    // starts as units of out_radiance / in_scalar_irrad so multiply by in solid angle means divide in_scalar_irrad by sa
  vsf_func->multiplyAllByInSA();

    // zero size return means not specified at all in this case thats an error
  if (atten.bandCount() == 0) { jlog::es << "Attenuation must be specified.\n"; return false; }
    // negation of attenuation is useful in the solution integration
  minus_atten = atten;
  minus_atten.negate();
    // now can calculate optical thickness at each band
  calcOpticalThickness();
   
  if (jlog::vbLevel(3)) jlog::ls << "Optical thickness calculated: [ " << optical_thickness << " ]\n";

    // create half functions
  trans_zp = vsf_func->duplicateHalfStructure();
  trans_zm = vsf_func->duplicateHalfStructure();
  reflec_zp_zm = vsf_func->duplicateHalfStructure();
  reflec_zm_zp = vsf_func->duplicateHalfStructure();

  if (jlog::ls.vbLevel(8)) {
    jlog::ls << "Full phase func:\n";
    vsf_func->display(jlog::ls);
  }

    // split VSF into hemisphere functions
    // note that for most normal VSFs Suu == Sdd and Sdu == Sud
    // Suu and Sdd are the forward scatter
    // Sud and Sdu are the back scatter
  vsf_func->splitNegate(reflec_zm_zp, trans_zm, trans_zp, reflec_zp_zm);

  //trans_zp->makeAll(SBandData(band_count,0));
  //reflec_zp_zm->makeAll(SBandData(band_count,0));
  //reflec_zp_zm->reverseExitantY();

  if (jlog::ls.vbLevel(8)) {
    jlog::ls << "Forward (trans_zp) Bplus:\n";
    trans_zp->display(jlog::ls);
    jlog::ls << "Back (reflec_zp_zm) Bminus:\n";
    reflec_zp_zm->display(jlog::ls);
  }

    // Bplus is a scattering transmission
  Bplus_func = trans_zp->duplicate();
    // Bminus is scattering reflectance
  Bminus_func = reflec_zp_zm->duplicate();
    // convert vsf to phase func times single scattering albedo as used by plane slab
  Bplus_func->divideAll(atten);
  Bminus_func->divideAll(atten);
 
 
 
  /*
  RadDirecDirecImp<SD>* Bminus_conv = Bminus_func->duplicateStructureAndDataType();
 
  const HDS& ds(IOPData<SD,HDS>::direcStruct());

  for (int in_index=0; in_index<IOPData<SD,HDS>::direcStruct().units(); in_index++) {
    for (int out_index=0; out_index<IOPData<SD,HDS>::direcStruct().units(); out_index++) {

        // the directions these indices represent in the hemisphere
      Vec3D dv_in(IOPData<SD,HDS>::direcStruct().indexToPointDirec(in_index));
      Vec3D dv_out(ds.indexToPointDirec(out_index));

        // undo the rotation about x
      Vec3D dv_out_trans(dv_out.rotate180AboutX());
        // convert to to a negation
      dv_out_trans.negate();
        // the out index this values should be at
      int out_index_trans = ds.direcToIndex(dv_out_trans);

      Bminus_conv->setData(out_index_trans, in_index, Bminus_func->atIndices(out_index, in_index));  
    }
  }

  */
    // build dft representations of phase func times single scattering albedo
    // these are all that is required to calculate tau and rho for the slab integration
  //Bplus_dft.copy(*trans_zp);
  //Bminus_dft.copy(*reflec_zp_zm);
  //Bplus_dft.divide(atten);
  //Bminus_dft.divide(atten);
 
  Bplus_dft.copy(*Bplus_func);
  Bminus_dft.copy(*Bminus_func);
 
  if (jlog::ls.vbLevel(9)) {
    jlog::ls << "Bplus:\n";
    Bplus_dft.display(jlog::ls);
    jlog::ls << "Bminus:\n";
    Bminus_dft.display(jlog::ls);
  }

  //exit(0);
  //delete Bminus_conv;

  return true;
}



// IOPSet::buildTauAndRho()
// L&W pg. 400

template <typename SD, typename HDS> 
void IOPSet<SD,HDS>::buildTauAndRhoTable() {

  const HDS& hds(IOPData<SD,HDS>::halfDirecStruct());

  delete[] tau_table;
  delete[] rho_table;

  tau_table = new matrix<double>[band_count * hds.lmNumDFT()];
  rho_table = new matrix<double>[band_count * hds.lmNumDFT()];

  for (int i=0; i<band_count * hds.lmNumDFT(); i++) {
    tau_table[i].resize(hds.rowsDFT(), hds.rowsDFT());
    rho_table[i].resize(hds.rowsDFT(), hds.rowsDFT()); 
  }

  matrix<double>* curr_tau(tau_table);
  matrix<double>* curr_rho(rho_table);
  
    // loop through all bands
  for (int band=0; band<band_count; band++) {

      // loop through all l-modes
    for (int lm=0; lm<hds.lmNumDFT(); lm++) {

      this->calcTauAndRho(*curr_tau, *curr_rho, Bplus_dft, Bminus_dft, band, lm);

      if (jlog::ls.vbLevel(6)) {
	jlog::ls << "Band: " << band << " lm: " << lm << "\n";
	jlog::ls << "tau:\n" << (*curr_tau) << "\n";
	jlog::ls << "rho:\n" << (*curr_rho) << "\n";
      }

      curr_tau++;
      curr_rho++;
    }
  }
}



template <typename SD, typename HDS> 
void IOPSet<SD,HDS>::writeToReport(Report& rep) const {

  rep.writeBandHeader<SD>("IOPs, (m-1)", "var", "z", band_count);
  rep.writeSpectralData<SD>("a", absorp);
  rep.writeSpectralData<SD>("b", scat);
  rep.writeSpectralData<SD>("c", atten);

  rep.stream() << "# Physical depth is " << physical_thickness << " m\n";
  rep.writeBandHeader<SD>("Optical depth, zeta, (unitless)", "var", "-", band_count);
  rep.writeSpectralData<SD>("zeta", optical_thickness);
}



} } // namespace jude::rad

#endif // _JUDE_RAD_IOPSET_H


