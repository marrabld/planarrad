
#ifndef _JUDE_WL_ARCHIVEFILE
#define _JUDE_WL_ARCHIVEFILE

#include "base/jlist.h"
#include "base/jstring.h"
#include "base/jtextstream.h"
#include "wl/channel.h"
#include "wl/sensorconfig.h"
#include "wl/archivepacket.h"

using namespace jude::base;

namespace jude {
namespace wl {



class ArchiveFile {

  const SensorConfig& config;

  JString archive_fp;

  JList<ArchivePacket> ap_list;
  JList<Record> record_list;

  Channel channel_array[HARDWARE_CHANNEL_COUNT];

  Channel channel_merge;

  int64 logger_start_time_msec; // logger time of first packet (always in msec)
  int64 logger_end_time_msec;   // logger time of last packet  (msec)

    // will be the total number of packets in the file
  int bad_packet_count;

  int max_channel_index;

  int file_size_bytes;
  int bytes_read_count;

  bool stop_processing;

  //static int error_code;
  //enum { NO_ERROR=0, FILE_NOT_FOUND };

public:

    /*! \brief Constructor. */
  ArchiveFile(const SensorConfig& config);
    /*! \brief Destructor. */
  ~ArchiveFile();

  const JString& filepath() const { return archive_fp; }

  void clearData();

  void stopProcessing() { stop_processing = true; }

  Channel& channel(int i) { return channel_array[i]; }
  const Channel& channel(int i) const { return channel_array[i]; }

  const Channel& channelMerge() const { return channel_merge; }

  int maxChannelIndex() const { return max_channel_index; }

  bool loadProcessed(int i, const JParser& prm);
  //bool loadProcessedEdAndLu(int i, const JParser& prm);
  //bool loadProcessedEs(int i, const JParser& prm);
  bool processOrLoadProcessed(const JParser& prm);

  double pctReadCompleted() const { return (100 * ((double) bytes_read_count)) / file_size_bytes; } 

    /*! \brief Returns the total number of packets in the archive file. */
  int packetCount() const { return ap_list.count(); }

  ArchivePacket* archivePacket(int i) { return ap_list.at(i); }

  int recordCount() const { return record_list.count(); }

  Record* record(int i) { return record_list.at(i); }

  const JList<Record>& recordList() const { return record_list; }

    /*! \brief Reads packet data from file \a filepath, stores the data if \a preview_only is false. Returns \c true if went ok. */
  bool readPackets(const JString& filename, bool preview_only);

  void doMerge(const JParser& prm, int merge_chan, bool* inc_flags);
  bool mergeDone() const { return (channel_merge.recordCount() > 0); }

    /*! \brief Returns the logger time of the first packet in milliseconds. */
  int64 loggerStartTimeMsec() const { return logger_start_time_msec; }
    /*! \brief Returns the logger time of the last packet in milliseconds. */
  int64 loggerEndTimeMsec() const { return logger_end_time_msec; }

    /*! \brief Displays summary statistics to the text stream \a str. */
  void display(JTextStream& str) const;
};



} } // namespace jude::wl

#endif // _JUDE_WL_ARCHIVEFILE


