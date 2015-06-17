
#include "profilechart.h"


// ProfileChart::(constructor)

ProfileChart::ProfileChart(QWidget *p) : 
  BufferWidget(p),
  depth_line(chart) { 

    // default is 5,5,2,2
  chart.setMargins(4,8,4,4);
  title_ti.setText("Depth Profiles");
  chart.setTitleItem(&title_ti);

  yaxis_ti.setText("Depth (m)");
  yaxis_ti.setRotation(270);
  chart.yLeftAxis().setLabelItem(&yaxis_ti);
  chart.yLeftAxis().setNumberGen(&num_gen);
  chart.yLeftAxis().setDisplayNumbers(true);
  chart.yLeftAxis().setValueRange(10, 0);
  chart.yLeftAxis().setTickSubdivision(0.001, 10);
  chart.yRightAxis().setLinkSubdivisions(&chart.yLeftAxis());
 
  //chart.xBottomAxis().setLabelItem(new TextDrawItem2D("Irrad. (Wm-2nm-1)"));
  chart.xBottomAxis().setValueRange(0, 10);
  chart.xBottomAxis().setTickSubdivision(0.00001, 10);
  //chart.xBottomAxis().setNumberGen(new TextChartNumberGen2D);
  chart.xBottomAxis().setNumberGen(&num_gen);
  chart.xBottomAxis().setDisplayNumbers(true);

  chart.xTopAxis().setValueRange(0, 10);
  chart.xTopAxis().setTickSubdivision(0.00001, 10);
  chart.xTopAxis().setNumberGen(&num_gen);
  chart.xTopAxis().setDisplayNumbers(false);

  chart.underLinesGroup().addItem(&depth_line);
}
 


// ProfileChart::sizePolicy()

QSizePolicy ProfileChart::sizePolicy() const {
  return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}



// ProfileChart::resizeEvent()
 
void ProfileChart::resizeEvent(QResizeEvent* e) { 
  // jlog::ls << "WLChart resize: " << width() << " " << height() << "\n"; 
  BufferWidget::resize(width(), height());
  drawToBuffer();
}



void ProfileChart::setCurrentDepth(double d) {
  depth_line.setDepth(d);
}



void ProfileChart::setMaxDepth(double d) {
  chart.yLeftAxis().setValueRange(d, 0);
  chart.yRightAxis().setValueRange(d, 0);
}



void ProfileChart::setBottomAxisIsLog(bool f) {
  if (f) chart.xBottomAxis().setNumberGen(&log_num_gen);
  else chart.xBottomAxis().setNumberGen(&num_gen);
}



void ProfileChart::setTopAxisIsLog(bool f) {
  if (f) chart.xTopAxis().setNumberGen(&log_num_gen);
  else chart.xTopAxis().setNumberGen(&num_gen);
}



// WLChart::setShowKey()

//void WLChart::setShowKey(bool f) {
//  chart.setKey((f) ? key : 0);
//  chart.arrangeDrawing(buffer().width(), buffer().height());
//  chart.drawToBuffer(buffer());
//}



// ProfileChart::drawToBuffer()

void ProfileChart::drawToBuffer() {
  chart.arrangeDrawing(buffer().width(), buffer().height());
  chart.drawToBuffer(buffer());
}

