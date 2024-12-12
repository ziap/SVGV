#ifndef COMMON_H
#define COMMON_H
#include <string_view>

class Attribute {
public:
  std::string_view key;
  std::string_view value;
};

double convert_opacity(std::string_view value);

std::string_view trim_start(std::string_view sv);

void convert_array(std::string_view value, double *a, int *count);

double read_double(std::string_view *str);

#endif
