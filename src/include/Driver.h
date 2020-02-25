#pragma once
#include <memory>

#include "../../Lexer.h"
#include "../../parser.tab.hh"
#include "ast.h"

namespace yy {

struct Driver {
  Driver() = default;
  Driver(Driver const &) = delete;

  std::unique_ptr<Lexer> m_lexer;
  std::unique_ptr<Parser> m_parser;
  ASTNode *m_ast;

  ASTNode *parse(std::istream *is);
};

} // namespace yy
