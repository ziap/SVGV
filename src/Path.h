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
};

enum PathCommand{
	PATH_COMMAND_MOVE = 0,
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
};


class CommandBezier: public BaseCommand{
public:
	Point point_0; 			// point start
	Point point_N;			// point end
	Point point_CS; 		// point control start
	Point point_CE; 		//point control end
	CommandBezier();	
};


class CommandEllipse: public BaseCommand{
public:
	Point end_point;			//the end point, need to Move before draw
	Point radi_point;
	double angle_degree; 	//rotation of ellipse relative to x-axis
	int large_arc_flag;   // 2 types of value: 0 & 1, use bool? 
	int sweep_flag; 			// 1 as cockwise, 0 as counter
	//The center of ellipse will be automatically determined
	
	CommandEllipse();
};

class CommandMove: public BaseCommand{
public:
	double x;
	double y;

	CommandMove();
};

class CommandClosePath: public BaseCommand{
public:
	double x;
	double y;

	CommandClosePath();
};



class Path: public BaseShape{
public:
	PathCommand command;
	Path(Attribute *attrs, int attrs_countt, BaseShape *parent);
	void draw(); 
};


#endif
