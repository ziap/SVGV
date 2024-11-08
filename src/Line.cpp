#include "Line.h"
#include <charconv>
// #include "raylib.h"

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
void Line::draw() const {
  // if (this->fill != nullptr) {
  //   RGB* colorFill = static_cast<RGB*>(fill.get());
  //   Color color = {
  //    static_cast<unsigned char>(colorFill->r * 255),
  //    static_cast<unsigned char>(colorFill->g * 255),
  //    static_cast<unsigned char>(colorFill->b * 255),
  //    static_cast<unsigned char>(this->fill_opacity * 255),
  //   };
  
  //  DrawLine(this->x1, this->y1, this->x1, this->y2, color); 
  // }
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
