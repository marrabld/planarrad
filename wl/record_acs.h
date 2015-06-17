
#ifndef _RECORD_ACS_JDH
#define _RECORD_ACS_JDH

#include "record.h"
#include "base/jtypes.h"
#include "wl/blockstream.h"
#include "math/ublas.h"

using namespace jude::base;
using namespace jude::math;

namespace jude {
namespace wl {


class Sensor_ACS;



class Record_ACS : public Record {

  uns16 record_length;
  uns8 packet_type;
  uns8 reserved_1;
  uns8 meter_type;
  uns32 serial_number;
  uns16 a_ref_dark_counts;
  uns16 pressure_counts;
  uns16 a_sig_dark_counts;
  uns16 ext_temp_counts;
  uns16 int_temp_counts;
  uns16 c_ref_dark_counts;
  uns16 c_sig_dark_counts;
  uns32 msec_since_power;
  uns8 reserved_2;
  uns8 wave_num;

  uns16* cref;
  uns16* aref;
  uns16* csig;
  uns16* asig;
 
  uns16 check_sum;
  uns8 pad_byte;

  // processed data

  double int_temp;

  vector<double> c_corrected;
  vector<double> a_corrected;

  bool is_processed;

  bool buildRecord(int& final_pos);
  void findStart(BlockStream& bs, int& pos);

  void dataText(const char* str, char *buff, int buff_len, uns16* data) const;

public:

    /*! \brief Constructor, create an empty record for sensor \a s. */
  Record_ACS(const Sensor_ACS* s);
    /*! \brief Desstructor. */
  ~Record_ACS();

  bool isProcessed() const { return is_processed; }

  void appendBytes(const uns8*& ptr, int& bytes_left);

  bool read(ByteStream& bs, int& bytes_read);
  uns16 checkSumCalc() const;

  const vector<double>& cCorrected() const { return c_corrected; }
  const vector<double>& aCorrected() const { return a_corrected; }

  const char* textDisplayLine(int i) const;

  bool process();

  void writeBytes(JDataStream& str) const;

  void writeProcessedHeader(JTextStream& str) const;
  void writeProcessed(JTextStream& str) const;

  int processedColumnsCount() const;
  const JString processedHeader(int i) const;
  const JString processed(int i) const;

  void display() const;
};



} } // namespace jude::wl

#endif // _RECORD_ACS_JDH

