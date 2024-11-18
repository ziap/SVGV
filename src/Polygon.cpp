#include "Polygon.h"
#include "Path.h"

using namespace SVGShapes;

void Polygon::render(Gdiplus::Graphics *graphics) const {
  Gdiplus::FillMode fillmode;
  switch (this->fill_rule) {
    case FILL_RULE_NONZERO:
      fillmode = Gdiplus::FillModeWinding;
      break;
    case FILL_RULE_EVENODD:
      fillmode = Gdiplus::FillModeAlternate;
      break;
    case FILL_RULE_COUNT:
       __builtin_unreachable();
  }

  Gdiplus::GraphicsPath path_list = {fillmode};

  Gdiplus::Matrix matrix = {
    (Gdiplus::REAL)this->transform[0][0],
    (Gdiplus::REAL)this->transform[0][1],
    (Gdiplus::REAL)this->transform[1][0],
    (Gdiplus::REAL)this->transform[1][1],
    (Gdiplus::REAL)this->transform[0][2],
    (Gdiplus::REAL)this->transform[1][2]
  };

  path_list.StartFigure();
  
  uint32_t length = this->point_list.len();
  for (uint32_t i = 0; i < length - 1; ++i){
    path_list.AddBezier((Gdiplus::REAL)this->point_list[i][0],
                        (Gdiplus::REAL)this->point_list[i][1],
                        (Gdiplus::REAL)this->point_list[i][0],
                        (Gdiplus::REAL)this->point_list[i][1],
                        (Gdiplus::REAL)this->point_list[i + 1][0],
                        (Gdiplus::REAL)this->point_list[i + 1][1],
                        (Gdiplus::REAL)this->point_list[i + 1][0],
                        (Gdiplus::REAL)this->point_list[i + 1][1]);
  }
  
  path_list.CloseFigure();

  path_list.Transform(&matrix);

  if (this->fill_brush) {
    graphics->FillPath(this->fill_brush.get(), &path_list);
  }

  if (this->stroke_brush) {
    Gdiplus::Pen pen = {this->stroke_brush.get(), (float)this->stroke_width};
    graphics->DrawPath(&pen, &path_list);
  }
}

static std::string_view trim_start(std::string_view str) {
  while (str.size() && (isspace(str[0]) || str[0] == ',')) {
    str = str.substr(1);
  }
  return str;
}

static ArrayList<Point> read_point(std::string_view str) {
  ArrayList<Point> point_list;
  Point new_point;
  while (str.size()) {
    str = trim_start(str);
    char *out;
    new_point[0] = strtod(str.data(), &out);
    if (out == str.data()) break;
    str = str.substr(out - str.data());
      
    str = trim_start(str);
    new_point[1] = strtod(str.data(), &out);
    if (out == str.data()) break;
    str = str.substr(out - str.data());

    point_list.push(new_point);
  }
  return point_list;
}

Polygon::Polygon(Attribute *attrs, int attrs_count, BaseShape *parent) : BaseShape(attrs, attrs_count, parent) {

  std::cout << "INFO: Creating Polygon\n";
  for (int i = 0; i < attrs_count; ++i){
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    if (key == "points"){
      this->point_list = read_point(value);
    }
  }
  std::cout << "INFO: Finished read Polyline attributes\n";
}
