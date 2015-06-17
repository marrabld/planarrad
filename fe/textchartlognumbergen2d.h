
// textchartlognumbergen2d.h

#ifndef _JUDE_FE_TEXTCHARTLOGNUMBERITEMGEN2D_H
#define _JUDE_FE_TEXTCHARTLOGNUMBERITEMGEN2D_H

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



class TextChartLogNumberGen2D : public ChartNumberGen2D {

  QFont font;
  QFontMetrics font_metrics;

  QFont font_sup;
  QFontMetrics font_sup_metrics;

  JColour col;
  JColour back_col;
 
  int sep;
  int offset;
 
public:

  TextChartLogNumberGen2D();
  virtual ~TextChartLogNumberGen2D() { }

  int width(const JString& s) const;
  int height(const JString& s) const;
  int heightHint() const;

  DrawItem2D* createItem(const JString& s);
};



} } // namespace jude::fe

#endif // _JUDE_FE_TEXTCHARTLOGNUMBERITEMGEN2D_H

