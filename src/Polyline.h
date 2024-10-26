#ifndef POLYLINE_H
#define POLYLINE_H

#include "BaseShape.h"

class Polyline : public BaseShape {
public:
  Polyline(Attribute *attrs, int attrs_count, BaseShape *parent);
};

#endif
