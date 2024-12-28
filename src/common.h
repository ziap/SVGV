#ifndef COMMON_H
#define COMMON_H
#include "ArrayList.h"

#include <string_view>
#include <unordered_map>

class Attribute {
public:
  std::string_view key;
  std::string_view value;
};

using StyleSheet = std::unordered_map<std::string_view, ArrayList<Attribute>>;

struct PercentUnit{
  double val;
  bool percent;
};


double convert_percent(std::string_view value);

std::string_view trim_start(std::string_view sv);

void convert_array(std::string_view value, double *a, int *count);

double read_double(std::string_view *str);

std::string_view trim_end(std::string_view data);

ArrayList<Attribute> process_style(std::string_view value);

PercentUnit read_percent_unit(std::string_view value);

#endif
