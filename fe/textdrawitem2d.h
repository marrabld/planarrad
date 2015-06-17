
// textdrawitem2d.h

#ifndef _JUDE_FE_TEXTDRAWITEM2D_H
#define _JUDE_FE_TEXTDRAWITEM2D_H

#include "drawS2/drawitem2d.h"
#include "base/jstring.h"
#include <QFont>
#include <QFontMetrics>
#include <QColor>

using namespace jude::base;
using namespace jude::S2;
using namespace jude::drawS2;

namespace jude {
namespace fe {



class TextDrawItem2D : public DrawItem2D {

  QString text_str;
  QFont font;
  QFontMetrics font_metrics;

  QColor col;
  QColor back_col;
 
  int rotate_ang;
 
  int x_size;
  int y_size;

  Vec2D centre_pos;
  Box2D bbox;

  void precalc();

public:

  TextDrawItem2D();
  TextDrawItem2D(const JString& s);
  virtual ~TextDrawItem2D();
 
    /*! Returns the text bounding box. */
  const Box2D boundingBox() const;

  void setText(const JString& s);
 
  void setCentrePos(const Vec2D& v);

  void setRotation(int ra);

  void setFont(const QFont& f);

  void setForegroundColour(const JColour& c) { col = JCOLOUR_TO_QCOLOR(c); }

  void setBackgroundColour(const JColour& c) { back_col = JCOLOUR_TO_QCOLOR(c); }

  void draw(const Trans2D& trans, JBuffer& buff) const;
  void drawEPS(const Trans2D& trans, JTextStream& str) const;
};



} } // namespace jude::fe

#endif // _JUDE_FE_TEXTDRAWITEM2D_H


