
#ifndef _JUDE_WL_SENSOR
#define _JUDE_WL_SENSOR

//#include "wl/recordsrc.h"
#include "wl/channel.h"
#include "base/jstring.h"
#include "base/jparser.h"

using namespace jude::base;

namespace jude {
namespace wl {

class Record;



// base class for all sensors

class Sensor {

  JString name_str;
  int chan_index;

protected:

  void setNameText(const JString& name_str);

public:

  Sensor(int ci);
  Sensor(const JString& name, int ci);
  virtual ~Sensor() { }

    /*! \brief Returns the name of the type of sensor. */
  virtual const char* typeText() const = 0;
    /*! \brief Returns the name of this specific sensor. */
  const JString& nameText() const { return name_str; }

    // use this - typeText() depreceated
  const JString sensorType() { return JString(typeText()); }

  virtual const JString& deviceFilepath() const { return JString::null; }
  virtual bool hasDeviceFile() const { return false; }

  int channelIndex() const { return chan_index; }
  void setChannelIndex(int ci) { chan_index = ci; }

    /*! \brief Interface to a function that a sensor can provide to return how many columns to output in a spreadsheet. */
  virtual int outputColumnCount() const { return 0; }
    /*! \brief Interface to a function that a sensor can provide to return the header text for each sporeadsheet output column. */
  virtual const JString outputColumnHeadText(int) const { return ""; }

    /*! \brief Interface to a function that a sensor must provide to create a record. */
  virtual Record* createRecord() const = 0;

  virtual bool loadProcessed(const JParser& prm, Channel& chan, int chan_index) { return true; }

  virtual void prepareForMerge(const JParser& prm, int chan_index) { }

  virtual void stopProcessing() { }
 
  virtual void writeHeader(JDataStream& str) const { }
 
  virtual const char* fileExtension() const { return ""; }

  virtual bool readDeviceFile(const JString& filepath) = 0;

  virtual double pctRecordsProcessed() const { return 100; }
};



} } // namespace jude::wl

#endif // _JUDE_WL_SENSOR

