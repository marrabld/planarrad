
// filedesc.h

#ifndef _JUDE_FILE_FILEDESC_H
#define _JUDE_FILE_FILEDESC_H

#include "base/jstring.h"
#include "base/jlist.h"
#include "base/jdatastream.h"
#include "base/jtextstream.h"

using namespace jude::base;

namespace jude {
namespace file {



class FileDesc {

  JString filename;
  JList<const JString> hdr_list;
  bool is_text_file;

public:

  FileDesc() :
    filename(""),
    is_text_file(false) { }

  FileDesc(const FileDesc& fd) :
    filename(fd.filename),
    is_text_file(fd.is_text_file) { }

  FileDesc(const JString& fn) :
    filename(fn),
    is_text_file(false) { }

  FileDesc(const JString& fn, bool it) :
    filename(fn),
    is_text_file(it) { }

  FileDesc(JDataStream& str) {
    str >> (*this);
  }

    // deprecated
  void write(JDataStream& str) const { str << (*this); }
  void read(JDataStream& str) { str >> (*this); }
 
  bool isUndefined() const { return filename.isEmpty(); }

  bool fileCanBeRead() const;

  const JString& name() const { return filename; }  // depreceated
  const JString& path() const { return filename; }

  bool isTextFile() const { return is_text_file; }
  void setIsTextFile(bool f) { is_text_file = f; }

  bool isDefined() const { return !filename.isEmpty(); } 

  void appendToHeader(const JString& s) { hdr_list.append(&s); }
  void writeHeader(JTextStream& str) const;
 
  friend JDataStream& operator>>(JDataStream& str, FileDesc& fd);
  friend JDataStream& operator<<(JDataStream& str, const FileDesc& fd);
};

 
JDataStream& operator>>(JDataStream& str, FileDesc& fd);
JDataStream& operator<<(JDataStream& str, const FileDesc& fd);


} } // namespace jude::file

#endif // _JUDE_FILE_FILEDESC_H

