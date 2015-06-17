
// aqrad
// specview.cc
// (c) John Hedley 2003

#include "config.h"

#ifdef GUI

#include "specview.h"
#include "quadview.h"
#include "specimp.h"
#include "jvboxlayout.h"
#include <stdio.h>



// SpecView::(constructor)

SpecView::SpecView(JWidget *p) : 
  JWidget(p) {

  JVBoxLayout* vbox = new JVBoxLayout(this);

  Box3D bbox(Vector3D(-0.8,-0.8,-0.8), Vector3D(0.8,0.8,0.8));

  quad_view = new QuadView(this);
  vbox->addWidget(quad_view);
  quad_view->setFrame(1);
  quad_view->trans().setBoundingBox(bbox);
  quad_view->signalTranslated.connect(this, slotTranslated);
  quad_view->trans().rotateX(-M_PI/2);
  quad_view->trans().rotateY(-M_PI/7);
  quad_view->trans().rotateX(-M_PI/8);
  quad_view->setDrawShape(true);

  //quad_view->trans().rotateZ(M_PI*0.9);

  //setSpacing(4);
}



// SpecView::sizePolicy()

JSizePolicy SpecView::sizePolicy() const {
  return JSizePolicy(JSizePolicy::Expanding, JSizePolicy::Expanding);
}



// SpecView::setSurfaceContext()

void SpecView::setSurfaceContext(bool f) {
  if (f) quad_view->setSurfaceContext();
  else quad_view->setAxesContext();
}



// SpecView::clear()

void SpecView::clear() {
  quad_view->clear();
}



// SpecView::setSpec()

void SpecView::setSpec(const SpecImp<SpectralData>* spec) {
  quad_view->setRadSource(spec);
}



// SpecView::exportPostcript()

void SpecView::exportPostscript(const JString& filepath) {
  quad_view->exportPostscript(filepath);
} 



// SpecView::actionTranslated()

void SpecView::actionTranslated(View3D* dv) {
  quad_view->drawSpec();
}



#endif // GUI

