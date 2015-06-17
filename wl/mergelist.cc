
#include "mergelist.h"
#include "archivefile.h"
#include "channel.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

using namespace jude::wl;



// MergeList::(constructor)

MergeList::MergeList() { 
  record_list.setAutoDelete(true);
}



// MergeList::(destructor)

MergeList::~MergeList() {
  record_list.clear();
}



// MergeList::build()

void MergeList::build(const ArchiveFile& af, int merge_channel) {

  record_list.clear();

  //const JList<Channel>& clist(af.channelList());
  int cnum = af.maxChannelIndex();
  //int cnum = clist.count();
  
   // get first and next record of each channel
  Record* curr_r[cnum];
  Record* next_r[cnum];

  for (int i=0;i<cnum;i++) {
    curr_r[i] = af.channel(i).recordList().first();
    if (i != merge_channel) next_r[i] = af.channel(i).recordList().next();
  }

  while (curr_r[merge_channel]) {

    if (curr_r[merge_channel]->isComplete() && !curr_r[merge_channel]->isExcluded()) {

        // this is the time we are looking for
      int64 curr_logger_time_msec = curr_r[merge_channel]->loggerTimeMsec();

        // set up the merged record
      Record_MERGE* merge_rec = new Record_MERGE();
      merge_rec->setLoggerTimeMsec(af.loggerStartTimeMsec(), curr_logger_time_msec);
      merge_rec->setRecord(merge_channel, curr_r[merge_channel], 0);

	// loop through channels to find matched records
      for (int i=0;i<cnum;i++) {

          // for all channels other than the master
	if (i != merge_channel) {
	  
	  int curr_dt;
	  int next_dt;
	
	  do {
	      // time differences to current and next record
	    curr_dt = abs((int)(curr_r[i]->loggerTimeMsec() - curr_logger_time_msec));
	    next_dt = next_r[i] ? abs((int)(next_r[i]->loggerTimeMsec() - curr_logger_time_msec)) : INT_MAX;
	  
	      // if next is nearer then move on to next
	    if (curr_dt > next_dt) {
	      curr_r[i] = next_r[i];
	      next_r[i] = af.channel(i).recordList().next();
	    }

	    printf("%d %d %d %p\n",i,curr_dt,next_dt,curr_r[i]);
	    
	    // continue until got to closest record
	  } while (curr_dt > next_dt);
	  

	  merge_rec->setRecord(i, curr_r[i], 0);  
	}
      }
      
      record_list.append(merge_rec);
    }

    curr_r[merge_channel] = af.channel(merge_channel).recordList().next();
  }
}



// MergeList::display()

void MergeList::display() const {
  /*
  Record_MERGE* rec = record_list.first();
  while (rec) {
    printf("%d ", (int)rec->loggerTimeMsec());
    for (int i=0;i<HARDWARE_CHANNEL_COUNT;i++) {
      printf("%s: ",config.sensor(i)->nameText());
      if (rec->record(i)) printf("%d ",(int) rec->record(i)->loggerTimeMsec());
      else printf("- ");
    }
    printf("\n");
    rec = record_list.next();
  }
  */
}

