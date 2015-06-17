
#ifndef _JUDE_WL_SENSORCONFIG
#define _JUDE_WL_SENSORCONFIG

#include "wl/sensor.h"
#include "base/jparser.h"

namespace jude {
namespace wl {

enum { HARDWARE_CHANNEL_COUNT = 9 };



class SensorConfig {

  JString sensor_setup;
 
  Sensor* sensor_array[HARDWARE_CHANNEL_COUNT];

  bool status_ok;

  void deleteAll();

  void setToRawFrom(int start_index);

  void setSensor(int i, Sensor* s);

public:

  SensorConfig();
  SensorConfig(const JParser& prm);
  ~SensorConfig();

  bool statusOk() const { return status_ok; }

  void clearAllProcessed();

  static void clearParser(JParser& prm);

  bool buildFromParser(const JParser& prm);
  void writeToParser(JParser& prm) const;

  bool setup_raw(const JParser& prm);
  bool setup_fsf(const JParser& prm);
  bool setup_fsf_no_hocr(const JParser& prm);
  bool setup_custom(const JParser& prm);
 
  Sensor& sensor(int i) const { return *sensor_array[i]; }

  bool readSensorDeviceFiles(const JParser& prm);
};



} } // namespace jude::wl

#endif // _JUDE_WL_SENSORCONFIG

