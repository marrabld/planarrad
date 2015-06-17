
// symbol2d.cc

#include "symbol2d.h"
#include "drawS2/trans2d.h"
#include "base/jbuffer.h"

using namespace jude::drawS2;



// Symbol2D::(constructor)

Symbol2D::Symbol2D(int code, const Vec2D& p, const JColour& c, int sz, int t) :
  symbol_code(code),
  pixel_size(sz),
  line_thickness(t),
  pos(p),
  col(c) { }



// Symbol2D::boundingBox()

const Box2D Symbol2D::boundingBox() const {
  return Box2D(pos, pos);
}

 

// Symbol2D::draw()

void Symbol2D::draw(const Trans2D& trans, JBuffer& buff) const {

  Vec2D v(trans.intoTrans(pos));
 
  int32 x = (int32) v.x();
  int32 y = (int32) v.y();
 
  int32 half_d = pixel_size >> 1;
  //int32 d = 3;

  //if (selected_flag) buffer.drawRectangle(x-d, y-d, pixel_size, pixel_size, col);
 
  switch (symbol_code) {

    case Circle : {
      //buffer.drawEllipse(x-d, y-d, d, d, col);
      break;
    }

    case FilledCircle : {
      buff.fillEllipse(x-half_d, y-half_d, x+half_d, y+half_d, col);
      break;
    }

    case Square : {
      //buffer.fillRectangle(x-half_d, y-half_d, x+half_d, y+half_d, col);
      break;
    }

    case FilledSquare : {
      buff.fillRectangle(x-half_d, y-half_d, x+half_d, y+half_d, col);
      break;
    }

    case Triangle : {
        // third of the height of triangle
      //int t = (296 * pixel_size + 512) >> 10;
      // buffer.drawLine(x-d, y+t, x+d, y+t, col);
      //buffer.drawLine(x-d, y+t, x, y-(t<<1), col);
      //buffer.drawLine(x+d, y+t, x, y-(t<<1), col);
      break;
    }

    case FilledTriangle : {
        // third of the height of triangle
      //int t = (296 * pixel_size + 512) >> 10;
      //buffer.drawLine(x-d, y+t, x+d, y+t, col);
      //buffer.drawLine(x-d, y+t, x, y-(t<<1), col);
      //buffer.drawLine(x+d, y+t, x, y-(t<<1), col);
      break;
    }

    case StraightCross : {
      //buffer.drawLine(x, y-d, x, y+d, col);
      //buffer.drawLine(x-d, y, x+d, y, col);
      break;
    }

    case DiagonalCross : {
      //buffer.drawLine(x-d, y-d, x+d, y+d, col);
      //buffer.drawLine(x-d, y+d, x+d, y-d, col);
      break;
    }

    case VerticalBar : {
      //buffer.drawLine(x, y-d, x, y+d, col);
      break;
    }

    case HorizontalBar : {
      //buffer.drawLine(x-d, y, x+d, y, col);
      break;
    }

  }
}



void Symbol2D::drawEPS(const Trans2D& trans, JTextStream& str) const {

  /*
  int rx = trans.realToDrawX(pos.x());
  int ry = trans.realToDrawY(pos.y());

  int d = pixel_size >> 1;

  switch (symbol_code) {

  case Circle :
  case FilledCircle : {
    pc.stream() << "newpath\n";
    pc.stream() << rx << " " << ry << " 3 0 360 arc\n";
    if (symbol_code == FilledCircle) pc.stream() << "fill\n";
    pc.stream() << "stroke\n";
    break;
  }

  case Triangle :
  case FilledTriangle : {
      // third of the height of triangle
    int t = (296 * pixel_size + 512) >> 10;
    pc.stream() << "newpath\n";
    pc.stream() << rx-d << " " << ry-t << " moveto\n";
    pc.stream() << rx+d << " " << ry-t << " lineto\n";
    pc.stream() << rx << " " << ry+(t<<1) << " lineto\n";
    pc.stream() << rx-d << " " << ry-t << " lineto\n";
    if (symbol_code == FilledTriangle) pc.stream() << "fill\n";
    pc.stream() << "stroke\n";
    break;
  }

  case Square :
  case FilledSquare : {
    pc.stream() << "newpath\n";
    pc.stream() << rx-d << " " << ry-d << " moveto\n";
    pc.stream() << rx-d << " " << ry+d << " lineto\n";
    pc.stream() << rx+d << " " << ry+d << " lineto\n";
    pc.stream() << rx+d << " " << ry-d << " lineto\n";
    pc.stream() << rx-d << " " << ry-d << " lineto\n";
    if (symbol_code == FilledSquare) pc.stream() << "fill\n";
    pc.stream() << "stroke\n";
    break;
  }

  case StraightCross : {
    pc.stream() << "newpath\n";
    pc.stream() << rx << " " << ry-d << " moveto\n";
    pc.stream() << rx << " " << ry+d << " lineto\n";
    pc.stream() << rx-d << " " << ry << " moveto\n";
    pc.stream() << rx+d << " " << ry << " lineto\n";
    pc.stream() << "stroke\n";
    break;
  }

  case DiagonalCross : {
    pc.stream() << "newpath\n";
    pc.stream() << rx-d << " " << ry-d << " moveto\n";
    pc.stream() << rx+d << " " << ry+d << " lineto\n";
    pc.stream() << rx-d << " " << ry+d << " moveto\n";
    pc.stream() << rx+d << " " << ry-d << " lineto\n";
    pc.stream() << "stroke\n";
    break;
  }

  }
  */
}

