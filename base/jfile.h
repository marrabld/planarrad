
// jfile.h

#ifndef _JUDE_BASE_FILE_H
#define _JUDE_BASE_FILE_H

#include "base/jtypes.h"
#include "base/jiodevice.h"
#include "base/jstring.h"

#include <stdio.h>

namespace jude {
namespace base {



class JFile : public JIODevice {

  JString filepath;
  FILE* file;

  int64 file_size;

public:

  JFile(const JString& name);
  virtual ~JFile();

  const JString& name() const { return filepath; }

  int64 size() const { return file_size; }

  bool open(int mode);
  void close();

  int32 readBlock(char *data, uns32 len);
  int32 writeBlock(const char *data, uns32 len);

  int32 writeBlockUntilZero(const char *data);

  int getch();
  int putch(int ch);
 
  //int32 readDouble(double &v);
  //int32 writeDouble(double v);
  //int32 readInt32(int32 &v);
  //int32 writeInt32(int32 v);

  void flush();
 
  bool atEnd() const;

  static bool binaryCompare(const JString& fp1, const JString& fp2);
  static bool binaryCopy(const JString& fp1, const JString& fp2);
};



} } // namespace jude::base

#endif // _JUDE_BASE_FILE_H

