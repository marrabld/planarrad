
// bufferwidget.h

#ifndef _JUDE_FE_BUFFERWIDGET_H
#define _JUDE_FE_BUFFERWIDGET_H

#include <qwidget.h>
#include "base/jbuffer.h"

using namespace jude::base;

namespace jude {
namespace fe {



class BufferWidget : public QWidget {

  Q_OBJECT

  JBuffer buff;
 
protected:

  int frame_width;
  int border_width;
  bool need_redraw;

  virtual void redrawRequest();

  void paintEvent(QPaintEvent*);

public:

  BufferWidget(QWidget *parent);
  virtual ~BufferWidget() { }

  virtual void resize(int w, int h);
 
  JBuffer& buffer() { return buff; }
  const JBuffer& buffer() const { return buff; }

  void setBorder(int w);
  void setFrame(int f);
  void setNeedRedraw();
  void nowVisible();

  virtual void drawToBuffer() { }
};



} } // namespace jude::fe

#endif // _JUDE_FE_BUFFERWIDGET_H

