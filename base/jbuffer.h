
// jbuffer.h

#ifndef _JUDE_BASE_JBUFFER_H
#define _JUDE_BASE_JBUFFER_H

#include "base/jtypes.h"
#include "base/jcolour.h"

namespace jude { 
namespace base {



// JBuffer

class JBuffer {

protected:

  int w;
  int h;
  uns32* data_ptr;
  bool has_alpha_buffer;

public:

  JBuffer();
  JBuffer(int w, int h);
  //JBuffer(int w, int h, uns32 *buff);
  virtual ~JBuffer();

  void setAlphaBuffer(bool enable);

  int width() const { return w; }
  int height() const { return h; }
  int bytesPerLine() const { return w << 2; }

  int minDimension() const { return (w<h) ? w : h; }

  //void setData(char* d) { data_ptr = (uns32*) d; }
  void setSize(int iw, int ih) { w=iw; h=ih; }
  uns32* data() const { return data_ptr; }
  uns8* bits() const { return (uns8*) data_ptr; }
  bool isNull() const;

  void fill(uns32 v);

  void shiftBlock(int xs, int ys, int xe, int ye, int dx, int dy);

  virtual bool requestResize(int nw, int nh);

    /*! \brief Writes a single pixel of colour \a col at position \a x, \a y. 
      Note this does not perform any bounds checking so caller must ensure \a x, \a y is within the buffer. */
  void writePixel(int x, int y, const JColour& col) { *(data_ptr + y*w + x) = col.rgba(); }

    /*! \brief Clears the entire buffer to colour \col. */
  void fill(const JColour& col) { fill(col.rgba()); }

  void fillRectangle(int32 x0, int32 y0, int32 x1, int32 y1, const JColour& col);
  void fillEllipse(int32 x0, int32 y0, int32 x1, int32 y1, const JColour& col);

    /*! \brief Draw a single pixel thickness line from \a x0,y0 to \a x1,y1 in colour \a col. */
  void drawLine(int32 x0, int32 y0, int32 x1, int32 y1, const JColour& col);

  void drawData_ARGB32(uns32* data_ptr, int x_size, int y_size, int sx, int sy, int dx, int dy, int aw, int ah);
};



} } // namespace jude::base

#endif // _JUDE_BASE_JBUFFER_H

