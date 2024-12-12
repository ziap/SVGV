#ifndef RADIALGRADIENT_H
#define RADIALGRADIENT_H

#include "Paint.h"
#include "Matrix.h"

struct RadialGradient {
  Point c;
  int r;
  Point fc;
  int fr;
  Paint to_paint() const;
};

#endif
