#include "parser.h"
#include "Gradient.h"
#include "InverseIndex.h"

#include "Path.h"
#include "Rect.h"
#include "Circle.h"
#include "Ellipse.h"
#include "Line.h"
#include "Polyline.h"
#include "Polygon.h"
#include "Text.h"
#include "Group.h"

enum ShapeTags {
  SHAPE_TAG_G = 0,
  SHAPE_TAG_PATH,
  SHAPE_TAG_RECT,
  SHAPE_TAG_CIRCLE,
  SHAPE_TAG_ELLIPSE,
  SHAPE_TAG_LINE,
  SHAPE_TAG_POLYLINE,
  SHAPE_TAG_POLYGON,
  SHAPE_TAG_TEXT,
  SHAPE_TAG_SVG,
  SHAPE_TAG_COUNT
};

enum OtherTags {
  OTHER_TAG_DEFS = 0,
  OTHER_TAG_LINEAR_GRADIENT,
  OTHER_TAG_RADIAL_GRADIENT,
  OTHER_TAG_STOP,
  OTHER_TAG_STYLE,
  OTHER_TAG_COUNT
};

constexpr std::string_view shape_tags_str[SHAPE_TAG_COUNT] = {
  "g",
  "path",
  "rect",
  "circle",
  "ellipse",
  "line",
  "polyline",
  "polygon",
  "text",
  "svg",
};

constexpr std::string_view other_tags_str[OTHER_TAG_COUNT] = {
  "defs",
  "linearGradient",
  "radialGradient",
  "stop",
  "style",
};

constexpr InverseIndex<SHAPE_TAG_COUNT> inv_shape_tags {&shape_tags_str};
constexpr InverseIndex<OTHER_TAG_COUNT> inv_other_tags {&other_tags_str};

void parse_stylesheet(std::string_view data, StyleSheet *styles) {
  while (true) {
    size_t pos_start = data.find('{');
    if (pos_start > data.size()) break;

    data = data.substr(1);
    std::string_view key = data.substr(0, pos_start - 1);

    data = data.substr(pos_start);
    size_t pos_end = data.find('}');
    std::string_view value = data.substr(0, pos_end - 1);

    ArrayList<Attribute> attrs = process_style(value);
   
    data = data.substr(pos_end + 1);

    styles->emplace(key, std::move(attrs));
  }
}

GradientMap link_gradients(GradientMap gradients) {
  for (GradientMap::iterator it = gradients.begin(); it != gradients.end(); ++it) {
    std::string_view href = it->second.href;
    if (href.size() == 0 || href[0] != '#') continue;
    href = href.substr(1);
    GradientMap::iterator found = gradients.find(href);
    if (found != gradients.end()) {
      it->second.stops.append(found->second.stops);
    }
  }

  return gradients;
}

ParseResult parse_xml(std::string_view content) {
  int cursor = 0;
  int end = content.size();
  int mark = 0;

  std::unique_ptr<BaseShape> stack;

  std::unique_ptr<BaseShape> head;
  std::unique_ptr<BaseShape> *tail = &head;

  GradientMap gradient_map;
  StyleSheet stylesheet;

  std::string_view current_gradient = "";

  bool reading_style = false;

  // TODO: Add states
  // - Parsing element
  // - Parsing defs
  // - Parsing gradient

  bool is_parsing_tag = false;
  while (cursor < end) {
    if (!is_parsing_tag && content[cursor] == '<') {
      if (SVGShapes::Text* text = dynamic_cast<SVGShapes::Text*>(stack.get())) {
        text->set_text(content.substr(mark, cursor - mark));
      }

      if (reading_style) {
        reading_style = false;
        parse_stylesheet(content.substr(mark, cursor - mark), &stylesheet);
      }
      ++cursor;
      mark = cursor;
      is_parsing_tag = true;
    } else if (is_parsing_tag && content[cursor] == '>') {
      std::string_view tag_content = content.substr(mark, cursor - mark);
      ++cursor;
      mark = cursor;
      is_parsing_tag = false;

      while (tag_content.size() && isspace(tag_content[0])) tag_content = tag_content.substr(1);
      if (tag_content[0] == '!' || tag_content[0] == '?') continue;

      while (tag_content.size() && isspace(tag_content[0])) tag_content = tag_content.substr(1);
      size_t name_end = 0;

      while (name_end < tag_content.size() && !isspace(tag_content[name_end])) ++name_end;

      std::string_view tag_name = tag_content.substr(0, name_end);
      if (tag_name[0] == '/') {
        tag_name = tag_name.substr(1);
        if (inv_shape_tags[tag_name] == -1) {
          if (tag_name == other_tags_str[OTHER_TAG_LINEAR_GRADIENT] ||
              tag_name == other_tags_str[OTHER_TAG_RADIAL_GRADIENT]) {
            current_gradient = "";
          }
          continue;
        }
        if (stack) {
          std::unique_ptr<BaseShape> node = std::move(stack);
          stack = std::move(node->next);

          *tail = std::move(node);
          if (stack.get() == nullptr) {
            if (SVGShapes::SVG *svg = dynamic_cast<SVGShapes::SVG*>(tail->get())) {
              return ParseResult {
                std::move(head),
                link_gradients(std::move(gradient_map)),
                std::move(stylesheet),
                svg
              };
            }
          }

          tail = &(*tail)->next;
        }
        continue;
      }

      tag_content = tag_content.substr(name_end);

      ArrayList<Attribute> attrs;

      while (tag_content.size() && tag_content[0] != '/') {
        while (tag_content.size() && isspace(tag_content[0])) tag_content = tag_content.substr(1);
        size_t eq = tag_content.find('=');
        if (eq == tag_content.npos) break;
        std::string_view attr_key = tag_content.substr(0, eq);
        tag_content = tag_content.substr(eq + 1);
        while (tag_content.size() && tag_content[0] != '\'' && tag_content[0] != '"') {
          tag_content = tag_content.substr(1);
        }
        if (tag_content.size() == 0) break;
        char quote = tag_content[0];
        tag_content = tag_content.substr(1);
        size_t quote_end = tag_content.find(quote);
        if (quote_end == tag_content.npos) break;
        std::string_view attr_value = tag_content.substr(0, quote_end);
        tag_content = tag_content.substr(quote_end + 1);
        attr_key = trim_start(attr_key);
        attr_key = trim_end(attr_key);
        attrs.push(Attribute {attr_key, attr_value});
      }

      std::unique_ptr<BaseShape> new_shape;
 

      switch ((ShapeTags)inv_shape_tags[tag_name]) {
        case SHAPE_TAG_G: {
          new_shape = std::make_unique<SVGShapes::Group>(attrs.begin(), attrs.len(), stack.get(), &stylesheet);
        } break;
        case SHAPE_TAG_PATH: {
          new_shape = std::make_unique<SVGShapes::Path>(attrs.begin(), attrs.len(), stack.get(), &stylesheet);
        } break;
        case SHAPE_TAG_RECT: {
          new_shape = std::make_unique<SVGShapes::Rect>(attrs.begin(), attrs.len(), stack.get(), &stylesheet);
        } break;
        case SHAPE_TAG_CIRCLE: {
          new_shape = std::make_unique<SVGShapes::Circle>(attrs.begin(), attrs.len(), stack.get(), &stylesheet);
        } break;
        case SHAPE_TAG_ELLIPSE: {
          new_shape = std::make_unique<SVGShapes::Ellipse>(attrs.begin(), attrs.len(), stack.get(), &stylesheet);
        } break;
        case SHAPE_TAG_LINE: {
          new_shape = std::make_unique<SVGShapes::Line>(attrs.begin(), attrs.len(), stack.get(), &stylesheet);
        } break;
        case SHAPE_TAG_POLYLINE: {
          new_shape = std::make_unique<SVGShapes::Polyline>(attrs.begin(), attrs.len(), stack.get(), &stylesheet);
        } break;
        case SHAPE_TAG_POLYGON: {
          new_shape = std::make_unique<SVGShapes::Polygon>(attrs.begin(), attrs.len(), stack.get(), &stylesheet);
        } break;
        case SHAPE_TAG_TEXT: {
          new_shape = std::make_unique<SVGShapes::Text>(attrs.begin(), attrs.len(), stack.get(), &stylesheet);
        } break;
        case SHAPE_TAG_SVG: {
          new_shape = std::make_unique<SVGShapes::SVG>(attrs.begin(), attrs.len(), stack.get(), &stylesheet);
        } break;
        case SHAPE_TAG_COUNT: {
          __builtin_unreachable();
        }
      }

      switch ((OtherTags)inv_other_tags[tag_name]) {
        case OTHER_TAG_LINEAR_GRADIENT: {
          if (current_gradient == "") {
            Gradient gradient = read_gradient(GRADIENT_TYPE_LINEAR, attrs.begin(), attrs.len());
            if (gradient.id != "") {
              gradient_map.emplace(gradient.id, std::move(gradient));
              current_gradient = gradient.id;
            }
          }
        } break;
        case OTHER_TAG_RADIAL_GRADIENT: {
          if (current_gradient == "") {
            Gradient gradient = read_gradient(GRADIENT_TYPE_RADIAL, attrs.begin(), attrs.len());
            if (gradient.id != "") {
              gradient_map.emplace(gradient.id, std::move(gradient));
              current_gradient = gradient.id;
            }
          }
        } break;
        case OTHER_TAG_STOP: {
          if (current_gradient != "") {
            gradient_map[current_gradient].stops.push(read_stop(attrs.begin(), attrs.len()));
          }
        } break;
        case OTHER_TAG_DEFS: {
        } break;
        case OTHER_TAG_STYLE: {
          reading_style = true;
        } break;
        case OTHER_TAG_COUNT: {
          __builtin_unreachable();
        } break;
      }

      bool inline_end = tag_content.size() && tag_content[0] == '/';
      if (new_shape) {
        if (inline_end) {
          *tail = std::move(new_shape);
          tail = &(*tail)->next;
        } else {
          new_shape->next = std::move(stack);
          stack = std::move(new_shape);
        }
      } else if (inline_end) {
        if (tag_name == other_tags_str[OTHER_TAG_LINEAR_GRADIENT] ||
            tag_name == other_tags_str[OTHER_TAG_RADIAL_GRADIENT]) {
          current_gradient = "";
        }
      }
    } else {
      ++cursor;
    }
  }

  return ParseResult {
    std::move(head),
    link_gradients(std::move(gradient_map)),
    std::move(stylesheet),
    nullptr
  };
}
