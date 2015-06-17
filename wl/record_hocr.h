
#ifndef _JUDE_WL_RECORD_HOCR
#define _JUDE_WL_RECORD_HOCR

#include "record.h"
#include "base/jtypes.h"
#include "math/ublas.h"

using namespace jude::base;
using namespace jude::math;

namespace jude {
namespace wl {


class Sensor_HOCR;



class Record_HOCR : public Record {

  typedef enum { UNKNOWN=0, SATHPE, SATPED, SATHPL, SATPLD } inst_type;
 
  // SATHPE Ed light
  // SATPED Ed dark
  // SATHPL Lu light
  // SATPLD Lu dark

  inst_type it;
  int curr_pos;
  int bytes_read_count;

  uns8 data_bytes[427];

  double time_secs;

  // processed

  bool has_processed_ed_and_lu;
  bool has_processed_es;

  vector<double> ed_data;
  vector<double> lu_data;

  vector<double> es_data;

  int processed_ed_time_stamp;

  JString ed_timetag2;
  JString lu_timetag2;
  JString es_timetag2;

  void dataText(const char* str, char *buff, int buff_len) const;

public:

    /*! \brief Constructor, create an empty record for sensor \a s. */
  Record_HOCR(const Sensor_HOCR* s);
    /*! \brief Desstructor. */
  ~Record_HOCR();

  void appendBytes(const uns8*& ptr, int& bytes_left);

  //bool read(ByteStream& bs, int& bytes_read);

  bool isEdLight() const { return it==SATHPE; }
  bool isEdDark() const { return it==SATPED; }
  bool isLuLight() const { return it==SATHPL; }
  bool isLuDark() const { return it==SATPLD; }

  const uns32 rawData(int b) const { 
    const uns8* ptr = data_bytes + 15 + (b<<1); 
    return ((uns32)(*ptr)) + (((uns32)*(ptr+1)) << 8);
  }

  int rawDataBandCount() const { return 194; }

  const JString typeText() const;

  const JString instrument() const { return JString((const char*)data_bytes, 6); }
  const JString serialNo() const { return JString((const char*)data_bytes+6, 4); }
  uns32 intTime() const { return ((uns32)*(data_bytes+10)) + (((uns32)*(data_bytes+11)) << 8); }
  uns32 sampleDelay() const { return ((uns32)*(data_bytes+12)) + (((uns32)*(data_bytes+13)) << 8); }
  uns32 darkSamp() const { return data_bytes[404]; }
  uns32 darkAverage() const { return ((uns32)*(data_bytes+405)) + (((uns32)*(data_bytes+406)) << 8); }
  uns32 frameCounter() const { return data_bytes[413]; }
  const JString timer() const { return JString((const char*)data_bytes+414, 10); }
  uns32 checkSum() const { return data_bytes[424]; }
 
  bool isProcessed() const { return has_processed_ed_and_lu && has_processed_es; }

  bool hasProcessedEs() const { return has_processed_es; }
  bool hasProcessedEdAndLu() const { return has_processed_ed_and_lu; }

  void setHasProcessedEs(bool f) { has_processed_es = f; }
  void setHasProcessedEdAndLu(bool f) { has_processed_ed_and_lu = f; }

  void clearProcessed();
  void clearProcessedEdAndLu();
  void clearProcessedEs();

  vector<double>& edData() { return ed_data; }
  const vector<double>& edData() const { return ed_data; }

  vector<double>& luData() { return lu_data; }
  const vector<double>& luData() const { return lu_data; }

  vector<double>& esData() { return es_data; }
  const vector<double>& esData() const { return es_data; }

  void writeBytes(JDataStream& str) const;

  void writeProcessedHeader(JTextStream& str) const;
  void writeProcessed(JTextStream& str) const;

  int processedColumnsCount() const;
  const JString processedHeader(int i) const;
  const JString processed(int i) const;

  int processedEdTimeStamp() const { return processed_ed_time_stamp; }
  void setProcessedEdTimeStamp(int i) { processed_ed_time_stamp = i; }
 
  void setEdTimetag2(const JString& s) { ed_timetag2 = s; } 
  void setLuTimetag2(const JString& s) { lu_timetag2 = s; } 
  void setEsTimetag2(const JString& s) { es_timetag2 = s; } 
 
  const JString& edTimetag2() const { return ed_timetag2; } 
  const JString& luTimetag2() const { return lu_timetag2; } 
  const JString& esTimetag2() const { return es_timetag2; } 
 
  const char* textDisplayLine(int i) const;

  void display() const;
};



} } // namespace jude::wl

#endif // _RECORD_HOCR_JDH

