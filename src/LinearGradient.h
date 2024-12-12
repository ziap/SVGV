#ifndef LINEARGRADIENT_H
#define LINEARGRADIENT_H

#include <string_view>
#include "Matrix.h"
#include "Stop.h"

struct LinearGradient{
public:
  Point x;
  Point y;
  Paint to_paint();
};

#endif
