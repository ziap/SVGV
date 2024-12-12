#include "Stop.h"
#include "Attribute.h"
#include "InverseIndex.h"
#include "common.h"
#include "Paint.h"

enum StopAttribute {
  STOP_OFFSET = 0,
  STOP_OPACITY,
  STOP_COLOR,
  STOP_COUNT,
};

constexpr std::string_view stop_attr_name[STOP_COUNT] {
  "offset", 
  "stop-opacity",
  "stop-color",
};

constexpr InverseIndex<STOP_COUNT> inv_stop_attr = {&stop_attr_name};

Stop read_stop(Attribute *attrs, int attribute_count) {
  Stop result; 
  for (int i = 0; i < attribute_count; ++i) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;

    switch((StopAttribute)inv_stop_attr[key]) {
      case STOP_OFFSET: {
        result.offset = convert_opacity(value);
      } break;
      case STOP_OPACITY: {
        result.stop_opacity = convert_opacity(value);
      } break;
      case STOP_COLOR: {
        Paint paint = read_paint(value);
        if (paint.type == PAINT_RGB) {
          result.stop_color = paint.variants.rgb_paint;
        }
      } break;
      case STOP_COUNT: {
        __builtin_unreachable();
      };
    }
  }
  return result;
}
