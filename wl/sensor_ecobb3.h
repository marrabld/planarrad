
#ifndef _JUDE_WL_SENSOR_ECOBB3_H
#define _JUDE_WL_SENSOR_ECOBB3_H

#include "sensor_text.h"

namespace jude {
namespace wl {



// class to describe an ECO-BB3 sensor

class Sensor_ECOBB3 : public Sensor_TEXT {

public:

  Sensor_ECOBB3(int ci) : Sensor_TEXT(ci) { }

  //  int type() const { return ECOBB3; }
  const char* typeText() const { return "ECOBB3"; }

  bool readDeviceFile(const JString& filepath) { return true; }

  Record* createRecord() const;
};



} } // namespace jude::wl

#endif // _JUDE_WL_SENSOR_ECOBB3_H

