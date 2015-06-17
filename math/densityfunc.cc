
#include "densityfunc.h"
#include "base/jfile.h"

using namespace jude::math;



// DensityFunc::(constructor)

DensityFunc::DensityFunc(double x_min, double x_max, int bin_count) :
  x_min(x_min),
  x_max(x_max) {

  bin_weight.resize(bin_count);
  clear();
}



// DensityFunc::(destructor)

DensityFunc::~DensityFunc() {
  // nothing to do
}



// DensityFunc::clear()

void DensityFunc::clear() {
  under_weight_sum = 0;
  over_weight_sum = 0;
  total_weight = 0;
  max_x_seen = 0;
  min_x_seen = 0;
  for (unsigned int i=0; i<bin_weight.size(); i++) bin_weight[i] = 0;
}



// DensityFunc::addWeight()

void DensityFunc::addWeight(double x, double w) {

  if (total_weight == 0) {
    max_x_seen = x;
    min_x_seen = x;
  } else {
    if (x > max_x_seen) max_x_seen = x;
    if (x < min_x_seen) min_x_seen = x; 
  }

  total_weight += w;

  if (x < x_min) under_weight_sum += w;
  else if (x >= x_max) over_weight_sum += w;
  else {
    int i = (int) ((bin_weight.size() * (x - x_min)) / (x_max - x_min));
    bin_weight[i] += w; 
  }
}



// DensityFunc::maxXSeen()

double DensityFunc::maxXSeen() const {
  return max_x_seen;
}



// DensityFunc::minXSeen()

double DensityFunc::minXSeen() const {
  return min_x_seen;
}



// DensityFunc::binXLowLimit()

double DensityFunc::binXLowLimit(int i) const {
  return x_min + (i * (x_max - x_min)) / binCount();
}



// DensityFunc::binXHighLimit()

double DensityFunc::binXHighLimit(int i) const {
  return x_min + ((i+1) * (x_max - x_min)) / binCount();
}



// DensityFunc::binXCentre()

double DensityFunc::binXCentre(int i) const {
  return x_min + ((i+0.5) * (x_max - x_min)) / binCount();
}



// DensityFunc::upperTail()

double DensityFunc::upperTail(double frac) const {

  double thresh = total_weight * frac;

  if (over_weight_sum > thresh) return x_max;

  double sum = 0;
  for (int i=binCount()-1; i>=0; i--) {
    sum += bin_weight[i];
    if (sum > thresh) return binXCentre(i);
  }

  return x_min;
}



// DensityFunc::writeDataForSteppedPlot()

void DensityFunc::writeDataForSteppedPlot(JTextStream& str) const {

  double x_lo = x_min;

  for (int i=0; i<binCount(); i++) {

    double x_hi = x_min + ((i+1) * (x_max - x_min)) / binCount();

    //double v = bin_weight[i];
    //if (data_code == TRACK_MEAN) if (data_count[i]) v /= data_count[i];

    str << x_lo << " " << bin_weight[i] << "\n";
    str << x_hi << " " << bin_weight[i] << "\n";

    x_lo = x_hi;
  }
}



// DensityFunc::writeFileForSteppedPlot()

void DensityFunc::writeFileForSteppedPlot(const JString& filepath) const {
  JFile f(filepath);
  f.open(JIO_WriteOnly);
  JTextStream str(&f);
  writeDataForSteppedPlot(str);
  f.close();
}



// DensityFunc::writeDataForPlot()

void DensityFunc::writeDataForPlot(JTextStream& str) const {

  double x_lo = x_min;

  for (int i=0; i<binCount(); i++) {

    double x_hi = x_min + ((i+1) * (x_max - x_min)) / binCount();

    //double v = data_value[i];
    //if (data_code == TRACK_MEAN) if (data_count[i]) v /= data_count[i];

    str << ((x_lo + x_hi)/2) << " " << bin_weight[i] << "\n";
  
    x_lo = x_hi;
  }
}



// DensityFunc::writeFileForPlot()

void DensityFunc::writeFileForPlot(const JString& filepath) const {
  JFile f(filepath);
  f.open(JIO_WriteOnly);
  JTextStream str(&f);
  writeDataForPlot(str);
  f.close();
}

