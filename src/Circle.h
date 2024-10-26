#ifndef CIRCLE_H
#define CIRCLE_H

#include "BaseShape.h"

class Circle : public BaseShape {
public:
  Circle(Attribute *attrs, int attrs_count, BaseShape *parent);
};

#endif
