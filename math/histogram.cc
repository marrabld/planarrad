
#include "histogram.h"
#include "jfile.h"



// Histogram::(constructor)

Histogram::Histogram(double x_min, double x_max, int bin_count, int data_code) :
  x_min(x_min),
  x_max(x_max),
  bin_count(bin_count),
  data_code(data_code) { 

  data_count = new int[bin_count];
  data_value = new double[bin_count];
  clear();
}



// Histogram::(constructor)

Histogram::Histogram(int x_data_count, double* x_data, int bin_count, double ix_min, double ix_max) :
  x_min(ix_min),
  x_max(ix_max),
  bin_count(bin_count),
  data_code(TRACK_NOP) {

  if (x_min == x_max) {

    x_min = x_data[0];
    x_max = x_min;

    for (int i=1; i<x_data_count; i++) {
      if (x_data[i] > x_max) x_max = x_data[i];
      if (x_data[i] < x_min) x_min = x_data[i];
    }
  }

  data_count = new int[bin_count];
  data_value = new double[bin_count];
  clear();

  for (int i=0; i<x_data_count; i++) addCount(x_data[i]);
}
 


// Histogram::(destructor)

Histogram::~Histogram() {
  delete[] data_count;
  delete[] data_value;
}



// Histogram::clear()

void Histogram::clear() {
  under_count = 0;
  over_count = 0;
  total_count = 0;
  x_sum = 0;
  max_x_seen = 0;
  min_x_seen = 0;
  for (int i=0; i<bin_count; i++) {
    data_count[i] = 0;
    data_value[i] = 0;
  }
}



// Histogram::addCount()

void Histogram::addCount(double x) {
  
  if (total_count==0) {
    max_x_seen = x;
    min_x_seen = x;
  } else {
    if (x > max_x_seen) max_x_seen = x;
    if (x < min_x_seen) min_x_seen = x; 
  }
  total_count++;
  x_sum += x;

  if (x < x_min) under_count++;
  else if (x >= x_max) over_count++;
  else {
    int i = (int) ((bin_count * (x - x_min)) / (x_max - x_min));
    data_count[i]++;
  }
}



// Histogram::addCount()

void Histogram::addCountAndData(double x, double y) {

  x_sum += x;

  if (total_count==0) {
    max_x_seen = x;
    min_x_seen = x;
  } else {
    if (x > max_x_seen) max_x_seen = x;
    if (x < min_x_seen) min_x_seen = x; 
  }

  total_count++;

  if (x < x_min) under_count++;
  else if (x >= x_max) over_count++;
  else {

    int i = (int) ((bin_count * (x - x_min)) / (x_max - x_min));

    switch (data_code) {
      case TRACK_MEAN : 
	data_value[i] += y; 
	break;
      case TRACK_MAX : 
	if (data_value[i] < y || data_count[i]==0) data_value[i] = y; 
	break;
      case TRACK_MIN : 
	if (data_value[i] > y || data_count[i]==0) data_value[i] = y; 
	break;
      }

    data_count[i]++;
  }
}



// Histogram::meanX()

double Histogram::meanX() const { 
  return x_sum / total_count; 
}



// Histogram::maxX()

double Histogram::maxX() const {
  return max_x_seen;
}



// Histogram::minX()

double Histogram::minX() const {
  return min_x_seen;
}



// Histogram::binXLowLimit()

double Histogram::binXLowLimit(int i) const {
  return x_min + (i * (x_max - x_min)) / bin_count;
}



// Histogram::binXHighLimit()

double Histogram::binXHighLimit(int i) const {
  return x_min + ((i+1) * (x_max - x_min)) / bin_count;
}



// Histogram::writeDataForSteppedPlot()

void Histogram::writeDataForSteppedPlot(JTextStream& str) const {

  double x_lo = x_min;

  for (int i=0; i<bin_count; i++) {

    double x_hi = x_min + ((i+1) * (x_max - x_min)) / bin_count;

    double v = data_value[i];
    if (data_code == TRACK_MEAN) if (data_count[i]) v /= data_count[i];

    str << x_lo << " " << data_count[i] << " " << v << "\n";
    str << x_hi << " " << data_count[i] << " " << v << "\n";

    x_lo = x_hi;
  }
}



// Histogram::writeFileForSteppedPlot()

void Histogram::writeFileForSteppedPlot(const JString& filepath) const {
  JFile f(filepath);
  f.open(IO_WriteOnly);
  JTextStream str(&f);
  writeDataForSteppedPlot(str);
  f.close();
}



// Histogram::writeDataForIpolPlot()

void Histogram::writeDataForIpolPlot(JTextStream& str) const {

  double x_lo = x_min;

  for (int i=0; i<bin_count; i++) {

    double x_hi = x_min + ((i+1) * (x_max - x_min)) / bin_count;

    double v = data_value[i];
    if (data_code == TRACK_MEAN) if (data_count[i]) v /= data_count[i];

    str << ((x_lo + x_hi)/2) << " " << data_count[i] << " " << v << "\n";
  
    x_lo = x_hi;
  }
}



// Histogram::writeFileForIpolPlot()

void Histogram::writeFileForIpolPlot(const JString& filepath) const {
  JFile f(filepath);
  f.open(IO_WriteOnly);
  JTextStream str(&f);
  writeDataForIpolPlot(str);
  f.close();
}

