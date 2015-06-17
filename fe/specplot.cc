
// aqrad
// specplot.cc
// (c) John Hedley 2003

#include "config.h"
#include "specplot.h"
#include "jvboxlayout.h"
#include "linechartitem2d.h"
#include "specimp.h"
#include <stdio.h>

//JFont main_font("Sans",9,JFont::Normal,false);


// SpecPlot::(constructor)

SpecPlot::SpecPlot(JWidget *p) : 
  ChartView2D(chart, p),
  spec(0) {

  setFrame(1);

  chart.paper().title().setText("Planar Irradiance");
  chart.paper().setTitleMargin(4);
  chart.paper().xAxisLabel().setText("Wavelength (nm)");
  chart.paper().yAxisLabel().setText("Irradiance (Wm-2nm-1)");
  chart.paper().setYAxisMin(0);
  chart.paper().setXNumbersMargin(6);
  chart.paper().setYNumbersMargin(8);
  chart.paper().setXAxisNumberFormat("%.0f");
  chart.paper().setYAxisNumberFormat("%.2f");
 
  chart.paper().paperGrid().setVisible(true);
  chart.paper().border().setColour(JColour(192,192,192));
  //void setBorderMargin(int l, int r, int t, int b);
 
  //chart_view->setFixedSize(sz,sz);
  //chart.paper().title().setFont(main_font);
  //chart.paper().xAxisLabel().setFont(Gui::font());
  //chart.paper().yAxisLabel().setFont(Gui::font());
}



// SpecPlot::sizePolicy()

JSizePolicy SpecPlot::sizePolicy() const {
  return JSizePolicy(JSizePolicy::Expanding, JSizePolicy::Expanding);
}


// SpecPanel::clearAdditionalData()

//void SpecPlot::clearAdditionalData() {
  // spec_plot->clearAdditionalData();
//}



// SpecPanel::addAdditionalData()

//void SpecPlot::addAdditionalData(const SpectralData& sd) {
//  spec_plot->addAdditionalData(sd);
//}


void SpecPlot::clear() {

  chart.paper().removeAllItems();
}


// SpecPlot::setSpec()

void SpecPlot::addSpec(const SpecImp<SpectralData>* s, const JColour& col) {

  spec = s;

  if (!spec) return;

  SpectralData sd(spec->sumBySolidAngleAbsCosTheta());

  addSpectralData(sd, col);
}



// SpecPlot::addSpectralData()

void SpecPlot::addSpectralData(const SpectralData& sd, const JColour& col) {

  Vector xd(SpectralData::bands());
  Vector yd(SpectralData::bands());

  for (int i=0; i<SpectralData::bands(); i++) {
    xd.put(i, SpectralData::bandCentres()[i]);
    yd.put(i, sd[i]);
  }

  LineChartItem2D* item = new LineChartItem2D(xd, yd);
  item->setLineThickness(2);
  item->setLineColour(col);
  chart.paper().addItem(*item);

  chart.paper().setYAxisPosition(SpectralData::bandCentres()[0]);
}



// SpecPlot::setTitle()

void SpecPlot::setTitle(bool is_down, double depth) {
  JString s = (is_down) ? "Downwelling Irradiance " : "Upwelling Irradiance ";
  if (depth < 0) s.append("(air)");
  else s.append(JString("(") + JString::number(depth, 'f', 1) + " m)"); 
  chart.paper().title().setText(s);
}



// SpecPlot::setYAxisAnchor()

void SpecPlot::setYAxisAnchor(bool f, double max_value) {
  if (!f) chart.paper().setYAxisAutoScale();
  else chart.paper().setYAxisMax(max_value);
  chart.paper().setYAxisMin(0);
}

