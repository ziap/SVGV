#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "BaseShape.h"

class Ellipse : public BaseShape {
public:
  Ellipse(Attribute *attrs, int attrs_count, BaseShape *parent);
};

#endif
