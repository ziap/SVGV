#include "parser.h"
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

#include <iostream>

enum SVGTags {
  TAG_G = 0,
  TAG_PATH,
  TAG_RECT,
  TAG_CIRCLE,
  TAG_ELLIPSE,
  TAG_LINE,
  TAG_POLYLINE,
  TAG_POLYGON,
  TAG_TEXT,
  TAG_SVG,
  TAG_LINEAR_GRADIENT,
  TAG_RADIAL_GRADIENT,
  TAG_STOP,
  TAG_DEFS,
  TAG_COUNT
};

constexpr std::string_view tags_str[TAG_COUNT] = {
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
  "linearGradient",
  "radialGradient",
  "stop",
  "defs"
};

constexpr InverseIndex<TAG_COUNT> inv_tags = {&tags_str};

ParseResult parse_xml(std::string_view content) {
  int cursor = 0;
  int end = content.size();
  int mark = 0;

  std::unique_ptr<BaseShape> stack;

  std::unique_ptr<BaseShape> head;
  std::unique_ptr<BaseShape> *tail = &head;

  // TODO: Add states
  // - Parsing element
  // - Parsing defs
  // - Parsing gradient

  bool is_parsing_tag = false;
  while (cursor < end) {
    if (!is_parsing_tag && content[cursor] == '<') {
      if (SVGShapes::Text* text = dynamic_cast<SVGShapes::Text*>(stack.get())) {
        text->content = content.substr(mark, cursor - mark);
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
        if (inv_tags[tag_name.substr(1)] == -1) continue;
        if (stack) {
          std::unique_ptr<BaseShape> node = std::move(stack);
          stack = std::move(node->next);

          *tail = std::move(node);
          if (stack.get() == nullptr) {
            if (SVGShapes::SVG *svg = dynamic_cast<SVGShapes::SVG*>(tail->get())) {
              return ParseResult { std::move(head), svg };
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
        attrs.push({attr_key, attr_value});
      }

      std::unique_ptr<BaseShape> new_shape;
 

      switch ((SVGTags)inv_tags[tag_name]) {
        case TAG_G: {
          new_shape = std::make_unique<SVGShapes::Group>(attrs.begin(), attrs.len(), stack.get());
        } break;
        case TAG_PATH: {
          new_shape = std::make_unique<SVGShapes::Path>(attrs.begin(), attrs.len(), stack.get());
        } break;
        case TAG_RECT: {
          new_shape = std::make_unique<SVGShapes::Rect>(attrs.begin(), attrs.len(), stack.get());
        } break;
        case TAG_CIRCLE: {
          new_shape = std::make_unique<SVGShapes::Circle>(attrs.begin(), attrs.len(), stack.get());
        } break;
        case TAG_ELLIPSE: {
          new_shape = std::make_unique<SVGShapes::Ellipse>(attrs.begin(), attrs.len(), stack.get());
        } break;
        case TAG_LINE: {
          new_shape = std::make_unique<SVGShapes::Line>(attrs.begin(), attrs.len(), stack.get());
        } break;
        case TAG_POLYLINE: {
          new_shape = std::make_unique<SVGShapes::Polyline>(attrs.begin(), attrs.len(), stack.get());
        } break;
        case TAG_POLYGON: {
          new_shape = std::make_unique<SVGShapes::Polygon>(attrs.begin(), attrs.len(), stack.get());
        } break;
        case TAG_TEXT: {
          new_shape = std::make_unique<SVGShapes::Text>(attrs.begin(), attrs.len(), stack.get());
        } break;
        case TAG_SVG: {
          new_shape = std::make_unique<SVGShapes::SVG>(attrs.begin(), attrs.len(), stack.get());
        } break;
        case TAG_LINEAR_GRADIENT: {
          new_shape = std::make_unique<BaseShape>(attrs.begin(), attrs.len(), stack.get());
          std::cout << "ERROR: Gradient not supported\n";
        } break;
        case TAG_RADIAL_GRADIENT: {
          new_shape = std::make_unique<BaseShape>(attrs.begin(), attrs.len(), stack.get());
          std::cout << "ERROR: Gradient not supported\n";
        } break;
        case TAG_STOP: {
          new_shape = std::make_unique<BaseShape>(attrs.begin(), attrs.len(), stack.get());
          std::cout << "ERROR: Gradient not supported\n";
        } break;
        case TAG_DEFS: {
          new_shape = std::make_unique<BaseShape>(attrs.begin(), attrs.len(), stack.get());
          std::cout << "ERROR: Gradient not supported\n";
        } break;
        case TAG_COUNT: {
          __builtin_unreachable();
        } break;
      }

      if (new_shape) {
        if (tag_content.size() && tag_content[0] == '/') {
          *tail = std::move(new_shape);
          tail = &(*tail)->next;
        } else {
          new_shape->next = std::move(stack);
          stack = std::move(new_shape);
        }
      }
    } else {
      ++cursor;
    }
  }

  return ParseResult { std::move(head), nullptr };
}
