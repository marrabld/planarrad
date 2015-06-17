
// chart_hocr_data.h

#ifndef _WLTOOL_GUI_CHART_HOCR_DATA_H
#define _WLTOOL_GUI_CHART_HOCR_DATA_H

#include "wlchart.h"
#include "chartS2/chart2d.h"
#include "chartS2/chartlinedata2d.h"
#include "chartS2/chartkey2d.h"
#include "wl/record_hocr.h"
#include "wl/recordlist.h"

using namespace jude::fe;
using namespace jude::chartS2;
using namespace jude::wl;


class Chart_HOCR_Data : public WLChart {

public:

  typedef enum { ED_RAW=0, LU_RAW, ED, LU, ED_DARK, LU_DARK, ES } chart_type;

private:

  chart_type ct;

  ChartLineData2D data_line;

  const JString dataName() const { 
    switch (ct) {
      case ED_RAW : return "Ed Raw";
      case LU_RAW : return "Lu Raw";
      case ED : return "Ed";
      case LU : return "Lu";
      case ED_DARK : return "Ed Raw Dark";
      case LU_DARK : return "Lu Raw Dark";
      case ES : return "Es";
    }
    return "";
  }

  const JString yUnits() const { 
    switch (ct) {
      case ED_RAW : return "Counts x 1000";
      case LU_RAW : return "Counts x 1000";
      case ED : return "uWcm-2nm-1";
      case LU : return "uWcm-2nm-1sr-1";
      case ED_DARK : return "Counts x 1000";
      case LU_DARK : return "Counts x 1000";
      case ES : return "uWcm-2nm-1"; 
    }
    return "";
  }

  bool isChartType(const Record_HOCR* rec) const {
    if (!rec) return false; 
    return \
      ((ct==ED_RAW || ct==ED) && rec->isEdLight()) || \
      ((ct==LU_RAW || ct==LU) && rec->isLuLight()) || \
      (ct==ED_DARK && rec->isEdDark()) || \
      (ct==LU_DARK && rec->isLuDark()) || (ct==ES);
  }

  bool isEd() const { return (ct==ED_RAW || ct==ED || ct==ED_DARK); }
  bool isEs() const { return (ct==ES); }

  bool isRaw() const { return (ct==ED_RAW || ct==LU_RAW || ct==ED_DARK || ct==LU_DARK); }
 
public:

  Chart_HOCR_Data(const JString& title_postfix, chart_type ct, bool show_key, QWidget *p=0);
 
  void newRecordList(const RecordList* rec_list);
  void newRecordSelected(const Record* rec);
};



#endif // _WLTOOL_GUI_CHART_HOCR_DATA_H

