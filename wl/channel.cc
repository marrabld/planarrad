
#include "wl/channel.h"

using namespace jude::wl;



Channel::Channel() : 
  curr_rec(0), 
  channel_byte_count(0),
  processed_done_count(0),
  is_completely_processed(false) { }



void Channel::clear() {
  record_list.clear();
  curr_rec = 0; 
  channel_byte_count = 0; 
  is_completely_processed = false;
}



// Channel::process()

bool Channel::process() {

  is_completely_processed = false;
  stop_processing = false;
 
  JListIterator<Record> iter(record_list);
  Record* rec = iter.first();

  processed_done_count = 0;

  while (rec) {
    //usleep(200);
    if (stop_processing) return false;
    if (rec->isComplete() && !rec->hasProblem()) if (!rec->process()) return false;
    processed_done_count++;
    rec = iter.next();
  }

  is_completely_processed = true;

  return true;
}



// Channel::process()

void Channel::clearAllProcessed() {

  is_completely_processed = false;
 
  JListIterator<Record> iter(record_list);
  Record* rec = iter.first();

  while (rec) {
    rec->clearProcessed();
    rec = iter.next();
  }
}



// Channel::writeBytes()

void Channel::writeBytes(JDataStream& str) const {

  JListIterator<Record> iter(record_list);

  Record* rec = iter.first();

  while (rec) {
    if (rec->isComplete() && !rec->hasProblem()) {
      rec->writeBytes(str);
      str << "\n";
    }
    rec = iter.next();
  }
}



// Channel::writeProcessed()

void Channel::writeProcessed(JTextStream& str) const {

  JListIterator<Record> iter(record_list);
  Record* rec = iter.first();

  if (!rec) return;

  rec->writeProcessedHeader(str);
  str << "\n";
 
  while (rec) {
    if (rec->isComplete() && !rec->hasProblem()) {
      rec->writeProcessed(str);
      str << "\n";
    }
    rec = iter.next();
  }
}

