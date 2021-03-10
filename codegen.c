#include "mincc.h"

void gen_expr(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  // lhs on RAX
  // rhs on RDI
  gen_expr(node->lhs);
  gen_expr(node->rhs);
  printf("  pop rdi\n");
  printf("  pop rax\n");

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

  if (node->kind == ND_MUL) {
    printf("  imul rax, rdi\n");
    printf("  push rax\n");
    return;
  }

  if (node->kind == ND_DIV) {
    printf("  cqo\n");
    printf("  idiv rdi\n");
    printf("  push rax\n");
    return;
  }

  if (node->kind == ND_EQ) {
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    printf("  push rax\n");
    return;
  }

  if (node->kind == ND_NEQ) {
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    printf("  push rax\n");
    return;
  }

  error("expression expected");
}

void gen_stmt(Node *node) {
  if (node->kind == ND_EXPR_STMT) {
    gen_expr(node->lhs);
    printf("  pop rax\n");
    return;
  }

  error("statment expected");
}

void codegen(Node *node) {
  printf("  .intel_syntax noprefix\n");
  printf("  .globl main\n");
  printf("main:\n");

  for (Node *n = node; n; n = n->next)
    gen_stmt(n);

  printf("  ret\n");
}
