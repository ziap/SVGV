#ifndef RECT_H
#define RECT_H

#include "BaseShape.h"

class Rect : public BaseShape {
public:
  Rect(Attribute *attrs, int attrs_count, BaseShape *parent);
};

#endif
