
#include "record_acs.h"
#include "sensor_acs.h"
#include "byteio.h"
#include <stdio.h>
#include <stdlib.h>

using namespace jude::wl;



// Record_ACS::(costructor)

Record_ACS::Record_ACS(const Sensor_ACS* s) :
  Record(s),
  cref(0),
  aref(0),
  csig(0),
  asig(0),
  is_processed(false) { }



// Record_ACS::(destructor)

Record_ACS::~Record_ACS() {
  delete[] cref;
  delete[] aref;
  delete[] csig;
  delete[] asig;
}



// Record_ACS::appendBytes()

void Record_ACS::appendBytes(const uns8*& ptr, int& bytes_left) {

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
 


// Record_ACS::buildRecord() [private]

bool Record_ACS::buildRecord(int& final_pos) {
 
  int start_pos = 0;
  int bytes_read = 0;

    // might have to try a few start positions
  while (!isComplete()) {

      // try to find the start code 0xff00ff00
    BlockStream bs(first_block, start_pos);
    findStart(bs, start_pos);

      // couldn't find a start point so can't build
    if (bs.pastEnd()) return false; 

    //printf("start %d\n", start_pos);
      // try to build the packet from the data
      // fail here means the data has failed an internal check, i.e. bad packet
    if (read(bs, bytes_read)) {
        // if hit the end then not got enough data yet
      if (bs.pastEnd()) return false; 
        // if checksum and length are ok then got a packet
      setIsComplete(record_length == bytes_read+4-3);
      setHasProblem(check_sum != checkSumCalc());
    
      //printf("cs %d %d\n",check_sum,checkSumCalc()); 
      //printf("rl %d %d\n",record_length,bytes_read+4-3); 
    }
      // otherwise will need to look for another start point from one past the previous
    start_pos++;
  }

    // don't need the data blocks anymore
  clearDataBlocks();

  final_pos = start_pos + bytes_read - 1;  
  return true;
}



// Record_ACS::findStart() [private]

void Record_ACS::findStart(BlockStream& bs, int& pos) {

  uns8 b0 = bs.read();
  uns8 b1 = bs.read();
  uns8 b2 = bs.read();
  uns8 b3 = bs.read();

  if (bs.pastEnd()) return;

  while (b0!=0xff || b1!=0 || b2!=0xff || b3!=0) {
    b0 = b1;
    b1 = b2;
    b2 = b3;
    b3 = bs.read();
    if (bs.pastEnd()) return;
    pos++;
  }
}



// Record_ACS::read()
// this version is for reading fomr buffered file data
// already read the start code 0xff00ff00 and the 2 byte record length
// shouldn't get a bs.pastEnd() because meant to check if enough data beforehand

bool Record_ACS::read(ByteStream& bs, int& bytes_read) {

  byteRead2BE(record_length, bs);
  byteRead1(packet_type, bs);
  byteRead1(reserved_1, bs);
  byteRead1(meter_type, bs);
  byteRead3BE(serial_number, bs);
  byteRead2BE(a_ref_dark_counts, bs);
  byteRead2BE(pressure_counts, bs);
  byteRead2BE(a_sig_dark_counts, bs);
  byteRead2BE(ext_temp_counts, bs);
  byteRead2BE(int_temp_counts, bs);
  byteRead2BE(c_ref_dark_counts, bs);
  byteRead2BE(c_sig_dark_counts, bs);
  byteRead4BE(msec_since_power, bs);
  byteRead1(reserved_2, bs);
  byteRead1(wave_num, bs);

    // double check that wave_num is not corrupted
  if (record_length != 32 + wave_num * 8) return false;

  cref = new uns16[wave_num];
  aref = new uns16[wave_num];
  csig = new uns16[wave_num];
  asig = new uns16[wave_num];

  for (int i=0;i<wave_num;i++) {
    byteRead2BE(cref[i], bs);
    byteRead2BE(aref[i], bs);
    byteRead2BE(csig[i], bs);
    byteRead2BE(asig[i], bs);  
  }
  byteRead2BE(check_sum, bs);  

  byteRead1(pad_byte, bs);

    // number of bytes actually read here (i.e. not including the 4 byte start code)
  bytes_read = wave_num*8 + 31;

    // initialise these things in case they don't get processed
  c_corrected = zero_vector<double>(wave_num);
  a_corrected = zero_vector<double>(wave_num);
  int_temp = 0;

  return true;
}



// Record_ACS::checkSumCalc()

uns16 Record_ACS::checkSumCalc() const {

  uns16 v = (255 * 2);
  v += byteAdd2(record_length);
  v += byteAdd1(packet_type);
  v += byteAdd1(reserved_1);
  v += byteAdd1(meter_type);
  v += byteAdd3(serial_number);
  v += byteAdd2(a_ref_dark_counts);
  v += byteAdd2(pressure_counts);
  v += byteAdd2(a_sig_dark_counts);
  v += byteAdd2(ext_temp_counts);
  v += byteAdd2(int_temp_counts);
  v += byteAdd2(c_ref_dark_counts);
  v += byteAdd2(c_sig_dark_counts);
  v += byteAdd4(msec_since_power);
  v += byteAdd1(reserved_2);
  v += byteAdd1(wave_num);

  for (int i=0;i<wave_num;i++) {
    v += byteAdd2(cref[i]);
    v += byteAdd2(aref[i]);
    v += byteAdd2(csig[i]);
    v += byteAdd2(asig[i]);  
  }
  //byteRead2BE(check_sum, f);  

  return v; 
}



// Record_ACS::writeBytesProcessed()

void Record_ACS::writeBytes(JDataStream& str) const {

  str.byteWrite4BE(0xff00ff00);
  
  str.byteWrite2BE(record_length);
  str.byteWrite1(packet_type);
  str.byteWrite1(reserved_1);
  str.byteWrite1(meter_type);
  str.byteWrite3BE(serial_number);
  str.byteWrite2BE(a_ref_dark_counts);
  str.byteWrite2BE(pressure_counts);
  str.byteWrite2BE(a_sig_dark_counts);
  str.byteWrite2BE(ext_temp_counts);
  str.byteWrite2BE(int_temp_counts);
  str.byteWrite2BE(c_ref_dark_counts);
  str.byteWrite2BE(c_sig_dark_counts);
  str.byteWrite4BE(msec_since_power);
  str.byteWrite1(reserved_2);
  str.byteWrite1(wave_num);

  for (int i=0;i<wave_num;i++) {
    str.byteWrite2BE(cref[i]);
    str.byteWrite2BE(aref[i]);
    str.byteWrite2BE(csig[i]);
    str.byteWrite2BE(asig[i]);  
  }

  str.byteWrite2BE(check_sum);  

  str.byteWrite1(pad_byte);
}



// Record_ACS::process()

bool Record_ACS::process() {
 
  is_processed = false;

  if (!sensor()->hasDeviceFile()) return true;

  const Sensor_ACS* sens = dynamic_cast<const Sensor_ACS*>(sensor()); 
  if (!sens) { jlog::es << "Internal error in Record_ACS::process()"; return false; }

  if (wave_num != sens->bandCount()) {
    jlog::es << "ACS on channel " << sens->channelIndex()+1 << ": Device file band count differs from data band count. Probably it's the wrong device file. ";
    jlog::es << "Change or clear the device file and load the archive file again.\n";
    return false;
  }

    // calculate the internal temperature
  double volts = (5 * int_temp_counts) / 65536.0;
  double res = 10000 * volts / (4.516 - volts);
  const double a = 0.00093135;
  const double b = 0.000221631;
  const double c = 0.000000125741;
  int_temp = (1 / (a + b*log(res) + c*pow(log(res),3))) - 273.15;

  for (int i=0;i<wave_num;i++) {
    c_corrected(i) = sens->calcC(cref[i], csig[i], i) - sens->deltaTC(int_temp, i);
    a_corrected(i) = sens->calcA(aref[i], asig[i], i) - sens->deltaTA(int_temp, i);
  }

  is_processed = true;

  return true;
}



// Record_ACS::writeProcessed()

void Record_ACS::writeProcessed(JTextStream& str) const {

  if (!sensor()->hasDeviceFile()) {
    str << "UNPROCESSED_ACS";
    return;
  }

  str << (int) timeSinceStartMsec() << "\t";
  
  for (int i=0; i<wave_num; i++) str << c_corrected(i) << "\t";
  for (int i=0; i<wave_num; i++) str << a_corrected(i) << "\t";

  str << int_temp;
}



void Record_ACS::writeProcessedHeader(JTextStream& str) const {

  if (!sensor()->hasDeviceFile()) {
    str << "UNPROCESSED_ACS";
    return;
  }

  const Sensor_ACS* sens = dynamic_cast<const Sensor_ACS*>(sensor()); 

  str << "msec\t";

  int prec = str.precision();
  str.setFixed(1);
  
  for (int i=0; i<wave_num; i++) str << JString("c") << sens->cBandWavelength(i) << "\t";
  for (int i=0; i<wave_num; i++) str << JString("a") << sens->aBandWavelength(i) << "\t";

  str << "int_temp";

  str.setScientific(prec);
}



// Record_ACS::processedColumnsCount()

int Record_ACS::processedColumnsCount() const {
  if (!sensor()->hasDeviceFile()) return 1;
  return 2*wave_num+2;
}



// Record_ACS::processedHeader()

const JString Record_ACS::processedHeader(int i) const {
  if (!sensor()->hasDeviceFile()) return "UNPROCESSED_ACS";
  const Sensor_ACS* sens = dynamic_cast<const Sensor_ACS*>(sensor()); 
  if (!sens || i == 0) return "msec";
  if (i <= wave_num) return JString("c")+JString::number(sens->cBandWavelength(i-1),'f',1);
  if (i <= 2*wave_num) return JString("a")+JString::number(sens->aBandWavelength(i-wave_num-1),'f',1);
  return "int_temp";
}



// Record_ACS::processed()

const JString Record_ACS::processed(int i) const {

  if (!isProcessed()) return "UNPROCESSED_ACS";

  const Sensor_ACS* sens = dynamic_cast<const Sensor_ACS*>(sensor()); 
  if (!sens) return "";

  if (i == 0) return JString::number((int)timeSinceStartMsec());
  
  if (i <= wave_num) return JString::number(c_corrected(i-1),'e',5);
  if (i <= 2*wave_num) return JString::number(a_corrected(i-wave_num-1),'e',5);

  return JString::number(int_temp,'f',2);
}



// Record_ACS::textDisplayLine()

const char* Record_ACS::textDisplayLine(int i) const {

  static const int bufflen = 3000;
  static char buffer[bufflen];

  switch (i) {
  case 0 : snprintf(buffer, bufflen, "<b>record_length</b> = %d", record_length); break;
  case 1 : snprintf(buffer, bufflen, "<b>packet_type</b> = %d", packet_type); break;
  case 2 : snprintf(buffer, bufflen, "<b>reserved_1</b> = %d", reserved_1); break;
  case 3 : snprintf(buffer, bufflen, "<b>meter_type</b> = %d", meter_type); break;
  case 4 : snprintf(buffer, bufflen, "<b>serial_number</b> = %u", (int)serial_number); break;
  case 5 : snprintf(buffer, bufflen, "<b>a_ref_dark_counts</b> = %d", a_ref_dark_counts); break;
  case 6 : snprintf(buffer, bufflen, "<b>pressure_counts</b> = %d", pressure_counts); break;
  case 7 : snprintf(buffer, bufflen, "<b>a_sig_dark_counts</b> = %d", a_sig_dark_counts); break;
  case 8 : snprintf(buffer, bufflen, "<b>ext_temp_counts</b> = %d", ext_temp_counts); break;
  case 9 : snprintf(buffer, bufflen, "<b>c_ref_dark_counts</b> = %d", c_ref_dark_counts); break;
  case 10 : snprintf(buffer, bufflen, "<b>c_sig_dark_counts</b> = %d", c_sig_dark_counts); break;
  case 11 : snprintf(buffer, bufflen, "<b>int_temp_counts</b> = %d", int_temp_counts); break;
  case 12 : snprintf(buffer, bufflen, "<b>msec_since_power</b> = %d", (int)msec_since_power); break;
  case 13 : snprintf(buffer, bufflen, "<b>reserved_2</b> = %d", reserved_2); break;
  case 14 : snprintf(buffer, bufflen, "<b>wave_num</b> = %d", wave_num); break;
  case 15 : snprintf(buffer, bufflen, "<b>check_sum</b> = %d (calc = %d)", check_sum, checkSumCalc()); break;
  case 16 : snprintf(buffer, bufflen, "<b>pad_byte</b> = %d", pad_byte); break;
  case 17 : dataText("<br><b>aref</b> =", buffer, bufflen, aref); break;
  case 18 : dataText("<br><b>asig</b> =", buffer, bufflen, asig); break;
  case 19 : dataText("<br><b>cref</b> =", buffer, bufflen, cref); break;
  case 20 : dataText("<br><b>csig</b> =", buffer, bufflen, csig); break;
  case 21 : snprintf(buffer, bufflen, "<br>[<b>is_complete</b> = %d]", isComplete()); break;
  case 22 : snprintf(buffer, bufflen, "[<b>has_problem</b> = %d]", hasProblem()); break;
 
  default: return 0;
  }

  return buffer;
}



// Record_ACS::dataText() [private]

void Record_ACS::dataText(const char* str, char *buff, int buff_len, uns16* data) const {

  int n = snprintf(buff, buff_len, "%s ",str);
  buff += n;
  buff_len -= n;

  for (int i=0;i<wave_num && buff_len>0;i++) {
    int n = snprintf(buff, buff_len, "%d ", data[i]);
    if (n<0) return;
    buff += n;
    buff_len -= n;
  }
}



// Record_ACS::display()

void Record_ACS::display() const {
  printf("record_length %d\n",record_length);
  printf("packet_type %d\n",packet_type);
  printf("reserved_1 %d\n",reserved_1);
  printf("meter_type %d\n",meter_type);
  printf("serial_number %u\n",(int)serial_number);
  printf("a_ref_dark_counts %d\n",a_ref_dark_counts);
  printf("pressure_counts %d\n",pressure_counts);
  printf("a_sig_dark_counts %d\n",a_sig_dark_counts);
  printf("ext_temp_counts %d\n",ext_temp_counts);
  printf("c_ref_dark_counts %d\n",c_ref_dark_counts);
  printf("c_sig_dark_counts %d\n",c_sig_dark_counts);
  printf("int_temp_counts %d\n",int_temp_counts);
  printf("msec_since_power %d\n",(int)msec_since_power);
  printf("reserved_2 %d\n",reserved_2);
  printf("wave_num %d\n",wave_num);
  printf("check_sum %d\n",check_sum);
  printf("pad_byte %d\n",pad_byte); 
  printf("aref: ");
  for (int i=0;i<wave_num;i++) printf("%d ",aref[i]);
  printf("\n");
  printf("asig: ");
  for (int i=0;i<wave_num;i++) printf("%d ",asig[i]);
  printf("\n");
  printf("cref: ");
  for (int i=0;i<wave_num;i++) printf("%d ",cref[i]);
  printf("\n");
  printf("csig: ");
  for (int i=0;i<wave_num;i++) printf("%d ",csig[i]);
  printf("\n");
  printf("[is_complete = %d]\n",isComplete());
  printf("[has_problem = %d]\n",hasProblem());
  printf("calc check sum %d\n",checkSumCalc());
}

/*



// ACSPacket::write(FILE* f)

void ACSPacket::write(FILE* f) const {

  byteWrite4BE(0xff00ff00, f);
  byteWrite2BE(record_length, f);
  byteWrite1(packet_type, f);
  byteWrite1(reserved_1, f);
  byteWrite1(meter_type, f);
  byteWrite3BE(serial_number, f);
  byteWrite2BE(a_ref_dark_counts, f);
  byteWrite2BE(pressure_counts, f);
  byteWrite2BE(a_sig_dark_counts, f);
  byteWrite2BE(ext_temp_counts, f);
  byteWrite2BE(int_temp_counts, f);
  byteWrite2BE(c_ref_dark_counts, f);
  byteWrite2BE(c_sig_dark_counts, f);
  byteWrite4BE(msec_since_power, f);
  byteWrite1(reserved_2, f);
  byteWrite1(wave_num, f);
  for (int i=0;i<wave_num;i++) {
    byteWrite2BE(cref[i], f);
    byteWrite2BE(aref[i], f);
    byteWrite2BE(csig[i], f);
    byteWrite2BE(asig[i], f);  
  }
  byteWrite2BE(check_sum, f);  
  byteWrite1(pad_byte, f);
}
*/

