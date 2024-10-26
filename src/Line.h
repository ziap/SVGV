#ifndef LINE_H
#define LINE_H

#include "BaseShape.h"

class Line : public BaseShape {
public:
  Line(Attribute *attrs, int attrs_count, BaseShape *parent);
};

#endif
