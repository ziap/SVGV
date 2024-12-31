#include "GdiplusFragment.h"
#include "Text.h"
#include "Gradient.h"

#include <string_view>
#include <cmath>
#include <deque>

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

static double distance(Point p1, Point p2) {
  Point d = p1 - p2;
  return std::hypot(d[0], d[1]);
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
      AABB size = shape->get_bounding();

      double width = (size.max[0] - size.min[0]);
      double height = (size.max[1] - size.min[1]);

      Point vertices[4] = {
        shape->transform * Point {size.min[0], size.min[1]},
        shape->transform * Point {size.min[0], size.max[1]},
        shape->transform * Point {size.max[0], size.min[1]},
        shape->transform * Point {size.max[0], size.max[1]},
      };

      Point tmax = vertices[0];
      Point tmin = vertices[0];

      for (int i = 1; i < 4; ++i) {
        tmin[0] = std::min(tmin[0], vertices[i][0]);
        tmax[0] = std::max(tmax[0], vertices[i][0]);
        tmin[1] = std::min(tmin[1], vertices[i][1]);
        tmax[1] = std::max(tmax[1], vertices[i][1]);
      }
      switch (gradient->type) {
        case GRADIENT_TYPE_LINEAR: {
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

          double pad = tmax[0] - tmin[0] + tmax[1] - tmin[1];
          Point d = p1 - p0;
          double gap = std::hypot(d[0], d[1]);

          Point min = p0 - pad * d / gap;
          Point max = p1 + pad * d / gap;

          double new_gap = gap + 2 * pad;

          Gdiplus::PointF start {
            (Gdiplus::REAL)min[0],
            (Gdiplus::REAL)min[1],
          };

          Gdiplus::PointF end {
            (Gdiplus::REAL)max[0],
            (Gdiplus::REAL)max[1],
          };

          size_t stop_count = gradient->stops.len();

          if (stop_count == 0) return nullptr;

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
            blendPositions[i + 1] = (Gdiplus::REAL)((gradient->stops[i].offset * gap + pad) / new_gap);
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
          int stop_count = gradient->stops.len();
          RadialGradient radial_grad = gradient->variants.radial;
          PercentUnit p_fx = radial_grad.fx || radial_grad.cx;
          PercentUnit p_fy = radial_grad.fy || radial_grad.cy;

          double fx = apply_percent(p_fx.val, p_fx.percent, width, gradient->gradient_units);
          double fy = apply_percent(p_fy.val, p_fy.percent, height, gradient->gradient_units);
          double cx = apply_percent(radial_grad.cx.val, radial_grad.cx.percent, width, gradient->gradient_units);
          double cy = apply_percent(radial_grad.cy.val, radial_grad.cy.percent, height, gradient->gradient_units);
          double r = apply_percent(radial_grad.r.val, radial_grad.r.percent, width, gradient->gradient_units);
          Point f = {fx, fy};

          Point center = {cx, cy};
          Point high = {cx, cy - r};
          Point right = {cx + r, cy};

          center = gradient->transform * center;
          high = gradient->transform * high;
          right = gradient->transform * right;
          f = gradient->transform * f;

          if (gradient->gradient_units == GRADIENT_UNIT_OBJECT_BOUNDING_BOX) {
            center[0] = (size.min[0] + center[0] * width);
            center[1] = (size.min[1] + center[1] * height);
            high[0] = (size.min[0] + high[0] * width);
            high[1] = (size.min[1] + high[1] * height);
            right[0] = (size.min[0] + right[0] * width);
            right[1] = (size.min[1] + right[1] * height);
            f[0] = (size.min[0] + f[0] * width);
            f[1] = (size.min[1] + f[1] * height);
          }

          center = shape->transform * center;
          high = shape->transform * high;
          right = shape->transform * right;
          f = shape->transform * f;

          double height_out = 0, width_out = 0;
          if (center[0] < tmin[0]) {
            width_out = tmin[0] - center[0];
          } else if (center[0] > tmax[0]) {
            width_out = center[0] - tmax[0];
          }

          if (center[1] < tmin[1]) {
            height_out = tmin[1] - center[1];
          } else if (center[1] > tmax[1]) {
            height_out = center[1] - tmax[1];
          }

          double max_r = height_out + width_out + tmax[1] - tmin[1] + tmax[0] - tmin[0];
          double min_r = std::min(distance(high, center), distance(right, center));
          double new_r = r * max_r / min_r;

          Point brush_point[12];
          brush_point[0] = {cx - new_r, cy};
          brush_point[3] = {cx, cy + new_r};
          brush_point[6] = {cx + new_r, cy};
          brush_point[9] = {cx, cy - new_r};
          brush_point[1] = brush_point[0] + Point{new_r * KX, new_r * KY};
          brush_point[2] = brush_point[3] - Point{new_r * KY, new_r * KX};
          brush_point[4] = brush_point[3] + Point{new_r * KY, -new_r * KX};
          brush_point[5] = brush_point[6] - Point{new_r * KX, -new_r * KY};
          brush_point[7] = brush_point[6] + Point{-new_r * KX, -new_r * KY};
          brush_point[8] = brush_point[9] - Point{-new_r * KY, -new_r * KX};
          brush_point[10] = brush_point[9] + Point{-new_r * KY, new_r * KX};
          brush_point[11] = brush_point[0] - Point{-new_r * KX, new_r * KY};

          for (size_t i = 0; i < 12; ++i) {
            brush_point[i] = gradient->transform * brush_point[i];
          }

           if (gradient->gradient_units == GRADIENT_UNIT_OBJECT_BOUNDING_BOX) {
             for (size_t i = 0; i < 12; ++i) {
               brush_point[i][0] = (size.min[0] + brush_point[i][0] * width);
               brush_point[i][1] = (size.min[1] + brush_point[i][1] * height);
             }
           }

          BezierCurve brush_curve[4] = {
            BezierCurve{brush_point[0], brush_point[3], brush_point[1], brush_point[2]},
            BezierCurve{brush_point[3], brush_point[6], brush_point[4], brush_point[5]},
            BezierCurve{brush_point[6], brush_point[9], brush_point[7], brush_point[8]},
            BezierCurve{brush_point[9], brush_point[0], brush_point[10], brush_point[11]}
          };

          Gdiplus::GraphicsPath path;
          for (size_t i = 0; i < 4; ++i) {
            add_bezier_transformed(&path, brush_curve[i], shape->transform);
          }
          std::unique_ptr<Gdiplus::PathGradientBrush> brush = std::make_unique<Gdiplus::PathGradientBrush>(&path);
          brush->SetCenterPoint(Gdiplus::PointF{(Gdiplus::REAL)f[0], (Gdiplus::REAL)f[1]});

          std::deque<Gdiplus::Color> colors;
          std::deque<Gdiplus::REAL> iter_positions;

          for (int i = 0; i < stop_count; ++i) {
            colors.emplace_back(Gdiplus::Color {
              (BYTE)(gradient->stops[stop_count - i - 1].stop_opacity * opacity * 255),
              (BYTE)(gradient->stops[stop_count - i - 1].stop_color.r * 255),
              (BYTE)(gradient->stops[stop_count - i - 1].stop_color.g * 255),
              (BYTE)(gradient->stops[stop_count - i - 1].stop_color.b * 255),
            });

            iter_positions.emplace_back((Gdiplus::REAL)((1 - ((gradient->stops[stop_count - i - 1].offset) * min_r / max_r))));
          }

          if (colors.size() == 0) return nullptr;

          SpreadMethod method = (colors.size() > 1) ? gradient->spread_method : SPREAD_METHOD_PAD;

          switch (method) {
            case SPREAD_METHOD_PAD: {
              iter_positions.emplace_front(0);
              colors.emplace_front(Gdiplus::Color {
                (BYTE)(gradient->stops[stop_count - 1].stop_opacity * opacity * 255),
                (BYTE)(gradient->stops[stop_count - 1].stop_color.r * 255),
                (BYTE)(gradient->stops[stop_count - 1].stop_color.g * 255),
                (BYTE)(gradient->stops[stop_count - 1].stop_color.b * 255),
              });

              colors.emplace_back(Gdiplus::Color {
                (BYTE)(gradient->stops[0].stop_opacity * opacity * 255),
                (BYTE)(gradient->stops[0].stop_color.r * 255),
                (BYTE)(gradient->stops[0].stop_color.g * 255),
                (BYTE)(gradient->stops[0].stop_color.b * 255),
              });

              iter_positions.emplace_back(1);
            } break;

            case SPREAD_METHOD_REFLECT: {
              while (iter_positions.back() < 1) {
                Gdiplus::REAL *last_pos = &iter_positions.back();
                colors.emplace_back(colors[stop_count - 1]);
                iter_positions.emplace_back(*last_pos);
                for (int i = 1; i < stop_count; ++i) {
                  colors.emplace_back(colors[stop_count - i - 1]);
                  iter_positions.emplace_back(*last_pos + iter_positions[stop_count - 1] - iter_positions[stop_count - i - 1]);

                  if (iter_positions.back() > 1) {
                    iter_positions.back() = 1;
                    break;
                  }
                }
              }

              while (iter_positions.front() > 0) {
                int first = 0;
                Gdiplus::REAL *last_pos = &iter_positions.front();
                colors.emplace_front(colors[first]);
                iter_positions.emplace_front(*last_pos);
                ++first;

                for (int i = 1; i < stop_count; ++i) {
                  colors.emplace_front(colors[first + i]);
                  iter_positions.emplace_front(*last_pos + iter_positions[0] - iter_positions[first + i]);
                  ++first;

                  if (iter_positions.front() < 0) {
                    iter_positions.front() = 0;
                    break;
                  }
                }
              }
            } break;

            case SPREAD_METHOD_REPEAT: {
              while (iter_positions.back() < 1) {
                Gdiplus::REAL *last_pos = &iter_positions.back();
                colors.emplace_back(colors[0]);
                iter_positions.emplace_back(*last_pos);
                for (int i = 1; i < stop_count; ++i) {
                  colors.emplace_back(colors[i]);
                  iter_positions.emplace_back(*last_pos + iter_positions[i] - iter_positions[0]);

                  if (iter_positions.back() > 1) {
                    iter_positions.back() = 1;
                    break;
                  }
                }
              }

              while (iter_positions.front() > 0) {
                int first = 0;
                Gdiplus::REAL *last_pos = &iter_positions.front();
                colors.emplace_front(colors[first + stop_count - 1]);
                iter_positions.emplace_front(*last_pos);
                ++first;

                for (int i = 1; i < stop_count; ++i) {
                  colors.emplace_front(colors[first + stop_count - i - 1]);
                  iter_positions.emplace_front(*last_pos + iter_positions[first + stop_count - i - 1] - iter_positions[first + stop_count - 1]);
                  ++first;

                  if (iter_positions.front() < 0) {
                    iter_positions.front() = 0;
                    break;
                  }
                }
              }
            } break;

            case SPREAD_METHOD_COUNT: {
              __builtin_unreachable();
            };
          }

          int size = colors.size();

          std::unique_ptr<Gdiplus::Color[]> colors_ptr = std::make_unique<Gdiplus::Color[]>(size);
          std::unique_ptr<Gdiplus::REAL[]> positions_ptr = std::make_unique<Gdiplus::REAL[]>(size);

          std::copy(colors.begin(), colors.end(), colors_ptr.get());
          std::copy(iter_positions.begin(), iter_positions.end(), positions_ptr.get());

          brush->SetInterpolationColors(colors_ptr.get(), positions_ptr.get(), (INT)size);
          return brush;
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

std::wstring string_to_wide_string(std::string_view string) {
  if (string.empty()) return L"";

  const size_t size_needed = MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), nullptr, 0);

  std::wstring result(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), result.data(), size_needed);
  return result;
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
      while(font.size() && isspace(font[font.size() - 1])) font = font.substr(0, font.size() - 2);

      if (pos < tmp_font_family.size()) tmp_font_family = tmp_font_family.substr(pos + 1);
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

    Gdiplus::Matrix matrix {
      (Gdiplus::REAL)shape->transform.m[0][0],
      (Gdiplus::REAL)shape->transform.m[1][0],
      (Gdiplus::REAL)shape->transform.m[0][1],
      (Gdiplus::REAL)shape->transform.m[1][1],
      (Gdiplus::REAL)shape->transform.d[0],
      (Gdiplus::REAL)shape->transform.d[1]
    };
    this->path.Transform(&matrix);
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
  if (this->pen.GetWidth() > 0)
  graphics->DrawPath(&this->pen, &this->path);
}
