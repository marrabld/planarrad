
// jenvvar.h

#ifndef _JUDE_BASE_JENVVAR_H
#define _JUDE_BASE_JENVVAR_H

#include <stdlib.h>

namespace jude {
namespace base {



inline JString getEnvironmentalVariable(const JString& env_var) { 
  return JString(getenv(env_var.latin1())); 
}



} } // namespace jude::base

#endif // _JUDE_BASE_JENVVAR_H

