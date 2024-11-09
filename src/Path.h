#ifndef PATH_H
#define PATH_H

#include "BaseShape.h"  
#include "ArrayList.h"
#include "Point.h"

//class BaseCommand{ //what will in the base command?
//public:
//  //Move: M, m
//  //LineTo: L, l, H, h, V, v
//  //Bezier: C, c, S, s ... Quadratic: Q, q, T, t
//  //Elliptic: A, a
//  //ClosePath: Z, z
//  //std::unique_ptr<BaseCommand> next_command;
//  //virtual void draw() const = 0;
//  //virtual ~BaseCommand() = default;
//};

namespace SVGShapes {


class CommandBezier {
public:
  Point point_0;       // point start
  Point point_N;      // point end
  Point point_CS;     // point control start
  Point point_CE;     //point control end
  // void draw() const;
};


//class CommandLine final : public BaseCommand{
//public:
//  Point point_0;       // point start
//  Point point_1;      // point end
//  CommandLine(Point p0, Point p1);  
//  void draw() const override;
//};
//
//class CommandEllipse final: public BaseCommand{
//public:
//  Point current_point;
//  Point point_end;      //the end point, need to Move before draw
//  double rx;
//  double ry;
//  double angle_degree;   //rotation of ellipse relative to x-axis
//  int large_arc_flag;   // 2 types of value: 0 & 1, use bool? 
//  int sweep_flag;       // 1 as cockwise, 0 as counter
//  //The center of ellipse will be automatically determined
//  
//  CommandEllipse(Point current_point, Point point_end, double rx, double ry, double angle_degree, int large_arc_flag, int sweep_flag);
//  void draw() const override;
//};

//class CommandMove final : public BaseCommand{
//public:
//  double x;
//  double y;
//  CommandMove(double x, double y);
//  void draw() const override;
//};

//class CommandClosePath final : public BaseCommand{
//public:
//  Point start_point;
//  Point current_point;
//  CommandClosePath(Point start_point, Point current_point);
//  void draw() const override;
//};


class Path: public BaseShape{
public:
  ArrayList<CommandBezier> bezier_list;
  Path(Attribute *attrs, int attrs_countt, BaseShape *parent);
  void render(Gdiplus::Graphics *graphics) const override;
};

};

#endif
