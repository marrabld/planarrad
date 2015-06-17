
// fileinfo.h

#ifndef _JUDE_FILE_FILEINFO_H
#define _JUDE_FILE_FILEINFO_H

#include "base/jtypes.h"

using namespace jude::base;

namespace jude {
namespace file {



class FileInfo {

  uns32 saved_version;

  JString notes_text;

public:

  FileInfo() : saved_version(0xffffffff) { }

  uns32 savedVersion() const { return saved_version; }
  void setSavedVersion(uns32 v) { saved_version = v; }

  const JString& notesText() const { return notes_text; }
  void setNotesText(const JString& s) { notes_text = s; }
};



} } // namespace jude::file

#endif // _JUDE_FILE_FILEINFO_H

