#pragma once
#include <memory>

#include "AST.h"
#include "Lexer.h"
#include "parser.tab.hh"



namespace yy {
struct Driver {
	Driver() = default;
	Driver(Driver const&) = delete;

	std::unique_ptr<Lexer> m_lexer;
	std::unique_ptr<Parser> m_parser;
	ASTNode* m_ast;

	ASTNode* parse(std::istream* is);
};

}
