#include "common.h"
#include <cctype>
#include <cmath>

double convert_percent(std::string_view value) {
  double opacity = strtod(value.data(), nullptr);
  if (value[value.size() - 1] == '%') {
    opacity /= 100;
  }

  return opacity;
}

std::string_view trim_start(std::string_view sv) {
  while (sv.size() && (isspace(sv[0]) || sv[0] == ',')) sv = sv.substr(1);
  return sv;
}

void convert_array(std::string_view value, double *a, int *count) {
  while (value.size() > 0 && (*count) < 8) {
    value = trim_start(value);
    char *end;
    a[*count] = strtod(value.data(), &end);
    if (end != value.data()) ++*count;
    value = value.substr(end - value.data());
  }
}

double read_double(std::string_view *str) {
  *str = trim_start(*str);

  char *out;
  double num = strtod(str->data(), &out);
  *str = str->substr(out - str->data());

  *str = trim_start(*str);
  return num;
}