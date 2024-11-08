#include "Text.h"
#include <charconv>
// #include "raylib.h"
#include <string>
enum TextAttr {
  TEXT_ATTR_X = 0,
  TEXT_ATTR_Y,
  TEXT_ATTR_COUNT
};

constexpr std::string_view text_attr_name[TEXT_ATTR_COUNT] {
  "x",
  "y",
};

constexpr InverseIndex<TEXT_ATTR_COUNT> inv_text_attribute = {&text_attr_name};

Text::Text(Attribute *attrs, int attrs_count, BaseShape *parent)
  : BaseShape(attrs, attrs_count, parent),
    content{""},
    pos{0, 0} {
  for (int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
    
    switch ((TextAttr)inv_text_attribute[key]) {
      case TEXT_ATTR_X: {
        this->pos.x = strtod(value.data(), nullptr);
      } break;
      case TEXT_ATTR_Y: {
        this->pos.y = strtod(value.data(), nullptr);
      } break;
      case TEXT_ATTR_COUNT: {
        __builtin_unreachable();
      }
    }
  }
}

void Text::draw() const {
  
  //   if (this->fill != nullptr) {
  //   RGB* colorFill = static_cast<RGB*>(fill.get());
  //   Color color = {
  //     static_cast<unsigned char>(colorFill->r * 255),
  //     static_cast<unsigned char>(colorFill->g * 255),
  //     static_cast<unsigned char>(colorFill->b * 255),
  //     static_cast<unsigned char>(this->fill_opacity * 255),
  //   };

  // std::string str = {this->content.begin(), this->content.end()}; 
  // DrawText(str.c_str(), this->pos.x, this->pos.y, this->font_size, color);
  // }
}
