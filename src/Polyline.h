#ifndef POLYLINE_H
#define POLYLINE_H

#include "BaseShape.h"
#include "ArrayList.h"
#include "Path.h"
#include <charconv>
#include <memory>

class Polyline : public BaseShape {
public:
	ArrayList<Point> point_list;

  Polyline(Attribute *attrs, int attrs_count, BaseShape *parent);
	void draw() const;
};

#endif
