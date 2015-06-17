
#include "jiodevicestore.h"

using namespace jude::base;


void JIODeviceStore::clear() {
  curr_text = "";
  data_array.resize(0);
}



int32 JIODeviceStore::writeBlock(const char *data, uns32 len) {
  data_array.reserve(data_array.size() + len);
  for (unsigned int i=0; i<len; i++) data_array.push_back(*data++);
  return len;
}



int32 JIODeviceStore::writeBlockUntilZero(const char *data) {
  int count = 0;
  while (*data!=0) { data_array.push_back(*data++); count++; }
  return count;
}



int JIODeviceStore::putch(int ch) {
  data_array.push_back(ch);
  return 1;
}



const JString& JIODeviceStore::currentText() const {
  if (curr_text.length() != data_array.size()) curr_text = JString(&data_array[0], data_array.size());
  return curr_text;
}

