#ifndef CIRCLE_H
#define CIRCLE_H

#include "BaseShape.h"

namespace SVGShapes {

class Circle final : public BaseShape {
public:
  Circle(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles);

  ArrayList<BezierCurve> get_beziers() const override;
private:
  Point c;
  double r;
};

}

#endif
