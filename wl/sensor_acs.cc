
#include "sensor_acs.h"
#include "record_acs.h"
#include "base/jtexttable.h"
#include "base/jlog.h"

using namespace jude::wl;
using namespace jude::math;



Sensor_ACS::Sensor_ACS(int ci) :
  Sensor(ci), 
  band_count(0), 
  c_temp_data(0),
  a_temp_data(0),
  has_device_file(false) { }



Sensor_ACS::~Sensor_ACS() { 
  delete[] c_temp_data; 
  delete[] a_temp_data; 
}
  


// Sensor_ACS::readDeviceFile()

bool Sensor_ACS::readDeviceFile(const JString& filepath) {

  has_device_file = false;
  device_filepath = filepath;
 
  setNameText("ACS");

  if (jlog::vbLevel(1)) jlog::ls << "Reading device file '" << filepath << "'\n"; 

  int row_count;
  int max_cols;
  bool is_consistent;

  JTextTable::getSize(row_count, max_cols, is_consistent, filepath, '\t', false);

  if (jlog::vbLevel(3)) jlog::ls << "Rows: " << row_count << " Columns: " << max_cols << "\n"; 

    // table is not big enough or far too big, must be some problem
  if (row_count<10 || row_count>200) return false;

  JTextTable ttab(max_cols);
  ttab.setSeperator('\t');
  if (!ttab.readFromFile(filepath)) return false;

  if (jlog::vbLevel(5)) {
    ttab.display();
  }

  band_count = ttab.getText(7,0).toInt();
  temp_bin_count = ttab.getText(8,0).toInt();

  if (jlog::vbLevel(3)) {
    jlog::ls << "Band count: " << band_count << "\n";
    jlog::ls << "Temperature bin count: " << temp_bin_count << "\n";
  }

    // table is not big enough, must be some problem
  if (row_count < 11+band_count || max_cols < 7+temp_bin_count) return false;

  temp_array.resize(temp_bin_count);
    // table to hold the temperature correction data
  delete[] c_temp_data;
  delete[] a_temp_data;
  c_temp_data = new vector<double>[band_count];
  a_temp_data = new vector<double>[band_count];

  c_band_wavelength.resize(band_count);
  a_band_wavelength.resize(band_count);

  c_offset.resize(band_count);
  a_offset.resize(band_count);

  for (int i=0; i<temp_bin_count; i++) temp_array(i) = ttab.getText(9,i+5).toDouble();

    // not enough columns for what about to read
  if (max_cols < 7+2*temp_bin_count) return false;

  for (int b=0; b<band_count; b++) {

    JString s(ttab.getText(10+b, 0));
    c_band_wavelength(b) = s.right(s.length()-1).toDouble();

    s = ttab.getText(10+b, 1);
    a_band_wavelength(b) = s.right(s.length()-1).toDouble();

    c_offset(b) = ttab.getText(10+b, 3).toDouble();
    a_offset(b) = ttab.getText(10+b, 4).toDouble();
  
    c_temp_data[b].resize(temp_bin_count);
    a_temp_data[b].resize(temp_bin_count);

    for (int i=0; i<temp_bin_count; i++) {
      c_temp_data[b](i) = ttab.getText(10+b,i+6).toDouble();
      a_temp_data[b](i) = ttab.getText(10+b,i+7+temp_bin_count).toDouble();
    }
  }

  if (jlog::vbLevel(3)) {
    jlog::ls << "band count: " << band_count << "\n";
    jlog::ls << "c-wavelengths:\n" << c_band_wavelength << "\n";
    jlog::ls << "a-wavelengths:\n" << a_band_wavelength << "\n";
    jlog::ls << "c-offset:\n" << c_offset << "\n";
    jlog::ls << "a-offset:\n" << a_offset << "\n";
    jlog::ls << "Temperature points:\n" << temp_array << "\n";
    for (int j=0; j<band_count; j++) {
      jlog::ls << "c-temperature data band " << (j+1) << ":\n" << c_temp_data[j] << "\n";
      jlog::ls << "a-temperature data band " << (j+1) << ":\n" << a_temp_data[j] << "\n";
    }
  }

  has_device_file = true;

  return true;
}

