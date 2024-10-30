#ifndef RECT_H
#define RECT_H

#include "BaseShape.h"	
#include "ArrayList.h"
#include <charconv>
#include <memory>

enum RectAttr{
	RECT_ATTR_X = 0,
	RECT_ATTR_Y,
	RECT_ATTR_RX,
	RECT_ATTR_RY,
	RECT_ATTR_WIDTH,
	RECT_ATTR_HEIGHT,
	RECT_ATTR_COUNT,
};

constexpr std::string_view rect_attr_name[RECT_ATTR_COUNT]{
	"x", 
	"y",
	"rx",
	"rx",
	"width",
	"height",
};

constexpr InverseIndex<RECT_ATTR_COUNT> inv_rect_attribute = {&rect_attr_name};

class Rect: public BaseShape{
public:
	double x;
	double y;
	double rx;
	double ry;
	double width;
	double height;
	
	Rect(Attribute *attrs, int attrs_count, BaseShape *parent);
	void draw();
};

#endif
