#ifndef LINE_H
#define LINE_H

#include "BaseShape.h"

namespace SVGShapes {

class Line final : public BaseShape {
public:
  Line(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles);

  ArrayList<BezierCurve> get_beziers() const override;
private:
  Point p1;
  Point p2;
};

};

#endif
