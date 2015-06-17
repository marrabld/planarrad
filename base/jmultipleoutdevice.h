
// jmultipleoutdevice.h

#ifndef _JUDE_BASE_MULTIPLEOUTDEVICE_H
#define _JUDE_BASE_MULTIPLEOUTDEVICE_H

#include "jiodevice.h"
#include "jlist.h"

namespace jude {
namespace base {



/*! 
\brief Acts like a splitter for JIODevice. Enables redirecting of a single output stream to multiple devices.
*/

class JMultipleOutDevice : public JIODevice {

  JList<JIODevice> dev_list;

public:

  JMultipleOutDevice() { dev_list.setAutoDelete(false); }

  void addDevice(JIODevice* d) { dev_list.append(d); }

    // these probably out to be nops
  virtual bool open(int mode) { 
    bool f = true;
    JIODevice* dev = dev_list.first();
    while (dev) {
      f = f && dev->open(mode);
      dev = dev_list.next(); 
    }  
    return f; 
  }

  virtual void close() { 
    JIODevice* dev = dev_list.first();
    while (dev) {
      dev->close();
      dev = dev_list.next(); 
    }  
  }

  virtual int32 readBlock(char *data, uns32 len) { /* can't read */ return 0; }

  virtual int32 writeBlock(const char *data, uns32 len) {
    int32 v = 0;
    JIODevice* dev = dev_list.first();
    while (dev) {
      v = dev->writeBlock(data, len);
      dev = dev_list.next(); 
    }  
    return v;
  }

  virtual int32 writeBlockUntilZero(const char *data) {
    int32 v = 0;
    JIODevice* dev = dev_list.first();
    while (dev) {
      v = dev->writeBlockUntilZero(data);
      dev = dev_list.next(); 
    }
    return v;
  }

  virtual int getch() { return 0; }

  virtual int putch(int ch) {
    int v = 0;
    JIODevice* dev = dev_list.first();
    while (dev) {
      dev->putch(ch);
      dev = dev_list.next(); 
    }  
    return v;
  }

  virtual void flush() {
    JIODevice* dev = dev_list.first();
    while (dev) {
      dev->flush();
      dev = dev_list.next(); 
    }  
  }

  virtual void cursorUp(int n) { 
    JIODevice* dev = dev_list.first();
    while (dev) {
      dev->cursorUp(n);
      dev = dev_list.next(); 
    }  
  }

  virtual bool atEnd() const { return false; }
};



} } // namespace jude::base

#endif // _JUDE_BASE_MULTIPLEOUTDEVICE_H


