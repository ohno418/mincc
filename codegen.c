#include "mincc.h"

void codegen(Node *node) {
  printf("  .intel_syntax noprefix\n");
  printf("  .globl main\n");
  printf("main:\n");
  printf("  mov rax, %d\n", node->val);
  printf("  ret\n");
}
