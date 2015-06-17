
// jbufferedwidget.h
// part of jude/qt
// (c) J Hedley 2003

#ifndef _JUDE_BUFFEREDWIDGET_H
#define _JUDE_BUFFEREDWIDGET_H

#include "jwidget.h"
#include "jbuffer.h"



// JBufferedWidget

class JBufferedWidget : public JWidget {

  JBuffer* _buffer;
 
protected:

  int frame_width;
  int border_width;
  bool need_redraw;

  virtual void redrawRequest();

public:

  JBufferedWidget(JWidget *parent);
  JBufferedWidget(JBuffer& b, JWidget *parent);
  ~JBufferedWidget() { }

  virtual void resize(int w, int h);
 
  JBuffer* buffer() { return _buffer; }
  const JBuffer* buffer() const { return _buffer; }
  void setBuffer(JBuffer* b);

  void setBorder(int w);
  void setFrame(int f);
  void setNeedRedraw();
  void nowVisible();
  void paintEvent(JPaintEvent*);

  virtual void drawToBuffer() { }                     // draw to the buffer
  virtual void exportPostscript(const JString&) { }   // draw to a postscript file
};


#endif // _JUDE_BUFFEREDWIDGET_H

