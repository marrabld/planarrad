
#ifndef _JUDE_WL_SENSOR_ACS_H
#define _JUDE_WL_SENSOR_ACS_H

#include "wl/sensor.h"
#include "wl/record_acs.h"
#include "math/ublas.h"
#include "math/ipol.h"

using namespace jude::math;

namespace jude {
namespace wl {



// class to describe an ACS sensor

class Sensor_ACS : public Sensor {

    // from the device file, if zero means has no device file
  int band_count;

  int temp_bin_count;

  vector<double> temp_array;

  vector<double>* c_temp_data;
  vector<double>* a_temp_data;

  vector<double> c_band_wavelength;
  vector<double> a_band_wavelength;
 
  vector<double> c_offset;
  vector<double> a_offset;

  JString device_filepath;
  bool has_device_file;

public:

  Sensor_ACS(int ci);
  ~Sensor_ACS();
  
    /*! \brief Returns the type of this sensor, "ACS". */
  const char* typeText() const { return "ACS"; }

  const char* fileExtension() const { return ".raw"; }
  
  bool readDeviceFile(const JString& filepath);

  const JString& deviceFilepath() const { return device_filepath; }
  
    /*! \brief Creates a record for this sensor. */
  Record_ACS* createRecord() const { return new Record_ACS(this); }
 
    /*! \brief Returns true if a device file has been read for this sensor. */
  bool hasDeviceFile() const { return has_device_file; }

    /*! \brief Returns the number of bands of this ACS sensor. */
  int bandCount() const { return band_count; }

  double calcC(uns16 cref, uns16 csig, int b) const { return c_offset(b) - log(((double)csig)/cref)/0.25; }
  double calcA(uns16 aref, uns16 asig, int b) const { return a_offset(b) - log(((double)asig)/aref)/0.25; }

  double deltaTC(double temp, int b) const { return Ipol::linearInterpolate(temp, c_temp_data[b], temp_array); }
  double deltaTA(double temp, int b) const { return Ipol::linearInterpolate(temp, a_temp_data[b], temp_array); }

  double cBandWavelength(int i) const { return c_band_wavelength(i); }
  double aBandWavelength(int i) const { return a_band_wavelength(i); }

  const vector<double>& aBandWavelength() const { return a_band_wavelength; }
  const vector<double>& cBandWavelength() const { return c_band_wavelength; }
};



} } // namespace jude::wl

#endif // _JUDE_WL_SENSOR_ACS_H

