#ifndef TEXT_H
#define TEXT_H

#include "BaseShape.h"

namespace SVGShapes {

class Text : public BaseShape {
public:
  std::string_view content;
  Point pos;
  Point d;

  Text(Attribute *attrs, int attrs_count, BaseShape *parent);
};

};

#endif
