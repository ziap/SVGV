#include "Path.h"

Point::Point(): x(0), y(0){
}

Point::Point(double x, double y): x(x), y(y){
}

CommandMove::CommandMove(double x, double y): x(x), y(y){
}

CommandLine::CommandLine(double p0_x, double p0_y, double pN_x, double pN_y): point_0(p0_x, p0_y), point_N(pN_x, pN_y){
}

static bool is_next_command(const char &chr){
	switch(chr){
		case 'M': return true;
		case 'm': return true;
		case 'L': return true;
		case 'l': return true;
		case 'H': return true;
		case 'h': return true;
		case 'V': return true;
		case 'v': return true;
		case 'C': return true;
		case 'c': return true;
		case 'S': return true;
		case 's': return true;
		case 'Q': return true;
		case 'q': return true;
		case 'T': return true;
		case 't': return true;
		case 'A': return true;
		case 'a': return true;
		case 'Z': return true;
		case 'z': return true;
		default: return false;
	}
}

Path::Path(Attribute *attrs, int attrs_count, BaseShape *parent)
  : BaseShape(attrs, attrs_count, parent) {

	std::unique_ptr<BaseCommand> new_command;
	Point current_point;
	
  std::from_chars_result res;
	for (int i = 0; i < attrs_count; ++i){
		std::string_view key = attrs[i].key;
		std::string_view value = attrs[i].value;
		if (key == "d"){
			while(!value.empty()){
				char command = value[0];
				value = value.substr(1);
				switch(command){
					case 'M':{
						// read 2 points 
						double x, y;
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						res  = std::from_chars(value.data(), value.data() + value.size(), y); 
						value = value.substr(res.ptr - value.data());
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						res  = std::from_chars(value.data(), value.data() + value.size(), y); 
						value = value.substr(res.ptr - value.data());
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						new_command = std::make_unique<CommandMove>(x, y);
						current_point.x = x;
						current_point.y = y;

						//supsequent parameter
						while(!is_next_command(value[0])){
								res  = std::from_chars(value.data(), value.data() + value.size(), x); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
								res  = std::from_chars(value.data(), value.data() + value.size(), y); 
								value = value.substr(res.ptr - value.data());

								//delete space behind
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
								new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, x, y);
								current_point.x = x;
								current_point.y = y;
					} break;
					case 'm':{
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						double x = current_point.x;
						double y = current_point.x;
						double dx, dy;
						res  = std::from_chars(value.data(), value.data() + value.size(), dx); 
						value = value.substr(res.ptr - value.data());
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						res  = std::from_chars(value.data(), value.data() + value.size(), dy); 
						value = value.substr(res.ptr - value.data());
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						new_command = std::make_unique<CommandMove>(x + dx, y + dy);
						current_point.x = x + dx;
						current_point.y = y + dy;
							
						//supsequent parameter
						while(!is_next_command(value[0])){
								res  = std::from_chars(value.data(), value.data() + value.size(), dx); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
								res  = std::from_chars(value.data(), value.data() + value.size(), dy); 
								value = value.substr(res.ptr - value.data());
								//delete space behind
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
								new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x + dx, current_point.y + dy);
								//update current
								current_point.x += dx;
								current_point.y += dy;
						}
					} break;
					case 'L':{
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						double x, y;
						res  = std::from_chars(value.data(), value.data() + value.size(), x); 
						value = value.substr(res.ptr - value.data());
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						res  = std::from_chars(value.data(), value.data() + value.size(), y); 
						value = value.substr(res.ptr - value.data());
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, x, y);
						
						current_point.x = x;
						current_point.y = y;
						//supsequent parameter
						while(!is_next_command(value[0])){
							res  = std::from_chars(value.data(), value.data() + value.size(), x); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							res  = std::from_chars(value.data(), value.data() + value.size(), y); 
							value = value.substr(res.ptr - value.data());
							//delete space behind
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
								new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, x, y);
						//update current
						current_point.x = x;
						current_point.y = y;
						}
					} break;

					case 'l':{
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						double dx, dy;
						res  = std::from_chars(value.data(), value.data() + value.size(), dx); 
						value = value.substr(res.ptr - value.data());
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						res  = std::from_chars(value.data(), value.data() + value.size(), dy); 
						value = value.substr(res.ptr - value.data());
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x + dx, current_point.y + dy);
						current_point.x += dx;
						current_point.y += dy;

						//supsequent parameter
						while(!is_next_command(value[0])){
							//read next point
							res  = std::from_chars(value.data(), value.data() + value.size(), dx); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							res  = std::from_chars(value.data(), value.data() + value.size(), dy); 
							value = value.substr(res.ptr - value.data());
							new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x + dx,current_point.y + dy);
							//delete space behind
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
						current_point.x += dx;
						current_point.y += dy;
						}
					} break;

					case 'H':{
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						double x;
						res  = std::from_chars(value.data(), value.data() + value.size(), x); 
						value = value.substr(res.ptr - value.data());
						new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, x, current_point.y);
						//delete space behind
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						current_point.x = x;

						//supsequent parameter
						while(!is_next_command(value[0])){
							//read next point
							res  = std::from_chars(value.data(), value.data() + value.size(), x); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							//command
							new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, x, current_point.y);
							//delete space behind
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							//update
							current_point.x = x;
						}
					} break;
					case 'h':{
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						double dx;
						res  = std::from_chars(value.data(), value.data() + value.size(), dx); 
						value = value.substr(res.ptr - value.data());
						new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x + dx, current_point.y);
						//delete space behind
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						current_point.x += dx;
						//supsequent parameter
						while(!is_next_command(value[0])){
							//read next point
							res  = std::from_chars(value.data(), value.data() + value.size(), dx); 
							value = value.substr(res.ptr - value.data());
							//command
							new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x + dx, current_point.y);
							//delete space behind
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							current_point.x += dx;
						}
					} break;
					case 'V':{
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						double y;
						res  = std::from_chars(value.data(), value.data() + value.size(), y); 
						value = value.substr(res.ptr - value.data());
						new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x, y);
						//delete space behind
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						current_point.y = y;

						//supsequent parameter
						while(!is_next_command(value[0])){
							//read next point
							res  = std::from_chars(value.data(), value.data() + value.size(), y); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							//command
							new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x, y);
							//delete space behind
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							//update
							current_point.y = y;
						}
					} break;
					case 'v':{
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						double dy;
						res  = std::from_chars(value.data(), value.data() + value.size(), dy); 
						value = value.substr(res.ptr - value.data());
						new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x, current_point.y + dy);
						//delete space behind
						while(isspace(value[0]) || value[0] == ','){
							value = value.substr(1);
						}
						current_point.y += dy;
						//supsequent parameter
						while(!is_next_command(value[0])){
							//read next point
							res  = std::from_chars(value.data(), value.data() + value.size(), dy); 
							value = value.substr(res.ptr - value.data());
							//command
							new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x, current_point.y + dy);
							//delete space behind
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							current_point.y += dy;
						}
					} break;
					case 'C':{
						double x[3];
						double y[3];
						for (int i = 0; i < 3; ++i){
							res  = std::from_chars(value.data(), value.data() + value.size(), x[i]); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							res  = std::from_chars(value.data(), value.data() + value.size(), y[i]); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
						}
						Point point_n(x[0],y[0]);
						Point point_cs(x[1],y[1]);
						Point point_ce(x[2],y[2]);

						new_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
						current_point = point_n;
						
						//supsequent parameter
						while(!is_next_command(value[0])){
							//read next point
							double x[3];
							double y[3];
							for (int i = 0; i < 3; ++i){
								res  = std::from_chars(value.data(), value.data() + value.size(), x[i]); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
								res  = std::from_chars(value.data(), value.data() + value.size(), y[i]); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
							}
							Point point_n(x[0],y[0]);
							Point point_cs(x[1],y[1]);
							Point point_ce(x[2],y[2]);

							new_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
							current_point = point_n;
						}
					} break;
					case 'c':{
						double x[3];
						double y[3];
						for (int i = 0; i < 3; ++i){
							res  = std::from_chars(value.data(), value.data() + value.size(), x[i]); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							res  = std::from_chars(value.data(), value.data() + value.size(), y[i]); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
						}
						Point d_point_n(x[0],y[0]);
						Point d_point_cs(x[1] + current_point.x, y[1] + current_point.y);
						Point d_point_ce(x[2] + current_point.x, y[2] + current_point.y);

						new_command = std::make_unique<CommandBezier>(current_point, d_point_n, d_point_cs, d_point_ce);
						current_point = d_point_n;
						
						//supsequent parameter
						while(!is_next_command(value[0])){
							//read next point
							double x[3];
							double y[3];
							for (int i = 0; i < 3; ++i){
								res  = std::from_chars(value.data(), value.data() + value.size(), x[i]); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
								res  = std::from_chars(value.data(), value.data() + value.size(), y[i]); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
							}
						Point d_point_n(x[0] + current_point.x ,y[0] + current_point.y);
						Point d_point_cs(x[1] + current_point.x, y[1] + current_point.y);
						Point d_point_ce(x[2] + current_point.x, y[2] + current_point.y);

							new_command = std::make_unique<CommandBezier>(current_point, d_point_n, d_point_cs, d_point_ce);
							current_point = d_point_n;
						}

					} break;
					case 'S':{
						double x[2];
						double y[2];
						for (int i = 0; i < 2; ++i){
							res  = std::from_chars(value.data(), value.data() + value.size(), x[i]); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							res  = std::from_chars(value.data(), value.data() + value.size(), y[i]); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
						}
						Point point_ce(x[0],y[0]);
						Point point_n(x[1],y[1]);
						Point point_cs(0, 0);
						new_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
						current_point = point_n;
						
						//supsequent parameter
						while(!is_next_command(value[0])){
							//read next point
							double x[2];
							double y[2];
							for (int i = 0; i < 2; ++i){
								res  = std::from_chars(value.data(), value.data() + value.size(), x[i]); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
								res  = std::from_chars(value.data(), value.data() + value.size(), y[i]); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
							}
							Point point_ce(x[0],y[0]);
							Point point_n(x[1],y[1]);
							Point point_cs(0, 0);

							new_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
							current_point = point_n;
						}
					} break;
					case 's':{
						double x[2];
						double y[2];
						for (int i = 0; i < 2; ++i){
							res  = std::from_chars(value.data(), value.data() + value.size(), x[i]); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							res  = std::from_chars(value.data(), value.data() + value.size(), y[i]); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
						}
						Point d_point_ce(current_point.x + x[0], current_point.y + y[0]);
						Point d_point_n(current_point.x + x[1], current_point.y + y[1]);
						Point d_point_cs(0, 0);

						new_command = std::make_unique<CommandBezier>(current_point, d_point_n, d_point_cs, d_point_ce);
						current_point = d_point_n;
						
						//supsequent parameter
						while(!is_next_command(value[0])){
							//read next point
							double x[2];
							double y[2];
							for (int i = 0; i < 2; ++i){
								res  = std::from_chars(value.data(), value.data() + value.size(), x[i]); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
								res  = std::from_chars(value.data(), value.data() + value.size(), y[i]); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
							}
						Point d_point_ce(current_point.x + x[0], current_point.y + y[0]);
						Point d_point_n(current_point.x + x[1], current_point.y + y[1]);
						Point d_point_cs(0, 0);

						new_command = std::make_unique<CommandBezier>(current_point, d_point_n, d_point_cs, d_point_ce);
							current_point = d_point_n;
						}
					} break;
					case 'Q':{
						double x[2];
						double y[2];
						for (int i = 0; i < 2; ++i){
							res  = std::from_chars(value.data(), value.data() + value.size(), x[i]); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							res  = std::from_chars(value.data(), value.data() + value.size(), y[i]); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
						}
						Point point_c(x[0],y[0]);
						Point point_n(x[1],y[1]);
						Point point_cn(0, 0);
						new_command = std::make_unique<CommandBezier>(current_point, point_n, point_c, point_cn);
						current_point = point_n;
						
						//supsequent parameter
						while(!is_next_command(value[0])){
							//read next point
							double x[2];
							double y[2];
							for (int i = 0; i < 2; ++i){
								res  = std::from_chars(value.data(), value.data() + value.size(), x[i]); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
								res  = std::from_chars(value.data(), value.data() + value.size(), y[i]); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
							}
							Point point_c(x[0],y[0]);
							Point point_n(x[1],y[1]);
							Point point_cn(0, 0);

							new_command = std::make_unique<CommandBezier>(current_point, point_n, point_c, point_cn);
							current_point = point_n;
						}
					} break;

					case 'q':{
						double x[2];
						double y[2];
						for (int i = 0; i < 2; ++i){
							res  = std::from_chars(value.data(), value.data() + value.size(), x[i]); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
							res  = std::from_chars(value.data(), value.data() + value.size(), y[i]); 
							value = value.substr(res.ptr - value.data());
							while(isspace(value[0]) || value[0] == ','){
								value = value.substr(1);
							}
						}
						Point d_point_c(current_point.x + x[0], current_point.y + y[0]);
						Point d_point_n(current_point.x + x[1], current_point.y + y[1]);
						Point d_point_cn(0, 0);

						new_command = std::make_unique<CommandBezier>(current_point, d_point_n, d_point_c, d_point_cn);
						current_point = d_point_n;
						
						//supsequent parameter
						while(!is_next_command(value[0])){
							//read next point
							double x[2];
							double y[2];
							for (int i = 0; i < 2; ++i){
								res  = std::from_chars(value.data(), value.data() + value.size(), x[i]); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
								res  = std::from_chars(value.data(), value.data() + value.size(), y[i]); 
								value = value.substr(res.ptr - value.data());
								while(isspace(value[0]) || value[0] == ','){
									value = value.substr(1);
								}
							}
							Point point_ce(x[0],y[0]);
							Point point_n(x[1],y[1]);
							Point point_cs(0, 0);

							new_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
							current_point = point_n;
						}
					} break;
					case 'T':{
						while(isspace(value[0])){
							value = value.substr(1);
						}
						new_command = std::make_unique<CommandBezier>(value);
					} break;
					case 't':{
						while(isspace(value[0])){
							value = value.substr(1);
						}
						new_command = std::make_unique<CommandBezier>(value);
					} break;
					case 'A':{
						while(isspace(value[0])){
							value = value.substr(1);
						}
						new_command = std::make_unique<CommandEllipse>(value);
					} break;
					case 'a':{
						while(isspace(value[0])){
							value = value.substr(1);
						}
						new_command = std::make_unique<CommandEllipse>(value);
					} break;
					case 'Z':{
						while(isspace(value[0])){
							value = value.substr(1);
						}
						new_command = std::make_unique<CommandEllipse>(value);
					} break;
					case 'z':{
						while(isspace(value[0])){
							value = value.substr(1);
						}
						new_command = std::make_unique<CommandEllipse>(value);
					} break;
					default: {
          __builtin_unreachable();
					}
				}
			}
		}
	}
}
}
