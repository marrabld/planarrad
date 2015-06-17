
// profiledepthline.h

#ifndef _PLANARRAD_PROFILEDEPTHLINE_H
#define _PLANARRAD_PROFILEDEPTHLINE_H

#include "drawS2/drawgroup2d.h"
#include "drawS2/line2d.h"
#include "chartS2/chart2d.h"

using namespace jude::drawS2;
using namespace jude::chartS2;



class ProfileDepthLine : public DrawGroup2D {

  const Chart2D& chart;

  double curr_depth;
  
public:

  ProfileDepthLine(const Chart2D& chart);

  void setDepth(double d);

  void draw(const Trans2D& trans, JBuffer& buff) const;
};



#endif // _PLANARRAD_PROFILEDEPTHLINE_H

