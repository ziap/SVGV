#include "Ellipse.h"
#include "InverseIndex.h"

using namespace SVGShapes;

enum EllipseAttr {
  ELLIPSE_ATTR_CX = 0,
  ELLIPSE_ATTR_CY,
  ELLIPSE_ATTR_RX,
  ELLIPSE_ATTR_RY,
  ELLIPSE_ATTR_COUNT,
};

constexpr std::string_view ellipse_attr_name[ELLIPSE_ATTR_COUNT] = {
  "cx", 
  "cy",
  "rx",
  "ry",
};

constexpr InverseIndex<ELLIPSE_ATTR_COUNT> inv_ellipse_attribute {&ellipse_attr_name};

Ellipse::Ellipse(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles) :
  BaseShape(attrs, attrs_count, parent, styles),
  c{0, 0},
  rx{0}, 
  ry{0} {

  for (int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    switch ((EllipseAttr)inv_ellipse_attribute[key]){
      case ELLIPSE_ATTR_CX: {
        this->c[0] = strtod(value.data(), nullptr);
      } break;

      case ELLIPSE_ATTR_CY: {
        this->c[1] = strtod(value.data(), nullptr);
      } break;

      case ELLIPSE_ATTR_RX: {
        this->rx = strtod(value.data(), nullptr);
      } break;

      case ELLIPSE_ATTR_RY: {
        this->ry = strtod(value.data(), nullptr);
      } break;

      case ELLIPSE_ATTR_COUNT: {
        __builtin_unreachable();
      }
    }
  }
}

ArrayList<BezierCurve> Ellipse::get_beziers() const {
  double rx = this->rx;
  double ry = this->ry;

  Point p0 = this->c + Point {-rx, 0};
  Point p1 = this->c + Point {0, ry};
  Point p2 = this->c + Point {rx, 0};
  Point p3 = this->c + Point {0, -ry};

  ArrayList<BezierCurve> curves;
  curves.push(BezierCurve {
    p0,
    p1,
    p0 + Point {rx * KX, ry * KY},
    p1 - Point {rx * KY, ry * KX},
  });

  curves.push(BezierCurve {
    p1,
    p2,
    p1 + Point {rx * KY, -ry * KX},
    p2 - Point {rx * KX, -ry * KY},
  });

  curves.push(BezierCurve {
    p2,
    p3,
    p2 + Point {-rx * KX, -ry * KY},
    p3 - Point {-rx * KY, -ry * KX },
  });

  curves.push(BezierCurve {
    p3,
    p0,
    p3 + Point {-rx * KY, ry * KX},
    p0 - Point {-rx * KX, ry * KY},
  });

  return curves;
}
