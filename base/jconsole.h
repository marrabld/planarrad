
// jconsole.h

#ifndef _JUDE_BASE_CONSOLE_H
#define _JUDE_BASE_CONSOLE_H

#include "base/jiodevice.h"
#include <stdio.h>

namespace jude {
namespace base {



/*! \brief A text console output device.

All console output is simply directed to the standard library printf() function. 
Direct use of JConsole is now depreceated, writing to jlog::ls is preferred (which in turn uses JConsole).
*/

class JConsole : public JIODevice {

public:

  bool open(int mode) { return true; }
  void close() { }

  int32 readBlock(char *data, uns32 len) { return -1; }
  int32 writeBlock(const char *data, uns32 len);

  int32 writeBlockUntilZero(const char *data);

  int32 readDouble(double &v) { return -1; }
  int32 writeDouble(double v) { return -1; }
 
  int32 readInt32(int32 &v) { return -1; }
  int32 writeInt32(int32 v) { return -1; }
 
  int getch() { return -1; }
  int putch(int ch) { printf("%c",ch); return ch; }

  void flush();

  bool atEnd() const { return false; }

  void cursorUp(int n);

  static JConsole standard;
};



} } // namespace jude::base

#endif // _JUDE_BASE_CONSOLE_H

