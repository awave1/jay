#ifndef AST_H
#define AST_H

struct ast_node {
  struct ast_node *next;     // list of nodes at the same level
  struct ast_node *children; // first child
  char const *val;
};

extern struct ast_node *ast_node_create(char const *val);
extern void printAST(struct ast_node *node, int depth);

#endif /* AST_H */