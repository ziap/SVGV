#ifndef PAINT_H
#define PAINT_H

#include <string_view>

enum PaintType {
  PAINT_TRANSPARENT = 0,
  PAINT_RGB
};

struct RGBPaint {
  double r, g, b;
};

class Paint {
public:
  PaintType type;
  union {
    RGBPaint rgb_paint;
  } variants;

  static Paint new_transparent();
  static Paint new_rgb(double r, double g, double b);
};

Paint read_paint(std::string_view value);


#endif
