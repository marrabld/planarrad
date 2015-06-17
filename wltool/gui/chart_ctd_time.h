
// chart_ctd_time.h

#ifndef _WLTOOL_GUI_CHART_CTD_TIME_H
#define _WLTOOL_GUI_CHART_CTD_TIME_H

#include "chart_time.h"
#include "wl/recordlist.h"

using namespace jude::fe;
using namespace jude::chartS2;
using namespace jude::wl;



class Chart_CTD_Time : public Chart_Time {

public:

  typedef enum { TEMP=0, PRES, SALIN } chart_type;

private:

  chart_type ct;

  ChartLineData2D line;

  const JString dataName() const { 
    switch (ct) {
      case TEMP: return "Temperature";
      case PRES: return "Pressure";
      case SALIN: return "Salinity";
    }
    return "";
  }

  const JString yUnits() const { 
    switch (ct) {
      case TEMP: return "(C)";
      case PRES: return "(m) ";
      case SALIN: return "(ppt)";
    }
    return "";
  }


  const JColour lineColour() const { 
    switch (ct) {
      case TEMP: return JColour(160,160,0);
      case PRES: return JColour(0,200,200);
      case SALIN: return JColour(200,0,200);
    }
    return JColour::Black;
  }

  double ySnapStep() const { 
    switch (ct) {
      case TEMP: return 0.1;
      case PRES: return 1;
      case SALIN: return 0.01;
    }
    return 1;
  }

public:

  Chart_CTD_Time(const JString& title_postfix, chart_type ct, bool show_key, QWidget *p=0);

  void newRecordList(const RecordList* rec_list);
};



#endif // _WLTOOL_GUI_CTD_TIME_H

