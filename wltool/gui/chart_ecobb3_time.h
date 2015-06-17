
// chart_ecobb3_time.h

#ifndef _WLTOOL_GUI_CHART_ECOBB3_TIME_H
#define _WLTOOL_GUI_CHART_ECOBB3_TIME_H

#include "chart_time.h"
#include "chartS2/chart2d.h"
#include "chartS2/chartlinedata2d.h"
#include "chartS2/chartkey2d.h"
#include "wl/record_ecobb3.h"
#include "wl/recordlist.h"

using namespace jude::fe;
using namespace jude::chartS2;
using namespace jude::wl;


class Chart_ECOBB3_Time : public Chart_Time {

  ChartLineData2D line_470;
  ChartLineData2D line_530;
  ChartLineData2D line_650;

public:

  Chart_ECOBB3_Time(const JString& title_postfix, bool show_key, QWidget *p=0);

  void newRecordList(const RecordList* rec_list);
};



#endif // _WLTOOL_GUI_CHARTECOBB3_TIME_H

