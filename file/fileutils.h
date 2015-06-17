
// fileutils.h

#ifndef _JUDE_FILE_FILEUTILS_H
#define _JUDE_FILE_FILEUTILS_H

#include "base/jversion.h"
#include "base/jmagicnum.h"
#include "base/jstring.h"
#include "base/jdatastream.h"
#include "base/jtextstream.h"
#include "base/jfile.h"
#include "base/jutil.h"
#include "base/jgenericfactory.h"
#include "file/filedesc.h"
#include "file/fileinfo.h"

using namespace jude::base;

namespace jude {
namespace file {

class FileUtils {

public:

  static inline bool readTextLine(JString& line, JTextStream& str);

  template <typename T>
  static bool writeFactoryBuiltToFile(const T& obj, const FileDesc& fd, const JString& notes_text="");

  template <typename T>
  static bool readFactoryBuiltFromFile(T& obj, const FileDesc& fd, FileInfo* fi=0);

  template <typename B>
  static B* createFactoryBuiltFromFile(const FileDesc& fd, FileInfo* fi=0);

  template <typename T>
  static bool writeTextFile(const T& obj, const FileDesc& fd);


  //template <typename T>
  //bool readNonFactoryFromFile(T& obj, const FileDesc& fd);

};



/*! \brief Reads a line of text from \a str int \a line, discarding lines that are entirely white space or start with a '#'.
  Returns \c false if the end of stream was reached and the string is empty.
*/
  
inline bool FileUtils::readTextLine(JString& line, JTextStream& str) {

  do { line = str.readLine().stripWhiteSpace(); } 
  while ((line.isEmpty() || line.left(1)=="#") && !str.atEnd());

  return !(str.atEnd() && line.isEmpty());
}



template <typename T>
T* createFromFileNonPolymorphic(const FileDesc& fd) {

  T* obj_ptr = new T();
  if (!obj_ptr) return 0;

  if (!readFromFile(*obj_ptr, fd)) { delete obj_ptr; return 0; }

  return obj_ptr;
}



/*! \brief Creates an object of base type \a B from a data file using polymorphic type data from the file and objects read() function. 

  All reading of data files should be done in the jude library should ve done with this or the readFromFile() function. 
  Returns a null pointer if any error occurs. Note that the returned type may be a derived class of T.\n
  Normally non-polymorphic types will not be registered in the generic factory and so should not use this function. 
  They should be created first and then read with readFromFile().
*/

template <typename B>
B* FileUtils::createFactoryBuiltFromFile(const FileDesc& fd, FileInfo* fi) {

    // open the file
  JFile f(fd.path().latin1());
    // check opened ok
  if (!f.open(JIO_ReadOnly)) return 0;

    // start on the presumption somthing will go wrong
  bool all_ok = false;
    // this will be become a pointer to the object (might be base class of the actual object)
  B* obj_ptr = 0;

  uns32 magic_num;
  uns32 saved_version;
  
  switch (fd.isTextFile()) { 
  case true : {

    // to do

      // don't do next bit
    break;
  }
  case false : {

      // get a data stream from the file
    JDataStream str(&f);

      // read magic number and quite with error if no good
    str >> magic_num;
    if (magic_num != JUDE_MAGICNUM) { jlog::es << "File '" << fd.path() << "' is not of the right type or is corrupt.\n"; break; }

      // read the saved version
    str >> saved_version;
      // save the version to the file descriptor
    if (fi) fi->setSavedVersion(saved_version);

      // get factory id of the object which was saved
    JString factory_id;
    if (!readFromDataStream(factory_id, str, saved_version)) break;

    if (jlog::ls.vbLevel(6)) jlog::ls << "Factory ID: " << factory_id << "\n";

      // load notes if version is 2.2.0 or later
    if (saved_version >= VERSION_CODE(2, 2, 0)) {
      JString s;
      str >> s;
      if (fi) fi->setNotesText(s);
    }

      // create the object from the factory    
    obj_ptr = JGenericFactory<B>::instance().create(factory_id.latin1());
      // if couldn't create the objevct then quit with error
    //if (!v_ptr) { jlog::es << "File is not of a type supported by this program,\n"; break; }
      // cast it to the required type
    //obj_ptr = dynamic_cast<T*>(v_ptr);
      // if cast failed the return null because the loaded object wasn't a derived class of B
    if (!obj_ptr) break;

      // read the object from the file using the JDataStream template function
      // since for full generality T could be a basic type
    if (!readFromDataStream(*obj_ptr, str, saved_version)) break;

      // read the magic number again
    str >> magic_num;
    if (magic_num != JUDE_MAGICNUM) { jlog::es << "File '" << fd.path() << "' is not of the right type or is corrupt.\n"; break; }

      // everything went ok
    all_ok = true;
  }
  }

    // if something went wrong then delete the object if it was created
  if (!all_ok && obj_ptr!=0) {
    delete obj_ptr;
    obj_ptr = 0;
  }
  
    // close the file
  f.close();
  
  return obj_ptr;
}



/*! \brief Reads \a obj from a data file using its read() function. Returns \a false if any error occurs.

  All reading of data files should be done in the jude library should ve done with this or the createFromFile() function. 
  This function requires that \a obj is already created and is of the correct polymorphic type for the file being read.
  If \a obj is not of the type which corresponds to the data file an error will be streamed to jlog::es and the return is false.
  Non-polymorphic types which are not registered in the generic factory will have a factory id of an empty string.
  In that case the above function createFromFile() cannot be used, instead the object should be created first then read with this.
*/

template <typename T>
bool FileUtils::readFactoryBuiltFromFile(T& obj, const FileDesc& fd, FileInfo* fi) {

    // open the file
  JFile f(fd.path().latin1());
    // check opened ok
  if (!f.open(JIO_ReadOnly)) return false;

  bool ret_val = false;
  uns32 magic_num;
  uns32 saved_version;

  switch (fd.isTextFile()) {
  case true : {

      // get a text stream from the file
    JTextStream str(&f);
      // write the object to the file
    ret_val = obj.read(str);
      // don't do next bit
    break;
  }
  case false : {

      // get a data stream from the file
    JDataStream str(&f);

      // read magic number
    str >> magic_num;
    if (magic_num != JUDE_MAGICNUM) { jlog::es << "File is not the right type or is corrupt.\n"; break; }

      // read jude version that created this file
    str >> saved_version;
      // save the version to the file descriptor
    if (fi) fi->setSavedVersion(saved_version);

      // get factory id of the object which was saved
    JString factory_id;
    if (!readFromDataStream(factory_id, str, saved_version)) break;
   
    if (jlog::ls.vbLevel(6)) jlog::ls << "Factory ID: " << factory_id << "\n";

      // load notes if version is 2.2.0 or later
    if (saved_version >= VERSION_CODE(2, 2, 0)) {
      JString s;
      str >> s;
      if (fi) fi->setNotesText(s);
    }

       // check the factory id of the object
       // note that non polymorphic objects can have an empty factory id
       // they are not registered in the generic factory but they can be created first then read
    //if (JFactory<T>::instance().id() != factory_id) { jlog::es << "File is not of correct type\n"; break; }
    if (obj.factoryID() != factory_id) { jlog::es << "File '" << fd.path() << "' is not of correct type\n"; break; }
      // read the object from the file using the JDataStream template function
      // since for full generality T could be a basic type
    if (!readFromDataStream(obj, str, saved_version)) break;

      // read magic number again at the end and check
    str >> magic_num;
    if (magic_num != JUDE_MAGICNUM) { jlog::es << "File '" << fd.path() << "' is not the right type or is corrupt.\n"; break; }

      // got to the end so file is read ok
    ret_val = true;
  } 
  }

    // close the file
  f.close();

  return ret_val;
}



/*! \brief Writes \a obj to a file using its write() and factoryID() functions. Returns \c false if any error occurs.

  This is the way all writing of data files should be done in the jude library.
*/

template <typename T>
bool FileUtils::writeFactoryBuiltToFile(const T& obj, const FileDesc& fd, const JString& notes_text) {

    // open the file
  JFile f(fd.path().latin1());
    // check opened ok

  if (!f.open(JIO_WriteOnly)) return false;
  bool ret_val = false;

  switch (fd.isTextFile()) {
  case true : {

      // get a text stream to the file
    JTextStream str(&f);
      // write the object to the file using the JTextStream template function
      // since for full generality T could be a basic type
      // or T must provide bool write(JDataStream&)
    if (!writeToTextStream(obj, str)) break;
       // got to the end so file is probably ok
    ret_val = true;
      // don't do next bit
    break;
  } 
  case false : {

      // get a data stream to the file
    JDataStream str(&f);

      // write magic number and version
    str << (uns32) JUDE_MAGICNUM;
    str << (uns32) JUDE_VERSION;
 
    if (jlog::ls.vbLevel(6)) jlog::ls << "Factory ID: " << obj.factoryID() << "\n";

      // write construction factory string so we can check or instantiate a polymorphic type when reading in
      // writeToDataStream() is in base/jutil.h
    if (!writeToDataStream(obj.factoryID(), str)) break;

      // write notes
    str << notes_text;

      // write the object to the file using the JDataStream template function
      // since for full generality T could be a basic type
      // or T must provide bool write(JDataStream&)
    if (!writeToDataStream(obj, str)) break;

      // write magic number again at the end
    str << (uns32) JUDE_MAGICNUM;

      // got to the end so file is probably ok
    ret_val = true;
  }
  }

    // close the file
  f.close();

  return ret_val;
}



template <typename T>
bool FileUtils::writeTextFile(const T& obj, const FileDesc& fd) {

    // open the file
  JFile f(fd.path().latin1());
    // check opened ok
  if (!f.open(JIO_WriteOnly)) return false;
    // get a text stream from the file
  JTextStream str(&f);
    // write the object to the file
  obj.write(str);
    // close the file
  f.close();

  return true;
}



/*! Reads in any type T using the function readFromDataStream(), which calss teh objects read(JDataStream&) function (I think).
This for objects which are not built by the factory system, i.e. there type is always known before they are read in.
*/
/*
template <typename T>
bool FileUtils::readNonFactoryFromFile(T& obj, const FileDesc& fd) {

    // open the file
  JFile f(fd.path().latin1());
    // check opened ok
  if (!f.open(JIO_ReadOnly)) return false;

  bool ret_val = false;
  uns32 magic_num;
  uns32 saved_version;

  switch (fd.isTextFile()) {
  case true : {

      // get a text stream from the file
    JTextStream str(&f);
      // write the object to the file
    ret_val = obj.read(str);
      // don't do next bit
    break;
  }
  case false : {

      // get a data stream from the file
    JDataStream str(&f);

      // read magic number
    str >> magic_num;
    if (magic_num != JUDE_MAGICNUM) { jlog::es << "File is not the right type or is corrupt.\n"; break; }

      // read jude version that created this file
    str >> saved_version;
      // read the object from the file using the JDataStream template function
      // since for full generality T could be a basic type
    if (!readFromDataStream(obj, str, saved_version)) break;

      // read magic number again at the end and check
    str >> magic_num;
    if (magic_num != JUDE_MAGICNUM) { jlog::es << "File is not the right type or is corrupt.\n"; break; }

      // got to the end so file is read ok
    ret_val = true;
  } 
  }

    // close the file
  f.close();

  return ret_val;
}
*/


} } // namespace jude::file

#endif // _JUDE_FILE_FILEUTILS_H

