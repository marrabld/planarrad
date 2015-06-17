
#ifndef _JUDE_RAD_PLANEINTERFACE_H
#define _JUDE_RAD_PLANEINTERFACE_H

#include "rad/raddirecdirecimp.h"
#include "base/jerror.h"
#include "base/jparser.h"
#include "S3/vec3d.h"

using namespace jude::S3;

namespace jude {
namespace rad {

class RDD_MatrixDFT;



template <typename SD>
class PlaneInterface {

  RadDirecDirecImp<SD>* surf_full;

  RadDirecDirecImp<SD>* surf_r0;
  RadDirecDirecImp<SD>* surf_r1;
  RadDirecDirecImp<SD>* surf_t01;
  RadDirecDirecImp<SD>* surf_t10;

public:

  PlaneInterface() :
    surf_full(0),
    surf_r0(0),
    surf_r1(0),
    surf_t01(0),
    surf_t10(0) { }
  
  ~PlaneInterface() { clear(); }

  void clear() {
    delete surf_full;
    delete surf_r0;
    delete surf_r1;
    delete surf_t01;
    delete surf_t10;
    surf_full = 0;
    surf_r0 = 0;
    surf_r1 = 0;
    surf_t01 = 0;
    surf_t10 = 0;
  }

    /*! \brief Initialise the interface from \a prm looking for parameters prefixed by \a name. */
  bool load(const JString& name, const JParser& prm, const DirecStruct& master_ds, int band_count);

  const RadDirecDirecImp<SD>& reflecZmZp() const { 
    assert(surf_r0 != 0); 
    return *surf_r0;
  }

  const RadDirecDirecImp<SD>& reflecZpZm() const { 
    assert(surf_r1 != 0); 
    return *surf_r1;
  }

  const RadDirecDirecImp<SD>& transZm() const { 
    assert(surf_t01 != 0); 
    return *surf_t01;
  }

  const RadDirecDirecImp<SD>& transZp() const { 
    assert(surf_t10 != 0); 
    return *surf_t10;
  }
};



template <typename SD>
bool PlaneInterface<SD>::load(const JString& name, const JParser& prm, const DirecStruct& master_ds, int band_count) {

  clear();
  
    // try to get a filepath to load
  JString fp = prm.getStringOrEmpty(name+"_fp");
  if (fp.isEmpty()) { jlog::es << "No specification of the interface '" << name << "'.\n"; return false; }
 
    // try to load the full directional function
  surf_full = FileUtils::createFactoryBuiltFromFile< RadDirecDirecImp<SD> >(FileDesc(fp, false));
  if (!surf_full) return false;
      // info
  if (jlog::ls.vbLevel(3)) jlog::ls << "Loaded '" << fp << "'.\n";
    // check number of bands
  if (surf_full->bandCount() != band_count && surf_full->bandCount() != 1) {
    jlog::es << "File '" << fp << "' is in " << surf_full->bandCount() << " bands but should be 1 or " << band_count << ".\n";
    return false; 
  }
    // if is mono and we need more than one band then expand
  if (surf_full->bandCount() == 1 && band_count != 1) {
      // info
    if (jlog::vbLevel(3)) jlog::ls << "Expanding '" << fp << "' from one band to " << band_count << " bands by repeating the data.\n";
      // do it
    surf_full->expandBandCount(band_count);
  }

  surf_r0 = surf_full->duplicateHalfStructure();
  surf_r1 = surf_full->duplicateHalfStructure();
  surf_t01 = surf_full->duplicateHalfStructure();
  surf_t10 = surf_full->duplicateHalfStructure();

    // split the full sphere function into four
  surf_full->splitNegate(surf_r0, surf_t01, surf_t10, surf_r1);
 
    // check directional structure   
  if (!master_ds.isEqual(surf_r0->direcStruct()) ||
      !master_ds.isEqual(surf_r1->direcStruct()) ||
      !master_ds.isEqual(surf_t01->direcStruct()) ||
      !master_ds.isEqual(surf_t10->direcStruct())) {
    
    jlog::es << "File '" << fp << "' has incompatible directional structure.\n"; 
    return false;
  }

    // convert transfer functions to work purely on radiances
    // they are stored like BRDFs to translate integral of radiance cos theta (i.e planar irradiance) into radiance 
  surf_r0->multiplyAllByInSACT();
  surf_r1->multiplyAllByInSACT();
  surf_t01->multiplyAllByInSACT();
  surf_t10->multiplyAllByInSACT();
    
  return true;
}



} } // namespace jude::rad

#endif // _JUDE_RAD_PLANEINTERFACE_H

