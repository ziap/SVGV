#include "Rect.h"
#include <charconv>
// #include "raylib.h"

enum RectAttr {
  RECT_ATTR_X = 0,
  RECT_ATTR_Y,
  RECT_ATTR_RX,
  RECT_ATTR_RY,
  RECT_ATTR_WIDTH,
  RECT_ATTR_HEIGHT,
  RECT_ATTR_COUNT,
};

constexpr std::string_view rect_attr_name[RECT_ATTR_COUNT] {
  "x", 
  "y",
  "rx",
  "rx",
  "width",
  "height",
};

constexpr InverseIndex<RECT_ATTR_COUNT> inv_rect_attribute = {&rect_attr_name};


void Rect::draw() const {
    // if (this->fill != nullptr) {
    //     RGB* colorFill = static_cast<RGB*>(fill.get());
    //     Color color = {
    //         static_cast<unsigned char>(colorFill->r * 255),
    //         static_cast<unsigned char>(colorFill->g * 255),
    //         static_cast<unsigned char>(colorFill->b * 255),
    //         static_cast<unsigned char>(this->fill_opacity * 255)
    //     };

    //     DrawRectangle(
    //         static_cast<int>(this->x),
    //         static_cast<int>(this->y),
    //         static_cast<int>(this->width),
    //         static_cast<int>(this->height),
    //         color
    //     );
    // }

    // // Draw stroke
    // if (this->stroke != nullptr) {
    //     RGB* colorStroke = static_cast<RGB*>(stroke.get());
    //     Color color = {
    //         static_cast<unsigned char>(colorStroke->r * 255),
    //         static_cast<unsigned char>(colorStroke->g * 255),
    //         static_cast<unsigned char>(colorStroke->b * 255),
    //         255 // or use static_cast<unsigned char>(this->stroke_opacity * 255) if opacity is desired
    //     };

    //     Rectangle rect = {
    //         static_cast<float>(this->x),
    //         static_cast<float>(this->y),
    //         static_cast<float>(this->width),
    //         static_cast<float>(this->height)
    //     };

    //     DrawRectangleLinesEx(rect, static_cast<int>(this->stroke_width), color);
    // }
}

Rect::Rect(Attribute *attrs, int attrs_count, BaseShape *parent)
  : BaseShape(attrs, attrs_count, parent) {
    std::cout << "INFO: Creating Rect\n";
    this->x = 0;
    this->y = 0;
    this->width = 0;
    this->height = 0;
    this->rx = 0;
    this->ry = 0;
  
    //go through all of attributes readed, read the rect's attrs
    for (int i = 0; i < attrs_count; ++i) {
      std::string_view key = attrs[i].key;
      std::string_view value = attrs[i].value;

      switch((RectAttr)inv_rect_attribute[key]){
        case RECT_ATTR_X: {
          this->x = strtod(value.data(), nullptr);
        } break;

        case RECT_ATTR_Y: {
          this->y = strtod(value.data(), nullptr);
        } break;

        case RECT_ATTR_RX: {
          this->rx = strtod(value.data(), nullptr);
        } break;

        case RECT_ATTR_RY: {
          this->ry = strtod(value.data(), nullptr);
        } break;
                            
        case RECT_ATTR_WIDTH: {
          this->width = strtod(value.data(), nullptr);
        } break;

        case RECT_ATTR_HEIGHT: {
          this->height = strtod(value.data(), nullptr);
        } break;

        case RECT_ATTR_COUNT: {
          __builtin_unreachable();
        }
      }
    }
    std::cout << "INFO: Finished read rect attributes\n";
}
