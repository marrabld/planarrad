
// jfile.cc

#include "jfile.h"
#include <string.h>

using namespace jude::base;



// note the b's are required on win32 platforms

static const char* IO_ReadOnly_Text = "rb";
static const char* IO_WriteOnly_Text = "wb";
static const char* IO_ReadWrite_Text = "rb+";
static const char* IO_AppendOnly_Text = "ab";
static const char* IO_ReadAppend_Text = "ab+";



// JFile::(constructor)

JFile::JFile(const JString& name) :
  filepath(name),
  file(0),
  file_size(-1) { }



// JFile::(destructor)

JFile::~JFile() {
  close();
}



// JFile::open()

bool JFile::open(int mode) {
  const char* mode_text = IO_ReadOnly_Text;
  switch (mode) {
    case JIO_WriteOnly : { mode_text = IO_WriteOnly_Text; break; }
    case JIO_ReadWrite : { mode_text = IO_ReadWrite_Text; break; }
    case JIO_AppendOnly : { mode_text = IO_AppendOnly_Text; break; }
    case JIO_ReadAppend : { mode_text = IO_ReadAppend_Text; break; }
  }
  file = fopen(filepath.latin1(), mode_text);

  if (file) {
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);
  }

  return (file!=0);
}



// JFile::close()

void JFile::close() {
  if (!file) return; 
  fclose(file); 
  file=0;
}



// JFile::readBlock()

int32 JFile::readBlock(char *data, uns32 len) {
  if (!file) return 0;
  return fread(data, 1, len, file);
}



// JFile::writeBlock()

int32 JFile::writeBlock(const char *data, uns32 len) {
  if (!file) return 0;
  return fwrite(data, 1, len, file);
}



// JFile::writeBlockUntilZero()

int32 JFile::writeBlockUntilZero(const char *data) { 
  if (!file) return 0;
  int len = strlen(data);
  return fwrite(data, 1, len, file);
}



// JFile::getch()

int JFile::getch() {
  if (!file) return -1;
  int r = fgetc(file);
  return (r==EOF) ? -1 : r;
}



// JFile::putch()

int JFile::putch(int ch) {
  if (!file) return -1;
  int r = fputc(ch,file);
  return (r==EOF) ? -1 : r;
}



// JFile::flush()

void JFile::flush() {
  fflush(file);
}
 


// JFile::atEnd()

bool JFile::atEnd() const {
  if (!file) return true;
  return (feof(file)!=0);
}



/*! Returns \c true of both files can be opened and are byte-for-byte identical. */

bool JFile::binaryCompare(const JString& fp1, const JString& fp2) {

  JFile f1(fp1);
  JFile f2(fp2);

  bool ret_val = true;

  if (!f1.open(JIO_ReadOnly)) ret_val = false;
  if (!f2.open(JIO_ReadOnly)) ret_val = false;

  int v1,v2;

  if (ret_val) {
    
    do {
      v1 = f1.getch();
      v2 = f2.getch();
    } while (v1 >= 0 && v2 >= 0 && v2 == v1);

    if (!(v1 == v2 && v1 < 0)) ret_val = false;
  }

  f1.close();
  f2.close();

  return ret_val;
}



/*! Copies file \a src_fp to \a targ_fp and returns \c true if went ok. */

bool JFile::binaryCopy(const JString& src_fp, const JString& targ_fp) {

  JFile f1(src_fp);
  JFile f2(targ_fp);

  bool ret_val = true;

  if (!f1.open(JIO_ReadOnly)) ret_val = false;
  if (!f2.open(JIO_WriteOnly)) ret_val = false;

  if (ret_val) {

    while (!f1.atEnd()) {
      int v1 = f1.getch(); 
      if (v1 >= 0) f2.putch(v1); 
    }
  }

  f1.close();
  f2.close();

  return ret_val;
}

