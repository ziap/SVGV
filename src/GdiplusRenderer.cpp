#include "GdiplusRenderer.h"

#include <cmath>
#include <fstream>
#include <sstream>

#include "parser.h"
#include "SVG.h"

GdiplusRenderer::GdiplusRenderer(int init_width, int init_height)
    : shapes{},
      center{0, 0},
      scale{1},
      dragging{false},
      width{init_width},
      height{init_height} {}

void GdiplusRenderer::load_file(const char *filename) {
  std::ifstream fin(filename);
  std::ostringstream ss;
  ss << fin.rdbuf();
  this->svg_file = ss.str();
  this->shapes.clear();
  this->center = {0, 0};
  this->scale = 1;
  ParseResult svg = parse_xml(this->svg_file);

  for (const BaseShape *shape = svg.shapes.get(); shape; shape = shape->next.get()) {

    this->shapes.emplace_back(shape);
  }

  if (svg.root) {
    if (svg.root->view_width && svg.root->view_height) {
      double scale1 = this->width / svg.root->view_width;
      double scale2 = this->height / svg.root->view_height;

      double scale = std::min(scale1, scale2);
      double pad1 = (this->width - svg.root->view_width * scale) / 2;
      double pad2 = (this->height - svg.root->view_height * scale) / 2;
      this->scale = scale;
      this->center = svg.root->view_min + Point {pad1, pad2};
    } else {
      this->center = svg.root->view_min;
    }
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

void GdiplusRenderer::resize(int new_width, int new_height) {
  this->center = (this->center - Point {this->width * 0.5, this->height * 0.5}) / this->scale;
  this->scale /= std::sqrt(this->width * this->height);
  this->width = new_width;
  this->height = new_height;
  this->scale *= std::sqrt(this->width * this->height);
  this->center = this->center * this->scale + Point {this->width * 0.5, this->height * 0.5};
}
