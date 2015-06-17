
// profilechart.h

#ifndef _PLANARRAD_PROFILECHART_H
#define _PLANARRAD_PROFILECHART_H

#include "chartS2/chart2d.h"
#include "chartS2/chartlinedata2d.h"
#include "fe/bufferwidget.h"
#include "profiledepthline.h"
#include "fe/textchartnumbergen2d.h"
#include "fe/textchartlognumbergen2d.h"
#include "fe/textdrawitem2d.h"

using namespace jude::fe;
using namespace jude::chartS2;


class ProfileChart : public BufferWidget {

  Q_OBJECT

  TextChartNumberGen2D num_gen;
  TextChartLogNumberGen2D log_num_gen;

protected:
 
  Chart2D chart;
  //ChartKey2D* key;

  TextDrawItem2D title_ti;
  TextDrawItem2D yaxis_ti;

  ProfileDepthLine depth_line;

public:

  ProfileChart(QWidget *p=0);

  QSizePolicy sizePolicy() const;

  Chart2D& chartObject() { return chart; }

  void setBottomAxisRange(double min_v, double max_v) { chart.xBottomAxis().setValueRange(min_v, max_v); }
  void setTopAxisRange(double min_v, double max_v) { chart.xTopAxis().setValueRange(min_v, max_v); }
  
  void resizeEvent(QResizeEvent* e);

  void setMaxDepth(double d);
  void setCurrentDepth(double d);

  void setTopAxisIsLog(bool f);
  void setBottomAxisIsLog(bool f);

  void drawToBuffer();

  //virtual void newRecordList(const RecordList* rec_list) { }
  //virtual void newRecordSelected(const Record* rec) { }

public slots:

    //void slotNewRecordList(const RecordList* rec_list);
    //void slotNewRecordSelected(const Record* rec);
    //void setShowKey(bool f);
};



#endif // _PLANARRAD_PROFILECHART_H

