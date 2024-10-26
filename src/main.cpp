#include "ArrayList.h"
#include "XMLNode.h"
#include "BaseShape.h"
#include <iostream>
#include <fstream>
#include <sstream>

BaseShape *parse_xml(std::string_view content) {
  int cursor = 0;
  int end = content.size();
  int mark = 0;

  ArrayList<BaseShape *> stack;

  std::unique_ptr<BaseShape> head;
  std::unique_ptr<BaseShape> *tail = &head;

  bool is_parsing_tag = false;
  while (cursor < end) {
    if (!is_parsing_tag && content[cursor] == '<') {
      // TODO: Add text support
      // std::cout << "Content: " << content.substr(mark, cursor - mark) << '\n';
      ++cursor;
      mark = cursor;
      is_parsing_tag = true;
    } else if (is_parsing_tag && content[cursor] == '>') {
      std::string_view tag_content = content.substr(mark, cursor - mark);
      // std::cout << "Tag: " << tag_content << '\n';
      ++cursor;
      mark = cursor;
      is_parsing_tag = false;

      while (tag_content.size() && isspace(tag_content[0])) tag_content = tag_content.substr(1);
      if (tag_content[0] == '!' || tag_content[0] == '?') continue;

      if (tag_content[0] == '/') {
        continue;
      }

      while (tag_content.size() && isspace(tag_content[0])) tag_content = tag_content.substr(1);
      int name_end = 0;

      while (name_end < tag_content.size() && !isspace(tag_content[name_end])) ++name_end;
      std::string_view tag_name = tag_content.substr(0, name_end);
      tag_content = tag_content.substr(name_end);
      
      while (tag_content.size() && isspace(tag_content[0])) tag_content = tag_content.substr(1);

      std::cout << tag_name << ": " << tag_content << '\n';

      ArrayList<Attribute> attrs;

      while (tag_content.size() && tag_content[0] != '/') {
        
      }

      if (tag_content.size() && tag_content[0] == '/') {

      }
    } else {
      ++cursor;
    }
  }

  return *stack.pop();
}

std::string read_file(const char *filename) {
  std::ifstream fin(filename);
  std::ostringstream ss;
  ss << fin.rdbuf();
  return ss.str();
}

int main() {
  std::string svg = read_file("examples/tiger.svg");
  BaseShape *shape = parse_xml(svg);

  return 0;
}
