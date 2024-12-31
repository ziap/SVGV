#include "Gradient.h"
#include "InverseIndex.h"

enum LinearGradientAttr {
  LINEAR_GRADIENT_ATTR_X1 = 0,
  LINEAR_GRADIENT_ATTR_X2,
  LINEAR_GRADIENT_ATTR_Y1,
  LINEAR_GRADIENT_ATTR_Y2,
  LINEAR_GRADIENT_ATTR_COUNT,
};


constexpr std::string_view linear_gradient_attr_name[LINEAR_GRADIENT_ATTR_COUNT] = {
  "x1", 
  "x2",
  "y1",
  "y2",
};

constexpr InverseIndex<LINEAR_GRADIENT_ATTR_COUNT> inv_linear_gradient_attribute {&linear_gradient_attr_name};

enum RadialGradientAttr {
  RADIAL_GRADIENT_ATTR_CX = 0,
  RADIAL_GRADIENT_ATTR_CY,
  RADIAL_GRADIENT_ATTR_R,
  RADIAL_GRADIENT_ATTR_FX,
  RADIAL_GRADIENT_ATTR_FY,
  RADIAL_GRADIENT_ATTR_FR,
  RADIAL_GRADIENT_ATTR_COUNT,
};


constexpr std::string_view radial_gradient_attr_name[RADIAL_GRADIENT_ATTR_COUNT] = {
  "cx", 
  "cy",
  "r",
  "fx",
  "fy",
  "fr",
};

constexpr InverseIndex<RADIAL_GRADIENT_ATTR_COUNT> inv_radial_gradient_attribute {&radial_gradient_attr_name};

enum GradientAttribute {
  GRADIENT_ATTR_ID = 0,
  GRADIENT_ATTR_HREF,
  GRADIENT_ATTR_SPREAD_METHOD,
  GRADIENT_ATTR_UNITS,
  GRADIENT_ATTR_TRANSFORM,
  GRADIENT_ATTR_COUNT,
};

constexpr std::string_view gradient_attrs_name[GRADIENT_ATTR_COUNT] = {
  "id", 
  "xlink:href",
  "spreadMethod",
  "gradientUnits",
  "gradientTransform",
};

constexpr InverseIndex<GRADIENT_ATTR_COUNT> inv_gradient_attribute {&gradient_attrs_name};

constexpr std::string_view spread_method_name[SPREAD_METHOD_COUNT] = {
  "pad",
  "reflect",
  "repeat",
};

constexpr InverseIndex<SPREAD_METHOD_COUNT> inv_spread_method {&spread_method_name};

static LinearGradient read_linear_gradient(Attribute *attrs, int attribute_count) {
  LinearGradient result;
  for (int i = 0; i < attribute_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    switch ((LinearGradientAttr)inv_linear_gradient_attribute[key]) {
      case LINEAR_GRADIENT_ATTR_X1: {
        result.x1 = read_percent_unit(value);
      } break;

      case LINEAR_GRADIENT_ATTR_X2: {
        result.x2 = read_percent_unit(value);
      } break;

      case LINEAR_GRADIENT_ATTR_Y1: {
        result.y1 = read_percent_unit(value);
      } break;

      case LINEAR_GRADIENT_ATTR_Y2: {
        result.y2 = read_percent_unit(value);
      } break;

      case LINEAR_GRADIENT_ATTR_COUNT: {
        __builtin_unreachable();
      }
    }
  }
  return result;
}


static RadialGradient read_radial_gradient(Attribute *attrs, int attribute_count) {
  RadialGradient result;

  result.fx = Optional<PercentUnit>::none();
  result.fy = Optional<PercentUnit>::none();
  result.fr.val = 0;
  result.fr.percent = false;
  result.cx.val = 50;
  result.cx.percent = true;
  result.cy.val = 50;
  result.cy.percent = true;

  
  for (int i = 0; i < attribute_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    switch ((RadialGradientAttr)inv_radial_gradient_attribute[key]) {
      case RADIAL_GRADIENT_ATTR_CX: {
        result.cx = read_percent_unit(value);
      } break;

      case RADIAL_GRADIENT_ATTR_CY: {
        result.cy = read_percent_unit(value);
      } break;

      case RADIAL_GRADIENT_ATTR_R: {
        result.r = read_percent_unit(value);
      } break;

      case RADIAL_GRADIENT_ATTR_FX: {
        result.fx = Optional<PercentUnit>::some(read_percent_unit(value));
      } break;

      case RADIAL_GRADIENT_ATTR_FY: {
        result.fy = Optional<PercentUnit>::some(read_percent_unit(value));
      } break;

      case RADIAL_GRADIENT_ATTR_FR: {
        result.fr = read_percent_unit(value);
      } break;
      case RADIAL_GRADIENT_ATTR_COUNT: {
        __builtin_unreachable();
      }
    }
  }

  return result;
}


Gradient read_gradient(GradientType type, Attribute *attrs, int attribute_count) {
  Gradient result;
  result.type = type;
  switch (type) {
    case GRADIENT_TYPE_LINEAR: {
      result.variants.linear = read_linear_gradient(attrs, attribute_count);
    } break;
    case GRADIENT_TYPE_RADIAL: {
      result.variants.radial = read_radial_gradient(attrs, attribute_count);
    } break;
    case GRADIENT_TYPE_COUNT: {
      __builtin_unreachable();
    };
  }

  result.transform = Transform::identity();
  result.gradient_units = GRADIENT_UNIT_OBJECT_BOUNDING_BOX;
  result.spread_method = SPREAD_METHOD_PAD;

  for (int i = 0; i < attribute_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
    switch ((GradientAttribute)inv_gradient_attribute[key]) {
      case GRADIENT_ATTR_ID: {
        result.id = value; 
      } break;

      case GRADIENT_ATTR_HREF: {
        result.href = value;
      } break;

      case GRADIENT_ATTR_SPREAD_METHOD: {
        int type = inv_spread_method[value];
        if (type != -1) result.spread_method = (SpreadMethod)type;
      } break;

      case GRADIENT_ATTR_UNITS: {
        if (value == "userSpaceOnUse") {
          result.gradient_units = GRADIENT_UNIT_USER_SPACE_ON_USE;
        } else {
          result.gradient_units = GRADIENT_UNIT_OBJECT_BOUNDING_BOX;
        }
      } break;

      case GRADIENT_ATTR_TRANSFORM: {
        result.transform = convert_transform(attrs[i].value);
      } break;

      case GRADIENT_ATTR_COUNT: {
        __builtin_unreachable();
      } break;
    }
  }
  return result;
}

