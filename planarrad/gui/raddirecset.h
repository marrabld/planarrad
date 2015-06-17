
#ifndef _PLANARRAD_RADDIRECSET_H
#define _PLANARRAD_RADDIRECSET_H

#include "base/jtexttable.h"
#include "rad/sbanddata.h"
#include "rad/raddirectable.h"
#include <vector>

using namespace jude::base;
using namespace jude::rad;



class DepthRadDirecData {

  double d;
  RadDirecTable<SBandData, SpherePartVxH>* rd;

public:

  DepthRadDirecData(double d, int band_count, const SpherePartVxH* ds) : d(d) {
    rd = new RadDirecTable<SBandData, SpherePartVxH>(*ds, band_count);
    rd->makeAll(SBandData(band_count)); 
  }

  double depth() const { return d; }

  RadDirecTable<SBandData, SpherePartVxH>* radDirecData() { return rd; }
  const RadDirecTable<SBandData, SpherePartVxH>* radDirecData() const { return rd; }

  static bool sortPredicate(const DepthRadDirecData* ds1, const DepthRadDirecData* ds2) { return ds1->depth() < ds2->depth(); }
};



class RadDirecSet {

  BandSpec* bs;
  SpherePartVxH* ds;

  mutable std::vector< DepthRadDirecData* > l_sample_list;

  bool has_sample_data;

  /*
  bool has_data;

  double min_sample_value;
  double max_sample_value;

  double min_log10_sample_value;
  double max_log10_sample_value;
  */

  mutable bool samples_are_sorted;

  //void load(SBandData& sd, const JTextTable& ttab, int start_row, int start_col, int band_count);

  void verifySorted() const;

public:

  RadDirecSet();
  ~RadDirecSet();
 
  BandSpec* bandSpec() const { return bs; }
 
  void setBandSpec(const BandSpec* b) { bs = new BandSpec(*b); }
  void setDirecSpec(SpherePartVxH* d) { ds = d; }

  bool hasSampleData() const { return l_sample_list.size() > 0; }

  RadDirecTable<SBandData, SpherePartVxH>* findOrCreate(double depth);

  RadDirecImp<SBandData>* sampleInterpolated(double d) const;

  //void initValueRange();
  //void finaliseValueRange();

  //double minSampleValue() const { return min_sample_value; }
  //double maxSampleValue() const { return max_sample_value; }

  //double minLog10SampleValue() const { return min_log10_sample_value; }
  //double maxLog10SampleValue() const { return max_log10_sample_value; }

  //void loadSample(double d, const JTextTable& ttab, int start_row, int start_col, int band_count);

  //void getProfileData(vector<double>& depth_data, vector<double>& val_data, double waveln) const;
 
  //const SBandData& sdAir() const { return sd_a; }
  //const SBandData& sdWater() const { return sd_w; }
  //const SBandData& sdBottom() const { return sd_b; }

  //void sampleInterpolated(vector<double>& waveln_data, vector<double>& val_data, double d) const;
};



#endif // _PLANARRAD_RADDIRECSET_H

