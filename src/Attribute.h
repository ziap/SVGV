#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string_view>

class Attribute {
public:
  std::string_view key;
  std::string_view value;
};

#endif
