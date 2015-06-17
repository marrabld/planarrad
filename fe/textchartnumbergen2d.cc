
#include "textchartnumbergen2d.h"
#include "textdrawitem2d.h"

using namespace jude::fe;



// TextChartNumberGen2D::(constructor)

TextChartNumberGen2D::TextChartNumberGen2D() :
  font("Helvetiva",8),
  font_metrics(font),
  col(0,0,0),
  back_col(0) { }



int TextChartNumberGen2D::width(const JString& s) const {
  return font_metrics.width(J2Q(s));
}



int TextChartNumberGen2D::height(const JString&) const {
  return font_metrics.height();
}



int TextChartNumberGen2D::heightHint() const {
  return font_metrics.height();
}



// TextChartNumberGen2D::createItem()

DrawItem2D* TextChartNumberGen2D::createItem(const JString& s) {

  TextDrawItem2D* tdi = new TextDrawItem2D(s);
  tdi->setFont(font);
  tdi->setForegroundColour(col);
  tdi->setBackgroundColour(back_col);

  return tdi;
}

