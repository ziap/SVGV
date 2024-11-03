#ifndef TEXT_H
#define TEXT_H

#include "BaseShape.h"
#include "ArrayList.h"
#include <charconv>
#include <memory>

enum LengthAdjust{
	LENGTH_ADJUST_SPACING = 0,
	LENGTH_ADJUST_SPACING_AND_GLYPHS,
	LENGTH_ADJUST_COUNT,
};


constexpr std::string_view length_adjust_name[LENGTH_ADJUST_COUNT]{
	"spacing"
	"spacingAndGlyphs"
};

enum TextAttr{
	TEXT_ATTR_X = 0,	
	TEXT_ATTR_Y,	
	TEXT_ATTR_DX,	
	TEXT_ATTR_DY,	
	TEXT_ATTR_TEXT_LENGTH,	
	TEXT_ATTR_COUNT,	
};


constexpr std::string_view text_attr_name[TEXT_ATTR_COUNT]{
	"x", 
	"y",
	"dx",
	"dy",
	"textLength",
};

constexpr InverseIndex<LENGTH_ADJUST_COUNT> inv_length_adjust_attribute= {&length_adjust_name};

constexpr InverseIndex<TEXT_ATTR_COUNT> inv_text_attribute= {&text_attr_name};

class Text: public BaseShape {
public:
	double x;
	double y;
	double dx;
	double dy;
	int text_length;
	LengthAdjust len_adjust;
  Text(Attribute *attrs, int attrs_count, BaseShape *parent);
	void draw();
};
#endif 
