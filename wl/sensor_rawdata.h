
#ifndef _JUDE_WL_SENSOR_RAWDATA_H
#define _JUDE_WL_SENSOR_RAWDATA_H

#include "wl/sensor.h"
#include "wl/record_rawdata.h"

namespace jude {
namespace wl {



// base class for all sensors

class Sensor_RawData : public Sensor {

public:

  Sensor_RawData(int ci) : Sensor(ci) { }

  ~Sensor_RawData() { }

    /*! \brief Returns the type of this sensor, "RawData". */
  const char* typeText() const { return "RawData"; }
   
    /*! \brief Creates a record for this sensor. */
  Record_RawData* createRecord() const { return new Record_RawData(this); }

  bool readDeviceFile(const JString& filepath) { return true; }
};



} } // namespace jude::wl

#endif // _JUDE_WL_SENSOR_TEXT_H

