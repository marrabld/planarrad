
// getimage.cc

#include "fe/getimage.h"
#include "dynamic/images.h"
#include <qimage.h>


// getImage()
// returns an image which has been included from a jembed header file

const QImage& getImage(const char* name) {
  return qembed_findImage(name);
}

