
#include "wl/archivefile.h"
#include "wl/archivepacket.h"
#include "wl/channel.h"
#include "base/jfile.h"
#include "base/jlog.h"
#include "wl/record_merge.h"
#include <math.h>
#include <stdio.h>

using namespace jude::wl;

const bool debug = false;


//int ArchiveFile::error_code;



// ArchiveFile::(constructor)

ArchiveFile::ArchiveFile( const SensorConfig& config) : 
  config(config) { 

  ap_list.setAutoDelete(true);

    // this contains duplicate pointers to the records also held in the channel lists
  record_list.setAutoDelete(false);
}



// ArchiveFile::(destructor)

ArchiveFile::~ArchiveFile() {
   // nothing to do
}




void ArchiveFile::clearData() {

  ap_list.clear();
  record_list.clear();

  for (int c=0; c<HARDWARE_CHANNEL_COUNT; c++) channel_array[c].clear();

  max_channel_index = 0;

  bad_packet_count = 0;

  archive_fp = "";
}



// ArchiveFile::read()

bool ArchiveFile::readPackets(const JString& filepath, bool preview_only) {

  stop_processing = false;

    // deletes all packet and record data and resets counts to zero
  clearData();

  JFile f(filepath);
  if (!f.open(JIO_ReadOnly)) {
    jlog::es << "Archive file '" << filepath << "' does not exist or could not be read.\n"; 
    return false; 
  }

  archive_fp = filepath;

    // clear the numner of bytes read
  file_size_bytes = f.size();
  bytes_read_count = 0;

  if (jlog::vb(5)) jlog::ls << "File size: " << file_size_bytes << "\n";
 
    // intial value so picks up time of first packet
  logger_start_time_msec = -1;

  JDataStream str(&f);
 
    // will quit when read causes eof - also discards any final incomplete packet
  while (true) {

    //usleep(1000);
    if (stop_processing) return false;

      // create packet
    ArchivePacket* ap = new ArchivePacket;
      // quit when can't read anymore 
    if (!ap->read(str)) { delete ap; break; }
      // read ok so put this packet on the list
    ap_list.append(ap);
      // uopdate progess
    bytes_read_count += ap->bytesReadCount();
  
      // at loggimg level 4 show packets
    if (jlog::vbLevel(4)) ap->briefDisplay(packetCount());
      // go to next if this packet was corrupt
    if (ap->isBad()) { bad_packet_count++; continue; }

      // get first logger time point
    if (logger_start_time_msec < 0) logger_start_time_msec = ap->originTimeMsec();
      // this will be last logger time point
    logger_end_time_msec = ap->originTimeMsec();

      // loop through all channels in the packet
    for (int c=0; c<HARDWARE_CHANNEL_COUNT; c++) {

        // if got no data for this channel then continue
      if (ap->channelBytes(c) == 0) continue;

        // record the number of bytes seen for each channel
      channel_array[c].addToByteCount(ap->channelBytes(c));

      if (!preview_only) {

          // the current record for this channel
	Record* curr_rec = channel_array[c].currentRecord();
	  // if is null means we need to create a new record to start writing
	if (!curr_rec) {
	  curr_rec = config.sensor(c).createRecord();
	  curr_rec->setLoggerTimeMsec(logger_start_time_msec, ap->originTimeMsec());
	  curr_rec->setRecordPosition(channel_array[c].recordCount());
	  channel_array[c].appendRecord(curr_rec);
	  record_list.append(curr_rec);
	}

	  // get start pointer and number of bytes for this channel
	const uns8* ptr = ap->channelDataStart(c);
	int bytes_left = ap->channelBytes(c);
	  
	  // there might be enough bytes for this and more sensor packets, so loop
	while (bytes_left) {
	    // offer the data to the current packet, increments ptr and decrements bytes_left
	  curr_rec->appendBytes(ptr, bytes_left);
	    // if the sensor packet is done then append a new packet to the linked list
	  if (curr_rec->isComplete()) {
	    curr_rec = config.sensor(c).createRecord();
	    curr_rec->setLoggerTimeMsec(logger_start_time_msec, ap->originTimeMsec());
	    curr_rec->setRecordPosition(channel_array[c].recordCount());
	    channel_array[c].appendRecord(curr_rec);
	    record_list.append(curr_rec);
	  }
	}
      }
    }
  }
 
  f.close();

  if (packetCount() == 0) {
    logger_start_time_msec = 0;
    logger_end_time_msec = 0;
  }

  bytes_read_count = file_size_bytes;

  return true;
}



// ArchiveFile::process()
// this version is for command line processing - processes the archive file and loads hocr data

bool ArchiveFile::processOrLoadProcessed(const JParser& prm) {
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    if (!channel_array[i].process()) return false; 
    if (!config.sensor(i).loadProcessed(prm, channel_array[i], i)) return false;
  }
  return true;
}



bool ArchiveFile::loadProcessed(int i, const JParser& prm) {
  if (!config.sensor(i).loadProcessed(prm, channel_array[i], i)) return false;
  return true;
}



// this version is used by the gui just to do the load

//bool ArchiveFile::loadProcessedEdAndLu(int i, const JParser& prm) {
//  if (!config.sensor(i).loadProcessedEdAndLu(prm, channel_array[i], i)) return false;
// return true;
//}



// this version is used by the gui just to do the load

//bool ArchiveFile::loadProcessedEs(int i, const JParser& prm) {
//  if (!config.sensor(i).loadProcessedEs(prm, channel_array[i], i)) return false;
//  return true;
//}



// ArchiveFile::doMerge()

void ArchiveFile::doMerge(const JParser& prm, int merge_chan, bool* inc_flags) {
 
    // iterators to go through record lists
  JListIterator<Record>* iter[HARDWARE_CHANNEL_COUNT];
    // the current record in each channel
  Record* curr_rec[HARDWARE_CHANNEL_COUNT];
    // the current time difference of the records in each channel
  //long time_diff[HARDWARE_CHANNEL_COUNT];

    // set up iterators and first valid record in each channel (or null)
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    config.sensor(i).prepareForMerge(prm, merge_chan);
    iter[i] = new JListIterator<Record>(channel_array[i].recordList());
    curr_rec[i] = iter[i]->first();
    while (curr_rec[i]) {
      if (curr_rec[i]->isComplete() && !curr_rec[i]->hasProblem() && !curr_rec[i]->isExcluded()) break;
      curr_rec[i] = iter[i]->next();
    }
  }

    // clear the merge channel of records
  channel_merge.clear();

  while (curr_rec[merge_chan]) {

    if (curr_rec[merge_chan]->isComplete() && !curr_rec[merge_chan]->hasProblem() && !curr_rec[merge_chan]->isExcluded()) {
	
      Record_MERGE* rec_merge = new Record_MERGE();

      long ref_time = curr_rec[merge_chan]->timeSinceStartMsec();
     
        // move each record iterator forward to the one past the closest time 
      for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {

	if (!inc_flags[i] || !curr_rec[i]) continue;

	long time_diff = 0;

	if (i != merge_chan) {

	    // the time difference to the current record
	  time_diff = ref_time - curr_rec[i]->timeSinceStartMsec();
	
	  while (true) {
	    Record* r = iter[i]->next();
	    if (!r) break;
	    if (!r->isComplete() || r->hasProblem() || r->isExcluded()) continue;
	    long td = ref_time - r->timeSinceStartMsec();
	    if (fabs(td) > fabs(time_diff)) { iter[i]->prev(); break; }
	      // this is now the record that is closer
	    curr_rec[i] = r;
	      // and this is its time difference
	    time_diff = td;
	  }
	}
	rec_merge->setRecord(i, curr_rec[i], time_diff);
      }
    
      channel_merge.appendRecord(rec_merge);
    }

    curr_rec[merge_chan] = iter[merge_chan]->next();
  }

  if (jlog::vbLevel(2)) jlog::ls << "Merged record count: " << channel_merge.recordCount() << "\n";
}



// ArchiveFile::display()

void ArchiveFile::display(JTextStream& str) const {

  str.printf("Sampling Duration : %.1f s\n",((double)logger_end_time_msec - logger_start_time_msec)/1000); 

  str << "Packet count: " << packetCount() << "\n";
  str << "Bad packet count: " << bad_packet_count << "\n";

  str << "Channel byte counts:\n";
  for (int c=0; c<HARDWARE_CHANNEL_COUNT; c++) {
    str << "Channel " << (c+1) << " : " << channel_array[c].byteCount() << "\n";
  }

  str << "Channel record counts:\n";
  for (int c=0; c<HARDWARE_CHANNEL_COUNT; c++) {
    str << "Channel " << (c+1) << " (" << config.sensor(c).typeText() << ") : " << channel_array[c].recordCount() << "\n";
  }
}

