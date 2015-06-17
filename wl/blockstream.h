
#ifndef _JUDE_WL_BLOCKSTREAM_H
#define _JUDE_WL_BLOCKSTREAM_H

#include "base/jtypes.h"
#include "base/jdatastream.h"
#include "wl/bytestream.h"

namespace jude {
namespace wl {



class BlockStream : public ByteStream {

public:

  struct Block {

    int length;
    uns8* data;

    Block* next;
  
    Block(const uns8* d, int len);
    ~Block();
  };

private:

  Block* curr_block;

  uns8* curr_pos;
  uns8* curr_end;
  bool past_end;

public:

  BlockStream(Block* first_block);

  BlockStream(const BlockStream& sp);

  BlockStream(Block* first_block, int pos);
  
  uns8 read();
  uns8 current();
  void forward();

  bool pastEnd() const;
};



} } // namespace jude::wl

#endif // _JUDE_WL_BLOCKSTREAM_H

