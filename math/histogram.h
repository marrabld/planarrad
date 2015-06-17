
// histogram.h
// (c) John Hedley 2008

#ifndef _JUDE_HISTOGRAM_H
#define _JUDE_HISTOGRAM_H

#include "jtextstream.h"


class Histogram {

  double x_min;  // inclusive
  double x_max;  // non-inclusive
  int bin_count;

  int data_code;

  int* data_count;
  double* data_value;

  int under_count;
  int over_count;

  double x_sum;
  double max_x_seen;
  double min_x_seen;
  int total_count;

public:

  enum { TRACK_NOP, TRACK_MEAN, TRACK_MAX, TRACK_MIN };

  Histogram(double x_min, double x_max, int bin_count, int data_code=TRACK_MEAN);
  Histogram(int x_data_count, double* x_data, int bin_count, double ix_min=0, double ix_max=0);
  ~Histogram();

  void clear();
  void addCount(double x);
  void addCountAndData(double x, double y);

  double binXLowLimit(int i) const;
  double binXHighLimit(int i) const;

  double meanX() const;
  double maxX() const;
  double minX() const;

  void writeDataForSteppedPlot(JTextStream& str) const;
  void writeDataForIpolPlot(JTextStream& str) const;

  void writeFileForSteppedPlot(const JString& filepath) const;
  void writeFileForIpolPlot(const JString& filepath) const;
};



#endif // _JUDE_HISTOGRAM_H

