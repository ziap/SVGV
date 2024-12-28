#ifndef PARSER_H
#define PARSER_H

#include "SVG.h"
#include "Gradient.h"
#include "BaseShape.h"

using GradientMap = std::unordered_map<std::string_view, Gradient>;

struct ParseResult {
  std::unique_ptr<BaseShape> shapes;
  GradientMap gradient_map;
  StyleSheet stylesheet;
  SVGShapes::SVG *root;
};

ParseResult parse_xml(std::string_view content);

#endif
