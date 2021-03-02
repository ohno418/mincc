#include "mincc.h"

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

_Bool equal(Token *tok, char *str) {
  return tok->len == strlen(str) &&
    strncmp(tok->loc, str, tok->len) == 0;
}

Node *add(Token **rest, Token *tok);
Node *num(Token **rest, Token *tok);

// add = num (+ num)?
Node *add(Token **rest, Token *tok) {
  Node *node = num(&tok, tok);

  if (tok && equal(tok, "+")) {
    Node *binary = new_node(ND_ADD);
    binary->lhs = node;
    binary->rhs = num(&tok, tok->next);
    node = binary;
  }

  *rest = tok;
  return node;
}

Node *num(Token **rest, Token *tok) {
  if (tok->kind != TK_NUM) {
    printf("expected a number\n");
    exit(1);
  }

  Node *node = new_node(ND_NUM);
  node->val = atoi(tok->loc);
  *rest = tok->next;
  return node;
}

Node *parse(Token *tok) {
  return add(&tok, tok);
}
