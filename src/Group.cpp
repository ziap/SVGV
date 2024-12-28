#include "Group.h"

using namespace SVGShapes;

Group::Group(Attribute *attrs, int attrs_count, BaseShape *parent, StyleSheet *styles) :
  BaseShape{attrs, attrs_count, parent, styles} {}
