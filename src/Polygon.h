#ifndef POLYGON_H
#define POLYGON_H

#include "BaseShape.h"

class Polygon : public BaseShape {
public:
  Polygon(Attribute *attrs, int attrs_count, BaseShape *parent);
};

#endif
