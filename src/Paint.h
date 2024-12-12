#ifndef PAINT_H
#define PAINT_H

#include <string_view>

enum PaintType {
  PAINT_TRANSPARENT = 0,
  PAINT_RGB,
  PAINT_URL,
};

struct RGBPaint {
  double r, g, b;
};


struct UrlPaint {
  const char *data;
  int len;
};

class Paint {
public:
  PaintType type;
  union {
    RGBPaint rgb_paint;
    UrlPaint url_paint;
  } variants;

  static Paint new_transparent();
  static Paint new_rgb(double r, double g, double b);
  static Paint new_url(std::string_view value);
};

Paint read_paint(std::string_view value);


#endif
