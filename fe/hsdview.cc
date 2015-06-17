
// hsdview.cc

#include "fe/hsdview.h"
#include <qlayout.h>

using namespace jude::fe;
using namespace jude::rad;



// HSDView::(constructor)

HSDView::HSDView(QWidget *parent) : 
  BufferWidget(parent) {

  //setMinimumSize(160,160);

  setFrame(1); 
}



// HSDView::(destructor)

HSDView::~HSDView() {

}



// HSDView::sizePolicy()

QSizePolicy HSDView::sizePolicy() const {
  return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}



// HSDView::resizeEvent()
 
void HSDView::resizeEvent(QResizeEvent* e) { 
  BufferWidget::resize(width()-2, height()-2);
  drawToBuffer();
}



// HSDView::clear()

void HSDView::clear() {
  hsd.setSourceData(0);
  drawToBuffer();
} 



// HSDView::setSpec()
// s = null to disable

void HSDView::setSourceData(const RadDirecImp<SBandData>* s) {
    // copy pointer to drawer or null to disable the view
  hsd.setSourceData(s);
    // render the projection
  drawToBuffer();
}



// HSDView::autoSensitivity()
// perhaps this function ought to be globally avalaible somehow so can use the same system everywhere
// need to auto set sensitivities

double HSDView::autoSetSensK() {
  double s = hsd.autoSetSensK();
  drawToBuffer();
  return s;
}



// HSDView::setSensitivity()

void HSDView::setRelativeSensK(double s) { 
  hsd.setRelativeSensK(s);
  drawToBuffer();
}



// HSDView::drawToBuffer()

void HSDView::drawToBuffer() {
 
    // if the widget isn't visible then just note to redraw it later
  if (!isVisible()) { need_redraw = true; return; }

    // draw the hemisphere image to the buffer
  hsd.drawToBuffer(buffer());
  
    // no longer needs a redraw
  need_redraw = false;
}

