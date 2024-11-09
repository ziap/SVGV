#ifndef RECT_H
#define RECT_H

#include "BaseShape.h"  

namespace SVGShapes {

class Rect final : public BaseShape{
public:
  double x;
  double y;
  double rx;
  double ry;
  double width;
  double height;
  
  Rect(Attribute *attrs, int attrs_count, BaseShape *parent);
  void render(Gdiplus::Graphics *graphics) const override;
};

};

#endif
