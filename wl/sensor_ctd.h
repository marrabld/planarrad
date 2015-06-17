
#ifndef _JUDE_WL_SENSOR_CTD
#define _JUDE_WL_SENSOR_CTD

#include "wl/sensor_text.h"
#include "wl/record_ctd.h"

namespace jude {
namespace wl {



// class to describe a CTD sensor

class Sensor_CTD : public Sensor_TEXT {

public:

  Sensor_CTD(int ci) : Sensor_TEXT(ci) { }

    /*! \brief Returns the type of this sensor, "CTD". */
  const char* typeText() const { return "CTD"; }
  
  bool readDeviceFile(const JString& filepath);

  Record_CTD* createRecord() const { return new Record_CTD(this); }
};



} } // namespace jude::wl

#endif // _JUDE_WL_SENSOR_CTD

