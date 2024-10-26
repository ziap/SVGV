#include "BaseShape.h"

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

class SVG : public BaseShape {
public:
  double width;
  double height;

  double view_min_x;
  double view_min_y;

  double view_width;
  double view_height;

  AlignType align_type;
  AxisAlignType align_x;
  AxisAlignType align_y;

  SVG(Attribute *attrs, int attrs_count, BaseShape *parent);
};
