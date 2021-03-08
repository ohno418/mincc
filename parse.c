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
Node *equality(Token **rest, Token *tok);
Node *add(Token **rest, Token *tok);
Node *mul(Token **rest, Token *tok);
Node *num(Token **rest, Token *tok);

// expr = equality
Node *expr(Token **rest, Token *tok) {
  return equality(rest, tok);
}

// equality = add ("==" add | "!=" add)?
Node *equality(Token **rest, Token *tok) {
  Node *node = add(&tok, tok);

  if (equal(tok, "==")) {
    Node *binary = new_node(ND_EQ);
    binary->lhs = node;
    binary->rhs = add(&tok, tok->next);
    node = binary;
  }

  if (equal(tok, "!=")) {
    Node *binary = new_node(ND_NEQ);
    binary->lhs = node;
    binary->rhs = add(&tok, tok->next);
    node = binary;
  }

  *rest = tok;
  return node;
}

// add = mul ("+" mul | "-" mul)*
Node *add(Token **rest, Token *tok) {
  Node *node = mul(&tok, tok);

  for (;;) {
    if (equal(tok, "+")) {
      Node *binary = new_node(ND_ADD);
      binary->lhs = node;
      binary->rhs = mul(&tok, tok->next);
      node = binary;
      continue;
    }

    if (equal(tok, "-")) {
      Node *binary = new_node(ND_SUB);
      binary->lhs = node;
      binary->rhs = mul(&tok, tok->next);
      node = binary;
      continue;
    }

    break;
  }

  *rest = tok;
  return node;
}

// mul = num ("*" num | "/" num)*
Node *mul(Token **rest, Token *tok) {
  Node *node = num(&tok, tok);

  for (;;) {
    if (equal(tok, "*")) {
      Node *binary = new_node(ND_MUL);
      binary->lhs = node;
      binary->rhs = num(&tok, tok->next);
      node = binary;
      continue;
    }

    if (equal(tok, "/")) {
      Node *binary = new_node(ND_DIV);
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
  if (tok->kind != TK_NUM)
    error("expected a number");

  Node *node = new_node(ND_NUM);
  node->val = atoi(tok->loc);
  *rest = tok->next;
  return node;
}

Node *parse(Token *tok) {
  Node *node = expr(&tok, tok);

  if (tok->kind != TK_EOF)
    error("extra token");

  return node;
}
