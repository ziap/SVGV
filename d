[1mdiff --git a/src/Circle.cpp b/src/Circle.cpp[m
[1mindex d9fe20b..0079d79 100644[m
[1m--- a/src/Circle.cpp[m
[1m+++ b/src/Circle.cpp[m
[36m@@ -1,5 +1,41 @@[m
 #include "Circle.h"[m
 [m
[32m+[m[32mstatic double read_circle_attr(std::string_view value) {[m
[32m+[m[32m  double db_attrs;[m
[32m+[m[32m  if (value[value.size() - 1] == '%') {[m
[32m+[m[32m    std::from_chars(value.data(), value.data() + value.size() - 1, db_attrs);[m
[32m+[m[32m    db_attrs/= 100;[m
[32m+[m[32m  } else std::from_chars(value.data(), value.data() + value.size(), db_attrs);[m
[32m+[m
[32m+[m[32m  return db_attrs;[m
[32m+[m[32m}[m
[32m+[m
 Circle::Circle(Attribute *attrs, int attrs_count, BaseShape *parent)[m
   : BaseShape(attrs, attrs_count, parent) {[m
[32m+[m	[32mthis->cx = 0;[m
[32m+[m	[32mthis->cy = 0;[m
[32m+[m	[32mthis->r = 0;[m
[32m+[m
[32m+[m	[32mfor(int i = 0; i < attrs_count + CIRCLE_ATTR_COUNT; ++i){[m
[32m+[m		[32mstd::string_view key = attrs[i].key;[m
[32m+[m		[32mstd::string_view value = attrs[i].value;[m
[32m+[m			[32mCircleAttr attr = (CircleAttr)inv_circle_attribute[key];[m
[32m+[m			[32mswitch(attr){[m
[32m+[m				[32mcase CIRCLE_ATTR_CX: {[m
[32m+[m					[32mthis->cx = read_circle_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m
[32m+[m				[32mcase CIRCLE_ATTR_CY: {[m
[32m+[m					[32mthis->cy = read_circle_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m
[32m+[m				[32mcase CIRCLE_ATTR_R: {[m
[32m+[m					[32mthis->r = read_circle_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m
[32m+[m				[32mcase CIRCLE_ATTR_COUNT: {[m
[32m+[m					[32m__builtin_unreachable();[m
[32m+[m				[32m}[m
[32m+[m			[32m}[m
[32m+[m	[32m}[m
 }[m
[1mdiff --git a/src/Circle.h b/src/Circle.h[m
[1mindex fc6e8d4..f2c317a 100644[m
[1m--- a/src/Circle.h[m
[1m+++ b/src/Circle.h[m
[36m@@ -2,9 +2,31 @@[m
 #define CIRCLE_H[m
 [m
 #include "BaseShape.h"[m
[32m+[m[32m#include "ArrayList.h"[m
[32m+[m[32m#include <charconv>[m
[32m+[m[32m#include <memory>[m
[32m+[m
[32m+[m[32menum CircleAttr{[m
[32m+[m	[32mCIRCLE_ATTR_CX = 0,[m
[32m+[m	[32mCIRCLE_ATTR_CY,[m
[32m+[m	[32mCIRCLE_ATTR_R,[m
[32m+[m	[32mCIRCLE_ATTR_COUNT,[m
[32m+[m[32m};[m
[32m+[m
[32m+[m[32mconstexpr std::string_view circle_attr_name[CIRCLE_ATTR_COUNT]{[m
[32m+[m	[32m"cx",[m[41m [m
[32m+[m	[32m"cy",[m
[32m+[m	[32m"r",[m
[32m+[m[32m};[m
[32m+[m
[32m+[m[32mconstexpr InverseIndex<CIRCLE_ATTR_COUNT> inv_circle_attribute= {&circle_attr_name};[m
 [m
 class Circle : public BaseShape {[m
 public:[m
[32m+[m	[32mdouble cx;[m
[32m+[m	[32mdouble cy;[m
[32m+[m	[32mdouble r;[m
[32m+[m
   Circle(Attribute *attrs, int attrs_count, BaseShape *parent);[m
 };[m
 [m
[1mdiff --git a/src/Ellipse.cpp b/src/Ellipse.cpp[m
[1mindex fd2d7c2..c8bc8a2 100644[m
[1m--- a/src/Ellipse.cpp[m
[1m+++ b/src/Ellipse.cpp[m
[36m@@ -1,5 +1,45 @@[m
 #include "Ellipse.h"[m
 [m
[32m+[m[32mstatic double read_ellipse_attr(std::string_view value) {[m
[32m+[m[32m  double db_attrs;[m
[32m+[m[32m  if (value[value.size() - 1] == '%') {[m
[32m+[m[32m    std::from_chars(value.data(), value.data() + value.size() - 1, db_attrs);[m
[32m+[m[32m    db_attrs/= 100;[m
[32m+[m[32m  } else std::from_chars(value.data(), value.data() + value.size(), db_attrs);[m
[32m+[m
[32m+[m[32m  return db_attrs;[m
[32m+[m[32m}[m
[32m+[m
 Ellipse::Ellipse(Attribute *attrs, int attrs_count, BaseShape *parent)[m
   : BaseShape(attrs, attrs_count, parent) {[m
[32m+[m	[32mthis->cx = 0;[m
[32m+[m	[32mthis->cy = 0;[m
[32m+[m	[32mthis->rx = 0;[m
[32m+[m	[32mthis->ry = 0;[m
[32m+[m	[32mfor(int i = 0; i < attrs_count + ELLIPSE_ATTR_COUNT; ++i){[m
[32m+[m		[32mstd::string_view key = attrs[i].key;[m
[32m+[m		[32mstd::string_view value = attrs[i].value;[m
[32m+[m			[32mEllipseAttr attr = (EllipseAttr)inv_ellipse_attribute[key];[m
[32m+[m			[32mswitch(attr){[m
[32m+[m				[32mcase ELLIPSE_ATTR_CX: {[m
[32m+[m					[32mthis->cx = read_ellipse_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m
[32m+[m				[32mcase ELLIPSE_ATTR_CY: {[m
[32m+[m					[32mthis->cy = read_ellipse_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m
[32m+[m				[32mcase ELLIPSE_ATTR_RX: {[m
[32m+[m					[32mthis->rx = read_ellipse_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m
[32m+[m				[32mcase ELLIPSE_ATTR_RY: {[m
[32m+[m					[32mthis->ry = read_ellipse_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m
[32m+[m				[32mcase ELLIPSE_ATTR_COUNT: {[m
[32m+[m					[32m__builtin_unreachable();[m
[32m+[m				[32m}[m
[32m+[m			[32m}[m
[32m+[m	[32m}[m
 }[m
[1mdiff --git a/src/Ellipse.h b/src/Ellipse.h[m
[1mindex c19720e..0bc4b65 100644[m
[1m--- a/src/Ellipse.h[m
[1m+++ b/src/Ellipse.h[m
[36m@@ -2,9 +2,32 @@[m
 #define ELLIPSE_H[m
 [m
 #include "BaseShape.h"[m
[32m+[m[32m#include "ArrayList.h"[m
[32m+[m[32m#include <charconv>[m
[32m+[m[32m#include <memory>[m
 [m
[32m+[m[32menum EllipseAttr{[m
[32m+[m	[32mELLIPSE_ATTR_CX = 0,[m
[32m+[m	[32mELLIPSE_ATTR_CY,[m
[32m+[m	[32mELLIPSE_ATTR_RX,[m
[32m+[m	[32mELLIPSE_ATTR_RY,[m
[32m+[m	[32mELLIPSE_ATTR_COUNT,[m
[32m+[m[32m};[m
[32m+[m
[32m+[m[32mconstexpr std::string_view ellipse_attr_name[ELLIPSE_ATTR_COUNT]{[m
[32m+[m	[32m"cx",[m[41m [m
[32m+[m	[32m"cy",[m
[32m+[m	[32m"rx",[m
[32m+[m	[32m"ry",[m
[32m+[m[32m};[m
[32m+[m
[32m+[m[32mconstexpr InverseIndex<ELLIPSE_ATTR_COUNT> inv_ellipse_attribute= {&ellipse_attr_name};[m
 class Ellipse : public BaseShape {[m
 public:[m
[32m+[m	[32mdouble cx;[m
[32m+[m	[32mdouble cy;[m
[32m+[m	[32mdouble rx;[m
[32m+[m	[32mdouble ry;[m
   Ellipse(Attribute *attrs, int attrs_count, BaseShape *parent);[m
 };[m
 [m
[1mdiff --git a/src/Path.h b/src/Path.h[m
[1mindex 7092a2c..c6ca7ae 100644[m
[1m--- a/src/Path.h[m
[1m+++ b/src/Path.h[m
[36m@@ -1,11 +1,63 @@[m
 #ifndef PATH_H[m
 #define PATH_H[m
 [m
[31m-#include "BaseShape.h"[m
[32m+[m[32m#include "BaseShape.h"[m[41m	[m
[32m+[m[32m#include "ArrayList.h"[m
[32m+[m[32m#include <charconv>[m
[32m+[m[32m#include <memory>[m
 [m
[31m-class Path : public BaseShape {[m
[32m+[m[32mclass Point{[m
 public:[m
[31m-  Path(Attribute *attrs, int attrs_count, BaseShape *parent);[m
[32m+[m	[32mint x;[m
[32m+[m	[32mint y;[m
[32m+[m[32m};[m
[32m+[m
[32m+[m
[32m+[m[32mclass BaseCommand{[m
[32m+[m[32mpublic:[m
[32m+[m[41m	[m
[32m+[m[32m};[m
[32m+[m
[32m+[m
[32m+[m[32mclass CommandBezier: public BaseCommand{[m
[32m+[m[32mpublic:[m
[32m+[m	[32mPoint p0, pN, pCS, pCE;[m
[32m+[m
[32m+[m[32m};[m
[32m+[m
[32m+[m
[32m+[m[32mclass CommandEllipse: public BaseCommand{[m
[32m+[m[32mpublic:[m
[32m+[m	[32mPoint[m[41m 	[m
[32m+[m[32m};[m
[32m+[m
[32m+[m[32mclass CommandMove: public BaseCommand{[m
[32m+[m[32mpublic:[m
[32m+[m	[32mdouble x;[m
[32m+[m	[32mdouble y;[m
[32m+[m[32m};[m
[32m+[m[32mclass CommandClosePath: public BaseCommand{[m
[32m+[m[32mpublic:[m
[32m+[m	[32mdouble x;[m
[32m+[m	[32mdouble y;[m
[32m+[m[32m};[m
[32m+[m
[32m+[m
[32m+[m[32menum PathCommand{[m
[32m+[m	[32mCOMMAND_MOVE = 0,[m
[32m+[m	[32mCOMMAND_BEZIER,[m
[32m+[m	[32mCOMMAND_ELLIPTIC,[m
[32m+[m	[32mCOMMAND_CLOSE_PATH[m
[32m+[m[32m};[m
[32m+[m
[32m+[m[32mclass Path: public BaseShape{[m
[32m+[m[32mpublic:[m
[32m+[m	[32mPathCommand command;[m
[32m+[m	[32mvoid read_xml_node(XMLNode* node){[m
[32m+[m		[32mBaseShape::read_xml_node(node);[m
[32m+[m[41m		[m
[32m+[m
[32m+[m	[32m}[m
 };[m
 [m
 #endif[m
[1mdiff --git a/src/Polygon.cpp b/src/Polygon.cpp[m
[1mindex 2f732d6..c3b45c6 100644[m
[1m--- a/src/Polygon.cpp[m
[1m+++ b/src/Polygon.cpp[m
[36m@@ -1,5 +1,27 @@[m
 #include "Polygon.h"[m
 [m
[31m-Polygon::Polygon(Attribute *attrs, int attrs_count, BaseShape *parent)[m
[31m-  : BaseShape(attrs, attrs_count, parent) {[m
[32m+[m[32mvoid read_point(std::string_view str, ArrayList<Point>* points){[m
[32m+[m	[32mPoint new_point;[m
[32m+[m[32m  std::from_chars_result res;[m
[32m+[m	[32mwhile(str.size() != 0)[m
[32m+[m	[32m{[m
[32m+[m		[32mres  = std::from_chars(str.data(), str.data() + str.size(), new_point.x);[m[41m [m
[32m+[m		[32mstr = str.substr(res.ptr - str.data());[m
[32m+[m[41m		[m
[32m+[m		[32mres  = std::from_chars(str.data(), str.data() + str.size(), new_point.y);[m[41m [m
[32m+[m		[32mstr = str.substr(res.ptr - str.data());[m
[32m+[m		[32mpoints->push(new_point);[m
[32m+[m	[32m}[m
[32m+[m[32m}[m
[32m+[m
[32m+[m[32mPolygon::Polygon(Attribute *attrs, int attrs_count, BaseShape *parent) : BaseShape(attrs, attrs_count, parent) {[m
[32m+[m		[32mthis->point_length = 0;[m
[32m+[m[41m		[m
[32m+[m		[32mfor (int i = 0; i < attrs_count + 1; ++i){[m
[32m+[m			[32mstd::string_view key = attrs[i].key;[m
[32m+[m			[32mstd::string_view value = attrs[i].value;[m
[32m+[m			[32mif (key == point_name){[m
[32m+[m				[32mread_point(value, &point_list);[m
[32m+[m			[32m}[m
[32m+[m	[32m}[m
 }[m
[1mdiff --git a/src/Polygon.h b/src/Polygon.h[m
[1mindex bf77dfb..82c4d69 100644[m
[1m--- a/src/Polygon.h[m
[1m+++ b/src/Polygon.h[m
[36m@@ -1,11 +1,30 @@[m
 #ifndef POLYGON_H[m
 #define POLYGON_H[m
 [m
[31m-#include "BaseShape.h"[m
[32m+[m[32m#include "BaseShape.h"[m[41m	[m
[32m+[m[32m#include "ArrayList.h"[m
[32m+[m[32m#include <charconv>[m
[32m+[m[32m#include <memory>[m
 [m
[31m-class Polygon : public BaseShape {[m
[32m+[m[32mclass Point{[m
 public:[m
[31m-  Polygon(Attribute *attrs, int attrs_count, BaseShape *parent);[m
[32m+[m	[32mint x;[m
[32m+[m	[32mint y;[m
 };[m
 [m
[32m+[m[32mclass Polygon: public BaseShape{[m
[32m+[m[32mpublic:[m
[32m+[m	[32mint point_length;[m
[32m+[m	[32mstd::string_view point_name = "points";[m
[32m+[m	[32mshort POINT_KEY = 0;[m
[32m+[m	[32mArrayList<Point> point_list;[m
[32m+[m	[32m//std::unique_ptr<Point[]> point_start;[m
[32m+[m
[32m+[m	[32mPolygon(Attribute *attrs, int attrs_count, BaseShape *parent);[m
[32m+[m
[32m+[m[32m};[m
[32m+[m
[32m+[m
[32m+[m
[32m+[m
 #endif[m
[1mdiff --git a/src/Polyline.cpp b/src/Polyline.cpp[m
[1mindex 43568eb..102e1ca 100644[m
[1m--- a/src/Polyline.cpp[m
[1m+++ b/src/Polyline.cpp[m
[36m@@ -1,5 +1,24 @@[m
 #include "Polyline.h"[m
 [m
[32m+[m[32mvoid read_point(std::string_view str, ArrayList<Point>* points){[m
[32m+[m	[32mPoint new_point;[m
[32m+[m[32m  std::from_chars_result res;[m
[32m+[m	[32mwhile(str.size() != 0)[m
[32m+[m	[32m{[m
[32m+[m		[32mres  = std::from_chars(str.data(), str.data() + str.size(), new_point.x);[m[41m [m
[32m+[m		[32mstr = str.substr(res.ptr - str.data());[m
[32m+[m[41m		[m
[32m+[m		[32mres  = std::from_chars(str.data(), str.data() + str.size(), new_point.y);[m[41m [m
[32m+[m		[32mstr = str.substr(res.ptr - str.data());[m
[32m+[m		[32mpoints->push(new_point);[m
[32m+[m	[32m}[m
[32m+[m[32m}[m
[32m+[m
[32m+[m
 Polyline::Polyline(Attribute *attrs, int attrs_count, BaseShape *parent)[m
   : BaseShape(attrs, attrs_count, parent) {[m
[32m+[m	[32mthis->point_length = 0;[m
[32m+[m	[32mfor (int i = 0; i < attrs_count + 1; ++i){[m
[32m+[m[41m		[m
[32m+[m	[32m}[m
 }[m
[1mdiff --git a/src/Polyline.h b/src/Polyline.h[m
[1mindex 70c9560..0a1a63e 100644[m
[1m--- a/src/Polyline.h[m
[1m+++ b/src/Polyline.h[m
[36m@@ -2,9 +2,24 @@[m
 #define POLYLINE_H[m
 [m
 #include "BaseShape.h"[m
[32m+[m[32m#include "BaseShape.h"[m[41m	[m
[32m+[m[32m#include "ArrayList.h"[m
[32m+[m[32m#include <charconv>[m
[32m+[m[32m#include <memory>[m
[32m+[m
[32m+[m[32mclass Point{[m
[32m+[m[32mpublic:[m
[32m+[m	[32mint x;[m
[32m+[m	[32mint y;[m
[32m+[m[32m};[m
 [m
 class Polyline : public BaseShape {[m
 public:[m
[32m+[m	[32mint point_length;[m
[32m+[m	[32mstd::string_view point_name = "points";[m
[32m+[m	[32mshort POINT_KEY = 0;[m
[32m+[m	[32mArrayList<Point> point_list;[m
[32m+[m
   Polyline(Attribute *attrs, int attrs_count, BaseShape *parent);[m
 };[m
 [m
[1mdiff --git a/src/Rect.cpp b/src/Rect.cpp[m
[1mindex 17c71d5..cd7f12f 100644[m
[1m--- a/src/Rect.cpp[m
[1m+++ b/src/Rect.cpp[m
[36m@@ -1,5 +1,59 @@[m
 #include "Rect.h"[m
 [m
[32m+[m[32mstatic double read_rect_attr(std::string_view value) {[m
[32m+[m[32m  double db_attrs;[m
[32m+[m[32m  if (value[value.size() - 1] == '%') {[m
[32m+[m[32m    std::from_chars(value.data(), value.data() + value.size() - 1, db_attrs);[m
[32m+[m[32m    db_attrs/= 100;[m
[32m+[m[32m  } else std::from_chars(value.data(), value.data() + value.size(), db_attrs);[m
[32m+[m
[32m+[m[32m  return db_attrs;[m
[32m+[m[32m}[m
[32m+[m
 Rect::Rect(Attribute *attrs, int attrs_count, BaseShape *parent)[m
   : BaseShape(attrs, attrs_count, parent) {[m
[32m+[m
[32m+[m		[32mthis->x = 0;[m
[32m+[m		[32mthis->y = 0;[m
[32m+[m		[32mthis->width = 0;[m
[32m+[m		[32mthis->height = 0;[m
[32m+[m		[32mthis->rx = 0;[m
[32m+[m		[32mthis->ry = 0;[m
[32m+[m[41m	[m
[32m+[m		[32m//go through all of attributes readed, read the rect's attrs[m
[32m+[m		[32mfor (int i = 0; i < attrs_count + RECT_ATTR_COUNT; ++i){[m
[32m+[m			[32mstd::string_view key = attrs[i].key;[m
[32m+[m			[32mstd::string_view value = attrs[i].value;[m
[32m+[m
[32m+[m			[32mRectAttr attr = (RectAttr)inv_rect_attribute[key];[m
[32m+[m			[32mswitch(attr){[m
[32m+[m				[32mcase RECT_ATTR_X: {[m
[32m+[m					[32mthis->x = read_rect_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m
[32m+[m				[32mcase RECT_ATTR_Y: {[m
[32m+[m					[32mthis->y = read_rect_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m
[32m+[m				[32mcase RECT_ATTR_RX: {[m
[32m+[m					[32mthis->rx = read_rect_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m
[32m+[m				[32mcase RECT_ATTR_RY: {[m
[32m+[m					[32mthis->ry = read_rect_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m[41m														[m
[32m+[m				[32mcase RECT_ATTR_WIDTH: {[m
[32m+[m					[32mthis->width = read_rect_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m
[32m+[m				[32mcase RECT_ATTR_HEIGHT: {[m
[32m+[m					[32mthis->height = read_rect_attr(value);[m
[32m+[m				[32m} break;[m
[32m+[m
[32m+[m				[32mcase RECT_ATTR_COUNT: {[m
[32m+[m					[32m__builtin_unreachable();[m
[32m+[m				[32m}[m
[32m+[m			[32m}[m
[32m+[m		[32m}[m
 }[m
[1mdiff --git a/src/Rect.h b/src/Rect.h[m
[1mindex 5edecca..7240445 100644[m
[1m--- a/src/Rect.h[m
[1m+++ b/src/Rect.h[m
[36m@@ -1,11 +1,43 @@[m
 #ifndef RECT_H[m
 #define RECT_H[m
 [m
[31m-#include "BaseShape.h"[m
[32m+[m[32m#include "BaseShape.h"[m[41m	[m
[32m+[m[32m#include "ArrayList.h"[m
[32m+[m[32m#include <charconv>[m
[32m+[m[32m#include <memory>[m
 [m
[31m-class Rect : public BaseShape {[m
[32m+[m[32menum RectAttr{[m
[32m+[m	[32mRECT_ATTR_X = 0,[m
[32m+[m	[32mRECT_ATTR_Y,[m
[32m+[m	[32mRECT_ATTR_RX,[m
[32m+[m	[32mRECT_ATTR_RY,[m
[32m+[m	[32mRECT_ATTR_WIDTH,[m
[32m+[m	[32mRECT_ATTR_HEIGHT,[m
[32m+[m	[32mRECT_ATTR_COUNT,[m
[32m+[m[32m};[m
[32m+[m
[32m+[m[32mconstexpr std::string_view rect_attr_name[RECT_ATTR_COUNT]{[m
[32m+[m	[32m"x",[m[41m [m
[32m+[m	[32m"y",[m
[32m+[m	[32m"rx",[m
[32m+[m	[32m"rx",[m
[32m+[m	[32m"width",[m
[32m+[m	[32m"height",[m
[32m+[m[32m};[m
[32m+[m
[32m+[m[32mconstexpr InverseIndex<RECT_ATTR_COUNT> inv_rect_attribute = {&rect_attr_name};[m
[32m+[m
[32m+[m[32mclass Rect: public BaseShape{[m
 public:[m
[31m-  Rect(Attribute *attrs, int attrs_count, BaseShape *parent);[m
[32m+[m	[32mdouble x;[m
[32m+[m	[32mdouble y;[m
[32m+[m	[32mdouble rx;[m
[32m+[m	[32mdouble ry;[m
[32m+[m	[32mdouble width;[m
[32m+[m	[32mdouble height;[m
[32m+[m[41m	[m
[32m+[m	[32mRect(Attribute *attrs, int attrs_count, BaseShape *parent);[m
[32m+[m
 };[m
 [m
 #endif[m
