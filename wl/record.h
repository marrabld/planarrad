
#ifndef _JUDE_WL_RECORD_H
#define _JUDE_WL_RECORD_H

#include "base/jtypes.h"
#include "base/jdatastream.h"
#include "base/jtextstream.h"
#include "wl/blockstream.h"
#include "wl/bytestream.h"

using namespace jude::base;

namespace jude {
namespace wl {


class Sensor;


class Record {

protected:

  const Sensor* sens;

  BlockStream::Block* first_block;
  int total_length;

    // sequence in channel
  int record_pos;

  bool is_complete;
  bool has_problem;
  bool is_excluded;

private: 

    // the actual time as recorded in the logger, in msec
  long logger_time_msec;
    // the time since the first record in this channel, in msec
  long time_since_start_msec; 

protected:

  void newDataBlock(const uns8* data, int length);
  void clearDataBlocks();

  void setIsComplete(bool f) { is_complete = f; }
  void setHasProblem(bool f) { has_problem = f; }

public:

  Record(const Sensor* s=0);
  virtual ~Record();

  virtual const Sensor* sensor() const { return sens; }

  virtual bool isProcessed() const { return true; }

  virtual const JString typeText() const;

  void setRecordPosition(int p) { record_pos = p; }
  int recordPosition() const { return record_pos; }

    /*! \brief Returns \c true of the complete record was read. */
  bool isComplete() const { return is_complete; }
    /*! \brief Returns \c true if any internal consistency check of the record failed, e.g. a check sum. */
  bool hasProblem() const { return has_problem; }

    /*! \brief Returns \c true of this record is flagged to be excluded from processing and output. */
  bool isExcluded() const { return is_excluded; }
    /*! \brief Flags this record to be excluded from processing and output if \a f is \c true. */
  void setIsExcluded(bool f) { is_excluded = f; }

    /*! \brief Returns the actual time as recorded in the logger, in msec. */
  long loggerTimeMsec() const { return logger_time_msec; }
    /*! \brief Returns the time since the first logger packet, in msec. */
  long timeSinceStartMsec() const { return time_since_start_msec; }

    /*! \brief Sets the actual time of this record as \a t and time since the starts as \a t - \a start, in msec. */
  void setLoggerTimeMsec(long start, long t) { 
    logger_time_msec = t;
    time_since_start_msec = t - start; 
  }

  virtual bool process() { return true; } 

  virtual void clearProcessed() { }

  virtual void appendBytes(const uns8*&, int&) = 0;

  virtual void writeBytes(JDataStream& str) const = 0;

  virtual void writeProcessedHeader(JTextStream& str) const { }
  virtual void writeProcessed(JTextStream& str) const = 0;

  virtual int processedColumnsCount() const { return 1; }
  virtual const JString processed(int i) const { return "X"; }
  virtual const JString processedHeader(int i) const { return "X"; }
 
  virtual const char* textDisplayLine(int i) const = 0;
};



} } // namespace jude::wl

#endif // _JUDE_WL_RECORD_H

