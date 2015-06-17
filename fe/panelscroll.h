
// panelscroll.h

#ifndef _JUDE_FE_PANELSCROLL_H
#define _JUDE_FE_PANELSCROLL_H

#include <QScrollArea>
#include <QVBoxLayout>
#include "fe/basepanel.h"
#include "base/jlist.h"

using namespace jude::base;

namespace jude {
namespace fe {



class PanelScroll : public QScrollArea {
 
  Q_OBJECT

  QVBoxLayout *vbox;

  JList<BasePanel> panel_list;

public:

  PanelScroll(QWidget* parent=0);

  void resizeEvent(QResizeEvent* e);

  void addPanel(BasePanel* panel);
  void insertPanel(BasePanel* panel, int index);
  void deletePanel(BasePanel* panel);
  void deleteAllPanels();

  const JList<BasePanel>& panelList() const { return panel_list; }

  int panelCount() const { return panel_list.count(); }
  BasePanel* panel(int i) { return panel_list.at(i); }


public slots:

  //void buttonsChanged();
    //void slotDataSetsChanged();
};



} } // namespace jude::fe

#endif // _JUDE_FE_PANELSCROLL_H

