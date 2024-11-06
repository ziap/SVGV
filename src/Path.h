#ifndef PATH_H
#define PATH_H

#include "BaseShape.h"  
#include "ArrayList.h"
#include <charconv>
#include <memory>

class Point{
public:
  double x;
  double y;
  //Point();
  //Point(const Point &point);
  //Point(double x, double y);
};

enum PathCommand{
  PATH_COMMAND_MOVE = 0,
  PATH_COMMAND_LINETO = 0,
  PATH_COMMAND_BEZIER,
  PATH_COMMAND_ELLIPTIC,
  PATH_COMMAND_CLOSE_PATH,
  PATH_COMMAND_COUNT
};

constexpr std::string_view path_command_name[PATH_COMMAND_COUNT]{
  // Don't know what to do here  
};

class BaseCommand{ //what will in the base command?
public:
  //Move: M, m
  //LineTo: L, l, H, h, V, v
  //Bezier: C, c, S, s ... Quadratic: Q, q, T, t
  //Elliptic: A, a
  //ClosePath: Z, z
  std::unique_ptr<BaseCommand> next_command;
  virtual void draw() const;
};


class CommandBezier: public BaseCommand{
public:
  Point point_0;       // point start
  Point point_N;      // point end
  Point point_CS;     // point control start
  Point point_CE;     //point control end
  CommandBezier(Point point_0, Point point_n, Point point_cs, Point point_ce);  
  void draw() const override;
};


class CommandLine: public BaseCommand{
public:
  Point point_0;       // point start
  Point point_N;      // point end
  CommandLine(double p0_x, double p0_y, double pN_x, double pN_y);  
  void draw() const override;
};

class CommandEllipse: public BaseCommand{
public:
  Point current_point;
  Point point_end;      //the end point, need to Move before draw
  Point point_radi;
  double angle_degree;   //rotation of ellipse relative to x-axis
  int large_arc_flag;   // 2 types of value: 0 & 1, use bool? 
  int sweep_flag;       // 1 as cockwise, 0 as counter
  //The center of ellipse will be automatically determined
  
  CommandEllipse(Point current_point, Point point_end, Point point_radi, double angle_degree, int large_arc_flag, int sweep_flag);
  void draw() const override;
};

class CommandMove: public BaseCommand{
public:
  double x;
  double y;
  CommandMove(double x, double y);
  void draw() const override;
};

class CommandClosePath: public BaseCommand{
public:
  Point start_point;
  Point current_point;
  CommandClosePath(Point start_point, Point current_point);
  void draw() const override;
};


class Path: public BaseShape{
public:
  PathCommand command;
  std::unique_ptr<BaseCommand> head_command;
  Path(Attribute *attrs, int attrs_countt, BaseShape *parent);
  void draw() const override; 
};


#endif
