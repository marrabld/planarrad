
#include "wl/blockstream.h"
#include <string.h>

using namespace jude::wl;



// BlockStream::Block::(constructor)

BlockStream::Block::Block(const uns8* src_data, int len) :
  length(len),
  data(new uns8[len]),
  next(0) { 

  memcpy(data, src_data, len);
}



// BlockStream::Block::(destructor)

BlockStream::Block::~Block() {
  delete[] data;
}



// BlockStream::(constructor)

BlockStream::BlockStream(Block* first_block) :
  curr_block(first_block),
  curr_pos(first_block->data),
  curr_end(first_block->data + first_block->length),
  past_end(false) { }



// BlockStream::(constructor)

BlockStream::BlockStream(const BlockStream& sp) :
  curr_block(sp.curr_block),
  curr_pos(sp.curr_pos),
  curr_end(sp.curr_end),
  past_end(sp.past_end) { }

  

// BlockStream::(constructor)

BlockStream::BlockStream(Block* first_block, int pos) :
  curr_block(first_block),
  curr_pos(first_block->data),
  curr_end(first_block->data + first_block->length),
  past_end(false) { 

  while (pos > curr_block->length) {
    pos -= curr_block->length;
    curr_block = curr_block->next;
    if (!curr_block) return;
  }

  curr_pos = curr_block->data + pos;
  curr_end = curr_block->data + first_block->length;
}



// BlockStream::read()

uns8 BlockStream::read() {

    // past end becomes true when a read is attempted beyond the end
  if (!curr_block) { past_end = true; return 0; }
    // return zero if go past end
  if (past_end) return 0;
    // the value to return
  uns8 v = *curr_pos;
    // move forward one position if still in block then quit
  curr_pos++;
  if (curr_pos < curr_end) return v;
  do {
      // get the next block
    curr_block = curr_block->next;
      // quit if at end 
    if (!curr_block) { return v; }
    // continue until got a block with something in it or hit the end
  } while (curr_block->length == 0);

    // set up pointers to new block  
  curr_pos = curr_block->data;
  curr_end = curr_pos + curr_block->length;

  return v;
}



// BlockStream::current()

uns8 BlockStream::current() {
  if (!curr_block || past_end) { past_end = true; return 0; }
  return *curr_pos;
}



// BlockStream::forward()

void BlockStream::forward() {

  if (!curr_block || past_end) return;

    // move forward one position if still in block then quit
  curr_pos++;
  if (curr_pos < curr_end) return;
  do {
      // get the next block
    curr_block = curr_block->next;
      // quit if at end 
    if (!curr_block) return;
    // continue until got a block with something in it or hit the end
  } while (curr_block->length == 0);

    // set up pointers to new block  
  curr_pos = curr_block->data;
  curr_end = curr_pos + curr_block->length;
}



// BlockStream::pastEnd()

bool BlockStream::pastEnd() const { 
  return past_end; 
}

