#include "mincc.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "no input");
    exit(1);
  }

  Token *tok = tokenize(argv[1]);
  Node *node = parse(tok);
  codegen(node);

  return 0;
}
