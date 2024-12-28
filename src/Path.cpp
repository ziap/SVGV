#include "Path.h"
#include <cmath>
#include "common.h"
using namespace SVGShapes;


static double get_vector_angle(Point u, Point v){
  double ta = std::atan2(u[1], u[0]);
  double tb = std::atan2(v[1], v[0]);

  if (tb >= ta){
    return tb - ta;
  }
  return (2 * PI) - (ta - tb);
}


ArrayList<BezierCurve> arcs_to_curves(Point point_start, Point point_end, double rx, double ry, double angle_degree, int large_arc_flag, int sweep_flag){
  ArrayList<BezierCurve> arcs_list;
  if (point_start[0] == point_end[0] && point_start[1] == point_end[1]){ 
    return {};
  }

  if (rx == 0.0f && ry == 0.0f){
    arcs_list.push(BezierCurve {point_start, point_end, point_start, point_end});
    return arcs_list;
  }

  double sin_phi = std::sin(angle_degree * PI / 180);
  double cos_phi = std::cos(angle_degree * PI / 180);

  double x1_dash = cos_phi * (point_start[0] - point_end[0]) / 2.0
                 + sin_phi * (point_start[1] - point_end[1]) / 2.0;
  double y1_dash = -sin_phi * (point_start[0] - point_end[0]) / 2.0
                 + cos_phi * (point_start[1] - point_end[1]) / 2.0;
  double root;
  double numerator = rx * rx * ry * ry - rx * rx
                   * y1_dash * y1_dash - ry * ry * x1_dash * x1_dash;

  double radius_x = rx;
  double radius_y = ry;

  if (numerator < 0.0){
    double s = std::sqrt(1.0 - numerator / (rx * rx * ry * ry));
    radius_x *= s;
    radius_y *= s;
    root = 0.0;
  } else{
    root = ((large_arc_flag == 1 && sweep_flag == 1) || (large_arc_flag == 0 && sweep_flag == 0) ? -1.0 : 1.0) 
         * std::sqrt(numerator / (rx * rx * y1_dash * y1_dash + ry * ry * x1_dash * x1_dash));
  }

  double cx_dash = root * radius_x * y1_dash / radius_y;
  double cy_dash = -root * radius_y * x1_dash / radius_x;

  double cx = cos_phi * cx_dash - sin_phi * cy_dash + (point_start[0] + point_end[0]) / 2.0;
  double cy = sin_phi * cx_dash + cos_phi * cy_dash + (point_start[1] + point_end[1]) / 2.0;

  double theta1 = get_vector_angle(
    Point{1.0, 0.0},
    Point{(x1_dash - cx_dash) / radius_x, (y1_dash - cy_dash) / radius_y}
  );
  double dtheta = get_vector_angle(
    Point{(x1_dash - cx_dash) / radius_x, (y1_dash - cy_dash) / radius_y},
    Point{(-x1_dash - cx_dash) / radius_x, (-y1_dash - cy_dash) / radius_y}
  ); 

  if (sweep_flag == 0 && dtheta > 0){
    dtheta -= 2.0 * PI;
  } else if (sweep_flag == 1 && dtheta < 0){
    dtheta += 2.0 * PI;
  }

  double segments = (int)std::ceil((double)std::abs(dtheta / (PI / 2.0)));
  double delta = dtheta / segments;
  double t = 8.0 / 3.0 * std::sin(delta / 4.0) * std::sin(delta / 4.0) / std::sin(delta / 2.0);

  double startX = point_start[0];
  double startY = point_start[1];

  for (int i = 0; i < segments; ++i){
    double cos_theta1 = std::cos(theta1);
    double sin_theta1 = std::sin(theta1);
    double theta2 = theta1 + delta;
    double cos_theta2 = std::cos(theta2);
    double sin_theta2 = std::sin(theta2);

    double end_point_X = cos_phi * radius_x * cos_theta2 - sin_phi
                         * radius_y * sin_theta2 + cx;
    double end_point_Y = sin_phi * radius_x * cos_theta2 + cos_phi
                         * radius_y * sin_theta2 + cy;
                         
    double dx1 = t * (-cos_phi * radius_x * sin_theta1 - sin_phi * radius_y
                 * cos_theta1);
    double dy1 = t * (-sin_phi * radius_x * sin_theta1 + cos_phi * radius_y
                 * cos_theta1);
    
    double dxe = t * (cos_phi * radius_x * sin_theta2 + sin_phi * radius_y
                 * cos_theta2);
    double dye = t * (sin_phi * radius_x * sin_theta2 - cos_phi * radius_y
                 * cos_theta2);
    if (i == 0) {
      startX = point_start[0];
      startY = point_start[1];
    } else if (i == segments - 1){
      end_point_X = point_end[0];
      end_point_Y = point_end[1]; 
    }

    arcs_list.push(BezierCurve {
      Point{startX, startY},
      Point{end_point_X, end_point_Y},
      Point{startX + dx1, startY + dy1},
      Point{end_point_X + dxe, end_point_Y + dye}
    });

    theta1 = theta2;
    startX = end_point_X;
    startY = end_point_Y;
  }
   
  return arcs_list;
}

static bool is_next_command(char chr) {
  switch (chr) {
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

Path::Path(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles)
  : BaseShape(attrs, attrs_count, parent, styles) {

  for (int i = 0; i < attrs_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
    if (key == "d") {
      Point current_point;
      Point start_point;
      Point pre_control_point;

      value = trim_start(value);
      while (!value.empty()) {
        char command = value[0];
        value = value.substr(1);
        switch (command) {
          case 'M': {
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
              Point mid_point = (current_point + des_point) / 2;
              this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});

              current_point = {x, y};
              pre_control_point = current_point; 
            }
          } break;

          case 'm': {
            double dx = read_double(&value);
            double dy = read_double(&value);
            current_point[0] += dx;
            current_point[1] += dy;
            start_point = current_point;
            pre_control_point = current_point; 
              
            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              dx = read_double(&value);
              dy = read_double(&value);
              Point des_point = {current_point[0] + dx, current_point[1] + dy};
              Point mid_point = (current_point + des_point) / 2;
              this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});
              //update current
              current_point[0] += dx;
              current_point[1] += dy;
              pre_control_point = current_point; 
            }
          } break;

          case 'L': {
            // read 2 points 
            double x = read_double(&value);
            double y = read_double(&value);
            Point des_point = {x, y};
            Point mid_point = (current_point + des_point) / 2;
            this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});
              //update current

            current_point = des_point;
            pre_control_point = current_point; 

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              x = read_double(&value);
              y = read_double(&value);
              des_point[0] = x;
              des_point[1] = y;
              mid_point = (current_point + des_point) / 2;
              this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});

              current_point = des_point;
              pre_control_point = current_point; 
            }
          } break;

          case 'l': {
            double dx = read_double(&value);
            double dy = read_double(&value);
            Point des_point = {current_point[0] + dx, current_point[1] + dy};
            Point mid_point = (current_point + des_point) / 2;
            this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});

            current_point = des_point;
            pre_control_point = current_point; 
              
            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              double dx = read_double(&value);
              double dy = read_double(&value);
              Point des_point = {current_point[0] + dx, current_point[1] + dy};
              mid_point = (current_point + des_point) / 2;
              this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});
              //update current_point
              current_point = des_point;
              pre_control_point = current_point; 
            }
          } break;

          case 'H': {
            double x = read_double(&value);
            Point des_point = {x, current_point[1]};
            Point mid_point = (current_point + des_point) / 2;
            this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});

            current_point = des_point;
            pre_control_point = current_point; 

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              x = read_double(&value);
              Point des_point = {x, current_point[1]};
              mid_point = (current_point + des_point) / 2;
              this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});

              current_point = des_point;
              pre_control_point = current_point; 
            }
          } break;

          case 'h': {
            double dx = read_double(&value);
            Point des_point = {current_point[0] + dx, current_point[1]};
            Point mid_point = (current_point + des_point) / 2;
            this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});

            current_point = des_point;
            pre_control_point = current_point; 

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              dx = read_double(&value);
              Point des_point = {current_point[0] + dx, current_point[1]};
              mid_point = (current_point + des_point) / 2;
              this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});

              current_point = des_point;
              pre_control_point = current_point; 
            }
          } break;

          case 'V': {
            double y = read_double(&value);
            Point des_point = {current_point[0], y};
            Point mid_point = (current_point + des_point) / 2;
            this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});

            current_point = des_point;
            pre_control_point = current_point; 

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              y = read_double(&value);
              Point des_point = {current_point[0], y};
              mid_point = (current_point + des_point) / 2;
              this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});

              current_point = des_point;
              pre_control_point = current_point; 
            }
          } break;

          case 'v': {
            double dy = read_double(&value);
            Point des_point = {current_point[0], current_point[1] + dy};
            Point mid_point = (current_point + des_point) / 2;
            this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});
            current_point = des_point;
            pre_control_point = current_point; 

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              dy = read_double(&value);
              Point des_point = {current_point[0], current_point[1] + dy};
              mid_point = (current_point + des_point) / 2;
              this->bezier_list.push(BezierCurve{current_point, des_point, mid_point, mid_point});

              current_point = des_point;
              pre_control_point = current_point; 
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
            Point point_cs = {x[0] + current_point[0], y[0] + current_point[1]};
            Point point_ce = {x[1] + current_point[0], y[1] + current_point[1]};
            Point point_n = {x[2] + current_point[0], y[2] + current_point[1]};

            this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
            current_point = point_n;
            pre_control_point = point_ce;
            
            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              for (int i = 0; i < 3; ++i) {
                x[i] = read_double(&value);
                y[i] = read_double(&value);
              }
              Point point_cs = {x[0] + current_point[0], y[0] + current_point[1]};
              Point point_ce = {x[1] + current_point[0], y[1] + current_point[1]};
              Point point_n = {x[2] + current_point[0], y[2] + current_point[1]};

              this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
              current_point = point_n;
              pre_control_point = point_ce;
            }
          } break;

          case 'S': {
            double x[2];
            double y[2];
            for (int i = 0; i < 2; ++i) {
              x[i] = read_double(&value);
              y[i] = read_double(&value);
            }
            Point point_ce = {x[0], y[0]};
            Point point_n = {x[1], y[1]};
            Point point_cs = {
              current_point[0] - pre_control_point[0] + current_point[0],
              current_point[1] - pre_control_point[1] + current_point[1],
            };

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
              Point point_cs = {
                current_point[0] - pre_control_point[0] + current_point[0],
                current_point[1] - pre_control_point[1] + current_point[1],
              };

              this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
              current_point = point_n;
              pre_control_point = point_ce;
            }
          } break;

          case 's': {
            double x[2];
            double y[2];
            for (int i = 0; i < 2; ++i) {
              x[i] = read_double(&value);
              y[i] = read_double(&value);
            }
            Point point_ce = {x[0] + current_point[0], y[0] + current_point[1]};
            Point point_n = {x[1] + current_point[0], y[1] + current_point[1]};
            Point point_cs = {
              current_point[0] - pre_control_point[0] + current_point[0],
              current_point[1] - pre_control_point[1] + current_point[1],
            };

            this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
            current_point = point_n;
            pre_control_point = point_ce;
            
            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              for (int i = 0; i < 2; ++i) {
                x[i] = read_double(&value);
                y[i] = read_double(&value);
              }
              Point point_ce = {x[0] + current_point[0], y[0] + current_point[1]};
              Point point_n = {x[1] + current_point[0], y[1] + current_point[1]};
              Point point_cs = {
                current_point[0] - pre_control_point[0] + current_point[0],
                current_point[1] - pre_control_point[1] + current_point[1],
              };

              this->bezier_list.push(BezierCurve{current_point, point_n, point_cs, point_ce});
              current_point = point_n;
              pre_control_point = point_ce;
            }
          } break;

          case 'Q': {
            double x[2];
            double y[2];
            for (int i = 0; i < 2; ++i) {
              x[i] = read_double(&value);
              y[i] = read_double(&value);
            }
            Point point_c = {x[0], y[0]};
            Point point_n = {x[1], y[1]};
            
            Point c1 = (2 * point_c + current_point) / 3;
            Point c2 = (2 * point_c + point_n) / 3;

            this->bezier_list.push(BezierCurve{current_point, point_n, c1, c2});
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

              Point c1 = (2 * point_c + current_point) / 3;
              Point c2 = (2 * point_c + point_n) / 3;

              this->bezier_list.push(BezierCurve{current_point, point_n, c1, c2});
              current_point = point_n;
              pre_control_point = point_c;
            }
          } break;

          case 'q': { //d_point_n, d_point_c (from current point)
            double x[2];
            double y[2];
            for (int i = 0; i < 2; ++i) {
              x[i] = read_double(&value);
              y[i] = read_double(&value);
            }
            Point point_c = {x[0] + current_point[0], y[0] + current_point[1]};
            Point point_n = {x[1] + current_point[0], y[1] + current_point[1]};

            Point c1 = (2 * point_c + current_point) / 3;
            Point c2 = (2 * point_c + point_n) / 3;

            this->bezier_list.push(BezierCurve{current_point, point_n, c1, c2});
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
              Point point_c = {x[0] + current_point[0], y[0] + current_point[1]};
              Point point_n = {x[1] + current_point[0], y[1] + current_point[1]};

              Point c1 = (2 * point_c + current_point) / 3;
              Point c2 = (2 * point_c + point_n) / 3;

              this->bezier_list.push(BezierCurve{current_point, point_n, c1, c2});
              current_point = point_n;
              pre_control_point = point_c;
            }
          } break;

          case 'T': {//quadratic abnormal, Input the end of previous bezier curve 
            double x = read_double(&value);
            double y = read_double(&value);
            Point point_n = {x, y};
            Point point_c = {
              current_point[0] - pre_control_point[0] + current_point[0],
              current_point[1] - pre_control_point[1] + current_point[1],
            };

            Point c1 = (2 * point_c + current_point) / 3;
            Point c2 = (2 * point_c + point_n) / 3;

            this->bezier_list.push(BezierCurve{current_point, point_n, c1, c2});
            current_point = {x, y};
            pre_control_point = point_c;

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              x = read_double(&value);
              y = read_double(&value);
              point_n[0] = x;
              point_n[1] = y;
              Point point_c = {
                current_point[0] - pre_control_point[0] + current_point[0],
                current_point[1] - pre_control_point[1] + current_point[1],
              };

              Point c1 = (2 * point_c + current_point) / 3;
              Point c2 = (2 * point_c + point_n) / 3;
              this->bezier_list.push(BezierCurve{current_point, point_n, c1, c2});
              current_point = {x, y};
              pre_control_point = point_c;
            }
          } break;

          case 't': {
            double x = read_double(&value);
            double y = read_double(&value);
            Point point_n = {current_point[0] + x, current_point[0] + y};
            Point point_c = {
              current_point[0] - pre_control_point[0] + current_point[0],
              current_point[1] - pre_control_point[1] + current_point[1],
            };

            Point c1 = (2 * point_c + current_point) / 3;
            Point c2 = (2 * point_c + point_n) / 3;

            //control point will be the previous control point
            this->bezier_list.push(BezierCurve{current_point, point_n, c1, c2});
            current_point[0] += x;
            current_point[1] += y;
            pre_control_point = point_c;

            //supsequent parameter
            while (!value.empty() && !is_next_command(value[0]) )  {
              x = read_double(&value);
              y = read_double(&value);
              Point point_n = {current_point[0] + x, current_point[1] + y};
              Point point_c = {
                current_point[0] - pre_control_point[0] + current_point[0],
                current_point[1] - pre_control_point[1] + current_point[1],
              };

              Point c1 = (2 * point_c + current_point) / 3;
              Point c2 = (2 * point_c + point_n) / 3;

              //control point will be the previous control point
              this->bezier_list.push(BezierCurve{current_point, point_n, c1, c2});
              current_point[0] += x;
              current_point[1] += y;
              pre_control_point = point_c;
            }
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
            point_n[0] = read_double(&value);
            point_n[1] = read_double(&value);

            this->bezier_list.append(arcs_to_curves(current_point, point_n, rx, ry, angle_degree, large_arc_flag, sweep_flag));
            current_point = this->bezier_list[this->bezier_list.len() - 1].end;

            pre_control_point = current_point;

            while (!value.empty() && !is_next_command(value[0]) )  {
              rx = read_double(&value);
              ry = read_double(&value);
              angle_degree = read_double(&value);  
              large_arc_flag = (int)read_double(&value);   
              sweep_flag = (int)read_double(&value);   
              point_n[0] = read_double(&value);
              point_n[1] = read_double(&value);

              this->bezier_list.append(arcs_to_curves(current_point, point_n, rx, ry, angle_degree, large_arc_flag, sweep_flag));

              current_point = this->bezier_list[this->bezier_list.len() - 1].end;
              pre_control_point = current_point;
            }
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
            point_n[0] = current_point[0] + read_double(&value);
            point_n[1] = current_point[1] + read_double(&value);

            this->bezier_list.append(arcs_to_curves(current_point, point_n, rx, ry, angle_degree, large_arc_flag, sweep_flag));

            current_point = this->bezier_list[this->bezier_list.len() - 1].end;
            pre_control_point = current_point;

            while (!value.empty() && !is_next_command(value[0]) )  {
              rx = read_double(&value);
              ry = read_double(&value);
              angle_degree = read_double(&value);  
              large_arc_flag = (int)read_double(&value);   
              sweep_flag = (int)read_double(&value);   
              point_n[0] = current_point[0] + read_double(&value);
              point_n[1] = current_point[1] + read_double(&value);
              this->bezier_list.append(arcs_to_curves(current_point, point_n, rx, ry, angle_degree, large_arc_flag, sweep_flag));

              current_point = this->bezier_list[this->bezier_list.len() - 1].end;
              pre_control_point = current_point;
            }
          } break;

          case 'Z': 
          case 'z': {
            Point mid_point = (current_point + start_point) / 2;
            this->bezier_list.push(BezierCurve{current_point, start_point, mid_point, mid_point});
            while (!value.empty() && (isspace(value[0]) || value[0] == ',')) {
              value = value.substr(1);
            }
            current_point = start_point;
          } break;
        }
      }
    }
  }
}

ArrayList<BezierCurve> Path::get_beziers() const {
  return this->bezier_list.clone();
}
