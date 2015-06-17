
#include "raddirecset.h"


RadDirecSet::RadDirecSet() : 
  bs(0),
  samples_are_sorted(false) { }




RadDirecSet::~RadDirecSet() {
  delete bs;
}



void RadDirecSet::verifySorted() const {

  if (samples_are_sorted) return;

  std::sort(l_sample_list.begin(), l_sample_list.end(), DepthRadDirecData::sortPredicate);
  samples_are_sorted = true;
}




RadDirecTable<SBandData, SpherePartVxH>* RadDirecSet::findOrCreate(double depth) {

  for (unsigned int i=0; i<l_sample_list.size(); i++) {
    if (l_sample_list[i]->depth() == depth) return l_sample_list[i]->radDirecData();
  }

  DepthRadDirecData* drd = new DepthRadDirecData(depth, bs->bandCount(), ds);

  l_sample_list.push_back(drd);

  return drd->radDirecData();
}



RadDirecImp<SBandData>* RadDirecSet::sampleInterpolated(double d) const {

  if (l_sample_list.size() == 0) return 0;

  RadDirecImp<SBandData>* data = new RadDirecTable<SBandData, SpherePartVxH>(*ds, bs->bandCount());
  data->makeAll(SBandData(bs->bandCount(), 0));

  verifySorted();

  double curr_d = l_sample_list[0]->depth();

  for (unsigned int i=1; i<l_sample_list.size(); i++) {

    double prev_d = curr_d;
    curr_d = l_sample_list[i]->depth();

    if (curr_d > d && prev_d <= d) {
      double frac = (d - prev_d) / (curr_d - prev_d);
      data->copy(*l_sample_list[i-1]->radDirecData());
      data->multiply(1-frac);
      RadDirecTable<SBandData, SpherePartVxH> data2(*ds, bs->bandCount());
      data2.copy(*l_sample_list[i]->radDirecData());
      data2.multiply(frac);
      data->add(data2);
      break;
    }
  }

  if (d >= curr_d) data->copy(*l_sample_list[l_sample_list.size()-1]->radDirecData());

  return data;
}

