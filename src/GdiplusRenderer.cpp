#include "GdiplusRenderer.h"

#include <cmath>
#include <fstream>
#include <sstream>

#include "parser.h"

GdiplusRenderer::GdiplusRenderer()
    : shapes{},
      center{0, 0},
      scale{1},
      dragging{false} {}

void GdiplusRenderer::load_file(const char *filename) {
  std::ifstream fin(filename);
  std::ostringstream ss;
  ss << fin.rdbuf();
  this->svg_file = ss.str();
  this->shapes.clear();
  this->center = {0, 0};
  this->scale = 1;
  std::unique_ptr<BaseShape> shapes = parse_xml(this->svg_file);
  for (const BaseShape *shape = shapes.get(); shape; shape = shape->next.get()) {
    this->shapes.emplace_back(shape);
  }
}

void GdiplusRenderer::render(Gdiplus::Graphics *graphics) {
  graphics->TranslateTransform(
    (Gdiplus::REAL)this->center[0],
    (Gdiplus::REAL)this->center[1]
  );
  graphics->ScaleTransform(
    (Gdiplus::REAL)this->scale,
    (Gdiplus::REAL)this->scale
  );
  for (GdiplusFragment &shape : this->shapes) {
    shape.render(graphics);
  }
}

void GdiplusRenderer::drag_start(Point pos) {
  this->mouse_last = pos;
  this->dragging = true;
}

bool GdiplusRenderer::drag_move(Point pos) {
  if (this->dragging) {
    this->center = this->center + pos - this->mouse_last;
  }
  this->mouse_last = pos;
  return this->dragging;
}

void GdiplusRenderer::drag_end() {
  this->dragging = false;
}

void GdiplusRenderer::zoom(double delta) {
  this->center = (this->center - this->mouse_last) / this->scale;
  this->scale *= std::exp2(delta / 3);
  this->center = this->center * this->scale + this->mouse_last;
}
