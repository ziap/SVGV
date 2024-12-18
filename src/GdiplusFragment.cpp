#include "GdiplusFragment.h"
#include "Text.h"
#include "Gradient.h"

#include <string>
#include <string_view>
#include <iostream>
#include <cmath>

enum GenericFont {
  GENERIC_FONT_SERIF = 0,
  GENERIC_FONT_SANSSERIF,
  GENERIC_FONT_MONOSPACE,
  GENERIC_FONT_COUNT,
};

constexpr std::string_view genericfont_name[GENERIC_FONT_COUNT] = {
  "serif",
  "sans-serif",
  "monospace",
};

constexpr InverseIndex<GENERIC_FONT_COUNT> inv_genericfont{&genericfont_name};

static double apply_percent(double val, bool percent, double max_val, GradientUnits gradient_units) {
  switch(gradient_units) {
    case GRADIENT_UNIT_USER_SPACE_ON_USE: {
      if (percent == true) {
        return max_val / 100 * val;
      }
      return val;
    } break;
    case GRADIENT_UNIT_OBJECT_BOUNDING_BOX: {
      if (percent == true) {
        return val / 100;
      }
      return val;
    }
    case GRADIENT_UNIT_COUNT: {
      __builtin_unreachable();
    }
  }
}

static std::unique_ptr<const Gdiplus::Brush> paint_to_brush(Paint paint, double opacity, ParseResult *svg, const BaseShape *shape) {
  GradientMap *gradient_map = &svg->gradient_map;
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
    case PAINT_URL: {
      std::string_view url {
        paint.variants.url_paint.data,
        (size_t)paint.variants.url_paint.len
      };

      while (url.size() > 1 && url[0] != '#') {
        url = url.substr(1, url.size() - 2);
      }
      
      if (url.size() <= 1 || url[0] != '#') return nullptr;
      url = url.substr(1);

      GradientMap::iterator it = gradient_map->find(url);
      if (it == gradient_map->end()) return nullptr;

      Gradient *gradient = &it->second;

      switch (gradient->type) {
        case GRADIENT_TYPE_LINEAR: {
          AABB size = shape->get_bounding();

          double width = (size.max[0] - size.min[0]);
          double height = (size.max[1] - size.min[1]);

          Point p0 = {
            apply_percent(gradient->variants.linear.x1.val, gradient->variants.linear.x1.percent, svg->root->width, gradient->gradient_units),
            apply_percent(gradient->variants.linear.y1.val, gradient->variants.linear.y1.percent, svg->root->height, gradient->gradient_units),
          };

          Point p1 = {
            apply_percent(gradient->variants.linear.x2.val, gradient->variants.linear.x2.percent, svg->root->width, gradient->gradient_units),
            apply_percent(gradient->variants.linear.y2.val, gradient->variants.linear.y2.percent, svg->root->height, gradient->gradient_units),
          };

          p0 = gradient->transform * p0;
          p1 = gradient->transform * p1;

          if (gradient->gradient_units == GRADIENT_UNIT_OBJECT_BOUNDING_BOX) {
            p0[0] = (size.min[0] + p0[0] * width);
            p0[1] = (size.min[1] + p0[1] * height);

            p1[0] = (size.min[0] + p1[0] * width);
            p1[1] = (size.min[1] + p1[1] * height);
          }

          p0 = shape->transform * p0;
          p1 = shape->transform * p1;

          Point d = p1 - p0;

          double gap = std::sqrt(d[0] * d[0] + d[1] * d[1]);

          double z = 1e6;
          Point min = p0 - z * d / gap;
          Point max = p1 + z * d / gap;

          double new_gap = gap + 2 * z;

          Gdiplus::PointF start {
            (Gdiplus::REAL)min[0],
            (Gdiplus::REAL)min[1],
          };

          Gdiplus::PointF end {
            (Gdiplus::REAL)max[0],
            (Gdiplus::REAL)max[1],
          };

          size_t stop_count = gradient->stops.len();

          std::unique_ptr<Gdiplus::Color[]> colors = std::make_unique<Gdiplus::Color[]>(stop_count + 2);
          std::unique_ptr<Gdiplus::REAL[]> blendPositions = std::make_unique<Gdiplus::REAL[]>(stop_count + 2);

          colors[0] = Gdiplus::Color {
            (BYTE)(gradient->stops[0].stop_opacity * opacity * 255),      
            (BYTE)(gradient->stops[0].stop_color.r * 255),            
            (BYTE)(gradient->stops[0].stop_color.g * 255),             
            (BYTE)(gradient->stops[0].stop_color.b * 255),         
          };
          blendPositions[0] = 0.0f;

          for (size_t i = 0; i < stop_count; i++) {
            colors[i + 1] = Gdiplus::Color{
              (BYTE)(gradient->stops[i].stop_opacity * opacity * 255),
              (BYTE)(gradient->stops[i].stop_color.r * 255),
              (BYTE)(gradient->stops[i].stop_color.g * 255),
              (BYTE)(gradient->stops[i].stop_color.b * 255),
            };
            blendPositions[i + 1] = (Gdiplus::REAL)((gradient->stops[i].offset * gap + z) / new_gap);
          }

          colors[stop_count + 1] = Gdiplus::Color{
            (BYTE)(gradient->stops[stop_count - 1].stop_opacity * opacity * 255),
            (BYTE)(gradient->stops[stop_count - 1].stop_color.r * 255),
            (BYTE)(gradient->stops[stop_count - 1].stop_color.g * 255),
            (BYTE)(gradient->stops[stop_count - 1].stop_color.b * 255),
          };
          blendPositions[stop_count + 1] = 1.0f;

          stop_count += 2;

          std::unique_ptr<Gdiplus::LinearGradientBrush> brush = std::make_unique<Gdiplus::LinearGradientBrush>(
            start,
            end,
            colors[0], 
            colors[stop_count - 1]
          );

          brush->SetInterpolationColors(colors.get(), blendPositions.get(), (INT)stop_count);
          return brush;
        } break;
        case GRADIENT_TYPE_RADIAL: {

        } break;
        case GRADIENT_TYPE_COUNT: {
          __builtin_unreachable();
        } break;
      }
      return nullptr;
    }
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

static std::wstring remove_spaces(const std::wstring& input) {
  std::wstring result = L"";
  bool is_space = true;

  for (size_t i = 0; i < input.size(); ++i) {
    if (std::iswspace(input[i])) {
      if (is_space == false) {
        result.push_back(L' ');
        is_space = true;
      }
    } else {
      result.push_back(input[i]);
      is_space = false;
    }
  }

  return result;
}

std::wstring string_to_wide_string(std::string_view string) {
  if (string.empty()) return L"";

  const size_t size_needed = MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), nullptr, 0);

  std::wstring result(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), result.data(), size_needed);
  return result;
}

static void add_bezier_transformed(Gdiplus::GraphicsPath *path, BezierCurve curve, Transform matrix) {
  Point start = matrix * curve.start;
  Point end = matrix * curve.end;

  Point control_start = matrix * curve.control_start;
  Point control_end = matrix * curve.control_end;

  path->AddBezier(
    (Gdiplus::REAL)start[0],
    (Gdiplus::REAL)start[1],
    (Gdiplus::REAL)control_start[0],
    (Gdiplus::REAL)control_start[1],
    (Gdiplus::REAL)control_end[0],
    (Gdiplus::REAL)control_end[1],
    (Gdiplus::REAL)end[0],
    (Gdiplus::REAL)end[1]
  );
}

GdiplusFragment::GdiplusFragment(const BaseShape *shape, ParseResult *svg) :
  fill_brush{paint_to_brush(shape->fill, shape->fill_opacity * shape->opacity, svg, shape)},
  stroke_brush{paint_to_brush(shape->stroke, shape->stroke_opacity * shape->opacity, svg, shape)},
  pen{
    this->stroke_brush.get(),
    (Gdiplus::REAL)(shape->stroke_width * (std::sqrt(det(shape->transform)))),
  },
  path {get_gdiplus_fillmode(shape->fill_rule)} {
  if (const SVGShapes::Text *text = dynamic_cast<const SVGShapes::Text*>(shape)) {
    std::wstring str = string_to_wide_string(text->content);
    str = remove_spaces(str);

    int font_style;
    switch (text->font_style) {
      case FONTSTYLE_NORMAL: {
        if (text->font_weight >= 500) font_style = Gdiplus::FontStyleBold;
        else font_style = Gdiplus::FontStyleRegular;

      } break;

      case FONTSTYLE_ITALIC:
      case FONTSTYLE_OBLIQUE: {
        if (text->font_weight >= 500) font_style = Gdiplus::FontStyleBoldItalic;
        else font_style = Gdiplus::FontStyleItalic;
      } break;

      case FONTSTYLE_COUNT: {
        __builtin_unreachable();
      }     
    }

    Gdiplus::PointF origin{
      (Gdiplus::REAL)(text->pos[0] + text->d[0]), 
      (Gdiplus::REAL)(text->pos[1] + text->d[1] - text->font_size)
    };

    Gdiplus::StringFormat format;
    
    switch (text->text_anchor) {
      case TEXTANCHOR_START: {
        format.SetAlignment(Gdiplus::StringAlignmentNear);
      } break;
      case TEXTANCHOR_MIDDLE: {
        format.SetAlignment(Gdiplus::StringAlignmentCenter);
      } break;
      case TEXTANCHOR_END: {
        format.SetAlignment(Gdiplus::StringAlignmentFar);
      } break;
      case TEXTANCHOR_COUNT: {
        __builtin_unreachable();
      }
    }

    bool set_font_family = false;
    std::string_view tmp_font_family = text->font_family;
    
    while (tmp_font_family.size() > 0) {
      size_t pos = (tmp_font_family).find(',');
      std::string_view font = tmp_font_family.substr(0, pos);

      while(isspace(font[0])) font = font.substr(1);
      while(isspace(font[font.size() - 1])) font = font.substr(0, font.size() - 2);

      if (pos != std::string_view::npos) tmp_font_family = tmp_font_family.substr(pos + 1);
      else tmp_font_family = "";
      
      int type = inv_genericfont[font];
      if (type == -1) {
        std::wstring s{font.begin(), font.end()};
        Gdiplus::FontFamily family{s.c_str()};
        if (family.IsAvailable()) {
          set_font_family = true;
          this->path.AddString(
            str.c_str(), 
            (INT)(str.length()), 
            &family,
            font_style,
            (Gdiplus::REAL) text->font_size,
            origin,
            &format
          );
          break;
        }
      } else {
        set_font_family = true;
        const Gdiplus::FontFamily *family;
        switch ((GenericFont)type) {
          case GENERIC_FONT_SERIF: {
            family = Gdiplus::FontFamily::GenericSerif();
          } break;
          case GENERIC_FONT_SANSSERIF: {
            family = Gdiplus::FontFamily::GenericSansSerif();
          } break;
          case GENERIC_FONT_MONOSPACE: {
            family = Gdiplus::FontFamily::GenericMonospace();
          } break;
          case GENERIC_FONT_COUNT: {
            __builtin_unreachable();
          } 
        }
        this->path.AddString(
          str.c_str(), 
          (INT)(str.length()), 
          family,
          font_style,
          (Gdiplus::REAL) text->font_size,
          origin,
          &format
        );
        break;
      }
    }

    if (set_font_family == false) {
      this->path.AddString(
        str.c_str(), 
        (INT)(str.length()), 
        Gdiplus::FontFamily::GenericSerif(),
        font_style,
        (Gdiplus::REAL) text->font_size,
        origin,
        &format
      );
    }
  } else {
    ArrayList<BezierCurve> beziers = shape->get_beziers();

    if (beziers.len()) {
      add_bezier_transformed(&this->path, beziers[0], shape->transform);

      Point first_point = beziers[0].start;
      Point last_point = beziers[0].end;

      for (uint32_t i = 1; i < beziers.len() - 1; ++i){
        BezierCurve curve = beziers[i];

        if (last_point[0] != curve.start[0] ||
          last_point[1] != curve.start[1]) {
          this->path.StartFigure();
        }

        add_bezier_transformed(&this->path, curve, shape->transform);

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

        add_bezier_transformed(&this->path, curve, shape->transform);
      }
    }
  }

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
