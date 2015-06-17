
// planeboundaryemis.h

#ifndef _JUDE_RAD_PLANEBOUNDARYEMIS_H
#define _JUDE_RAD_PLANEBOUNDARYEMIS_H

#include "rad/raddirecimp.h"
#include "base/jerror.h"

namespace jude {
namespace rad {



/*! \brief A template for directional emission data to be used as a boundary condition in a planar RTE solution. */

template <typename SD>
class PlaneBoundaryEmis {

    // this must be on a half directional structure
  RadDirecImp<SD>* rd;
    // lazy evaluated dft
  mutable RD_MatrixDFT* rd_matrix_dft;

    // delete any allocated structures
  void clear();
    // builds the internal DFT representation of the emission
  void buildDFT() const {
    rd_matrix_dft = new RD_MatrixDFT;
    rd->convertToMatrixDFT(*rd_matrix_dft);
  }
 
public:

    /*! \brief Default constructor leaves emission undefined. */
  PlaneBoundaryEmis() : rd(0), rd_matrix_dft(0) { }
    /*! \brief Destructor. */
  ~PlaneBoundaryEmis() { clear(); }

    /*! \brief Load directional emission data from file \a filepath. The directional structure is also defined by the file. */
  //  bool loadFromFile(const JString& filepath);

  bool load(const JParser& prm, const JString& param_code, const JString& desc, const DirecStruct& targ_ds, const BandSpec& targ_bs) {
    return RadUtils::loadRadDirec(rd, prm, param_code, desc, targ_ds, targ_bs);
  }

    /*! \brief Returns the directional emission data. */
  const RadDirecImp<SD>& emis() const {
    if (!rd) { jlog::es << "PlaneBoundaryEmis::emis() called but emission not defined.\n"; internalError(); }
    return *rd;
  }

    /*! \brief Returns the DFT matrix representation of the directional emission data. */
  const RD_MatrixDFT& emisMatrixDFT() const {
    if (!rd) { jlog::es << "PlaneBoundaryEmis::emisMatrixDFT() called but emission not defined"; internalError(); }
    if (!rd_matrix_dft) buildDFT();
    return *rd_matrix_dft;
  }
};



template <typename SD>
void PlaneBoundaryEmis<SD>::clear() {

  delete rd;
  delete rd_matrix_dft;

  rd = 0;
  rd_matrix_dft = 0;
}



/*
template <typename SD>
bool PlaneBoundaryEmis<SD>::loadFromFile(const JString& filepath) {

    // clear any previous allocations
  clear();

    // try to load it
  rd = FileUtils::createFactoryBuiltFromFile< RadDirecImp<SD> >(FileDesc(filepath, false));
  if (!rd) { jlog::es << "Failed to load '" << filepath << "'.\n"; return false; }
  
  if (jlog::ls.vbLevel(3)) jlog::ls << "Loaded '" << filepath << "'.\n";
  
  return true;
}
*/



} } // namespace jude::rad

#endif // _JUDE_RAD_PLANEBOUNDARYEMIS_H

