#ifndef BASEGRADIENT_H
#define BASEGRADIENT_H

#include <string_view>

#include "Paint.h"
#include "Stop.h"
#include "ArrayList.h"
#include "Matrix.h"
#include "utils.h"
#include "Transform.h"

enum GradientType {
  GRADIENT_TYPE_LINEAR= 0,
  GRADIENT_TYPE_RADIAL,
  GRADIENT_TYPE_COUNT,
};

enum GradientUnits {
  GRADIENT_UNIT_USER_SPACE_ON_USE = 0,
  GRADIENT_UNIT_OBJECT_BOUNDING_BOX,
  GRADIENT_UNIT_COUNT,
};

struct LinearGradient{
  PercentUnit x1;
  PercentUnit x2;
  PercentUnit y1;
  PercentUnit y2;

  Paint to_paint();
};

struct RadialGradient {
  PercentUnit cx;
  PercentUnit cy;
  PercentUnit r;
  Optional<PercentUnit> fx; 
  Optional<PercentUnit> fy; 
  PercentUnit fr;

  Paint to_paint() const;
};

struct Gradient {
  GradientType type;
  union {
    LinearGradient linear;  
    RadialGradient radial;
  } variants;
 
  Transform transform;
  GradientUnits gradient_units;
  std::string_view id;
  ArrayList<Stop> stops;
};

Gradient read_gradient(GradientType type, Attribute *attrs, int attribute_count);

#endif
