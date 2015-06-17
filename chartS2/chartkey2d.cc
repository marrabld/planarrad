
#include "chartkey2d.h"
#include "chart2d.h"
#include "base/jlog.h"

using namespace jude::chartS2;



ChartKey2D::ChartKey2D() :
  left_margin(5),
  right_margin(5),
  top_margin(2),
  bottom_margin(2),
  line_length(25),
  line_item_spacing(4),
  item_x_spacing(2),
  item_y_spacing(2) { 

  item_list.setAutoDelete(true); 

  back_rect = new Rectangle2D(Box2D(Vec2D(0,0), Vec2D(0,0)), JColour::White);
  top_draw_group.addItem(back_rect);
 
  line_top = new Line2D(Vec2D(0,0), Vec2D(0,0), JColour::Black);
  line_left = new Line2D(Vec2D(0,0), Vec2D(0,0), JColour::Black);
  line_bottom = new Line2D(Vec2D(0,0), Vec2D(0,0), JColour::Black);
  line_right = new Line2D(Vec2D(0,0), Vec2D(0,0), JColour::Black);
 
  top_draw_group.addItem(line_top);
  top_draw_group.addItem(line_left);
  top_draw_group.addItem(line_bottom);
  top_draw_group.addItem(line_right);

}



void ChartKey2D::addItem(DrawItem2D* di, const JColour col) { 
  key_item* ki = new key_item(di, col);
  item_list.append(ki); 
  top_draw_group.addItem(ki->line);
  top_draw_group.addItem(di);
}



// ChartKey2D::arrangeDrawing()

void ChartKey2D::arrangeDrawing(const Chart2D& chart) {

    // calc size
  int total_w = left_margin + right_margin + 2;
  int total_h = top_margin + bottom_margin + 2;
  if (item_list.count() > 0) total_h += (item_list.count()-1) * item_y_spacing;

  JListIterator<key_item> iter(item_list);
  key_item* ki = iter.first();
  while (ki) {
    int w = (int) (ki->label->boundingBox().xSize() + line_item_spacing + line_length + left_margin+right_margin + 2);
    if (w > total_w) total_w = w;
    total_h += (int) (ki->label->boundingBox().ySize());
    ki = iter.next();
  }

  //jlog::ls << "total_w " << total_w << " total_h " << total_h << "\n"; 
  //jlog::ls << "tr " << chart.plotAreaTopRight().x() << " " << chart.plotAreaTopRight().y() << "\n"; 

  Vec2D v(chart.plotAreaTopRight());
  Vec2D corn00(v.x() - total_w - 4, v.y() - total_h - 4);
  Vec2D corn01(corn00.x(), corn00.y()+total_h-1);
  Vec2D corn10(corn00.x()+total_w-1, corn00.y());
  Vec2D corn11(corn00.x()+total_w-1, corn00.y()+total_h-1);

  back_rect->setTo(Box2D(corn00, corn11));

  line_top->setEndPositions(corn01, corn11);
  line_left->setEndPositions(corn00, corn01);
  line_right->setEndPositions(corn10, corn11);
  line_bottom->setEndPositions(corn00, corn10);

  int line_x0 = (int) (corn00.x() + 1 + left_margin);
  int line_x1 = (int) (line_x0 + line_length);

  ki = iter.first();
  if (ki) {
    int curr_y = (int) (corn01.y() - ki->label->boundingBox().ySize() / 2 - top_margin - 1); 
    while (ki) {
      ki->line->setEndPositions(Vec2D(line_x0, curr_y), Vec2D(line_x1, curr_y));
      ki->label->setCentrePos(Vec2D(line_x1 + line_item_spacing + ki->label->boundingBox().xSize() / 2, curr_y));
      curr_y -= (int) (ki->label->boundingBox().ySize() + item_y_spacing);
      ki = iter.next();
    }
  }

}

