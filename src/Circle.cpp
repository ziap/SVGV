#include "Circle.h"
#include "InverseIndex.h"

using namespace SVGShapes;

enum CircleAttr {
  CIRCLE_ATTR_CX = 0,
  CIRCLE_ATTR_CY,
  CIRCLE_ATTR_R,
  CIRCLE_ATTR_COUNT,
};

constexpr std::string_view circle_attr_name[CIRCLE_ATTR_COUNT] = {
  "cx", 
  "cy",
  "r",
};

constexpr InverseIndex<CIRCLE_ATTR_COUNT> inv_circle_attribute {&circle_attr_name};

Circle::Circle(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles) :
  BaseShape(attrs, attrs_count, parent, styles),
  c{0, 0},
  r{0} {
  for (int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
      
    switch ((CircleAttr)inv_circle_attribute[key]) {
      case CIRCLE_ATTR_CX: {
        this->c[0] = strtod(value.data(), nullptr);
      } break;

      case CIRCLE_ATTR_CY: {
        this->c[1] = strtod(value.data(), nullptr);
      } break;

      case CIRCLE_ATTR_R: {
        this->r = strtod(value.data(), nullptr);
      } break;

      case CIRCLE_ATTR_COUNT: {
        __builtin_unreachable();
      }
    }
  }
}

ArrayList<BezierCurve> Circle::get_beziers() const {
  double r = this->r;

  Point p0 = this->c + Point {-r, 0};
  Point p1 = this->c + Point {0, r};
  Point p2 = this->c + Point {r, 0};
  Point p3 = this->c + Point {0, -r};

  ArrayList<BezierCurve> curves;
  curves.push(BezierCurve {
    p0, p1, p0 + Point {r * KX, r * KY}, p1 - Point {r * KY, r * KX}
  });

  curves.push(BezierCurve {
    p1, p2, p1 + Point {r * KY, -r * KX}, p2 - Point {r * KX, -r * KY}
  });

  curves.push(BezierCurve {
    p2, p3, p2 + Point {-r * KX, -r * KY}, p3 - Point {-r * KY, - r * KX }
  });

  curves.push(BezierCurve {
    p3, p0, p3 + Point {-r * KY, r * KX}, p0 - Point {-r * KX, r * KY}
  });

  return curves;
}
