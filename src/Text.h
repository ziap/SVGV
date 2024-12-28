#ifndef TEXT_H
#define TEXT_H

#include "BaseShape.h"
#include "InverseIndex.h"
#include <string>

enum TextAnchor {
  TEXTANCHOR_START,
  TEXTANCHOR_MIDDLE,
  TEXTANCHOR_END,
  TEXTANCHOR_COUNT,
};

namespace SVGShapes {

class Text : public BaseShape {
public:
  std::string content;
  Point pos;
  Point d;
  TextAnchor text_anchor;

  Text(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles);
  AABB get_bounding() const override;

  void set_text(std::string_view text);
};

};

#endif
