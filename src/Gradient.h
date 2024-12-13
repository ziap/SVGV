#ifndef BASEGRADIENT_H
#define BASEGRADIENT_H

#include "Paint.h"
#include "Stop.h"
#include "ArrayList.h"
#include "Matrix.h"
#include <string_view>

enum GradientType {
  LINEAR_GRADIENT = 0,
  RADIAL_GRADIENT,
  GRADIENT_TYPE_COUNT,
};

struct RadialGradient {
  Point c;
  double r;
  Paint to_paint() const;
};

struct LinearGradient{
  Point x;
  Point y;
  Paint to_paint();
};

struct Gradient {
  GradientType type;
  union {
    LinearGradient linear;  
    RadialGradient radial;
  } variants;
 
  std::string_view id;
  ArrayList<Stop> stops;
};
Gradient read_gradient(GradientType type, Attribute *attrs, int attribute_count);
#endif
