
// ublas.h

#ifndef _JUDE_MATH_UBLAS_H
#define _JUDE_MATH_UBLAS_H

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_expression.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include "base/jtextstream.h"
#include "base/jdatastream.h"
#include "base/jlog.h"
#include "file/fileutils.h"

//#include "matrix.h"
//#include "vector.h"

using namespace boost::numeric::ublas;
using namespace jude::base;
//using namespace jude::file;

namespace jude {
namespace math {

using namespace jude::file;

//typedef double mat_type;

//typedef vector<double> vec;
//typedef const vector<double> cvec;

//typedef matrix<mat_type> mat;
//typedef const matrix<mat_type> cmat;



/*! Copies a std::vector to a ublas::vector. */
 
template <typename T>
void copy(vector<T>& lhs, const std::vector<T>& rhs) {
  lhs.resize(rhs.size());
  for (unsigned int i=0; i<lhs.size(); i++) lhs(i) = rhs[i]; // fixme - do this better
}



/*! Copies a ublas::vector to a std::vector. */
 
template <typename T>
void copy(std::vector<T>& lhs, const vector<T>& rhs) {
  lhs.resize(rhs.size());
  for (unsigned int i=0; i<lhs.size(); i++) lhs[i] = rhs(i); // fixme - do this better
}



/*! Copies an array of values to ublas::vector. */
 
template <typename T>
void copy(vector<T>& lhs, int array_size, const T* array_ptr) {
  lhs.resize(array_size);
  for (unsigned int i=0; i<lhs.size(); i++) lhs(i) = *(array_ptr++); // fixme - do this better
}



/*! Copies an array of values to ublas::vector. */
 
template <typename T>
void copy(vector<T>& lhs, int array_size, const T* array_ptr, int stride) {
  lhs.resize(array_size);
  for (unsigned int i=0; i<lhs.size(); i++, array_ptr+=stride) lhs(i) = *array_ptr;
}



/*! Returns the dot product (inner product) of \a v1 and \a v2. */

template <typename T>
T dotProduct(const vector<T>& v1, const vector<T>& v2) {
  return inner_prod(v1, v2);
}



inline bool readFromTextStream(float& t, jude::base::JTextStream& str) {
  JString line; 
  if (!FileUtils::readTextLine(line, str)) return false; 
  t = (float) line.toDouble();
  return true;
} 



inline bool readFromTextStream(double& t, JTextStream& str) {
  JString line; 
  if (!FileUtils::readTextLine(line, str)) return false; 
  t = line.toDouble();
  return true;
} 



inline bool readFromTextStream(int& t, JTextStream& str) { 
  JString line; 
  if (!FileUtils::readTextLine(line, str)) return false; 
  t = line.toInt();
  return true;
} 



/*! Reads a ublas::vector from a text stream using the T based function readFromTextStream(T&, str).
  The function readFromTextStream(T&, str) for type T must be defined and reads the information for T from one or more lines in a block. 
*/

template <typename T>
bool readFromTextStream(vector<T>& v, JTextStream& str) {
    // read into a standard vector first because we din't know how many there are
  std::vector<T> temp_v;
  T t;
  while (readFromTextStream(t, str)) temp_v.push_back(t);
  copy(v, temp_v);
  return true;
}



/*! Writes a ublas::vector to a text stream using the T based function writeToTextStream(T&, str).
  The function writeToTextStream(T&, str, sub_sep) for type T must be defined and writes the information for T in one line. 
*/

//template <typename T>
//bool writeToTextStream(const vector<T>& v, JTextStream& str) {
//  for (int i=0; i<v.size(); i++) if (!writeToTextStream(v(i), str)) return false;
//  return true;
//}




// so vectors of doubles can be written to text streams

static inline void writeToTextStream(double d, JTextStream& str, const JString&) { str << d; }



/*! Writes a ublas::vector to a text stream using the T based function writeToTextStream(T&, str).
  The function writeToTextStream(T&, str, sub_sep) for type T must be defined and writes the information for T in one line. 
*/

template <typename T>
bool writeToTextStream(const vector<T>& v, JTextStream& str, const JString& band_sep, const JString& sub_sep) {
  for (unsigned int i=0; i<v.size(); i++) {
    writeToTextStream(v(i), str, sub_sep);
    if (i != v.size()-1) str << band_sep;
  }
  return true;
}



/*! Read a ublas::vector from a JDataStream. */

template <typename T>
bool writeToDataStream(const vector<T>& v, JDataStream& str) {
  str << (int32) v.size();
  for (unsigned int i=0; i<v.size(); i++) str << v(i);
  return true;
}



/*! Write a ublas::vector to a JDataStream. */

template <typename T>
bool readFromDataStream(vector<T>& v, JDataStream& str, uns32 saved_version) {
  int32 n;
  str >> n;
  v.resize(n);
  for (unsigned int i=0; i<v.size(); i++) str >> v(i);
  return true;
}



template <typename T>
JTextStream& operator<<(JTextStream& str, const vector<T>& v) {
  str << "[ ";
  for (unsigned int i=0; i<v.size(); i++) { str << v(i); if (i<v.size()-1) str << ", "; }
  str << " ]";
  return str;
}



template <typename T>
JTextStream& operator<<(JTextStream& str, const matrix<T>& m) {
  for (unsigned int i=0; i<m.size1(); i++) {
    str << "[ ";
    for (unsigned int j=0; j<m.size2(); j++) { 
      if (m(i,j) >= 0) str << " ";
      str << m(i,j);
      if (j<m.size2()-1) str << ","; 
    }
    str << " ]\n";
  }
  return str;
}
 

// depreceated
//template <typename T>
//JDataStream& operator<<(JDataStream& str, const vector<T>& v) {
//  str << (int) v.size();
//  for (unsigned int i=0; i<v.size(); i++) str << v(i);
//  return str;
//}
 

// depreceated
//template <typename T>
//JDataStream& operator>>(JDataStream& str, vector<T>& v) {
//  int n;
//  str >> n;
//  v.resize(n);
//  for (unsigned int i=0; i<v.size(); i++) str >> v(i);
//  return str;
//}
 


template <typename T>
T norm_2_squared(const vector<T>& v) {
  T t(0);
  for (unsigned int i=0; i<v.size(); i++) t += v(i)*v(i);
  return t;
}




template <typename T>
T norm_max(const vector<T>& v) {

  assert(v.size()!=0);

  T max_value = v(0);

  for (unsigned int i=1; i<v.size(); i++) {
    if (v(i) > max_value) max_value = v(i);
  }

  return max_value;
}



template <typename T>
T norm_min(const vector<T>& v) {

  assert(v.size()!=0);

  T min_value = v(0);

  for (unsigned int i=1; i<v.size(); i++) {
    if (v(i) < min_value) min_value = v(i);
  }

  return min_value;
}



template <typename T>
T norm_max(const vector<T>& v, int& index) {

  assert(v.size()!=0);

  T max_value = v(0);
  index = 0;

  for (unsigned int i=1; i<v.size(); i++) {
    if (v(i) > max_value) { max_value = v(i); index=i; }
  }

  return max_value;
}



template <typename T>
T norm_fabs_max(const vector<T>& v, int& index) {

  assert(v.size()!=0);

  T max_value = v(0);
  index = 0;

  for (unsigned int i=1; i<v.size(); i++) {
    if (fabs(v(i)) > max_value) { max_value = v(i); index=i; }
  }

  return max_value;
}



template <typename T>
T norm_max(const matrix<T>& m, int& index1, int& index2) {

  assert(m.size1()!=0 && m.size2()!=0);

  T max_value = m(0,0); 
  index1 = 0; 
  index2 = 0;

  for (unsigned int i=0; i<m.size1(); i++) {
    for (unsigned int j=0; j<m.size2(); j++) {
      if (m(i,j) > max_value) { max_value = m(i,j); index1=i; index2=j; }
    }
  }

  return max_value;
}



template <typename T>
T norm_fabs_max(const matrix<T>& m, int& index1, int& index2) {

  assert(m.size1()!=0 && m.size2()!=0);

  T max_value = m(0,0); 
  index1 = 0; 
  index2 = 0;

  for (unsigned int i=0; i<m.size1(); i++) {
    for (unsigned int j=0; j<m.size2(); j++) {
      if (fabs(m(i,j)) > max_value) { max_value = m(i,j); index1=i; index2=j; }
    }
  }

  return max_value;
}



/*! \brief Convienience function that returns the entry with the maximum fabs() evalauted magnitude from the matrix \a m without the indices. */

template <typename T>
T norm_fabs_max(const vector<T>& v) {
  int index;
  return norm_fabs_max(v, index);
}



/*! \brief Convienience function that returns the entry with the maximum fabs() evalauted magnitude from the matrix \a m without the indices. */

template <typename T>
T norm_fabs_max(const matrix<T>& m) {
  int index1;
  int index2;
  return norm_fabs_max(m, index1, index2);
}



template <typename T>
T norm_min(const matrix<T>& m, int& index1, int& index2) {

  assert(m.size1()!=0 && m.size2()!=0);

  T min_value = m(0,0);

  for (unsigned int i=0; i<m.size1(); i++) {
    for (unsigned int j=0; j<m.size2(); j++) {
      if (m(i,j) < min_value) { min_value = m(i,j); index1=i; index2=j; }
    }
  }

  return min_value;
}



/*! \brief Returns the maximum value from the matrix \a m. */

template <typename T>
T norm_max(const matrix<T>& m) {
  int index1;
  int index2;
  return norm_max(m, index1, index2);
}



/*! \brief Returns the minimum value from the matrix \a m. */

template <typename T>
T norm_min(const matrix<T>& m) {
  int index1;
  int index2;
  return norm_min(m, index1, index2);
}



template <typename T>
T norm_sum(const matrix<T>& m) {

  T sum;

  for (unsigned int i=0; i<m.size1(); i++) {
    for (unsigned int j=0; j<m.size2(); j++) {
      sum += m(i,j);
    }
  }

  return sum;
}



template <typename T>
void writeMatrix(const matrix<T>& m, JDataStream& str) {

  str << (int) m.size1();
  str << (int) m.size2();

  for (unsigned i=0; i<m.size1(); i++) {
    for (unsigned j=0; j<m.size2(); j++) {
      str << m(i,j);
    }
  }
}



template <typename T>
void readMatrix(matrix<T>& m, JDataStream& str) {

  int s1,s2;
  str >> s1;
  str >> s2;
  m.resize(s1,s2);

  for (unsigned i=0; i<m.size1(); i++) {
    for (unsigned j=0; j<m.size2(); j++) {
      str >> m(i,j);
    }
  }
}



//inline void convertVector(vec& targ, const Vector& src) {
//  targ.resize(src.size());
//  for (unsigned i=0; i<targ.size(); i++) targ(i) = src.get(i);
//}



//inline void convertVector(Vector& targ, const vec& src) {
//  targ.makeZeros(src.size());
//  for (unsigned i=0; i<src.size(); i++) targ.put(i,src(i));
//}



//template <typename T>
//inline void convertMatrix(Matrix& targ, const matrix<T>& src) {
//  targ.setSize(src.size1(), src.size2());
//  for (int i=0;i<targ.rows();i++) {
//    for (int j=0;j<targ.columns();j++) {
//      targ.put(i, j, src(i,j));
//    }
//  }
//}



//template <typename T>
//inline void convertMatrix(matrix<T>& targ, const Matrix& src) {
//  targ.resize(src.rows(), src.columns());
//  for (int i=0;i<src.rows();i++) {
//    for (int j=0;j<src.columns();j++) {
//      targ(i, j) = src.get(i,j);
//    }
//  }
//}




/*! \brief Write the vector \a v to a data file \a filepath.

\todo Need to implement.
*/

template <typename T>
bool writeToTextFile(const vector<T>& v, const JString& filepath) {
  //Vector targ;
  //convertVector(targ, v);
  //return targ.writeToTextFile(filepath);
  return false;
}



/*! \brief Read the vector \a targ to a data file \a filepath.

\todo Need to implement.
*/

template <typename T>
bool readFromTextFile(vector<T>& targ, const JString& filepath) {
  JFile f(filepath);
  if (!f.open(JIO_ReadOnly)) return false;
  JTextStream str(&f);
  bool ret_val = readFromTextStream(targ, str);
  f.close();
  return ret_val;
}


/*! \brief Write the matrix \a m to a data file \a filepath.

\todo Need to implement.
*/

template <typename T>
bool writeToDataFile(const matrix<T>& m, const JString& filepath) {
  //Matrix targ;
  //convertMatrix(targ, m);
  //return targ.writeToDataFile(filepath);
  return false;
}


/*! \brief Read the matrix \a targ from a data file \a filepath.

\todo Need to implement.
*/

template <typename T>
bool readFromDataFile(matrix<T>& targ, const JString& filepath) {
  //Matrix m;
  //if (!m.readFromDataFile(filepath)) return false;
  //convertMatrix(targ, m);
  //return true;
  return false;
}



// Finds the inverse of a matrix directly by Gauss-Jordan method using partial pivoting on the 
// downward elimination - i.e. small pivots are re-ordered by row swaps.
// Returns the inverse of (m) in (inv).
// Returns (true) if successful, (false) if the inverse could not be found because the matrix was not 
// square or a pivot was zero, in which case (inv) is uneffected.
// Note that only exactly zero pivots cause the failure of the inverse.
// The existence of very small pivots may result in an innacurate inverse but no error is flagged.
// the algorithm can be found in Strang, G. 1988 "Linear Algebra and its Applications. 3rd Ed." Harcourt Brace Jovanovich

template <typename T>
bool invert(const matrix<T>& m, matrix<T>& inv) {

  int rn = m.size1();
  int cn = m.size2();

    // quit if not square
  if (rn != cn) { jlog::es.printf("invert() matrix (m) %d x %d not square\n", rn, cn); return false; }

  int twice_cn = cn << 1;
 
    // create temporary workspace with twice the number of columns
  matrix<T> temp(rn, twice_cn);
    // put the source matrix in the left half
  project(temp, range(0,rn), range(0,cn)) = m;
    // and the identity matrix in the right half
    // by the time the left matrix has been converted to the identity the right will be the inverse
  project(temp, range(0,rn), range(cn,twice_cn)) = identity_matrix<T>(rn);

    // do forward elimination pass - go through each row
  for (int i=0; i<rn-1; i++) {

      // the column of the pivot to use for this row
    int pivot_column = i;
      // start by getting the diagonal value for the pivot
    int pivot_row = i;
    double pivot = temp(i,i);
      // now see if there is a better pivot in the rows below
    for (int pr=i+1; pr<rn; pr++) {
      double test_pivot = temp(pr, i);
      if (fabs(test_pivot) > fabs(pivot)) { pivot = test_pivot; pivot_row = pr; } 
    }
      // swap rows if necessary
    if (pivot_row != i) swap(row(temp, i), row(temp, pivot_row));
      // quit if singular - only check for zero here but if pivot is very small inverse might be bogus
    if (pivot==0) return false;

      // loop through remaining rows
    for (int ri=i+1; ri<rn; ri++) {
        // multiplier to eliminate the pivot column value in this row
      double m = temp(ri, pivot_column) / pivot;
        // do the elimination
      temp(ri, pivot_column) = 0;
        // adjust values to the right
      for (int j=pivot_column+1; j<twice_cn; j++) temp(ri,j) -= m*temp(i,j);
    }
  }
 
    // do upward elimination pass
  for (int i=rn-1; i>0; i--) {

    int pivot_column = i;
      // note that pivots have already been checked for != zero
    double pivot = temp(i,i);
      // quit if singular - only check for zero here but if pivot is very small inverse might be bogus
    //if (pivot==0) return false;
      // loop through remaining rows upwards
    for (int ri=i-1; ri>=0; ri--) {
        // multiplier to eliminate the pivot column value in this row
      double m = temp(ri,pivot_column) / pivot;
        // do the elimination
      temp(ri,pivot_column) = 0;
      for (int j=pivot_column+1; j<twice_cn; j++) temp(ri,j) -= m*temp(i,j);
    } 
  }
 
    // divide through by pivots
  for (int i=0; i<rn; i++) {
    double m_div = 1.0 / temp(i,i);
    for (int j=rn; j<twice_cn; j++) temp(i,j) *= m_div;    
  }
 
    // extract the inverse
  inv = project(temp, range(0,rn), range(cn,twice_cn));

  return true;
}



// Cholesky decomoposition of G
// returns the lower triangular matrix L such that G = L * trans(L)
// G is assumed to be symmetrical and square
// from Wolfe, M.A. 1978 "Numerical Methods for Unconstrained Optimization" Van Nostrand Reinhold. pg. 100
 
template <typename T>
const matrix<T> cholesky(const matrix<T>& G) {

  int n = G.size1();
  
  matrix<T> L;
  L = zero_matrix<T>(n,n);

    // for each column
  for (int i=0; i<n; i++) {

    T t = G(i,i);
    for (int m=0; m<i; m++) t -= L(i,m) * L(i,m);
    L(i,i) = sqrt(t);
    
    for (int j=i+1; j<n; j++) {
      double t = G(j,i);
      for (int m=0; m<i; m++) t -= L(j,m) * L(i,m);
      L(j,i) = t / L(i,i);
    }
  }

  return L;
}



// this one works ok

/*

template <typename T>
bool invert2(matrix<T>& input, matrix<T>& inverse) {

  Matrix m(input.size1(), input.size2());
  Matrix m_inv(input.size1(), input.size2());

  for (int i=0;i<m.rows();i++) {
    for (int j=0;j<m.columns();j++) {
      m.put(i, j, input(i,j));
    }
  }

  if (!m_inv.inverse(m)) return false;

  inverse.resize(input.size1(), input.size2());

  for (int i=0;i<m.rows();i++) {
    for (int j=0;j<m.columns();j++) {
      inverse(i,j) = m_inv.get(i,j);
    }
  }

  return true;
}

*/


} } // namespace jude::math

#endif // _JUDE_MATH_UBLAS_H

