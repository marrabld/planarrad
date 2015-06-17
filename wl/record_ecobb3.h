
#ifndef _RECORD_ECOBB3_JDH
#define _RECORD_ECOBB3_JDH

#include "record_text.h"

namespace jude {
namespace wl {

class Sensor_ECOBB3;



class Record_ECOBB3 : public Record_TEXT {

protected:

  bool buildRecord(int& final_pos);

public:

  Record_ECOBB3(const Sensor_ECOBB3* s);
  ~Record_ECOBB3();

  const JString dataString() const { return itemToString(0); }
  const JString timeString() const { return itemToString(1); }
  int ref470() const { return itemToString(2).toInt(); }
  int sig470() const { return itemToString(3).toInt(); }
  int ref530() const { return itemToString(4).toInt(); }
  int sig530() const { return itemToString(5).toInt(); }
  int ref650() const { return itemToString(6).toInt(); }
  int sig650() const { return itemToString(7).toInt(); }
  int therm() const { return itemToString(8).toInt(); }

  void writeProcessedHeader(JTextStream& str) const;

  int processedColumnsCount() const;
  const JString processedHeader(int i) const;

  const char* textDisplayLine(int i) const;
};



} } // namespace jude::wl

#endif // _RECORD_ECOBB3_JDH

