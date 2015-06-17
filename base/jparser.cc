
// jparser.cc
  
#include "base/jparser.h"
#include "base/jfile.h"
#include "base/jtextstream.h"
#include "base/jlog.h"
#include <stdlib.h>

using namespace jude::base;



// JParser::(constructor)

JParser::JParser() :
  not_defined_error(false) {

  name_list.setAutoDelete(true);
  value_list.setAutoDelete(true);
  string_list.setAutoDelete(true);

  base_dir = ".";
}



// JParser::readFile()

bool JParser::readFile(const JString& filepath, int vb_level) {

    // remember the base directory because any referenced files will first be assumed to be relative to here
  base_dir = filepath.section('/',0,filepath.contains('/')-1);

     // open the file
  JFile f(filepath);
  if (!f.open(JIO_ReadOnly)) { 
    if (jlog::vbLevel(vb_level)) jlog::ls.printf("Couldn't open file '%s'.\n",filepath.latin1()); 
    return false; 
  }
  
    // get a text stream on the file
  JTextStream str(&f);       

  JString line;

  while (!f.atEnd()) {
    
      // read next line ignoring comments and empty lines
    do { line = str.readLine(); } 
    while ((line.isEmpty() || line.stripWhiteSpace().left(1)=="#") && !f.atEnd());

    if (f.atEnd()) break;

    if (line.contains('=')==0) { 
      if (jlog::vbLevel(vb_level)) jlog::ls.printf("Syntax error in file (no '=' on a line).\n"); 
      return false; 
    }

    if (line.contains('=')>1) { 
      if (jlog::vbLevel(vb_level)) jlog::ls.printf("Syntax error in file (more than one '=' on a line).\n"); 
      return false; 
    }

    addPair(line);
  }  

  f.close();

  return true;
}



// JParser::writeFile()

bool JParser::writeFile(const JString& filepath) const {

     // open the file
  JFile f(filepath);
  if (!f.open(JIO_WriteOnly)) { printf("Couldn't open file '%s'\n",filepath.latin1()); return false; }
  
    // get a text stream on the file
  JTextStream str(&f);       
  displayList(str);
  f.close();

  return true;
}



// JParser::processCommandLine()

//! Read in parameters from the command line. argc and argv should be supplied with argc-1 and argv+1 directly from main()
//! to strip off the executable name.
//! Parameters on the command line must be supplied in the form <param_name>=<value> with no spaces.
//! The special parameter params=<param_filepath> will load in parameters from a text file.
//! Arguments that do not contain an '=' are added as to the filename list.
//! Command line arguments override file supplied arguments with the same name.
//! Parameters with more than one '=' are ignored.

void JParser::processCommandLine(int argc, char **argv) {

    // find parameter files first since want command line to override file supplied parameters.
  for (int i=0; i<argc; i++) {
      // get this argument
    JString line(argv[i]);
      // name and value
    JString var_name(line.section(JChar('='), 0, 0).stripWhiteSpace());
    JString var_value(line.section(JChar('='), 1, 1).stripWhiteSpace());
    if (var_name == "params") readFile(var_value);
  }

    // now go through again and add all as pair or filenames or '-' prefixed options
  for (int i=0; i<argc; i++) {
      // get this parameter
    JString line(argv[i]);
    line = line.stripWhiteSpace();
      // add a parameter-value pair
    if (line.contains('=')==1) addPair(line);
      // add a hypenated parameter
    else if (line.left(1)=="-") addHyphenParam(line);
      // add a filename
    else if (line.contains('=')==0) addString(line);
  }
}



// JParser::addPair() [private]

void JParser::addPair(const JString& line) {

  JString var_name(line.section(JChar('='), 0, 0).stripWhiteSpace());
  JString var_value(line.section(JChar('='), 1, 1).stripWhiteSpace());

  if (var_value.left(1)=="\"" && var_value.right(1)=="\"") {
    var_value = var_value.left(var_value.length()-1);
    var_value = var_value.right(var_value.length()-1);
  }

  int index;
  JString* v = findName(var_name, index);

  if (v) { *v = var_value; return; }

  name_list.append(new JString(var_name));
  value_list.append(new JString(var_value));
}



// JParser::addHyphenParam() [private]

void JParser::addHyphenParam(const JString& line) {

  JString s = line.stripWhiteSpace();
  unsigned int hyphen_count = 0;

  while (hyphen_count < s.length()) {
    if (s.at(hyphen_count) != '-') break;
    hyphen_count++;
  }

  s = s.right(line.length()-hyphen_count);
  if (s.isEmpty()) return;

  hyphen_param_list.append(new JString(s));
}



// JParser::addString() [private]

void JParser::addString(const JString& line) {
  string_list.append(new JString(line.stripWhiteSpace()));
}



// JParser::findName() [private]

JString* JParser::findName(const JString& name, int& index) const {

  JListIterator<JString> iter_n(name_list);
  JString* n = iter_n.first();

  JListIterator<JString> iter_v(value_list);
  JString* v = iter_v.first();

  index = 0;
  while (n) {
    if (*n==name) return v;
    n = iter_n.next();
    v = iter_v.next();
    index++;
  }

  return 0;
}



// JParser::findNameOrError()

JString* JParser::findNameOrError(const JString& name) const {
  int index;
  JString* v = findName(name, index);
  if (v) return v;
  jlog::es.printf("Required parameter '%s' not found\n",name.latin1());
  not_defined_error = true;
  return 0;
}



// JParser::clear()

void JParser::clear() {
  name_list.clear();
  value_list.clear();
  string_list.clear();
} 



void JParser::removeLabel(const JString& var_name) {

  int index;
  if (!findName(var_name, index)) return;

  name_list.at(index);
  name_list.removeAtCurrentPos();

  value_list.at(index);
  value_list.removeAtCurrentPos();
}



// JParser::setLabelAndValue()

void JParser::setLabelAndValue(const JString& var_name, const JString& var_value) {

  int index;
  JString* v = findName(var_name, index);

  if (v) { *v = var_value; return; }

  name_list.append(new JString(var_name));
  value_list.append(new JString(var_value));
}



void JParser::setLabelAndValue(const JString& var_name, int i) {
  setLabelAndValue(var_name, JString::number(i));
}



void JParser::setLabelAndValue(const JString& var_name, double v) {
  setLabelAndValue(var_name, JString::number(v));
}



// JParser::setLabelAndValue()

void JParser::setLabelAndValue(const JString& var_name, const std::vector<double>& v) {

  JString var_value;
  for (unsigned int i=0; i<v.size(); i++) {
    var_value += JString::number(v[i]);
    if (i < v.size()-1) var_value += ",";
  }

  setLabelAndValue(var_name, var_value);
}


void JParser::appendOrOverwriteEntries(const JParser& prm, const JString& prefix) {
 
  JListIterator<JString> iter_n(prm.name_list);
  JString* n = iter_n.first();

  JListIterator<JString> iter_v(prm.value_list);
  JString* v = iter_v.first();

  while (n) {
    setLabelAndValue(prefix+(*n), *v);
    n = iter_n.next();
    v = iter_v.next();
  }
}



// JParser::setFromEnvVarIfDefined()
// adds or overwites (name) with the content of environmental variable (env_var)
// if the env_var is not defined or is empty then no action is taken

void JParser::setFromEnvVarIfDefined(const JString& name, const JString& env_var) {
  JString s(getenv(env_var.latin1()));
  if (!s.isEmpty()) setLabelAndValue(name, s);
}



/*! \brief Copies the label-value pair of \a name from \a prm. */

void JParser::copyLabelAndValue(const JParser& prm, const JString& name) {
  if (!isDefined(name)) return;
  setLabelAndValue(name, prm.getString(name));
}



/*! \brief Returns true if the label \a name is defined. */ 

bool JParser::isDefined(const JString& name) const {
  int index;
  return (findName(name, index) != 0);
}


/*! \brief Returns true if the label \a name is defined and is not blank. */ 

bool JParser::isDefinedAndNotEmpty(const JString& name) const {
  if (!isDefined(name)) return false;
  return (!getString(name).isEmpty()); 
}



/*! \brief Returns true if the label \a name is defined and is a true bool value, i.e. 'yes', 'true' or '1'. */ 

bool JParser::isDefinedAndTrueBool(const JString& name) const {
  if (!isDefined(name)) return false;
  return getBool(name);
}



// JParser::valueCheck()

bool JParser::valueCheck(const JString& name, const JString& value) const {
  JString* v = findNameOrError(name);
  if (!v) return false;
  return (*v==value);
}



// JParser::valueCheckOrFalse()

bool JParser::valueCheckOrFalse(const JString& name, const JString& value) const {
  int index;
  JString* v = findName(name, index);
  if (!v) return false;
  return (*v==value);
}



// JParser::getBool()

bool JParser::getBool(const JString& name) const {
  JString* v = findNameOrError(name);
  if (!v) return false;
  return (*v == "true" || *v == "yes" || *v == "1");
}



// JParser::getInt()

int JParser::getInt(const JString& name) const {
  JString* v = findNameOrError(name);
  if (!v) return 0;
  return v->toInt();
}



// JParser::getDouble()

double JParser::getDouble(const JString& name) const {
  JString* v = findNameOrError(name);
  if (!v) return 0;
  return v->toDouble();
}



// JParser::getString()

const JString& JParser::getString(const JString& name) const {
  JString* v = findNameOrError(name);
  if (!v) return JString::null;
  return *v;
}



// JParser::getStringOrEmpty()

const JString JParser::getStringOrEmpty(const JString& name) const {
  int index;
  JString* v = findName(name, index);
  if (!v) return "";
  return *v;
}



// JParser::getBool()

bool JParser::getBool(bool& v, const JString& name) const {
  JString* v_str = findNameOrError(name);
  if (!v_str) return false;
  v = (*v_str == "true" || *v_str == "yes" || *v_str == "1");
  return true;
}



// JParser::getInt()

bool JParser::getInt(int& v, const JString& name) const {
  JString* v_str = findNameOrError(name);
  if (!v_str) return false;
  v = v_str->toInt();
  return true;
}



// JParser::getDouble()

bool JParser::getDouble(double& v, const JString& name) const {
  JString* v_str = findNameOrError(name);
  if (!v_str) return false;
  v = v_str->toDouble();
  return true;
}



// JParser::getVec3D()
/*
const Vec3D JParser::getVec3D(const JString& name) const {

  JString* s = findNameOrError(name);
  if (!s) return Vec3D(0,0,0);

  if (!s.contains(',') != 2) {
    jlog::es.printf("Field '%s' should be a 3-vector but does not contain two commas\n",name.latin1());
    not_defined_error = true;
    return Vec3D(0,0,0);
  }

  v.setX(s.section(',',0,0).toDouble());
  v.setY(s.section(',',0,0).toDouble());
  v.setZ(s.section(',',0,0).toDouble());

  return v;
}
*/


// JParser::getVectorOfInts()
// FIXME should throw an error if not defined?

const std::vector<int> JParser::getVectorOfInts(const JString& name) const {

  std::vector<int> v;

  if (!isDefined(name)) return v;

  JString s = getString(name).stripWhiteSpace();
  int num = s.contains(',') + 1;
  v.resize(num);
  for (int i=0; i<num; i++) v[i] = s.section(',',i,i).toInt(); 

  return v;
}



// JParser::getVectorOfDoubles()
// FIXME should throw an error if not defined?

const std::vector<double> JParser::getVectorOfDoubles(const JString& name) const {

  std::vector<double> v;

  if (!isDefined(name)) return v;

  JString s = getString(name).stripWhiteSpace();
  int num = s.contains(',') + 1;
  v.resize(num);
  for (int i=0; i<num; i++) v[i] = s.section(',',i,i).toDouble(); 

  return v;
}



const JColour JParser::getColour(const JString& name) const {

  std::vector<double> v(getVectorOfDoubles(name));

  if (v.size()==0) return JColour(0,0,0);

  int r = (int) (255 * v[0]);
  int g = (int) (255 * v[1 % v.size()]);
  int b = (int) (255 * v[2 % v.size()]);

  return JColour(r,g,b);
}



// JParser::getVectorOfDoubles()
// FIXME should throw an error if not defined?

const std::vector<JString> JParser::getVectorOfStrings(const JString& name) const {

  std::vector<JString> v;

  if (!isDefined(name)) return v;

  JString s = getString(name).stripWhiteSpace();
  int num = s.contains(',') + 1;
  v.resize(num);
  for (int i=0; i<num; i++) v[i] = s.section(',',i,i); 

  return v;
}



// JParser::getTextTableColumn()
// FIXME should throw an error if not defined?

bool JParser::getTextTableColumn(const JString& name, JTextTable& ttab, int col) const {

  if (!isDefined(name)) return false;

  JString s = getString(name).stripWhiteSpace();
  int num = s.contains(',') + 1;
  for (int i=0; i<num; i++) ttab.setText(i, col, s.section(',',i,i)); 

  return true;
}



// JParser::writeParams()

void JParser::writeParams(JTextStream& str) const {

  JListIterator<JString> iter_n(name_list);
  JString* n = iter_n.first();

  JListIterator<JString> iter_v(value_list);
  JString* v = iter_v.first();

  while (n) {
    str.printf("%s = %s\n", n->latin1(), v->latin1());
    n = iter_n.next();
    v = iter_v.next();
  }
}



// JParser::displayList()

void JParser::displayList(JTextStream& str) const {
 
  writeParams(str);

  JListIterator<JString> iter_h(hyphen_param_list);
  JString* s = iter_h.first();

  int count = 0;
  while (s) {
    str.printf("hypen %d: %s\n", count, s->latin1());
    count++;
    s = iter_h.next();
  }

  JListIterator<JString> iter_s(string_list);
  s = iter_s.first();

  count = 0;
  while (s) {
    str.printf("file %d: %s\n", count, s->latin1());
    count++;
    s = iter_s.next();
  }
}


