#ifndef _JUDE_BASE_TYPES_H
#define _JUDE_BASE_TYPES_H

#include <limits>

namespace jude {
namespace base {


/*!
\todo Build some test cases that verify the types are of the specified size.
*/



typedef char int8;          /*! Signed integer garaunteed to be 8 bits */ 
typedef short int16;        /*! Signed integer garaunteed to be 16 bits */
typedef int int32;          /*! Signed integer garaunteed to be exactly 32 bits */ 
typedef long long int64;    /*! Signed integer garaunteed to be exactly 64 bits */ 

typedef unsigned char uns8;          /*! Unsigned integer garaunteed to be 8 bits */ 
typedef unsigned short uns16;        /*! Unsigned integer garaunteed to be 16 bits */
typedef unsigned int uns32;          /*! Unsigned integer garaunteed to be 32 bits */
typedef unsigned long long uns64;    /*! Unsigned integer garaunteed to be 64 bits */

typedef float float32;            /*! Floating point garaunteed to be 32 bits */
typedef double double64;          /*! Floating point garaunteed to be 64 bits */
typedef long double doubleLONG;   /*! Floating point should be 128 bits, on 32-bit Linux this is only 96 bits */

// interferes with GL
//typedef unsigned int uint;      /*! Convienient synonym for \c unsigned \c int. Size dependent on platform and compiler. */


const uns32 UNS32_MIN = std::numeric_limits<uns32>::min();
const uns32 UNS32_MAX = std::numeric_limits<uns32>::max();

const uns64 UNS64_MIN = std::numeric_limits<uns64>::min();
const uns64 UNS64_MAX = std::numeric_limits<uns64>::max();

const int32 INT32_MIN = std::numeric_limits<int32>::min();
const int32 INT32_MAX = std::numeric_limits<int32>::max();
  
const int64 INT64_MIN = std::numeric_limits<int64>::min();
const int64 INT64_MAX = std::numeric_limits<int64>::max();

const float FLOAT_MIN = std::numeric_limits<float>::min();
const float FLOAT_MAX = std::numeric_limits<float>::max();

const double DOUBLE_MIN = std::numeric_limits<double>::min();
const double DOUBLE_MAX = std::numeric_limits<double>::max();

const float32 FLOAT32_MIN = std::numeric_limits<float32>::min();
const float32 FLOAT32_MAX = std::numeric_limits<float32>::max();

const double64 DOUBLE64_MIN = std::numeric_limits<double64>::min();
const double64 DOUBLE64_MAX = std::numeric_limits<double64>::max();

const doubleLONG DOUBLELONG_MIN = std::numeric_limits<doubleLONG>::min();
const doubleLONG DOUBLELONG_MAX = std::numeric_limits<doubleLONG>::max();



  /*! \brief Returns 1 if \a v is greater than or equal to one, returns -1 if not. */
template <typename T>
inline int sign(const T& t) { return (t >= 0) ? 1 : -1; }


  /*! \brief Swaps the contents of \a t1 and \a t2 using the assigment operator (operator=). T must also provide a default constructor. */
template <typename T>
static void swap(T& t1, T& t2) { T temp=t1; t1=t2; t2=temp; } 


  /*! \brief Returns a reference to \a t1 if \a t2 > \a t1 is true, otherwise returns a reference to \a t2. */
template <typename T>
static const T& min(const T& t1, const T& t2) { return (t2>t1) ? t1 : t2; } 


  /*! \brief Returns a reference to \a t1 if \a t1 < \a t2 is true, otherwise returns a reference to \a t2. */
template <typename T>
static const T& max(const T& t1, const T& t2) { return (t1>t2) ? t1 : t2; } 


  /*! \brief Macro which combines two integers \a a and \a b into one by shifting of \a b up by \a c bits and adding them. 
  \todo Probably this should be a an inline function not a macro. */
#define COMP(a,b,c) (a + (b<<c))



//typedef uns32 JRgb;   // 32 bit colour data

//#ifndef XOR
//#define XOR(a,b) ((a || b) && !(a && b))
//#endif

#ifdef WIN32

// once M_PI was not defined in the windows headers
#ifndef M_PI
#define M_PI 3.1415927
#endif

#endif


#ifdef MINGW

// HUGE is not defined in the windows headers
#define HUGE 1E30 // fix me

#endif

#ifdef VC6

// fix for variable scope defined in for loop
#define for if(0); else for

#endif


#define JUDE_DEFAULT_OUTPUT_PREC 6


} } // namespace jude::base

#endif // _JUDE_BASE_TYPES_H

