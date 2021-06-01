#include "mincc.h"

void gen_expr(Node *node) {
  switch (node->kind) {
  case ND_NUM:
    printf("    push %d\n", node->num);
    break;
  case ND_ADD:
    gen_expr(node->lhs);
    gen_expr(node->rhs);
    printf("    pop rax\n");
    printf("    pop rdi\n");
    printf("    add rax, rdi\n");
    printf("    push rax\n");
    break;
  default:
    fprintf(stderr, "unexpected node");
    exit(1);
  }
}

void codegen(Node *node) {
  printf("    .intel_syntax noprefix\n");
  printf("    .globl main\n");
  printf("main:\n");
  gen_expr(node);
  printf("    pop rax\n");
  printf("    ret\n");
}
