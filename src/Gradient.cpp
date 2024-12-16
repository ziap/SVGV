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


constexpr std::string_view linear_gradient_attr_name[LINEAR_GRADIENT_ATTR_COUNT] {
  "x1", 
  "x2",
  "y1",
  "y2",
};

constexpr InverseIndex<LINEAR_GRADIENT_ATTR_COUNT> inv_linear_gradient_attribute = {&linear_gradient_attr_name};

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


constexpr std::string_view radial_gradient_attr_name[RADIAL_GRADIENT_ATTR_COUNT] {
  "cx", 
  "cy",
  "r",
  "fx",
  "fy",
  "fr",
};

constexpr InverseIndex<RADIAL_GRADIENT_ATTR_COUNT> inv_radial_gradient_attribute = {&radial_gradient_attr_name};

static RadialGradient read_radial_gradient(Attribute *attrs, int attribute_count) {
  RadialGradient result;

  result.fx = Optional<double>::none();
  result.fy = Optional<double>::none();
  result.fr = 0.0;
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

static std::string_view read_id(Attribute *attrs, int attribute_count) {
  for (int i = 0; i < attribute_count; ++i) {
    if (attrs[i].key == "id") {
      return attrs[i].value;
    }
  }
  return "";
}

static GradientUnits read_gradient_unit(Attribute *attrs, int attribute_count) {
  for (int i = 0; i < attribute_count; ++i) {
    if (attrs[i].key == "gradientUnits") {
      if (attrs[i].value == "userSpaceOnUse") {
        return GRADIENT_UNIT_USER_SPACE_ON_USE;
      } else if (attrs[i].value == "objectBoundingBox") {
        return GRADIENT_UNIT_OBJECT_BOUNDING_BOX;
      }
    }
  }
  
  return GRADIENT_UNIT_OBJECT_BOUNDING_BOX;
}

static Transform read_transform(Attribute* attrs, int attribute_count){
<<<<<<< HEAD
  Transform result = Transform::identity();
=======
  Transform result;
>>>>>>> 9b96703d96e03c2ad8ac18dd93892a7610fec99c
  for (int i = 0; i < attribute_count; ++i) {
    if (attrs[i].key == "gradientTransform") {
      result = convert_transform(attrs[i].value);
    }
  }
  return result;
}


Gradient read_gradient(GradientType type, Attribute *attrs, int attribute_count) {
  Gradient result;
  result.type = type;
  result.id = read_id(attrs, attribute_count);
  result.gradient_units = read_gradient_unit(attrs, attribute_count);
  result.transform = read_transform(attrs, attribute_count);

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

  return result;
}

