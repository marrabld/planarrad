
#include "wl/record_text.h"
#include "wl/sensor.h"
#include "base/jlog.h"

using namespace jude::wl;

const bool debug = false;



// Record_TEXT::(constructor)

Record_TEXT::Record_TEXT(const Sensor* s) :
  Record(s) { 

  item_list.setAutoDelete(true);
}



// Record_TEXT::(destructor)

Record_TEXT::~Record_TEXT() {
  item_list.clear();
}



// Record_TEXT::decompose() [protected]

bool Record_TEXT::decompose(int& bytes_read) {

  item_list.clear();

    // first data block
  BlockStream bs(first_block);
  uns8 c = bs.current();
  bytes_read = 1;
 
    // go past any leading spaces
  while (!bs.pastEnd() && c==' ') { bs.forward(); c = bs.current(); bytes_read++; }
    // quit if nothing but space
  if (bs.pastEnd()) return false;
    // start first text item
  TextItem* curr_item = new TextItem(bs);
  item_list.append(curr_item);
    // start counting chars
  int count = 0;
  bool wait_for_next = false;

    // continue until hit the buffer end or line end
  while (!bs.pastEnd() && c!=0x0a && c!=0x0d) {

    //printf("%c",c);
      // if waiting for the next text item
    if (wait_for_next) {
        // ignore leading spaces
      if (c!=' ') {
	curr_item = new TextItem(bs);
	item_list.append(curr_item);
	wait_for_next = false;
	count = 0;
      }
    }

      // if hit a seperator then note length of previous item
    if (c==0x09 || c==',') {
      curr_item->setLength(count);
      wait_for_next = true;
    } else count++;

      // get next char
    bs.forward();
    c = bs.current();

    bytes_read++;
  }

  curr_item->setLength(count);

  //printf("\nat end %d count %d\n",bs.pastEnd(), count);

    // if hit the end before a line end then not complete
  if (bs.pastEnd()) return false;

  return true;
}



// Record_TEXT::appendData()

void Record_TEXT::appendBytes(const uns8*& ptr, int& bytes_left) {

  if (jlog::vbLevel(5)) jlog::ls << "Record_TEXT::appendBytes() " << bytes_left << "\n"; 

    // add the whole new data block
  newDataBlock(ptr, bytes_left);

    // if can complete the packet then might be some data left for the next one
  int final_pos;
  if (buildRecord(final_pos)) {
      // calculate number of bytes used from the last block
    int bytes_used = bytes_left - (total_length - final_pos);
      // might be some bytes left from this file packet
    bytes_left += bytes_used;
    ptr += bytes_used;
  }

    // otherwise will use the whole block
  ptr += bytes_left;
  bytes_left = 0;
}



void Record_TEXT::writeBytes(JDataStream& str) const {
 
  TextItem* item = item_list.first();
  while (item) {
    BlockStream bs(item->streamStart());
    for (int j=0; j<item->length(); j++) str << bs.read();
    item = item_list.next();
    if (item) str << "\t";
  }
}




void Record_TEXT::writeProcessed(JTextStream& str) const {
 
  TextItem* item = item_list.first();
  while (item) {
    BlockStream bs(item->streamStart());
    for (int j=0; j<item->length(); j++) str.device().putch(bs.read());
    item = item_list.next();
    if (item) str << "\t";
  }
}



const JString Record_TEXT::processed(int i) const {
  if (i<0 || i>= itemCount()) return "";
  return itemToString(i);
}



// Record_TEXT::textDisplayLine()

const char* Record_TEXT::textDisplayLine(int i) const {

  if (!sensor()) return "ERROR";
 
  static const int bufflen = 100;
  static char buffer[bufflen];
  static char buffval[bufflen];

  if (i<0 || i>=item_list.count()+2) return 0;

  if (i==item_list.count()) snprintf(buffer, bufflen, "<br>[<b>is_complete</b> = %d]", isComplete());
  else if (i==item_list.count()+1) snprintf(buffer, bufflen, "[<b>has_problem</b> = %d]", hasProblem());
  else {

    TextItem* item = item_list.at(i);
 
    int j;
    BlockStream bs(item->streamStart());
    for (j=0;j<item->length() && j<bufflen-1;j++) buffval[j] = bs.read();
    buffval[j] = 0;
  }

  //if (debug) snprintf(buffer, bufflen, "<b>%s</b> = \"%s\"", sensor()->columnHead(i), buffval);

  return buffer;
}



// Record_TEXT::display()

const JString Record_TEXT::itemToString(int index) const {

  TextItem* item = item_list.at(index);
  if (!item) return "";

  char buff[item->length()];

  BlockStream bs(item->streamStart());
  for (int i=0; i<item->length(); i++) buff[i] = bs.read();

  return JString(buff, item->length());
}



// Record_TEXT::display()

void Record_TEXT::display() const {

  if (item_list.count()==0) { printf("  No items\n"); return; }

  TextItem* curr_item = item_list.first();
  while (curr_item) {
    printf("  <");
    BlockStream bs(curr_item->streamStart());
    for (int i=0;i<curr_item->length();i++) printf("%c",bs.read());
    printf(">\n");
    curr_item = item_list.next();
  }
}

