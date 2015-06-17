
#include "fe/lognumberdrawitem2d.h"

using namespace jude::fe;



LogNumberDrawItem2D::LogNumberDrawItem2D() :
  main_num("10"),
  sup_num("1"),
  rotate_ang(0),
  centre_pos(0,0),
  sep(2),
  offset(4) {

  precalc();
}



LogNumberDrawItem2D::LogNumberDrawItem2D(const JString& s) :
  main_num("10"),
  sup_num(s),
  rotate_ang(0),
  centre_pos(0,0),
  sep(2),
  offset(4) {

  precalc();
}



LogNumberDrawItem2D::~LogNumberDrawItem2D() {
  // nothing to do
}



// LogNumberDrawItem2D::precalc() [private]

void LogNumberDrawItem2D::precalc() {

  if (rotate_ang==270 || rotate_ang==90) {
    x_size = (int) (main_num.boundingBox().ySize() - offset + sup_num.boundingBox().ySize());
    y_size = (int) (main_num.boundingBox().xSize() + sep + sup_num.boundingBox().xSize());
  } else {
    x_size = (int) (main_num.boundingBox().xSize() + sep + sup_num.boundingBox().xSize());
    y_size = (int) (main_num.boundingBox().ySize() - offset + sup_num.boundingBox().ySize());
  }

  Vec2D half_size(x_size/2, y_size/2);
  bbox = Box2D(centre_pos-half_size, centre_pos+half_size);
}



const Box2D LogNumberDrawItem2D::boundingBox() const {
  return bbox;
}



void LogNumberDrawItem2D::setText(const JString& s) {
  sup_num.setText(s);
  precalc();
}



void LogNumberDrawItem2D::setCentrePos(const Vec2D& v) {

  centre_pos = v;
  Vec2D half_size(x_size/2, y_size/2);
  bbox = Box2D(centre_pos-half_size, centre_pos+half_size);

  if (rotate_ang==270 || rotate_ang==90) {
 
    Vec2D bot_left(centre_pos - Vec2D(x_size/2, -y_size/2));
  
    main_num.setCentrePos(bot_left + Vec2D(-main_num.boundingBox().ySize() / 2, main_num.boundingBox().xSize() / 2));
    sup_num.setCentrePos(bot_left + Vec2D(-main_num.boundingBox().xSize() + offset - sup_num.boundingBox().xSize() / 2,
					  main_num.boundingBox().ySize() + sep + sup_num.boundingBox().ySize()/2));
 
  } else {

    Vec2D bot_left(centre_pos - half_size);
 
    main_num.setCentrePos(bot_left + Vec2D(main_num.boundingBox().xSize() / 2, main_num.boundingBox().ySize() / 2));
    sup_num.setCentrePos(bot_left + Vec2D(main_num.boundingBox().xSize() + sep + sup_num.boundingBox().xSize()/2, 
					  main_num.boundingBox().ySize() - offset + sup_num.boundingBox().ySize() / 2));
 
  }
}



void LogNumberDrawItem2D::setRotation(int ra) {
  rotate_ang = ra;
  main_num.setRotation(ra);
  sup_num.setRotation(ra);
  precalc();
}



void LogNumberDrawItem2D::setFont(const QFont& f) {
  main_num.setFont(f);
  precalc();
}



void LogNumberDrawItem2D::setSupFont(const QFont& f) {
  sup_num.setFont(f);
  precalc();
}



void LogNumberDrawItem2D::setSep(int s) {
  sep = s;
  precalc();
}



void LogNumberDrawItem2D::setOffset(int v) {
  offset = v;
  precalc();
}
 


void LogNumberDrawItem2D::setForegroundColour(const JColour& c) { 
  main_num.setForegroundColour(c);
  sup_num.setForegroundColour(c);
}



void LogNumberDrawItem2D::setBackgroundColour(const JColour& c) { 
  main_num.setBackgroundColour(c);
  sup_num.setBackgroundColour(c);
}



void LogNumberDrawItem2D::draw(const Trans2D& trans, JBuffer& buff) const {
  main_num.draw(trans, buff);
  sup_num.draw(trans, buff);
}



void LogNumberDrawItem2D::drawEPS(const Trans2D& trans, JTextStream& str) const {
  main_num.drawEPS(trans, str);
  sup_num.drawEPS(trans, str);
}

