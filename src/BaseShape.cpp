#include "BaseShape.h"
#include "InverseIndex.h"
#include <charconv>

enum AttributeType {
    ATTRIBUTE_VISIBLE = 0,
    ATTRIBUTE_FILL,
    ATTRIBUTE_STROKE,
    ATTRIBUTE_OPACITY,
    ATTRIBUTE_FILL_OPACITY,
    ATTRIBUTE_STROKE_OPACITY,
    ATTRIBUTE_STROKE_WIDTH,
    ATTRIBUTE_STROKE_DASH_OFFSET,
    ATTRIBUTE_STROKE_DASH_ARRAY,
    ATTRIBUTE_STROKE_LINE_JOIN,
    ATTRIBUTE_STROKE_LINE_CAP,
    ATTRIBUTE_MITER_LIMIT,
    ATTRIBUTE_FILL_RULE,
    ATTRIBUTE_TRANSFORM,
    ATTRIBUTE_COUNT,
};

constexpr std::string_view attribute_name[ATTRIBUTE_COUNT] {
    "visibility",
    "fill",
    "stroke",
    "opacity",
    "fill-opacity",
    "stroke-opacity",
    "stroke-width",
    "stroke-dashoffset",
    "stoke-dasharray",
    "stroke-linejoin",
    "stroke-linecap",
    "stroke-miterlimit",
    "fill-rule",
};

constexpr InverseIndex<ATTRIBUTE_COUNT> inv_attribute = {&attribute_name};

static Paint read_paint(std::string_view value) {
    Paint paint;
    int start = value.find('(');
    int end = value.find(',');
    std::from_chars(value.data() + start + 1, value.data() + end, paint.r);    

    while (isspace(value[end + 1])) end += 1;
    value = value.substr(end + 1);
    start = 0;
    end = value.find(',');
    std::from_chars(value.data() + start, value.data() + end, paint.g);

    while (isspace(value[end + 1])) end += 1;
    value = value.substr(end + 1);
    std::from_chars(value.data() + start, value.data() + end, paint.b);

    paint.r /= 255;
    paint.g /= 255;
    paint.b /= 255;

    return paint;
}

static double convert_opacity(std::string_view value) {
  double opacity;
  if (value[value.size() - 1] == '%') {
    std::from_chars(value.data(), value.data() + value.size() - 1, opacity);
    opacity /= 100;
  }
  else std::from_chars(value.data(), value.data() + value.size(), opacity);

  return opacity;
}

static void convert_array(std::string_view value, float *a, int *count) {

  while (value.size() > 0 && (*count) < 8) {
    while (value.size() > 0 && isspace(value[0])) value = value.substr(1);
    std::from_chars_result res = std::from_chars(value.data(), value.data() + value.size(), a[(*count)]);
    if (res.ptr != value.data()) (*count) = (*count) + 1;
    value = value.substr(res.ptr - value.data());
  }
}

static void mutiply_matrix(double matrix1[2][3], double matrix2[2][3]) {
  double matrix[2][3] = {0};
  matrix[0][0] = matrix1[0][0] * matrix2[0][0] + matrix1[0][1] * matrix2[1][0];
  matrix[0][1] = matrix1[0][0] * matrix2[0][1] + matrix1[0][1] * matrix2[1][1];
  matrix[0][2] = matrix1[0][0] * matrix2[0][2] + matrix1[0][1] * matrix2[1][2] + matrix1[0][2];

  matrix[1][0] = matrix1[1][0] * matrix2[0][0] + matrix1[1][1] * matrix2[1][0];
  matrix[1][1] = matrix1[1][0] * matrix2[0][1] + matrix1[1][1] * matrix2[1][1];
  matrix[1][2] = matrix1[1][0] * matrix2[0][2] + matrix1[1][1] * matrix2[1][2] + matrix1[1][2];

  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++)
      matrix1[i][j] = matrix[i][j];
}


void BaseShape::read_xml_node(XMLNode *node) {
    this->visible = true;
    this->fill = Paint{0, 0, 0};
    this->stroke = Paint{0, 0, 0};
    this->opacity = 1.0;
    this->fill_opacity = 1.0;
    this->stroke_opacity = 1.0;
    this->stroke_width = 1.0;
    this->stroke_dash_offset = 0.0;
    this->stroke_dash_count = 0;
    this->stroke_line_join = StrokeLineJoin::LINE_JOIN_MITER;
    this->stroke_line_cap = StrokeLineCap::LINE_CAP_BUTT;
    this->miter_limit = 4;
    this->transform[0][0] = 1;
    this->transform[0][1] = 0;
    this->transform[0][2] = 0;
    this->transform[1][0] = 0;
    this->transform[1][1] = 1;
    this->transform[1][2] = 0;
    this->fill_rule = FillRule::FILL_RULE_NONZERO;

    for(int i = 0; i < node->attrs_count; i++) {
        std::string_view key = node->attrs[i].key;
        std::string_view value = node->attrs[i].value;
        
        AttributeType type = (AttributeType)inv_attribute[key];
        switch (type) {
            case ATTRIBUTE_VISIBLE: {
                if (value != "visible") this->visible = false;
            } break;

            case ATTRIBUTE_FILL: {
                this->fill = read_paint(value);
            } break;

            case ATTRIBUTE_STROKE: {
                this->stroke = read_paint(value);
            } break;

            case ATTRIBUTE_OPACITY: {
              this->opacity = convert_opacity(value);
            } break;

            case ATTRIBUTE_FILL_OPACITY: {
              this->fill_opacity = convert_opacity(value);
            } break;

            case ATTRIBUTE_STROKE_OPACITY: {
              this->stroke_opacity = convert_opacity(value);
            } break;

            case ATTRIBUTE_STROKE_WIDTH: {
              this->stroke_width = convert_opacity(value);
            } break;

            case ATTRIBUTE_STROKE_DASH_OFFSET: {
              std::from_chars(value.data(), value.data() + value.size(), this->stroke_dash_offset);
            } break;

            case ATTRIBUTE_STROKE_DASH_ARRAY: {
              convert_array(value, this->stroke_dash_array, &this->stroke_dash_count);

            } break;

            case ATTRIBUTE_STROKE_LINE_JOIN: {
              this->stroke_line_join = (StrokeLineJoin)inv_linejoin[value];

            } break;

            case ATTRIBUTE_STROKE_LINE_CAP: {
              this->stroke_line_cap = (StrokeLineCap)inv_linecap[value];
            } break;

            case ATTRIBUTE_MITER_LIMIT: {
              std::from_chars(value.data(), value.data() + value.size(), this->miter_limit);
            } break;

            case ATTRIBUTE_FILL_RULE: {
              this->fill_rule = (FillRule)inv_fillrule[value];

            } break;
            
            case ATTRIBUTE_TRANSFORM: {
              
            } break;

            default: {

            }
        }

    }
}

#include <iostream>

int main() {
  std::cout << "Hello, world!\n";

    XMLNode *node = new XMLNode {
    "rect",
    new Attribute[7] {
      Attribute {"fill-opacity", "1%"},
      Attribute {"stroke", "rgb(200, 200, 200)"},
      Attribute {"stroke-width", "2"},
      Attribute {"x", "25"},
      Attribute {"y", "25"},
      Attribute {"width", "800"},
      Attribute {"height", "400"}
    },
    7,
    new XMLNode[] {},
    0
  };

  BaseShape shape;
  shape.read_xml_node(node);

  Paint stroke = *shape.stroke;
  std::cout << stroke.r << " " << stroke.g << " " << stroke.b << " \n"; 
  std::cout << shape.fill_opacity << " " << shape.stroke_width << " \n"; 

  return 0;
}
