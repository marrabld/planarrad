
#ifndef _JUDE_WL_RECORD_RAWDATA
#define _JUDE_WL_RECORD_RAWDATA

#include "record.h"

namespace jude {
namespace wl {

class Sensor_RawData;



class Record_RawData : public Record {

  uns8* packet_bytes;
  int byte_count;

public:

  Record_RawData(const Sensor_RawData* s);
  ~Record_RawData();

  void appendBytes(const uns8*& ptr, int& bytes_left);

  void writeBytes(JDataStream& str) const;

  void writeProcessedHeader(JTextStream& str) const;
  void writeProcessed(JTextStream& str) const;

  const char* textDisplayLine(int i) const;
};



} } // namespace jude::wl

#endif // _JUDE_WL_RECORD_RAWDATA

