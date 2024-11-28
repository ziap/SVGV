#include "BaseShape.h"
#include "InverseIndex.h"
#include <cmath>
#include <cctype>



RGBPaint::RGBPaint(double r, double g, double b) : r{r}, g{g}, b{b} {}

std::unique_ptr<IPaint> RGBPaint::clone() const {
  return std::make_unique<RGBPaint>(r, g, b);
}

std::unique_ptr<const Gdiplus::Brush> RGBPaint::get_brush(double opacity){
  return std::make_unique<const Gdiplus::SolidBrush>(Gdiplus::Color{
    (BYTE)(opacity * 255), 
    (BYTE)(this->r * 255), 
    (BYTE)(this->g * 255), 
    (BYTE)(this->b * 255)
  });
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


enum ColorType {
  COLOR_ALICEBLUE = 0, 
  COLOR_ANTIQUEWHITE, 
  COLOR_AQUA, 
  COLOR_AQUAMARINE, 
  COLOR_AZURE, 
  COLOR_BEIGE, 
  COLOR_BISQUE, 
  COLOR_BLACK, 
  COLOR_BLANCHEDALMOND, 
  COLOR_BLUE, 
  COLOR_BLUEVIOLET, 
  COLOR_BROWN, 
  COLOR_BURLYWOOD, 
  COLOR_CADETBLUE, 
  COLOR_CHARTREUSE, 
  COLOR_CHOCOLATE, 
  COLOR_CORAL, 
  COLOR_CORNFLOWERBLUE, 
  COLOR_CORNSILK, 
  COLOR_CRIMSON, 
  COLOR_CYAN, 
  COLOR_DARKBLUE, 
  COLOR_DARKCYAN, 
  COLOR_DARKGOLDENROD, 
  COLOR_DARKGRAY, 
  COLOR_DARKGREY, 
  COLOR_DARKGREEN, 
  COLOR_DARKKHAKI, 
  COLOR_DARKMAGENTA, 
  COLOR_DARKOLIVEGREEN, 
  COLOR_DARKORANGE, 
  COLOR_DARKORCHID, 
  COLOR_DARKRED, 
  COLOR_DARKSALMON, 
  COLOR_DARKSEAGREEN, 
  COLOR_DARKSLATEBLUE, 
  COLOR_DARKSLATEGRAY, 
  COLOR_DARKSLATEGREY, 
  COLOR_DARKTURQUOISE, 
  COLOR_DARKVIOLET, 
  COLOR_DEEPPINK, 
  COLOR_DEEPSKYBLUE, 
  COLOR_DIMGRAY, 
  COLOR_DIMGREY, 
  COLOR_DODGERBLUE, 
  COLOR_FIREBRICK, 
  COLOR_FLORALWHITE, 
  COLOR_FORESTGREEN, 
  COLOR_FUCHSIA, 
  COLOR_GAINSBORO, 
  COLOR_GHOSTWHITE, 
  COLOR_GOLD, 
  COLOR_GOLDENROD, 
  COLOR_GRAY, 
  COLOR_GREY, 
  COLOR_GREEN, 
  COLOR_GREENYELLOW, 
  COLOR_HONEYDEW, 
  COLOR_HOTPINK, 
  COLOR_INDIANRED, 
  COLOR_INDIGO, 
  COLOR_IVORY, 
  COLOR_KHAKI, 
  COLOR_LAVENDER, 
  COLOR_LAVENDERBLUSH, 
  COLOR_LAWNGREEN, 
  COLOR_LEMONCHIFFON, 
  COLOR_LIGHTBLUE, 
  COLOR_LIGHTCORAL, 
  COLOR_LIGHTCYAN, 
  COLOR_LIGHTGOLDENRODYELLOW, 
  COLOR_LIGHTGRAY, 
  COLOR_LIGHTGREY, 
  COLOR_LIGHTGREEN, 
  COLOR_LIGHTPINK, 
  COLOR_LIGHTSALMON, 
  COLOR_LIGHTSEAGREEN, 
  COLOR_LIGHTSKYBLUE, 
  COLOR_LIGHTSLATEGRAY, 
  COLOR_LIGHTSLATEGREY, 
  COLOR_LIGHTSTEELBLUE, 
  COLOR_LIGHTYELLOW, 
  COLOR_LIME, 
  COLOR_LIMEGREEN, 
  COLOR_LINEN, 
  COLOR_MAGENTA, 
  COLOR_MAROON, 
  COLOR_MEDIUMAQUAMARINE, 
  COLOR_MEDIUMBLUE, 
  COLOR_MEDIUMORCHID, 
  COLOR_MEDIUMPURPLE, 
  COLOR_MEDIUMSEAGREEN, 
  COLOR_MEDIUMSLATEBLUE, 
  COLOR_MEDIUMSPRINGGREEN, 
  COLOR_MEDIUMTURQUOISE, 
  COLOR_MEDIUMVIOLETRED, 
  COLOR_MIDNIGHTBLUE, 
  COLOR_MINTCREAM, 
  COLOR_MISTYROSE, 
  COLOR_MOCCASIN, 
  COLOR_NAVAJOWHITE, 
  COLOR_NAVY, 
  COLOR_OLDLACE, 
  COLOR_OLIVE, 
  COLOR_OLIVEDRAB, 
  COLOR_ORANGE, 
  COLOR_ORANGERED, 
  COLOR_ORCHID, 
  COLOR_PALEGOLDENROD, 
  COLOR_PALEGREEN, 
  COLOR_PALETURQUOISE, 
  COLOR_PALEVIOLETRED, 
  COLOR_PAPAYAWHIP, 
  COLOR_PEACHPUFF, 
  COLOR_PERU, 
  COLOR_PINK, 
  COLOR_PLUM, 
  COLOR_POWDERBLUE, 
  COLOR_PURPLE, 
  COLOR_REBECCAPURPLE, 
  COLOR_RED, 
  COLOR_ROSYBROWN, 
  COLOR_ROYALBLUE, 
  COLOR_SADDLEBROWN, 
  COLOR_SALMON, 
  COLOR_SANDYBROWN, 
  COLOR_SEAGREEN, 
  COLOR_SEASHELL, 
  COLOR_SIENNA, 
  COLOR_SILVER, 
  COLOR_SKYBLUE, 
  COLOR_SLATEBLUE, 
  COLOR_SLATEGRAY, 
  COLOR_SLATEGREY, 
  COLOR_SNOW, 
  COLOR_SPRINGGREEN, 
  COLOR_STEELBLUE, 
  COLOR_TAN, 
  COLOR_TEAL, 
  COLOR_THISTLE, 
  COLOR_TOMATO, 
  COLOR_TURQUOISE, 
  COLOR_VIOLET, 
  COLOR_WHEAT, 
  COLOR_WHITE, 
  COLOR_WHITESMOKE, 
  COLOR_YELLOW, 
  COLOR_YELLOWGREEN, 
  COLOR_COUNT,
};

constexpr std::string_view color_name[COLOR_COUNT] = {
  "aliceblue", 
  "antiquewhite", 
  "aqua", 
  "aquamarine", 
  "azure", 
  "beige", 
  "bisque", 
  "black", 
  "blanchedalmond", 
  "blue", 
  "blueviolet", 
  "brown", 
  "burlywood", 
  "cadetblue", 
  "chartreuse", 
  "chocolate", 
  "coral", 
  "cornflowerblue", 
  "cornsilk", 
  "crimson", 
  "cyan", 
  "darkblue", 
  "darkcyan", 
  "darkgoldenrod", 
  "darkgray", 
  "darkgrey", 
  "darkgreen", 
  "darkkhaki", 
  "darkmagenta", 
  "darkolivegreen", 
  "darkorange", 
  "darkorchid", 
  "darkred", 
  "darksalmon", 
  "darkseagreen", 
  "darkslateblue", 
  "darkslategray", 
  "darkslategrey", 
  "darkturquoise", 
  "darkviolet", 
  "deeppink", 
  "deepskyblue", 
  "dimgray", 
  "dimgrey", 
  "dodgerblue", 
  "firebrick", 
  "floralwhite", 
  "forestgreen", 
  "fuchsia", 
  "gainsboro", 
  "ghostwhite", 
  "gold", 
  "goldenrod", 
  "gray", 
  "grey", 
  "green", 
  "greenyellow", 
  "honeydew", 
  "hotpink", 
  "indianred ", 
  "indigo ", 
  "ivory", 
  "khaki", 
  "lavender", 
  "lavenderblush", 
  "lawngreen", 
  "lemonchiffon", 
  "lightblue", 
  "lightcoral", 
  "lightcyan", 
  "lightgoldenrodyellow", 
  "lightgray", 
  "lightgrey", 
  "lightgreen", 
  "lightpink", 
  "lightsalmon", 
  "lightseagreen", 
  "lightskyblue", 
  "lightslategray", 
  "lightslategrey", 
  "lightsteelblue", 
  "lightyellow", 
  "lime", 
  "limegreen", 
  "linen", 
  "magenta", 
  "maroon", 
  "mediumaquamarine", 
  "mediumblue", 
  "mediumorchid", 
  "mediumpurple", 
  "mediumseagreen", 
  "mediumslateblue", 
  "mediumspringgreen", 
  "mediumturquoise", 
  "mediumvioletred", 
  "midnightblue", 
  "mintcream", 
  "mistyrose", 
  "moccasin", 
  "navajowhite", 
  "navy", 
  "oldlace", 
  "olive", 
  "olivedrab", 
  "orange", 
  "orangered", 
  "orchid", 
  "palegoldenrod", 
  "palegreen", 
  "paleturquoise", 
  "palevioletred", 
  "papayawhip", 
  "peachpuff", 
  "peru", 
  "pink", 
  "plum", 
  "powderblue", 
  "purple", 
  "rebeccapurple", 
  "red", 
  "rosybrown", 
  "royalblue", 
  "saddlebrown", 
  "salmon", 
  "sandybrown", 
  "seagreen", 
  "seashell", 
  "sienna", 
  "silver", 
  "skyblue", 
  "slateblue", 
  "slategray", 
  "slategrey", 
  "snow", 
  "springgreen", 
  "steelblue", 
  "tan", 
  "teal", 
  "thistle", 
  "tomato", 
  "turquoise", 
  "violet", 
  "wheat", 
  "white", 
  "whitesmoke", 
  "yellow", 
  "yellowgreen", 
};

constexpr InverseIndex<COLOR_COUNT> inv_color = {&color_name};

std::string_view hex_color[COLOR_COUNT] = {
  "#F0F8FF", "#FAEBD7", "#00FFFF", "#7FFFD4", "#F0FFFF", "#F5F5DC", "#FFE4C4", "#000000", "#FFEBCD", 
  "#0000FF", "#8A2BE2", "#A52A2A", "#DEB887", "#5F9EA0", "#7FFF00", "#D2691E", "#FF7F50", "#6495ED", 
  "#FFF8DC", "#DC143C", "#00FFFF", "#00008B", "#008B8B", "#B8860B", "#A9A9A9", "#A9A9A9", "#006400", 
  "#BDB76B", "#8B008B", "#556B2F", "#FF8C00", "#9932CC", "#8B0000", "#E9967A", "#8FBC8F", "#483D8B", 
  "#2F4F4F", "#2F4F4F", "#00CED1", "#9400D3", "#FF1493", "#00BFFF", "#696969", "#696969", "#1E90FF", 
  "#B22222", "#FFFAF0", "#228B22", "#FF00FF", "#DCDCDC", "#F8F8FF", "#FFD700", "#DAA520", "#808080", 
  "#808080", "#008000", "#ADFF2F", "#F0FFF0", "#FF69B4", "#CD5C5C", "#4B0082", "#FFFFF0", "#F0E68C", 
  "#E6E6FA", "#FFF0F5", "#7CFC00", "#FFFACD", "#ADD8E6", "#F08080", "#E0FFFF", "#FAFAD2", "#D3D3D3", 
  "#D3D3D3", "#90EE90", "#FFB6C1", "#FFA07A", "#20B2AA", "#87CEFA", "#778899", "#778899", "#B0C4DE", 
  "#FFFFE0", "#00FF00", "#32CD32", "#32CD32", "#FF00FF", "#800000", "#66CDAA", "#0000CD", "#BA55D3", 
  "#9370DB", "#3CB371", "#7B68EE", "#00FA9A", "#48D1CC", "#C71585", "#191970", "#F5FFFA", "#FFE4E1", 
  "#FFE4B5", "#FFDEAD", "#000080", "#FDF5E6", "#808000", "#6B8E23", "#FFA500", "#FF4500", "#DA70D6", 
  "#EEE8AA", "#98FB98", "#AFEEEE", "#DB7093", "#FFEFD5", "#FFDAB9", "#CD853F", "#FFC0CB", "#DDA0DD", 
  "#B0E0E6", "#800080", "#663399", "#FF0000", "#BC8F8F", "#4169E1", "#8B4513", "#FA8072", "#F4A460", 
  "#2E8B57", "#FFF5EE", "#A0522D", "#C0C0C0", "#87CEEB", "#6A5ACD", "#708090", "#708090", "#FFFAFA", 
  "#00FF7F", "#4682B4", "#D2B48C", "#008080", "#D8BFD8", "#FF6347", "#40E0D0", "#EE82EE", "#F5DEB3", 
  "#FFFFFF", "#F5F5F5", "#FFFF00", "#9ACD32", 
};


static std::string_view trim_start(std::string_view sv) {
  while (sv.size() && (isspace(sv[0]) || sv[0] == ',')) sv = sv.substr(1);
  return sv;
}

static std::unique_ptr<IPaint> read_color_hex(std::string_view value) {
  double r, g, b; 
  value = value.substr(1);

  uint32_t p = 0;
  p = strtoul(value.data(), nullptr, 16);

  b = p & 0xFF;
  g = (p >> 8) & 0xFF;
  r = (p >> 16) & 0xFF;

  r /= 255;
  g /= 255;
  b /= 255;

  return (std::make_unique<RGBPaint> (r, g, b));
}

static std::unique_ptr<IPaint> read_color_text(std::string_view value) {
  if (inv_color[value] == -1) return nullptr;
  std::string_view color_hex = hex_color[inv_color[value]]; 
  return read_color_hex(color_hex);
}

static std::unique_ptr<IPaint> read_RGB(std::string_view value) {
  double r, g, b;
  int start = value.find('(');
  int end = value.find(',');
  r = strtod(value.data() + start + 1, nullptr);

  value = trim_start(value.substr(end + 1));

  end = value.find(',');
  g = strtod(value.data(), nullptr);

  value = trim_start(value.substr(end + 1));

  b = strtod(value.data(), nullptr);

  r/= 255;
  g/=255;
  b/=255;
  return (std::make_unique<RGBPaint> (r, g, b));
}

static std::unique_ptr<IPaint> read_paint(std::string_view value) {
  if (value[0] == '#') return read_color_hex(value);
  if (value[0] == 'r' && value[1] == 'g' && value[2] == 'b') return read_RGB(value);
  if (value == "none") return nullptr;
  return read_color_text(value);
} 

static double convert_opacity(std::string_view value) {
  double opacity = strtod(value.data(), nullptr);;
  if (value[value.size() - 1] == '%') {
    opacity /= 100;
  }

  return opacity;
}

static void convert_array(std::string_view value, float *a, int *count) {
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
  Transform transform = Transform::identity();
  transform.m[0][0] = arr.a[0]; 
  transform.m[1][0] = arr.a[1]; 
  transform.m[0][1] = arr.a[2]; 
  transform.m[1][1] = arr.a[3];
  transform.d[0] = arr.a[4], 
  transform.d[1] = arr.a[5];

  return transform;
}

static Transform scale(Array arr) {
  Transform transform = Transform::identity();
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
  Transform transform = Transform::identity();
  double angle = arr.a[0] * PI / 180;
  transform.m[0][0] = std::cos(angle); 
  transform.m[0][1] = -std::sin(angle);
  transform.m[1][0] = std::sin(angle);
  transform.m[1][1] = std::cos(angle);

 
  if (arr.n > 1) {
    Array translate_arr;
    translate_arr.a[0] = arr.a[1]; translate_arr.a[1] = arr.a[2]; translate_arr.n = arr.n - 1;
    Transform translate1 = Transform::identity();
    translate1 = translate(translate_arr);
    transform = translate1 * transform;

    translate1 = Transform::identity();
    translate_arr.a[0] = -arr.a[1]; translate_arr.a[1] = -arr.a[2];
    translate1 = translate(translate_arr);
    transform = transform * translate1;
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



static Transform solve_transform(std::string_view inf) {
  int end = inf.find('(');
  std::string_view str_type = inf.substr(0, end);

  inf = inf.substr(end + 1);

  Array arr = split_number(inf);
  
  Transform transform = Transform::identity();

  switch ((TransformType)inv_transform[str_type]){
    case TRANSFORM_MATRIX: {
      transform = matrix(arr);
    } break;

    case TRANSFORM_TRANSLATE:{
      transform = translate(arr);
    } break;

    case TRANSFORM_SCALE: {
      transform = scale(arr);
    } break;

    case TRANSFORM_ROTATE: {
      transform = rotate(arr);
    } break;

    case TRANSFORM_SKEWX: {
      transform = skewX(arr);
    } break;

    case TRANSFORM_SKEWY: {
      transform = skewY(arr);
    } break;

    case TRANSFORM_COUNT: {
      __builtin_unreachable();
    }
  }

  return transform;
}

static Transform convert_transform(std::string_view value) {
  Transform tmp = Transform::identity();
  Transform transform = Transform::identity();
  
  int start = 0, end = 0;
  while (value.size() > 0) {
    value = trim_start(value);
    end = value.find(")");
    std::string_view type = value.substr(start, end - start);
    tmp = solve_transform(type);
    transform = transform * tmp;

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
    this->fill = std::make_unique<RGBPaint>(0, 0, 0);
    this->stroke = nullptr;
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
    if (parent->fill != nullptr)
      this->fill = (parent->fill)->clone();
    if (parent->stroke != nullptr)
      this->stroke = (parent->stroke)->clone();
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

  for(int i = 0; i < attrs_count; i++) {
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

  if (this->fill) {
    this->fill_brush = this->fill->get_brush(this->fill_opacity);
  }

  if (this->stroke) {
    this->stroke_brush = this->stroke->get_brush(this->stroke_opacity);
  }

}
