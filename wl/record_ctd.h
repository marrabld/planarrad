
#ifndef _JUDE_WL_RECORD_CTD_H
#define _JUDE_WL_RECORD_CTD_H

#include "record_text.h"

namespace jude {
namespace wl {


class Sensor_CTD;



class Record_CTD : public Record_TEXT {

protected:

  bool buildRecord(int& final_pos);

public:

  Record_CTD(const Sensor_CTD* s);
  ~Record_CTD();

  const JString tempertureString() const { return itemToString(0); }
  const JString conductivityString() const { return itemToString(1); }
  const JString pressureString() const { return itemToString(2); }
  const JString salinityString() const { return itemToString(3); }

  double temperature() const { return itemToString(0).toDouble(); }
  double conductivity() const { return itemToString(1).toDouble(); }
  double pressure() const { return itemToString(2).toDouble(); }
  double salinity() const { return itemToString(3).toDouble(); }
 
  void writeProcessedHeader(JTextStream& str) const;

  int processedColumnsCount() const;
  const JString processedHeader(int i) const;

  const char* textDisplayLine(int i) const;
};



} } // namespace jude::wl

#endif // _JUDE_WL_RECORD_CTD_H

