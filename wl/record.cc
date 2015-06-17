
#include "record.h"
#include "sensor.h"
#include <string.h>

using namespace jude::wl;



// Record::(constructor)

Record::Record(const Sensor* s) : 
  sens(s),
  first_block(0),
  total_length(0),
  is_complete(false),
  has_problem(false),
  is_excluded(false) { }




// Record::(destructor)

Record::~Record() {
  clearDataBlocks();
}



const JString Record::typeText() const { 
  return JString(sens->typeText()); 
}



// Record::newDataBlock()

void Record::newDataBlock(const uns8* data, int length) {

  total_length += length;

    // if no blocks at all yet then just create first
  if (!first_block) { first_block = new BlockStream::Block(data, length); return; }

    // othewise insert into start of list
  BlockStream::Block* curr = first_block;
  while (curr->next) curr = curr->next;

  curr->next = new BlockStream::Block(data, length);
}  



// Record::clearDataBlocks()

void Record::clearDataBlocks() {

  BlockStream::Block* curr = first_block;

  while (curr) {
    BlockStream::Block* next = curr->next;
    delete curr;
    curr = next;
  }
  first_block = 0;
  total_length = 0;
}

