#include "mincc.h"

void gen_expr(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen_expr(node->lhs);
  printf("  pop rdi\n");
  gen_expr(node->rhs);
  printf("  pop rax\n");

  if (node->kind == ND_ADD) {
    printf("  add rax, rdi\n");
    printf("  push rax\n");
    return;
  }

  printf("expression expected");
  exit(1);
}

void codegen(Node *node) {
  printf("  .intel_syntax noprefix\n");
  printf("  .globl main\n");
  printf("main:\n");
  gen_expr(node);
  printf("  pop rax\n");
  printf("  ret\n");
}
