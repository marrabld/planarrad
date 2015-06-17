
#include "wl/archivepacket.h"
#include "wl/byteio.h"
#include "base/jlog.h"

using namespace jude::wl;



// ArchivePacket::(constructor)

ArchivePacket::ArchivePacket() :
  data(0),
  bytes_read_count(0) { }



// ArchivePacket::(destructor)

ArchivePacket::~ArchivePacket() { 
  delete[] data;
}



// ArchivePacket::read()

bool ArchivePacket::read(JDataStream& str) {

  str.byteRead1(destunit); 
  str.byteRead1(pkttype); 
  str.byteRead1(senderunit); 
  str.byteRead1(mod); 
  str.byteRead2SE(length); 
  str.byteRead4SE(origintime);

  int16 length_test = 0;
  for (int i=0;i<9;i++) {
    str.byteRead2SE(chanbytes[i]);
    length_test += chanbytes[i];
  }

  bytes_read_count = 10 + 9*2;

    // sometimes for the first packet the length field is correct but the 
    // individual channel lengths don't add up to it as they should
    // so this is marked as a bad packet but should be read anyway
    // as afterwards is generally ok
  is_bad = (length > 2400 || length != length_test);
 
     // quit if hit the end
  if (str.device().atEnd()) return false;

    // allocate memory for data plus four # bytes for termination
  try {
    data = new unsigned char[length+4];
  } catch (const std::bad_alloc& e) {
    return false;
  }

  uns8* ptr = data;
  uns8* data_end = data + length + 4;
  
    // read one byte at a time 
    // note there is no point in looking for the hash terminator since can be fooled
    // by an additional hash at the end of the data
  while (ptr!=data_end) { str.byteRead1(*ptr++); bytes_read_count++; }

    // record if hash code is at the end
  found_hash = (ptr[-1]=='#' && ptr[-2]=='#' && ptr[-3]=='#' && ptr[-4]=='#');

    // if got an eof then not a complete packet
  return (!str.device().atEnd());
}



// ArchivePacket::channelDataStart()

const uns8* ArchivePacket::channelDataStart(int c) const {
  unsigned char *ptr = data;
  for (int i=0;i<c;i++) ptr += channelBytes(i);
  return ptr;
}



void ArchivePacket::briefDisplay(int pnum) const {

  printf("Packet %d msec %.3f [%d] ",pnum, (double)originTimeMsec()/1000, dataLength());
  for (int j=0;j<9;j++) jlog::ls << channelBytes(j) << " ";
  jlog::ls << ((is_bad) ? "BAD" : "OK") << "\n";

  //for (int j=0;j<9;j++) {
  // if (p.channelBytes(j) > 0) {
  //   printf(" Chan: %d ",j); 
  //  p.displayChannelData(j);
  //  }
  // }
}

// ArchivePacket::display()

void ArchivePacket::display() const {
  printf("Destination Unit: %d\n",destinationUnit());
  printf("Packet Type: %d\n",packetType());
  printf("Logger ID: %d\n",senderUnit());
  printf("Modulus: %d\n",modulus());
  printf("Length: %d\n",dataLength());
  printf("Origin Time: %d (%.3f s)\n",(int)originTime(),(double)originTimeMsec()/1000);
  printf("Chan Bytes: ");
  for (int i=0;i<9;i++) printf(" %d",channelBytes(i)); 
  printf("\n");
}



// ArchivePacket::displayChannelData()

void ArchivePacket::displayChannelData(int c) const {

  unsigned char *ptr = data;
  for (int i=0;i<c;i++) ptr += channelBytes(i);

  unsigned char *ptr_end = ptr + channelBytes(c);

  if (ptr[0]==0xff && ptr[1]==0 && ptr[2]==0xff && ptr[3]==0) printf("ACS\n");
  else {

    unsigned char *t = ptr;
    bool is_text = true;
    while (t != ptr_end) {
      if (! ((*t>31 && *t<128) || *t==10 || *t==9 || *t==13) ) is_text = false;
      t++;
    }

    if (is_text) while (ptr != ptr_end) printf("%c",*ptr++);
  
    printf("\n");
  }
}

