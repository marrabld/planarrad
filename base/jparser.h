
// jparser.h

#ifndef _JUDE_TOOL_JPARSER_H
#define _JUDE_TOOL_JPARSER_H

#include "base/jlist.h"
#include "base/jstring.h"
#include "base/jtextstream.h"
#include "base/jtexttable.h"
#include "base/jcolour.h"

namespace jude {
namespace base {



/*! For reading and writing of parameters from files or the command line.

\todo This shouldn't really be in jude::base because then can't read items such as Vec3D.
*/


class JParser {

  JString base_dir;

  JList<JString> name_list;
  JList<JString> value_list;

  JList<JString> hyphen_param_list;
  JList<JString> string_list;

  //int orig_argc;
  //char **orig_argv;

  mutable bool not_defined_error;

  void addPair(const JString& line);
  void addHyphenParam(const JString& line);
  void addString(const JString& line);

  JString* findName(const JString& name, int& index) const;
  JString* findNameOrError(const JString& name) const;

public:

  JParser();
 
  const JString& baseDir() const { return base_dir; }

  bool readFile(const JString& filepath, int vb_level=2);
  bool writeFile(const JString& filepath) const;
  void processCommandLine(int argc, char **argv); 

  void clear();
  void removeLabel(const JString& var_name);

  void setLabelAndValue(const JString& var_name, const JString& var_value);
  void setLabelAndValue(const JString& var_name, int i);
  void setLabelAndValue(const JString& var_name, double v);
  void setLabelAndValue(const JString& var_name, const std::vector<double>& v);

  void appendOrOverwriteEntries(const JParser& prm, const JString& prefix);

  void setFromEnvVarIfDefined(const JString& name, const JString& env_var);

  void copyLabelAndValue(const JParser& prm, const JString& var_name);

  bool somethingWasNotDefined() const { return not_defined_error; }
  void clearSomethingWasNotDefinedFlag() { not_defined_error = false; }

  bool isDefined(const JString& name) const;
  bool isDefinedAndNotEmpty(const JString& name) const;
  bool isDefinedAndTrueBool(const JString& name) const;

  bool valueCheck(const JString& name, const JString& value) const;
  bool valueCheckOrFalse(const JString& name, const JString& value) const;

  bool getBool(const JString& name) const;
  int getInt(const JString& name) const;
  double getDouble(const JString& name) const;
  const JString& getString(const JString& name) const;
  const JString getStringOrEmpty(const JString& name) const;

     // these versions may be more useful
  bool getBool(bool& v, const JString& name) const;
  bool getInt(int& v, const JString& name) const;
  bool getDouble(double& v, const JString& name) const;

  //const Vec3D getVec3D(const JString& name) const;

  const std::vector<int> getVectorOfInts(const JString& name) const;
  const std::vector<double> getVectorOfDoubles(const JString& name) const;
  const std::vector<JString> getVectorOfStrings(const JString& name) const;

  const JColour getColour(const JString& name) const;

  bool getTextTableColumn(const JString& name, JTextTable& ttab, int col) const;

  int paramCount() const { return name_list.count(); }
  const JString& getName(int i) const { return *name_list.at(i); }
  const JString& getValue(int i) const { return *value_list.at(i); }

  int filePathCount() const { return string_list.count(); }
  const JString& getFilePath(int i) const { return *string_list.at(i); }

  int hyphenParamCount() const { return hyphen_param_list.count(); }
  const JString& getHyphenParam(int i) const { return *hyphen_param_list.at(i); }

  void writeParams(JTextStream& str) const;
  void displayList(JTextStream& str) const;
 
  //int originalArgc() { return orig_argc; }
  //char** originalArgv() { return orig_argv; }
};



} } // namespace::base

#endif // _JUDE_TOOL_JPARSER_H

