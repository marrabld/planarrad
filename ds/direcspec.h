
// direcspec.h

#ifndef _JUDE_DS_DIRECSPEC_H
#define _JUDE_DS_DIRECSPEC_H

#include "base/jstring.h"
#include "base/jlist.h"
#include "base/jtextstream.h"
#include "base/jparser.h"

using namespace jude::base;

namespace jude {
namespace ds {



class DirecSpec {

    // if the directional specification was read froma dedicated file this is its path
    // otherwise is empty
  JString filepath_str;

    // name and code for this DirecSpec
    // optional but name at least is required for menus in GUI context
  JString name_str;
  JString code_str;

    // the directional spec is stored as a set of parser options here
  JParser prm;

    // list of available DirecSpecs read in in GUI context programs
  static JList<DirecSpec> available_list;
    // the default in the list
  static DirecSpec* default_ds_spec;

    // default constructor unspecified
  DirecSpec();
    // constructor which just sets filepath is private
  DirecSpec(const JString& fp);

public:
  
    /*! \brief Initialises and clears the global list of available directional specifications. */
  static void initAvailableList();
    /*! \brief If a directional specification can be found in \a prm using \a prefix the create and add it. */
  static DirecSpec* addToAvailableList(const JParser& prm, const JString& prefix, bool make_default);
    /*! \brief Populates the static list of available directional specifications by reading files in the directory \a dir_path. */
  static bool addToAvailableList(const JString& dir_path);
  static bool addDefaultToAvailableList();
    /*! \brief Returns a reference to the global list of available directional specifications. */
  static JList<DirecSpec>& availableList() { return available_list; }
    /*! \brief Returns the default directional specification in the global list of available directional specifications. */
  static const DirecSpec* availableDefault() { return default_ds_spec; }

    /*! \brief Creates a directional specification directly from the supplied parser data or via a referenced file. */
  static DirecSpec* create(const JParser& prm, const JString& prefix);
    /*! \brief Creates a directional specification directly from the supplied parser data or via a referenced file. */
  static DirecSpec* create(const JParser& prm, const JString& prefix, bool& preferred_default);
    /*! \brief Creates a diectional specification directly from a file, specified by \a file_path. */
  static DirecSpec* createFromFile(const JString& file_path, bool& preferred_default);
 
  const JParser& parser() const { return prm; }

    /*! \brief Reads in the directional specification from the parser \a prm from parameter labels prefixed by \a prefix. */
  bool readFromParser(const JParser& prm, const JString& prefix, bool& preferred_default);
    /*! \brief Adds parameters to reconstruct the band specification to \a prm with labels prefixed with \a prefix. */ 
  void addToParser(JParser& targ_prm, const JString& prefix) const;

    /*! \brief Returns the name of the directional specification. */
  JString name() const { return name_str; }
    /*! \brief Sets the name of the directional specification. */
  void setName(const JString& s) { name_str = s; }

    /*! \brief Returns the code of the directional specification. */
  JString code() const { return code_str; }
    /*! \brief Sets the code of the directional specification. */
  void setCode(const JString& s) { code_str = s; }


  JString filepath() const { return filepath_str; }

     /*! \brief Display brief help text on parameters required to build a directional specification. */  
  static void helpText(JTextStream& str);

     /*! \brief Display info on the directional specification on a text stream. */
  void display(JTextStream& str) const;
};



} } // namespace jude::tool

#endif // _JUDE_DS_DIRECSPEC_H

