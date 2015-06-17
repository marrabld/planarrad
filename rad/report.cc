
#include "rad/report.h"

using namespace jude::rad;



Report::Report() :
  major_sep(", "),
  sub_sep(", "),
  file(0) { }



Report::~Report() {
  closeFile();
}



bool Report::openFile(const JString& filepath) {
    // open the file
  file = new JFile(filepath.latin1());
    // quit if couldn't allocate
  if (!file) return false;
    // check opened ok
  if (!file->open(JIO_WriteOnly)) { delete file; return false; }
    // attach text stream to the file
  str.setDevice(file);

  return true;
}



void Report::closeFile() {
  if (!file) return;
  file->close();
  delete file;
  file = 0;
  str.setDevice(0);
}

