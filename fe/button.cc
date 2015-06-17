
// button.cc

#include "fe/button.h"
#include "fe/buttongroup.h"
#include <qimage.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qcolor.h>

using namespace jude::fe;



// Button::constructor()

Button::Button(const QImage& im, QWidget* p) : 
  QWidget(p), 
  icon(QPixmap(im)),
  border(1),
  grp(0),
  is_togglable(false),
  down(false) { 
  
  setFixedSize(im.width()+2*border, im.height()+2*border);
}



// Button::constructor()
// a button which is a member of group (g)

Button::Button(const QImage& im, ButtonGroup* g, QWidget* p) :
  QWidget(p), 
  icon(QPixmap(im)),
  border(1),
  grp(g),
  down(false) { 

  grp->add(this);

  setFixedSize(im.width()+2*border, im.height()+2*border);
}



// Button::constructor()

Button::Button(const QPixmap& pix, QWidget* p) : 
  QWidget(p), 
  icon(pix),
  border(1),
  grp(0),
  down(false) { 
  
  setFixedSize(pix.width()+2, pix.height()+2);
}



// Button::constructor()
// a button which is a member of group (g)

Button::Button(const QPixmap& pix, ButtonGroup* g, QWidget* p) :
  QWidget(p), 
  icon(pix),
  border(1),
  grp(g),
  down(false) { 

  grp->add(this);

  setFixedSize(pix.width()+2, pix.height()+2);
}


 
// Button::constructor()

Button::Button(const QPixmap& pix, int border, QWidget* p) : 
  QWidget(p), 
  icon(pix),
  border(border),
  grp(0),
  down(false) { 

  setFixedSize(pix.width()+(border<<1), pix.height()+(border<<1));
}



// Button::constructor()
// a button which is a member of group (g)

Button::Button(const QPixmap& pix, int border, ButtonGroup* g, QWidget* p) :
  QWidget(p), 
  icon(pix),
  border(border),
  grp(g),
  down(false) { 

  grp->add(this);

  setFixedSize(pix.width()+(border<<1), pix.height()+(border<<1));
}



// Button::setDown()

void Button::setDown(bool f) { 
  down = f; 
  if (f && grp) grp->down(this); 
}
 


// Button::paintEvent()

void Button::paintEvent(QPaintEvent*) {

  //QPixmap buffer(width(),height()); 
  //QPainter p(&buffer);
  //buffer.fill(backgroundColor().rgb());
  //p.drawImage(border,border,im);
  
  QColor tl;
  QColor br;

  if (down) {
    tl = palette().active().dark();
    br = palette().active().light();
  } else {
    tl = palette().active().light();
    br = palette().active().dark();
  }

  QPainter p(this);

  p.setPen(tl);
  p.drawLine(0,0,width()-1,0);
  p.drawLine(0,1,0,height()-1);
  p.setPen(br);
  p.drawLine(width()-1,1,width()-1,height()-1);
  p.drawLine(1,height()-1,width()-2,height()-1);
  
  int sz = width() - border * 2;

  p.drawPixmap(border, border, icon.pixmap(QSize(sz,sz), isEnabled() ? QIcon::Normal : QIcon::Disabled, QIcon::On));
}



// Button::mousePressEvent()

void Button::mousePressEvent(QMouseEvent*) {
  if (!is_togglable && down) return; 
  down = !down;
  if (down && grp) grp->down(this); 
  repaint(false); 
  emit clicked(this);
}

