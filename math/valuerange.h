
// valuerange.h

#ifndef _JUDE_MATH_VALUERANGE_H
#define _JUDE_MATH_VALUERANGE_H

#include <math.h>
#include <vector>

namespace jude {
namespace math {


/*! \brief Expresses the range of a value and discretizes it into a number of steps. */

class ValueRange {

  double min_inc;
  double max_inc;
  int step_count;

  std::vector<double> value_array;

public:

  ValueRange() { value_array.resize(0); }

    /*! Constructs a value range which is just a single value \a v. */
  ValueRange(double v) :
    min_inc(v),
    max_inc(v),
    step_count(0) { value_array.resize(0); }

    /*! Constructs a value range from \a mn to \a mx inclusive discretized into \a sc steps, i.e. \a sc intervals. */
  ValueRange(double mn, double mx, int sc) :
    min_inc(mn),
    max_inc(mx),
    step_count(sc) { value_array.resize(0); }

    /*! Constructs a value range from \a mn to \a mx inclusive discretized into \a sc steps, i.e. \a sc intervals. */
  ValueRange(const std::vector<double>& va) :
    min_inc(va[0]),
    max_inc(va[va.size()-1]),
    step_count(va.size()-1),
    value_array(va) { }

  double min() const { return min_inc; }
  double max() const { return max_inc; }
  int stepCount() const { return step_count; }

    /*! Returns the value at discrete point \a s, where \a s can be from zero to stepCount(). */
  double stepValue(int s) const {
    if (value_array.size() > 0) return value_array[s]; 
    return step_count ? (min_inc + (s * (max_inc - min_inc)) / step_count) : min_inc; 
  }
};



} } // namespace jude::math

#endif // _JUDE_MATH_VALUERANGE_H

