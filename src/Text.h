#ifndef TEXT_H
#define TEXT_H

#include "BaseShape.h"
#include "Point.h"

class Text : public BaseShape {
public:
  std::string_view content;
  Point pos;

  Text(Attribute *attrs, int attrs_count, BaseShape *parent);
  void draw() const override;
};

#endif
