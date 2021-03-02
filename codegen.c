#include "mincc.h"

void expr(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  expr(node->lhs);
  printf("  pop rdi\n");
  expr(node->rhs);
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
  expr(node);
  printf("  pop rax\n");
  printf("  ret\n");
}
