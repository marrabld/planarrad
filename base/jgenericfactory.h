
// jgenericfactory.h

#ifndef _JUDE_BASE_JGENERICFACTORY_H
#define _JUDE_BASE_JGENERICFACTORY_H

#include "base/jtypes.h"
#include "base/jfile.h"
#include "base/jtextstream.h"
#include "base/jlog.h"
#include <map>
#include <string>
#include <stdio.h>


namespace jude {
namespace base {


template <typename B>
class JGenericFactory {

  typedef B* (*CreateFuncPtr)();
  typedef std::map<std::string, CreateFuncPtr> RegSToP;
  typedef std::map<CreateFuncPtr, std::string> RegPToS;

    // a map associating key strings with create functions
  RegSToP reg_s_to_p;
  RegPToS reg_p_to_s;

  JGenericFactory() { }
  JGenericFactory(const JGenericFactory&);             // not implemeented to prevent instantiation
  JGenericFactory& operator=(const JGenericFactory&);  // not implemeented to prevent instantiation

public:

  static JGenericFactory& instance() { static JGenericFactory gf; return gf; } 

  void RegCreateFunc(const std::string& s, CreateFuncPtr fn_ptr) { 
    //jlog::ls << "Registered in factory '" << &s[0] << "'\n.";
    reg_s_to_p[s] = fn_ptr; 
    reg_p_to_s[fn_ptr] = s; 
  }

  B* create(const std::string& s) { 
    typename std::map<std::string, CreateFuncPtr >::iterator r = reg_s_to_p.find(s);
    if (r != reg_s_to_p.end()) return r->second();
    return 0;
  }

  bool isRegistered(const std::string& s) { 
    typename std::map<std::string, CreateFuncPtr >::iterator r = reg_s_to_p.find(s);
    return (r != reg_s_to_p.end());
  }

  const std::string& getString(CreateFuncPtr fn_ptr) {
    typename RegPToS::iterator r = reg_p_to_s.find(fn_ptr);
    if (r != reg_p_to_s.end()) return r->second();
    return "";
  }

  void display(JTextStream& str) {
    jlog::ls << "Factory IDs:\n";
    typename RegPToS::iterator r = reg_p_to_s.begin();
    while (r != reg_p_to_s.end()) {
      jlog::ls << &(r->second[0]) << "\n";
      ++r;
    }
  }
};



template <class B, class T>
class RegisterInFactory {

public:

  static B* createInstance() { return new T(); }

  RegisterInFactory(const std::string& s) { 
    if (JGenericFactory<B>::instance().isRegistered(s)) return;
    JGenericFactory<B>::instance().RegCreateFunc(s, createInstance);
  }

  const std::string& getString() {
    return JGenericFactory<B>::instance().getString(createInstance);
  }
};



//JFactory<SpherePartVxH> registerBase<DirecStruct>("sphere");


template <class B, class T>
class JFactory {

  JString id_str;

  JFactory() { }
  JFactory(const JFactory&);
  JFactory& operator=(const JFactory&);  // not implemeented to prevent instantiation

public:

  // template<typename B>
  //JFactory(const JString& s) {
  // id_str = s; 
  //  RegisterInFactory<B,T> t(s.latin1()); 
  // }

  static JFactory& instance() { static JFactory f; return f; } 

  void reg() {
    id_str = T::staticFactoryID(); 
    //printf("Registered in factory %p '%s'\n", this, id_str.latin1());
    RegisterInFactory<B,T> t(id_str.latin1()); 
  }

  //void init() { }

  const JString& id() { 
    //printf("Request %p '%s'\n", this, id_str.latin1());
    return id_str; 
  } 
};


// we dont know if type A provides staticFactoryID() so provide it here for basic types

template <typename A> 
inline const JString staticIdOf() { return A::staticFactoryID(); }

template <>
inline const JString staticIdOf<float>() { return "flt"; }

template <>
inline const JString staticIdOf<double>() { return "dbl"; }


// to use put a single static object
//
// RegisterInFactory<Base, Derived> registerMe("type=deriv");
//
// note use of map would prevent duplication if more than one object is instantiated
//
// e.g.
// RegisterInFactory< DirecStruct, SpherePartVxH > register("partition=sphere is_full=true");
// RegisterInFactory< RadDirec, RadDirecTable<SBandData, SpherePartVxH> > register("partition=sphere is_full=true");
// 
// note that the second does not overwrite the first because DirecStruct factory and RadDirec factory are different objects.
// the JParser supplied may contain other label-value pairs so need to sort on those they do have in common, but the first must all be specified
// can do in map? probably not
//
// JParser::JParser(const JString& init);
// JParser JParser::subset("partition is_full") const;
//
// whynot just have single specifier string??
// partition = full_sphere
// partition = hemi_sphere
// partition = full_cube
// partition = hemi_cube
// because need also to have storage type
// storage_type = table
// storage_type = sparse
//
// template <typename A, typename DS>
// class RegRadDirec : public RegisterInFactory< RadDirec, RadDirecTable<A, DS> > {
//
// }
//
// RegRadDirec<SBandData, > register(DS::specString());
//
// RegisterInFactory< RadDirec, RadDirecAdap<A, DS> > register(DS::specString());


} } // namespace jude::base

#endif // _JUDE_BASE_JGENERICFACTORY_H

