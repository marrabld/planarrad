
// rand.cc

#include <stdlib.h>
#include <math.h>

#include "base/jtypes.h"
#include "rand.h"
#include "ranlecuyer.h"

using namespace jude::base;

namespace jude {
namespace math {



// random number generator

static RanLEcuyer ranlecuyer;


RanLEcuyer& standardRanGen() { return ranlecuyer; }



/*! \brief Returns a random \c true or \c false value with an alleged probability of 0.5 either way. 

This uses the C standard rand() function.

\todo Build test cases to test the distribution of these values falls within expected bounds.
*/

bool rand_bool_cstd() {
  return rand() & 1;
}



/*! \brief Returns a random integer witha value from zero to \a max_plus_one-1 with an alleged equal probability for any value. 

This uses the RanLEcuyer class, which is intended to be an improvement over the C standard rand().

\todo Build test cases to test the distribution of these values falls within expected bounds.
*/

int rand_int(int max_plus_one) {
  return (int) floor(ranlecuyer.generateDouble() * max_plus_one);
}



/*! \brief Re-seeds both the C standard rand() function and RanLEcuyer with the value \a i.

Seeding with a specific value \a i is garaunteed to produce identical sequence of ranbdom values from any Jude random function. Note that the replicability of a sequence can never be gauranteed in a threaded environment. This can be achieved by each thread containing its own RanLEcuyer object each of which are seeded by different values.

\todo Build test cases to check consistence of random sequences for specific \a i.
*/

void random_seed(long i) {
  ranlecuyer.initSeed(i);
  srand(i);
}



/*! \brief Returns a value from uniform distribution between \a min (inclusive) and \a max (exclusive).

This uses the RanLEcuyer class, which is intended to be an improvement over the C standard rand(). Note however the returned values will inevitably be disretized to a certain resolution since it is internally an integer based generator.

\todo Build test cases to test the distribution of these values falls within expected bounds.
*/

double uniform_dbn(double min, double exc_max) {
  return min + ranlecuyer.generateDouble() * (exc_max-min);
}



double uniform_dbn_cstd(double min, double exc_max) {
  return min + (((double)rand()) / RAND_MAX) * (exc_max-min);
}



/*! \brief Returns a value from an approximate normal distribution centred on \a mean with standard deviation \a sd.

This functions works by taking the average of 12 values from the uniform_dbn(0,1) function which uses the RanLEcuyer generator. A Normal distribution is therefore approximated as by the Central Limit Theorum.

\todo Build test cases to test the distribution of these values falls within expected bounds.
*/


double normal_dbn(double mean, double sd) {
  double v = 0;
  int i; 
    // generate a normally distributed number of mean=0 sd=1
  ////for (i=0;i<3;i++) v += (6*((double)rand()) / RAND_MAX) - 3;
  //for (i=0;i<3;i++) v += uniform_dbn(-1,1);
  ////v = v / 3;
 
  for (i=0;i<12;i++) v += uniform_dbn(0.0, 1.0);
  v = v - 6;

   // centre on mean and change standard deviation
  v = mean + (v*sd);
 
  return v;
}



double normal_dbn_m48(double mean, double sd) {
  double v = 0;
  int i; 
    // generate a normally distributed number of mean=0 sd=1
  ////for (i=0;i<3;i++) v += (6*((double)rand()) / RAND_MAX) - 3;
  //for (i=0;i<3;i++) v += uniform_dbn(-1,1);
  ////v = v / 3;
 
  for (i=0;i<48;i++) v += uniform_dbn(0.0, 1.0);
  v = v - 24;

   // centre on mean and change standard deviation
  v = mean + ((v*sd)/2);
 
  return v;
}



/*! \brief Returns a value from an approximate normal distribution centred on \a mean with standard deviation \a sd.

This function is intended to be quick and dirty. It works by taking the average of 3 values generated from the C standard rand() function.

\todo Build test cases to test the distribution of these values falls within expected bounds.
*/

double normal_dbn_cstd3(double mean, double sd) {

  double v = 0;

    // generate a normally distributed number of mean=0 sd=1
  for (int i=0; i<3; i++) v += (2*((double)rand()) / RAND_MAX) - 1;
 
   // centre on mean and change standard deviation
  v = mean + (v*sd);
 
  return v;
}



// returns the y-value of the normal pdf for this x-value

/*! \brief Returns the y-value for the x-value \a x for the normal probability distribution function defined by \a mean and \a sd.

\todo Build test cases to check this.
*/

double normal_value(double mean, double sd, double x) {
  double v = x - mean;
  return exp(-(v*v)/(2*sd*sd)) / (sd * sqrt(2*M_PI));
} 

} }

