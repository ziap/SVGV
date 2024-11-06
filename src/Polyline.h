#ifndef POLYLINE_H
#define POLYLINE_H

#include "BaseShape.h"
#include "ArrayList.h"
#include "Point.h"
#include <memory>

class Polyline : public BaseShape {
public:
  ArrayList<Point> point_list;

  Polyline(Attribute *attrs, int attrs_count, BaseShape *parent);
  void draw() const;
};

#endif
