
// jdir.h

#ifndef _JUDE_BASE_JDIR_H
#define _JUDE_BASE_JDIR_H

#include "base/jlist.h"
#include "base/jstring.h"

namespace jude {
namespace base {



/*! \brief For reading the entires of a directory on the file system. */

class JDir {

  const JString dir_path;

public:

  JDir(const JString& p) : 
    dir_path(p) { }

  bool exists() const;

  bool mkdir() const;

  const JString& path() const { return dir_path; }

  void getEntryList(JList<JString>& entry_list) const;
};



} } // namespace jude::base;

#endif // _JUDE_BASE_JDIR_H

