
// chart_acs_time.h

#ifndef _WLTOOL_GUI_CHART_ACS_TIME_H
#define _WLTOOL_GUI_CHART_ACS_TIME_H

#include "chart_time.h"
#include "chartS2/chart2d.h"
#include "chartS2/chartlinedata2d.h"
#include "chartS2/chartkey2d.h"
#include "wl/record_acs.h"
#include "wl/recordlist.h"

using namespace jude::fe;
using namespace jude::chartS2;
using namespace jude::wl;


class Chart_ACS_Time : public Chart_Time {

public:

  typedef enum { A=0, C } chart_type;

private:

  chart_type ct;

  ChartLineData2D line_470;
  ChartLineData2D line_530;
  ChartLineData2D line_650;

  const JString dataName() const { return (ct==A) ? "a" : "c"; } 

public:

  Chart_ACS_Time(const JString& title_postfix, chart_type ct, bool show_key, QWidget *p=0);

  void newRecordList(const RecordList* rec_list);
};



#endif // _WLTOOL_GUI_CHART_ACS_TIME_H

