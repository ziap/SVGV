#ifndef PARSER_H
#define PARSER_H

#include "BaseShape.h"

std::unique_ptr<BaseShape> parse_xml(std::string_view content);

#endif
