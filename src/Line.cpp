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

void Line::render(Gdiplus::Graphics *graphics) const {
  Gdiplus::GraphicsPath path;
  path.AddBezier((Gdiplus::REAL)this->p1[0], (Gdiplus::REAL)this->p1[1],
                 (Gdiplus::REAL)this->p1[0], (Gdiplus::REAL)this->p1[1], 
                 (Gdiplus::REAL)this->p2[0], (Gdiplus::REAL)this->p2[1], 
                 (Gdiplus::REAL)this->p2[0], (Gdiplus::REAL)this->p2[1]); 

  Gdiplus::Matrix matrix = {
    (Gdiplus::REAL)this->transform.m[0][0],
    (Gdiplus::REAL)this->transform.m[1][0],
    (Gdiplus::REAL)this->transform.m[0][1],
    (Gdiplus::REAL)this->transform.m[1][1],
    (Gdiplus::REAL)this->transform.d[0],
    (Gdiplus::REAL)this->transform.d[1]
  };
  path.Transform(&matrix);

  if (this->stroke_brush) {
    Gdiplus::Pen pen = {this->stroke_brush.get(), (float)this->stroke_width};
    graphics->DrawPath(&pen, &path);
  }
}

Line::Line(Attribute *attrs, int attrs_count, BaseShape *parent)
  : BaseShape(attrs, attrs_count, parent) {
  std::cout << "INFO: Creating Line\n";

  for(int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
      switch((LineAttr)inv_line_attribute[key]){
        case LINE_ATTR_X1: {
          this->p1[0] = strtod(value.data(), nullptr);
        } break;

        case LINE_ATTR_Y1: {
          this->p1[1] = strtod(value.data(), nullptr);
        } break;

        case LINE_ATTR_X2: {
          this->p2[0] = strtod(value.data(), nullptr);
        } break;

        case LINE_ATTR_Y2: {
          this->p2[1] = strtod(value.data(), nullptr);
        } break;

        case LINE_ATTR_COUNT: {
          __builtin_unreachable();
        }
      }
  }
  std::cout << "INFO: Finished read Line attributes\n";
}
