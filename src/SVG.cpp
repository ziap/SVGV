#include "SVG.h"
#include "InverseIndex.h"
#include <cctype>

using namespace SVGShapes;


enum AttributeType {
  ATTRIBUTE_WIDTH = 0,
  ATTRIBUTE_HEIGHT,
  ATTRIBUTE_VIEWBOX,
  ATTRIBUTE_PRESERVE_ASPECT_RATIO,
  ATTRIBUTE_COUNT
};

constexpr std::string_view attribute_name[ATTRIBUTE_COUNT] = {
  "width",
  "height",
  "viewBox",
  "preserveAspectRatio"
};

constexpr InverseIndex<ATTRIBUTE_COUNT> inv_attribute = {&attribute_name};

constexpr std::string_view x_align_name[AXIS_ALIGN_COUNT] = {
  "xMin",
  "xMid",
  "xMax",
};
constexpr InverseIndex<AXIS_ALIGN_COUNT> inv_x_align = {&x_align_name};

constexpr std::string_view y_align_name[AXIS_ALIGN_COUNT] = {
  "YMin",
  "YMid",
  "YMax",
};
constexpr InverseIndex<AXIS_ALIGN_COUNT> inv_y_align = {&y_align_name};

SVG::SVG(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles) :
  BaseShape(attrs, attrs_count, parent, styles),
  width{0}, height{0}, 
  view_min{0, 0}, 
  view_width{0}, 
  view_height{0}, 
  align_type{ALIGN_NONE}, 
  align_x{AXIS_ALIGN_MIN}, 
  align_y{AXIS_ALIGN_MIN} {

  for (int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    switch ((AttributeType)inv_attribute[key]) {
      case ATTRIBUTE_WIDTH: {
        this->width = strtod(value.data(), nullptr);
      } break;
      case ATTRIBUTE_HEIGHT: {
        this->height = strtod(value.data(), nullptr);
      } break;
      case ATTRIBUTE_VIEWBOX: {
        {
          char *out;
          this->view_min[0] = strtod(value.data(), &out);
          value = value.substr(out - value.data());
        }
        while (value.size() && (isspace(value[0]) || value[0] == '%' || value[0] == ',')) {
          value = value.substr(1);
        }
        {
          char *out ;
          this->view_min[1] = strtod(value.data(), &out);
          value = value.substr(out - value.data());
        }
        while (value.size() && (isspace(value[0]) || value[0] == '%' || value[0] == ',')) {
          value = value.substr(1);
        }
        {
          char *out;
          this->view_width = strtod(value.data(), &out);
          value = value.substr(out - value.data());
        }
        while (value.size() && (isspace(value[0]) || value[0] == '%' || value[0] == ',')) {
          value = value.substr(1);
        }
        {
          char *out;
          this->view_height = strtod(value.data(), &out);
          value = value.substr(out - value.data());
        }
      } break;
      case ATTRIBUTE_PRESERVE_ASPECT_RATIO: {
        if (value == "none") {
          this->align_type = ALIGN_NONE;
        } else {
          this->align_x = (AxisAlignType)inv_x_align[value.substr(0, 4)];
          this->align_y = (AxisAlignType)inv_y_align[value.substr(4, 4)];

          this->align_type = ALIGN_MEET;
          value = value.substr(8);
          while (value.size() && isspace(value[0])) value = value.substr(1);
          if (value == "slice") this->align_type = ALIGN_SLICE;
        }
      } break;
      case ATTRIBUTE_COUNT: {
        __builtin_unreachable();
      }
    }
  }
}

AABB SVG::get_bounding() const {
  if (this->height != 0 || this->width != 0) {
    return AABB {
      Point{0, 0},
      Point{this->width, this->height},
    };
  }

  if (this->view_height != 0 || this->view_width != 0) {
    return AABB {
      view_min,
      view_min + Point{this->view_width, this->view_height},
    };
  }

  return AABB {
    Point{0, 0},
    Point{300, 300},
  };

}
