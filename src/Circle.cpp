#include "Circle.h"

using namespace SVGShapes;

enum CircleAttr {
  CIRCLE_ATTR_CX = 0,
  CIRCLE_ATTR_CY,
  CIRCLE_ATTR_R,
  CIRCLE_ATTR_COUNT,
};

constexpr std::string_view circle_attr_name[CIRCLE_ATTR_COUNT] {
  "cx", 
  "cy",
  "r",
};

constexpr InverseIndex<CIRCLE_ATTR_COUNT> inv_circle_attribute= {&circle_attr_name};

void Circle::render(Gdiplus::Graphics *) const {

}

Circle::Circle(Attribute *attrs, int attrs_count, BaseShape *parent)
  : BaseShape(attrs, attrs_count, parent) {
  std::cout << "INFO: Creating Circle\n";
  this->cx = 0;
  this->cy = 0;
  this->r = 0;

  for(int i = 0; i < attrs_count; ++i){
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
      
      switch ((CircleAttr)inv_circle_attribute[key]) {
        case CIRCLE_ATTR_CX: {
          this->cx = strtod(value.data(), nullptr);
        } break;

        case CIRCLE_ATTR_CY: {
          this->cy = strtod(value.data(), nullptr);
        } break;

        case CIRCLE_ATTR_R: {
          this->r = strtod(value.data(), nullptr);
        } break;

        case CIRCLE_ATTR_COUNT: {
          __builtin_unreachable();
        }
      }
  }
  std::cout << "INFO: Finished read attributes circle\n";
}
