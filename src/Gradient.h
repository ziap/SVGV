#ifndef BASEGRADIENT_H
#define BASEGRADIENT_H

#include "Attribute.h"
#include "Paint.h"
#include "Stop.h"
#include "LinearGradient.h"
#include "RadialGradient.h"
#include "ArrayList.h"
#include <string_view>

enum GradientType {
  LINEAR_GRADIENT = 0,
  RADIAL_GRADIENT,
  GRADIENT_TYPE_COUNT,
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
