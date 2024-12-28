#ifndef POLYGON_H
#define POLYGON_H

#include "BaseShape.h"  

namespace SVGShapes {

class Polygon final : public BaseShape{
public:
  Polygon(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles);

  virtual ArrayList<BezierCurve> get_beziers() const override;
private:
  ArrayList<Point> point_list;
};

};

#endif
