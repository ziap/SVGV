#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "BaseShape.h"

namespace SVGShapes {

class Ellipse final : public BaseShape {
public:
  double cx;
  double cy;
  double rx;
  double ry;

  Ellipse(Attribute *attrs, int attrs_count, BaseShape *parent);
  void render(Gdiplus::Graphics *graphics) const override;
};

};

#endif
