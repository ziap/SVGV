#include "GdiplusFragment.h"

static std::unique_ptr<const Gdiplus::Brush> paint_to_brush(Paint paint, double opacity) {
  switch (paint.type) {
    case PAINT_TRANSPARENT:
      return nullptr;
    case PAINT_RGB:
      return std::make_unique<const Gdiplus::SolidBrush>(Gdiplus::Color{
        (BYTE)(opacity * 255), 
        (BYTE)(paint.variants.rgb_paint.r * 255), 
        (BYTE)(paint.variants.rgb_paint.g * 255), 
        (BYTE)(paint.variants.rgb_paint.b * 255)
      });
  }
}

static Gdiplus::FillMode get_gdiplus_fillmode(FillRule fillrule) {
  switch (fillrule) {
    case FILL_RULE_NONZERO:
      return Gdiplus::FillModeWinding;
    case FILL_RULE_EVENODD:
      return Gdiplus::FillModeAlternate;
    case FILL_RULE_COUNT:
      __builtin_unreachable();
  }
}


GdiplusFragment::GdiplusFragment(const BaseShape *shape)
  : fill_brush{paint_to_brush(shape->fill, shape->fill_opacity)},
  stroke_brush{paint_to_brush(shape->stroke, shape->stroke_opacity)},
  pen{
    this->stroke_brush.get(),
    (Gdiplus::REAL)shape->stroke_width,
  },
  path {get_gdiplus_fillmode(shape->fill_rule)} {
  ArrayList<BezierCurve> beziers = shape->get_beziers();

  if (beziers.len()) {
    this->path.AddBezier(
      (Gdiplus::REAL)beziers[0].start[0],
      (Gdiplus::REAL)beziers[0].start[1],
      (Gdiplus::REAL)beziers[0].control_start[0],
      (Gdiplus::REAL)beziers[0].control_start[1],
      (Gdiplus::REAL)beziers[0].control_end[0],
      (Gdiplus::REAL)beziers[0].control_end[1],
      (Gdiplus::REAL)beziers[0].end[0],
      (Gdiplus::REAL)beziers[0].end[1]
    );

    Point last_point = beziers[0].end;

    for (uint32_t i = 1; i < beziers.len(); ++i){
      BezierCurve curve = beziers[i];

      if (last_point[0] != curve.start[0] ||
        last_point[1] != curve.start[1]) {
        this->path.StartFigure();
      }

      this->path.AddBezier(
        (Gdiplus::REAL)curve.start[0],
        (Gdiplus::REAL)curve.start[1],
        (Gdiplus::REAL)curve.control_start[0],
        (Gdiplus::REAL)curve.control_start[1],
        (Gdiplus::REAL)curve.control_end[0],
        (Gdiplus::REAL)curve.control_end[1],
        (Gdiplus::REAL)curve.end[0],
        (Gdiplus::REAL)curve.end[1]
      );

      last_point = curve.end;
    }
  }

  Gdiplus::Matrix matrix {
    (Gdiplus::REAL)shape->transform.m[0][0],
    (Gdiplus::REAL)shape->transform.m[1][0],
    (Gdiplus::REAL)shape->transform.m[0][1],
    (Gdiplus::REAL)shape->transform.m[1][1],
    (Gdiplus::REAL)shape->transform.d[0],
    (Gdiplus::REAL)shape->transform.d[1]
  };

  this->path.Transform(&matrix);
}

void GdiplusFragment::render(Gdiplus::Graphics *graphics) {
  if (this->fill_brush) {
    graphics->FillPath(this->fill_brush.get(), &this->path);
  }

  graphics->DrawPath(&this->pen, &this->path);
}
