
// bufferwidget.cc

#include "fe/bufferwidget.h"
#include "base/jlog.h"

#include <qimage.h>
#include <qpainter.h>

using namespace jude::fe;



// BufferWidget::(constructor)

BufferWidget::BufferWidget(QWidget *parent) : 
  QWidget(parent),
  frame_width(0),
  border_width(0) { }



// BufferWidget::resize()

void BufferWidget::resize(int w, int h) {
  buff.requestResize(w-(frame_width<<1), h-(frame_width<<1));
}




// JBufferedWidget::redrawRequest()
/**
This function is called when the contents of the buffer need to be redrawn, typically after
a widget resize, for example. The default is no action, it is intended that tha derived classes
will overload this function to redraw their buffer contents.
*/ 

void BufferWidget::redrawRequest() { }
 


// JBufferedWidget::setBorder()

void BufferWidget::setBorder(int w) { 
  border_width = w;
}



// BufferWidget::setFrame()

void BufferWidget::setFrame(int w) { 
  frame_width = w;
}



// JBufferedWidget::setNeedRedraw()

void BufferWidget::setNeedRedraw() {
  need_redraw = true;
}



// BufferWidget::nowVisible()

void BufferWidget::nowVisible() {    
  //if (need_redraw) drawToBuffer();
}



// JBufferedWidget::paintEvent()

void BufferWidget::paintEvent(QPaintEvent*) {

  //jlog::ls << "REDRAW " << need_redraw << "\n";

  if (need_redraw) drawToBuffer();
 
#if QT_MAJOR < 4

  QPainter p(this); 
  if (frame_width) {
    p.setPen(QPen(palette().active().dark(),1));
    p.drawLine(0,0,width()-1,0);
    p.drawLine(0,1,0,height()-1);
    p.setPen(QPen(palette().active().light(),1));
    p.drawLine(width()-1,0,width()-1,height()-1);
    p.drawLine(1,height()-1,width(),height()-1);
  }

  QImage buffer_image((uchar*)buff.data(), buff.width(),buff.height(), 32, 0, 0, QImage::LittleEndian);
  buffer_image.setAlphaBuffer(false);

#else
  
  QPalette qp(palette());
  qp.setCurrentColorGroup(QPalette::Active);

  QPainter p(this); 
  if (frame_width) {
    p.setPen(QPen(qp.dark().color(),1));
    p.drawLine(0,0,width()-1,0);
    p.drawLine(0,1,0,height()-1);
    p.setPen(QPen(qp.light().color(),1));
    p.drawLine(width()-1,0,width()-1,height()-1);
    p.drawLine(1,height()-1,width(),height()-1);
  }

  QImage buffer_image((uchar*)buff.data(), buff.width(),buff.height(), QImage::Format_ARGB32);

#endif

  p.drawImage(frame_width, frame_width, buffer_image);
}


