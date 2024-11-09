#ifndef TEXT_H
#define TEXT_H

#include "BaseShape.h"
#include "Point.h"

namespace SVGShapes {

class Text : public BaseShape {
public:
  std::string_view content;
  Point pos;

  Text(Attribute *attrs, int attrs_count, BaseShape *parent);
  void render(Gdiplus::Graphics *graphics) const override;
};

};

#endif
