
// wlchart.h

#ifndef _WLTOOL_GUI_WLCHART_H
#define _WLTOOL_GUI_WLCHART_H

#include "chartS2/chart2d.h"
#include "chartS2/chartlinedata2d.h"
#include "fe/bufferwidget.h"
#include "wl/record_acs.h"
#include "wl/recordlist.h"

using namespace jude::fe;
using namespace jude::chartS2;
using namespace jude::wl;


class WLChart : public BufferWidget {

  Q_OBJECT

protected:
 
  Chart2D chart;
  ChartKey2D* key;

public:

  WLChart(QWidget *p=0);

  QSizePolicy sizePolicy() const;

  void resizeEvent(QResizeEvent* e);

  void drawToBuffer();

  virtual void newRecordList(const RecordList* rec_list) { }
  virtual void newRecordSelected(const Record* rec) { }

public slots:

  void slotNewRecordList(const RecordList* rec_list);
  void slotNewRecordSelected(const Record* rec);
  void setShowKey(bool f);
};



#endif // _WLTOOL_GUI_WLCHART_H

