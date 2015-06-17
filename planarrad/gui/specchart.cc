
#include "specchart.h"
#include "fe/textdrawitem2d.h"


// SpecChart::(constructor)

SpecChart::SpecChart(QWidget *p) : 
  BufferWidget(p) { 

    // default is 5,5,2,2
  chart.setMargins(4,8,4,4);  
  title_ti.setText("Spectra Depth 0.0 m");
  chart.setTitleItem(&title_ti);

  //num_gen.setBackgroundColour(JColour(0));

  ti = new TextDrawItem2D("Irrad. (Wm-2nm-1)");
  ti->setRotation(270);
  chart.yLeftAxis().setLabelItem(ti);
  chart.yLeftAxis().setNumberGen(&num_gen);
  chart.yLeftAxis().setDisplayNumbers(true);
  chart.yLeftAxis().setValueRange(0, 10);
  chart.yLeftAxis().setTickSubdivision(0.00001, 10);

  chart.yRightAxis().setValueRange(0, 10);
  chart.yRightAxis().setTickSubdivision(0.00001, 10);
  chart.yRightAxis().setNumberGen(&num_gen);
  chart.yRightAxis().setDisplayNumbers(false);
  //chart.yRightAxis().setLinkSubdivisions(&chart.yLeftAxis());

  //chart.xBottomAxis().setLabelItem(new TextDrawItem2D("Irrad. (Wm-2nm-1)"));
  chart.xBottomAxis().setValueRange(400, 740);
  chart.xBottomAxis().setTickSubdivision(0.001, 10);
  //chart.xBottomAxis().setNumberGen(new TextChartNumberGen2D);
  chart.xBottomAxis().setNumberGen(&num_gen);
  chart.xBottomAxis().setDisplayNumbers(true);
  chart.xTopAxis().setValueRange(400, 740);
  chart.xTopAxis().setLinkSubdivisions(&chart.xBottomAxis());

  //chart.underLinesGroup().addItem(&depth_line);
}
 


SpecChart::~SpecChart() {
  delete ti; 
}



// SpecChart::sizePolicy()

QSizePolicy SpecChart::sizePolicy() const {
  return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}



// SpecChart::resizeEvent()
 
void SpecChart::resizeEvent(QResizeEvent* e) { 
  // jlog::ls << "WLChart resize: " << width() << " " << height() << "\n"; 
  BufferWidget::resize(width(), height());
  drawToBuffer();
}



void SpecChart::setCurrentDepth(double d) {
  JString s = JString("Spectra Depth ") + JString::number(d,'f',1) + " m";
  title_ti.setText(s);
  //depth_line.setDepth(d);
}



void SpecChart::setRightAxisIsLog(bool f) {
  if (f) chart.yRightAxis().setNumberGen(&log_num_gen);
  else chart.yRightAxis().setNumberGen(&num_gen);
}



void SpecChart::setLeftAxisIsLog(bool f) {
  if (f) chart.yLeftAxis().setNumberGen(&log_num_gen);
  else chart.yLeftAxis().setNumberGen(&num_gen);
}



// WLChart::setShowKey()

//void WLChart::setShowKey(bool f) {
//  chart.setKey((f) ? key : 0);
//  chart.arrangeDrawing(buffer().width(), buffer().height());
//  chart.drawToBuffer(buffer());
//}



// SpecChart::drawToBuffer()

void SpecChart::drawToBuffer() {
  chart.arrangeDrawing(buffer().width(), buffer().height());
  chart.drawToBuffer(buffer());
}

