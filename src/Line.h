#ifndef LINE_H
#define LINE_H

#include "BaseShape.h"
#include "Matrix.h"

namespace SVGShapes {

class Line final : public BaseShape {
public:
  Point p1;
  Point p2;

  Line(Attribute *attrs, int attrs_count, BaseShape *parent);
  void render(Gdiplus::Graphics *graphics) const override;
};

};

#endif
