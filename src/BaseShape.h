#ifndef BASE_SHAPE_H
#define BASE_SHAPE_H

#include <objidl.h>
#include <gdiplus.h>

#include "utils.h"
#include "XMLNode.h"
#include "InverseIndex.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include "Matrix.h"

class IPaint {
public:
  virtual std::unique_ptr<IPaint> clone() const = 0; 
  virtual ~IPaint() = default;
  virtual std::unique_ptr<const Gdiplus::Brush> get_brush(double opacity) = 0;
};

class RGBPaint final : public IPaint {
public: 
  RGBPaint(double r, double g, double b);

  std::unique_ptr<IPaint> clone() const override;
  std::unique_ptr<const Gdiplus::Brush> get_brush(double opacity) override;

private:
  double r, g, b;
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

class BaseShape {
public:
  bool visible;

  std::unique_ptr<IPaint> fill;
  std::unique_ptr<IPaint> stroke;

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
  std::unique_ptr<const Gdiplus::Brush> fill_brush;
  std::unique_ptr<const Gdiplus::Brush> stroke_brush;
  std::unique_ptr<BaseShape> next;

  BaseShape(Attribute *attrs, int attrs_count, BaseShape *parent);
  virtual void render(Gdiplus::Graphics *graphics) const = 0;
  virtual ~BaseShape() = default;
};


#endif
