#ifndef BASE_SHAPE_H
#define BASE_SHAPE_H

#include "common.h"
#include "ArrayList.h"
#include "utils.h"
#include "Matrix.h"
#include "Paint.h"

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

enum FontStyle {
  FONTSTYLE_NORMAL = 0,
  FONTSTYLE_ITALIC,
  FONTSTYLE_OBLIQUE,
  FONTSTYLE_COUNT,
};

class BezierCurve {
public:
  Point start;
  Point end;
  Point control_start;
  Point control_end;
};


struct AABB {
  Point min;
  Point max;
};

class BaseShape {
public:
  BaseShape(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles);
  virtual ~BaseShape() = default;

  bool visible;

  Paint fill;
  Paint stroke;

  double font_size;
  FontStyle font_style;
  int font_weight;
  std::string_view font_family;

  double opacity;
  double fill_opacity;
  double stroke_opacity;

  double stroke_width;
  double stroke_dash_offset;
  double stroke_dash_array[8];
  int stroke_dash_count;

  StrokeLineJoin stroke_line_join;
  StrokeLineCap stroke_line_cap;

  double miter_limit;
  FillRule fill_rule;

  bool xml_space;

  Transform transform;
  std::unique_ptr<BaseShape> next;

  virtual ArrayList<BezierCurve> get_beziers() const;

  virtual AABB get_bounding() const;
};


#endif
