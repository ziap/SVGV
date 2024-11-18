#ifndef PATH_H
#define PATH_H

#include "BaseShape.h"  
#include "ArrayList.h"
#include "Matrix.h"

namespace SVGShapes {


class BezierCurve {
public:
  Point point_0;       // point start
  Point point_N;      // point end
  Point point_CS;     // point control start
  Point point_CE;     //point control end
};

class Path: public BaseShape{
public:
  ArrayList<BezierCurve> bezier_list;
  Path(Attribute *attrs, int attrs_countt, BaseShape *parent);
  void render(Gdiplus::Graphics *graphics) const override;
};

};

#endif
