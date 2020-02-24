#include "./include/ast.h"

#include <stdio.h>
#include <stdlib.h>

struct ast_node *ast_node_create(char const *val) {
  struct ast_node *ret = malloc(sizeof(struct ast_node));
  ret->val = val;
  ret->next = 0;
  return ret;
}

void printAST(struct ast_node *node, int depth) {
  if (!node)
    return;

  for (int i = 0; i < depth; ++i)
    printf("\t");

  printf("%s\n", node->val);

  printAST(node->children, depth + 1);
  for (struct ast_node *next = node->next; next; next = next->next)
    printAST(next, depth);
}
