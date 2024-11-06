#include "Polygon.h"

void Polygon::draw() const{
  std::cout << "The Polygon attrs: " << '\n' <<
    "POINTS:\n";
  std::cout << this->point_list.len() << '\n';
  for (uint32_t i = 0; i < this->point_list.len(); ++i){
    std::cout << "X: " << this->point_list[i].x << " | " <<
                 "Y: " << this->point_list[i].y << '\n';
  }
}


static ArrayList<Point> read_point(std::string_view str){
  ArrayList<Point> point_list;
  Point new_point;
  std::from_chars_result res;
  while(!str.empty()){
    while (str[0] == ' ' || str[0] == ','){
      str = str.substr(1);
    }
    res  = std::from_chars(str.data(), str.data() + str.size(), new_point.x); 
    str = str.substr(res.ptr - str.data());
      
    while (str[0] == ' ' || str[0] == ','){
      str = str.substr(1);
    }

    res  = std::from_chars(str.data(), str.data() + str.size(), new_point.y); 
    str = str.substr(res.ptr - str.data());
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
