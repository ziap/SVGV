#include "Line.h"

using namespace SVGShapes;

enum LineAttr {
  LINE_ATTR_X1 = 0,
  LINE_ATTR_Y1,
  LINE_ATTR_X2,
  LINE_ATTR_Y2,
  LINE_ATTR_COUNT,
};

constexpr std::string_view line_attr_name[LINE_ATTR_COUNT] {
  "x1", 
  "y1",
  "x2",
  "y2",
};

constexpr InverseIndex<LINE_ATTR_COUNT> inv_line_attribute= {&line_attr_name};

void Line::render(Gdiplus::Graphics *) const {

}

Line::Line(Attribute *attrs, int attrs_count, BaseShape *parent)
  : BaseShape(attrs, attrs_count, parent) {
  std::cout << "INFO: Creating Line\n";
  this->x1 = 0;
  this->y1 = 0;
  this->x2 = 0;
  this->y2 = 0;

  for(int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
      switch((LineAttr)inv_line_attribute[key]){
        case LINE_ATTR_X1: {
          this->x1 = strtod(value.data(), nullptr);
        } break;

        case LINE_ATTR_Y1: {
          this->y1 = strtod(value.data(), nullptr);
        } break;

        case LINE_ATTR_X2: {
          this->x2 = strtod(value.data(), nullptr);
        } break;

        case LINE_ATTR_Y2: {
          this->y2 = strtod(value.data(), nullptr);
        } break;

        case LINE_ATTR_COUNT: {
          __builtin_unreachable();
        }
      }
  }
  std::cout << "INFO: Finished read Line attributes\n";
}
