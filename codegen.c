#include "mincc.h"

Function *current_fn;
int label_cnt = 0;

// Push address of the variable.
void gen_addr(Node *node) {
  if (node->kind != ND_VAR) {
    fprintf(stderr, "not lvalue\n");
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
  case ND_LT:
    gen_expr(node->lhs);
    gen_expr(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");
    printf("    cmp rax, rdi\n");
    printf("    jl .L.%d\n", label_cnt);
    printf("    push 0\n");
    printf("    jmp .L.end.%d\n", label_cnt);
    printf(".L.%d:\n", label_cnt);
    printf("    push 1\n");
    printf(".L.end.%d:\n", label_cnt);
    label_cnt++;
    break;
  case ND_LTE:
    gen_expr(node->lhs);
    gen_expr(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");
    printf("    cmp rax, rdi\n");
    printf("    jle .L.%d\n", label_cnt);
    printf("    push 0\n");
    printf("    jmp .L.end.%d\n", label_cnt);
    printf(".L.%d:\n", label_cnt);
    printf("    push 1\n");
    printf(".L.end.%d:\n", label_cnt);
    label_cnt++;
    break;
  case ND_ASSIGN:
    gen_addr(node->lhs);
    gen_expr(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");
    printf("    mov [rax], rdi\n");
    printf("    push rdi\n");
    break;
  case ND_VAR:
    if (!node->var) {
      fprintf(stderr, "unknown variable\n");
      exit(1);
    }

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->var->offset);
    printf("    mov rax, [rax]\n");
    printf("    push rax\n");
    break;
  case ND_FUNCALL:
    printf("    call %s\n", node->fn_name);
    printf("    push rax\n");
    break;
  default:
    fprintf(stderr, "unknown expression\n");
    exit(1);
  }
}

void gen_stmt(Node *node) {
  switch (node->kind) {
  case ND_RETURN:
    gen_expr(node->lhs);
    printf("    pop rax\n");
    printf("    jmp .L.return.%s\n", current_fn->name);
    break;
  case ND_EXPR_STMT:
    gen_expr(node->lhs);
    printf("    pop rax\n");
    break;
  default:
    fprintf(stderr, "unknown statement\n");
    exit(1);
  }
}

void assign_lvar_offsets(Function *fn) {
  int offset = 0;
  for (Var *var = fn->lvars; var; var = var->next) {
    offset += 8;
    var->offset = offset;
  }
}

int align_to(int n, int align) {
  return (n + align - 1) / align * align;
}

int stack_size(Function *fn) {
  int stack_size = 0;
  for (Var *var = fn->lvars; var; var = var->next)
    stack_size += var->offset;
  return align_to(stack_size, 16);
}

void codegen(Function *fn) {
  printf("    .intel_syntax noprefix\n");

  for (Function *f = fn; f; f = f->next) {
    current_fn = f;

    printf("    .globl %s\n", f->name);
    printf("%s:\n", f->name);
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");

    assign_lvar_offsets(f);
    printf("    sub rsp, %d\n", stack_size(f));

    for (Node *node = f->body; node; node = node->next)
      gen_stmt(node);

    printf(".L.return.%s:\n", f->name);
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
  }
}
