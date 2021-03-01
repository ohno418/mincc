#include "mincc.h"

Node *parse(Token *tok) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = atoi(tok->loc);
  return node;
}
