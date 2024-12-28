#include "Line.h"
#include "InverseIndex.h"

using namespace SVGShapes;

enum LineAttr {
  LINE_ATTR_X1 = 0,
  LINE_ATTR_Y1,
  LINE_ATTR_X2,
  LINE_ATTR_Y2,
  LINE_ATTR_COUNT,
};

constexpr std::string_view line_attr_name[LINE_ATTR_COUNT] = {
  "x1", 
  "y1",
  "x2",
  "y2",
};

constexpr InverseIndex<LINE_ATTR_COUNT> inv_line_attribute {&line_attr_name};

Line::Line(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles) :
  BaseShape(attrs, attrs_count, parent, styles) {
  for(int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
    switch((LineAttr)inv_line_attribute[key]){
      case LINE_ATTR_X1: {
        this->p1[0] = strtod(value.data(), nullptr);
      } break;

      case LINE_ATTR_Y1: {
        this->p1[1] = strtod(value.data(), nullptr);
      } break;

      case LINE_ATTR_X2: {
        this->p2[0] = strtod(value.data(), nullptr);
      } break;

      case LINE_ATTR_Y2: {
        this->p2[1] = strtod(value.data(), nullptr);
      } break;

      case LINE_ATTR_COUNT: {
        __builtin_unreachable();
      }
    }
  }
}

ArrayList<BezierCurve> Line::get_beziers() const {
  ArrayList<BezierCurve> curves;

  Point mid = (this->p1 + this->p2) / 2;

  curves.push(BezierCurve { this->p1, this->p2, mid, mid });

  return curves;
}
