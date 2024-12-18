#include "Gradient.h"
#include "InverseIndex.h"
#include <iostream>

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

static LinearGradient read_linear_gradient(Attribute *attrs, int attribute_count) {
  LinearGradient result;
  for (int i = 0; i < attribute_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    switch ((LinearGradientAttr)inv_linear_gradient_attribute[key]) {
      case LINEAR_GRADIENT_ATTR_X1: {
        result.x[0] = strtod(value.data(), nullptr);
      } break;

      case LINEAR_GRADIENT_ATTR_X2: {
        result.x[1] = strtod(value.data(), nullptr);
      } break;

      case LINEAR_GRADIENT_ATTR_Y1: {
        result.y[0] = strtod(value.data(), nullptr);
      } break;

      case LINEAR_GRADIENT_ATTR_Y2: {
        result.y[1] = strtod(value.data(), nullptr);
      } break;

      case LINEAR_GRADIENT_ATTR_COUNT: {
        __builtin_unreachable();
      }
    }
  }
  return result;
}

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

enum GradientAttribute {
  GRADIENT_ATTR_ID = 0,
  GRADIENT_ATTR_UNITS,
  GRADIENT_ATTR_TRANSFORM,
  GRADIENT_ATTR_COUNT,
};

constexpr std::string_view gradient_attrs_name[GRADIENT_ATTR_COUNT] = {
  "id", 
  "gradientUnits",
  "gradientTransform",
};

constexpr InverseIndex<GRADIENT_ATTR_COUNT> inv_gradient_attribute {&gradient_attrs_name};

static RadialGradient read_radial_gradient(Attribute *attrs, int attribute_count) {
  RadialGradient result;

  result.fx = Optional<double>::none();
  result.fy = Optional<double>::none();
  result.fr = 0.0;
  result.c[0] = 0.5;
  result.c[1] = 0.5;
  for (int i = 0; i < attribute_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    switch ((RadialGradientAttr)inv_radial_gradient_attribute[key]) {
      case RADIAL_GRADIENT_ATTR_CX: {
        result.c[0] = strtod(value.data(), nullptr);
      } break;

      case RADIAL_GRADIENT_ATTR_CY: {
        result.c[1] = strtod(value.data(), nullptr);
      } break;

      case RADIAL_GRADIENT_ATTR_R: {
        result.r = strtod(value.data(), nullptr);
      } break;

      case RADIAL_GRADIENT_ATTR_FX: {
        result.fx.data = strtod(value.data(), nullptr);
      } break;

      case RADIAL_GRADIENT_ATTR_FY: {
        result.fy.data = strtod(value.data(), nullptr);
      } break;

      case RADIAL_GRADIENT_ATTR_FR: {
        result.fr = strtod(value.data(), nullptr);
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

  for (int i = 0; i < attribute_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
    switch ((GradientAttribute)inv_gradient_attribute[key]) {
      case GRADIENT_ATTR_ID: {
        result.id = value; 
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

