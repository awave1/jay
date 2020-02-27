#pragma once
#include <memory>

#include "../../Lexer.h"
#include "../../parser.tab.hh"
#include "ast.hpp"

namespace yy {

struct Driver {
  Driver() = default;
  Driver(Driver const &) = delete;

  std::unique_ptr<Lexer> m_lexer;
  std::unique_ptr<Parser> m_parser;
  ast_node_t *ast;

  ast_node_t *parse(std::istream *is);
};

} // namespace yy
