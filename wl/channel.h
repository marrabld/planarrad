
#ifndef _JUDE_WL_CHANNEL_H
#define _JUDE_WL_CHANNEL_H

#include "wl/recordlist.h"

namespace jude {
namespace wl {



// Channel

class Channel {

    // the list of records in the order they were read in
  RecordList record_list;

    // the current record writing to (last in the list)
  Record* curr_rec;

    // accumulated bytes of data that have been read in this channel
  int channel_byte_count;
 
  bool stop_processing;

  int processed_done_count; 
  bool is_completely_processed;

public:

  Channel();

  double pctProcessedDone() const { return (record_list.count() > 0) ? (100.0*processed_done_count) / record_list.count() : 100; }
  bool isCompletelyProcessed() const { return is_completely_processed; }

  void clear();

  bool process(); 

  void clearAllProcessed();

  Record* currentRecord() { return curr_rec; }
  void appendRecord(Record *r) { record_list.append(r); curr_rec=r; }
 
  const Record* record(int i) const { return record_list.at(i); }
 
  RecordList& recordList() { return record_list; }
  const RecordList& recordList() const { return record_list; }

  int recordCount() const { return record_list.count(); }

  int byteCount() const { return channel_byte_count; }

  void addToByteCount(int n) { channel_byte_count += n; }

  void stopProcessing() { stop_processing = true; }

    /*! \brief Writes all of the raw record bytes in this channel to \a str. */
  void writeBytes(JDataStream& str) const;
 
    /*! \brief Writes out processed records in this channel to \a str. */
  void writeProcessed(JTextStream& str) const;
};



} } // namespace jude::wl

#endif // _JUDE_WL_CHANNEL_H

