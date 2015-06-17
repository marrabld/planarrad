
#ifndef _RECORD_MERGE_H_JDH
#define _RECORD_MERGE_H_JDH

#include "wl/record.h"
#include "wl/sensorconfig.h"
#include "base/jlist.h"

using namespace jude::base;

namespace jude {
namespace wl {


class Record_MERGE : public Record {

  const Record** record_array;
  long* time_diff;

public:

  Record_MERGE();
  ~Record_MERGE();

  const Record* record(int i) const;
  void setRecord(int i, const Record* r, long td);

  void appendBytes(const uns8*&, int&) { }

  const char* textDisplayLine(int i) const;

  void writeBytes(JDataStream& str) const { }

  void writeProcessedHeader(JTextStream& str) const;
  void writeProcessed(JTextStream& str) const;

  int processedColumnsCount() const;
  const JString processedHeader(int i) const;
  const JString processed(int i) const;
 
  void display() const;
};



} } // namespace jude::wl

#endif // _RECORD_MERGE_JDH

