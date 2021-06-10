#include "mincc.h"

static Function *current_fn;
char *arg_regs[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
int label_cnt = 0;

void gen_expr(Node *node);

// Push address of the variable.
void gen_addr(Node *node) {
  switch (node->kind) {
  case ND_VAR:
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->var->offset);
    printf("    push rax\n");
    return;
  case ND_DEREF:
    gen_expr(node->lhs);
    return;
  default:
    fprintf(stderr, "not lvalue\n");
    exit(1);
  }
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
  case ND_LT: {
    int label = label_cnt;
    label_cnt++;
    gen_expr(node->lhs);
    gen_expr(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");
    printf("    cmp rax, rdi\n");
    printf("    jl .L.%d\n", label);
    printf("    push 0\n");
    printf("    jmp .L.end.%d\n", label);
    printf(".L.%d:\n", label);
    printf("    push 1\n");
    printf(".L.end.%d:\n", label);
    break;
  }
  case ND_LTE: {
    int label = label_cnt;
    label_cnt++;
    gen_expr(node->lhs);
    gen_expr(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");
    printf("    cmp rax, rdi\n");
    printf("    jle .L.%d\n", label);
    printf("    push 0\n");
    printf("    jmp .L.end.%d\n", label);
    printf(".L.%d:\n", label);
    printf("    push 1\n");
    printf(".L.end.%d:\n", label);
    break;
  }
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
  case ND_FUNCALL: {
    int i = 0;
    for (Node *arg = node->args; arg; arg = arg->next) {
      gen_expr(arg);
      printf("    pop rax\n");
      printf("    mov %s, rax\n", arg_regs[i]);
      i++;
    }

    printf("    call %s\n", node->fn_name);
    printf("    push rax\n");
    break;
  }
  case ND_ADDR:
    gen_addr(node->lhs);
    printf("    pop rax\n");
    printf("    lea rax, [rax]\n");
    printf("    push rax\n");
    break;
  case ND_DEREF:
    gen_expr(node->lhs);
    printf("    pop rax\n");
    printf("    mov rax, [rax]\n");
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
  case ND_BLOCK:
    for (Node *stmt = node->body; stmt; stmt = stmt->next)
      gen_stmt(stmt);
    break;
  case ND_IF: {
    int label = label_cnt;
    label_cnt++;
    gen_expr(node->cond);
    printf("    pop rax\n");
    printf("    cmp rax, 0\n");
    printf("    je .L.if.else.%d\n", label);
    gen_stmt(node->then);

    printf(".L.if.else.%d:\n", label);
    if (node->els)
      gen_stmt(node->els);
    break;
  }
  case ND_FOR: {
    int label = label_cnt;
    label_cnt++;
    if (node->init) {
      gen_expr(node->init);
      printf("    pop rax\n");
    }
    printf(".L.for.then.%d:\n", label);
    gen_stmt(node->then);
    if (node->inc) {
      gen_expr(node->inc);
      printf("    pop rax\n");
    }
    if (node->cond) {
      gen_expr(node->cond);
      printf("    pop rax\n");
    }
    printf("    cmp rax, 0\n");
    printf("    jne .L.for.then.%d\n", label);
    printf(".L.for.end.%d:\n", label);
    break;
  }
  case ND_SWITCH: {
    int label = label_cnt;
    label_cnt++;

    gen_expr(node->cond);
    printf("    pop rax\n");

    int i = 0;
    for (
      Node *case_node = node->case_next;
      case_node;
      case_node = case_node->case_next
    ) {
      gen_expr(case_node->cond);
      printf("    pop rdi\n");
      printf("    cmp rax, rdi\n");
      printf("    je .L.%d.case.%d\n", label, i);
      i++;
    }

    int j = 0;
    for (
      Node *case_node = node->case_next;
      case_node;
      case_node = case_node->case_next
    ) {
      printf(".L.%d.case.%d:\n", label, j);
      gen_stmt(case_node->then);
      printf("    pop rax\n");
      j++;
    }
    break;
  }
  case ND_EXPR_STMT:
    gen_expr(node->lhs);
    printf("    pop rax\n");
    break;
  default:
    fprintf(stderr, "unknown statement\n");
    exit(1);
  }
}

int align_to(int n, int align) {
  return (n + align - 1) / align * align;
}

void assign_lvar_offsets(Function *fn) {
  int offset = 0;
  for (Var *var = fn->params; var; var = var->next) {
    offset += var->ty->size;
    var->offset = offset;
  }
  for (Var *var = fn->locals; var; var = var->next) {
    offset += var->ty->size;
    var->offset = offset;
  }
  fn->stack_size = align_to(offset, 16);
}

void assign_params(Var *params) {
  int i = 0;
  for (Var *param = params; param; param = param->next) {
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", param->offset);
    printf("    mov [rax], %s\n", arg_regs[i]);
    i++;
  }
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
    printf("    sub rsp, %d\n", f->stack_size);

    assign_params(f->params);

    for (Node *node = f->body; node; node = node->next)
      gen_stmt(node);

    printf(".L.return.%s:\n", f->name);
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
  }
}
