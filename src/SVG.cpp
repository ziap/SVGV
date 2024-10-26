#include "SVG.h"
#include <cctype>
#include <charconv>

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

SVG::SVG(Attribute *attrs, int attrs_count, BaseShape *parent)
  : BaseShape(attrs, attrs_count, parent) {
  this->width = 0;
  this->height = 0;

  this->view_min_x = 0;
  this->view_min_y = 0;
  this->view_width = 0;
  this->view_height = 0;

  this->align_type = ALIGN_NONE;
  this->align_x = AXIS_ALIGN_MIN;
  this->align_y = AXIS_ALIGN_MIN;

  for (int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    AttributeType type = (AttributeType)inv_attribute[key];
    switch (type) {
      case ATTRIBUTE_WIDTH: {
        std::from_chars(value.data(), value.data() + value.size(), this->width);
      } break;
      case ATTRIBUTE_HEIGHT: {
        std::from_chars(value.data(), value.data() + value.size(), this->height);
      } break;
      case ATTRIBUTE_VIEWBOX: {
        {
          std::from_chars_result res = std::from_chars(value.data(), value.data() + value.size(), this->view_min_x);
          value = value.substr(res.ptr - value.data());
        }
        while (value.size() && (isspace(value[0]) || value[0] == '%' || value[0] == ',')) {
          value = value.substr(1);
        }
        {
          std::from_chars_result res = std::from_chars(value.data(), value.data() + value.size(), this->view_min_y);
          value = value.substr(res.ptr - value.data());
        }
        while (value.size() && (isspace(value[0]) || value[0] == '%' || value[0] == ',')) {
          value = value.substr(1);
        }
        {
          std::from_chars_result res = std::from_chars(value.data(), value.data() + value.size(), this->view_width);
          value = value.substr(res.ptr - value.data());
        }
        while (value.size() && (isspace(value[0]) || value[0] == '%' || value[0] == ',')) {
          value = value.substr(1);
        }
        {
          std::from_chars_result res = std::from_chars(value.data(), value.data() + value.size(), this->view_height);
          value = value.substr(res.ptr - value.data());
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
      } break;
    }
  }
}
