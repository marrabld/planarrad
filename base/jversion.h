
// jversion.h

#ifndef _JUDE_BASE_VERSION_H
#define _JUDE_BASE_VERSION_H

#include "jstring.h"

namespace jude {
namespace base {


// format is four blocks of 8 bits, should be saved as uns32
// xx << 24 - currently unused
// xx << 16 - major number
// xx << 8  - minor number
// xx << 0  - sub minor number

// changes which affect the structure of saved files should occur only in the minor or major version number
// bug-fixes and small updates etc. should be reflected in the sub-minor version number



#define JUDE_MAJOR_VERSION 2
#define JUDE_MINOR_VERSION 2
#define JUDE_SUBMINOR_VERSION 0
#define JUDE_RELEASE_VERSION alpha

#define JUDE_VERSION ((JUDE_MAJOR_VERSION << 24) + (JUDE_MINOR_VERSION << 16) + (JUDE_SUBMINOR_VERSION << 8))

#define VERSION_CODE(major, minor, subminor) ((major << 24) + (minor << 16) + (subminor << 8))

#define MAJOR_VERSION(d) ((d >> 24) & 255)
#define MINOR_VERSION(d) ((d >> 16) & 255)
#define SUBMINOR_VERSION(d) ((d >> 8) & 255)

// these are needed to expand the macro values to s atring
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#define JUDE_RELEASE_STRING STRINGIZE(JUDE_MAJOR_VERSION) \
  "." STRINGIZE(JUDE_MINOR_VERSION) \
  "." STRINGIZE(JUDE_SUBMINOR_VERSION) \
  STRINGIZE(JUDE_RELEASE_VERSION)



} } // namespace jude::base

#endif // _JUDE_BASE_VERSION_H

