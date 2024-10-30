#ifndef POLYGON_H
#define POLYGON_H

#include "BaseShape.h"	
#include "ArrayList.h"
#include "Path.h"
#include <charconv>
#include <memory>


class Polygon: public BaseShape{
public:
	ArrayList<Point> point_list;

	Polygon(Attribute *attrs, int attrs_count, BaseShape *parent);
	void draw();			
};


#endif
