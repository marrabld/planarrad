
// bytestream.h

#ifndef _JUDE_WL_BYTESTREAM_H
#define _JUDE_WL_BYTESTREAM_H

#include "base/jtypes.h"

using namespace jude::base;

namespace jude {
namespace wl {



class ByteStream {

public:

  virtual ~ByteStream() { }

  virtual uns8 read() = 0;
  virtual bool pastEnd() const = 0;
};



} } // namespace jude::wl

#endif // _JUDE_WL_BYTESTREAM_H

