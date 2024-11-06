#include "Line.h"

void Line::draw() const {
  std::cout << "The line attrs: " << 
    "X1: " << this->x1 << " | " <<
    "Y1: " << this->y1 << "\t " << 
    "X2: " << this->x2 << " | " << 
    "Y2: " << this->y2 << '\n';
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
    LineAttr attr = (LineAttr)inv_line_attribute[key];
      switch(attr){
        case LINE_ATTR_X1: {
          std::from_chars(value.data(), value.data() + value.size(), this->x1);
        } break;

        case LINE_ATTR_Y1: {
          std::from_chars(value.data(), value.data() + value.size(), this->y1);
        } break;

        case LINE_ATTR_X2: {
          std::from_chars(value.data(), value.data() + value.size(), this->x2);
        } break;

        case LINE_ATTR_Y2: {
          std::from_chars(value.data(), value.data() + value.size(), this->y2);
        } break;

        case LINE_ATTR_COUNT: {
          __builtin_unreachable();
        }
      }
  }
  std::cout << "INFO: Finished read Line attributes\n";
}
