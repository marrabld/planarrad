
// specproj.cc

#include "fe/specproj.h"
#include <qlayout.h>

using namespace jude::fe;
using namespace jude::rad;



// SpecProj::(constructor)

SpecProj::SpecProj(QWidget *parent) : 
  BufferWidget(parent) {

  //setMinimumSize(160,160);

  setFrame(1); 
}



// SpecProj::(destructor)

SpecProj::~SpecProj() {

}



// SpecProj::sizePolicy()

QSizePolicy SpecProj::sizePolicy() const {
  return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}



// SpecProj::resizeEvent()
 
void SpecProj::resizeEvent(QResizeEvent* e) { 
  BufferWidget::resize(width()-2, height()-2);
  drawToBuffer();
}



// SpecProj::clear()

void SpecProj::clear() {
  hsd.setSourceData(0);
  drawToBuffer();
} 



// SpecProj::setSpec()
// s = null to disable

void SpecProj::setSourceData(const RadDirecImp<SBandData>* s) {
    // copy pointer to drawer or null to disable the view
  hsd.setSourceData(s);
    // render the projection
  drawToBuffer();
}



// SpecProj::autoSensitivity()
// perhaps this function ought to be globally avalaible somehow so can use the same system everywhere
// need to auto set sensitivities

double SpecProj::autoSetSensK() {
  double s = hsd.autoSetSensK();
  drawToBuffer();
  return s;
}



// SpecProj::setSensitivity()

void SpecProj::setSensK(double s) { 
  hsd.setSensK(s);
  drawToBuffer();
}



// SpecProj::drawToBuffer()

void SpecProj::drawToBuffer() {
 
    // if the widget isn't visible then just note to redraw it later
  if (!isVisible()) { need_redraw = true; return; }

    // draw the hemisphere image to the buffer
  hsd.drawToBuffer(buffer());
  
    // no longer needs a redraw
  need_redraw = false;
}

