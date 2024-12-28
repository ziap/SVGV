#ifndef POLYLINE_H
#define POLYLINE_H

#include "BaseShape.h"

namespace SVGShapes {

class Polyline final : public BaseShape {
public:
  Polyline(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles);

  ArrayList<BezierCurve> get_beziers() const override;
private:
  ArrayList<Point> point_list;
};

};

#endif
