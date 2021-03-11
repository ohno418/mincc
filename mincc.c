#include "mincc.h"

int main(int argc, char **argv) {
  if (argc != 2)
    error("expected one argment");

  Token *tok = tokenize(argv[1]);
  Function *prog = parse(tok);
  codegen(prog);
}
