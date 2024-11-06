#include "Path.h"

enum PathCommand{
  PATH_COMMAND_MOVE = 0,
  PATH_COMMAND_LINETO = 0,
  PATH_COMMAND_BEZIER,
  PATH_COMMAND_ELLIPTIC,
  PATH_COMMAND_CLOSE_PATH,
  PATH_COMMAND_COUNT
};


void BaseCommand::draw() const {
}

void CommandLine::draw() const {
  std::cout << "Draw line from: " << this->point_0.x << '|' << this->point_0.y << " to " 
            << this->point_1.x << '|' << this->point_1.y << '\n';
}

void CommandMove::draw() const {
  std::cout << "Move to: " << this-> x << '|' << this->y << '\n'; 
}

void CommandBezier::draw() const {
  std::cout << "Draw Bezier from: " << this->point_0.x << '|' << this->point_0.y << " to " 
            << this->point_N.x << '|' << this->point_N.y << '\n'
            << "Control point: " << this->point_CS.x << '|' << this->point_CS.y << '\n'
            << "Control end point: " << this->point_CE.x << '|' << this->point_CE.y << '\n';
}

void CommandEllipse::draw() const {
  std::cout << "Draw Ellipse from: " << this->current_point.x << '|' << this->current_point.y << " to " 
            << this->point_end.x << '|' << this->point_end.y << '\n'
            << "Radi point: " << this->rx << '|' << this->ry << '\n'
            << "Angle degree: " << this->angle_degree << '\n'
            << "Large arc flage: " << this->large_arc_flag<< '\n'
            << "Sweep flag: " << this->sweep_flag << '\n';
}

void CommandClosePath::draw() const {
  std::cout << "Close path from: " << this->start_point.x << '|' << this->start_point.y << " to " 
            << this->current_point.x << '|' << this->current_point.y << '\n';
}

CommandMove::CommandMove(double x, double y): x{x}, y{y} {
}

CommandLine::CommandLine(Point p0, Point p1) : point_0(p0), point_1(p1){
}

CommandBezier::CommandBezier(Point point_0, Point point_n, Point point_cs, Point point_ce)
  : point_0(point_0), 
    point_N(point_n), 
    point_CS(point_cs), 
    point_CE(point_ce) {
}

CommandClosePath::CommandClosePath(Point start_point, Point current_point) 
  : start_point(start_point), 
    current_point(current_point) {
}

CommandEllipse::CommandEllipse(Point current_point, Point point_end, double rx, double ry, double angle_degree, int large_arc_flag, int sweep_flag) 
  : current_point{current_point}, 
    point_end{point_end},
    rx{rx},
    ry{ry},
    angle_degree{angle_degree}, 
    large_arc_flag{large_arc_flag}, 
    sweep_flag{sweep_flag} {
}


static double read_double(std::string_view &str) {
  double num;
  std::from_chars_result res;
  while (isspace(str[0]) || str[0] == ',') {
    str = str.substr(1);
  }
  res  = std::from_chars(str.data(), str.data() + str.size(), num); 
  str = str.substr(res.ptr - str.data());
  while (isspace(str[0]) || str[0] == ',') {
    str = str.substr(1);
  }
  return num;
}

static bool is_next_command(const char &chr) {
  switch(chr) {
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

  std::unique_ptr<BaseCommand> *tail_command = &(this->head_command);

  Point current_point = {};
  Point start_point = {};
  bool is_start_point_exist = false;
    
  for (int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
    if (key == "d") {
      std::cout << "Founded path\n";
      while (!value.empty()) {
        char command = value[0];
        value = value.substr(1);
        switch(command) {
          case 'M': {
            std::cout << "INFO: start M\n";
            // read 2 points 
            double x = read_double(value);
            double y = read_double(value);
            *tail_command = std::make_unique<CommandMove>(x, y);
            current_point.x = x;
            current_point.y = y;

            if (!is_start_point_exist) {
              start_point = current_point;
              is_start_point_exist = true;
            }

            //supsequent parameter
            while (!is_next_command(value[0])) {
              x = read_double(value);
              y = read_double(value);
              *tail_command = std::make_unique<CommandMove>(x, y);
              current_point.x = x;
              current_point.y = y;
            }
            std::cout << "INFO: done M\n";
          } break;
          case 'm': {
            std::cout << "INFO: start m\n";
            double x = current_point.x;
            double y = current_point.x;
            double dx = read_double(value);
            double dy = read_double(value);
            *tail_command = std::make_unique<CommandMove>(x + dx, y + dy);
            current_point.x += dx;
            current_point.y += dy;
              
            //supsequent parameter
            while (!is_next_command(value[0])) {
              double dx = read_double(value);
              double dy = read_double(value);
              *tail_command = std::make_unique<CommandMove>(current_point.x + dx, current_point.y + dy);
              //update current
              current_point.x += dx;
              current_point.y += dy;
              std::cout << "INFO: done m\n";
            }
          } break;
          case 'L': {
              std::cout << "INFO: start L\n";
            // read 2 points 
            double x = read_double(value);
            double y = read_double(value);
            Point des_point = {x, y};
            *tail_command = std::make_unique<CommandLine>(current_point, des_point);
            current_point = {x, y};

            //supsequent parameter
            while (!is_next_command(value[0])) {
              x = read_double(value);
              y = read_double(value);
              des_point = {x, y};
              *tail_command = std::make_unique<CommandLine>(current_point, des_point);
              current_point = {x, y};
              std::cout << "INFO: done L\n";
            }
          } break;

          case 'l': {
            std::cout << "INFO: start l\n";
            double dx = read_double(value);
            double dy = read_double(value);
            Point d_point = {current_point.x + dx, current_point.y + dy};
            *tail_command = std::make_unique<CommandLine>(current_point, d_point);
            current_point.x += dx;
            current_point.y += dy;
              
            //supsequent parameter
            while (!is_next_command(value[0])) {
              double dx = read_double(value);
              double dy = read_double(value);
              d_point = {current_point.x + dx, current_point.y + dy};
              *tail_command = std::make_unique<CommandLine>(current_point, d_point);
              //update current
              current_point.x += dx;
              current_point.y += dy;
            }
             std::cout << "INFO: done l\n";
          } break;

          case 'H': {
            std::cout << "INFO: start H\n";
            double x = read_double(value);
            Point h_point = {x, current_point.y};
            *tail_command = std::make_unique<CommandLine>(current_point, h_point);
            current_point.x = x;

            //supsequent parameter
            while (!is_next_command(value[0])) {
              x = read_double(value);
              h_point = {x, current_point.y};
              *tail_command = std::make_unique<CommandLine>(current_point, h_point);
              current_point.x = x;
            }
            std::cout << "INFO: done H\n";
          } break;

          case 'h': {
            std::cout << "INFO: start h\n";
            double dx = read_double(value);
            Point h_point = {current_point.x + dx, current_point.y};
            *tail_command = std::make_unique<CommandLine>(current_point, h_point);
            current_point.x += dx;

            //supsequent parameter
            while (!is_next_command(value[0])) {
              dx = read_double(value);
              Point h_point = {current_point.x + dx, current_point.y};
              *tail_command = std::make_unique<CommandLine>(current_point, h_point);
              current_point.x += dx;
            }
          } break;
          case 'V': {
            std::cout << "INFO: start V\n";
            double y = read_double(value);
            Point v_point = {current_point.x, y};
            *tail_command = std::make_unique<CommandLine>(current_point, v_point);
            current_point.y = y;

            //supsequent parameter
            while (!is_next_command(value[0])) {
              y = read_double(value);
              Point v_point = {current_point.x, y};
              *tail_command = std::make_unique<CommandLine>(current_point, v_point);
              current_point.y = y;
              std::cout << "INFO: done V\n";
            }
          } break;
          case 'v': {
            std::cout << "INFO: start v\n";
            double dy = read_double(value);
            Point v_point = {current_point.x, current_point.y + dy};
            *tail_command = std::make_unique<CommandLine>(current_point, v_point);
              current_point.y = dy;

            //supsequent parameter
            while (!is_next_command(value[0])) {
              dy = read_double(value);
            Point v_point = {current_point.x, current_point.y + dy};
            *tail_command = std::make_unique<CommandLine>(current_point, v_point);
              current_point.y = dy;
            std::cout << "INFO: done v\n";
            }
          } break;
          case 'C': {
            std::cout << "INFO: start C\n";
            double x[3];
            double y[3];
            for (int i = 0; i < 3; ++i) {
              x[i] = read_double(value);
              y[i] = read_double(value);
            }
            Point point_n = {x[0], y[0]};
            Point point_cs = {x[1], y[1]};
            Point point_ce = {x[2], y[2]};

            *tail_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
            current_point = point_n;
            
            //supsequent parameter
            while (!is_next_command(value[0])) {
              for (int i = 0; i < 3; ++i) {
                x[i] = read_double(value);
                y[i] = read_double(value);
              }
              Point point_n = {x[0], y[0]};
              Point point_cs = {x[1], y[1]};
              Point point_ce = {x[2], y[2]};
              *tail_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
              current_point = point_n;
            }
            std::cout << "INFO: done C\n";
          } break;
          case 'c': {
            std::cout << "INFO: start c\n";
            double x[3];
            double y[3];
            for (int i = 0; i < 3; ++i) {
              x[i] = read_double(value);
              y[i] = read_double(value);
            }
            Point point_n = {x[0] + current_point.x, y[0] + current_point.y};
            Point point_cs = {x[1] + current_point.x, y[1] + current_point.y};
            Point point_ce = {x[2] + current_point.x, y[2] + current_point.y};

            *tail_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
            current_point = point_n;
            
            //supsequent parameter
            while (!is_next_command(value[0])) {
              for (int i = 0; i < 3; ++i) {
                x[i] = read_double(value);
                y[i] = read_double(value);
              }
              Point point_n = {x[0] + current_point.x, y[0] + current_point.y};
              Point point_cs = {x[1] + current_point.x, y[1] + current_point.y};
              Point point_ce = {x[2] + current_point.x, y[2] + current_point.y};

              *tail_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
              current_point = point_n;
            }
            std::cout << "INFO: done c\n";

          } break;
          case 'S': {
            std::cout << "INFO: start S\n";
            double x[2];
            double y[2];
            for (int i = 0; i < 2; ++i) {
              x[i] = read_double(value);
              y[i] = read_double(value);
            }
            Point point_ce = {x[0], y[0]};
            Point point_n = {x[1], y[1]};
            Point point_cs = {0, 0};

            *tail_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
            current_point = point_n;
            
            //supsequent parameter
            while (!is_next_command(value[0])) {
              for (int i = 0; i < 2; ++i) {
                x[i] = read_double(value);
                y[i] = read_double(value);
              }
              Point point_ce = {x[0], y[0]};
              Point point_n = {x[1], y[1]};
              Point point_cs = {0, 0};

              *tail_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
              current_point = point_n;
            }
            std::cout << "INFO: done S\n";
          } break;
          case 's': {
            std::cout << "INFO: start s\n";
            double x[2];
            double y[2];
            for (int i = 0; i < 2; ++i) {
              x[i] = read_double(value);
              y[i] = read_double(value);
            }
            Point point_ce = {x[0] + current_point.x, y[0] + current_point.y};
            Point point_n = {x[1] + current_point.x, y[1] + current_point.y};
            Point point_cs = {0, 0};

            *tail_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
            current_point = point_n;
            
            //supsequent parameter
            while (!is_next_command(value[0])) {
              for (int i = 0; i < 2; ++i) {
                x[i] = read_double(value);
                y[i] = read_double(value);
              }
              Point point_ce = {x[0] + current_point.x, y[0] + current_point.y};
              Point point_n = {x[1] + current_point.x, y[1] + current_point.y};
              Point point_cs = {0, 0};

              *tail_command = std::make_unique<CommandBezier>(current_point, point_n, point_cs, point_ce);
              current_point = point_n;
            }
            std::cout << "INFO: done s\n";
          } break;
          case 'Q': {
            std::cout << "INFO: start Q\n";
            double x[2];
            double y[2];
            for (int i = 0; i < 2; ++i) {
              x[i] = read_double(value);
              y[i] = read_double(value);
            }
            Point point_c = {x[0], y[0]};
            Point point_n = {x[1], y[1]};
            Point point_cn = {0, 0};
            *tail_command = std::make_unique<CommandBezier>(current_point, point_n, point_c, point_cn);
            current_point = point_n;
            
            //supsequent parameter
            while  (!is_next_command(value[0])) {
              //read next point
              double x[2];
              double y[2];
              for (int i = 0; i < 2; ++i) {
                x[i] = read_double(value);
                y[i] = read_double(value);
              }
              Point point_c = {x[0], y[0]};
              Point point_n = {x[1], y[1]};
              Point point_cn = {0, 0};

              *tail_command = std::make_unique<CommandBezier>(current_point, point_n, point_c, point_cn);
              current_point = point_n;
            }
            std::cout << "INFO: done Q\n";
          } break;

          case 'q': { //d_point_n, d_point_c (from current point)
            std::cout << "INFO: start q\n";
            double x[2];
            double y[2];
            for (int i = 0; i < 2; ++i) {
              x[i] = read_double(value);
              y[i] = read_double(value);
            }
            Point point_c = {x[0] + current_point.x, y[0] + current_point.y};
            Point point_n = {x[1] + current_point.x, y[1] + current_point.y};
            Point point_cn = {0, 0};
            *tail_command = std::make_unique<CommandBezier>(current_point, point_n, point_c, point_cn);
            current_point = point_n;
            
            //supsequent parameter
            while  (!is_next_command(value[0])) {
              //read next point
              double x[2];
              double y[2];
              for (int i = 0; i < 2; ++i) {
                x[i] = read_double(value);
                y[i] = read_double(value);
              }
              Point point_c = {x[0] + current_point.x, y[0] + current_point.y};
              Point point_n = {x[1] + current_point.x, y[1] + current_point.y};
              Point point_cn = {0, 0};
              *tail_command = std::make_unique<CommandBezier>(current_point, point_n, point_c, point_cn);
              current_point = point_n;
            }
            std::cout << "INFO: done q\n";
          } break;
          case 'T': {//quadratic abnormal, Input the end of previous bezier curve 
            std::cout << "INFO: start T\n";
            double x = read_double(value);
            double y = read_double(value);
            Point point_n = {x, y};
            //control point will be the previous control point
            *tail_command = std::make_unique<CommandBezier>(current_point, point_n, current_point, current_point);
            current_point = {x, y};

            //supsequent parameter
            while (!is_next_command(value[0])) {
              x = read_double(value);
              y = read_double(value);
              point_n.x = x;
              point_n.y = y;
              *tail_command = std::make_unique<CommandBezier>(current_point, point_n, current_point, current_point);
              current_point = {x, y};
            }
            std::cout << "INFO: done T\n";
          } break;
          case 't': {
            std::cout << "INFO: start t\n";
            double x = read_double(value);
            double y = read_double(value);
            Point point_n = {current_point.x + x, current_point.y + y};
            //control point will be the previous control point
            *tail_command = std::make_unique<CommandBezier>(current_point, point_n, current_point, current_point);
            current_point.x += x;
            current_point.y += y;

            //supsequent parameter
            while (!is_next_command(value[0])) {
              x = read_double(value);
              y = read_double(value);
              Point point_n = {current_point.x + x, current_point.y + y};
              *tail_command = std::make_unique<CommandBezier>(current_point, point_n, current_point, current_point);
              current_point.x += x;
              current_point.y += y;
            }
            std::cout << "INFO: done t\n";
          } break;
          case 'A': {
            std::cout << "INFO: start A\n";
            double rx, ry;
            double angle_degree;  
            int large_arc_flag;   
            int sweep_flag;       
            Point point_n;

            rx = read_double(value);
            ry = read_double(value);
            angle_degree = read_double(value);  
            large_arc_flag = (int)read_double(value);   
            sweep_flag = (int)read_double(value);   
            point_n.x = read_double(value);
            point_n.y = read_double(value);

            *tail_command = std::make_unique<CommandEllipse>(current_point, point_n, rx, ry, angle_degree, large_arc_flag, sweep_flag);

            current_point = point_n;

            while (!is_next_command(value[0])) {
              rx = read_double(value);
              ry = read_double(value);
              angle_degree = read_double(value);  
              large_arc_flag = (int)read_double(value);   
              sweep_flag = (int)read_double(value);   
              point_n.x = read_double(value);
              point_n.y = read_double(value);

              *tail_command = std::make_unique<CommandEllipse>(current_point, point_n, rx, ry, angle_degree, large_arc_flag, sweep_flag);

              current_point = point_n;
            }
            std::cout << "INFO: done A\n";

          } break;
          case 'a': {
            std::cout << "INFO: start a\n";
            double rx, ry;
            double angle_degree;  
            int large_arc_flag;   
            int sweep_flag;       
            Point point_n;

            rx = read_double(value);
            ry = read_double(value);
            angle_degree = read_double(value);  
            large_arc_flag = (int)read_double(value);   
            sweep_flag = (int)read_double(value);   
            point_n.x = current_point.x + read_double(value);
            point_n.y = current_point.y + read_double(value);

            *tail_command = std::make_unique<CommandEllipse>(current_point, point_n, rx, ry, angle_degree, large_arc_flag, sweep_flag);
            current_point = point_n;

            while (!is_next_command(value[0])) {
              rx = read_double(value);
              ry = read_double(value);
              angle_degree = read_double(value);  
              large_arc_flag = (int)read_double(value);   
              sweep_flag = (int)read_double(value);   
              point_n.x = current_point.x + read_double(value);
              point_n.y = current_point.y + read_double(value);

              *tail_command = std::make_unique<CommandEllipse>(current_point, point_n, rx, ry, angle_degree, large_arc_flag, sweep_flag);
              current_point = point_n;
            }
            std::cout << "INFO: done a\n";
          } break;
          case 'Z': 
          case 'z': {
            std::cout << "INFO: start Z\n";
            *tail_command = std::make_unique<CommandClosePath>(start_point, current_point);
            std::cout << "INFO: done Z\n";
          } break;
          default: {
          __builtin_unreachable();
          }
        }
        tail_command = &(*tail_command)->next_command;
      }
    }
  }
}

void Path::draw() const {
  for (BaseCommand* c = this->head_command.get(); c; c = c->next_command.get()) {
     c->draw(); 
}
      }
