
#include "textchartlognumbergen2d.h"
#include "lognumberdrawitem2d.h"

using namespace jude::fe;



// TextChartLogNumberGen2D::(constructor)

TextChartLogNumberGen2D::TextChartLogNumberGen2D() :
  font("Helvetiva",8),
  font_metrics(font),
  font_sup("Helvetiva",7),
  font_sup_metrics(font_sup),
  col(0,0,0),
  back_col(255,255,255) { 

  sep = 2;
  offset = (font_metrics.height() * 3) / 4;
}



int TextChartLogNumberGen2D::width(const JString& s) const {
  return font_sup_metrics.width(J2Q(s)) + sep + font_metrics.width("10");
}



int TextChartLogNumberGen2D::height(const JString&) const {
  return font_metrics.height() - offset + font_sup_metrics.height();
}



int TextChartLogNumberGen2D::heightHint() const {
  return font_metrics.height() - offset + font_sup_metrics.height();
}



// TextChartLogNumberGen2D::createItem()

DrawItem2D* TextChartLogNumberGen2D::createItem(const JString& s) {

  LogNumberDrawItem2D* tdi = new LogNumberDrawItem2D(s);
  tdi->setFont(font);
  tdi->setSupFont(font_sup);
  tdi->setSep(sep);
  tdi->setOffset(offset);
  tdi->setForegroundColour(col);
  tdi->setBackgroundColour(back_col);

  return tdi;
}

