
// chart_acs_data.h

#ifndef _WLTOOL_GUI_CHART_ACS_DATA_H
#define _WLTOOL_GUI_CHART_ACS_DATA_H

#include "wlchart.h"
#include "chartS2/chart2d.h"
#include "chartS2/chartlinedata2d.h"
#include "chartS2/chartkey2d.h"
#include "wl/record_acs.h"
#include "wl/recordlist.h"

using namespace jude::fe;
using namespace jude::chartS2;
using namespace jude::wl;


class Chart_ACS_Data : public WLChart {

  ChartLineData2D a_line;
  ChartLineData2D c_line;
 
public:

  Chart_ACS_Data(const JString& title_postfix, bool show_key, QWidget *p=0);
 
  void newRecordList(const RecordList* rec_list);
  void newRecordSelected(const Record* rec);
};



#endif // _WLTOOL_GUI_CHART_ACS_DATA_H

