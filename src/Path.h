#ifndef PATH_H
#define PATH_H

#include "BaseShape.h"  

namespace SVGShapes {

class Path: public BaseShape{
public:
  Path(Attribute *attrs, int attrs_countt, BaseShape *parent, StyleSheet *styles);

  ArrayList<BezierCurve> get_beziers() const override;
private:
  ArrayList<BezierCurve> bezier_list;
};

};

#endif
