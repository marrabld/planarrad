
// specpanel.h

#ifndef _JUDE_FE_SPECPANEL_H
#define _JUDE_FE_SPECPANEL_H

#include <qwidget.h>
#include "rad/sbanddata.h"
#include "fe/widgetstack.h"
#include "fe/basepanel.h"

namespace jude { namespace rad {
class BandSpec;
template <typename SD> class RadDirecImp;
} }

using namespace jude::rad;

class QSlider;

namespace jude {
namespace fe {

class HSDView;
class SpecView;
class SpecPlot;
class SpecPanelBar;



class SpecPanel : public BasePanel {
 
  Q_OBJECT
 
  QWidgetStack* display_stk;

  HSDView* spec_proj;
  SpecView* spec_view;
  SpecPlot* spec_plot;
 
  SpecPanelBar* bar;
  QSlider* sensitivity_slider;

  bool auto_set_sens;

  const RadDirecImp<SBandData>* upward_spec;
  const RadDirecImp<SBandData>* downward_spec;

  double title_depth;

  double max_Eu;
  double max_Ed;
  double max_Lu;
  double max_Ld;

  //void updateBufferImage();

public:

  SpecPanel(QWidget* parent=0);

  void setBandSpec(BandSpec* bs);
 
  void setProjSquare(int sz);
 
  void setUpwardSpec(const RadDirecImp<SBandData>* s, double max_v);
  void setDownwardSpec(const RadDirecImp<SBandData>* s, double max_v);
  void setTitleDepth(double d);

  void clear();
  void addAdditionalData(const SBandData& sd);

  void rebuild();

  void autoSetSensitivity();

public slots:

  void buttonsChanged();
  void sensitivityChanged(int v);
};



} } // namespace jude::fe

#endif // _JUDE_FE_SPECPANEL_H

