#ifndef GDIPLUS_RENDERER_H
#define GDIPLUS_RENDERER_H

#include "GdiplusFragment.h"
#include <deque>

class GdiplusRenderer {
public:
  GdiplusRenderer();
  
  void load_file(const char *filename);

  void render(Gdiplus::Graphics *graphics);

  void drag_start(Point pos);
  bool drag_move(Point pos);
  void drag_end();

  void zoom(double delta);
private:
  std::deque<GdiplusFragment> shapes;
  std::string svg_file;

  Point center;
  double scale;
  bool dragging;

  Point mouse_last;
};

#endif
