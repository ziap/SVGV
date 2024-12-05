#include "BaseShape.h"
#include "InverseIndex.h"
#include <cmath>
#include <cctype>
#include <cstdint>

Paint Paint::new_transparent() {
  Paint paint;
  paint.type = PAINT_TRANSPARENT;
  return paint;
}

Paint Paint::new_rgb(double r, double g, double b) {
  Paint paint;
  paint.type = PAINT_RGB;
  paint.variants.rgb_paint = RGBPaint {
    std::clamp(r, 0.0, 1.0),
    std::clamp(g, 0.0, 1.0),
    std::clamp(b, 0.0, 1.0),
  };

  return paint;
}

constexpr std::string_view fillrule_name[FILL_RULE_COUNT] {
  "nonzero",
  "evenodd",
};

constexpr InverseIndex<FILL_RULE_COUNT> inv_fillrule = {&fillrule_name};

constexpr std::string_view linecap_name[LINE_CAP_COUNT] {
  "butt",
  "round",
  "square",
};

constexpr InverseIndex<LINE_CAP_COUNT> inv_linecap = {&linecap_name};

constexpr std::string_view linejoin_name[LINE_JOIN_COUNT] {
  "arcs",
  "bevel",
  "miter",
  "miter-clip",
  "round",
};

constexpr InverseIndex<LINE_JOIN_COUNT> inv_linejoin = {&linejoin_name};

enum AttributeType {
  ATTRIBUTE_VISIBLE = 0,
  ATTRIBUTE_FILL,
  ATTRIBUTE_STROKE,
  ATTRIBUTE_FONT_SIZE,
  ATTRIBUTE_OPACITY,
  ATTRIBUTE_FILL_OPACITY,
  ATTRIBUTE_STROKE_OPACITY,
  ATTRIBUTE_STROKE_WIDTH,
  ATTRIBUTE_STROKE_DASH_OFFSET,
  ATTRIBUTE_STROKE_DASH_ARRAY,
  ATTRIBUTE_STROKE_LINE_JOIN,
  ATTRIBUTE_STROKE_LINE_CAP,
  ATTRIBUTE_MITER_LIMIT,
  ATTRIBUTE_FILL_RULE,
  ATTRIBUTE_TRANSFORM,
  ATTRIBUTE_STYLE, 
  ATTRIBUTE_COUNT,
};

constexpr std::string_view attribute_name[ATTRIBUTE_COUNT] = {
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
  "transform",
  "style",
};

constexpr InverseIndex<ATTRIBUTE_COUNT> inv_attribute = {&attribute_name};

constexpr std::string_view color_name[] = {
  "aliceblue",       "antiquewhite",      "aqua",                 "aquamarine",
  "azure",           "beige",             "bisque",               "black",
  "blanchedalmond",  "blue",              "blueviolet",           "brown",
  "burlywood",       "cadetblue",         "chartreuse",           "chocolate",
  "coral",           "cornflowerblue",    "cornsilk",             "crimson",
  "cyan",            "darkblue",          "darkcyan",             "darkgoldenrod",
  "darkgray",        "darkgrey",          "darkgreen",            "darkkhaki",
  "darkmagenta",     "darkolivegreen",    "darkorange",           "darkorchid",
  "darkred",         "darksalmon",        "darkseagreen",         "darkslateblue",
  "darkslategray",   "darkslategrey",     "darkturquoise",        "darkviolet",
  "deeppink",        "deepskyblue",       "dimgray",              "dimgrey",
  "dodgerblue",      "firebrick",         "floralwhite",          "forestgreen",
  "fuchsia",         "gainsboro",         "ghostwhite",           "gold",
  "goldenrod",       "gray",              "grey",                 "green",
  "greenyellow",     "honeydew",          "hotpink",              "indianred ",
  "indigo ",         "ivory",             "khaki",                "lavender",
  "lavenderblush",   "lawngreen",         "lemonchiffon",         "lightblue",
  "lightcoral",      "lightcyan",         "lightgoldenrodyellow", "lightgray",
  "lightgrey",       "lightgreen",        "lightpink",            "lightsalmon",
  "lightseagreen",   "lightskyblue",      "lightslategray",       "lightslategrey",
  "lightsteelblue",  "lightyellow",       "lime",                 "limegreen",
  "linen",           "magenta",           "maroon",               "mediumaquamarine",
  "mediumblue",      "mediumorchid",      "mediumpurple",         "mediumseagreen",
  "mediumslateblue", "mediumspringgreen", "mediumturquoise",      "mediumvioletred",
  "midnightblue",    "mintcream",         "mistyrose",            "moccasin",
  "navajowhite",     "navy",              "oldlace",              "olive",
  "olivedrab",       "orange",            "orangered",            "orchid",
  "palegoldenrod",   "palegreen",         "paleturquoise",        "palevioletred",
  "papayawhip",      "peachpuff",         "peru",                 "pink",
  "plum",            "powderblue",        "purple",               "rebeccapurple",
  "red",             "rosybrown",         "royalblue",            "saddlebrown",
  "salmon",          "sandybrown",        "seagreen",             "seashell",
  "sienna",          "silver",            "skyblue",              "slateblue",
  "slategray",       "slategrey",         "snow",                 "springgreen",
  "steelblue",       "tan",               "teal",                 "thistle",
  "tomato",          "turquoise",         "violet",               "wheat",
  "white",           "whitesmoke",        "yellow",               "yellowgreen",
};

constexpr size_t COLOR_COUNT = sizeof(color_name) / sizeof(color_name[0]);

constexpr InverseIndex<COLOR_COUNT> inv_color = {&color_name};

static constexpr RGBPaint hex_u32(uint32_t hex) {
  double b = hex & 0xFF;
  double g = (hex >> 8) & 0xFF;
  double r = (hex >> 16) & 0xFF;

  return RGBPaint {r / 255, g / 255, b / 255};
}

constexpr RGBPaint hex_color[COLOR_COUNT] = {
    hex_u32(0xF0F8FF), hex_u32(0xFAEBD7), hex_u32(0x00FFFF), hex_u32(0x7FFFD4),
    hex_u32(0xF0FFFF), hex_u32(0xF5F5DC), hex_u32(0xFFE4C4), hex_u32(0x000000),
    hex_u32(0xFFEBCD), hex_u32(0x0000FF), hex_u32(0x8A2BE2), hex_u32(0xA52A2A),
    hex_u32(0xDEB887), hex_u32(0x5F9EA0), hex_u32(0x7FFF00), hex_u32(0xD2691E),
    hex_u32(0xFF7F50), hex_u32(0x6495ED), hex_u32(0xFFF8DC), hex_u32(0xDC143C),
    hex_u32(0x00FFFF), hex_u32(0x00008B), hex_u32(0x008B8B), hex_u32(0xB8860B),
    hex_u32(0xA9A9A9), hex_u32(0xA9A9A9), hex_u32(0x006400), hex_u32(0xBDB76B),
    hex_u32(0x8B008B), hex_u32(0x556B2F), hex_u32(0xFF8C00), hex_u32(0x9932CC),
    hex_u32(0x8B0000), hex_u32(0xE9967A), hex_u32(0x8FBC8F), hex_u32(0x483D8B),
    hex_u32(0x2F4F4F), hex_u32(0x2F4F4F), hex_u32(0x00CED1), hex_u32(0x9400D3),
    hex_u32(0xFF1493), hex_u32(0x00BFFF), hex_u32(0x696969), hex_u32(0x696969),
    hex_u32(0x1E90FF), hex_u32(0xB22222), hex_u32(0xFFFAF0), hex_u32(0x228B22),
    hex_u32(0xFF00FF), hex_u32(0xDCDCDC), hex_u32(0xF8F8FF), hex_u32(0xFFD700),
    hex_u32(0xDAA520), hex_u32(0x808080), hex_u32(0x808080), hex_u32(0x008000),
    hex_u32(0xADFF2F), hex_u32(0xF0FFF0), hex_u32(0xFF69B4), hex_u32(0xCD5C5C),
    hex_u32(0x4B0082), hex_u32(0xFFFFF0), hex_u32(0xF0E68C), hex_u32(0xE6E6FA),
    hex_u32(0xFFF0F5), hex_u32(0x7CFC00), hex_u32(0xFFFACD), hex_u32(0xADD8E6),
    hex_u32(0xF08080), hex_u32(0xE0FFFF), hex_u32(0xFAFAD2), hex_u32(0xD3D3D3),
    hex_u32(0xD3D3D3), hex_u32(0x90EE90), hex_u32(0xFFB6C1), hex_u32(0xFFA07A),
    hex_u32(0x20B2AA), hex_u32(0x87CEFA), hex_u32(0x778899), hex_u32(0x778899),
    hex_u32(0xB0C4DE), hex_u32(0xFFFFE0), hex_u32(0x00FF00), hex_u32(0x32CD32),
    hex_u32(0x32CD32), hex_u32(0xFF00FF), hex_u32(0x800000), hex_u32(0x66CDAA),
    hex_u32(0x0000CD), hex_u32(0xBA55D3), hex_u32(0x9370DB), hex_u32(0x3CB371),
    hex_u32(0x7B68EE), hex_u32(0x00FA9A), hex_u32(0x48D1CC), hex_u32(0xC71585),
    hex_u32(0x191970), hex_u32(0xF5FFFA), hex_u32(0xFFE4E1), hex_u32(0xFFE4B5),
    hex_u32(0xFFDEAD), hex_u32(0x000080), hex_u32(0xFDF5E6), hex_u32(0x808000),
    hex_u32(0x6B8E23), hex_u32(0xFFA500), hex_u32(0xFF4500), hex_u32(0xDA70D6),
    hex_u32(0xEEE8AA), hex_u32(0x98FB98), hex_u32(0xAFEEEE), hex_u32(0xDB7093),
    hex_u32(0xFFEFD5), hex_u32(0xFFDAB9), hex_u32(0xCD853F), hex_u32(0xFFC0CB),
    hex_u32(0xDDA0DD), hex_u32(0xB0E0E6), hex_u32(0x800080), hex_u32(0x663399),
    hex_u32(0xFF0000), hex_u32(0xBC8F8F), hex_u32(0x4169E1), hex_u32(0x8B4513),
    hex_u32(0xFA8072), hex_u32(0xF4A460), hex_u32(0x2E8B57), hex_u32(0xFFF5EE),
    hex_u32(0xA0522D), hex_u32(0xC0C0C0), hex_u32(0x87CEEB), hex_u32(0x6A5ACD),
    hex_u32(0x708090), hex_u32(0x708090), hex_u32(0xFFFAFA), hex_u32(0x00FF7F),
    hex_u32(0x4682B4), hex_u32(0xD2B48C), hex_u32(0x008080), hex_u32(0xD8BFD8),
    hex_u32(0xFF6347), hex_u32(0x40E0D0), hex_u32(0xEE82EE), hex_u32(0xF5DEB3),
    hex_u32(0xFFFFFF), hex_u32(0xF5F5F5), hex_u32(0xFFFF00), hex_u32(0x9ACD32),
};

static std::string_view trim_start(std::string_view sv) {
  while (sv.size() && (isspace(sv[0]) || sv[0] == ',')) sv = sv.substr(1);
  return sv;
}

static Paint read_color_hex(std::string_view value) {
  value = value.substr(1);
  uint32_t p = strtoul(value.data(), nullptr, 16);

  if (value.size() == 3) {
    uint32_t b = p & 0xf;
    uint32_t g = p & 0xf0;
    uint32_t r = p & 0xf00;
    p = b | (b << 4) | (g << 4) | (g << 8) | (r << 8) | (r << 12);
  }
  
  Paint paint;
  paint.type = PAINT_RGB;
  paint.variants.rgb_paint = hex_u32(p);
  return paint;
}

static Paint read_color_text(std::string_view value) {
  if (inv_color[value] == -1) return Paint::new_transparent();

  Paint paint;
  paint.type = PAINT_RGB;
  paint.variants.rgb_paint = hex_color[inv_color[value]];
  return paint;
}

static Paint read_RGB(std::string_view value) {
  int start = value.find('(');
  int end = value.find(',');
  double r = strtod(value.data() + start + 1, nullptr);

  value = trim_start(value.substr(end + 1));

  end = value.find(',');
  double g = strtod(value.data(), nullptr);

  value = trim_start(value.substr(end + 1));

  double b = strtod(value.data(), nullptr);

  r /= 255;
  g /= 255;
  b /= 255;
  return Paint::new_rgb(r, g, b);
}

static Paint read_paint(std::string_view value) {
  if (value[0] == '#') return read_color_hex(value);
  if (value[0] == 'r' && value[1] == 'g' && value[2] == 'b') return read_RGB(value);
  if (value == "none") return Paint::new_transparent();
  return read_color_text(value);
} 

static double convert_opacity(std::string_view value) {
  double opacity = strtod(value.data(), nullptr);
  if (value[value.size() - 1] == '%') {
    opacity /= 100;
  }

  return opacity;
}

static void convert_array(std::string_view value, double *a, int *count) {
  while (value.size() > 0 && (*count) < 8) {
    value = trim_start(value);
    char *end;
    a[*count] = strtod(value.data(), &end);
    if (end != value.data()) ++*count;
    value = value.substr(end - value.data());
  }
}

enum TransformType {
  TRANSFORM_MATRIX = 0,
  TRANSFORM_TRANSLATE,
  TRANSFORM_SCALE,
  TRANSFORM_ROTATE,
  TRANSFORM_SKEWX,
  TRANSFORM_SKEWY,
  TRANSFORM_COUNT,
};

constexpr std::string_view transform_name[TRANSFORM_COUNT] = {
  "matrix",
  "translate",
  "scale",
  "rotate",
  "skewX",
  "skewY",
};

constexpr InverseIndex<TRANSFORM_COUNT> inv_transform = {&transform_name};

struct Array {
  double a[6];
  int n;
};

static Array split_number(std::string_view inf) {
  Array arr;
  arr.n = 0;
  while (inf.size() > 0) {
    inf = trim_start(inf);
    while(isspace(inf[0]) != (inf[0] == ',')) inf = inf.substr(1);
    char *out;
    arr.a[arr.n] = strtod(inf.data(), &out);
    inf = inf.substr(out - inf.data());
    arr.n++;
  }

  return arr;
} 

static Transform translate(Array arr) {
  Transform transform = Transform::identity();
  if (arr.n == 2) {
    transform.d[0] = arr.a[0];
    transform.d[1] = arr.a[1];
  } else {
    transform.d[0] = arr.a[0];
  }

  return transform;
}

static Transform matrix(Array arr) {
  Transform transform;
  transform.m[0][0] = arr.a[0]; 
  transform.m[1][0] = arr.a[1]; 
  transform.m[0][1] = arr.a[2]; 
  transform.m[1][1] = arr.a[3];
  transform.d[0] = arr.a[4], 
  transform.d[1] = arr.a[5];

  return transform;
}

static Transform scale(Array arr) {
  Transform transform = Transform::zeros();
  if (arr.n == 2) {
    transform.m[0][0] = arr.a[0];
    transform.m[1][1] = arr.a[1];
  } else {
    transform.m[0][0] = arr.a[0];
    transform.m[1][1] = arr.a[0];
  }

  return transform;
}

static Transform rotate(Array arr) {
  Transform transform;
  double angle = arr.a[0] * PI / 180;
  transform.m[0][0] = std::cos(angle); 
  transform.m[0][1] = -std::sin(angle);
  transform.m[1][0] = std::sin(angle);
  transform.m[1][1] = std::cos(angle);
  transform.d[0] = 0;
  transform.d[1] = 0;
 
  if (arr.n > 1) {
    Array translate_arr;
    translate_arr.a[0] = arr.a[1];
    translate_arr.a[1] = arr.a[2];
    translate_arr.n = arr.n - 1;
    transform = translate(translate_arr) * transform;

    translate_arr.a[0] = -arr.a[1];
    translate_arr.a[1] = -arr.a[2];
    transform = transform * translate(translate_arr);
  }

  return transform;
}

static Transform skewX(Array arr) {
  Transform transform = Transform::identity();
  transform.m[0][1] = std::tan(arr.a[0] * PI / 180);

  return transform;  
}

static Transform skewY(Array arr) {
  Transform transform = Transform::identity();
  transform.m[1][0] = std::tan(arr.a[0] * PI / 180);

  return transform;
}

Transform (*transform_fns[TRANSFORM_COUNT])(Array arr) = {
  matrix,
  translate,
  scale,
  rotate,
  skewX,
  skewY,
};

static Transform solve_transform(std::string_view inf) {
  int end = inf.find('(');
  std::string_view str_type = inf.substr(0, end);

  inf = inf.substr(end + 1);

  int type = inv_transform[str_type];
  if (type >= 0 && type < TRANSFORM_COUNT) {
    return transform_fns[type](split_number(inf));
  }

  return Transform::identity();
}

static Transform convert_transform(std::string_view value) {
  Transform transform = Transform::identity();
  
  while (value.size() > 0) {
    value = trim_start(value);
    int end = value.find(')');
    std::string_view type = value.substr(0, end);
    transform = transform * solve_transform(type);

    value = value.substr(end + 1);
  }
  return transform;
}

enum StyleType {
  STYLE_FILL = 0,
  STYLE_STROKE,
  STYLE_STROKE_WIDTH,
  STYLE_FONT_SIZE,
  STYLE_COUNT,
};

constexpr std::string_view style_name[STYLE_COUNT] = {
  "fill",
  "stroke",
  "stroke-width",
};

constexpr InverseIndex<STYLE_COUNT> inv_style = {&style_name};

static void solve_style(std::string_view value, BaseShape *shape) {
  size_t end = value.find(':');
  std::string_view key = value.substr(0, end);

  value = trim_start(value.substr(end + 1));

  switch ((StyleType)inv_style[key]) {
    case STYLE_FILL: {
      shape->fill = read_paint(value);
    } break;
    case STYLE_STROKE: {
      shape->stroke = read_paint(value);
    } break;
    case STYLE_FONT_SIZE: {
      shape->font_size = strtod(value.data(), nullptr);
    } break;
    case STYLE_STROKE_WIDTH: {
      shape->stroke_width = strtod(value.data(), nullptr);
    } break;
    case STYLE_COUNT: {
      __builtin_unreachable();
    } break;
  }
}


BaseShape::BaseShape(Attribute *attrs, int attrs_count, BaseShape *parent) {
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
  }

  for (int i = 0; i < attrs_count; i++) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
    
    switch ((AttributeType)inv_attribute[key]) {
      case ATTRIBUTE_VISIBLE: {
        if (value != "visible") this->visible = false;
      } break;

      case ATTRIBUTE_FILL: {
        this->fill = read_paint(value);
      } break;

      case ATTRIBUTE_STROKE: {
        this->stroke = read_paint(value);
      } break;

      case ATTRIBUTE_FONT_SIZE: {
        this->font_size = strtod(value.data(), nullptr);
      } break;

      case ATTRIBUTE_OPACITY: {
        this->opacity = convert_opacity(value);
      } break;

      case ATTRIBUTE_FILL_OPACITY: {
        this->fill_opacity = convert_opacity(value);
      } break;

      case ATTRIBUTE_STROKE_OPACITY: {
        this->stroke_opacity = convert_opacity(value);
      } break;

      case ATTRIBUTE_STROKE_WIDTH: {
        this->stroke_width = strtod(value.data(), nullptr);
      } break;

      case ATTRIBUTE_STROKE_DASH_OFFSET: {
        this->stroke_dash_offset = strtod(value.data(), nullptr);
      } break;

      case ATTRIBUTE_STROKE_DASH_ARRAY: {
        convert_array(value, this->stroke_dash_array, &this->stroke_dash_count);
      } break;

      case ATTRIBUTE_STROKE_LINE_JOIN: {
        this->stroke_line_join = (StrokeLineJoin)inv_linejoin[value];
      } break;

      case ATTRIBUTE_STROKE_LINE_CAP: {
        this->stroke_line_cap = (StrokeLineCap)inv_linecap[value];
      } break;

      case ATTRIBUTE_MITER_LIMIT: {
        this->miter_limit = strtod(value.data(), nullptr);
      } break;

      case ATTRIBUTE_FILL_RULE: {
        this->fill_rule = (FillRule)inv_fillrule[value];
      } break;
      
      case ATTRIBUTE_TRANSFORM: {
        this->transform = this->transform * convert_transform(value);
      } break;

      case ATTRIBUTE_STYLE: {
        while (value.size() > 0) {
          value = trim_start(value);
          size_t end = value.find(';');
          if (end > value.size()) end = value.size(); 
          std::string_view str = value.substr(0, end);
          solve_style(str, this);
          if (end != value.size()) value = value.substr(end + 1);
          else value = value.substr(end);
        }
      } break;

      case ATTRIBUTE_COUNT: {
        __builtin_unreachable();
      }
    }
  }
}

ArrayList<BezierCurve> BaseShape::get_beziers() const {
  return ArrayList<BezierCurve> {};
}
