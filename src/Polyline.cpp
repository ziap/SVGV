#include "Polyline.h"
#include <charconv>
#include <string_view>

void Polyline::draw() const{
  // std::cout << "The Polyline attrs: " << '\n' <<
  //   "POINTS:\n";
  // std::cout << this->point_list.len() << '\n';
  // for (uint32_t i = 0; i < this->point_list.len(); ++i){
  //   std::cout << "X: " << this->point_list[i].x << " | " <<
  //                "Y: " << this->point_list[i].y << '\n';
  // }
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
    new_point.x = strtod(str.data(), &out);
    if (out == str.data()) break;
    str = str.substr(out - str.data());
      
    str = trim_start(str);
    new_point.y = strtod(str.data(), &out);
    if (out == str.data()) break;
    str = str.substr(out - str.data());

    point_list.push(new_point);
  }
  return point_list;
}


Polyline::Polyline(Attribute *attrs, int attrs_count, BaseShape *parent)
  : BaseShape(attrs, attrs_count, parent) {
  std::cout << "INFO: Creating Polyline\n";
  for (int i = 0; i < attrs_count; ++i){
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    if (key == "points") {
      this->point_list = read_point(value);
    }
  }
  std::cout << "INFO: Finished read Polyline attributes\n";
}
