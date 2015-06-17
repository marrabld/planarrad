
// planeboundaryreflec.h

#ifndef _JUDE_RAD_PLANEBOUNDARYREFLEC_H
#define _JUDE_RAD_PLANEBOUNDARYREFLEC_H

#include "rad/raddirecimp.h"
#include "rad/radutils.h"
#include "base/jerror.h"

namespace jude {
namespace rad {



/*! \brief A template for directional reflection data to be used as a boundary condition in a planar RTE solution. */

template <typename SD>
class PlaneBoundaryReflec {

    // diffuse reflection, zero length until defined
  mutable SD subs_diffuse_r0;
    // direction reflection structure
  RadDirecDirecImp<SD>* subs_r0;
    // lazy evaluated dft
  RDD_MatrixDFT* subs_r0_dft;

    // delete any allocated structures
  void clear();
    // the directional version is always built by calling this if a diffuse reflection is loaded 
  void buildDirectionalFromDiffuse(const DirecStruct& ds);
    // builds the internal DFT representation of the reflection
  void buildDFT() {
    delete subs_r0_dft;
    subs_r0_dft = new RDD_MatrixDFT();
    subs_r0->copyToMatrixDFTSym(*subs_r0_dft);
  }

public:

  PlaneBoundaryReflec() :
    subs_diffuse_r0(0),
    subs_r0(0),
    subs_r0_dft(0) { }

  ~PlaneBoundaryReflec() { clear(); }

  //const DirecStruct& direcStruct() const {
  //  if (!subs_r0) { jlog::es << "Substrate::direcStruct() called but no directional relfectance defined\n"; internalError(); }
  //  return subs_r0->direcStruct();
  // }

    /*! \brief Initialise the boundary reflectance from \a prm looking for parameters prefixed by \a name. */
  bool load(const JString& name, const JParser& prm, const DirecStruct& master_ds, const BandSpec& master_bs);

  bool loadFromFile(const JString& filepath, const DirecStruct& ds);

  void setDiffuseReflectance(const SD& reflec, const DirecStruct& ds);

  const SD& diffuseReflectance() const {
      // lazy evaluate diffuse reflectance when loading from directional file as might not need it
    if (subs_diffuse_r0.bandCount() == 0 && subs_r0) subs_diffuse_r0 = subs_r0->diffuseReflectance();
    return subs_diffuse_r0; 
  }

  const RadDirecDirecImp<SD>& directionalReflectance() const {
    if (!subs_r0) { jlog::es << "Substrate::directionalReflection() called but directional reflection not defined\n"; internalError(); }
    return *subs_r0;
  }

  const RDD_MatrixDFT& reflecMatrixDFT() {
    if (!subs_r0_dft) buildDFT();  
    return *subs_r0_dft;
  }
};



template <typename SD>
void PlaneBoundaryReflec<SD>::clear() {
  subs_diffuse_r0 = SBandData(0);
  delete subs_r0;
  delete subs_r0_dft;
  subs_r0 = 0;
  subs_r0_dft = 0;
}



template <typename SD>
bool PlaneBoundaryReflec<SD>::load(const JString& name, const JParser& prm, const DirecStruct& master_ds, const BandSpec& master_bs) {

  clear();

    // attempt to load total irradiance, false return means specification attempted but something wrong
  if (!RadUtils::loadSBandData(subs_diffuse_r0, prm, name+"_diffuse", name+" diffuse reflectance", master_bs)) return false;
    // if diffuse was defined then build directional representation and quit
  if (subs_diffuse_r0.bandCount() != 0) {
    buildDirectionalFromDiffuse(master_ds);
    return true;
  } 
  
    // try to get a filepath to load
  JString fp = prm.getStringOrEmpty(name+"_fp");
  if (fp.isEmpty()) { jlog::es << "No specification of the reflectance boundary '" << name << "'.\n"; return false; }
    // try to load the directional reflectance
  subs_r0 = FileUtils::createFactoryBuiltFromFile< RadDirecDirecImp<SD> >(FileDesc(fp, false));
  if (!subs_r0) { jlog::es << "Failed to load '" << fp << "'.\n"; return false; }
    // info
  if (jlog::ls.vbLevel(3)) jlog::ls << "Loaded '" << fp << "'.\n";
    // check directional structure 
  if (!RadUtils::checkDirecStruct(fp, subs_r0->direcStruct(), master_ds)) return false;
    // check number of bands
  if (subs_r0->bandCount() != master_bs.bandCount() && subs_r0->bandCount() != 1) {
    jlog::es << "File '" << fp << "' is in " << subs_r0->bandCount() << " bands but should be 1 or " << master_bs.bandCount() << ".\n";
    return false; 
  }
    // check data type - I think BRDF is the most suitable for this
  if (!subs_r0->inDataTypeIsPlanarIrrad() || !subs_r0->exDataTypeIsMeanRad()) {
    jlog::es << "File '" << fp << "' is of data type " << subs_r0->inDataString() << " to " << subs_r0->exDataString();
    jlog::es << " but should be planar irradiance to quad-averaged radiance.\n";
    return false; 
  }
    // convert to L-> L function
  subs_r0->multiplyAllByInSACT();
 
  return true;
}



// PlaneBoundaryReflec::loadFromFile()

template <typename SD>
bool PlaneBoundaryReflec<SD>::loadFromFile(const JString& filepath, const DirecStruct& ds) {

  clear();
  
  FileDesc fd(filepath, true);
  if (!FileUtils::readFactoryBuiltFromFile(subs_diffuse_r0, fd)) { jlog::es << "Failed to load diffuse reflectance file " << fd.path() << "\n"; return false; }
 
  buildDirectionalFromDiffuse(ds);

  return true;
}



// PlaneBoundaryReflec::setDiffuseReflectance()

template <typename SD>
void PlaneBoundaryReflec<SD>::setDiffuseReflectance(const SD& reflec, const DirecStruct& ds) {

  clear();
  subs_diffuse_r0 = reflec;
  buildDirectionalFromDiffuse(ds);
}



// Substrate::buildDirecFromDiffuse() [private]

template <typename SD>
void PlaneBoundaryReflec<SD>::buildDirectionalFromDiffuse(const DirecStruct& ds) {

  if (!ds.isSameType(HemiSpherePartVxH())) {
    jlog::es << "PlaneBoundaryReflec<SD>::buildDirectionalFromDiffuse() unsupported directional structure.\n";
    internalError();
  }

    // create directional structure
  subs_r0 = new RadDirecDirecTable<SD,HemiSpherePartVxH>(ds, subs_diffuse_r0.bandCount(), RadDirecDirec::PLANAR_IRRAD, RadDirecDirec::MEAN_RAD);
    // make as diffuse reflectance (i.e all values R/pi)
  subs_r0->makeAll(subs_diffuse_r0 / M_PI);
    // convert to L-> L function
  subs_r0->multiplyAllByInSACT();
    // set data type
  subs_r0->setInDataTypeMeanRad();
  subs_r0->setExDataTypeMeanRad();
}



} } // namespace jude::rad

#endif // _JUDE_RAD_PLANEBOUNDARYREFLEC_H


