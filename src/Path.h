#ifndef PATH_H
#define PATH_H

#include "BaseShape.h"

class Path : public BaseShape {
public:
  Path(Attribute *attrs, int attrs_count, BaseShape *parent);
};

#endif
