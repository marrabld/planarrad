
// lognumberdrawitem2d.h

#ifndef _JUDE_FE_LOGNUMBERDRAWITEM2D_H
#define _JUDE_FE_LOGNUMBERDRAWITEM2D_H

#include "drawS2/drawitem2d.h"
#include "fe/textdrawitem2d.h"
#include "base/jstring.h"
#include <QFont>
#include <QFontMetrics>
#include <QColor>

using namespace jude::base;
using namespace jude::S2;
using namespace jude::drawS2;

namespace jude {
namespace fe {



class LogNumberDrawItem2D : public DrawItem2D {

  TextDrawItem2D main_num;
  TextDrawItem2D sup_num;
 
  int rotate_ang;
 
  int x_size;
  int y_size;

  Vec2D centre_pos;
  Box2D bbox;

  int sep;
  int offset;

  void precalc();

public:

  LogNumberDrawItem2D();
  LogNumberDrawItem2D(const JString& s);
  virtual ~LogNumberDrawItem2D();
 
    /*! Returns the text bounding box. */
  const Box2D boundingBox() const;

  void setText(const JString& s);
 
  void setCentrePos(const Vec2D& v);

  void setRotation(int ra);

  void setFont(const QFont& f);
  void setSupFont(const QFont& f);

  void setSep(int s);
  void setOffset(int offset);
 
  void setForegroundColour(const JColour& c);

  void setBackgroundColour(const JColour& c);

  void draw(const Trans2D& trans, JBuffer& buff) const;
  void drawEPS(const Trans2D& trans, JTextStream& str) const;
};



} } // namespace jude::fe

#endif // _JUDE_FE_LOGNUMBERDRAWITEM2D_H


