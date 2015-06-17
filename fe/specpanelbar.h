
// specpanelbar.h

#ifndef _JUDE_FE_SPECPANELBAR_H
#define _JUDE_FE_SPECPANELBAR_H

#include <q3vbox.h>
#include "fe/hbuttonbar.h"
#include "fe/button.h"

namespace jude {
namespace fe {



class SpecPanelBar : public Q3VBox {

  Q_OBJECT

  HButtonBar* bar1;
  //HButtonBar* bar2;
  //HButtonBar* bar3;

  Button* down_button;
  Button* up_button;

  Button* proj_button;
  //Button* view_button;
  Button* chart_button;
 
  Button* anchor_button;

private slots:
 
 void buttonClicked(Button* b);

public:

  SpecPanelBar(QWidget* p=0);

  bool drawDown() const { return down_button->isDown(); }
  bool drawUp() const { return up_button->isDown(); }

  bool drawProj() const { return proj_button->isDown(); }
  //bool drawView() const { return view_button->isDown(); }
  bool drawView() const { return false; }
  bool drawChart() const { return chart_button->isDown(); }
 
  bool axisAnchor() const { return anchor_button->isDown(); }
 
  void setDrawDown(bool f) { down_button->setDown(f); }
  void setDrawUp(bool f) { up_button->setDown(f); }
 
  void setDrawProj(bool f) { proj_button->setDown(f); }
  //void setDrawView(bool f) { view_button->setDown(f); }
  void setDrawView(bool f) { }
  void setDrawChart(bool f) { chart_button->setDown(f); }
 
signals:

  void buttonsChanged();
};



} } // namespace jude::fe

#endif // _JUDE_FE_SPECPANELBAR_H

