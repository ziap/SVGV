#ifndef RECT_H
#define RECT_H

#include "BaseShape.h"  
#include "ArrayList.h"
#include <memory>

class Rect: public BaseShape{
public:
  double x;
  double y;
  double rx;
  double ry;
  double width;
  double height;
  
  Rect(Attribute *attrs, int attrs_count, BaseShape *parent);
  void draw() const;
};

#endif
