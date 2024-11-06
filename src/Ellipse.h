#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "BaseShape.h"

class Ellipse : public BaseShape {
public:
  double cx;
  double cy;
  double rx;
  double ry;
  Ellipse(Attribute *attrs, int attrs_count, BaseShape *parent);
  void draw() const;
};

#endif
