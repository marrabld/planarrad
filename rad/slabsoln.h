
// slabsoln.h

#ifndef _JUDETOOLS_SLABSOLN_H
#define _JUDETOOLS_SLABSOLN_H

#include "rad/raddirecimp.h"
#include "rad/bandspec.h"
#include "base/jlog.h"
#include "base/jerror.h"
#include "rad/report.h"

using namespace jude::ds;

namespace jude {
namespace rad {


template <typename SD, typename DS>
class SlabSoln {

  DS full_ds;
  int band_count;

    // air-side radiance distribution
  RadDirecTable<SD, DS>* L_a;
    // water-side radiance distribution
  RadDirecTable<SD, DS>* L_w;
    // bottom of water column radiance distribution
  RadDirecTable<SD, DS>* L_b;
    // air-water interface transmission
  RadDirecTable<SD, DS>* L_it;
    // air-water interface reflection
  RadDirecTable<SD, DS>* L_ir;

  RadDirecTable<SD, DS>** L_array;
  RadDirecTable<SD, DS>** dL_dz_array;
  double depth;
  double* sample_depth;
  int array_size;

  SD sd_rrs;   // Lu(w) / Ed(w) Lee. 1999
  SD sd_Rrs;   // Lu(it) / Ed(a) Mobley L&W page 70 "returned through the surface".
  SD sd_Rtot;  // Lu(a) / Ed(a)
  SD sd_R0m;   // Eu(w) / Ed(w)
  SD sd_R0p;   // Eu(a) / Ed(a)

  RadDirecTable<SD, typename DS::half_type>* Lu_a;
  RadDirecTable<SD, typename DS::half_type>* Ld_a;
  RadDirecTable<SD, typename DS::half_type>* Lu_w;
  RadDirecTable<SD, typename DS::half_type>* Ld_w;
  RadDirecTable<SD, typename DS::half_type>* Lu_b;
  RadDirecTable<SD, typename DS::half_type>* Ld_b;
  RadDirecTable<SD, typename DS::half_type>* Lu_it;
  RadDirecTable<SD, typename DS::half_type>* Ld_it;
  RadDirecTable<SD, typename DS::half_type>* Lu_ir;
  RadDirecTable<SD, typename DS::half_type>* Ld_ir;

  RadDirecTable<SD, typename DS::half_type>** Ld_array;
  RadDirecTable<SD, typename DS::half_type>** Lu_array;
  RadDirecTable<SD, typename DS::half_type>** dLd_dz_array;
  RadDirecTable<SD, typename DS::half_type>** dLu_dz_array;

    // the number of precision digits to append on the depth
  int depth_code_prec;
  
  void clearArrayAlloc();
  void clearSplitAlloc();

public:

  SlabSoln(const DS& full_ds, int band_count);
  ~SlabSoln();

  void setDepth(double d) { depth = d; }
  void setDepthCodePrecision(int d) { depth_code_prec = d; }

  const RadDirecTable<SD,DS>* LAir() const { return L_a; }
  const RadDirecTable<SD,DS>* LWater() const { return L_w; }
  const RadDirecTable<SD,DS>* LBottom() const { return L_b; }
  const RadDirecTable<SD,DS>* LAirWaterTransmitted() const { return L_it; }
  const RadDirecTable<SD,DS>* LAirWaterReflected() const { return L_ir; }
  const RadDirecTable<SD,DS>* LSample(int i) const { return L_array[i]; }
  const RadDirecTable<SD,DS>* dLdzSample(int i) const { return dL_dz_array[i]; }

  const SD KdSample(int i) const { 
    SBandData sd(dLd_dz_array[i]->planarIrradiance());
    sd.negate();
    sd.divide(Ld_array[i]->planarIrradiance());
    return sd;
  }

  const SD KuSample(int i) const { 
    SBandData sd(dLu_dz_array[i]->planarIrradiance());
    sd.negate();
    sd.divide(Lu_array[i]->planarIrradiance());
    return sd;
  }

  const SD KodSample(int i) const { 
    SBandData sd(dLd_dz_array[i]->scalarIrradiance());
    sd.negate();
    sd.divide(Ld_array[i]->scalarIrradiance());
    return sd;
  }

  const SD KouSample(int i) const { 
    SBandData sd(dLu_dz_array[i]->scalarIrradiance());
    sd.negate();
    sd.divide(Lu_array[i]->scalarIrradiance());
    return sd;
  }

  const SD KoSample(int i) const { 
    SBandData sd(dL_dz_array[i]->scalarIrradiance());
    sd.negate();
    sd.divide(L_array[i]->scalarIrradiance());
    return sd;
  }

  const SD KLuSample(int i) const { 
    SBandData sd(dL_dz_array[i]->atZMinus());
    sd.negate();
    sd.divide(L_array[i]->atZMinus());
    return sd;
  }

  double sampleDepth(int i) const { return sample_depth[i]; }

  void setLAir(const RadDirecTable<SD, DS>& L) { 
    if (!L_a) L_a = new RadDirecTable<SD, DS>(L); 
    else L_a->copy(L);
  }

  void setLWater(const RadDirecTable<SD, DS>& L) { 
    if (!L_w) L_w = new RadDirecTable<SD, DS>(L); 
    else L_w->copy(L);
  }

  void setLBottom(const RadDirecTable<SD, DS>& L) { 
    if (!L_b) L_b = new RadDirecTable<SD, DS>(L); 
    else L_b->copy(L);
  }

  void setLAirWaterTransmitted(const RadDirecTable<SD, DS>& L) { 
    if (!L_it) L_it = new RadDirecTable<SD, DS>(L); 
    else L_it->copy(L);
  }

  void setLAirWaterReflected(const RadDirecTable<SD, DS>& L) { 
    if (!L_ir) L_ir = new RadDirecTable<SD, DS>(L); 
    else L_ir->copy(L);
  }

  void setLSample(int i, double depth, const RadDirecTable<SD, DS>& L) { 
    if (!L_array[i]) L_array[i] = new RadDirecTable<SD, DS>(L); 
    else L_array[i]->copy(L);
    sample_depth[i] = depth;
  }

  void setDeltaLSample(int i, const RadDirecTable<SD, DS>& L) { 
    if (!dL_dz_array[i]) dL_dz_array[i] = new RadDirecTable<SD, DS>(L); 
    else dL_dz_array[i]->copy(L);
  }

  void setLAir(const RadDirecImp<SD>* zp, const RadDirecImp<SD>* zm) { 
    if (!L_a) L_a = new RadDirecTable<SD, DS>(full_ds, band_count, RadDirec::MEAN_RAD); 
    L_a->mergeNegate(zp, zm);
  }

  void setLWater(const RadDirecImp<SD>* zp, const RadDirecImp<SD>* zm) { 
    if (!L_w) L_w = new RadDirecTable<SD, DS>(full_ds, band_count, RadDirec::MEAN_RAD); 
    L_w->mergeNegate(zp, zm);
  }

  void setLBottom(const RadDirecImp<SD>* zp, const RadDirecImp<SD>* zm) { 
    if (!L_b) L_b = new RadDirecTable<SD, DS>(full_ds, band_count, RadDirec::MEAN_RAD); 
    L_b->mergeNegate(zp, zm);
  }

  void setLAirWaterTransmitted(const RadDirecImp<SD>* zp, const RadDirecImp<SD>* zm) { 
    if (!L_it) L_it = new RadDirecTable<SD, DS>(full_ds, band_count, RadDirec::MEAN_RAD); 
    L_it->mergeNegate(zp, zm);
  }

  void setLAirWaterReflected(const RadDirecImp<SD>* zp, const RadDirecImp<SD>* zm) { 
    if (!L_ir) L_ir = new RadDirecTable<SD, DS>(full_ds, band_count, RadDirec::MEAN_RAD); 
    L_ir->mergeNegate(zp, zm);
  }

  void setLSample(int i, double depth, const RadDirecImp<SD>* zp, const RadDirecImp<SD>* zm) {
    //if (!jassert(i < array_size)) { jlog::es << "At " __FILE__ << ":" << __LINE__ << " i >= array_size " << i << " " << array_size << ".\n"; } 
    if (!L_array[i]) L_array[i] = new RadDirecTable<SD, DS>(full_ds, band_count, RadDirec::MEAN_RAD); 
    L_array[i]->mergeNegate(zp, zm);
    sample_depth[i] = depth;
  }

  void setDeltaLSample(int i, const RadDirecImp<SD>* zp, const RadDirecImp<SD>* zm) { 
    //if (!jassert(i < array_size)) { jlog::es << "At " __FILE__ << ":" << __LINE__ << " i >= array_size.\n"; internalError(); } 
    if (!dL_dz_array[i]) dL_dz_array[i] = new RadDirecTable<SD, DS>(full_ds, band_count, RadDirec::MEAN_RAD); 
    dL_dz_array[i]->mergeNegate(zp, zm);
  }

  void setSamplePointCount(int i);

  void calcOutputs();
  void saveOutputs(const JParser& prm, const BandSpec& master_bs, const JString& notes_text) const;

  void writeToReport(Report& rep) const;
};



template <typename SD, typename DS>
SlabSoln<SD,DS>::SlabSoln(const DS& full_ds, int band_count) :
  full_ds(full_ds),
  band_count(band_count),
  L_a(0),
  L_w(0),
  L_b(0),
  L_it(0),
  L_ir(0),
  L_array(0),
  dL_dz_array(0),
  depth(0),
  sample_depth(0),
  array_size(0),
  Lu_a(0),
  Ld_a(0),
  Lu_w(0),
  Ld_w(0),
  Lu_b(0),
  Ld_b(0),
  Lu_it(0),
  Ld_it(0),
  Lu_ir(0),
  Ld_ir(0),
  Ld_array(0),
  Lu_array(0),
  dLd_dz_array(0),
  dLu_dz_array(0),
  depth_code_prec(1) { }



template <typename SD, typename DS>
SlabSoln<SD,DS>::~SlabSoln() {
  delete L_a;
  delete L_w;
  delete L_b;
  delete L_it;
  delete L_ir;
  clearArrayAlloc();
  clearSplitAlloc();
}



template <typename SD, typename DS>
void SlabSoln<SD,DS>::clearArrayAlloc() {

  for (int i=0; i<array_size; i++) {
    delete L_array[i];
    delete Ld_array[i];
    delete Lu_array[i];
    delete dL_dz_array[i];
    delete dLd_dz_array[i];
    delete dLu_dz_array[i];
  }
  delete[] L_array;
  delete[] Ld_array;
  delete[] Lu_array;
  delete[] dL_dz_array;
  delete[] dLd_dz_array;
  delete[] dLu_dz_array;
  delete[] sample_depth;

  L_array = 0;
  Ld_array = 0;
  Lu_array = 0;
  dL_dz_array = 0;
  dLd_dz_array = 0;
  dLu_dz_array = 0;
  sample_depth = 0;
  array_size = 0;
}



template <typename SD, typename DS>
void SlabSoln<SD,DS>::clearSplitAlloc() {

  delete Lu_a;
  delete Ld_a;
  delete Lu_w;
  delete Ld_w;
  delete Lu_b;
  delete Ld_b;
  delete Lu_it;
  delete Ld_it;
  delete Lu_ir;
  delete Ld_ir;

  Lu_a = 0;
  Ld_a = 0;
  Lu_w = 0;
  Ld_w = 0;
  Lu_b = 0;
  Ld_b = 0;
  Lu_it = 0;
  Ld_it = 0;
  Lu_ir = 0;
  Ld_ir = 0;

  for (int i=0; i<array_size; i++) {
    delete Ld_array[i];
    delete Lu_array[i];
    delete dLd_dz_array[i];
    delete dLu_dz_array[i];
    Ld_array[i] = 0;
    Lu_array[i] = 0;
    dLd_dz_array[i] = 0;
    dLu_dz_array[i] = 0;
  }
}



template <typename SD, typename DS>
void SlabSoln<SD,DS>::setSamplePointCount(int sz) {

  clearArrayAlloc();

  array_size = sz;
  jlog::ls << "array size " << array_size << "\n";
  L_array = new RadDirecTable<SD, DS>*[array_size];
  Ld_array = new RadDirecTable<SD, typename DS::half_type>*[array_size];
  Lu_array = new RadDirecTable<SD, typename DS::half_type>*[array_size];

  dL_dz_array = new RadDirecTable<SD, DS>*[array_size];
  dLd_dz_array = new RadDirecTable<SD, typename DS::half_type>*[array_size];
  dLu_dz_array = new RadDirecTable<SD, typename DS::half_type>*[array_size];

  sample_depth = new double[array_size];

    // initialise to zero
  for (int i=0; i<array_size; i++) {
    L_array[i] = 0;
    Ld_array[i] = 0;
    Lu_array[i] = 0;
    dL_dz_array[i] = 0;
    dLd_dz_array[i] = 0;
    dLu_dz_array[i] = 0;
  }
}



template <typename SD, typename DS>
void SlabSoln<SD,DS>::calcOutputs() {
 
  clearSplitAlloc();
 
    // create 180 degree split versions of each distribution
  if (L_a) L_a->splitCreateRotateAboutX(Ld_a, Lu_a);
  if (L_w) L_w->splitCreateRotateAboutX(Ld_w, Lu_w);
  if (L_b) L_b->splitCreateRotateAboutX(Ld_b, Lu_b);
  if (L_it) L_it->splitCreateRotateAboutX(Ld_it, Lu_it);
  if (L_ir) L_ir->splitCreateRotateAboutX(Ld_ir, Lu_ir);
 
  for (int i=0; i<array_size; i++) {
    if (L_array[i]) L_array[i]->splitCreateRotateAboutX(Ld_array[i], Lu_array[i]);
    if (dL_dz_array[i]) dL_dz_array[i]->splitCreateRotateAboutX(dLd_dz_array[i], dLu_dz_array[i]);
  }

  SBandData sd_ed_a;

    // upward radiance including water surface reflection
  sd_Rtot = L_a->atZMinus();
    // upward radiance above water not including water surface reflection
  sd_Rrs = L_it->atZMinus(); 
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
  sd_rrs = L_w->atZMinus();
    // downward plane irradiance below water surface
  SBandData sd_ed_w = Ld_w->planarIrradiance();
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



// SlabSoln::saveOutputs() [private]

template <typename SD, typename DS>
void SlabSoln<SD,DS>::saveOutputs(const JParser& prm, const BandSpec& master_bs, const JString& notes_text) const {

    // directional data table files for quad-averaged radiance
  if (L_b) RadUtils::saveRadiance("L_b", "", *L_b, prm, notes_text);
  if (L_a) RadUtils::saveRadiance("L_a", "", *L_a, prm, notes_text);
  if (L_w) RadUtils::saveRadiance("L_w", "", *L_w, prm, notes_text);
  if (L_it) RadUtils::saveRadiance("L_it", "", *L_it, prm, notes_text);
  if (L_ir) RadUtils::saveRadiance("L_ir", "", *L_ir, prm, notes_text);

    // directional data table files for quad-averaged radiance
  if (Ld_b) RadUtils::saveRadiance("Ld_b", "", *Ld_b, prm, notes_text);
  if (Lu_b) RadUtils::saveRadiance("Lu_b", "", *Lu_b, prm, notes_text);
  if (Ld_a) RadUtils::saveRadiance("Ld_a", "", *Ld_a, prm, notes_text);
  if (Lu_a) RadUtils::saveRadiance("Lu_a", "", *Lu_a, prm, notes_text);
  if (Ld_w) RadUtils::saveRadiance("Ld_w", "", *Ld_w, prm, notes_text);
  if (Lu_w) RadUtils::saveRadiance("Lu_w", "", *Lu_w, prm, notes_text);
  if (Ld_it) RadUtils::saveRadiance("Ld_it", "", *Ld_it, prm, notes_text);
  if (Lu_it) RadUtils::saveRadiance("Lu_it", "", *Lu_it, prm, notes_text);
  if (Ld_ir) RadUtils::saveRadiance("Ld_ir", "", *Ld_ir, prm, notes_text);
  if (Lu_ir) RadUtils::saveRadiance("Lu_ir", "", *Lu_ir, prm, notes_text);

    // spectral data text files for planar irradiances
  if (Ld_b) RadUtils::savePlanarIrradiance("Ed_b", "", *Ld_b, prm);
  if (Lu_b) RadUtils::savePlanarIrradiance("Eu_b", "", *Lu_b, prm);
  if (Ld_a) RadUtils::savePlanarIrradiance("Ed_a", "", *Ld_a, prm);
  if (Lu_a) RadUtils::savePlanarIrradiance("Eu_a", "", *Lu_a, prm);
  if (Ld_w) RadUtils::savePlanarIrradiance("Ed_w", "", *Ld_w, prm);
  if (Lu_w) RadUtils::savePlanarIrradiance("Eu_w", "", *Lu_w, prm);
  if (Ld_it) RadUtils::savePlanarIrradiance("Ed_it", "", *Ld_it, prm);
  if (Lu_it) RadUtils::savePlanarIrradiance("Eu_it", "", *Lu_it, prm);
  if (Ld_ir) RadUtils::savePlanarIrradiance("Ed_ir", "", *Ld_ir, prm);
  if (Lu_ir) RadUtils::savePlanarIrradiance("Eu_ir", "", *Lu_ir, prm);

    // spectral data text files for scalar irradiances
  if (Ld_b) RadUtils::saveScalarIrradiance("Eod_b", "", *Ld_b, prm);
  if (Lu_b) RadUtils::saveScalarIrradiance("Eou_b", "", *Lu_b, prm);
  if (Ld_a) RadUtils::saveScalarIrradiance("Eod_a", "", *Ld_a, prm);
  if (Lu_a) RadUtils::saveScalarIrradiance("Eou_a", "", *Lu_a, prm);
  if (Ld_w) RadUtils::saveScalarIrradiance("Eod_w", "", *Ld_w, prm);
  if (Lu_w) RadUtils::saveScalarIrradiance("Eou_w", "", *Lu_w, prm);
  if (Ld_it) RadUtils::saveScalarIrradiance("Eod_it", "", *Ld_it, prm);
  if (Lu_it) RadUtils::saveScalarIrradiance("Eou_it", "", *Lu_it, prm);
  if (Ld_ir) RadUtils::saveScalarIrradiance("Eod_ir", "", *Ld_ir, prm);
  if (Lu_ir) RadUtils::saveScalarIrradiance("Eou_ir", "", *Lu_ir, prm);
	
    // hemispherical projection image files
  if (Ld_b) RadUtils::saveHemisphereImage("Ld_b", "", *Ld_b, master_bs, prm);
  if (Lu_b) RadUtils::saveHemisphereImage("Lu_b", "", *Lu_b, master_bs, prm);
  if (Ld_a) RadUtils::saveHemisphereImage("Ld_a", "", *Ld_a, master_bs, prm);
  if (Lu_a) RadUtils::saveHemisphereImage("Lu_a", "", *Lu_a, master_bs, prm);
  if (Ld_w) RadUtils::saveHemisphereImage("Ld_w", "", *Ld_w, master_bs, prm);
  if (Lu_w) RadUtils::saveHemisphereImage("Lu_w", "", *Lu_w, master_bs, prm);
  if (Ld_it) RadUtils::saveHemisphereImage("Ld_it", "", *Ld_it, master_bs, prm);
  if (Lu_it) RadUtils::saveHemisphereImage("Lu_it", "", *Lu_it, master_bs, prm);
  if (Ld_ir) RadUtils::saveHemisphereImage("Ld_ir", "", *Ld_ir, master_bs, prm);
  if (Lu_ir) RadUtils::saveHemisphereImage("Lu_ir", "", *Lu_ir, master_bs, prm);
 
  for (int i=0; i<array_size; i++) {

      // what to append on the end of the filenames
    JString depth_postfix(JString("_")+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");

    if (L_array[i]) RadUtils::saveRadiance("L_sample", depth_postfix, *L_array[i], prm, notes_text); 
    if (Ld_array[i]) RadUtils::saveRadiance("Ld_sample", depth_postfix, *Ld_array[i], prm, notes_text);
    if (Lu_array[i]) RadUtils::saveRadiance("Lu_sample", depth_postfix, *Lu_array[i], prm, notes_text);

    if (Ld_array[i]) RadUtils::saveHemisphereImage("Ld_sample", depth_postfix, *Ld_array[i], master_bs, prm);
    if (Lu_array[i]) RadUtils::saveHemisphereImage("Lu_sample", depth_postfix, *Lu_array[i], master_bs, prm);
  }
}



// SlabSoln::write()

template <typename SD, typename DS>
void SlabSoln<SD,DS>::writeToReport(Report& rep) const {

    // Ed header
  rep.writeBandHeader<SD>("Downwelling planar irradiance (Wm-2nm-1)", "code", "z", band_count);
    // Ed in air and water
  if (Ld_a) rep.writeSpectralData<SD>("Ed_a", 0, Ld_a->planarIrradiance());
  if (Ld_w) rep.writeSpectralData<SD>("Ed_w", 0, Ld_w->planarIrradiance());
  if (Ld_it) rep.writeSpectralData<SD>("Ed_it", 0, Ld_it->planarIrradiance());
  if (Ld_ir) rep.writeSpectralData<SD>("Ed_ir", 0, Ld_ir->planarIrradiance());
    // Ed at sample points
  for (int i=0; i<array_size; i++) {
    JString code("Ed_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    if (Ld_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], Ld_array[i]->planarIrradiance());
  }
    // Ed at the bottom
  if (Ld_b) rep.writeSpectralData<SD>("Ed_b", depth, Ld_b->planarIrradiance());
 
    // Eu header
  rep.writeBandHeader<SD>("Upwelling planar irradiance (Wm-2nm-1)", "code", "z", band_count);
    // Eu in air and water
  if (Lu_a) rep.writeSpectralData<SD>("Eu_a", 0, Lu_a->planarIrradiance());
  if (Lu_w) rep.writeSpectralData<SD>("Eu_w", 0, Lu_w->planarIrradiance());
  if (Lu_it) rep.writeSpectralData<SD>("Eu_it", 0, Lu_it->planarIrradiance());
  if (Lu_ir) rep.writeSpectralData<SD>("Eu_ir", 0, Lu_ir->planarIrradiance());
    // Eu at sample points
  for (int i=0; i<array_size; i++) {
    JString code("Eu_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    if (Lu_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], Lu_array[i]->planarIrradiance());
  }
    // Eu at the bottom
  if (Lu_b) rep.writeSpectralData<SD>("Eu_b", depth, Lu_b->planarIrradiance());
 
    // Eo header
  rep.writeBandHeader<SD>("Total scalar irradiance (Wm-2nm-1)", "code", "z", band_count);
    // Eo in air and water
  if (L_a) rep.writeSpectralData<SD>("Eo_a", 0, L_a->scalarIrradiance());
  if (L_w) rep.writeSpectralData<SD>("Eo_w", 0, L_w->scalarIrradiance());
    // Eo at sample points
  for (int i=0; i<array_size; i++) {
    JString code("Eo_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    if (L_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], L_array[i]->scalarIrradiance());
  }
    // Eo at the bottom
  if (L_b) rep.writeSpectralData<SD>("Eo_b", depth, L_b->scalarIrradiance());
 
    // Eod header
  rep.writeBandHeader<SD>("Downwelling scalar irradiance (Wm-2nm-1)", "code", "z", band_count);
    // Eod in air and water
  if (Ld_a) rep.writeSpectralData<SD>("Eod_a", 0, Ld_a->scalarIrradiance());
  if (Ld_w) rep.writeSpectralData<SD>("Eod_w", 0, Ld_w->scalarIrradiance());
  if (Ld_it) rep.writeSpectralData<SD>("Eod_it", 0, Ld_it->scalarIrradiance());
  if (Ld_ir) rep.writeSpectralData<SD>("Eod_ir", 0, Ld_ir->scalarIrradiance());
    // Eod at sample points
  for (int i=0; i<array_size; i++) {
    JString code("Eod_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    if (Ld_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], Ld_array[i]->scalarIrradiance());
  }
    // Eod at the bottom
  if (Ld_b) rep.writeSpectralData<SD>("Eod_b", depth, Ld_b->scalarIrradiance());
 
    // Eou header
  rep.writeBandHeader<SD>("Upwelling scalar irradiance, (Wm-2nm-1)", "code", "z", band_count);
    // Eou in air and water
  if (Lu_a) rep.writeSpectralData<SD>("Eou_a", 0, Lu_a->scalarIrradiance());
  if (Lu_w) rep.writeSpectralData<SD>("Eou_w", 0, Lu_w->scalarIrradiance());
  if (Lu_it) rep.writeSpectralData<SD>("Eou_it", 0, Lu_it->scalarIrradiance());
  if (Lu_ir) rep.writeSpectralData<SD>("Eou_ir", 0, Lu_ir->scalarIrradiance());
    // Eou at sample points
  for (int i=0; i<array_size; i++) {
    JString code("Eou_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    if (Lu_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], Lu_array[i]->scalarIrradiance());
  }
    // Eou at the bottom
  if (Lu_b) rep.writeSpectralData<SD>("Eou_b", depth, Lu_b->scalarIrradiance());

    // Lu header
  rep.writeBandHeader<SD>("Upward radiance, (Wm-2sr-1nm-1)", "code", "z", band_count);
    // Lu in air and water
  if (Lu_a) rep.writeSpectralData<SD>("Lu_a", 0, Lu_a->atZPlus());
  if (Lu_w) rep.writeSpectralData<SD>("Lu_w", 0, Lu_w->atZPlus());
  if (Lu_it) rep.writeSpectralData<SD>("Lu_it", 0, Lu_it->atZPlus());
  if (Lu_ir) rep.writeSpectralData<SD>("Lu_ir", 0, Lu_ir->atZPlus());
    // Lu at sample points
  for (int i=0; i<array_size; i++) {
    JString code("Lu_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    if (Lu_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], Lu_array[i]->atZPlus());
  }
    // Lu at the bottom
  if (Lu_b) rep.writeSpectralData<SD>("Lu_b", depth, Lu_b->atZPlus());

    // Ld header
  rep.writeBandHeader<SD>("Downward radiance, (Wm-2sr-1nm-1)", "code", "z", band_count);
    // Lu in air and water
  if (Ld_a) rep.writeSpectralData<SD>("Ld_a", 0, Ld_a->atZPlus());
  if (Ld_w) rep.writeSpectralData<SD>("Ld_w", 0, Ld_w->atZPlus());
  if (Ld_it) rep.writeSpectralData<SD>("Ld_it", 0, Ld_it->atZPlus());
  if (Ld_ir) rep.writeSpectralData<SD>("Ld_ir", 0, Ld_ir->atZPlus());
    // Lu at sample points
  for (int i=0; i<array_size; i++) {
    JString code("Ld_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    if (Ld_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], Ld_array[i]->atZPlus());
  }
    // Ld at the bottom
  if (Ld_b) rep.writeSpectralData<SD>("Ld_b", depth, Ld_b->atZPlus());

    // R0p
  rep.writeBandHeader<SD>("Above surface diffuse reflectance, R0p = Eu_a / Ed_a, (unitless)", "code", "-", band_count);
  if (sd_R0p.bandCount() > 0) rep.writeSpectralData<SD>("R0p", sd_R0p);
    // R0m
  rep.writeBandHeader<SD>("Sub-surface diffuse reflectance, R0m = Eu_w / Ed_w, (unitless)", "code", "-", band_count);
  if (sd_R0m.bandCount() > 0) rep.writeSpectralData<SD>("R0m", sd_R0m);
    // Rrs
  rep.writeBandHeader<SD>("Remote sensing reflectance, Rrs = Lu_it / Ed_a, (sr-1)", "code", "-", band_count);
  if (sd_Rrs.bandCount() > 0) rep.writeSpectralData<SD>("Rrs", sd_Rrs);
    // Rtot
  rep.writeBandHeader<SD>("Total remote sensing reflectance, Rtot = Lu_a / Ed_a, (sr-1)", "code", "-", band_count);
  if (sd_Rtot.bandCount() > 0) rep.writeSpectralData<SD>("Rtot", sd_Rtot);
    // rrs
  rep.writeBandHeader<SD>("Sub-surface remote sensing reflectance, rrs = Lu_w / Ed_w, (sr-1)", "code", "-", band_count);
  if (sd_rrs.bandCount() > 0) rep.writeSpectralData<SD>("rrs", sd_rrs);

    // mean cosines from Lu
  rep.writeBandHeader<SD>("Mean cosine from upward radiances, (unitless)", "code", "z", band_count);
    // from Lu in air and water
  if (Lu_a) rep.writeSpectralData<SD>("mu_bar(Lu_a)", 0, RadUtils::meanCosine(*Lu_a));
  if (Lu_w) rep.writeSpectralData<SD>("mu_bar(Lu_w)", 0, RadUtils::meanCosine(*Lu_w));
  if (Lu_it) rep.writeSpectralData<SD>("mu_bar(Lu_it)", 0, RadUtils::meanCosine(*Lu_it));
  if (Lu_ir) rep.writeSpectralData<SD>("mu_bar(Lu_ir)", 0, RadUtils::meanCosine(*Lu_ir));
    // Lu at sample points
  for (int i=0; i<array_size; i++) {
    JString code("mu_bar(Lu_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m)");
    if (Lu_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], RadUtils::meanCosine(*Lu_array[i]));
  }
    // Lu at the bottom
  if (Lu_b) rep.writeSpectralData<SD>("mu_bar(Lu_b)", depth, RadUtils::meanCosine(*Lu_b));

    // mean cosines from Ld
  rep.writeBandHeader<SD>("Mean cosine from downward radiances, (unitless)", "code", "z", band_count);
    // from Ld in air and water
  if (Ld_a) rep.writeSpectralData<SD>("mu_bar(Ld_a)", 0, RadUtils::meanCosine(*Ld_a));
  if (Ld_w) rep.writeSpectralData<SD>("mu_bar(Ld_w)", 0, RadUtils::meanCosine(*Ld_w));
  if (Ld_it) rep.writeSpectralData<SD>("mu_bar(Ld_it)", 0, RadUtils::meanCosine(*Ld_it));
  if (Ld_ir) rep.writeSpectralData<SD>("mu_bar(Ld_ir)", 0, RadUtils::meanCosine(*Ld_ir));
    // Lu at sample points
  for (int i=0; i<array_size; i++) {
    JString code("mu_bar(Ld_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m)");
    if (Ld_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], RadUtils::meanCosine(*Ld_array[i]));
  }
    // Ld at the bottom
  if (Ld_b) rep.writeSpectralData<SD>("mu_bar(Ld_b)", depth, RadUtils::meanCosine(*Ld_b));

    // mean cosines from L
  rep.writeBandHeader<SD>("Mean cosine from all radiances, (unitless)", "code", "z", band_count);
    // from L in air and water
  if (L_a) rep.writeSpectralData<SD>("mu_bar(L_a)", 0, RadUtils::meanCosine(*L_a));
  if (L_w) rep.writeSpectralData<SD>("mu_bar(L_w)", 0, RadUtils::meanCosine(*L_w));
  if (L_it) rep.writeSpectralData<SD>("mu_bar(L_it)", 0, RadUtils::meanCosine(*L_it));
  if (L_ir) rep.writeSpectralData<SD>("mu_bar(L_ir)", 0, RadUtils::meanCosine(*L_ir));
    // Lu at sample points
  for (int i=0; i<array_size; i++) {
    JString code("mu_bar(L_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m)");
    if (L_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], RadUtils::meanCosine(*L_array[i]));
  }
    // Ld at the bottom
  if (L_b) rep.writeSpectralData<SD>("mu_bar(L_b)", depth, RadUtils::meanCosine(*L_b));

    // R, diffuse reflectance
  rep.writeBandHeader<SD>("Diffuse reflectance, R = Eu/Ed, (unitless)", "code", "z", band_count);
    // from L in air and water
  if (Ld_a && Lu_a) rep.writeSpectralData<SD>("R_a", 0, Lu_a->planarIrradiance() / Ld_a->planarIrradiance());
  if (Ld_w && Lu_w) rep.writeSpectralData<SD>("R_w", 0, Lu_w->planarIrradiance() / Ld_w->planarIrradiance());
    // R at sample points
  for (int i=0; i<array_size; i++) {
    JString code("R_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    if (Ld_array[i] && Lu_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], Lu_array[i]->planarIrradiance() / Ld_array[i]->planarIrradiance());
  }
    // R at the bottom
  if (Ld_b && Lu_b) rep.writeSpectralData<SD>("R_b", depth, Lu_b->planarIrradiance() / Ld_b->planarIrradiance());

    // Q, Eu/Lu
  rep.writeBandHeader<SD>("Q-factor, Q = Eu/Lu, (sr)", "code", "z", band_count);
    // from L in air and water
  if (Lu_a) rep.writeSpectralData<SD>("Q_a", 0, Lu_a->planarIrradiance() / Lu_a->atZPlus());
  if (Lu_w) rep.writeSpectralData<SD>("Q_w", 0, Lu_w->planarIrradiance() / Lu_w->atZPlus());
    // Q at sample points
  for (int i=0; i<array_size; i++) {
    JString code("Q_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    if (Lu_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], Lu_array[i]->planarIrradiance() / Lu_array[i]->atZPlus());
  }
    // Q at the bottom
  if (Lu_b) rep.writeSpectralData<SD>("Q_b", depth, Lu_b->planarIrradiance() / Lu_b->atZPlus());

   // Total remote sensing reflectance, Rtot = Lu/Ed, (sr-1)
  rep.writeBandHeader<SD>("Total remote sensing reflectance, Rtot = Lu/Ed, (sr-1)", "code", "z", band_count);
    // from L in air and water
  if (Lu_a && Ld_a) rep.writeSpectralData<SD>("Rtot_a", 0, Lu_a->atZPlus() / Ld_a->planarIrradiance());
  if (Lu_w && Ld_w) rep.writeSpectralData<SD>("Rtot_w", 0, Lu_w->atZPlus() / Ld_w->planarIrradiance());
    // Rtot at sample points
  for (int i=0; i<array_size; i++) {
    JString code("Rtot_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    if (Lu_array[i] && Ld_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], Lu_array[i]->atZPlus() / Ld_array[i]->planarIrradiance());
  }
    // Rtot at the bottom
  if (Lu_b && Ld_b) rep.writeSpectralData<SD>("Rtot_b", depth, Lu_b->atZPlus() / Ld_b->planarIrradiance());

    // Kd at sample points - see Mobley L&W pg. 70
  rep.writeBandHeader<SD>("Downward attenuation of downwelling planar irradiance, Kd = -Delta Ed / (Ed * Delta z), (m-1)", "code", "-", band_count);
  for (int i=0; i<array_size; i++) {
    if (!dLd_dz_array[i] || !Ld_array[i]) continue;
    JString code("Kd_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    rep.writeSpectralData<SD>(code, sample_depth[i], KdSample(i));
  }
  
    // Ku at sample points - see Mobley L&W pg. 70
    // this equation has the same form as for Kd - i.e. with a negative
    // this is implied by Mobley "[Kd and Ku] asymptotically approach the same value at great depth"
    // and also gives the same result as Hydrolight
  rep.writeBandHeader<SD>("Downward attenuation of upwelling planar irradiance, Ku = -Delta Eu / (Eu * Delta z), (m-1)", "code", "-", band_count);
  for (int i=0; i<array_size; i++) {
    if (!dLu_dz_array[i] || !Lu_array[i]) continue;
    JString code("Ku_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    rep.writeSpectralData<SD>(code, sample_depth[i], KuSample(i));
  }

    // Ko at sample points - see Mobley L&W pg. 70
  rep.writeBandHeader<SD>("Downward attenuation of total scalar irradiance, Ko = -Delta Eo / (Eo * Delta z), (m-1)", "code", "-", band_count);
  for (int i=0; i<array_size; i++) {
    if (!dLd_dz_array[i] || !Ld_array[i] || !dLu_dz_array[i] || !Lu_array[i]) continue;
    JString code("Ko_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    rep.writeSpectralData<SD>(code, sample_depth[i], KoSample(i));
  }

    // Kod at sample points - see Mobley L&W pg. 70
  rep.writeBandHeader<SD>("Downward attenuation of downwelling scalar irradiance, Kod = -Delta Eod / (Eod * Delta z), (m-1)", "code", "-", band_count);
  for (int i=0; i<array_size; i++) {
    if (!dLd_dz_array[i] || !Ld_array[i]) continue;
    JString code("Kod_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    rep.writeSpectralData<SD>(code, sample_depth[i], KodSample(i));
  }

    // Kou at sample points - see Mobley L&W pg. 70
  rep.writeBandHeader<SD>("Downward attenuation of upwelling scalar irradiance, Kou = -Delta Eou / (Eou * Delta z), (m-1)", "code", "-", band_count);
  for (int i=0; i<array_size; i++) {
    if (!dLu_dz_array[i] || !Lu_array[i]) continue;
    JString code("Kou_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    rep.writeSpectralData<SD>(code, sample_depth[i], KouSample(i));
  }
  
    // KLu at sample points - see Mobley L&W pg. 70
  rep.writeBandHeader<SD>("Downward attenuation of upwelling scalar irradiance, KLu = -Delta Lu / (Lu * Delta z), (m-1)", "code", "-", band_count);
  for (int i=0; i<array_size; i++) {
    if (!dLu_dz_array[i] || !Lu_array[i]) continue;
    JString code("KLu_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    rep.writeSpectralData<SD>(code, sample_depth[i], KLuSample(i));
  }
  
    // ku at sample points
  //rep.writeBandHeader<SD>("Upward attenuation of upwelling planar irradiance, ku = Delta Eu / Delta z, (m-1)", "code", "-", band_count);
  //for (int i=0; i<array_size; i++) {
  //  JString code("ku_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
  //  if (dLd_dz_array[i]) rep.writeSpectralData<SD>(code, sample_depth[i], dLu_dz_array[i]->planarIrradiance());
  // }

    // useful directional structure tabulated values
  rep.stream() << "\n";
  full_ds.displayTableValues(rep.stream(), DsQuad::INDEX, rep.majorSep());
  rep.stream() << "\n";
  full_ds.displayTableValues(rep.stream(), DsQuad::SA, rep.majorSep());
  rep.stream() << "\n";
  full_ds.displayTableValues(rep.stream(), DsQuad::SACT, rep.majorSep());
  rep.stream() << "\n";
  full_ds.displayTableValues(rep.stream(), DsQuad::MEANTHETA, rep.majorSep());
  rep.stream() << "\n";
  full_ds.displayTableValues(rep.stream(), DsQuad::MEANPHI, rep.majorSep());

    // tabulated L
  rep.stream() << "# Tabulated quad-averaged radiances, L(theta,phi), (Wm-2sr-1nm-1)\n";
  if (L_a) rep.writeTableData("L_a", *L_a);
  if (L_w) rep.writeTableData("L_w", *L_w);
  if (L_it) rep.writeTableData("L_it", *L_it);
  if (L_ir) rep.writeTableData("L_ir", *L_ir);
    // sample points
  for (int i=0; i<array_size; i++) {
    JString code("L_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    if (L_array[i]) rep.writeTableData(code, *L_array[i]);
  }
    // L at the bottom
  if (L_b) rep.writeTableData("L_b", *L_b);

    // tabulated dL_dz at sample points
  rep.stream() << "# Tabulated quad-averaged rate of change of radiance with depth, dL(theta,phi)/dz, (Wm-2sr-1nm-1m-1)\n";
  for (int i=0; i<array_size; i++) {
    JString code("dL_dz_sample_"+JString::number(sample_depth[i], 'f', depth_code_prec) + "m");
    if (dL_dz_array[i]) rep.writeTableData(code, *dL_dz_array[i]);
  }
}



} } // namespace jude::rad

#endif // _JUDETOOLS_SLABSOLN_H

