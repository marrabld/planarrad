
// wlpanelscroll.h

#ifndef _WLTOOL_GUI_PANELSCROLL_H
#define _WLTOOL_GUI_PANELSCROLL_H

#include "fe/panelscroll.h"
#include "base/jstring.h"
#include "base/jparser.h"
#include "wl/sensorconfig.h"
#include "wlpanel.h"

using namespace jude::base;
using namespace jude::fe;
using namespace jude::wl;



class WLPanelScroll : public PanelScroll {

  Q_OBJECT

  bool show_key;

  WLPanel* createPanel(const JString& code, int channel_index);
  void checkPanel(const JString& code, int channel_index, int& curr_panel_index, const JParser& prm);

public:

  WLPanelScroll(QWidget* p=0);

  void rebuild(const SensorConfig& sc, const JParser& prm);

public slots:

  void slotNewRecordList(const RecordList* rec_list);
  void slotNewRecordSelected(const Record* rec);
  void slotSetShowKey(bool f);
};



#endif // _WLTOOL_GUI_PANELSCROLL_H


