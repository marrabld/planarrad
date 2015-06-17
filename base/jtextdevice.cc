
// jtextdevice.h

#include "jtextdevice.h"

using namespace jude::base;



bool JTextDevice::open(int mode) { 
  if (mode == JIO_WriteOnly) { s=""; return true; }
  if (mode == JIO_AppendOnly) { return true; }
  return false;
}



void JTextDevice::close() { 
  /* nop */ 
}



int32 JTextDevice::readBlock(char *data, uns32 len)  { 
  /* nop */ 
  return 0;
}




int32 JTextDevice::writeBlock(const char *data, uns32 len) {
  s.append(JString(data, len));
  return len;
}



int32 JTextDevice::writeBlockUntilZero(const char *data) {

    // count the number of characters
  const char* ptr = data;
  while (*ptr != 0) ptr++;
  int count = ptr - data;

  s.append(JString(data, count));

  return count;
}



int JTextDevice::getch() { 
  return 0; 
}



int JTextDevice::putch(int ch) {
  JChar c(ch);
  s.append(&c);
  return c;
}



void JTextDevice::flush()  { 
  /* nop */ 
}



bool JTextDevice::atEnd() const { 
  return false; 
}


