
#ifndef _JUDE_WL_SENSOR_TEXT_H
#define _JUDE_WL_SENSOR_TEXT_H

#include "sensor.h"

namespace jude {
namespace wl {



// base class for all sensors

class Sensor_TEXT : public Sensor {

  int column_count;
  char** column_head;

  void deleteColumnData();

public:

  Sensor_TEXT(int ci);
  ~Sensor_TEXT();
  
  const char* fileExtension() const { return ".txt"; }

  int columnCount() const { return column_count; }
  void setColumnCount(int n);

  const JString columnHead(int i) const { return column_head[i]; }
  void setColumnHead(int i, const char* txt);

  bool readDeviceFile(const char* filepath);
};



} } // namespace jude::wl

#endif // _JUDE_WL_SENSOR_TEXT_H

