
#include "specset.h"



SpecSet::SpecSet() : 
  bs(0),
  has_data(false), 
  has_sample_data(false), 
  min_sample_value(0),
  max_sample_value(0),
  min_log10_sample_value(-100),
  max_log10_sample_value(100),
  max_depth(0),
  samples_are_sorted(false) { }



SpecSet::~SpecSet() {
  delete bs;
}



void SpecSet::load(SBandData& sd, const JTextTable& ttab, int start_row, int start_col, int band_count) {

  sd = SBandData(band_count);

  for (int i=0; i<band_count; i++) sd.bandValue(i) = ttab.getDoubleFromArray(start_row, start_col+i);

  has_data = true;
}



void SpecSet::initValueRange() {
  min_sample_value = 0;
  max_sample_value = 0;
  min_log10_sample_value = 100;
  max_log10_sample_value = -100;
  max_depth = 0;
}



void SpecSet::loadSample(double d, const JTextTable& ttab, int start_row, int start_col, int band_count) {

  DepthSBandData* ds = new DepthSBandData(d, band_count);
 
  for (int i=0; i<band_count; i++) {

    double v = ttab.getDoubleFromArray(start_row, start_col+i);
    ds->sBandData().bandValue(i) = v;

    //jlog::ls << "start_row " << start_row << " c " << start_col+i << " " << v << "\n";

    if (v > max_sample_value) max_sample_value = v;
    if (v < min_sample_value) min_sample_value = v;
    //jlog::ls << i << " " << v << " " << max_sample_value << "\n";
    if (v > 0) {
      v = log10(v);
      if (v > max_log10_sample_value) max_log10_sample_value = v;
      if (v < min_log10_sample_value) min_log10_sample_value = v;
    }
  }
  //exit(0);
  sd_sample_list.push_back(ds);

  if (d > max_depth) max_depth = d;
  has_sample_data = true;
  samples_are_sorted = false;
}



void SpecSet::finaliseValueRange() {
  if (min_log10_sample_value == 100) min_log10_sample_value = -100;
}



void SpecSet::verifySorted() const {

  if (samples_are_sorted) return;

  std::sort(sd_sample_list.begin(), sd_sample_list.end(), DepthSBandData::sortPredicate);
  samples_are_sorted = true;
}




void SpecSet::getProfileData(vector<double>& depth_data, vector<double>& val_data, double waveln) const {

  //jlog::ls << "get profile data " << (int) sd_sample_list.size() << "\n";

  depth_data.resize(sd_sample_list.size());
  val_data.resize(sd_sample_list.size());

  if (sd_sample_list.size() == 0) return;

  verifySorted();

  int index = 0;
  double frac = 0;
  double curr_w = bs->bandCentres()(0);

    // get base band and interpolated fraction
  for (; index<((int)bs->bandCount())-1; index++) {

    double prev_w = curr_w;
    curr_w = bs->bandCentres()(index+1);

    if (curr_w > waveln && prev_w <= waveln) {
      frac = (waveln - prev_w) / (curr_w - prev_w);
      break;
    }
  }

  // jlog::ls << "here\n";

  for (unsigned int i=0; i<sd_sample_list.size(); i++) {

    depth_data(i) = sd_sample_list[i]->depth();

    const SBandData& sd(sd_sample_list[i]->sBandData());
    //jlog::ls << sd.vectorData() << "\n";

    //val_data(i) = sd.bandValue(0);
  
    if (frac==0) val_data(i) = sd.bandValue(index);
    else val_data(i) = sd.bandValue(index) * (1 - frac) + sd.bandValue(index+1) * frac;
  }
  //jlog::ls << "vv\n" << val_data << "\n";
}



void SpecSet::sampleInterpolated(vector<double>& waveln_data, vector<double>& val_data, double d) const {

  waveln_data.resize(bs->bandCount());
  val_data.resize(bs->bandCount());

  if (sd_sample_list.size() == 0) return;

  verifySorted();

  SBandData sd(sd_sample_list[0]->sBandData());

  double curr_d = sd_sample_list[0]->depth();

  for (unsigned int i=1; i<sd_sample_list.size(); i++) {

    double prev_d = curr_d;
    curr_d = sd_sample_list[i]->depth();

    if (curr_d > d && prev_d <= d) {
      double frac = (d - prev_d) / (curr_d - prev_d);
      sd = sd_sample_list[i-1]->sBandData() * (1-frac) + sd_sample_list[i]->sBandData() * frac;
      break;
    }
  }

  if (d >= curr_d) sd = sd_sample_list[sd_sample_list.size()-1]->sBandData();

  for (int i=0; i<bs->bandCount(); i++) {
    waveln_data(i) = bs->bandCentres()(i);
    val_data(i) = sd.bandValue(i);
  }
}




bool SpecSet::readFromDataFile(const JString& code, const JTextTable& ttab, int depth_col, double& min_waveln, double& max_waveln) {

  JList<double> waveln_list;
  JList<int> col_list;

  for (int i=0; i<ttab.columnCount(); i++) {
    JString s = ttab.getTextFromArray(0,i);
    if (s.left(code.length()).lower() != code) continue;
    double waveln = s.right(s.length() - code.length()).toDouble();
    waveln_list.append(new double(waveln));
    col_list.append(new int(i));
  }

  int band_count = waveln_list.count();

    // no data but no error
  if (band_count == 0) return true;

  double waveln_array[waveln_list.count()];
  double col_array[col_list.count()];

  JListIterator<double> waveln_iter(waveln_list);
  JListIterator<int> col_iter(col_list);
  double* waveln_ptr = waveln_iter.first();
  int* col_ptr = col_iter.first();
  int curr_band = 0;
  while (waveln_ptr) {
    waveln_array[curr_band] = *waveln_ptr;
    col_array[curr_band] = *col_ptr;
    waveln_ptr = waveln_iter.next();
    col_ptr = col_iter.next();
    curr_band++;
  }

  bs = new BandSpec(band_count, waveln_array);

  if (minWavelength() < min_waveln) min_waveln = minWavelength();
  if (maxWavelength() > max_waveln) max_waveln = maxWavelength();

  for (int row=1; row<ttab.rowCount(); row++) {
    double depth = ttab.getDoubleFromArray(row, depth_col);
    loadSample(depth, ttab, row, col_array[0], band_count);
  }

  return true;
}

