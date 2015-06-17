
#include "textdrawitem2d.h"
#include <QPixmap>
#include <QPainter>
#include <QImage>


using namespace jude::fe;


TextDrawItem2D::TextDrawItem2D() :
  text_str(""),
  font("Helvetiva",8),
  font_metrics(font),
  col(0,0,0),
  back_col(0,0,0,0),
  rotate_ang(0),
  centre_pos(0,0) {

  precalc();
}



TextDrawItem2D::TextDrawItem2D(const JString& s) :
  text_str(J2Q(s)),
  font("Helvetiva",8),
  font_metrics(font),
  col(0,0,0),
  back_col(0,0,0,0),
  rotate_ang(0),
  centre_pos(0,0) {

  precalc();
}



TextDrawItem2D::~TextDrawItem2D() {
  // nothing to do
}



void TextDrawItem2D::setFont(const QFont& f) {
  font = f;
  font_metrics = QFontMetrics(font);
  precalc();
}


// TextDrawItem2D::precalc() [private]

void TextDrawItem2D::precalc() {

  if (rotate_ang==270 || rotate_ang==90) {
    x_size = font_metrics.height();
    y_size = font_metrics.width(text_str);
  } else {
    x_size = font_metrics.width(text_str);
    y_size = font_metrics.height();
  }

  Vec2D half_size(x_size/2, y_size/2);
  bbox = Box2D(centre_pos-half_size, centre_pos+half_size);
}



void TextDrawItem2D::setText(const JString& s) { 
  text_str = J2Q(s); 
  precalc();
}
 


// TextDrawItem2D::boundingBox()

const Box2D TextDrawItem2D::boundingBox() const {
  return bbox;
}
 


void TextDrawItem2D::setCentrePos(const Vec2D& v) {
  centre_pos = v;
  Vec2D half_size(x_size/2, y_size/2);
  bbox = Box2D(centre_pos-half_size, centre_pos+half_size);
}



void TextDrawItem2D::setRotation(int ra) {
  rotate_ang = ra;
  precalc();
}



void TextDrawItem2D::draw(const Trans2D& trans, JBuffer& buff) const {

  QImage qimage(x_size, y_size, QImage::Format_ARGB32);
  qimage.fill(back_col.rgba());
  //qimage.fill(0x00ff0000);
  QPainter p(&qimage);
 
  p.setPen(col);
  p.setFont(font);
  if (rotate_ang == 270) {
    p.rotate(270);
    p.drawText(-y_size, x_size-font_metrics.descent()-1, text_str);
  } else {
    //p.drawText(0, ,f.descent()-1, t);
    p.drawText(0, y_size-font_metrics.descent()-1, text_str);
  }

  Vec2D tv(trans.intoTrans(Vec2D(bbox.xMin(), bbox.yMax())));
  buff.drawData_ARGB32((uns32*)qimage.bits(), x_size, y_size, 0, 0, (int)tv.x(), (int)tv.y(), x_size, y_size);
  //printf("here %d %d %d %d\n",x_size,y_size, (int)tv.x(), (int)tv.y());

  //JBuffer b(pi.width(), pi.height(), (uns32*)pi.bits());
  //buff.drawBuffer(b, 0, 0, centre_pos.x(), centre_pos.y() ,pi.width(), pi.height());
}



void TextDrawItem2D::drawEPS(const Trans2D& trans, JTextStream& str) const {

}

