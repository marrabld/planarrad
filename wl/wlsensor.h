
#ifndef _JUDE_WL_WLSENSOR
#define _JUDE_WL_WLSENSOR

#include "recordsrc.h"

namespace jude {
namespace wl {

class Record;



// base class for all sensors

class WLSensor {

  char* name_text;

public:

  enum { ACS, CTD, ECOBB3 };

  static WLSensor* create(int t);

  WLSensor();
  virtual ~WLSensor() { }

  virtual int type() const = 0;

  const char* nameText() const { return name_text; }
  void setNameText(const char* txt);

  virtual const char* columnHead(int) const { return 0; }

  virtual Record* createRecord() const = 0;

  virtual bool readDeviceFile(const char* filepath) = 0;
};



} } // namespace jude::wl

#endif // _JUDE_WL_WLSENSOR

