
#include "chartlabel2d.h"
#include "chart2d.h"

using namespace jude::chartS2;



void ChartLabel2D::arrangeDrawing(const Chart2D& chart) {

  label_item->setCentrePos(chart.plotAreaCentre());
}

