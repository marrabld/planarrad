
// policys.h

#ifndef _JUDE_RAD_POLICYS_H
#define _JUDE_RAD_POLICYS_H

#include "rad/radtypes.h"
#include "base/jstring.h"
#include "base/jgenericfactory.h"

namespace jude {
namespace rad {



class PolicyS {

public:

    // depreceated - use data_type
  typedef radtype elem_type;

  typedef radtype data_type;
  typedef radtype func_type;

  typedef doubleLONG long_data_type;

  static const JString staticFactoryID() { return staticIdOf<elem_type>(); }

  static JString reportBandHeader(const JString& sub_sep, int i) { 
    return JString("band") + JString::number(i+1); 
  }

  static void logWrite(JTextStream& str, const data_type& e, const JString& sub_sep) { 
    str << e;
  }
};



} } // namespace jude::rad

#endif // _JUDE_RAD_POLICYS_H

