#include "Rect.h"
#include "InverseIndex.h"

using namespace SVGShapes;

enum RectAttr {
  RECT_ATTR_X = 0,
  RECT_ATTR_Y,
  RECT_ATTR_RX,
  RECT_ATTR_RY,
  RECT_ATTR_WIDTH,
  RECT_ATTR_HEIGHT,
  RECT_ATTR_COUNT,
};

constexpr std::string_view rect_attr_name[RECT_ATTR_COUNT] = {
  "x", 
  "y",
  "rx",
  "ry",
  "width",
  "height",
};

constexpr InverseIndex<RECT_ATTR_COUNT> inv_rect_attribute {&rect_attr_name};

Rect::Rect(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles) :
  BaseShape(attrs, attrs_count, parent, styles),
  x{0}, y{0}, 
  rx{0}, ry{0},
  width{0}, height{0} {

  //go through all of attributes readed, read the rect's attrs
  for (int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    switch((RectAttr)inv_rect_attribute[key]) {
      case RECT_ATTR_X: {
        this->x = strtod(value.data(), nullptr);
      } break;

      case RECT_ATTR_Y: {
        this->y = strtod(value.data(), nullptr);
      } break;

      case RECT_ATTR_RX: {
        this->rx = strtod(value.data(), nullptr);
      } break;

      case RECT_ATTR_RY: {
        this->ry = strtod(value.data(), nullptr);
      } break;

      case RECT_ATTR_WIDTH: {
        this->width = strtod(value.data(), nullptr);
      } break;

      case RECT_ATTR_HEIGHT: {
        this->height = strtod(value.data(), nullptr);
      } break;

      case RECT_ATTR_COUNT: {
        __builtin_unreachable();
      }
    }
  }
}

ArrayList<BezierCurve> Rect::get_beziers() const {
  ArrayList<BezierCurve> curves;

  double rx = std::min(this->rx, this->width / 2);
  double ry = std::min(this->ry, this->height / 2);

  Point sides[4][2] = {
    {
      { this->x, this->y + ry },
      { this->x, this->y + this->height - ry },
    },
    {
      { this->x + rx, this->y + this->height },
      { this->x + this->width - rx, this->y + this->height },
    },
    {
      { this->x + this->width, this->y + this->height - ry },
      { this->x + this->width, this->y + ry },
    },
    {
      { this->x + this->width - rx, this->y },
      { this->x + rx, this->y },
    },
  };

  Point mids[4];
  for (int i = 0; i < 4; ++i) {
    mids[i] = (sides[i][0] + sides[i][1]) / 2;
  }

  curves.push(BezierCurve { sides[0][0], sides[0][1], mids[0], mids[0] });
  curves.push(BezierCurve {
    sides[0][1],
    sides[1][0],
    sides[0][1] + Point {rx * KX, ry * KY},
    sides[1][0] - Point {rx * KY, ry * KX},
  });

  curves.push(BezierCurve { sides[1][0], sides[1][1], mids[1], mids[1] });
  curves.push(BezierCurve {
    sides[1][1],
    sides[2][0],
    sides[1][1] + Point {rx * KY, -ry * KX},
    sides[2][0] - Point {rx * KX, -ry * KY},
  });

  curves.push(BezierCurve { sides[2][0], sides[2][1], mids[2], mids[2] });
  curves.push(BezierCurve {
    sides[2][1],
    sides[3][0],
    sides[2][1] + Point {-rx * KX, -ry * KY},
    sides[3][0] - Point {-rx * KY, - ry * KX },
  });

  curves.push(BezierCurve { sides[3][0], sides[3][1], mids[3], mids[3] });
  curves.push(BezierCurve {
    sides[3][1],
    sides[0][0],
    sides[3][1] + Point {-rx * KY, ry * KX},
    sides[0][0] - Point {-rx * KX, ry * KY},
  });

  return curves;
}
