#ifndef GDIPLUS_RENDERER_H
#define GDIPLUS_RENDERER_H

#include "GdiplusFragment.h"
#include <deque>

class GdiplusRenderer {
public:
  GdiplusRenderer(int init_width, int init_height);
  
  bool load_file(const char *filename);

  void render(Gdiplus::Graphics *graphics);

  void drag_start(Point pos);
  bool drag_move(Point pos);
  void drag_end();

  void zoom(double delta);
  void resize(int new_width, int new_height);

  void clear();
private:
  std::deque<GdiplusFragment> shapes;

  Point center;
  double scale;
  bool dragging;

  int width;
  int height;

  double view_width;
  double view_height;

  Point mouse_last;
};

#endif
