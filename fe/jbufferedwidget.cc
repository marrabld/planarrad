
// jbufferedwidget.cc
// part of jude/qt
// (c) J Hedley 2003

#include <qwidget.h>
#include <qimage.h>
#include <qpainter.h>

#include "jbufferedwidget.h"
#include "jbuffer.h"
#include "jbuffer.h"
#include "jpainttarget.h"



// JBufferedWidget::(constructor)

JBufferedWidget::JBufferedWidget(JWidget *parent) : 
  JWidget(parent),
  _buffer(0),
  frame_width(0),
  border_width(0) { }



// JBufferedWidget::(constructor)

JBufferedWidget::JBufferedWidget(JBuffer& b, JWidget *parent) : 
  JWidget(parent),
  _buffer(&b),
  frame_width(0),
  border_width(0) { 

  _buffer->resize(width()-(frame_width<<1), height()-(frame_width<<1));
}



// JBufferedWidget::resize()

void JBufferedWidget::resize(int w, int h) {
  if (_buffer) _buffer->resize(w-(frame_width<<1), h-(frame_width<<1));
}



// JBufferedWidget::setBuffer()

void JBufferedWidget::setBuffer(JBuffer* b) { 
  _buffer = b; 
  _buffer->resize(width()-(frame_width<<1), height()-(frame_width<<1));
}



// JBufferedWidget::redrawRequest()
/**
This function is called when the contents of the buffer need to be redrawn, typically after
a widget resize, for example. The default is no action, it is intended that tha derived classes
will overload this function to redraw their buffer contents.
*/ 

void JBufferedWidget::redrawRequest() { }
 


// JBufferedWidget::setBorder()

void JBufferedWidget::setBorder(int w) { 
  if (border_width==w) return;
  border_width = w;
}



// JBufferedWidget::setFrame()

void JBufferedWidget::setFrame(int w) { 
  if (frame_width==w) return;
  frame_width = w;
}



// JBufferedWidget::setNeedRedraw()

void JBufferedWidget::setNeedRedraw() {
  need_redraw = true;
}



// JBufferedWidget::nowVisible()

void JBufferedWidget::nowVisible() {    
  if (need_redraw) drawToBuffer();
}



// JBufferedWidget::paintEvent()

void JBufferedWidget::paintEvent(JPaintEvent*) {

#if QT_MAJOR < 4

  QPainter p(qw); 
  if (frame_width) {
    p.setPen(QPen(qwidget().palette().active().dark(),1));
    p.drawLine(0,0,width()-1,0);
    p.drawLine(0,1,0,height()-1);
    p.setPen(QPen(qwidget().palette().active().light(),1));
    p.drawLine(width()-1,0,width()-1,height()-1);
    p.drawLine(1,height()-1,width(),height()-1);
  }

  if (!_buffer) return;
  QImage buffer_image((uchar*)_buffer->data(), _buffer->width(), _buffer->height(), 32, 0, 0, QImage::LittleEndian);
  buffer_image.setAlphaBuffer(false);

#else

  QPalette qp(qwidget().palette());
  qp.setCurrentColorGroup(QPalette::Active);

  QPainter p(qw); 
  if (frame_width) {
    p.setPen(QPen(qp.dark().color(),1));
    p.drawLine(0,0,width()-1,0);
    p.drawLine(0,1,0,height()-1);
    p.setPen(QPen(qp.light().color(),1));
    p.drawLine(width()-1,0,width()-1,height()-1);
    p.drawLine(1,height()-1,width(),height()-1);
  }

  if (!_buffer) return;
  QImage buffer_image((uchar*)_buffer->data(), _buffer->width(), _buffer->height(), QImage::Format_ARGB32);

#endif

  p.drawImage(frame_width, frame_width, buffer_image);
}

