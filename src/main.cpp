#include <iostream>
#include <fstream>
#include <sstream>

#include "parser.h"

std::string read_file(const char *filename) {
  std::ifstream fin(filename);
  std::ostringstream ss;
  ss << fin.rdbuf();
  return ss.str();
}

int main() {
  std::string svg = read_file("examples/test.svg");
  std::unique_ptr<BaseShape> shape = parse_xml(svg);
  std::cout << "Parsed\n";
  return 0;
}
