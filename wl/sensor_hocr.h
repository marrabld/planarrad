
#ifndef _JUDE_WL_SENSOR_HOCR_H
#define _JUDE_WL_SENSOR_HOCR_H

#include "wl/sensor.h"
#include "wl/record_hocr.h"
#include "math/ublas.h"

namespace jude {
namespace wl {



// class to describe a data stream from Satlantic HyperOCR sensors

class Sensor_HOCR : public Sensor {

  bool do_ed_output;
  bool do_lu_output;
  bool do_es_output;

  int band_count;
  int es_band_count;

  vector<double> ed_band_wavelength;
  vector<double> lu_band_wavelength;
  vector<double> es_band_wavelength;

  double pct_records_processed;

  bool stop_processing;

  void writeHeaderRecord(const char* data, const char* ident, JDataStream& str) const;
  void writeHeader(JDataStream& str) const;

  int textTimeToInt(const JString& text_time);
  int absTimeDiff(int t1, int t2);

public:

    /*! \brief Constructor. */
  Sensor_HOCR(int ci) : Sensor(ci), do_ed_output(false), do_lu_output(false) { }
    /*! \brief Destructor. */
  ~Sensor_HOCR() { }
  
    /*! \brief Returns the type of this sensor, "HOCR". */
  const char* typeText() const { return "HOCR"; }
  
  const char* fileExtension() const { return ".raw"; }

  bool readDeviceFile(const JString& filepath) { return true; }
  
    /*! \brief Creates a record for this sensor. */
  Record_HOCR* createRecord() const { return new Record_HOCR(this); }

  bool loadProcessed(const JParser& prm, Channel& chan, int chan_index);
  bool loadProcessedEdAndLu(const JParser& prm, Channel& chan, int chan_index);
  bool loadProcessedEs(const JParser& prm, Channel& chan, int chan_index);

  void clearProcessedEdAndLu();
  void clearProcessedEs();

  bool hasProcessedEdAndLu() { return (int) (ed_band_wavelength.size()) != 0; }
  bool hasProcessedEs() { return (int) (es_band_wavelength.size()) != 0; }

  void prepareForMerge(const JParser& prm, int chan_index);

  void stopProcessing() { stop_processing = true; }

  bool doEdOutput() const { return do_ed_output; }
  bool doLuOutput() const { return do_lu_output; }
  bool doEsOutput() const { return do_es_output; }

  int bandCount() const { return band_count; }
  int esBandCount() const { return es_band_count; }

  const vector<double>& EdBandWavelength() const { return ed_band_wavelength; }
  const vector<double>& LuBandWavelength() const { return lu_band_wavelength; }
  const vector<double>& EsBandWavelength() const { return es_band_wavelength; }

  double pctRecordsProcessed() const { return pct_records_processed; }
};



} } // namespace jude::wl

#endif // _JUDE_WL_SENSOR_HOCR_H

