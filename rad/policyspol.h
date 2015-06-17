
// policyspol.h

#ifndef _JUDE_RAD_POLICYSPOL_H
#define _JUDE_RAD_POLICYSPOL_H

#include "rad/radtypes.h"
#include "rad/stokes.h"
#include "base/jstring.h"
#include "base/jgenericfactory.h"

namespace jude {
namespace rad {



class PolicySPol {

public:

    // depreceated - use data_type
  typedef stokes_vector<radtype> elem_type;

  typedef stokes_vector<radtype> data_type;
  typedef stokes_matrix<radtype> func_type;

  typedef stokes_vector<doubleLONG> long_data_type;

  static const JString staticFactoryID() { return staticIdOf<elem_type>(); }

  static JString reportBandHeader(const JString& sub_sep, int i) { 
    i++;
    JString s(JString("I") + JString::number(i));
    s += sub_sep + "Q" + JString::number(i);
    s += sub_sep + "U" + JString::number(i);
    s += sub_sep + "V" + JString::number(i);
    return s;
  }

  static void logWrite(JTextStream& str, const data_type& e, const JString& sub_sep) { 
    str << e.I() << sub_sep << e.Q() << sub_sep << e.U() << sub_sep << e.V(); 
  }
};



} } // namespace jude::rad

#endif // _JUDE_RAD_POLICYSPOL_H

