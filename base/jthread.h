
// jthread.h

#ifndef _JUDE_BASE_THREAD_H
#define _JUDE_BASE_THREAD_H


#ifndef WIN32
#include "base/jthread_linux.h"
#else
#include "base/jthread_win32.h"
#endif


#endif // _JUDE_BASE_THREAD_H

