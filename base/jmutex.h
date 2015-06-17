
// jmutex.h

#ifndef _JUDE_BASE_JMUTEX_H
#define _JUDE_BASE_JMUTEX_H


#ifndef WIN32
#include "base/jmutex_linux.h"
#else
#include "base/jmutex_win32.h"
#endif


#endif // _JUDE_BASE_JMUTEX_H

