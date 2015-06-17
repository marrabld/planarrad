
// hsdview.h

#ifndef _JUDE_FE_HSDVIEW_H
#define _JUDE_FE_HSDVIEW_H

#include "fe/bufferwidget.h"
#include "rad/hemispheredraw.h"

using namespace jude::rad;

namespace jude {
namespace fe {



class HSDView : public BufferWidget {

  HemiSphereDraw hsd;
 
public:

  HSDView(QWidget *p=0);
  ~HSDView();
 
  QSizePolicy sizePolicy() const;

  void setBandSpec(BandSpec* bs) { hsd.setBandSpec(bs); }

  void setViewZPlusHemisphere(bool f) { hsd.setViewZPlusHemisphere(f); }

  void clear();
  void setSourceData(const RadDirecImp<SBandData>* s);

  double autoSetSensK();
  double maxSensK() const { return hsd.maxSensK(); }
  void setRelativeSensK(double s);

  void drawToBuffer();

protected:

  void resizeEvent(QResizeEvent*);
};



} } // namsepace jude::fe

#endif // _JUDE_FE_HSDView_H

