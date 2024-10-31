#include "Rect.h"

void Rect::draw(){
	std::cout << "The rectangle attrs: " << '\n' <<
		"X: " << this->x << " | " <<
		"Y: " << this->y << " | " << 
		"RX: " << this->rx << " | " << 
		"RY: " << this->ry << " | " <<
		"WIDTH: " << this->width << " | " << 
		"HEIGHT: " << this->height << '\n';
}

Rect::Rect(Attribute *attrs, int attrs_count, BaseShape *parent)
  : BaseShape(attrs, attrs_count, parent) {
		std::cout << "Creating Rect\n";
		this->x = 0;
		this->y = 0;
		this->width = 0;
		this->height = 0;
		this->rx = 0;
		this->ry = 0;
	
		//go through all of attributes readed, read the rect's attrs
		for (int i = 0; i < attrs_count; ++i){
			std::string_view key = attrs[i].key;
			std::string_view value = attrs[i].value;

			RectAttr attr = (RectAttr)inv_rect_attribute[key];
			switch(attr){
				case RECT_ATTR_X: {
   				std::from_chars(value.data(), value.data() + value.size(), this->x);
				} break;

				case RECT_ATTR_Y: {
   				std::from_chars(value.data(), value.data() + value.size(), this->y);
				} break;

				case RECT_ATTR_RX: {
   				std::from_chars(value.data(), value.data() + value.size(), this->rx);
				} break;

				case RECT_ATTR_RY: {
   				std::from_chars(value.data(), value.data() + value.size(), this->ry);
				} break;
														
				case RECT_ATTR_WIDTH: {
   				std::from_chars(value.data(), value.data() + value.size(), this->width);
				} break;

				case RECT_ATTR_HEIGHT: {
   				std::from_chars(value.data(), value.data() + value.size(), this->height);
				} break;

				case RECT_ATTR_COUNT: {
					__builtin_unreachable();
				}
			}
		}
		std::cout << "Finished read rect attributes\n";
}
