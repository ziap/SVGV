#include "BaseShape.h"
#include "InverseIndex.h"
#include <charconv>
#include <cmath>
#include <cctype>

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

constexpr std::string_view attribute_name[ATTRIBUTE_COUNT] = {
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
  end = value.find(',');
  std::from_chars(value.data(), value.data() + end, paint.g);

  while (isspace(value[end + 1])) end += 1;
  value = value.substr(end + 1);
  end = value.find(')');
  std::from_chars(value.data(), value.data() + end, paint.b);

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
  } else std::from_chars(value.data(), value.data() + value.size(), opacity);

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


static void multiply_matrix(double matrix1[2][3], double matrix2[2][3]) {
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

enum TransformType {
  TRANSFORM_MATRIX = 0,
  TRANSFORM_TRANSLATE,
  TRANSFORM_SCALE,
  TRANSFORM_ROTATE,
  TRANSFORM_SKEWX,
  TRANSFORM_SKEWY,
  TRANSFORM_COUNT,
};

constexpr std::string_view transform_name[TRANSFORM_COUNT] = {
  "matrix",
  "translate",
  "scale",
  "rotate",
  "skewX",
  "skewY",
};

constexpr InverseIndex<TRANSFORM_COUNT> inv_transform = {&transform_name};


static void create_matrix_rotate(double num, double matrix[2][3]) {
   num = num * M_PI / 180;
   matrix[0][0] = std::cos(num);
   matrix[1][0] = std::sin(num);
   matrix[0][1] = -std::sin(num);
   matrix[1][1] = std::cos(num);
}

static void solve_transform(std::string_view inf, double matrix[2][3]) {
  matrix[0][0] = 1;
  matrix[0][1] = 0;
  matrix[0][2] = 0;
  matrix[1][0] = 0;
  matrix[1][1] = 1;
  matrix[1][2] = 0;

  int start = 0;
  int end = inf.find('(');
  std::string_view str_type = inf.substr(start, end - start);

  TransformType type = (TransformType)inv_transform[str_type];
  inf = inf.substr(end + 1);

  switch (type){
    case TRANSFORM_MATRIX: {
      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
          while (isspace(inf[0])) inf = inf.substr(1);

          std::from_chars_result res = std::from_chars(inf.data(), inf.data() + inf.size(), matrix[i][j]);
          inf = inf.substr(res.ptr - inf.data());
        }
      }
    } break;

    case TRANSFORM_TRANSLATE:{
      while (isspace(inf[0])) inf = inf.substr(1);
      std::from_chars_result res = std::from_chars(inf.data(), inf.data() + inf.size(), matrix[0][2]);
      inf = inf.substr(res.ptr - inf.data());

      if (inf.size() > 0) {
        while (isspace(inf[0])) inf = inf.substr(1);
        std::from_chars(inf.data(), inf.data() + inf.size(), matrix[1][2]);
      } else matrix[1][2] = 0;

    } break;

    case TRANSFORM_SCALE: {
      while (isspace(inf[0])) inf = inf.substr(1);
      std::from_chars_result res = std::from_chars(inf.data(), inf.data() + inf.size(), matrix[0][0]);
      inf = inf.substr(res.ptr - inf.data());

      while (isspace(inf[0])) inf = inf.substr(1);
      std::from_chars(inf.data(), inf.data() + inf.size(), matrix[1][1]);

    } break;

    case TRANSFORM_ROTATE: {
      double num;
      while (isspace(inf[0])) inf = inf.substr(1);
      std::from_chars_result res = std::from_chars(inf.data(), inf.data() + inf.size(), num);
      inf = inf.substr(res.ptr - inf.data());

      if (inf.size() > 0) {
        double x;
        while (isspace(inf[0])) inf = inf.substr(1);
        std::from_chars_result res = std::from_chars(inf.data(), inf.data() + inf.size(), x);
        inf = inf.substr(res.ptr - inf.data());
  
        double y;
        if (inf.size() > 0) {
          while (isspace(inf[0])) inf = inf.substr(1);
          res = std::from_chars(inf.data(), inf.data() + inf.size(), y);
          inf = inf.substr(res.ptr - inf.data());
        } else y = 0;

        //tranlate x, y
        matrix[0][2] = x;
        matrix[1][2] = y;

        //rotate
        double I_matrix[2][3];
        I_matrix[0][2] = 0;
        I_matrix[1][2] = 0;

        create_matrix_rotate(num, I_matrix);
        multiply_matrix(matrix, I_matrix);

        //tranlate -x, -y
        I_matrix[0][0] = 1;
        I_matrix[0][1] = 0;
        I_matrix[0][2] = -x;
        I_matrix[1][0] = 0;
        I_matrix[1][1] = 1;
        I_matrix[1][2] = -y;  

        multiply_matrix(matrix, I_matrix);
      } else {
        create_matrix_rotate(num, matrix);
      }
      
    } break;

    case TRANSFORM_SKEWX: {
      double num;
      while (isspace(inf[0])) inf = inf.substr(1);
      std::from_chars(inf.data(), inf.data() + inf.size(), num);

      num = num * M_PI / 180;
      matrix[0][1] = std::tan(num);
    } break;

    case TRANSFORM_SKEWY: {
      double num;
      while (isspace(inf[0])) inf = inf.substr(1);
      std::from_chars(inf.data(), inf.data() + inf.size(), num);

      num = num * M_PI / 180;
      matrix[1][0] = std::tan(num);
    } break;

    case TRANSFORM_COUNT: {
      __builtin_unreachable();
    }
  }

}

static void convert_transform(std::string_view value, double matrix[2][3]) {
  matrix[0][0] = 1;
  matrix[0][1] = 0;
  matrix[0][2] = 0;
  matrix[1][0] = 0;
  matrix[1][1] = 1;
  matrix[1][2] = 0;

  double Ematrix[2][3];
  int start = 0, end = 0;
  while (value.size() > 0) {
    while(isspace(value[0]) || value[0] == '\n') value = value.substr(1);
    end = value.find(")");
    std::string_view type = value.substr(start, end - start);
    solve_transform(type, Ematrix);
    multiply_matrix(matrix, Ematrix);
    value = value.substr(end + 1);
  }
}

BaseShape::BaseShape(Attribute *attrs, int attrs_count, BaseShape *parent) {
  if (parent == nullptr) {
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
  } else {
    this->visible = parent->visible;
    this->fill = parent->fill;
    this->stroke = parent->stroke;
    this->opacity = parent->opacity;
    this->fill_opacity = parent->fill_opacity;
    this->stroke_opacity = parent->stroke_opacity;
    this->stroke_width = parent->stroke_width;
    this->stroke_dash_offset = parent->stroke_dash_offset;
    this->stroke_dash_count = parent->stroke_dash_count;
    this->stroke_line_join = parent->stroke_line_join;
    this->stroke_line_cap = parent->stroke_line_cap;
    this->miter_limit = parent->miter_limit;
    this->transform[0][0] = parent->transform[0][0];
    this->transform[0][1] = parent->transform[0][1];
    this->transform[0][2] = parent->transform[0][2];
    this->transform[1][0] = parent->transform[1][0];
    this->transform[1][1] = parent->transform[1][1];
    this->transform[1][2] = parent->transform[1][2];
    this->fill_rule = parent->fill_rule;
  };

  for(int i = 0; i < attrs_count; i++) {
    std::string_view key = attrs[i].key;
    std::string_view value = attrs[i].value;
    
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
        std::from_chars(value.data(), value.data() + value.size(), this->stroke_width);
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
        convert_transform(value, this->transform);   
      } break;

      case ATTRIBUTE_COUNT: {
        __builtin_unreachable();
      }
    }
  }
}



