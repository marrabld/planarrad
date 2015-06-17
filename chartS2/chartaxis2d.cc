
// chartaxis2d.cc

#include "chartaxis2d.h"
#include "base/jlog.h"

using namespace jude::base;
using namespace jude::chartS2;



ChartAxis2D::ChartAxis2D(axis_type ax_type) :
  is_defined(false),
  ax_type(ax_type),
  label_item(0),
  number_item_gen(new ChartNumberGen2D),
  line_item(JColour::Black),
  min_value(0),
  max_value(10),
  reverse_axis(false),
  is_log(false),
  display_numbers(false),
  numbers_cross_size(0),
  sub(5),
  sub_start(0),
  sub_end(10),
  min_sub(2),
  max_sub_count(6),
  tick_location_count(0),
  link_subdiv(0),
  //  min_fixed(true),
  // max_fixed(true),
  label_margin(1),
  numbers_margin(1),
  tick_length(2) { 

  top_draw_group.addItem(&line_item);
  top_draw_group.addItem(&numbers_group);
  top_draw_group.addItem(&ticks_group);
}



ChartAxis2D::~ChartAxis2D() {
  delete number_item_gen;
}



void ChartAxis2D::setNumberGen(ChartNumberGen2D* ng) {
  //delete number_item_gen;
  number_item_gen = ng;
  if (isXAxis() && number_item_gen) numbers_cross_size = number_item_gen->heightHint();
}



// ChartAxis2D::setLabelItem()

void ChartAxis2D::setLabelItem(DrawItem2D* li) { 
  top_draw_group.removeItem(label_item); 
  label_item = li;
  top_draw_group.addItem(label_item); 
}



// ChartAxis2D::crossSize()

int ChartAxis2D::crossSize() const {

    // line width in pixels
  int s = 1;
    // if there is a label object add the dimension of that on
  if (label_item) s += label_margin + (int) ((isXAxis()) ? label_item->boundingBox().ySize() : label_item->boundingBox().xSize());
    // if displaying numbers then add on current best estimate of the numbers size
  if (display_numbers) s += numbers_cross_size + numbers_margin;
    // any tick
  s += tick_length;

  return s;
}



// ChartAxis2D::setBoundingBox()

void ChartAxis2D::setBoundingBox(const Box2D& bbox) {

    // copy the box - now can work out where to put things
    // the max values are non-inclusive
  bounding_box = bbox;

  Vec2D label_centre;
  int tick_lo;
  int tick_hi;

  switch (ax_type) {
    
  case XBottom : {

    if (label_item) label_centre = Vec2D(bbox.xCentre(), bbox.yMin() + label_item->boundingBox().ySize()/2 - 1);
    line_pos0 = Vec2D(bbox.xMin(), bbox.yMax()-1);
    line_pos1 = Vec2D(bbox.xMax()-1, bbox.yMax()-1);
    tick_lo = (int)bbox.yMax()-1-tick_length;
    tick_hi = (int)bbox.yMax()-1;
    break;
  }

  case XTop : {

    if (label_item) label_centre = Vec2D(bbox.xCentre(), bbox.yMax() - label_item->boundingBox().ySize()/2 - 1);
    line_pos0 = Vec2D(bbox.xMin(), bbox.yMin());
    line_pos1 = Vec2D(bbox.xMax()-1, bbox.yMin());
    tick_lo = (int)bbox.yMin()+tick_length;
    tick_hi = (int)bbox.yMin()+1;
    break;
  }

  case YLeft : {

    if (label_item) label_centre = Vec2D(bbox.xMin() + label_item->boundingBox().xSize()/2, bbox.yCentre());    
    line_pos0 = Vec2D(bbox.xMax()-1, bbox.yMin());
    line_pos1 = Vec2D(bbox.xMax()-1, bbox.yMax()-1);
    tick_lo = (int)bbox.xMax()-1-tick_length;
    tick_hi = (int)bbox.xMax()-1;
    break;
  }

  default : { // YRight

    if (label_item) label_centre = Vec2D(bbox.xMax() - label_item->boundingBox().xSize()/2, bbox.yCentre());    
    line_pos0 = Vec2D(bbox.xMin(), bbox.yMin());
    line_pos1 = Vec2D(bbox.xMin(), bbox.yMax()-1);
    tick_lo = (int)bbox.xMin()+1;
    tick_hi = (int)bbox.xMin()+1+tick_length;
  } 

  }

  if (label_item) label_item->setCentrePos(label_centre);
  line_item.setEndPositions(line_pos0, line_pos1);

  DrawItem2D* number_item = numbers_group.itemList().first();
  Line2D* tick_item = dynamic_cast<Line2D*>(ticks_group.itemList().first());
  double curr_val = sub_start;
    // for all numbers or ticks
  while (number_item || tick_item) {

      // the position on the axis pixel-wise
    int pixel_val = valueToPixelPosition(curr_val);

    if (number_item) {
        // half the width to edge align the numbers on a y-axis
      int w = (int)(number_item->boundingBox().xSize()/2);
      int h = (int)(number_item->boundingBox().ySize()/2);
        // position the number
      switch (ax_type) {
        case YLeft : number_item->setCentrePos(Vec2D(bbox.xMax()-1-tick_length-numbers_margin-w, pixel_val-1)); break;
        case YRight : number_item->setCentrePos(Vec2D(bbox.xMin()+1+tick_length+numbers_margin+number_item->boundingBox().xSize()-w, pixel_val-1)); break;
        case XBottom : number_item->setCentrePos(Vec2D(pixel_val+1, bbox.yMax()-1-tick_length-numbers_margin-number_item->boundingBox().ySize()+h-1)); break;
        case XTop : number_item->setCentrePos(Vec2D(pixel_val+1, bbox.yMin()+1+tick_length+numbers_margin+h-1)); break;
      }
    }
  
    if (tick_item) {
      if (isYAxis()) tick_item->setEndPositions(Vec2D(tick_lo, pixel_val), Vec2D(tick_hi, pixel_val));
      else tick_item->setEndPositions(Vec2D(pixel_val, tick_lo), Vec2D(pixel_val, tick_hi));
    }

      // next items and subdivision
    number_item = numbers_group.itemList().next();
    tick_item = dynamic_cast<Line2D*>(ticks_group.itemList().next());
    if (!reverse_axis) curr_val += sub;
    else curr_val -= sub;
  }
}



int ChartAxis2D::valueToPixelPosition(double v) const {

  if (isXAxis()) return (int) floor((bounding_box.xMin() + ((v - min_value) * (bounding_box.xSize()-1)) / (max_value - min_value)) + 0.5);
  else return (int) floor((bounding_box.yMin() + ((v - min_value) * (bounding_box.ySize()-1)) / (max_value - min_value)) + 0.5);
}



// ChartAxis2D::calcSubAxesIntervalsY() [private]
// calculates what axis label intervals it is appropriate to draw

void ChartAxis2D::calcSubAxisIntervalsY(int pixel_length) {

    // the real axis value range
  double value_range = fabs(max_value - min_value);
    // factor to turn real axis values into pixels, use -1 because we wants the ends to be inclusive
  double scale_factor = (pixel_length - 1) / value_range; 

    // get sub from linked axis assumed already calculated
  if (link_subdiv) sub = link_subdiv->sub;
  else {

    int h = number_item_gen->heightHint()*3;
    sub = 1000000;
  
    while (sub*scale_factor>h && sub>min_sub && value_range/sub<max_sub_count) { 
      sub = sub/2;
      if (sub*scale_factor <= h || sub<=min_sub) break; 
      sub = sub/2.5; 
      if (sub*scale_factor <= h || sub<=min_sub) break; 
      sub = sub/2; 
    }
  }

    // get position of first visible y sub axis line 
  int start_index = (int) floor(min_value/sub);
  sub_start = start_index * sub;
  if (!reverse_axis) { if (sub_start < min_value) { sub_start += sub; start_index++; } }
  else { if (sub_start > min_value) { sub_start += sub; start_index++; } }
    // get position of last visible y sub axis line 
  int end_index = (int) ceil(max_value/sub);
  sub_end = end_index * sub;
  if (!reverse_axis) { if (sub_end > max_value) { sub_end -= sub; end_index--; } }
  else { if (sub_end < max_value) { sub_end -= sub; end_index--; } }
 
  //jlog::ls << "rv " << reverse_axis << " si " << start_index << " ei " << end_index << " ss " << sub_start << " se " << sub_end << " max " << max_value << " sub " << sub << "\n";
 
  if (!user_axis_number_format.isEmpty()) axis_number_format = user_axis_number_format;
  else {
    if (sub < 0.001) axis_number_format = "%.4f";
    else if (sub < 0.01) axis_number_format = "%.3f";
    else if (sub < 0.1) axis_number_format = "%.2f";
    else if (sub < 1) axis_number_format = "%.1f";
    else axis_number_format = "%.0f";
  }

  //jlog::ls << "sub " << sub << " sub_start " << sub_start << "\n";

  numbers_group.deleteAllItems();
  ticks_group.deleteAllItems();
  
  numbers_cross_size = 0;

    // count of the number of tick positions regardless of whether ticks are drawn - used for grid
  if (!reverse_axis) tick_location_count = end_index - start_index + 1;
  else tick_location_count = start_index - end_index + 1;

  for (int i=0; i<tick_location_count; i++) {
  
    //double t = sub_start + i * sub;
    double t = tickLocation(i);
  
    if (display_numbers) { 
      DrawItem2D* item = number_item_gen->createItem(getAxisLabelString(t));
      int cross_sz = (int) ((isYAxis()) ? item->boundingBox().xSize() : item->boundingBox().ySize());
      if (cross_sz > numbers_cross_size) numbers_cross_size = cross_sz;
      numbers_group.addItem(item);
    }
    if (tick_length > 0) {
      ticks_group.addItem(new Line2D(Vec2D(0,0), Vec2D(0,tick_length), JColour::Black));
    }
  }

  //jlog::ls << "numbers_cross_size " << numbers_cross_size << "\n";
}




// ChartAxis2D::calcSubAxesIntervals() [private]
// calculates what axis label intervals it is appropriate to draw

void ChartAxis2D::calcSubAxisIntervalsX(int pixel_length) {

    // text strings at start and end
  JString s0(JString::number(min_value,'g',3));
  JString s1(JString::number(max_value,'g',3));
    // how wide the labels will be
  int tw1 = number_item_gen->width(s0);
  int tw2 = number_item_gen->width(s1);
    // make tw1 the larger of the two
  if (tw2>tw1) tw1 = tw2;
    // three times the maximum width in pixels
  tw1 = tw1*3;

    // the real axis value range
  double value_range = fabs(max_value - min_value);
    // factor to turn real axis values into pixels, use -1 because we wants the ends to be inclusive
  double scale_factor = (pixel_length - 1) / value_range; 

    // get sub from linked axis assumed already calculated
  if (link_subdiv) sub = link_subdiv->sub;
  else {

      // start subdivision
    sub = 1000000;
      // keep subdividing while too large, not yet at minimum size or limit of number
    while (sub*scale_factor>tw1 && sub>min_sub && value_range/sub<max_sub_count) { 
      sub = sub/2;
      if (sub*scale_factor <= tw1 || sub<=min_sub) break; 
      sub = sub/2.5; 
      if (sub*scale_factor <= tw1 || sub<=min_sub) break; 
      sub = sub/2; 
    }
  }

    // get position of first visible x sub axis line 
  int start_index = (int) floor(min_value / sub);
  sub_start = start_index * sub;
  if (sub_start < min_value) { sub_start += sub; start_index++; }
    // get position of first visible x sub axis line 
  int end_index = (int) ceil(max_value / sub);
  sub_end = end_index * sub;
  if (sub_end > max_value) { sub_end -= sub; end_index--; }
 
  //jlog::ls << "si " << start_index << " ei " << end_index << " se " << sub_end << " max " << max_value << " sub " << sub << "\n";
 
  if (!user_axis_number_format.isEmpty()) axis_number_format = user_axis_number_format;
  else {
    if (sub < 0.001) axis_number_format = "%.4f";
    else if (sub < 0.01) axis_number_format = "%.3f";
    else if (sub < 0.1) axis_number_format = "%.2f";
    else if (sub < 1) axis_number_format = "%.1f";
    else axis_number_format = "%.0f";
  }

  //jlog::ls << "sub " << sub << " sub_start " << sub_start << "\n";

  numbers_group.deleteAllItems();
  ticks_group.deleteAllItems();
  
  numbers_cross_size = 0;

    // count of the number of tick positions regardless of whether ticks are drawn - used for grid
  tick_location_count = end_index - start_index + 1;

  for (int i=0; i<tick_location_count; i++) {
  
    double t = sub_start + i * sub;

    if (display_numbers) { 
      DrawItem2D* item = number_item_gen->createItem(getAxisLabelString(t));
      int cross_sz = (int) ((isYAxis()) ? item->boundingBox().xSize() : item->boundingBox().ySize());
      if (cross_sz > numbers_cross_size) numbers_cross_size = cross_sz;
      numbers_group.addItem(item);
    }
    if (tick_length > 0) {
      ticks_group.addItem(new Line2D(Vec2D(0,0), Vec2D(tick_length,0), JColour::Black));
    }
  }

  //jlog::ls << "numbers_cross_size " << numbers_cross_size << "\n";
}





// ChartAxisSpec::getAxisLabelString()

JString ChartAxis2D::getAxisLabelString(double v) const {
  char buf[20];
  snprintf(buf, 20, axis_number_format.latin1(), v);
  return JString(buf);
}

