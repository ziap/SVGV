#ifndef POLYGON_H
#define POLYGON_H

#include "BaseShape.h"  
#include "ArrayList.h"
#include "Matrix.h"

namespace SVGShapes {

class Polygon final : public BaseShape{
public:
  ArrayList<Point> point_list;

  Polygon(Attribute *attrs, int attrs_count, BaseShape *parent);
  void render(Gdiplus::Graphics *graphics) const override;
};

};

#endif
