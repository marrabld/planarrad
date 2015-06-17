
// specplot.h

#ifndef _JUDE_FE_SPECPLOT_H
#define _JUDE_FE_SPECPLOT_H

#include "rad/sbanddata.h"
#include "chart/chart2d.h"
#include "chart/chartview2d.h"

template <typename SD> class RadDirecImp;

namespace jude {
namespace fe {



class SpecPlot : public ChartView2D {

  Chart2D chart;
 
  const RadDirecImp<SBandData>* spec;

public:

  SpecPlot(QWidget *p=0);
 
  QSizePolicy sizePolicy() const;

  void clear();

  void addSpectralData(const SBandData& sd, const JColour& col);

  void addSpec(const RadDirecImp<SBandData>* s, const JColour& col);

  void setTitle(bool is_down, double depth);

  void setYAxisAnchor(bool f, double max_value);
};



} } // namespace jude::fe

#endif // _JUDE_FE_SPECPLOT_H

