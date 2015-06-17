
// jconsole.cc

#include "jconsole.h"
#include <string.h>

#ifdef WIN32
#include "jwindows.h" 
#endif

using namespace jude::base;



/*! \brief The standard console. 

Outputing to this device is equivalent to using the standard printf() function.
Using jlog::ls and jlog::es is now preferred since these can be redirected more easily.
New code should not use JConsole::standard directly. */ 

JConsole JConsole::standard;



// JConsole::writeBlock()

int32 JConsole::writeBlock(const char *data, uns32 len) { 
  for (uns32 i=0;i<len;i++) printf("%c",data[i]); 
  return len; 
}



// JConsole::writeBlockUntilZero()

int32 JConsole::writeBlockUntilZero(const char *data) { 
  printf("%s",data); 
  return strlen(data);
}



#ifndef WIN32

void JConsole::cursorUp(int n) {
  printf("\033[%dA",n);
}

#else

void JConsole::cursorUp(int n) {
  HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO info;
  if (!GetConsoleScreenBufferInfo(h, &info)) return; 
  COORD pos = info.dwCursorPosition;
  pos.Y -= n;
  SetConsoleCursorPosition(h, pos); 
}

#endif



void JConsole::flush() {
  fflush(stdout);
}

