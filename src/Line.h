#ifndef LINE_H
#define LINE_H

#include "BaseShape.h"
#include "ArrayList.h"
#include <charconv>
#include <memory>

enum LineAttr{
  LINE_ATTR_X1 = 0,
  LINE_ATTR_Y1,
  LINE_ATTR_X2,
  LINE_ATTR_Y2,
  LINE_ATTR_COUNT,
};

constexpr std::string_view line_attr_name[LINE_ATTR_COUNT]{
  "x1", 
  "y1",
  "x2",
  "y2",
};

constexpr InverseIndex<LINE_ATTR_COUNT> inv_line_attribute= {&line_attr_name};

class Line : public BaseShape {
public:
  double x1;
  double y1;
  double x2;
  double y2;

  Line(Attribute *attrs, int attrs_count, BaseShape *parent);
  void draw() const;
};

#endif
