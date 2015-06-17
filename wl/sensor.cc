
#include "sensor.h"

using namespace jude::wl;




Sensor::Sensor(int ci) : 
  name_str(""), chan_index(ci) { }



Sensor::Sensor(const JString& name_str, int ci) : 
  name_str(name_str), chan_index(ci) { }



void Sensor::setNameText(const JString& s) {
  name_str = s;
}

