
// ranlecuyer2.cc

#include "ranlecuyer.h"

using namespace jude::math;


static const int32 m1 = 2147483563;
static const int32 a1 = 40014;
static const int32 q1 = 53668;
static const int32 r1 = 12211;

static const int32 m2 = 2147483399;
static const int32 a2 = 40692;
static const int32 q2 = 52774;
static const int32 r2 = 3791;

static const double m1_recip = 1.0 / m1;

static const double upper_lim = 1.0 - (1.0 / INT32_MAX);



/*! \brief Seed the generator with value \a i. 

The sequence of values produced by a RanLEcuyer object be always be the same for a given seed \a i. 

In a threaded environment using each thread should have its own RanLEcuyer object to ensure replicability of random sequences.
*/

void RanLEcuyer::initSeed(int32 i) {

  if (i>0) i = -i;
  while (i >= m1) i -= m1; 
  if (i==0) i = 1;

  mutex.lock();

  s1 = i;
  s2 = 123456789;

  mutex.unlock();
}



/*! \brief return a random \c double value in the range zero (inclusive) to one (not inclusive).

The random number generation algorithm used here is described in L'Ecuyer P. (1988) Communications of the ACM 31(6), 742-774.
*/

double RanLEcuyer::generateDouble() {

  int32 k;

  mutex.lock();

    // first MLCG  
  k = s1 / q1;
  s1 = a1 * (s1 - k * q1) - k * r1;
  if (s1 < 0) s1 += m1;

    // second MLCG
  k = s2 / q2;
  s2 = a2 * (s2 - k * q2) - k * r2;
  if (s2 < 0) s2 += m2;

  double Z = s1 - s2;

  mutex.unlock();

  if (Z < 1) Z += m1 - 1; 

  Z *= m1_recip;
  if (Z > upper_lim) Z = upper_lim;

  return Z;
}



/*! \fn RanLEcuyer::RanLEcuyer() 
Default constructor.
*/


