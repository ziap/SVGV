#include "Ellipse.h"
#include <charconv>
// #include "raylib.h"

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
  //std::cout << "The ellipse attrs: " << 
  //  "X: " << this->cx << " | " <<
  //  "Y: " << this->cy << " | " << 
  //  "RX: " << this->rx << " | " << 
  //  "RY: " << this->ry << '\n';
  //   if (this->fill != nullptr) {
  //   RGB* colorFill = static_cast<RGB*>(fill.get());
  //   Color color = {
  //     static_cast<unsigned char>(colorFill->r * 255),
  //     static_cast<unsigned char>(colorFill->g * 255),
  //     static_cast<unsigned char>(colorFill->b * 255),
  //     static_cast<unsigned char>((float)this->fill_opacity * 255),
  //   };
  
  // DrawEllipse(this->cx, this->cy, this->rx, this->ry, color);
//   }
    
//   //draw stroke
//   if (this->stroke != nullptr) {
//   RGB* colorFill = static_cast<RGB*>(stroke.get());
//   Color color = {
//     static_cast<unsigned char>(colorFill->r * 255),
//     static_cast<unsigned char>(colorFill->g * 255),
//     static_cast<unsigned char>(colorFill->b * 255),
//     static_cast<unsigned char>((float)this->stroke_opacity * 255),
//   };

//   for (int i = 0; i <= this->stroke_width; i++) {
//     DrawEllipseLines(this->cx, this->cy, this->rx + i, this->ry + i, color);
//   }
// }
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
          this->cx = strtod(value.data(), nullptr);
        } break;

        case ELLIPSE_ATTR_CY: {
          this->cy = strtod(value.data(), nullptr);
        } break;

        case ELLIPSE_ATTR_RX: {
          this->rx = strtod(value.data(), nullptr);
        } break;

        case ELLIPSE_ATTR_RY: {
          this->ry = strtod(value.data(), nullptr);
        } break;

        case ELLIPSE_ATTR_COUNT: {
          __builtin_unreachable();
        }
      }
  }
  std::cout << "INFO: Finished read Ellipse attributes\n";
}
