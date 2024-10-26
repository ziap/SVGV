#ifndef XML_NODE_H
#define XML_NODE_H

#include <string_view>

class Attribute {
public:
  std::string_view key;
  std::string_view value;
};

class XMLNode {
public:
  std::string_view tag_name;

  Attribute *attrs;
  int attrs_count;

  XMLNode *children;
  int children_count;
};

#endif
