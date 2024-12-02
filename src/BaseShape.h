#ifndef BASE_SHAPE_H
#define BASE_SHAPE_H

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>

#include "utils.h"
#include "ArrayList.h"
#include "InverseIndex.h"
#include "Matrix.h"

class Attribute {
public:
  std::string_view key;
  std::string_view value;
};

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

enum StrokeLineJoin {
  LINE_JOIN_ARCS = 0,
  LINE_JOIN_BEVEL,
  LINE_JOIN_MITER,
  LINE_JOIN_MITER_CLIP,
  LINE_JOIN_ROUND,
  LINE_JOIN_COUNT,
};

enum StrokeLineCap {
  LINE_CAP_BUTT = 0,
  LINE_CAP_ROUND,
  LINE_CAP_SQUARE,
  LINE_CAP_COUNT,
};

enum FillRule {
  FILL_RULE_NONZERO = 0,
  FILL_RULE_EVENODD,
  FILL_RULE_COUNT
};

class BezierCurve {
public:
  Point start;
  Point end;
  Point control_start;
  Point control_end;
};

class BaseShape {
public:
  BaseShape(Attribute *attrs, int attrs_count, BaseShape *parent);
  virtual ~BaseShape() = default;

  bool visible;

  Paint fill;
  Paint stroke;

  double font_size;

  double opacity;
  double fill_opacity;
  double stroke_opacity;

  double stroke_width;
  float stroke_dash_offset;
  float stroke_dash_array[8];
  int stroke_dash_count;

  StrokeLineJoin stroke_line_join;
  StrokeLineCap stroke_line_cap;

  double miter_limit;
  FillRule fill_rule;

  Transform transform;
  std::unique_ptr<BaseShape> next;

  virtual ArrayList<BezierCurve> get_beziers() const;
};


#endif
