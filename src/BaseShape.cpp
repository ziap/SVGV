#include "BaseShape.h"
#include "InverseIndex.h"
#include <cmath>
#include <cctype>
#include "common.h"
#include "Transform.h"


struct QuadraticRoots {
  double roots[2];
  int roots_len;
};

static QuadraticRoots find_solution_quadratic(double p0, double p1, double p2, double p3) {
  QuadraticRoots result;

  double a = 9 * (p1 - p2) + 3 * (p3 - p0);
  double b = 6 * (p0 + p2 - 2 * p1);
  double c = 3 * (p1 - p0);
        
  double delta= b * b - 4 * a * c;

  if (a == 0) {
    if (b != 0) {
      result.roots[0] = -c / b;
      result.roots_len = 1; 
    } else {
      if (c == 0) {
        result.roots_len = -1;
      } else {
        result.roots_len = 0;
      }
    }
  } else {
    if (delta < 0) {
      result.roots_len = 0;
    } else if (delta == 0){
      result.roots[0] = (-b + std::sqrt(delta)) / (2 * a);
      result.roots_len = 1; 
    } else {
      result.roots[0] = (-b + std::sqrt(delta)) / (2 * a);
      result.roots[1] = (-b - std::sqrt(delta)) / (2 * a);
      result.roots_len = 2; 
    }
  }

  return result;
}

AABB BaseShape::get_bounding() const {
  ArrayList<BezierCurve> beziers = this->get_beziers();

  AABB size;
  size.min[0] = std::numeric_limits<double>::max();
  size.min[1] = std::numeric_limits<double>::max();
  size.max[0] = std::numeric_limits<double>::min();
  size.max[1] = std::numeric_limits<double>::min();
          
  for (size_t i = 0; i < beziers.len(); i++) {
    for (int j = 0; j < 2; j++) {

      size.min[j] = std::min(size.min[j], std::min(beziers[i].start[j], beziers[i].end[j]));
      size.max[j] = std::max(size.max[j], std::max(beziers[i].start[j], beziers[i].end[j]));

      double p0 = beziers[i].start[j];
      double p1 = beziers[i].control_start[j];
      double p2 = beziers[i].control_end[j];
      double p3 = beziers[i].end[j];

      QuadraticRoots result = find_solution_quadratic(p0, p1, p2, p3);
      if (result.roots_len == -1) { 
        double t = 0.5;
        if (t <= 1 && t >= -1) {
          double ans = p0 + 3 * (p1 - p0) * t + 3 * (p0 + p2 - 2 * p1) * t * t
                      + (3 * (p1 - p2) + p3 - p0) * t * t * t;

          size.min[j] = std::min(size.min[j], ans);
          size.max[j] = std::max(size.max[j], ans);
        }
      }

      for (int k = 0; k < result.roots_len; k++) {
        double t = result.roots[k];
        if (t <= 1 && t >= -1) {
          double ans = p0 + 3 * (p1 - p0) * t + 3 * (p0 + p2 - 2 * p1) * t * t
                      + (3 * (p1 - p2) + p3 - p0) * t * t * t;

          size.min[j] = std::min(size.min[j], ans);
          size.max[j] = std::max(size.max[j], ans);
        }
      }
    }
  }

  return size;
}
    

enum FontWeight {
  FONTWEIGHT_NORMAL = 0,
  FONTWEIGHT_BOLD,
  FONTWEIGHT_BOLDER,
  FONTWEIGHT_LIGHTER,
  FONTWEIGHT_COUNT,
};

constexpr std::string_view fontweight_name[FONTWEIGHT_COUNT] = {
  "normal",
  "bold",
  "bolder",
  "lighter",
};

constexpr InverseIndex<FONTWEIGHT_COUNT> inv_fontweight{&fontweight_name};

constexpr std::string_view fillrule_name[FILL_RULE_COUNT] = {
  "nonzero",
  "evenodd",
};

constexpr InverseIndex<FILL_RULE_COUNT> inv_fillrule{&fillrule_name};

constexpr std::string_view linecap_name[LINE_CAP_COUNT] = {
  "butt",
  "round",
  "square",
};

constexpr InverseIndex<LINE_CAP_COUNT> inv_linecap{&linecap_name};

constexpr std::string_view linejoin_name[LINE_JOIN_COUNT] = {
  "arcs",
  "bevel",
  "miter",
  "miter-clip",
  "round",
};

constexpr InverseIndex<LINE_JOIN_COUNT> inv_linejoin{&linejoin_name};

constexpr std::string_view fontstyle_name[FONTSTYLE_COUNT] = {
  "normal",
  "italic",
  "oblique",
};

constexpr InverseIndex<FONTSTYLE_COUNT> inv_fontstyle{&fontstyle_name};


enum StyleType {
  STYLE_VISIBLE = 0,
  STYLE_FILL,
  STYLE_STROKE,
  STYLE_FONT_SIZE,
  STYLE_OPACITY,
  STYLE_FILL_OPACITY,
  STYLE_STROKE_OPACITY,
  STYLE_STROKE_WIDTH,
  STYLE_STROKE_DASH_OFFSET,
  STYLE_STROKE_DASH_ARRAY,
  STYLE_STROKE_LINE_JOIN,
  STYLE_STROKE_LINE_CAP,
  STYLE_MITER_LIMIT,
  STYLE_FILL_RULE,
  STYLE_FONT_STYLE,
  STYLE_FONT_WEIGHT,
  STYLE_FONT_FAMILY,
  STYLE_COUNT,
};

constexpr std::string_view style_name[STYLE_COUNT] = {
  "visibility",
  "fill",
  "stroke",
  "font-size",
  "opacity",
  "fill-opacity",
  "stroke-opacity",
  "stroke-width",
  "stroke-dashoffset",
  "stroke-dasharray",
  "stroke-linejoin",
  "stroke-linecap",
  "stroke-miterlimit",
  "fill-rule",
  "font-style",
  "font-weight",
  "font-family",
};

constexpr InverseIndex<STYLE_COUNT> inv_style = {&style_name};


enum AttributeType {
  ATTRIBUTE_TRANSFORM = 0,
  ATTRIBUTE_STYLE, 
  ATTRIBUTE_XML_SPACE,
  ATTRIBUTE_COUNT,
};

constexpr std::string_view attribute_name[ATTRIBUTE_COUNT] = {
  "transform",
  "style",
  "xml:space",
};

constexpr InverseIndex<ATTRIBUTE_COUNT> inv_attribute{&attribute_name};

static void apply_style(BaseShape *shape, BaseShape *parent, Attribute *attrs, int attrs_count) {
  for (int i = 0; i < attrs_count; i++) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    switch ((StyleType) inv_style[key]) {
      case STYLE_VISIBLE: {
        if (value != "visible") shape->visible = false;
      } break;

      case STYLE_FILL: {
        shape->fill = read_paint(value);
      } break;

      case STYLE_STROKE: {
        if (value != "") shape->stroke = read_paint(value);
      } break;

      case STYLE_FONT_SIZE: {
        shape->font_size = strtod(value.data(), nullptr);
      } break;

      case STYLE_OPACITY: {
        shape->opacity = convert_percent(value);
      } break;

      case STYLE_FILL_OPACITY: {
        shape->fill_opacity = convert_percent(value);
      } break;

      case STYLE_STROKE_OPACITY: {
        shape->stroke_opacity = convert_percent(value);
      } break;

      case STYLE_STROKE_WIDTH: {
        shape->stroke_width = strtod(value.data(), nullptr);
      } break;

      case STYLE_STROKE_DASH_OFFSET: {
        shape->stroke_dash_offset = strtod(value.data(), nullptr);
      } break;

      case STYLE_STROKE_DASH_ARRAY: {
        if (value != "none")
          convert_array(value, shape->stroke_dash_array, &shape->stroke_dash_count);
      } break;

      case STYLE_STROKE_LINE_JOIN: {
        int type = inv_linejoin[value];
        if (type != -1) shape->stroke_line_join = (StrokeLineJoin) type;
      } break;

      case STYLE_STROKE_LINE_CAP: {
        int type = inv_linecap[value];
        if (type != -1) shape->stroke_line_cap = (StrokeLineCap)type;
      } break;

      case STYLE_MITER_LIMIT: {
        shape->miter_limit = strtod(value.data(), nullptr);
      } break;

      case STYLE_FILL_RULE: {
        int type = inv_fillrule[value];
        if (type != -1) shape->fill_rule = (FillRule)type;
      } break;

      case STYLE_FONT_STYLE: {
        int type = inv_fontstyle[value];
        if (type != - 1) shape->font_style = (FontStyle)type;
      } break;

      case STYLE_FONT_WEIGHT: {
        int type = inv_fontweight[value];
        if (type == -1) {
          shape->font_weight = strtod(value.data(), nullptr);
        }  else {
          switch ((FontWeight)type) {
            case FONTWEIGHT_NORMAL: {
              shape->font_weight = 400;
            } break;
            case FONTWEIGHT_BOLD: {
              shape->font_weight = 700;
            } break;
            case FONTWEIGHT_BOLDER: {
              if (parent == nullptr) {
                shape->font_weight = 700;
              } else {
                if (parent->font_weight < 100) {
                  shape->font_weight = 100;
                } else if (parent->font_weight < 400) {
                  shape->font_weight = 400;
                } else if (parent->font_weight < 700) {
                  shape->font_weight = 700;
                } else if (parent->font_weight < 900){
                  shape->font_weight = 900;
                } else {
                  shape->font_weight = parent->font_weight;
                }
              } 
            } break;
            case FONTWEIGHT_LIGHTER: {
              if (parent == nullptr) {
                shape->font_weight = 100;
              } else {
                if (parent->font_weight > 900) {
                  shape->font_weight = 900;
                } else if (parent->font_weight > 700) {
                  shape->font_weight = 700;
                } else if (parent->font_weight > 400) {
                  shape->font_weight = 400;
                } else if (parent->font_weight > 100){
                  shape->font_weight = 100;
                } else {
                  shape->font_weight = parent->font_weight;
                }
              } 
            } break;
            case FONTWEIGHT_COUNT: {
              __builtin_unreachable();
            }
          }
        } 
      } break;
      
      case STYLE_FONT_FAMILY: {
        shape->font_family = value;
      } break;

      case STYLE_COUNT: {
        __builtin_unreachable();
      }
    }
  }
}

BaseShape::BaseShape(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles) {
  if (parent == nullptr) {
    this->visible = true;
    this->fill = Paint::new_rgb(0, 0, 0);
    this->stroke = Paint::new_transparent();
    this->font_size = 16;
    this->opacity = 1.0;
    this->fill_opacity = 1.0;
    this->stroke_opacity = 1.0;
    this->stroke_width = 1.0;
    this->stroke_dash_offset = 0.0;
    this->stroke_dash_count = 0;
    this->stroke_line_join = StrokeLineJoin::LINE_JOIN_MITER;
    this->stroke_line_cap = StrokeLineCap::LINE_CAP_BUTT;
    this->miter_limit = 4;
    this->transform = Transform::identity();
    this->fill_rule = FillRule::FILL_RULE_NONZERO;
    this->font_style = FontStyle::FONTSTYLE_NORMAL;
    this->font_weight = 400;
    this->font_family = "serif";
    this->xml_space = true;
  } else {
    this->visible = parent->visible;
    this->fill = parent->fill;
    this->stroke = parent->stroke;
    this->font_size = parent->font_size;
    this->opacity = parent->opacity;
    this->fill_opacity = parent->fill_opacity;
    this->stroke_opacity = parent->stroke_opacity;
    this->stroke_width = parent->stroke_width;
    this->stroke_dash_offset = parent->stroke_dash_offset;
    this->stroke_dash_count = parent->stroke_dash_count;
    this->stroke_line_join = parent->stroke_line_join;
    this->stroke_line_cap = parent->stroke_line_cap;
    this->miter_limit = parent->miter_limit;
    this->transform = parent->transform;
    this->fill_rule = parent->fill_rule;
    this->font_style = parent->font_style;
    this->font_weight = parent->font_weight;
    this->font_family = parent->font_family;
    this->xml_space = parent->xml_space;
  }

  for (int i = 0; i < attrs_count; i++) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    if (key == "class") {
      value = trim_start(value);
      value = trim_end(value);

      StyleSheet::iterator it = styles->find(value);
      if (it != styles->end()) {
        ArrayList<Attribute> *attr = &it->second;
        apply_style(this, parent, attr->begin(), attr->len());
      }
    }
  }

  for (int i = 0; i < attrs_count; i++) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
  
    switch ((AttributeType)inv_attribute[key]) {
      case ATTRIBUTE_TRANSFORM: {
        this->transform = this->transform * convert_transform(value);
      } break;

      case ATTRIBUTE_STYLE: {
        ArrayList<Attribute> attrs_style = process_style(value);
        apply_style(this, parent, attrs_style.begin(), attrs_style.len());
      } break;

      case ATTRIBUTE_XML_SPACE: {
        if(value == "preserve") this->xml_space = false;
      } break;

      case ATTRIBUTE_COUNT: {
        __builtin_unreachable();
      }
    }
  }

  apply_style(this, parent, attrs, attrs_count);
}

ArrayList<BezierCurve> BaseShape::get_beziers() const {
  return ArrayList<BezierCurve> {};
}
