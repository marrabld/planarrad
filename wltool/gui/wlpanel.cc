
// wlpanel.cc

#include "wlpanel.h"
#include <QVBoxLayout>

WLPanel::WLPanel(const JString& type_text, int channel_index, WLChart* wl_chart, QWidget *p) :
  BasePanel(p),
  wl_chart(wl_chart),
  type_text(type_text),
  channel_index(channel_index) {
 
  QVBoxLayout* vl = new QVBoxLayout(this);
  vl->setContentsMargins(0,0,0,0);
  vl->addWidget(wl_chart);
}




// ChartACS::sizePolicy()

QSizePolicy WLPanel::sizePolicy() const {
  return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


