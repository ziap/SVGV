#include "Path.h"

Path::Path(Attribute *attrs, int attrs_count, BaseShape *parent)
  : BaseShape(attrs, attrs_count, parent) {
	BaseCommand command;
	
  std::from_chars_result res;
	for (int i = 0; i < attrs_count; ++i){
		std::string_view key = attrs[i].key;
		std::string_view value = attrs[i].value;
		if (key == "d"){
			while(!value.empty()){
				char command = value[0];
				if (command == 'M' || command == 'm')	
					CommandMove move;
			}
		}
		
	}
}
