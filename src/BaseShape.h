#ifndef BASE_SHAPE_H
#define BASE_SHAPE_H

#include "utils.h"
#include "XMLNode.h"

class Paint {
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

  Optional<Paint> fill;
  Optional<Paint> stroke;

  double opacity;
  double fill_opacity;
  double stroke_opacity;

  double stroke_width;
  float stroke_dash_offset;
  float stroke_dash_array[8];
  int stroke_dash_count;

  StrokeLineJoin stroke_line_join;
  StrokeLineCap stroke_line_cap;

  double mitter_limit;
  FillRule fill_rule;

  double transform[2][3];

  void read_xml_node(XMLNode *node);
};

/*
class Rectangle : public BaseShape {
  void read_xml_node(XMLNode *node) {
    BaseShape::read_xml_node(node);
  }
};
*/

#endif
