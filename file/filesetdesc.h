
// filesetdesc.h

#ifndef _JUDE_FILE_FILESETDESC_H
#define _JUDE_FILE_FILESETDESC_H

#include "file/filedesc.h"
#include "base/jlist.h"

namespace jude {
namespace file {



class FileSetDesc {

  JList<FileDesc> fd_list;

public:

  FileSetDesc();
  FileSetDesc(const FileSetDesc& fs); 
  FileSetDesc(const FileDesc& fd); 
  FileSetDesc(const JString& s);
  //FileSetDesc(JDataStream& str);

  int fileCount() const { return fd_list.count(); }
  void clear() { return fd_list.clear(); }
  const FileDesc* file(int i) const { return fd_list.at(i); }
  const FileDesc* firstFile() const { return fd_list.first(); }
  const FileDesc* nextFile() const { return fd_list.next(); }

  void appendFile(const FileDesc& fd) { fd_list.append(new FileDesc(fd)); }

  bool isUndefined() const { return (fd_list.count() == 0); }

    // deprecated
  //void write(JDataStream& str) const { str << (*this); }
  //void read(JDataStream& str) { str >> (*this); }

  //friend JDataStream& operator>>(JDataStream& str, FileSetDesc& fs);
  //friend JDataStream& operator<<(JDataStream& str, const FileSetDesc& fs);
};



} } // namespace jude::file

#endif // _JUDE_FILE_FILEDESC_H

