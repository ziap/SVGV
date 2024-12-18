#include "GdiplusRenderer.h"

#include <cmath>
#include <fstream>
#include <sstream>

#include "parser.h"
#include "SVG.h"

GdiplusRenderer::GdiplusRenderer(int init_width, int init_height) :
  shapes{},
  center{0, 0},
  scale{1},
  dragging{false},
  width{init_width},
  height{init_height},
  view_width{0}, 
  view_height{0} {}

bool GdiplusRenderer::load_file(const char *filename) {
  std::ifstream fin(filename);

  if (!fin.is_open()) {
    return false;
  }

  std::ostringstream ss;
  ss << fin.rdbuf();
  std::string svg_file = ss.str();
  this->clear();
  ParseResult svg = parse_xml(svg_file);

  for (const BaseShape *shape = svg.shapes.get(); shape; shape = shape->next.get()) {
    this->shapes.emplace_back(shape, &svg);
  }

  if (svg.root) {
    if (svg.root->view_width && svg.root->view_height) {
      double scale = std::min(
        this->width / svg.root->view_width,
        this->height / svg.root->view_height
      );
      double pad1 = (this->width - svg.root->view_width * scale) / 2;
      double pad2 = (this->height - svg.root->view_height * scale) / 2;
      this->scale = scale;
      this->center = svg.root->view_min + Point {pad1, pad2};
      this->view_width = svg.root->view_width;
      this->view_height = svg.root->view_height;
    } else {
      this->center = svg.root->view_min;
      this->view_width = 0;
      this->view_height = 0;
    }
  }

  return true;
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
  if (this->view_width && this->view_height) {
    double old_rel_scale = std::min(
      this->width / this->view_width,
      this->height / this->view_height
    );

    double new_rel_scale = std::min(
      new_width / this->view_width,
      new_height / this->view_height
    );

    this->center = (this->center - Point {
      this->width * 0.5,
      this->height * 0.5
    }) / this->scale;

    this->scale = this->scale * new_rel_scale / old_rel_scale;

    this->center = this->center * this->scale + Point {
      new_width * 0.5,
      new_height * 0.5
    };
  } else {
    this->center = this->center + Point {
      (new_width - this->width) * 0.5,
      (new_height - this->height) * 0.5
    };
  }

  this->width = new_width;
  this->height = new_height;
}

void GdiplusRenderer::clear() {
  this->shapes.clear();
  this->center = {0, 0};
  this->scale = 1;
}
