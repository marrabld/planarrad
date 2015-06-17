
// chart2d.cc

#include "chart2d.h"
#include "base/jlog.h"
#include "drawS2/symbol2d.h"

using namespace jude::chartS2;

// note the layout is x+ left to right and y+ bottom to top


// Chart2D::(constructor)

Chart2D::Chart2D() :
  x_top_axis(ChartAxis2D::XTop),
  x_bottom_axis(ChartAxis2D::XBottom),
  y_left_axis(ChartAxis2D::YLeft),
  y_right_axis(ChartAxis2D::YRight),
  chart_key(0),
  left_margin(5),
  right_margin(5),
  top_margin(2),
  bottom_margin(2),
  title_item(0),
  title_margin(4) { 

  label_list.setAutoDelete(true);
}



// Chart2D::setBackgroundColour()

void Chart2D::setBackgroundColour(const JColour& col) { 
  chart_paper.setBackgroundColour(col); 
}



// Chart2D::setMargins()

void Chart2D::setMargins(int left_m, int right_m, int top_m, int bottom_m) {
  left_margin = left_m;
  right_margin = right_m;
  top_margin = top_m;
  bottom_margin = bottom_m;
}



void Chart2D::setLabelMargin(int t) {
  x_top_axis.setLabelMargin(t);
  x_bottom_axis.setLabelMargin(t);
  y_left_axis.setLabelMargin(t);
  y_right_axis.setLabelMargin(t);
}



void Chart2D::setNumbersMargin(int t) {
  x_top_axis.setNumbersMargin(t);
  x_bottom_axis.setNumbersMargin(t);
  y_left_axis.setNumbersMargin(t);
  y_right_axis.setNumbersMargin(t);
}



void Chart2D::setTickLength(int t) {
  x_top_axis.setTickLength(t);
  x_bottom_axis.setTickLength(t);
  y_left_axis.setTickLength(t);
  y_right_axis.setTickLength(t);
}



void Chart2D::setTitleItem(DrawItem2D* ti) {
  title_item = ti;
  chart_paper.addItem(title_item);
}



void Chart2D::setTitleMargin(int m) {
  title_margin = m;
}

 

// Chart2D::arrangeDrawing()

void Chart2D::arrangeDrawing(int xs, int ys) {

  if (xs<=0 || ys<=0) return;

    // clear any existing items
  //chart_paper.deleteAllItems();
  //data0_group.deleteAllItems();
  //data1_group.deleteAllItems();

    // the y-size of teh title and margin or zero if none
  int title_ys = (int) ((title_item) ? (title_item->boundingBox().ySize() + title_margin) : 0);

  //jlog::ls << "title_ys: " << title_ys << " xbottom: " << x_bottom_axis.crossSize() << "\n";

    // how much y-size there is for the plot
    // this is easier to know first since the height of numbers is generally uniform
  plot_size.setY(ys - (x_top_axis.crossSize() + x_bottom_axis.crossSize() + top_margin + bottom_margin + title_ys));

    // now we know how much y-space there is try to get sensible subdivisions on the y axes
  if (!y_left_axis.linkSubdivisions()) {
    y_left_axis.calcSubAxisIntervalsY((int)plot_size.y());
    y_right_axis.calcSubAxisIntervalsY((int)plot_size.y());
  } else {
    y_right_axis.calcSubAxisIntervalsY((int)plot_size.y());
    y_left_axis.calcSubAxisIntervalsY((int)plot_size.y());
  }

    // now the y-axis numbers are fixed so can get how much x-space there is
  plot_size.setX(xs - (y_left_axis.crossSize() + y_right_axis.crossSize() + left_margin + right_margin));

    // now we know how much x-space can do the x-axis subdivisions, which are most tricky
  if (!x_top_axis.linkSubdivisions()) {
    x_top_axis.calcSubAxisIntervalsX((int)plot_size.x());
    x_bottom_axis.calcSubAxisIntervalsX((int)plot_size.x());
  } else {
    x_bottom_axis.calcSubAxisIntervalsX((int)plot_size.x());
    x_top_axis.calcSubAxisIntervalsX((int)plot_size.x());
  }

    // the bottom left corner of the plot in pixels
  plot_origin = Vec2D(left_margin + y_left_axis.crossSize(), bottom_margin + x_bottom_axis.crossSize());

  //jlog::ls << "plot_size x " << plot_size.x()  << " y " << plot_size.y() << "\n";
  //jlog::ls << "plot_origin x " << plot_origin.y()  << " y " << plot_origin.y() << "\n";

  y_left_axis.setBoundingBox(Box2D(Vec2D(left_margin, bottom_margin+x_bottom_axis.crossSize()-1), 
				   Vec2D(left_margin+y_left_axis.crossSize(), ys-top_margin-title_ys-x_top_axis.crossSize()+1)));

  y_right_axis.setBoundingBox(Box2D(Vec2D(xs-right_margin-y_right_axis.crossSize(), bottom_margin+x_bottom_axis.crossSize()-1), 
				    Vec2D(xs-right_margin, ys-top_margin-title_ys-x_top_axis.crossSize()+1)));

  x_bottom_axis.setBoundingBox(Box2D(Vec2D(left_margin+y_left_axis.crossSize()-1, bottom_margin), 
				     Vec2D(xs-right_margin-y_right_axis.crossSize()+1, bottom_margin+x_bottom_axis.crossSize())));

  x_top_axis.setBoundingBox(Box2D(Vec2D(left_margin+y_left_axis.crossSize()-1, ys-top_margin-title_ys-x_top_axis.crossSize()), 
				  Vec2D(xs-right_margin-y_right_axis.crossSize()+1, ys-top_margin-title_ys)));

  chart_grid.setAxes(x_bottom_axis, y_left_axis);

  if (title_item) title_item->setCentrePos(Vec2D(left_margin+y_left_axis.crossSize()-1+plot_size.x()/2, ys-top_margin-title_ys/2));

  prepareData(data00_list, data00_group, x_bottom_axis, y_left_axis);
  prepareData(data01_list, data01_group, x_bottom_axis, y_right_axis);
  prepareData(data10_list, data10_group, x_top_axis, y_left_axis);
  prepareData(data11_list, data11_group, x_top_axis, y_right_axis);

  JListIterator<ChartLabel2D> label_iter(label_list);
  ChartLabel2D* label = label_iter.first();
  while (label) { 
    label->arrangeDrawing(*this);
    label = label_iter.next();
  }
  //jlog::ls << " HAS KEY " << ((chart_key) ? " yes\n" : "no\n");
  if (chart_key) chart_key->arrangeDrawing(*this);
}



// Chart2D::prepareData() [private]

void Chart2D::prepareData(JList<ChartLineData2D>& data_list, DrawGroup2D& data_group, const ChartAxis2D& x_axis, const ChartAxis2D& y_axis) {

  data_group.deleteAllItems();

  JListIterator<ChartLineData2D> iter(data_list);
  ChartLineData2D* cld = iter.first();
  while (cld) {
    Vec2D prev_v;
    for (unsigned int i=0; i<cld->xData().size(); i++) {
      Vec2D v(x_axis.valueToPixelPosition(cld->xData()(i)), y_axis.valueToPixelPosition(cld->yData()(i)));
      if (cld->displayAsLine()) { if (i != 0) data_group.addItem(new Line2D(v, prev_v, cld->colour())); }
      else data_group.addItem(new Symbol2D(Symbol2D::FilledCircle, v, cld->colour(), 3, 1));
      prev_v = v;
    }
    if (cld->highlightedPoint() >= 0 && cld->highlightedPoint()< (int)(cld->xData().size())) {
      Vec2D v(x_axis.valueToPixelPosition(cld->xData()(cld->highlightedPoint())), y_axis.valueToPixelPosition(cld->yData()(cld->highlightedPoint())));
      data_group.addItem(new Symbol2D(Symbol2D::FilledCircle, v, cld->colour(), 10, 1));
    }

    cld = iter.next();
  }
}



// Chart2D::drawToBuffer()

void Chart2D::drawToBuffer(JBuffer& buff) const {

  if (buff.width()==0 || buff.height()==0) return;

  Trans2D paper_trans;
  paper_trans.setYOffset(buff.height()-1);
  paper_trans.setYScale(-1);

    // draw background
  chart_paper.draw(paper_trans, buff);
 
    // draw grid
  chart_grid.topDrawGroup().draw(paper_trans, buff);

    // draw axes
  x_bottom_axis.topDrawGroup().draw(paper_trans, buff);
  x_top_axis.topDrawGroup().draw(paper_trans, buff);
  y_left_axis.topDrawGroup().draw(paper_trans, buff);
  y_right_axis.topDrawGroup().draw(paper_trans, buff);

    // draw under line extras
  under_lines_group.draw(paper_trans, buff);

    // draw axes set data
  data00_group.draw(paper_trans, buff);
  data01_group.draw(paper_trans, buff);
  data10_group.draw(paper_trans, buff);
  data11_group.draw(paper_trans, buff);

  JListIterator<ChartLabel2D> label_iter(label_list);
  ChartLabel2D* label = label_iter.first();
  while (label) { 
    label->drawItem().draw(paper_trans, buff);;
    label = label_iter.next();
  }

   // draw over line extras
  over_lines_group.draw(paper_trans, buff);

  if (chart_key) chart_key->topDrawGroup().draw(paper_trans, buff);
}

