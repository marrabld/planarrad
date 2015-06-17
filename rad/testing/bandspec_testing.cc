
#include "bandspec_testing.h"
#include "base/jlog.h"

using namespace jude::base;
using namespace jude::rad;



void BandSpec_Testing::runTests(int& total_count, int& fail_count) {

  int tc = 0;
  int fc = 0;
 
  jlog::ls << "BandSpec testing passed " << tc-fc << " of " << tc << " tests.\n\n";
  

  double cen_17[17] = { 410,430,450,470,490,510,530,550,570,590,610,630,650,670,690,710,730 };
  double wid_17[17] = { 20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20 };
  BandSpec bs_targ(17, cen_17, wid_17);

  double cen_1000[1000];
  for (int i=0; i<1000; i++) cen_1000[i] = 400.0 + (i * (740-400)) / 1000.0;
  BandSpec bs_src1(1000, cen_1000);

  SBandData sd_src1(1000);
  for (int i=0; i<1000; i++) sd_src1[i] = (1.0 + cos((i*4.0*M_PI)/1000)) / 2;

  BandSpec bs_src2(bs_src1, 100, 590);
  SBandData sd_src2(sd_src1, 100, 590);

  SBandData sd1(bs_targ.resampleToThis(sd_src1, bs_src1));
  SBandData sd2(bs_targ.resampleToThis(sd_src2, bs_src2));

  writePlotFile("data1.txt", sd_src1, bs_src1, sd1, bs_targ);
  writePlotFile("data2.txt", sd_src2, bs_src2, sd2, bs_targ);

  total_count+= tc;
  fail_count += fc;
}



void BandSpec_Testing::writePlotFile(const JString& fp, const SBandData& sd1, const BandSpec& bs1, const SBandData& sd2, const BandSpec& bs2) {

  JFile f(fp);
  f.open(JIO_WriteOnly);

  JTextStream str(&f);

  for (int i=0; i<bs1.bandCount(); i++) {
    str << bs1.bandLow()(i) << "\t" << sd1[i] << "\n";
    str << bs1.bandHigh()(i) << "\t" << sd1[i] << "\n";
  }

  str << "\n\n";
 
  for (int i=0; i<bs2.bandCount(); i++) {
    str << bs2.bandLow()(i) << "\t" << sd2[i] << "\n";
    str << bs2.bandHigh()(i) << "\t" << sd2[i] << "\n";
  }
 
  f.close();
}

