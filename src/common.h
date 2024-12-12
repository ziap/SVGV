#ifndef COMMON_H
#define COMMON_H
#include <string_view>

double convert_opacity(std::string_view value);

std::string_view trim_start(std::string_view sv);

void convert_array(std::string_view value, double *a, int *count);

double read_double(std::string_view *str);

#endif
