
#include "wl_testing.h"
#include "base/jlog.h"
#include "base/jimagesave.h"
#include "base/jbuffer.h"
#include "base/jcolour.h"
#include "base/jtypes.h"
#include "base/jfile.h"
#include "wl/sensorconfig.h"
#include "wl/archivefile.h"

using namespace jude::wl;



void WL_Testing::runTests(int& total_count, int& fail_count) {

  int tc = 0;
  int fc = 0;

  //WLConfig::init(Sensor::ACS, Sensor::CTD, Sensor::ECOBB3, Sensor::CTD);

  SensorConfig sc;
  ArchiveFile af(sc);
  af.readPackets("archive.042", true);
  //af.display();

  //WLConfig::clear();
 
  jlog::ls << "WL Testing passed " << tc-fc << " of " << tc << " tests.\n\n";
  
  total_count+= tc;
  fail_count += fc;
}

