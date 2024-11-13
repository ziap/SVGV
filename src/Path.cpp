#include "Path.h"

using namespace SVGShapes;

static std::string_view trim_start(std::string_view str) {
  while (str.size() && (isspace(str[0]) || str[0] == ',')) {
    str = str.substr(1);
  }
  return str;
}

static double read_double(std::string_view *str) {
  *str = trim_start(*str);

  char *out;
  double num = strtod(str->data(), &out);
  *str = str->substr(out - str->data());

  *str = trim_start(*str);
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

  Point current_point = {};
  Point start_point = {};
  Point pre_control_point = {};

    
  for (int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
    if (key == "d") {
      // std::cout << "Founded path\n";
      while (!value.empty()) {
        char command = value[0];
        value = value.substr(1);
        switch(command) {
          case 'M': {
            // std::cout << "INFO: start M\n";
            // read 2 points 
            double x = read_double(&value);
            double y = read_double(&value);
            current_point = {x, y};
            pre_control_point = current_point; 
           

            start_point = current_point;

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              x = read_double(&value);
              y = read_double(&value);
              Point des_point = {x, y};
              this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});

              current_point = {x, y};
              pre_control_point = current_point; 
            }
            // std::cout << "INFO: done M\n";
          } break;
          case 'm': {
            // std::cout << "INFO: start m\n";
            double dx = read_double(&value);
            double dy = read_double(&value);
            current_point.x += dx;
            current_point.y += dy;
            start_point = current_point;
            pre_control_point = current_point; 
              
            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              dx = read_double(&value);
              dy = read_double(&value);
              Point des_point = {current_point.x + dx, current_point.y + dy};
              this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});
              //update current
              current_point.x += dx;
              current_point.y += dy;
              pre_control_point = current_point; 
            }
            // std::cout << "INFO: done m\n";
          } break;
          case 'L': {
              // std::cout << "INFO: start L\n";
            // read 2 points 
            double x = read_double(&value);
            double y = read_double(&value);
            Point des_point = {x, y};
            this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});
              //update current

            current_point = des_point;
            pre_control_point = current_point; 

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              x = read_double(&value);
              y = read_double(&value);
              des_point = {x, y};
              this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});

              current_point = des_point;
              pre_control_point = current_point; 
              // std::cout << "INFO: done L\n";
            }
          } break;

          case 'l': {
            // std::cout << "INFO: start l\n";
            double dx = read_double(&value);
            double dy = read_double(&value);
            Point des_point = {current_point.x + dx, current_point.y + dy};
            this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});

            current_point = des_point;
            pre_control_point = current_point; 
              
            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              double dx = read_double(&value);
              double dy = read_double(&value);
              Point des_point = {current_point.x + dx, current_point.y + dy};
              this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});
              //update current_point
              current_point = des_point;
              pre_control_point = current_point; 
            }
            //  std::cout << "INFO: done l\n";
          } break;

          case 'H': {
            // std::cout << "INFO: start H\n";
            double x = read_double(&value);
            Point des_point = {x, current_point.y};
            this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});

            current_point = des_point;
            pre_control_point = current_point; 

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              x = read_double(&value);
              Point des_point = {x, current_point.y};
              this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});

              current_point = des_point;
              pre_control_point = current_point; 
            }
            // std::cout << "INFO: done H\n";
          } break;

          case 'h': {
            double dx = read_double(&value);
            Point des_point = {current_point.x + dx, current_point.y};
            this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});

            current_point = des_point;
            pre_control_point = current_point; 

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              dx = read_double(&value);
              Point des_point = {current_point.x + dx, current_point.y};
              this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});

              current_point = des_point;
              pre_control_point = current_point; 
            }
          } break;
          case 'V': {
            double y = read_double(&value);
            Point des_point = {current_point.x, y};
            this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});

            current_point = des_point;
            pre_control_point = current_point; 

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              y = read_double(&value);
              Point des_point = {current_point.x, y};
              this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});

              current_point = des_point;
              pre_control_point = current_point; 
              // std::cout << "INFO: done V\n";
            }
          } break;
          case 'v': {
            double dy = read_double(&value);
            Point des_point = {current_point.x, current_point.y + dy};
            this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});
            current_point = des_point;
            pre_control_point = current_point; 

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              dy = read_double(&value);
              Point des_point = {current_point.x, current_point.y + dy};
              this->bezier_list.push(BezierCurve{current_point, des_point, current_point, des_point});

              current_point = des_point;
              pre_control_point = current_point; 
              // std::cout << "INFO: done v\n";
            }
          } break;
          case 'C': {
            double x[3];
            double y[3];
            for (int i = 0; i < 3; ++i) {
              x[i] = read_double(&value);
              y[i] = read_double(&value);
            }
            Point point_cs = {x[0], y[0]};
            Point point_ce = {x[1], y[1]};
            Point point_n = {x[2], y[2]};
            pre_control_point = point_ce;
            this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
            current_point = point_n;
            pre_control_point = point_ce; 

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) ) {
              for (int i = 0; i < 3; ++i) {
                x[i] = read_double(&value);
                y[i] = read_double(&value);
              }

              Point point_cs = {x[0], y[0]};
              Point point_ce = {x[1], y[1]};
              Point point_n = {x[2], y[2]};

              this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
              current_point = point_n;
              pre_control_point = point_ce;
            }
          } break;
          case 'c': {
            double x[3];
            double y[3];
            for (int i = 0; i < 3; ++i) {
              x[i] = read_double(&value);
              y[i] = read_double(&value);
            }
            Point point_cs = {x[0] + current_point.x, y[0] + current_point.y};
            Point point_ce = {x[1] + current_point.x, y[1] + current_point.y};
            Point point_n = {x[2] + current_point.x, y[2] + current_point.y};

            this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
            current_point = point_n;
            pre_control_point = point_ce;
            
            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              for (int i = 0; i < 3; ++i) {
                x[i] = read_double(&value);
                y[i] = read_double(&value);
              }
              Point point_cs = {x[0] + current_point.x, y[0] + current_point.y};
              Point point_ce = {x[1] + current_point.x, y[1] + current_point.y};
              Point point_n = {x[2] + current_point.x, y[2] + current_point.y};

              this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
              current_point = point_n;
              pre_control_point = point_ce;
            }
            // std::cout << "INFO: done c\n";
          } break;
          case 'S': {
            // std::cout << "INFO: start S\n";
            double x[2];
            double y[2];
            for (int i = 0; i < 2; ++i) {
              x[i] = read_double(&value);
              y[i] = read_double(&value);
            }
            Point point_ce = {x[0], y[0]};
            Point point_n = {x[1], y[1]};
            Point point_cs = {current_point.x - pre_control_point.x + current_point.x,
                              current_point.y - pre_control_point.y + current_point.y};

            this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
            current_point = point_n;
            pre_control_point = point_ce;
            
            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              for (int i = 0; i < 2; ++i) {
                x[i] = read_double(&value);
                y[i] = read_double(&value);
              }
              Point point_ce = {x[0], y[0]};
              Point point_n = {x[1], y[1]};
              Point point_cs = {current_point.x - pre_control_point.x + current_point.x,
                              current_point.y - pre_control_point.y + current_point.y};

              this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
              current_point = point_n;
              pre_control_point = point_ce;
            }
            // std::cout << "INFO: done S\n";
          } break;
          case 's': {
            // std::cout << "INFO: start s\n";
            double x[2];
            double y[2];
            for (int i = 0; i < 2; ++i) {
              x[i] = read_double(&value);
              y[i] = read_double(&value);
            }
            Point point_ce = {x[0] + current_point.x, y[0] + current_point.y};
            Point point_n = {x[1] + current_point.x, y[1] + current_point.y};
            Point point_cs = {current_point.x - pre_control_point.x + current_point.x,
                              current_point.y - pre_control_point.y + current_point.y};

            this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
            current_point = point_n;
            pre_control_point = point_ce;
            
            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              for (int i = 0; i < 2; ++i) {
                x[i] = read_double(&value);
                y[i] = read_double(&value);
              }
              Point point_ce = {x[0] + current_point.x, y[0] + current_point.y};
              Point point_n = {x[1] + current_point.x, y[1] + current_point.y};
              Point point_cs = {current_point.x - pre_control_point.x + current_point.x,
                              current_point.y - pre_control_point.y + current_point.y};

              this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
              current_point = point_n;
              pre_control_point = point_ce;
            }
            // std::cout << "INFO: done s\n";
          } break;
          case 'Q': {
            // std::cout << "INFO: start Q\n";
      
            double x[2];
            double y[2];
            for (int i = 0; i < 2; ++i) {
              x[i] = read_double(&value);
              y[i] = read_double(&value);
            }
            Point point_c = {x[0], y[0]};
            Point point_n = {x[1], y[1]};
            this->bezier_list.push(BezierCurve{current_point, point_n, point_c, point_c});
            current_point = point_n;
            pre_control_point = point_c;
            
            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0])) {
              //read next point
              double x[2];
              double y[2];
              for (int i = 0; i < 2; ++i) {
                x[i] = read_double(&value);
                y[i] = read_double(&value);
              }
              Point point_c = {x[0], y[0]};
              Point point_n = {x[1], y[1]};

              this->bezier_list.push(BezierCurve{current_point, point_n, point_c, point_c});
              current_point = point_n;
              pre_control_point = point_c;
            }
            // std::cout << "INFO: done Q\n";
          } break;

          case 'q': { //d_point_n, d_point_c (from current point)
            // std::cout << "INFO: start q\n";
            double x[2];
            double y[2];
            for (int i = 0; i < 2; ++i) {
              x[i] = read_double(&value);
              y[i] = read_double(&value);
            }
            Point point_c = {x[0] + current_point.x, y[0] + current_point.y};
            Point point_n = {x[1] + current_point.x, y[1] + current_point.y};
            this->bezier_list.push(BezierCurve{current_point, point_n, point_c, point_c});
            current_point = point_n;
            pre_control_point = point_c;
            
            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0])) {
              //read next point
              double x[2];
              double y[2];
              for (int i = 0; i < 2; ++i) {
                x[i] = read_double(&value);
                y[i] = read_double(&value);
              }
              Point point_c = {x[0] + current_point.x, y[0] + current_point.y};
              Point point_n = {x[1] + current_point.x, y[1] + current_point.y};
              this->bezier_list.push(BezierCurve{current_point, point_n, point_c, point_c});
              current_point = point_n;
              pre_control_point = point_c;
            }
            // std::cout << "INFO: done q\n";
          } break;
          case 'T': {//quadratic abnormal, Input the end of previous bezier curve 
            // std::cout << "INFO: start T\n";
            double x = read_double(&value);
            double y = read_double(&value);
            Point point_n = {x, y};
            Point point_c = {current_point.x - pre_control_point.x + current_point.x,
                             current_point.y - pre_control_point.y + current_point.y};
            //control point will be the previous control point
            this->bezier_list.push(BezierCurve{current_point, point_n, point_c, current_point});
            current_point = {x, y};
            pre_control_point = point_c;

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              x = read_double(&value);
              y = read_double(&value);
              point_n.x = x;
              point_n.y = y;
              Point point_c = {current_point.x - pre_control_point.x + current_point.x,
                             current_point.y - pre_control_point.y + current_point.y};
              this->bezier_list.push(BezierCurve{current_point, point_n, current_point, current_point});
              current_point = {x, y};
              pre_control_point = point_c;
            }
            // std::cout << "INFO: done T\n";
          } break;
          case 't': {
            // std::cout << "INFO: start t\n";
            double x = read_double(&value);
            double y = read_double(&value);
            Point point_n = {current_point.x + x, current_point.y + y};
            Point point_c = {current_point.x - pre_control_point.x + current_point.x,
                             current_point.y - pre_control_point.y + current_point.y};
            //control point will be the previous control point
            this->bezier_list.push(BezierCurve{current_point, point_n, point_c, point_c});
            current_point.x += x;
            current_point.y += y;
            pre_control_point = point_c;

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              x = read_double(&value);
              y = read_double(&value);
              Point point_n = {current_point.x + x, current_point.y + y};
              Point point_c = {current_point.x - pre_control_point.x + current_point.x,
                               current_point.y - pre_control_point.y + current_point.y};
              //control point will be the previous control point
              this->bezier_list.push(BezierCurve{current_point, point_n, point_c, point_c});
              current_point.x += x;
              current_point.y += y;
              pre_control_point = point_c;
            }
            // std::cout << "INFO: done t\n";
          } break;
          case 'A': {
            double rx, ry;
            double angle_degree;  
            int large_arc_flag;   
            int sweep_flag;       
            Point point_n;

            rx = read_double(&value);
            ry = read_double(&value);
            angle_degree = read_double(&value);  
            large_arc_flag = (int)read_double(&value);   
            sweep_flag = (int)read_double(&value);   
            point_n.x = read_double(&value);
            point_n.y = read_double(&value);
            this->bezier_list.push(BezierCurve{current_point, point_n, current_point, point_n});

            current_point = point_n;
            pre_control_point = current_point;

            while (!value.empty() && !is_next_command(value[0]) )  {
              rx = read_double(&value);
              ry = read_double(&value);
              angle_degree = read_double(&value);  
              large_arc_flag = (int)read_double(&value);   
              sweep_flag = (int)read_double(&value);   
              point_n.x = read_double(&value);
              point_n.y = read_double(&value);

              this->bezier_list.push(BezierCurve{current_point, point_n, current_point, point_n});

              current_point = point_n;
              pre_control_point = current_point;
            }

            (void)rx;
            (void)ry;
            (void)angle_degree;
            (void)large_arc_flag;
            (void)sweep_flag;

          } break;
          case 'a': {
            double rx, ry;
            double angle_degree;  
            int large_arc_flag;   
            int sweep_flag;       
            Point point_n;

            rx = read_double(&value);
            ry = read_double(&value);
            angle_degree = read_double(&value);  
            large_arc_flag = (int)read_double(&value);   
            sweep_flag = (int)read_double(&value);   
            point_n.x = current_point.x + read_double(&value);
            point_n.y = current_point.y + read_double(&value);

            this->bezier_list.push(BezierCurve{current_point, point_n, current_point, point_n});

            current_point = point_n;
            pre_control_point = current_point;

            while (!value.empty() && !is_next_command(value[0]) )  {
              rx = read_double(&value);
              ry = read_double(&value);
              angle_degree = read_double(&value);  
              large_arc_flag = (int)read_double(&value);   
              sweep_flag = (int)read_double(&value);   
              point_n.x = current_point.x + read_double(&value);
              point_n.y = current_point.y + read_double(&value);
              this->bezier_list.push(BezierCurve{current_point, point_n, current_point, point_n});

              current_point = point_n;
              pre_control_point = current_point;
            }

            (void)rx;
            (void)ry;
            (void)angle_degree;
            (void)large_arc_flag;
            (void)sweep_flag;
          } break;
          case 'Z': 
          case 'z': {

            this->bezier_list.push(BezierCurve{current_point, start_point, current_point, start_point});

            while (!value.empty() && (isspace(value[0]) || value[0] == ',')) {
              value = value.substr(1);
            }
            // std::cout << "INFO: done Z\n";
          } break;
          default: {
            __builtin_unreachable();
          }
        }
      }
    }
  }
}

void Path::render(Gdiplus::Graphics *graphics) const {
  Gdiplus::FillMode fillmode;
  switch (this->fill_rule) {
    case FILL_RULE_NONZERO:
      fillmode = Gdiplus::FillModeWinding;
      break;
    case FILL_RULE_EVENODD:
      fillmode = Gdiplus::FillModeAlternate;
      break;
    case FILL_RULE_COUNT:
       __builtin_unreachable();
  }

  Gdiplus::GraphicsPath path_list = {fillmode};

  path_list.AddBezier((Gdiplus::REAL)this->bezier_list[0].point_0.x,
                      (Gdiplus::REAL)this->bezier_list[0].point_0.y,
                      (Gdiplus::REAL)this->bezier_list[0].point_CS.x,
                      (Gdiplus::REAL)this->bezier_list[0].point_CS.y,
                      (Gdiplus::REAL)this->bezier_list[0].point_CE.x,
                      (Gdiplus::REAL)this->bezier_list[0].point_CE.y,
                      (Gdiplus::REAL)this->bezier_list[0].point_N.x,
                      (Gdiplus::REAL)this->bezier_list[0].point_N.y);

  Point last_point = this->bezier_list[0].point_N;

  Gdiplus::Matrix matrix = {
    (Gdiplus::REAL)this->transform[0][0],
    (Gdiplus::REAL)this->transform[0][1],
    (Gdiplus::REAL)this->transform[1][0],
    (Gdiplus::REAL)this->transform[1][1],
    (Gdiplus::REAL)this->transform[0][2],
    (Gdiplus::REAL)this->transform[1][2]
  };

  for (uint32_t i = 1; i < this->bezier_list.len(); ++i){
    BezierCurve curve = this->bezier_list[i];

    if (last_point.x != curve.point_0.x || last_point.y != curve.point_0.y){
      path_list.StartFigure();
    }

    path_list.AddBezier((Gdiplus::REAL)curve.point_0.x, (Gdiplus::REAL)curve.point_0.y,
                        (Gdiplus::REAL)curve.point_CS.x, (Gdiplus::REAL)curve.point_CS.y,
                        (Gdiplus::REAL)curve.point_CE.x, (Gdiplus::REAL)curve.point_CE.y,
                        (Gdiplus::REAL)curve.point_N.x, (Gdiplus::REAL)curve.point_N.y);
    last_point = curve.point_N;
  }

  path_list.Transform(&matrix);

  if (this->fill_brush) {
    graphics->FillPath(this->fill_brush.get(), &path_list);
  }

  if (this->stroke_brush) {
    Gdiplus::Pen pen = {this->stroke_brush.get(), (float)this->stroke_width};
    graphics->DrawPath(&pen, &path_list);
  }
}
