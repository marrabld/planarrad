
// jdir.cc

#include "base/jdir.h"
#include "base/jstring.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

using namespace jude::base;

#ifndef WIN32
static int mkdir_cstd(const char *pathname, mode_t mode) { return mkdir(pathname, mode); }
#else
static int mkdir_cstd(const char *pathname, mode_t mode) { return mkdir(pathname); }
#endif


/*! \brief Returns \c true if the directory exists. 

Note this will actually return false if any error condition occured, including system 
level errors such as too many file descriptors being in use, etc. 
*/

bool JDir::exists() const {

  DIR* d = opendir(dir_path.latin1());
  closedir(d);

  return (d != 0);
}



/*! \brief Creates the directory on the file system. Returns \c true if went ok \c false if not. */

bool JDir::mkdir() const {
  return (mkdir_cstd(dir_path.latin1(), 0777) == 0);
}



/*! \brief Returns a list of the names of the entries within the directory. 

If the directory does not exist no error is reported but the returned list is empty.
Callers should use exists() to check the existance of the directory beforehand.
*/

void JDir::getEntryList(JList<JString>& entry_list) const {

  entry_list.clear();

  DIR* d = opendir(dir_path.latin1());
  if (d) {

    struct dirent* de;

    while ((de = readdir(d))) {
      if (strcmp(de->d_name, ".") == 0) continue;
      if (strcmp(de->d_name, "..") == 0) continue;
      entry_list.append(new JString(de->d_name));
    }
  }

  closedir(d);
}



/*! \fn JDir(const JString& p)
\brief Create a directory object with directory path \p. 

Note this does not physically create a directory on the file system and the specified directory may not exist.
Use exists() to verify that it does.
*/



/*! const JString& path() const
\brief Returns the directory path. */

