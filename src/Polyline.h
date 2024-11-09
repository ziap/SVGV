#ifndef POLYLINE_H
#define POLYLINE_H

#include "BaseShape.h"
#include "ArrayList.h"
#include "Point.h"

namespace SVGShapes {

class Polyline final : public BaseShape {
public:
  ArrayList<Point> point_list;

  Polyline(Attribute *attrs, int attrs_count, BaseShape *parent);
  void render(Gdiplus::Graphics *graphics) const override;
};

};

#endif
