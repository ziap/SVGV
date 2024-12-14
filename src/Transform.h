#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Matrix.h"
#include <string_view>

struct Array {
  double a[6];
  int n;
};

Array split_number(std::string_view inf);

Transform translate(Array arr); 

Transform matrix(Array arr);

Transform scale(Array arr);

Transform rotate(Array arr);

Transform skewX(Array arr);

Transform skewY(Array arr);

Transform solve_transform(std::string_view inf);

Transform convert_transform(std::string_view value);

#endif
