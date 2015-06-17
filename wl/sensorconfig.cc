
#include "wl/sensorconfig.h"
#include "wl/sensor_rawdata.h"
#include "wl/sensor_acs.h"
#include "wl/sensor_ctd.h"
#include "wl/sensor_ecobb3.h"
#include "wl/sensor_hocr.h"

using namespace jude::wl;



// SensorConfig::(constructor)
// default constructor just puts a raw data reader on each channel

SensorConfig::SensorConfig() {
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) sensor_array[i] = new Sensor_RawData(i);
  status_ok = true;
}



// SensorConfig::(constructor)

SensorConfig::SensorConfig(const JParser& prm) {
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) sensor_array[i] = 0;
  buildFromParser(prm);
}



// SensorConfig::buildFromParser()

bool SensorConfig::buildFromParser(const JParser& prm) {

  status_ok = false;

    // get the basic sensor configuration - this is irrelavent if operatiuon is 'preview'
  sensor_setup = prm.getStringOrEmpty("sensor_setup");
    // the default if unspecified is "fsf"
  if (sensor_setup.isEmpty()) sensor_setup = "fsf";
    // set up the sensor configuration or quit with error
  if (sensor_setup == "raw") setup_raw(prm);
  else if (sensor_setup == "fsf") setup_fsf(prm);
  else if (sensor_setup == "fsf_no_hocr") setup_fsf_no_hocr(prm);
  else if (sensor_setup == "custom") setup_custom(prm);
  else {
    jlog::es << "Sensor set-up '" << sensor_setup << "' is not supported.\n";
    return false;
  } 
    // report
  if (jlog::vb(3)) jlog::ls << "Sensor set-up: " << sensor_setup << "\n";

    // this will return false only if a device file was supplied but is invalid
  if (!readSensorDeviceFiles(prm)) return false;

  status_ok = true;

  return true;
}



// SensorConfig::clearParser() [static]

void SensorConfig::clearParser(JParser& prm) {
    // remove sensor setup code
  prm.removeLabel("sensor_setup"); 
    // remove any previous channel specs
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    JString chanX = JString("chan" + JString::number(i+1));
    prm.removeLabel(chanX+"_sensor");
    prm.removeLabel(chanX+"_device_fp");
  }
}



void SensorConfig::writeToParser(JParser& prm) const {
  clearParser(prm);
  prm.setLabelAndValue("sensor_setup", sensor_setup);

    // an fsf setup
  if (sensor_setup == "fsf" || sensor_setup == "fsf_no_hocr") {
    if (!sensor_array[0]->deviceFilepath().isEmpty()) prm.setLabelAndValue("chan1_device_fp", sensor_array[0]->deviceFilepath());
  }

    // custom specifies all channels
  if (sensor_setup == "custom") {
    for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
      if (sensor_array[0]->sensorType() == "RawData") continue;
      JString chanX = JString("chan" + JString::number(i+1));
      prm.setLabelAndValue(chanX+"_sensor", sensor_array[i]->sensorType());
      if (!sensor_array[i]->deviceFilepath().isEmpty()) prm.setLabelAndValue(chanX+"_device_fp", sensor_array[i]->deviceFilepath());
    }
  }
}



// SensorConfig::(destructor)
// clean up

SensorConfig::~SensorConfig() {
  deleteAll();
}



// SensorConfig::deleteAll() [private]

void SensorConfig::deleteAll() {
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    delete sensor_array[i];
    sensor_array[i] = 0;
  }
}



// SensorConfig::setToRawFrom() [private]

void SensorConfig::setToRawFrom(int start_index) {
  for (int i=start_index; i<HARDWARE_CHANNEL_COUNT; i++) {
    delete sensor_array[i];
    sensor_array[i] = new Sensor_RawData(i);
  }
}



// SensorConfig::setSensor()

void SensorConfig::setSensor(int i, Sensor* s) {
  delete sensor_array[i];
  if (s==0) sensor_array[i] = new Sensor_RawData(i);
  else {
    sensor_array[i] = s;
    s->setChannelIndex(i);
  }
}


void SensorConfig::clearAllProcessed() {
 
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {

    Sensor_HOCR* sens_hocr = dynamic_cast<Sensor_HOCR*>(sensor_array[i]);
    if (!sens_hocr) continue;

    sens_hocr->clearProcessedEdAndLu();
    sens_hocr->clearProcessedEs();
  }
}


bool SensorConfig::setup_raw(const JParser& prm) {
  setToRawFrom(0);
  return true;
} 



bool SensorConfig::setup_fsf(const JParser& prm) {
  deleteAll();
  setSensor(0, new Sensor_ACS(0));
  setSensor(1, new Sensor_CTD(1));
  setSensor(2, new Sensor_ECOBB3(2));
  setSensor(3, new Sensor_HOCR(3));
  setToRawFrom(4);
  return true;
}



bool SensorConfig::setup_fsf_no_hocr(const JParser& prm) {
  deleteAll();
  setSensor(0, new Sensor_ACS(0));
  setSensor(1, new Sensor_CTD(1));
  setSensor(2, new Sensor_ECOBB3(2));
  setToRawFrom(3); 
  return true;
}



bool SensorConfig::setup_custom(const JParser& prm) {

  deleteAll();

  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {

    JString type(prm.getStringOrEmpty("chan"+JString::number(i+1)+"_sensor"));

    if (type == "ACS") setSensor(i, new Sensor_ACS(i));
    else if (type == "CTD") setSensor(i, new Sensor_CTD(i));
    else if (type == "ECOBB3") setSensor(i, new Sensor_ECOBB3(i));
    else if (type == "HOCR") setSensor(i, new Sensor_HOCR(i));
    else setSensor(i, new Sensor_RawData(i));
  }

  return true;
} 



// SensorConfig::readSensorDeviceFiles()

bool SensorConfig::readSensorDeviceFiles(const JParser& prm) {
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    JString fp(prm.getStringOrEmpty("chan"+JString::number(i+1)+"_device_fp"));
    if (fp.isEmpty()) continue; 
    if (!sensor_array[i]->readDeviceFile(fp)) {
      jlog::es << "There was a problem reading the device file '" << fp << "' check the file format.\n";
      return false;
    }
  }
  return true;
}

