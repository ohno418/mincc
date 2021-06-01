#include "mincc.h"

_Bool equal(Token *tok, char *str) {
  return
    tok->len == strlen(str) &&
    strncmp(tok->loc, str, tok->len) == 0;
}

Node *new_num_node(Token *tok, Token **rest) {
  if (tok->kind != TK_NUM) {
    fprintf(stderr, "expected number token");
    exit(1);
  }

  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->num = atoi(tok->loc);
  *rest = tok->next;
  return node;
}

Node *new_binary_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

// num ("+" num)?
Node *expr(Token *tok, Token **rest) {
  Node *node = new_num_node(tok, &tok);

  if (equal(tok, "+")) {
    Node *rhs = new_num_node(tok->next, &tok);
    node = new_binary_node(ND_ADD, node, rhs);
  }

  *rest = tok;
  return node;
}

Node *parse(Token *tok) {
  Node *node = expr(tok, &tok);

  if (tok->kind != TK_EOF) {
    fprintf(stderr, "extra token");
    exit(1);
  }

  return node;
}
