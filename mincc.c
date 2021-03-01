#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("expected one argment\n");
    exit(1);
  }

  printf("  .intel_syntax noprefix\n");
  printf("  .globl main\n");
  printf("main:\n");
  printf("  mov rax, %s\n", argv[1]);
  printf("  ret\n");
}
