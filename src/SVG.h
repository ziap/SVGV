#ifndef SVG_H
#define SVG_H
#include "BaseShape.h"

namespace SVGShapes {

enum AlignType {
  ALIGN_NONE = 0,
  ALIGN_MEET,
  ALIGN_SLICE,
  ALIGN_COUNT
};

enum AxisAlignType {
  AXIS_ALIGN_MIN = 0,
  AXIS_ALIGN_MID,
  AXIS_ALIGN_MAX,
  AXIS_ALIGN_COUNT
};

class SVG final : public BaseShape {
public:
  double width;
  double height;

  Point view_min;

  double view_width;
  double view_height;

  AlignType align_type;
  AxisAlignType align_x;
  AxisAlignType align_y;

  SVG(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles);

  AABB get_bounding() const override;
};

};
#endif
