
// specpanelbar.h

#include "fe/specpanelbar.h"
#include "fe/buttongroup.h"
#include "fe/getimage.h"

using namespace jude::fe;



// SpecPanelBar::(constructor)

SpecPanelBar::SpecPanelBar(QWidget *p) : Q3VBox(p) {

  bar1 = new HButtonBar(this);
  //bar2 = new HButtonBar(this);
  //bar3 = new HButtonBar(this);

  bar1->setFixedHeight(22);
  //bar2->setFixedHeight(22);
  //bar3->setFixedHeight(22);

  // bar 1

  ButtonGroup* grp1 = new ButtonGroup();

  down_button = new Button(getImage("downward"), grp1, bar1);
  connect(down_button, SIGNAL(clicked(Button*)), this, SLOT(buttonClicked(Button*)));
  up_button = new Button(getImage("upward"), grp1, bar1);
  connect(up_button, SIGNAL(clicked(Button*)), this, SLOT(buttonClicked(Button*)));
  bar1->addSpace(4);

  ButtonGroup* grp2 = new ButtonGroup();

  proj_button = new Button(getImage("smoothdirec"), grp2, bar1);
  connect(proj_button, SIGNAL(clicked(Button*)), this, SLOT(buttonClicked(Button*)));
  ////view_button = new Button(getImage("view"), grp2, bar1);
  ////view_button->signalClicked.connect(this, slotButtonClicked);
  chart_button = new Button(getImage("chart"), grp2, bar1);
  connect(chart_button, SIGNAL(clicked(Button*)), this, SLOT(buttonClicked(Button*)));
  bar1->addSpace(4);

  anchor_button = new Button(getImage("axes"), bar1);
  connect(anchor_button, SIGNAL(clicked(Button*)), this, SLOT(buttonClicked(Button*)));

  bar1->addStretch(0, 1000);

  setFixedHeight(22);
}



// SpecPanelBar::buttonClicked() [slot]

void SpecPanelBar::buttonClicked(Button* b) {
  emit buttonsChanged();
}

