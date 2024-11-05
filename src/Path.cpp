#include "Path.h"

Point::Point(): x(0), y(0){
}

Point::Point(const Point &point): x(point.x), y(point.y){

}

Point::Point(double x, double y): x(x), y(y){
}

CommandMove::CommandMove(double x, double y): x(x), y(y){
}

CommandLine::CommandLine(double p0_x, double p0_y, double pN_x, double pN_y): point_0(p0_x, p0_y), point_N(pN_x, pN_y){
}

CommandBezier::CommandBezier(Point point_0, Point point_n, Point point_cs, Point point_ce): point_0(point_0), point_N(point_n), point_CS(point_cs), point_CE(point_ce){
}

CommandClosePath::CommandClosePath(Point start_point, Point current_point): start_point(start_point), current_point(current_point){
}

static double read_double(std::string_view &str){
	double num;
	std::from_chars_result res;
	while(isspace(str[0]) || str[0] == ','){
		str = str.substr(1);
	}
	res  = std::from_chars(str.data(), str.data() + str.size(), num); 
	str = str.substr(res.ptr - str.data());
	while(isspace(str[0]) || str[0] == ','){
		str = str.substr(1);
	}
	return num;
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
	std::unique_ptr<BaseCommand> head_command;
	std::unique_ptr<BaseCommand> *tail_command = &head_command;

	Point current_point;
	Point start_point;
	bool is_start_point_exist = false;
	Point curve_control_point;
		
  std::unique_ptr<BaseCommand> stack;
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
						double x = read_double(value);
						double y = read_double(value);
						new_command = std::make_unique<CommandMove>(x, y);
						current_point.x = x;
						current_point.y = y;

						if (!is_start_point_exist){
							start_point = current_point;
							is_start_point_exist = true;
						}

						//supsequent parameter
						while(!is_next_command(value[0])){
							x = read_double(value);
							y = read_double(value);
							new_command = std::make_unique<CommandMove>(x, y);
							current_point.x = x;
							current_point.y = y;
						}
					} break;
					case 'm':{
						double x = current_point.x;
						double y = current_point.x;
						double dx = read_double(value);
						double dy = read_double(value);
						new_command = std::make_unique<CommandMove>(x + dx, y + dy);
						current_point.x += dx;
						current_point.y += dy;
							
						//supsequent parameter
						while(!is_next_command(value[0])){
							double dx = read_double(value);
							double dy = read_double(value);
							new_command = std::make_unique<CommandMove>(current_point.x + dx, current_point.y + dy);
							//update current
							current_point.x += dx;
							current_point.y += dy;
						}
					} break;
					case 'L':{
						// read 2 points 
						double x = read_double(value);
						double y = read_double(value);
						new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, x, y);
						current_point.x = x;
						current_point.y = y;

						//supsequent parameter
						while(!is_next_command(value[0])){
							x = read_double(value);
							y = read_double(value);
							new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, x, y);
							current_point.x = x;
							current_point.y = y;
						}
					} break;

					case 'l':{
						double x = current_point.x;
						double y = current_point.x;
						double dx = read_double(value);
						double dy = read_double(value);
						new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, x + dx, y + dy);
						current_point.x += dx;
						current_point.y += dy;
							
						//supsequent parameter
						while(!is_next_command(value[0])){
							double dx = read_double(value);
							double dy = read_double(value);
							new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, x + dx, y + dy);
							//update current
							current_point.x += dx;
							current_point.y += dy;
						}
					} break;

					case 'H':{
						double x = read_double(value);
						new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, x, current_point.y);
						current_point.x = x;

						//supsequent parameter
						while(!is_next_command(value[0])){
							x = read_double(value);
							new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, x, current_point.y);
							current_point.x = x;
						}
					} break;
					case 'h':{
						double dx = read_double(value);
						new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x + dx, current_point.y);
						current_point.x += dx;

						//supsequent parameter
						while(!is_next_command(value[0])){
							dx = read_double(value);
							new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x + dx, current_point.y);
							current_point.x = dx;
						}
					} break;
					case 'V':{
						double y = read_double(value);
						new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x, y);
						current_point.y = y;

						//supsequent parameter
						while(!is_next_command(value[0])){
							y = read_double(value);
							new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x, y);
							current_point.y = y;
						}
					} break;
					case 'v':{
						double dy = read_double(value);
						new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x, current_point.y + dy);
						current_point.y += dy;

						//supsequent parameter
						while(!is_next_command(value[0])){
							dy = read_double(value);
							new_command = std::make_unique<CommandLine>(current_point.x, current_point.y, current_point.x, current_point.y + dy);
							current_point.y = dy;
						}
					} break;
					case 'C':{
						double x[3];
						double y[3];
						for (int i = 0; i < 3; ++i){
							x[i] = read_double(value);
							y[i] = read_double(value);
						}
						Point point_n(x[0],y[0]);
						Point point_cs(x[1],y[1]);
						Point point_ce(x[2],y[2]);

						new_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
						current_point = point_n;
						
						//supsequent parameter
						while(!is_next_command(value[0])){
							for (int i = 0; i < 3; ++i){
								x[i] = read_double(value);
								y[i] = read_double(value);
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
							x[i] = read_double(value);
							y[i] = read_double(value);
						}
						Point d_point_cs(current_point.x + x[0], current_point.y + y[0]);
						Point d_point_ce(current_point.x + x[1], current_point.y + y[1]);
						Point d_point_n(current_point.x + x[1], current_point.y + y[1]);

						new_command = std::make_unique<CommandBezier>(current_point, d_point_n, d_point_cs, d_point_ce);
						current_point = d_point_n;
						
						//supsequent parameter
						while(!is_next_command(value[0])){
							for (int i = 0; i < 3; ++i){
								x[i] = read_double(value);
								y[i] = read_double(value);
							}
							Point point_n(x[0],y[0]);
							Point point_cs(x[1],y[1]);
							Point point_ce(x[2],y[2]);

							new_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
							current_point = point_n;
						}

					} break;
					case 'S':{
						double x[2];
						double y[2];
						for (int i = 0; i < 2; ++i){
							x[i] = read_double(value);
							y[i] = read_double(value);
						}
						Point point_ce(x[0],y[0]);
						Point point_n(x[1],y[1]);
						Point point_cs(0, 0); 

						new_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
						current_point = point_n;
						
						//supsequent parameter
						while(!is_next_command(value[0])){
							for (int i = 0; i < 2; ++i){
								x[i] = read_double(value);
								y[i] = read_double(value);
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
							x[i] = read_double(value);
							y[i] = read_double(value);
						}
						Point point_ce(current_point.x + x[0], current_point.y + y[0]);
						Point point_n(current_point.x + x[1], current_point.y + y[1]);
						Point point_cs(0, 0);

						new_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
						current_point = point_n;
						
						//supsequent parameter
						while(!is_next_command(value[0])){
							for (int i = 0; i < 2; ++i){
								x[i] = read_double(value);
								y[i] = read_double(value);
							}
							Point point_ce(current_point.x + x[0], current_point.y + y[0]);
							Point point_n(current_point.x + x[1], current_point.y + y[1]);
							Point point_cs(0, 0);

							new_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
							current_point = point_n;
						}
					} break;
					case 'Q':{
						double x[2];
						double y[2];
						for (int i = 0; i < 2; ++i){
							x[i] = read_double(value);
							y[i] = read_double(value);
						}
						Point point_n(x[0],y[0]);
						Point point_c(x[1],y[1]);
						Point point_cn(0, 0);
						new_command = std::make_unique<CommandBezier>(current_point, point_n, point_c, point_cn);
						current_point = point_n;
						
						//supsequent parameter
						while(!is_next_command(value[0])){
							//read next point
							double x[2];
							double y[2];
							for (int i = 0; i < 2; ++i){
								x[i] = read_double(value);
								y[i] = read_double(value);
							}
							Point point_c(x[0],y[0]);
							Point point_n(x[1],y[1]);
							Point point_cn(0, 0);

							new_command = std::make_unique<CommandBezier>(current_point, point_n, point_c, point_cn);
							current_point = point_n;
						}
					} break;

					case 'q':{ //d_point_n, d_point_c (from current point)
						double x[2];
						double y[2];
						for (int i = 0; i < 2; ++i){
							x[i] = read_double(value);
							y[i] = read_double(value);
						}
						Point d_point_n(current_point.x + x[0], current_point.y + y[0]);
						Point d_point_c(current_point.x + x[1], current_point.y + y[1]);
						Point d_point_cn(0, 0);
						new_command = std::make_unique<CommandBezier>(current_point, d_point_n, d_point_c, d_point_cn);
						current_point = d_point_n;
						
						//supsequent parameter
						while(!is_next_command(value[0])){
						//read next point
						double x[2];
						double y[2];
						for (int i = 0; i < 2; ++i){
							x[i] = read_double(value);
							y[i] = read_double(value);
						}
						Point d_point_n(current_point.x + x[0], current_point.y + y[0]);
						Point d_point_c(current_point.x + x[1], current_point.y + y[1]);
						Point d_point_cn(0, 0);
						new_command = std::make_unique<CommandBezier>(current_point, d_point_n, d_point_c, d_point_cn);
						current_point = d_point_n;
						}
					} break;
					case 'T':{//quadratic abnormal, Input end_point
						double x = read_double(value);
						double y = read_double(value);
						Point point_n(x, y);
						//control point will be the previous control point
						new_command = std::make_unique<CommandBezier>(current_point, point_n, current_point, current_point);
						current_point.x = x;
						current_point.y = y;

						//supsequent parameter
						while(!is_next_command(value[0])){
							x = read_double(value);
							y = read_double(value);
							Point point_n(x, y);
							new_command = std::make_unique<CommandBezier>(current_point, point_n, current_point, current_point);
							current_point.x = x;
							current_point.y = y;
						}
					} break;
					case 't':{
						double x = read_double(value);
						double y = read_double(value);
						Point point_n(current_point.x + x, current_point.y + y);
						//control point will be the previous control point
						new_command = std::make_unique<CommandBezier>(current_point, point_n, current_point, current_point);
						current_point.x = x;
						current_point.y = y;

						//supsequent parameter
						while(!is_next_command(value[0])){
							x = read_double(value);
							y = read_double(value);
							Point point_n(current_point.x + x, current_point.y + y);
							new_command = std::make_unique<CommandBezier>(current_point, point_n, current_point, current_point);
							current_point.x = x;
							current_point.y = y;
						}
					} break;
					case 'A':{
						Point point_radi;
						double angle_degree; 	
						int large_arc_flag;   
						int sweep_flag; 			
						Point point_n;

						point_radi.x = read_double(value);
						point_radi.y = read_double(value);
						angle_degree = read_double(value); 	
						large_arc_flag = (int)read_double(value); 	
						sweep_flag = (int)read_double(value); 	
						point_n.x = read_double(value);
						point_n.y = read_double(value);

						new_command = std::make_unique<CommandEllipse>(current_point, point_n, point_radi, angle_degree, large_arc_flag, sweep_flag);
						current_point = point_n;

						while(!is_next_command(value[0])){

							point_radi.x = read_double(value);
							point_radi.y = read_double(value);
							angle_degree = read_double(value); 	
							large_arc_flag = (int)read_double(value); 	
							sweep_flag = (int)read_double(value); 	
							point_n.x = read_double(value);
							point_n.y = read_double(value);

							new_command = std::make_unique<CommandEllipse>(current_point, point_n, point_radi, angle_degree, large_arc_flag, sweep_flag);
							current_point = point_n;

						}

					} break;
					case 'a':{
						Point point_radi;
						double angle_degree; 	
						int large_arc_flag;   
						int sweep_flag; 			
						Point d_point_n;

						point_radi.x = read_double(value);
						point_radi.y = read_double(value);
						angle_degree = read_double(value); 	
						large_arc_flag = (int)read_double(value); 	
						sweep_flag = (int)read_double(value); 	
						d_point_n.x = current_point.x + read_double(value);
						d_point_n.y = current_point.y + read_double(value);

						new_command = std::make_unique<CommandEllipse>(current_point, d_point_n, point_radi, angle_degree, large_arc_flag, sweep_flag);
						current_point = d_point_n;

						while(!is_next_command(value[0])){

							point_radi.x = read_double(value);
							point_radi.y = read_double(value);
							angle_degree = read_double(value); 	
							large_arc_flag = (int)read_double(value); 	
							sweep_flag = (int)read_double(value); 	
							d_point_n.x = current_point.x + read_double(value);
							d_point_n.y = current_point.y + read_double(value);

							new_command = std::make_unique<CommandEllipse>(current_point, d_point_n, point_radi, angle_degree, large_arc_flag, sweep_flag);
							current_point = d_point_n;

						}
					} break;
					case 'Z':{
						new_command = std::make_unique<CommandClosePath>(start_point, current_point);
					} break;
					case 'z':{
						new_command = std::make_unique<CommandClosePath>(start_point, current_point);
					} break;
					default: {
          __builtin_unreachable();
					}
				}
      if (new_command) {
        if (value.size()) {
          *tail_command = std::move(new_command);
          tail_command = &(*tail_command)->next_command;
        } else {
          new_command->next_command = std::move(stack);
          stack = std::move(new_command);
        }
      }
			}
		}
	}
}
