#include "Stop.h"
#include "InverseIndex.h"
#include "common.h"
#include "Paint.h"

#include<iostream>


enum StyleGradientType {
  STYLE_GRADIENT_OPACITY = 0,
  STYLE_GRADIENT_COLOR,
  STYLE_GRADIENT_COUNT,
};

constexpr std::string_view stylegradient_name[STYLE_GRADIENT_COUNT] = {
 "stop-opacity",
 "stop-color",
};

constexpr InverseIndex<STYLE_GRADIENT_COUNT> inv_stylegradient = {&stylegradient_name};

enum StopAttribute {
  STOP_OFFSET = 0,
  STOP_STYLE,
  STOP_COUNT,
};

constexpr std::string_view stop_attr_name[STOP_COUNT] = {
  "offset", 
  "style",
};

constexpr InverseIndex<STOP_COUNT> inv_stop_attr {&stop_attr_name};


static void apply_style(Stop *result, Attribute *attrs, int attrs_count) {
  for (int i = 0; i < attrs_count; i++) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
    
    switch ((StyleGradientType)inv_stylegradient[key]) {
      case STYLE_GRADIENT_COLOR: {
        Paint paint = read_paint(value);
        if (paint.type == PAINT_RGB) {
          result->stop_color = paint.variants.rgb_paint;
        }
      } break;
      case STYLE_GRADIENT_OPACITY: {
        result->stop_opacity = convert_percent(value);
      } break;
      case STYLE_GRADIENT_COUNT: {
        __builtin_unreachable();
      }
    }
  }
}

Stop read_stop(Attribute *attrs, int attribute_count) {
  Stop result; 
  result.stop_opacity = 1.0;
  result.stop_color = RGBPaint{0, 0, 0};

  for (int i = 0; i < attribute_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    switch((StopAttribute)inv_stop_attr[key]) {
      case STOP_OFFSET: {
        result.offset = convert_percent(value);
      } break;
      case STOP_STYLE: {
        ArrayList<Attribute> attrs_style = process_style(value);
        apply_style(&result, attrs_style.begin(), attrs_style.len());
      } break;
      case STOP_COUNT: {
        __builtin_unreachable();
      }
    }
  }

  apply_style(&result, attrs, attribute_count);
  
  return result;
}
