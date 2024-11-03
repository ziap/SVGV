#include "Text.h"


Text::Text(Attribute *attrs, int attrs_count, BaseShape *parent): BaseShape(attrs, attrs_count, parent){
	this->x = 0;
	this->y = 0;
	this->dx = 0;
	this->dy = 0;
	this->len_adjust = LENGTH_ADJUST_SPACING;
	this->text_length = 0;
	
	for (int i = 0; i < attrs_count; ++i){
		std::string_view key = attrs[i].key;
		std::string_view value = attrs[i].value;

	}
}
