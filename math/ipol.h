
// interpolate.h

#ifndef _JUDE_MATH_INTERPOLATE_H
#define _JUDE_MATH_INTERPOLATE_H

#include "math/ublas.h"
#include "base/jerror.h"

namespace jude {
namespace math {


class Ipol {

public:

  template <typename T>
  static const T closestPoint(const vector<T>& v, double pos);

  template <typename T>
  static int closestIndex(const vector<T>& v, double value);

  template <typename T>
  static const T linearInterpolate(const vector<T>& v, int p, double frac);

  template <typename T>
  static const T linearInterpolate(const vector<T>& v, double pos);

  template <typename T>
  static const T linearInterpolate(double dest_x, const vector<T>& src_y, const vector<double>& src_x); 

  template <typename T>
  static const vector<T> linearInterpolate(const vector<double>& dest_x, const vector<T>& src_y, const vector<double>& src_x);
};




/*! \brief Returns the value from \a v located closest to the fractional index into the array \a pos. */

template <typename T>
const T Ipol::closestPoint(const vector<T>& v, double pos) {
  int i = (int) floor(pos + 0.5);
  if (i < 0) return v(0);
  if (((unsigned int) i) >= v.size()-1) return v(v.size()-1);
  return v(i);
}



/*! \brief Returns the value from \a v located closest to the fractional index into the array \a pos. */

template <typename T>
int Ipol::closestIndex(const vector<T>& v, double val) {

  int pos(-1);
  double min_d(DOUBLE_MAX);

  for (unsigned int i=0; i<v.size(); i++) {
    double d = fabs(v(i) - val);
    if (d < min_d) { min_d=d; pos=i; }
  }

  return pos;
}



/*! \brief Returns the linear interpolated value from \a v by the index \p and fractional component through the index \a frac. */

template <typename T>
const T Ipol::linearInterpolate(const vector<T>& v, int p, double frac) {
  return v(p) + frac * (v(p+1) - v(p));
}



/*! \brief Returns the linear interpolated value from \a v by the fractional index into the array \a pos. */

template <typename T>
const T Ipol::linearInterpolate(const vector<T>& v, double pos) {
  if (pos < 0) return v(0);
  if (pos >= v.size()-1) return v(v.size()-1);
  int i = (int) floor(pos);
  double frac = pos - i;
  return v(i) + frac * (v(i+1)-v(i));
}



/*! Returns a linear interpolated y-value from \a v where the x-position of each element of this vector is given by \a src_x 
    and the position of the value to be returned is \a src_y. */

template <typename T>
const T Ipol::linearInterpolate(double dest_x, const vector<T>& src_y, const vector<double>& src_x) { 

  unsigned int n = src_y.size();

    // that vectors are equal length should be ensured
  if (n != src_x.size()) { jlog::es << "Internal error: " << __FUNCTION__ << " vectors not equal length\n"; internalError(); }

    // if before the start of the function then just return the start value
  if (dest_x <= src_x(0)) return src_y(0);

    // if after the end of the function then just return the end value
  if (dest_x >= src_x(n-1)) return src_y(n-1);

    // find the index of the interval which contains this value
  unsigned int i;
  for (i=1;i<n && src_x(i) < dest_x; i++);

    // the fraction into this interval
  double frac = (dest_x - src_x(i-1)) / (src_x(i) - src_x(i-1));

    // return linear interpolated value
  return src_y(i-1) + frac * (src_y(i) - src_y(i-1));
}



/*! Returns a linear interpolated version of \a src_value where \a dest_pos gives the positions
    of the interpolated elements and  \a src_pos gives the position of the source elements. */

template <typename T>
const vector<T> Ipol::linearInterpolate(const vector<double>& dest_x, const vector<T>& src_y, const vector<double>& src_x) { 

  unsigned int n = src_y.size();

    // that vectors are equal length should be ensured
  if (n != src_x.size()) { jlog::es << "Internal error: " << __FUNCTION__ << " vectors not equal length\n"; internalError(); }

    // handle source data sizes which can't be interpolated
  if (n==0) { return scalar_vector<T>(dest_x.size(), 0); }
  if (n==1) { return scalar_vector<T>(dest_x.size(), src_y(0)); }

    // make an output vector of the right size
  vector<T> dest_y(dest_x.size());

  double curr_pos0 = src_x(0);
  double curr_pos1 = src_x(1);

  unsigned int curr_index = 0;

    // loop through entries
  for (unsigned int i=0; i<dest_y.size(); i++) {
   
      // position of this value
    double pos = dest_x(i);

      // if before the start of this interval then just set to start value
    if (pos < curr_pos0) dest_y(i) = src_y(curr_index);
    else {

        // if after the end of this interval then move source interval forward
      while (pos >= curr_pos1 && curr_index < n-2)  {
	curr_pos0 = curr_pos1;
	curr_index++;
	curr_pos1 = src_x(curr_index+1);
      }

        // if still past end of interval then set value to end of interval
      if (pos >= curr_pos1) dest_y(i) = src_y(curr_index+1);
      else {
	  // fraction between the two points
	double frac = (pos - curr_pos0) / (curr_pos1 - curr_pos0); 
          // interpolated value
	dest_y(i) = linearInterpolate(src_y, curr_index, frac);
      }
    }
  }

  return dest_y;
}



/*! Returns the area under the interval \a lo_x to \a hi_x of this vector where each element of this vector is the function value at the x-locations \a src_x
  basically returns the integral of the function between \a lo_x and \a hi_x assuming the function is linear between the defined points. */

/*
template <typename T>
double areaUnderInterval(double lo_x, double hi_x, const vector<T>& src_y, const vector<double>& src_x) const { 

  int n = src_y.size();

    // that vectors are equal length should be ensured
  if (n != src_x.size()) { jlog::es << "Internal error: " << __FUNCTION__ << " vectors not equal length\n"; internalError(); }

  double area = 0;

  //printf("lo %f hi %f\n",lo_x,hi_x);

    // trim any part of the interval that lies before the start of the defined function
    // assuming the function is flat at src_y(0) before it starts
  if (src_x(0) >= lo_x) { 
    if (hi_x >= src_x(0)) area += (src_x(0) - lo_x) * src_y(0);
    else area += (hi_x - lo_x) * src_y(0);
    lo_x = src_x(0);
  }

    // similarly trim any part of the interval that after the end of the defined function
    // assuming the function is flat at a[size()-1] after it ends
  if (src_x(n-1) <= hi_x) { 
    if (lo_x <= src_x(n-1)) area += (hi_x - src_x(n-1)) * src_y(n-1);
    else area += (hi_x - lo_x) * src_y(n-1);
    hi_x = src_x(n-1);
  }

  //printf("lo %f hi %f\n",lo_x,hi_x);
 
    // if the interval is of zero width then quit
  if (hi_x <= lo_x) return area;

    // find the index at which the interval starts
  int curr_index;
  for (curr_index=0; curr_index < n-1 && src_x(curr_index+1) < lo_x; curr_index++);

    // start of interval
  double curr_x = lo_x;

    // get the function value at the interval start
  double curr_y = linearInterpolate(lo_x, src_y, src_x);

    // loop until this element contains the interval end
    // only this test is required since hi_x has been forced to <= to src_x[size()-1]
    // so that will stop curr_index overrruning

  while (src_x(curr_index+1) < hi_x) {

    double next_y = src_y(curr_index+1);
    double next_x = src_x(curr_index+1);
      // area of this segment
    double da = (next_x - curr_x) * (curr_y + next_y) / 2;
  
    //printf("%d %f %f %f %f %f\n",curr_index,curr_x,next_x,curr_y,next_y,da);

      // sum area
    area += da;

      // move on to next element
    curr_index++; 
    curr_x = next_x;
    curr_y = next_y;
  }

    // all that should be left is the area up to the end point
  double next_y = linearInterpolate(hi_x, src_y, src_x);
  double next_x = hi_x;
    // area of this segment
  double da = (next_x - curr_x) * (curr_y + next_y) / 2;

  //printf("final %f %f %f %f %f\n",curr_x,next_x,curr_y,next_y,da);

  return area + da;
}
*/


/*! Returns a linear interpolated version of \a src_y where \a dest_lo_x and \a dest_hi_x gives the positions and widths
  of the x-limits of box function target elements and \a src_x gives the centre position of the source elements which are treated as linearly interpolated. */
/*
template <typename T>
const vector<T> integralInterpolateBoxFunc(const vector<double>& dest_lo_x, const vector<double>& dest_hi_x, const vector<T>& src_y, const vector<double>& src_x) { 

     // that vectors are equal length should be ensured
  if (dest_lo_x.size() != dest_hi_x.size()) { jlog::es << "Internal error: " << __FUNCTION__ << " dest vectors not equal length\n"; internalError(); }
  if (src_x.size() != src_y.size()) { jlog::es << "Internal error: " << __FUNCTION__ << " src vectors not equal length\n"; internalError(); }

    // make an output vector of the right size
  vector<T> dest_y(dest_lo_x.size());
  
    // generate integral for each band in the output vector
  for (int i=0; i<dest_y.size(); i++) {
   
    double area = src_y.areaUnderInterval(dest_lo_x(i), dest_hi_x(i), src_x);
    double width = dest_hi_x(i) - dest_lo_x(i);

    dest_y(i) = area / width;
  }
}
*/


} } // namespave jude::math

#endif // _JUDE_MATH_INTERPOLATE_H

