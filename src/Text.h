#ifndef TEXT_H
#define TEXT_H

#include "BaseShape.h"
#include "InverseIndex.h"

enum TextAnchor {
  TEXTANCHOR_START,
  TEXTANCHOR_MIDDLE,
  TEXTANCHOR_END,
  TEXTANCHOR_COUNT,
};

namespace SVGShapes {

class Text : public BaseShape {
public:
  std::string_view content;
  Point pos;
  Point d;
  TextAnchor text_anchor;

  Text(Attribute *attrs, int attrs_count, BaseShape *parent);
};

};

#endif
