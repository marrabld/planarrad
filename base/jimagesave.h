
// jimagesave.h

#ifndef _JUDE_BASE_JIMAGESAVE_H
#define _JUDE_BASE_JIMAGESAVE_H

#include "base/jbuffer.h"
#include "base/jstring.h"

namespace jude { 
namespace base {



/*! \brief Used to save the contents of a JBuffer as an image file. */

class JImageSave {

public:

  static bool savePPM(const JBuffer& buff, const JString& filepath);
};



} } // namespace jude::base

#endif // _JUDE_BASE_JIMAGESAVE_H

