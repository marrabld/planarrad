
// specchart.h

#ifndef _PLANARRAD_SPECCHART_H
#define _PLANARRAD_SPECCHART_H

#include "chartS2/chart2d.h"
#include "chartS2/chartlinedata2d.h"
#include "fe/bufferwidget.h"
#include "fe/textchartnumbergen2d.h"
#include "fe/textchartlognumbergen2d.h"
#include "fe/textdrawitem2d.h"

using namespace jude::fe;
using namespace jude::chartS2;


class SpecChart : public BufferWidget {

  Q_OBJECT
 
  TextDrawItem2D title_ti;

  TextChartNumberGen2D num_gen;
  TextChartLogNumberGen2D log_num_gen;

  TextDrawItem2D* ti;

protected:
 
  Chart2D chart;
  //ChartKey2D* key;

public:

  SpecChart(QWidget *p=0);
  ~SpecChart();

  QSizePolicy sizePolicy() const;

  Chart2D& chartObject() { return chart; }

  void setLeftAxisRange(double min_v, double max_v) { chart.yLeftAxis().setValueRange(min_v, max_v); }
  void setRightAxisRange(double min_v, double max_v) { chart.yRightAxis().setValueRange(min_v, max_v); }
  
  void resizeEvent(QResizeEvent* e);

  void setLeftAxisIsLog(bool f);
  void setRightAxisIsLog(bool f);

  void setCurrentDepth(double d);

  void drawToBuffer();

public slots:

};



#endif // _PLANARRAD_SPECCHART_H

