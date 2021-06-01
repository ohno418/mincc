#include "mincc.h"

// Push address of the variable.
void gen_addr(Node *node) {
  if (node->kind != ND_VAR) {
    fprintf(stderr, "not lvalue");
    exit(1);
  }

  printf("    mov rax, rbp\n");
  printf("    sub rax, %d\n", node->var->offset);
  printf("    push rax\n");
}

void gen_expr(Node *node) {
  switch (node->kind) {
  case ND_NUM:
    printf("    push %d\n", node->num);
    break;
  case ND_ADD:
    gen_expr(node->lhs);
    gen_expr(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");
    printf("    add rax, rdi\n");
    printf("    push rax\n");
    break;
  case ND_SUB:
    gen_expr(node->lhs);
    gen_expr(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");
    printf("    sub rax, rdi\n");
    printf("    push rax\n");
    break;
  case ND_MUL:
    gen_expr(node->lhs);
    gen_expr(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");
    printf("    imul rax, rdi\n");
    printf("    push rax\n");
    break;
  case ND_DIV:
    gen_expr(node->lhs);
    gen_expr(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");
    printf("    cqo\n");
    printf("    idiv rdi\n");
    printf("    push rax\n");
    break;
  case ND_ASSIGN:
    gen_addr(node->lhs);
    gen_expr(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");
    printf("    mov [rax], rdi\n");
    printf("    push rdi\n");
    break;
  case ND_VAR: {
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->var->offset);
    printf("    mov rax, [rax]\n");
    printf("    push rax\n");
    break;
  }
  default:
    fprintf(stderr, "unknown expression");
    exit(1);
  }
}

void gen_stmt(Node *node) {
  switch (node->kind) {
  case ND_EXPR_STMT:
    gen_expr(node->lhs);
    printf("    pop rax\n");
    break;
  default:
    fprintf(stderr, "unknown statement");
    exit(1);
  }
}

void codegen(Node *node) {
  printf("    .intel_syntax noprefix\n");
  printf("    .globl main\n");

  printf("main:\n");
  printf("    push rbp\n");
  printf("    mov rbp, rsp\n");
  printf("    sub rsp, 208\n");

  for (Node *stmt = node; stmt; stmt = stmt->next)
    gen_stmt(stmt);

  printf("    mov rsp, rbp\n");
  printf("    pop rbp\n");
  printf("    ret\n");
}
