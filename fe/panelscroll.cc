
#include "panelscroll.h"
#include <QResizeEvent>


using namespace jude::fe;


PanelScroll::PanelScroll(QWidget* parent) : 
  QScrollArea(parent) {

  //setBackgroundRole(QPalette::Dark);
  QWidget* widget_area = new QWidget;
  setWidget(widget_area);
  setWidgetResizable(true);
  //setWidgetResizable(false);
  //setFrameStyle(QFrame::NoFrame);
  //setFrameStyle(QFrame::Box);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  vbox = new QVBoxLayout(widget_area);
  vbox->setSpacing(0);
  vbox->setContentsMargins(0,0,0,0);
  vbox->setSizeConstraint(QLayout::SetMinAndMaxSize);
  setMinimumWidth(150);
}



void PanelScroll::addPanel(BasePanel* panel) {
  vbox->addWidget(panel);
  panel_list.append(panel);
  panel->setFixedSize(widget()->width(), widget()->width());
}


void PanelScroll::insertPanel(BasePanel* panel, int index) {
  vbox->insertWidget(index, panel);
  panel_list.insert(panel, index);
  panel->setFixedSize(widget()->width(), (int) (widget()->width() * panel->aspectRatio()));
}


void PanelScroll::deletePanel(BasePanel* panel) {
  panel_list.remove(panel);
  vbox->removeWidget(panel);
  delete panel;
}


void PanelScroll::deleteAllPanels() {
  BasePanel* panel = panel_list.first();
  while (panel) {
    vbox->removeWidget(panel);
    delete panel;
    panel = panel_list.next();
  }
  panel_list.clear();
}


// SpecProj::resizeEvent()

void PanelScroll::resizeEvent(QResizeEvent* e) { 
  printf("resize %d size %d %d\n",widget()->width(), e->size().width(), e->size().height());
  widget()->setFixedWidth(e->size().width());
  BasePanel* w = panel_list.first();
  while (w) {
    //printf("e %p\n",w);
    w->setFixedSize(e->size().width(), ((int) (e->size().width() * w->aspectRatio())) + w->fixedAdditionalHeight());
    w = panel_list.next();
  }
}

