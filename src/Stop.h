#ifndef STOP_H
#define STOP_H

#include <string_view>
#include "common.h"
#include "Paint.h"

struct Stop {
  double offset;
  double stop_opacity;
  RGBPaint stop_color;
};

Stop read_stop(Attribute *attrs, int attribute_count);
#endif
