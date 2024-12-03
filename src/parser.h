#ifndef PARSER_H
#define PARSER_H

#include "SVG.h"
#include "BaseShape.h"

struct ParseResult {
  std::unique_ptr<BaseShape> shapes;
  SVGShapes::SVG *root;
};

ParseResult parse_xml(std::string_view content);

#endif
