
#ifndef _JUDE_WL_ARCHIVEPACKET
#define _JUDE_WL_ARCHIVEPACKET

#include "base/jtypes.h"
#include "base/jdatastream.h"
#include <stdio.h>

using namespace jude::base;

namespace jude {
namespace wl {



/* this is the documented structure
   from the file WetLabs_Archive_Format.doc

struct {
  unsigned char destunit;
  unsigned char pkttype;
  unsigned char senderunit;
  unsigned char modulus;
  short length;
  long origintime;
  short chanbytes[9];
  unsigned char data[2400];  // actually only <length> bytes are sent
  unsigned char padding[4];
};

*/



class ArchivePacket {

  enum { MAX_CHANNELS = 9 };

  uns8 destunit;
  uns8 pkttype;
  uns8 senderunit;
  uns8 mod;
  int16 length;
  int32 origintime;
  int16 chanbytes[MAX_CHANNELS];
  uns8* data;

  int32 bytes_read_count;

  bool found_hash;

  bool is_bad;

public:

  ArchivePacket();
  ~ArchivePacket();

  bool read(FILE *f);

  bool read(JDataStream& str);

  int destinationUnit() const { return destunit; }
  int packetType() const { return pkttype; }
  int senderUnit() const { return senderunit; }
  int modulus() const { return mod; }

  int dataLength() const { return length; }

  int32 bytesReadCount() const { return bytes_read_count; }

    /*! \brief Returns the 'Origintime' field as it occurs in the data, interpretation of which is dependent on 'Pkttype'. */
  int64 originTime() const { return origintime; }
    /*! \brief Returns the 'Origintime' field converted to milliseconds. */
  int64 originTimeMsec() const { return (pkttype) ? ((int64)origintime)*10 : origintime; }  

  int maxChannels() const { return MAX_CHANNELS; }
  int channelBytes(int i) const { return chanbytes[i]; }
 
  const uns8* packetData() const { return data; }
  const uns8* channelDataStart(int c) const;

  bool foundHash() const { return found_hash; }

    /*! \brief Returns \c true if the bytes read for this packet do not match certain criteria for the expected format.*/
  bool isBad() const { return is_bad; }

  void briefDisplay(int pnum) const;
  void display() const;
  void displayChannelData(int c) const;
};



} } // namespace jude::wl

#endif // _JUDE_WL_ARCHIVEPACKET

