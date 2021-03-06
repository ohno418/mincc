#include "mincc.h"

void gen_expr(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  // lhs on RAX
  // rhs on RDI
  gen_expr(node->lhs);
  printf("  pop rax\n");
  gen_expr(node->rhs);
  printf("  pop rdi\n");

  if (node->kind == ND_ADD) {
    printf("  add rax, rdi\n");
    printf("  push rax\n");
    return;
  }

  if (node->kind == ND_SUB) {
    printf("  sub rax, rdi\n");
    printf("  push rax\n");
    return;
  }

  error("expression expected");
}

void codegen(Node *node) {
  printf("  .intel_syntax noprefix\n");
  printf("  .globl main\n");
  printf("main:\n");
  gen_expr(node);
  printf("  pop rax\n");
  printf("  ret\n");
}
