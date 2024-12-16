#include "Transform.h"
#include "common.h"
#include <string_view>
#include <cmath>
#include <cctype>
#include "InverseIndex.h"

struct Array {
  double a[6];
  int n;
};

static Array split_number(std::string_view inf) {
  Array arr;
  arr.n = 0;
  while (inf.size() > 0) {
    inf = trim_start(inf);
    while(isspace(inf[0]) != (inf[0] == ',')) inf = inf.substr(1);
    char *out;
    arr.a[arr.n] = strtod(inf.data(), &out);
    inf = inf.substr(out - inf.data());
    arr.n++;
  }

  return arr;
} 

static Transform translate(Array arr) {
  Transform transform = Transform::identity();
  if (arr.n == 2) {
    transform.d[0] = arr.a[0];
    transform.d[1] = arr.a[1];
  } else {
    transform.d[0] = arr.a[0];
  }

  return transform;
}

enum TransformType {
  TRANSFORM_MATRIX = 0,
  TRANSFORM_TRANSLATE,
  TRANSFORM_SCALE,
  TRANSFORM_ROTATE,
  TRANSFORM_SKEWX,
  TRANSFORM_SKEWY,
  TRANSFORM_COUNT,
};

constexpr std::string_view transform_name[TRANSFORM_COUNT] = {
  "matrix",
  "translate",
  "scale",
  "rotate",
  "skewX",
  "skewY",
};

constexpr InverseIndex<TRANSFORM_COUNT> inv_transform = {&transform_name};

static Transform matrix(Array arr) {
  Transform transform;
  transform.m[0][0] = arr.a[0]; 
  transform.m[1][0] = arr.a[1]; 
  transform.m[0][1] = arr.a[2]; 
  transform.m[1][1] = arr.a[3];
  transform.d[0] = arr.a[4], 
  transform.d[1] = arr.a[5];

  return transform;
}

static Transform scale(Array arr) {
  Transform transform = Transform::zeros();
  if (arr.n == 2) {
    transform.m[0][0] = arr.a[0];
    transform.m[1][1] = arr.a[1];
  } else {
    transform.m[0][0] = arr.a[0];
    transform.m[1][1] = arr.a[0];
  }

  return transform;
}

static Transform rotate(Array arr) {
  Transform transform;
  double angle = arr.a[0] * PI / 180;
  transform.m[0][0] = std::cos(angle); 
  transform.m[0][1] = -std::sin(angle);
  transform.m[1][0] = std::sin(angle);
  transform.m[1][1] = std::cos(angle);
  transform.d[0] = 0;
  transform.d[1] = 0;
 
  if (arr.n > 1) {
    Array translate_arr;
    translate_arr.a[0] = arr.a[1];
    translate_arr.a[1] = arr.a[2];
    translate_arr.n = arr.n - 1;
    transform = translate(translate_arr) * transform;

    translate_arr.a[0] = -arr.a[1];
    translate_arr.a[1] = -arr.a[2];
    transform = transform * translate(translate_arr);
  }

  return transform;
}

static Transform skewX(Array arr) {
  Transform transform = Transform::identity();
  transform.m[0][1] = std::tan(arr.a[0] * PI / 180);

  return transform;  
}

static Transform skewY(Array arr) {
  Transform transform = Transform::identity();
  transform.m[1][0] = std::tan(arr.a[0] * PI / 180);

  return transform;
}

static Transform (*transform_fns[TRANSFORM_COUNT])(Array arr) = {
  matrix,
  translate,
  scale,
  rotate,
  skewX,
  skewY,
};

static Transform solve_transform(std::string_view inf) {
  int end = inf.find('(');
  std::string_view str_type = inf.substr(0, end);

  inf = inf.substr(end + 1);

  int type = inv_transform[str_type];
  if (type != -1) {
    return transform_fns[type](split_number(inf));
  }

  return Transform::identity();
}

Transform convert_transform(std::string_view value) {
  Transform transform = Transform::identity();
  
  while (value.size() > 0) {
    value = trim_start(value);
    int end = value.find(')');
    std::string_view type = value.substr(0, end);
    transform = transform * solve_transform(type);

    value = value.substr(end + 1);
  }
  return transform;
}