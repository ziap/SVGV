#include "GdiplusFragment.h"
#include "Text.h"

#include <string>

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

static double det(Transform transform) {
  return transform.m[0][0] * transform.m[1][1] - transform.m[0][1] * transform.m[1][0];
}

GdiplusFragment::GdiplusFragment(const BaseShape *shape) :
  fill_brush{paint_to_brush(shape->fill, shape->fill_opacity)},
  stroke_brush{paint_to_brush(shape->stroke, shape->stroke_opacity)},
  pen{
    this->stroke_brush.get(),
    (Gdiplus::REAL)(shape->stroke_width * (sqrt(det(shape->transform)))),
  },
  path {get_gdiplus_fillmode(shape->fill_rule)} {
  if (const SVGShapes::Text *text = dynamic_cast<const SVGShapes::Text*>(shape)) {
    std::wstring str = std::wstring{text->content.begin(), text->content.end()};
    Gdiplus::FontFamily family{L"Times New Roman"};
    int font_style = Gdiplus::FontStyleRegular;

    Gdiplus::PointF origin{
      (Gdiplus::REAL)(text->pos[0] + text->d[0]), 
      (Gdiplus::REAL)(text->pos[1] + text->d[1] - text->font_size)
    };

    Gdiplus::StringFormat format = Gdiplus::StringFormat::GenericDefault();

    this->path.AddString(
      str.c_str(),
      (INT)(str.length()),
      &family,
      font_style,
      (Gdiplus::REAL) text->font_size,
      origin,
      &format
    );
  } else {
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

      Point first_point = beziers[0].start;
      Point last_point = beziers[0].end;

      for (uint32_t i = 1; i < beziers.len() - 1; ++i){
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
      BezierCurve curve = beziers[beziers.len() - 1];
      Point mid = (last_point + first_point) / 2; 

      if (curve.start[0] == last_point[0] && curve.start[1] == last_point[1]
        && curve.end[0] == first_point[0] && curve.end[1] == first_point[1] 
        && curve.control_end[0] == mid[0] && curve.control_end[1] == mid[1]) {
        this->path.CloseFigure();
      } else {
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
      }
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

  switch ((StrokeLineJoin)shape->stroke_line_join) {
    case LINE_JOIN_ARCS: {
      this->pen.SetLineJoin(Gdiplus::LineJoinMiter);
    } break;
    case LINE_JOIN_BEVEL: {
      this->pen.SetLineJoin(Gdiplus::LineJoinBevel);
    } break;
    case LINE_JOIN_MITER: {
      this->pen.SetLineJoin(Gdiplus::LineJoinMiter);
    } break;
    case LINE_JOIN_MITER_CLIP: {
      this->pen.SetLineJoin(Gdiplus::LineJoinMiterClipped);
    } break;
    case LINE_JOIN_ROUND: {
      this->pen.SetLineJoin(Gdiplus::LineJoinRound);
    } break;
    case LINE_JOIN_COUNT: {
      __builtin_unreachable();
    } break;
  }

  switch ((StrokeLineCap)(shape->stroke_line_cap)) {
    case LINE_CAP_BUTT: {
      this->pen.SetStartCap(Gdiplus::LineCapFlat);
      this->pen.SetEndCap(Gdiplus::LineCapFlat);
    } break;
    case LINE_CAP_ROUND: {
      this->pen.SetStartCap(Gdiplus::LineCapRound);
      this->pen.SetEndCap(Gdiplus::LineCapRound);
    } break;
    case LINE_CAP_SQUARE: {
      this->pen.SetStartCap(Gdiplus::LineCapSquare);
      this->pen.SetEndCap(Gdiplus::LineCapSquare);
    } break;
    case LINE_CAP_COUNT: {
      __builtin_unreachable();
    } break;
  }

  this->pen.SetDashOffset((Gdiplus::REAL)shape->stroke_dash_offset);
  Gdiplus::REAL dasharray[8];
  for (int i = 0; i < shape->stroke_dash_count; i++) {
    dasharray[i] = shape->stroke_dash_array[i];
  }
  this->pen.SetDashPattern(dasharray, shape->stroke_dash_count);
  this->pen.SetMiterLimit((Gdiplus::REAL)shape->miter_limit);
}

void GdiplusFragment::render(Gdiplus::Graphics *graphics) {
  if (this->fill_brush) {
    graphics->FillPath(this->fill_brush.get(), &this->path);
  }

  graphics->DrawPath(&this->pen, &this->path);
}
