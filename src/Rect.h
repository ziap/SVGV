#ifndef RECT_H
#define RECT_H

#include "BaseShape.h"  

namespace SVGShapes {

class Rect final : public BaseShape {
public:  
  Rect(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles);

  ArrayList<BezierCurve> get_beziers() const override;
private:
  double x;
  double y;
  double rx;
  double ry;
  double width;
  double height;
};

};

#endif
