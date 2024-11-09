#include "Rect.h"

using namespace SVGShapes;

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


void Rect::render(Gdiplus::Graphics *graphic) const {
  if (this->fill_brush) {
    graphic->FillRectangle(this->fill_brush.get(), (float)this->x, (float)this->y, (float)this->width, (float)this->height);
  }

  if (this->stroke_brush) {
    Gdiplus::Pen pen = {this->stroke_brush.get(), (float)this->stroke_width};
    graphic->DrawRectangle(&pen, (float)this->x, (float)this->y, (float)this->width, (float)this->height);
  }
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
