#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "BaseShape.h"
#include "ArrayList.h"
#include <charconv>
#include <memory>

enum EllipseAttr{
	ELLIPSE_ATTR_CX = 0,
	ELLIPSE_ATTR_CY,
	ELLIPSE_ATTR_RX,
	ELLIPSE_ATTR_RY,
	ELLIPSE_ATTR_COUNT,
};

constexpr std::string_view ellipse_attr_name[ELLIPSE_ATTR_COUNT]{
	"cx", 
	"cy",
	"rx",
	"ry",
};

constexpr InverseIndex<ELLIPSE_ATTR_COUNT> inv_ellipse_attribute= {&ellipse_attr_name};

class Ellipse : public BaseShape {
public:
	double cx;
	double cy;
	double rx;
	double ry;
  Ellipse(Attribute *attrs, int attrs_count, BaseShape *parent);
	void draw() const;
};

#endif
