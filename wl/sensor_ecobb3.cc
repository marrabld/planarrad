
#include "sensor_ecobb3.h"
#include "record_ecobb3.h"

using namespace jude::wl;


// Sensor_ECOBB3::readDeviceFile()

//bool Sensor_ECOBB3::readDeviceFile(const char* filename) {
//  return false;
//}



// Sensor_ECOBB3::createRecord()

jude::wl::Record* Sensor_ECOBB3::createRecord() const {
  return new Record_ECOBB3(this);
}

