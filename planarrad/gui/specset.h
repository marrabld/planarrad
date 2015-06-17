
#ifndef _PLANARRAD_SPECSET_H
#define _PLANARRAD_SPECSET_H

#include "base/jtexttable.h"
#include "rad/sbanddata.h"
#include <vector>

using namespace jude::base;
using namespace jude::rad;



class DepthSBandData {

  double d;
  SBandData sd;

public:

  DepthSBandData(double d, int band_count) : d(d), sd(band_count) { }

  double depth() const { return d; }

  SBandData& sBandData() { return sd; }
  const SBandData& sBandData() const { return sd; }

  static bool sortPredicate(const DepthSBandData* ds1, const DepthSBandData* ds2) { return ds1->depth() < ds2->depth(); }
};




class SpecSet {

  BandSpec* bs;

  SBandData sd_a;
  SBandData sd_w;
  SBandData sd_b;

  mutable std::vector<DepthSBandData*> sd_sample_list;

  bool has_data;
  bool has_sample_data;

  double min_sample_value;
  double max_sample_value;

  double min_log10_sample_value;
  double max_log10_sample_value;

  double max_depth;

  mutable bool samples_are_sorted;

  void load(SBandData& sd, const JTextTable& ttab, int start_row, int start_col, int band_count);

  void verifySorted() const;

public:

  SpecSet();
  ~SpecSet();

  bool hasData() const { return has_data; }

  bool hasSampleData() const { return has_sample_data; }

  void initValueRange();
  void finaliseValueRange();

  double minSampleValue() const { return min_sample_value; }
  double maxSampleValue() const { return max_sample_value; }

  double minLog10SampleValue() const { return min_log10_sample_value; }
  double maxLog10SampleValue() const { return max_log10_sample_value; }

  double minWavelength() const { return bs->bandCentres()(0); }
  double maxWavelength() const { return bs->bandCentres()(bs->bandCount()-1); }

  double maxDepth() const { return max_depth; }

  void setBandSpec(const BandSpec* b) { bs = new BandSpec(*b); }

  void loadAir(const JTextTable& ttab, int start_row, int start_col, int band_count) { 
    load(sd_a, ttab, start_row, start_col, band_count); }

  void loadWater(const JTextTable& ttab, int start_row, int start_col, int band_count) { 
    load(sd_w, ttab, start_row, start_col, band_count); }

  void loadBottom(const JTextTable& ttab, int start_row, int start_col, int band_count) { 
    load(sd_b, ttab, start_row, start_col, band_count); }

  void loadSample(double d, const JTextTable& ttab, int start_row, int start_col, int band_count);

  void getProfileData(vector<double>& depth_data, vector<double>& val_data, double waveln) const;
 
  const SBandData& sdAir() const { return sd_a; }
  const SBandData& sdWater() const { return sd_w; }
  const SBandData& sdBottom() const { return sd_b; }

  void sampleInterpolated(vector<double>& waveln_data, vector<double>& val_data, double d) const;

  bool readFromDataFile(const JString& code, const JTextTable& ttab, int depth_col, double& min_waveln, double& max_waveln);
};



#endif // _PLANARRAD_SPECSET_H

