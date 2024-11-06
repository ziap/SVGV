#ifndef CIRCLE_H
#define CIRCLE_H

#include "BaseShape.h"
#include "ArrayList.h"
#include <charconv>
#include <memory>

enum CircleAttr{
  CIRCLE_ATTR_CX = 0,
  CIRCLE_ATTR_CY,
  CIRCLE_ATTR_R,
  CIRCLE_ATTR_COUNT,
};

constexpr std::string_view circle_attr_name[CIRCLE_ATTR_COUNT]{
  "cx", 
  "cy",
  "r",
};

constexpr InverseIndex<CIRCLE_ATTR_COUNT> inv_circle_attribute= {&circle_attr_name};

class Circle : public BaseShape {
public:
  double cx;
  double cy;
  double r;

  Circle(Attribute *attrs, int attrs_count, BaseShape *parent);
  void draw() const;
};

#endif
