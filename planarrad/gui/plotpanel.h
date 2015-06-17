
// plotpanel.h

#ifndef _PLANARRAD_PLOTPANEL_H
#define _PLANARRAD_PLOTPANEL_H

#include "fe/basepanel.h"
#include "wlchart.h"

using namespace jude::fe;


class PlotPanel : public BasePanel {

  Q_OBJECT

  WLChart* wl_chart;

  JString type_text;
  int channel_index;

public:

  WLPanel(const JString& type_text, int channel_index, WLChart* wl_chart, QWidget *p=0);
  ~WLPanel() { delete wl_chart; }

  const JString& typeText() const { return type_text; }
  int channelIndex() const { return channel_index; }
 
  QSizePolicy sizePolicy() const;

public slots:

  void slotNewRecordList(const RecordList* rec_list) { wl_chart->newRecordList(rec_list); }
  void slotNewRecordSelected(const Record* rec) { wl_chart->newRecordSelected(rec); }
  void slotSetShowKey(bool f) { wl_chart->setShowKey(f); }
};



#endif // _PLANARRAD_PLOTPANEL_H

