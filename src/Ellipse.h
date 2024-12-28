#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "BaseShape.h"

namespace SVGShapes {

class Ellipse final : public BaseShape {
public:
  Ellipse(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles);

  ArrayList<BezierCurve> get_beziers() const override;
private:
  Point c;
  double rx;
  double ry;
};

};

#endif
