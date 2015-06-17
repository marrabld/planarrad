
#include "file/filesetdesc.h"

using namespace jude::file;



// FileSetDesc::(constructor)

FileSetDesc::FileSetDesc() {
  fd_list.setAutoDelete(true); 
}



// FileSetDesc:(constructor)

FileSetDesc::FileSetDesc(const FileSetDesc& fs) {
  fd_list.setAutoDelete(true);

  const FileDesc* fd = fs.firstFile();
  while (fd) {
    appendFile(*fd);
    fd = fs.nextFile();
  }
} 



// FileSetDesc::(constructor)

FileSetDesc::FileSetDesc(const FileDesc& fd) {
  fd_list.append(new FileDesc(fd));
  fd_list.setAutoDelete(true);
}



// FileSetDesc::(constructor)

FileSetDesc::FileSetDesc(const JString& s) {
  fd_list.setAutoDelete(true);
  if (s.isEmpty()) return;
  int n = s.contains(',') + 1;
  for (int i=0;i<n;i++) fd_list.append(new FileDesc(s.section(JChar(','), i, i)));
}
 


// FileSetDesc::(constructor)

//FileSetDesc::FileSetDesc(JDataStream& str) {
//  fd_list.setAutoDelete(true);
// str >> (*this);
//}



// FileSetDesc::operator<<()
/*
JDataStream& jude::file::operator<<(JDataStream& str, const FileSetDesc& fs) {
  
  str << (int) fs.fd_list.count();

  const FileDesc* fd = fs.firstFile();
  while (fd) {
    str << (*fd);
    fd = fs.nextFile();
  }

  return str;
}
*/


// FileSetDesc::operator>>()
/*
JDataStream& jude::file::operator>>(JDataStream& str, FileSetDesc& fs) {

  int num;
  str >> num;

  fs.clear();

  for (int i=0;i<num;i++) fs.appendFile(FileDesc(str));

  return str;
}
*/

