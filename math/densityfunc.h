
// histdata.h

#ifndef _JUDE_MATH_DENSITYFUNC_H
#define _JUDE_MATH_DENSITYFUNC_H

#include "math/ublas.h"

namespace jude {
namespace math {



/*! A DensityFunc stores a tabulated density function which is essentially a histogram. 
Data points can be added one at a time by specifying the x-position and the y-value weight (or count)
which are summed in bins accross the x-range. */

class DensityFunc {

    // minimum x-value that can be stored, inclusive
  double x_min;
    // maximum x-value that can be stored, non-inclusive
  double x_max;
    // array storing the current weight or count in each bin
    // size is the number of bins - bins are of equal width of (x_max - x_min) / bin_count
  vector<double> bin_weight;  
    // the sum of any weights that were outside the bin range
  double under_weight_sum;
  double over_weight_sum;
    // the sum of all weights, including those out of range
  double total_weight;
    // the highest and lowest x values input
  double max_x_seen;
  double min_x_seen;

  void writeDataForPlot(JTextStream& str) const;
  void writeDataForSteppedPlot(JTextStream& str) const;

public:

  DensityFunc(double x_min, double x_max, int bin_count);
  ~DensityFunc();

  void clear();
  void addWeight(double xpos, double w);

  int binCount() const { return bin_weight.size(); } 
  double binXLowLimit(int i) const;
  double binXHighLimit(int i) const;
  double binXCentre(int i) const;

  double meanX() const;
  double maxX() const;
  double minX() const;
  
  double maxXSeen() const;
  double minXSeen() const;

  double upperTail(double frac) const;

  void writeFileForPlot(const JString& filepath) const;
  void writeFileForSteppedPlot(const JString& filepath) const;
};



} } // namespace jude::math

#endif // _JUDE_MATH_DENSITYFUNC_H

