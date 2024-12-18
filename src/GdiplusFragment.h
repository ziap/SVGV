#ifndef GDIPLUS_FRAGMENT_H
#define GDIPLUS_FRAGMENT_H

#include <objidl.h>
#include <gdiplus.h>

#include <memory>

#include "parser.h"
#include "BaseShape.h"

class GdiplusFragment {
public:
  GdiplusFragment(const BaseShape *shape, ParseResult *svg);

  void render(Gdiplus::Graphics *graphics);
private:
  std::unique_ptr<const Gdiplus::Brush> fill_brush;
  std::unique_ptr<const Gdiplus::Brush> stroke_brush;
  Gdiplus::Pen pen;
  Gdiplus::GraphicsPath path;
};

#endif
