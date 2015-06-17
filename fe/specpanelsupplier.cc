
#include "specpanelsupplier.h" 

using namespace jude::fe;



void SpecPanelSupplier::addSpecPanel(SpecPanel& sp) { 
  spec_panel_list.append(&sp); 
}
 


void SpecPanelSupplier::setBandSpec(BandSpec* bs) {

  SpecPanel* sp = spec_panel_list.first();
  while (sp) {
    sp->setBandSpec(bs);
    sp = spec_panel_list.next();
  }
}


 
void SpecPanelSupplier::setUpwardSpec(const RadDirecImp<SBandData>* s, double max_v) {

  SpecPanel* sp = spec_panel_list.first();
  while (sp) {
    sp->setUpwardSpec(s, max_v);
    sp = spec_panel_list.next();
  }
}



void SpecPanelSupplier::setDownwardSpec(const RadDirecImp<SBandData>* s, double max_v) {
  SpecPanel* sp = spec_panel_list.first();
  while (sp) {
    sp->setDownwardSpec(s, max_v);
    sp = spec_panel_list.next();
  }
}



void SpecPanelSupplier::setTitleDepth(double d) {
  SpecPanel* sp = spec_panel_list.first();
  while (sp) {
    sp->setTitleDepth(d);
    sp = spec_panel_list.next();
  }
}

