
// specview.h

#ifndef _JUDE_FE_SPECVIEW_H
#define _JUDE_FE_SPECVIEW_H

#include <qwidget.h>
//#include "direcdrawing.h"
//#include "jsignal.h"
//#include "sensslider.h"

namepsace jude { namespace S3 { class View3D; } }

template <typename SD> class RadDirecImp;

namespace jude {
namespace fe {

class QuadView;
class QuadSelectView;



class SpecView : public QWidget {

  //static void slotTranslated(void* object, View3D* dv) { ((SpecView*)object)->actionTranslated(dv); }
  //static void slotIndexSelected(void* object, int i) { ((BRDFView*)object)->actionIndexSelected(i); }

  QuadView* quad_view;

public:

  SpecView(QWidget *p=0);
 
  QSizePolicy sizePolicy() const;

  void clear();
  void setSpec(const RadDirecImp<SBandData>* s);
 
  void setDrawShape(bool f);
  void setSurfaceContext(bool f);

  void actionTranslated(View3D* dv);
  //void actionIndexSelected(int i);

  void exportPostscript(const JString& filepath);
};



} } // namespace jude::fe

#endif // _JUDE_FE_SPECVIEW_H

