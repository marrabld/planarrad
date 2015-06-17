
// specpanelsupplier.h

#ifndef _JUDE_FE_SPECPANELSUPPLIER_H
#define _JUDE_FE_SPECPANELSUPPLIER_H

#include "fe/specpanel.h"

namespace jude { 
namespace fe {



class SpecPanelSupplier {
 
  JList<SpecPanel> spec_panel_list;

public:

  SpecPanelSupplier() { spec_panel_list.setAutoDelete(false); }
 
  void addSpecPanel(SpecPanel& sp);
 
  void setBandSpec(BandSpec* bs);
  void setUpwardSpec(const RadDirecImp<SBandData>* s, double max_v);
  void setDownwardSpec(const RadDirecImp<SBandData>* s, double max_v);
  void setTitleDepth(double d);
};



} } // namespace jude::fe

#endif // _JUDE_FE_SPECPANELSUPPLIER_H

