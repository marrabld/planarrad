
// jstring.h

#include <stdio.h>
#include <math.h>
#include "jstring.h"

using namespace jude::base;



/*!
\brief A zero length string.

This is a singly instantiated const zero length string. It is useful when a function should return a const reference to a JString, but has no string. It can return this.
*/

const JString JString::null;



/*!
\brief Constructor with no parameters which constructs an empty string.
*/

JString::JString() { 
  len = 0; 
  array = new JChar[1]; 
  array[0] = 0;
  latin1_text = 0;
  update_buffers = true;
}



/*!
\brief Constructs a string as a copy of a C-style null terminated character string.
*/

JString::JString(const char* text) {
  if (!text) len = 0;
  else {
    const char *p = text;
    while (*p) p++;
    len = p - text;
  }
  array = new JChar[len+1];
  for (int i=0;i<len;i++,text++) array[i] = *text;
  array[len] = 0;
  latin1_text = 0;
  update_buffers = true;
}



/*!
\brief Constructs a JString as a copy of an array of char characters starting at \a u of a specified length \a n.
*/

JString::JString(const char* text, unsigned int n) {
  if (!text) n = 0;
  len = n;
  array = new JChar[len+1];
  for (int i=0;i<len;i++,text++) array[i] = *text;
  array[len] = 0;
  latin1_text = 0;
  update_buffers = true;
}



/*!
\brief Constructs a string as a copy of a null terminated unicode (i.e. 16 bit) character string.
*/

JString::JString(const JChar* u) {
  if (!u) len = 0;
  else {
    const JChar *p = u;
    while (*p) p++;
    len = p - u;
  }
  array = new JChar[len+1];
  for (int i=0;i<len;i++,u++) array[i] = *u;
  array[len] = 0;
  latin1_text = 0;
  update_buffers = true;
}



/*!
\brief Constructs a string as a copy of an array of unicode (i.e. 16 bit) characters starting at \a u of a specified length \a n.
*/

JString::JString(const JChar* u, unsigned int n) {
  if (!u) n = 0;
  len = n;
  array = new JChar[len+1];
  for (int i=0;i<len;i++,u++) array[i] = *u;
  array[len] = 0;
  latin1_text = 0;
  update_buffers = true;
}



/*!
\brief Constructs a string as a copy of another JString \a s.
*/

JString::JString(const JString& s) {
  len = s.len;
  array = new JChar[len+1];
  for (int i=0;i<len;i++) array[i] = s.array[i];
  array[len] = 0;
  latin1_text = 0;
  update_buffers = true;
}



/*!
\brief Constructs a string as read from a JDataStream \a str.
*/

JString::JString(JDataStream& str) :
  array(0),
  latin1_text(0) {

  str >> *this;
}



/*!
\brief Destructor.
*/

JString::~JString() { 
  delete[] array;
  delete[] latin1_text;
}



// JString::allocate() [private]

void JString::allocate(int n) {
  delete[] array;
  array = new JChar[n+1];
  len = n;
  array[len] = 0;
}



/*!
\brief Returns false if the string is of zero length.

Equivalent to (s.length() == 0).
*/

bool JString::isEmpty() const { 
  return len==0; 
}



/*!
\brief Returns the number of occurrences of character \a c in the string.
*/

int JString::contains(JChar c) const {
  int count = 0;
  for (int i=0;i<len;i++) if (array[i]==c) count++;
  return count;
}



/*!
\brief Returns a standard C-style 8 bit character string version of the JString contents with null termination.

This is useful for interfacting with old style C functions such as printf(), E.g.
<pre>
JString s("the string");
printf("This is %s\n",s.latin1());
</pre>
*/

const char* JString::latin1() const {
  if (!update_buffers) return latin1_text;
  delete[] latin1_text;
  latin1_text = new char[len+1];
  latin1_text[len] = 0;
  for (int i=0;i<len;i++) latin1_text[i] = (char) array[i];
  update_buffers = false;
  return latin1_text;
}



/*!
\brief Modifies the string by appending JString \a s on to the end of it. 

Returns a reference the string itself, i.e. (*this).
*/

JString& JString::append(const JString& s) {
  JChar* old_array = array;
  array = new JChar[len + s.len + 1];
  JChar *from = old_array;
  JChar *to = array;
  int i;
  for (i=0;i<len;i++,to++,from++) *to = *from;
  from = s.array;
  for (i=0;i<s.len;i++,to++,from++) *to = *from;
  delete[] old_array;
  len += s.len;
  array[len] = 0;
  update_buffers = true;
  return *this;
}



/*!
\brief Modifies the string by prepending JString \a s on to the start of it. 

Returns a reference to the string itself, i.e. (*this).
*/

JString& JString::prepend(const JString& s) {
  JChar* old_array = array;
  array = new JChar[len + s.len + 1];
  JChar *from = s.array;
  JChar *to = array;
  int i;
  for (i=0;i<s.len;i++,to++,from++) *to = *from;
  from = old_array;
  for (i=0;i<len;i++,to++,from++) *to = *from;
  delete[] old_array;
  len += s.len;
  array[len] = 0;
  return *this;
} 



/*!
\brief Returns a copy of the string with any white space characters at the start or end removed.

Note this does not modify the string itself. White space characters are space, tab or newline (hex. codes 0x20, 0x09 and 0x0a).
*/

JString JString::stripWhiteSpace() const {
  int start = 0;
  int end = len-1;
  while (start<=end && (array[start]==' ' || array[start]==0x9 || array[start]==10)) start++;
  while (start<=end && (array[end]==' ' || array[end]==0x9 || array[start]==10)) end--;
  return JString(array+start, end-start+1);
}



/*!
\brief Returns a copy of the string converted to upper case.

Note this does not modify the string itself. Characters converted are 'a' to 'z'.
*/

JString JString::upper() const {
  JString s(*this);
  for (int i=0;i<len;i++) {
    if (s.array[i]>='a' && s.array[i]<='z') s.array[i] -= 'a'-'A';
  }
  return s;
}



/*!
\brief Returns a copy of the string converted to lower case.

Note this does not modify the string itself. Characters converted are 'A' to 'Z'.
*/

JString JString::lower() const {
  JString s(*this);
  for (int i=0;i<len;i++) {
    if (s.array[i]>='A' && s.array[i]<='Z') s.array[i] += 'a'-'A';
  }
  return s;
}



/*!
\brief Returns a copy of the \a n first characters of the string, i.e. the left-hand portion.
*/

JString JString::left(unsigned int n) const {
  return JString(array, n);
}



/*!
\brief Returns a copy of the \a n final characters of the string, i.e. the right-hand portion.
*/

JString JString::right(unsigned int n) const {
  return JString(array+len-n, n);
}
 


/*!
\brief Returns a substring consisting of the series of subsections defined by the delimiter \a sep and positions \a start to \a end inclusive. 

The string is treated as a sequence of fields separated by the character \a sep. The returned string consists of the fields from position \a start to position \a end inclusive, where zero is the first section. The number of the final section is therefore: s.contains(sep)
*/

JString JString::section(JChar sep, int start, int end) const {

    // return null form zero length string
  if (len==0 || start>end) return JString::null;

  //printf("start %d end %d len %d\n%s<<\n",start,end,len,latin1());
  int count = 0;
  int startp = 0;
  int i = 0;
    // start=0 means section starts at the start of the string
  if (start>0) {
      // otherwise find nth occurence of separator character or character past end of string 
    while (count<start && i<len) { if (array[i++]==sep) count++; }
      // if reached end then no section
    if (i==len) return JString::null;
      // startp is position of first character in section
    startp = i;
  }
    // go back one if not at start because next loop preincrements
  i--;
    // find nth occurence of seperator character or end of string
  while (count<=end && i<len-1) { if (array[++i]==sep) count++; }
  int endp = i;
    // endp is position of final character in section
  if (array[i]==sep) endp--;
  //if (i<0 || i>len) printf("start %d end %d count %d len %d i %d\n",start,end,count,len,i);
  //printf("i %d startp %d endp %d\n",i,startp,endp);
 
  int nlen = endp-startp+1;
  if (nlen<=0) return JString::null;
  return JString(array+startp, nlen);
}



/*!
\brief Converts the string to an \c int. 

Any leading or trailing spaces, tab characters (0x09) or commas are ignored. Any other characters except digits (0-9), a single '.' a single 'e' or 'E', and a single leading '+' or '-' cause a return of zero. Any digits after '.' are ignored so fractional values are rounded down. Simple handling of positive integer exponential notation is allowed, so '1E2', '1E02' or '1e2' will convert correctly to 100, but '1E+2', '100E-2' or '1.2E2' will return zero. 
*/

int JString::toInt() const {
  return toInt(0, len-1);
}



/*!
\brief Converts the segment of a string from \a start_pos to \a end_pos inclusive to an \c int. 

Works identically to toInt(), see the documentation for that function for details.
*/

int JString::toInt(int start_pos, int end_pos) const {

    // first jump start position past any spaces, tabs or commas
  for (; start_pos<len; start_pos++) {
    if (array[start_pos]!=' ' && array[start_pos]!=JChar(9) && array[start_pos]!=',') break; 
  }
    // move end position (inclusive) past any spaces, tabs or commas
  for (;end_pos>=0; end_pos--) {
    if (array[end_pos]!=' ' && array[end_pos]!=JChar(9) && array[end_pos]!=',') break; 
  }
    // quit if string is empty
  if (start_pos > end_pos) return 0;
 
    // one '+' or '-' is allowed at the start
  bool is_minus = false; 
  if (array[start_pos] == '+') start_pos++;
  else if (array[start_pos] == '-') { is_minus = true; start_pos++; }
 
  int dot_pos = end_pos+1;
  int e_pos = end_pos+1;
  int i;

    // now consider the characters which make up the number
  for (i=start_pos; i<=end_pos; i++) {
      // allow one decimal point before any 'e' or 'E' and note the position
    if (array[i]=='.' && dot_pos>end_pos && e_pos>end_pos) { dot_pos = i; }
      // allow one 'e' or 'E' and note position - this also sets the dot position
    else if ((array[i]=='e' || array[i]=='E') && e_pos>end_pos && dot_pos>end_pos) { e_pos = i; dot_pos = i; }
      // otherwise only numbers allowed so quit with zero if anything else seen
    else if ((array[i]<'0' || array[i]>'9')) return 0;
  }

    // an 'e' right at the end makes no sense
  if (e_pos == end_pos) return 0;
    // an 'e' in the middle of the number means we should do a toInt() on the remainder
    // note we already checked there was no other 'e's
    // unit is the power of ten to start with i.e. 1 = 10^0
  int unit = 1;
  if (e_pos < end_pos) { 
    int j = toInt(e_pos+1, end_pos);
    for ( ;j>0; j--) unit *= 10;
  }

    // this will be the result
  int v = 0;

    // now work backwards from the dot position
  for (i=dot_pos-1; i>=start_pos; i--) { 
    v += unit * (array[i]-'0'); 
    unit *= 10;
  }

  return (is_minus) ? -v : v;
}



/*!
\brief Converts the string to an \c double. 

Any leading or trailing spaces, tab characters (0x09) or commas are ignored. Any other characters except digits (0-9), a single '.' a single 'e' or 'E', and a single leading '+' or '-' cause a return of zero.

Note that this function may not give numerically identical results to, for example, the GCC compiler, when the number of significant bits is high. 
This is probably due to rounding errors in the implementation.
*/
 
double JString::toDouble() const { 
  return toDouble(0, len-1); 
}
 


/*!
\brief Converts the segment of the string from \a start_pos to \a end_pos inclusive to a \c double. 

Works identically to toDouble(), see the documentation for that function for details.
*/

double JString::toDouble(int start_pos, int end_pos) const {

    // first jump start position past any spaces, tabs or commas
  for (; start_pos<len; start_pos++) {
    if (array[start_pos]!=' ' && array[start_pos]!=JChar(9) && array[start_pos]!=',') break; 
  }
    // move end position (inclusive) past any spaces, tabs or commas
  for (;end_pos>=0; end_pos--) {
    if (array[end_pos]!=' ' && array[end_pos]!=JChar(9) && array[end_pos]!=',') break; 
  }
    // quit if string is empty
  if (start_pos > end_pos) return 0;

    // a '+' or '-' is allowed at the start
  bool is_minus = false; 
  if (array[start_pos] == '+') start_pos++;
  else if (array[start_pos] == '-') { is_minus = true; start_pos++; }

  int e_pos = end_pos + 1; 
  int expon = 0;

    // check for a single E or e, return zero if more than one
  for (int i=start_pos; i<=end_pos; i++) {
    if (array[i]=='e' || array[i]=='E') {
      if (e_pos > end_pos) e_pos = i;
      else return 0;
    }
  }
 
    // an 'e' or 'E' right at the end makes no sense
  if (e_pos == end_pos) return 0;
    // if found an 'e' or 'E' then get the exponent
  if (e_pos < end_pos) {
    expon = toInt(e_pos+1, end_pos);
    end_pos = e_pos -1;
  }

    // default place for the decimal point
  int dot_pos = end_pos + 1;

    // now consider the characters which make up the number
  for (int i=start_pos; i<=end_pos; i++) {
      // allow one decimal point before any 'e' or 'E' and note the position
    if (array[i]=='.' && dot_pos>end_pos) { dot_pos = i; }
       // otherwise only numbers allowed so quit with zero if anything else seen
    else if ((array[i]<'0' || array[i]>'9')) return 0;
  }

    // units to start with on left of decimal point
  double unit = pow(10, expon);
    // this will be the result
  double v = 0;
    // first do the part to teh left of the decimal point
  for (int i=dot_pos-1; i>=start_pos; i--) { 
    v += unit * (array[i]-'0'); 
    unit *= 10; 
  }
    // units to start with on the right of the decimal point
  unit = pow(10, expon-1);
    // now do the part to the right of the point
  for (int i=dot_pos+1; i<=end_pos; i++) { 
    v += unit * (array[i]-'0'); 
    unit /= 10;
  }

  return is_minus ? -v : v;
}



/*!
\brief Make the string a copy of a C-style null terminated character string. Returns a const reference to the resultant string.
*/

const JString& JString::operator=(const char* text) {
  const char *p = text;
  while (*p) p++;
  allocate(p-text);
  for (int i=0;i<len;i++,text++) array[i] = *text;
  update_buffers = true;
  return *this;
}



/*!
\brief Make the string a copy of JString \a s. Returns a const reference to the resultant string.
*/

const JString& JString::operator=(const JString& s) {
  allocate(s.len);
  const JChar *from = s.array;
  JChar *to = array;
  for (int i=0;i<len;i++,from++,to++) *to = *from;
  update_buffers = true;
  return *this;
}



/*!
\brief Appends a null terminated character string. Returns a const reference to the resultant string.
*/

const JString& JString::operator+=(const char* text) {
  append(JString(text));
  return *this;
}



/*!
\brief Appends JString \a s. Returns a const reference to the resultant string.
*/

const JString& JString::operator+=(const JString& s) {
  append(s);
  return *this;
}



/*!
\brief Makes a string representation of a \c double value.

The number of digits to the right of the decimal point can be set by \a prec which may be between 0 and 36. The string will be in scientific format if \a c is 'e', i.e. -d.ddd+-dd, or fixed if \a c is 'f', i.e. +-ddd.ddd. Internally the standard function sprintf() is used so other characters for \a \c are possible, the default 'g' will use exponential or fixed notation depednent on the value \a v and the precision \a prec. See the documentation for sprintf(). 
*/

JString JString::number(double v, const char c, int prec) {
  static char format[6];
  static char buffer[40];
  if (prec>36) prec=36;
  if (prec<0) prec = 0;
  format[0] = '%';
  format[1] = '.';
  if (prec<10) {
    format[2] = '0'+prec;
    format[3] = c;
    format[4] = 0;
  } else {
    format[2] = '0' + (prec/10);
    format[3] = '0' + (prec%10);
    format[4] = c;
    format[5] = 0;
  }
  sprintf(buffer,format,v);
  return JString(buffer);
}



/*!
\brief Makes a string representation of an \c int value.
*/

JString JString::number(int v) {
  static const char* format = "%d";
  static char buffer[40];
  sprintf(buffer,format,v);
  return JString(buffer);
}



/*!
\brief Makes a string representation of an \c int value.

\todo The implementation of this function may not be portable.
*/

JString JString::number(int64 v) {
  static const char* format = "%lld";
  static char buffer[40];
  sprintf(buffer,format,v);
  return JString(buffer);
}



/*!
\brief Appends two JStrings and returns the result.
*/

const JString jude::base::operator+(const JString& s1, const JString &s2) {
  JString s(s1);
  s.append(s2);
  return s;
}



/*!
\brief Returns true if two JStrings are equal in both length and content.
*/

bool jude::base::operator==(const JString& s1, const JString &s2) {
  if (s1.len != s2.len) return false;
  for (int i=0;i<s1.len;i++) if (s1.array[i] != s2.array[i]) return false;
  return true;
}



/*!
\brief Returns true if two JStrings differ in either length or content.
*/

bool jude::base::operator!=(const JString& s1, const JString &s2) {
  if (s1.len != s2.len) return true;
  for (int i=0;i<s1.len;i++) if (s1.array[i] != s2.array[i]) return true;
  return false;
}



/*!
\brief Writes a JString out to a JDataStream.
*/

JDataStream& jude::base::operator<<(JDataStream& str, const JString& s) {
  str.writeBytes((char*)s.array, (s.len<<1)+2);
  return str;
}



/*!
\brief Reads a JString from a JDataStream.
*/

JDataStream& jude::base::operator>>(JDataStream& str, JString& s) {
  uns32 len;
  delete[] s.array;
  str.readBytes((char*&)s.array, len);
  s.len = (len-2) >> 1;
  s.update_buffers = true;
  return str;
}



/*! \fn int JString::length() const
\brief Returns the length of the string (not including a termination zero character).
*/



/*! \fn const JChar* JString::unicode() const
\brief Returns a pointer to the JString internal representation as a zero-terminated array of 16-bit unicode JChar items.
*/



/*! \fn JChar JString::at(int i) const
\brief Returns the JChar character at position \a i in the string, where \a i starts at zero for the first character.

Note that no bounds checking is conducted.
*/

