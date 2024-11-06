#include "Ellipse.h"
#include <charconv>

enum EllipseAttr {
  ELLIPSE_ATTR_CX = 0,
  ELLIPSE_ATTR_CY,
  ELLIPSE_ATTR_RX,
  ELLIPSE_ATTR_RY,
  ELLIPSE_ATTR_COUNT,
};

constexpr std::string_view ellipse_attr_name[ELLIPSE_ATTR_COUNT] {
  "cx", 
  "cy",
  "rx",
  "ry",
};

constexpr InverseIndex<ELLIPSE_ATTR_COUNT> inv_ellipse_attribute= {&ellipse_attr_name};

void Ellipse::draw() const {
  std::cout << "The ellipse attrs: " << 
    "X: " << this->cx << " | " <<
    "Y: " << this->cy << " | " << 
    "RX: " << this->rx << " | " << 
    "RY: " << this->ry << '\n';
}

Ellipse::Ellipse(Attribute *attrs, int attrs_count, BaseShape *parent)
  : BaseShape(attrs, attrs_count, parent) {
  std::cout << "INFO: Creating Ellipse\n";
  this->cx = 0;
  this->cy = 0;
  this->rx = 0;
  this->ry = 0;

  for(int i = 0; i < attrs_count; ++i) {

    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

      EllipseAttr attr = (EllipseAttr)inv_ellipse_attribute[key];
      switch(attr){
        case ELLIPSE_ATTR_CX: {
          std::from_chars(value.data(), value.data() + value.size(), this->cx);
        } break;

        case ELLIPSE_ATTR_CY: {
          std::from_chars(value.data(), value.data() + value.size(), this->cy);
        } break;

        case ELLIPSE_ATTR_RX: {
          std::from_chars(value.data(), value.data() + value.size(), this->rx);
        } break;

        case ELLIPSE_ATTR_RY: {
          std::from_chars(value.data(), value.data() + value.size(), this->ry);
        } break;

        case ELLIPSE_ATTR_COUNT: {
          __builtin_unreachable();
        }
      }
  }
  std::cout << "INFO: Finished read Ellipse attributes\n";
}