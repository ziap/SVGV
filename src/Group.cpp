#include "Group.h"

using namespace SVGShapes;

Group::Group(Attribute *attrs, int attrs_count, BaseShape *parent) : BaseShape{attrs, attrs_count, parent} {}

void Group::render(Gdiplus::Graphics *) const {

}
