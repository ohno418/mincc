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

Node *expr(Token **rest, Token *tok);
Node *add(Token **rest, Token *tok);
Node *num(Token **rest, Token *tok);

// expr = add
Node *expr(Token **rest, Token *tok) {
  return add(rest, tok);
}

// add = num ("+" num | "-" num)*
Node *add(Token **rest, Token *tok) {
  Node *node = num(&tok, tok);

  for (;;) {
    if (equal(tok, "+")) {
      Node *binary = new_node(ND_ADD);
      binary->lhs = node;
      binary->rhs = num(&tok, tok->next);
      node = binary;
      continue;
    }

    if (equal(tok, "-")) {
      Node *binary = new_node(ND_SUB);
      binary->lhs = node;
      binary->rhs = num(&tok, tok->next);
      node = binary;
      continue;
    }

    break;
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
  Node *node = expr(&tok, tok);

  if (tok->kind != TK_EOF) {
    printf("extra token\n");
    exit(1);
  }

  return node;
}
