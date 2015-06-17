
// chart_time.h

#ifndef _WLTOOL_GUI_CHART_TIME_H
#define _WLTOOL_GUI_CHART_TIME_H

#include "wlchart.h"
#include "chartS2/chartlinedata2d.h"
#include "wl/record.h"

using namespace jude::fe;
using namespace jude::chartS2;
using namespace jude::wl;



class Chart_Time : public WLChart {

  ChartLineData2D time_line;

public:

  Chart_Time(QWidget* p=0);
 
  void newRecordSelected(const Record* rec);
};



#endif // _WLTOOL_GUI_CHART_TIME_H

