
// jtextstream.h

#include "jtextstream.h"
#include <stdio.h>
#include <stdarg.h>

using namespace jude::base;


enum { BUFFER_LENGTH = 1024 };
static char buffer[BUFFER_LENGTH+1];


//const int JTextStream::scientific = 1;
//const int JTextStream::fixed = 2;


// JTextStream::(constructor)

JTextStream::JTextStream() : 
  dev(0), 
  prec(4), 
  flags(scientific) { }



// JTextStream::(constructor)

JTextStream::JTextStream(JIODevice* d) : 
  dev(d), 
  prec(4), 
  flags(scientific) { }



// JTextStream::atEnd()

bool JTextStream::atEnd() const {
  if (!dev) return true;
  return dev->atEnd();
}



// JTextStream::setf()

int JTextStream::setf(uns32 f) { 
  flags = flags | f;
  return flags;
}



// JTextStream::unsetf()

int JTextStream::unsetf(uns32 f) {
  flags = flags & (~f); 
  return flags;
}



// JTextStream& operator<<()

JTextStream& JTextStream::operator<<(int v) {
  if (!dev) return *this;
  JString s = JString::number(v);
  dev->writeBlock(s.latin1(), s.length());
  return *this;
}



// JTextStream& operator<<()
/*! \todo This casts \a v to int, should be fixed. */

JTextStream& JTextStream::operator<<(unsigned int v) {
  if (!dev) return *this;
  JString s = JString::number((int)v);
  dev->writeBlock(s.latin1(), s.length());
  return *this;
}



// JTextStream& operator<<()

JTextStream& JTextStream::operator<<(int64 v) {
  if (!dev) return *this;
  JString s = JString::number(v);
  dev->writeBlock(s.latin1(), s.length());
  return *this;
}



// JTextStream::operator << double

JTextStream& JTextStream::operator<<(double d) {
  if (!dev) return *this;
  JString s;
  if ((flags & scientific)) s = JString::number(d,'e',prec);
  else if ((flags & fixed)) s = JString::number(d,'f',prec);
  else s = JString::number(d,'g',prec);
  dev->writeBlock(s.latin1(), s.length());
  return *this;
}



// JTextStream::operator << JString

JTextStream& JTextStream::operator<<(const JString& s) {
  if (!dev) return *this;
  dev->writeBlock(s.latin1(), s.length());
  return *this;
}



// JTextStream::operator << JString

JTextStream& JTextStream::operator<<(const char* s) {
  if (!dev) return *this;
  dev->writeBlockUntilZero(s);
  return *this;
}



// JTextStream::printf()

void JTextStream::printf(const char* message, ...) {

  if (!dev) return;
  
  va_list arg;
  va_start(arg, message);

 // build text string
#ifdef WIN32
  vsprintf(buffer, message, arg);
#else
  vsnprintf(buffer, BUFFER_LENGTH, message, arg);
#endif

  dev->writeBlockUntilZero(buffer);
  va_end(arg);
}



// JTextStream::readLine()

JString JTextStream::readLine() {
 
  if (!dev) return JString("");

    // FIX ME! 
  const static int bufflen = 1024;
  static char buffer[bufflen+1];
  buffer[bufflen] = 0;

  JString s;
  char ch = dev->getch();
  int bp = 0;
  while (ch!=-1 && ch!='\r' && ch!='\n') {
    buffer[bp++] = ch;
    if (bp==bufflen) { s.append(buffer); bp=0; }
    ch = dev->getch();
  }
  s.append(JString(buffer,bp));

    // get the extra end of line character form windows type files
  if (ch=='\r') ch = dev->getch();

  return s;
}

