#include "Circle.h"

void Circle::draw() const{
  std::cout << "The circle attrs: " << 
    "X: " << this->cx << " | " <<
    "Y: " << this->cy << " | " << 
    "R: " << this->r << '\n';
}

Circle::Circle(Attribute *attrs, int attrs_count, BaseShape *parent)
  : BaseShape(attrs, attrs_count, parent) {
  std::cout << "Creating Circle\n";
  this->cx = 0;
  this->cy = 0;
  this->r = 0;

  for(int i = 0; i < attrs_count; ++i){
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
      CircleAttr attr = (CircleAttr)inv_circle_attribute[key];
      switch(attr){
        case CIRCLE_ATTR_CX: {
          std::from_chars(value.data(), value.data() + value.size(), this->cx);
        } break;

        case CIRCLE_ATTR_CY: {
          std::from_chars(value.data(), value.data() + value.size(), this->cy);
        } break;

        case CIRCLE_ATTR_R: {
          std::from_chars(value.data(), value.data() + value.size(), this->r);
        } break;

        case CIRCLE_ATTR_COUNT: {
          __builtin_unreachable();
        }
      }
  }
  std::cout << "Finished read attributes circle\n";
}
