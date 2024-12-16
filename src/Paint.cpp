#include "Paint.h"

#include "InverseIndex.h"
#include <algorithm>
#include <cctype>

static std::string_view trim_start(std::string_view sv) {
  while (sv.size() && (isspace(sv[0]) || sv[0] == ',')) sv = sv.substr(1);
  return sv;
}

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

Paint Paint::new_url(std::string_view value) {
  Paint paint;
  paint.type = PAINT_URL;

  paint.variants.url_paint.data = value.data();
  paint.variants.url_paint.len = value.size();

  return paint;
}

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
  "greenyellow",     "honeydew",          "hotpink",              "indianred",
  "indigo",          "ivory",             "khaki",                "lavender",
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

constexpr int max_size(const std::string_view *a, int n) {
  int max = a[0].size();
  for (int i = 1; i < n; i++) {
    max = std::max(max, (int)a[i].size());
  }

  return max;
}

constexpr int color_max_size = max_size(color_name, COLOR_COUNT);

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

static Paint read_color_hex(std::string_view value) {
  value = value.substr(1);

  char result[color_max_size + 1];
  int len = std::min((int)value.size(), color_max_size + 1);
  for (int i = 0; i < len; i++) {
    result[i] = tolower(value[i]);
  }
  result[len] = '\0';
  
  uint32_t p = strtoul(result, nullptr, 16);

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
  char result[color_max_size + 1];
  int len = std::min((int)value.size(), color_max_size + 1);
  for (int i = 0; i < len; i++) {
    result[i] = tolower(value[i]);
  }

  result[len] = '\0';

  int type = inv_color[result];
  if (type == -1) return Paint::new_transparent();
  
  Paint paint;
  paint.type = PAINT_RGB;
  paint.variants.rgb_paint = hex_color[type];
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

static Paint read_URL(std::string_view value) {
  value = value.substr(4);
  value = value.substr(0, value.size() - 1);
  return Paint::new_url(value);
}

Paint read_paint(std::string_view value) {
  if (value[0] == '#') return read_color_hex(value);
  if (value.substr(0, 3) == "rgb") return read_RGB(value);
  if (value.substr(0, 3) == "url") return read_URL(value);
  if (value == "none") return Paint::new_transparent();
  return read_color_text(value);
} 
