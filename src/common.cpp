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

std::string_view trim_end(std::string_view data) {
  while (isspace(data[0])) data = data.substr(1);
  while (data.size() && isspace(data[data.size() - 1])) data = data.substr(0, data.size() - 1);

  return data;
}

ArrayList<Attribute> process_style(std::string_view value) {
  ArrayList<Attribute> arr;
  while (value.size() > 0) {
    value = trim_start(value);
    size_t end = value.find(';');
    if (end > value.size()) end = value.size(); 
    std::string_view str = value.substr(0, end);
    
    size_t pos = str.find(':');
    Attribute attr;
    attr.key = str.substr(0, pos);
    attr.key = trim_end(attr.key);

    attr.value = str.substr(pos + 1);
    attr.value = trim_end(attr.value);
    arr.push(attr);

    if (end != value.size()) value = value.substr(end + 1);
    else value = value.substr(end);
  }

  return arr;
}


PercentUnit read_percent_unit(std::string_view value) {
  PercentUnit result;
  result.val = strtod(value.data(), nullptr);
  result.percent = false;
  if (value.size() && value[value.size() - 1] == '%') {
    result.percent = true;
  }

  return result;
}
