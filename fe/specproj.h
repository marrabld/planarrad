
// specproj.h

#ifndef _JUDE_FE_SPECPROJ_H
#define _JUDE_FE_SPECPROJ_H

#include "fe/bufferwidget.h"
#include "rad/hemispheredraw.h"

using namespace jude::rad;

namespace jude {
namespace fe {



class SpecProj : public BufferWidget {

  HemiSphereDraw hsd;
 
public:

  SpecProj(QWidget *p=0);
  ~SpecProj();
 
  QSizePolicy sizePolicy() const;
 
  void setBandSpec(BandSpec* bs) { hsd.setBandSpec(bs); }
   
  void clear();

  void setSourceData(const RadDirecImp<SBandData>* s);
  double autoSetSensK();
  void setSensK(double s);

  void drawToBuffer();

protected:

  void resizeEvent(QResizeEvent*);
};



} } // namsepace jude::fe

#endif // _JUDE_FE_SPECPROJ_H

