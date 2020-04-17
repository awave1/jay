#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include "ASTNode.hpp"
#include "StringTable.hpp"
#include "SymTable.hpp"
#include "Symbol.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace yy;

// void insert_tabs(int depth, std::ostream &os, int tabsize);
inline void insert_tabs(int depth, std::ostream &os, int tabsize = 2) {
  for (int i = 0; i < depth; ++i) {
    os << std::string(tabsize, ' ');
  }
}

struct PrintDecorator {
  std::function<void(ASTNode *)> pre;
  std::function<void(ASTNode *)> post;
};

struct PrettyPrinter {
  struct tc_impl {
    friend std::ostream &operator<<(std::ostream &os,
                                    PrettyPrinter::tc_impl const &gen) {
      return os;
    }
  };

  struct line_impl {
    int tab_depth;
    std::string message;

    friend std::ostream &operator<<(std::ostream &os,
                                    PrettyPrinter::line_impl const &gen) {
      os << "\n";
      insert_tabs(gen.tab_depth, os);
      return os << gen.message;
    }
  };

  struct add_impl {
    std::string message;

    friend std::ostream &operator<<(std::ostream &os,
                                    PrettyPrinter::add_impl const &gen) {
      return os << gen.message;
    }
  };

  int tab_depth = 0;
  std::unordered_map<ASTNode *, PrintDecorator> decorations;

  add_impl add(std::string const &message, bool first_space = true) {
    if (first_space) {
      return add_impl{" " + message};
    }

    return add_impl{message};
  }

  add_impl add_name(std::string const &name) { return add("$" + name); }

  add_impl add_int_const(int val) {
    std::stringstream ss;
    ss << val;
    return add(ss.str());
  }

  add_impl add_bool_const(bool val) {
    std::stringstream ss;
    ss << val;
    return add(ss.str());
  }

  add_impl add_param(std::string const &name) {
    std::stringstream ss;
    ss << "(param $" << name << " i32)";
    return add(ss.str());
  }

  add_impl add_local(std::string const &name) {
    std::stringstream ss;
    ss << "(local $" << name << " i32)";
    return add(ss.str());
  }

  tc_impl indent() {
    ++tab_depth;
    return {};
  }

  tc_impl dedent() {
    --tab_depth;
    return {};
  }

  // generates a new line tabbed in
  line_impl line(std::string const &content) { return {tab_depth, content}; }
};

class CodeGenerator {
public:
  CodeGenerator(std::shared_ptr<ASTNode> ast,
                std::shared_ptr<SymTable> sym_table, std::ostream &out)
      : ast(ast), sym_table(sym_table), out(out) {
    str_table = std::unique_ptr<StringTable>(new StringTable());
    printer = std::shared_ptr<PrettyPrinter>(new PrettyPrinter);
    while_block_state = 0;
    stack_dummy_var = "__stack_dummy_var";
  };

  /**
   * @brief
   *
   * @return std::string
   */
  void generate_wasm();

private:
  std::shared_ptr<ASTNode> ast;
  std::shared_ptr<SymTable> sym_table;
  std::unique_ptr<StringTable> str_table;
  std::ostream &out;
  std::shared_ptr<PrettyPrinter> printer;
  int while_block_state;
  std::string start_func_name;
  std::string stack_dummy_var;

  void traverse(ASTNode *node,
                std::function<void(ASTNode *n, std::ostream &out)> pre,
                std::function<void(ASTNode *n, std::ostream &out)> post,
                std::ostream &out);

  void generate_vars(std::string scope_name);

  void build_function_call();

  void inject_runtime();

  std::string get_block_state() { return std::to_string(while_block_state); }

  void next_block_state() { while_block_state++; }
  void prev_block_state() { while_block_state--; }

  void codegen_pre_traversal_cb(ASTNode *node, std::ostream &out);
  void codegen_post_traversal_cb(ASTNode *node, std::ostream &out);

  /**
   * @brief Generate a string table that will be inserted in the generated WASM
   * code
   *
   * @param node AST node
   */
  void build_string_table_post_traversal_cb(ASTNode *node, std::ostream &out);
};

#endif /* CODE_GENERATOR_HPP */