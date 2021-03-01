#include "mincc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("expected one argment\n");
    exit(1);
  }

  Token *tok = tokenize(argv[1]);
  Node *node = parse(tok);
  codegen(node);
}
