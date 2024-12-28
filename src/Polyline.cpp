#include "Polyline.h"
#include "common.h"

using namespace SVGShapes;


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


Polyline::Polyline(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles) :
  BaseShape(attrs, attrs_count, parent, styles) {
  for (int i = 0; i < attrs_count; ++i){
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    if (key == "points") {
      this->point_list = read_point(value);
    }
  }
}

ArrayList<BezierCurve> Polyline::get_beziers() const {
  ArrayList<BezierCurve> curves;

  for (uint32_t i = 1; i < this->point_list.len(); ++i) {
    Point start = this->point_list[i - 1];
    Point end = this->point_list[i];
    Point mid = (start + end) / 2;

    curves.push(BezierCurve {start, end, mid, mid});
  }

  return curves;
}
