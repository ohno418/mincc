#include "mincc.h"

void gen_addr(Node *node) {
  if (node->kind != ND_VAR)
    error("expected a variable node");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->var->offset);
  printf("  push rax\n");
}

void gen_expr(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  if (node->kind == ND_VAR) {
    gen_addr(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  if (node->kind == ND_ASSIGN) {
    gen_addr(node->lhs);
    gen_expr(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
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

  if (node->kind == ND_RETURN) {
    gen_expr(node->lhs);
    printf("  pop rax\n");
    printf("  jmp .L.return\n");
    return;
  }

  error("statment expected");
}

void codegen(Function *prog) {
  printf("  .intel_syntax noprefix\n");
  printf("  .globl main\n");
  printf("main:\n");

  int stack_size = 0;
  for (Var *v = prog->lvars; v; v = v->next)
    stack_size = stack_size + v->offset;

  // prologue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", stack_size);

  for (Node *n = prog->body; n; n = n->next)
    gen_stmt(n);

  // epilogue
  printf(".L.return:\n");
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}
