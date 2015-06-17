
// hbuttonbar.h

#ifndef _JUDE_FE_HBUTTONBAR_H
#define _JUDE_FE_HBUTTONBAR_H

#include <q3hbox.h>

namespace jude {
namespace fe {

class Button;



class HButtonBar : public Q3HBox {

  class HSpace : public QWidget {
  public:
    HSpace(int w, QWidget *p) : QWidget(p) { setFixedWidth(w); }
  };

  class HStretch : public QWidget {
  public:
    HStretch(int w0, int w1, QWidget *p) : QWidget(p) { setMinimumWidth(w0); setMaximumWidth(w1); }
  };

public:

  HButtonBar(QWidget* p=0) : Q3HBox(p) { }

  void addSpace(int w) { new HSpace(w, this); }
  void addStretch(int w0, int w1) { new HStretch(w0, w1, this); }
};



} } // namespace jude::fe

#endif // _JUDE_FE_HBUTTONBAR_H

