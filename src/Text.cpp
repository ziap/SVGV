#include "Text.h"
#include "InverseIndex.h"

using namespace SVGShapes;


constexpr std::string_view textanchor_name[TEXTANCHOR_COUNT] {
  "start",
  "middle",
  "end",
};

constexpr InverseIndex<TEXTANCHOR_COUNT> inv_textanchor {&textanchor_name};

enum TextAttr {
  TEXT_ATTR_X = 0,
  TEXT_ATTR_Y,
  TEXT_ATTR_DX,
  TEXT_ATTR_DY,
  TEXT_ATTR_ANCHOR,
  TEXT_ATTR_COUNT
};

constexpr std::string_view text_attr_name[TEXT_ATTR_COUNT] {
  "x",
  "y",
  "dx",
  "dy",
  "text-anchor",
};

constexpr InverseIndex<TEXT_ATTR_COUNT> inv_text_attribute {&text_attr_name};


Text::Text(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles) :
  BaseShape(attrs, attrs_count, parent, styles),
  content{""},
  pos{0, 0},
  d{0, 0} {
  for (int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
    
    switch ((TextAttr)inv_text_attribute[key]) {
      case TEXT_ATTR_X: {
        this->pos[0] = strtod(value.data(), nullptr);
      } break;
      case TEXT_ATTR_Y: {
        this->pos[1] = strtod(value.data(), nullptr);
      } break;
      case TEXT_ATTR_DX: {
        this->d[0] = strtod(value.data(), nullptr);
      } break;
      case TEXT_ATTR_DY: {
        this->d[1] = strtod(value.data(), nullptr);
      } break;
      case TEXT_ATTR_ANCHOR: {
        int type = inv_textanchor[value];
        if (type != -1) this->text_anchor = (TextAnchor)type;
      } break;

      case TEXT_ATTR_COUNT: {
        __builtin_unreachable();
      }
    }
  }
}

AABB Text::get_bounding() const {
  return { pos, pos + Point{1e6 + 1e6} };
}
