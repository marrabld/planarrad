
// filedesc.cc

#include "file/filedesc.h"
#include "base/jfile.h"
#include "base/jtextstream.h"

using namespace jude::file;
using namespace jude::base;



// FileDesc::fileCanBeRead()
// checks if the file can be opened for read

bool FileDesc::fileCanBeRead() const {
  JFile f(filename);
  if (!f.open(JIO_ReadOnly)) return false;
  f.close();
  return true;
}



// FileDesc::writeHeader()

void FileDesc::writeHeader(JTextStream& str) const {

  JListIterator<const JString> iter(hdr_list);

  while (iter.current()) {
    str << "# " << *iter.current() << "\n";
    iter.next();
  }
}



// operator>>

JDataStream& jude::file::operator>>(JDataStream& str, FileDesc& fd) {
  str >> fd.filename;
  str >> fd.is_text_file;
  //printf("read %s\n",fd.path().latin1());
  return str;
}



// operator<<

JDataStream& jude::file::operator<<(JDataStream& str, const FileDesc& fd) {
  str << fd.filename;
  str << fd.is_text_file;
  //printf("write %s\n",fd.path().latin1());
  return str;
}

