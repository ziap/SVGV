#include "Polygon.h"
#include <charconv>
#include <vector>
// #include "raylib.h"



void Polygon::draw() const{
//  if (this->fill != nullptr) {
//    RGB* colorFill = static_cast<RGB*>(fill.get());
//    Color color = {
//      static_cast<unsigned char>(colorFill->r * 255),
//      static_cast<unsigned char>(colorFill->g * 255),
//      static_cast<unsigned char>(colorFill->b * 255),
//      static_cast<unsigned char>(this->fill_opacity * 255),
//    };
//   //std::cerr << "INFO: Polygon: " << (int)color.r << ' ' << (int)color.g << ' ' << (int)color.b << ' ' << (int)color.a << '\n';

//   std::vector<Vector2> vertices;
//   for (int i = 0; i < this->point_list.len(); ++i){
//     vertices.push_back({(float)point_list[i].x, (float)point_list[i].y});
//   }

//   if (!vertices.empty()){
//     DrawPoly({20, 20}, vertices.size(), 1.0f, 1.0f, color);
//   }

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
    char **out;
    new_point.x = strtod(str.data(), out);
    if (*out == str.data()) break;
    str = str.substr(*out - str.data());
      
    str = trim_start(str);
    new_point.y = strtod(str.data(), out);
    if (*out == str.data()) break;
    str = str.substr(*out - str.data());

    point_list.push(new_point);
  }
  return point_list;
}

Polygon::Polygon(Attribute *attrs, int attrs_count, BaseShape *parent) : BaseShape(attrs, attrs_count, parent) {

  std::cout << "Creating Polygon\n";
  for (int i = 0; i < attrs_count; ++i){
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    if (key == "points"){
      this->point_list = read_point(value);
    }
    std::cout << "value END " << value << '\n';
  }
  std::cout << "Finished read Polyline attributes\n";
}
