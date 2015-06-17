
// textchartnumbergen2d.h

#ifndef _JUDE_FE_TEXTCHARTNUMBERITEMGEN2D_H
#define _JUDE_FE_TEXTCHARTNUMBERITEMGEN2D_H

#include "chartS2/chartnumberitemgen2d.h"
#include "base/jcolour.h"
#include <QFont>
#include <QFontMetrics>
#include <QColor>

using namespace jude::base;
using namespace jude::math;
using namespace jude::chartS2;

namespace jude {
namespace fe {



class TextChartNumberGen2D : public ChartNumberGen2D {

  QFont font;
  QFontMetrics font_metrics;

  JColour col;
  JColour back_col;

public:

  TextChartNumberGen2D();
  virtual ~TextChartNumberGen2D() { }

  int width(const JString& s) const;
  int height(const JString& s) const;
  int heightHint() const;

  void setForegroundColour(const JColour& c) { col = c; }
  void setBackgroundColour(const JColour& c) { back_col = c; }

  DrawItem2D* createItem(const JString& s);
};



} } // namespace jude::fe

#endif // _JUDE_FE_TEXTCHARTNUMBERITEMGEN2D_H

