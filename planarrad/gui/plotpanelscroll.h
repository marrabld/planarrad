
// plotpanelscroll.h

#ifndef _PLANARRAD_PLOTPANELSCROLL_H
#define _PLANARRAD_PLOTPANELSCROLL_H

#include "fe/panelscroll.h"
#include "base/jstring.h"
#include "base/jparser.h"

using namespace jude::base;
using namespace jude::fe;



class PlotPanelScroll : public PanelScroll {

  Q_OBJECT

    // bool show_key;

    //PlotPanel* createPanel(const JString& code, int channel_index);
    //void checkPanel(const JString& code, int channel_index, int& curr_panel_index, const JParser& prm);

public:

  PlotPanelScroll(QWidget* p=0);

  //QSize sizeHint() const { return QSize(212,300); }

  //void rebuild(const SensorConfig& sc, const JParser& prm);

public slots:

    //void slotNewRecordList(const RecordList* rec_list);
    //void slotNewRecordSelected(const Record* rec);
    //void slotSetShowKey(bool f);
};



#endif // _PLANARRAD_PLOTPANELSCROLL_H


