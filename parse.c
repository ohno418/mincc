#include "mincc.h"

_Bool equal(Token *tok, char *str) {
  return tok->len == strlen(str) &&
    strncmp(tok->loc, str, tok->len) == 0;
}

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_unary(NodeKind kind, Node *lhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  return node;
}

Node *stmt(Token **rest, Token *tok);
Node *expr_stmt(Token **rest, Token *tok);
Node *expr(Token **rest, Token *tok);
Node *assign(Token **rest, Token *tok);
Node *equality(Token **rest, Token *tok);
Node *add(Token **rest, Token *tok);
Node *mul(Token **rest, Token *tok);
Node *primary(Token **rest, Token *tok);

// stmt = expr_stmt
Node *stmt(Token **rest, Token *tok) {
  return expr_stmt(rest, tok);
}

// expr_stmt = expr ";"
Node *expr_stmt(Token **rest, Token *tok) {
  Node *node = expr(&tok, tok);

  if (!equal(tok, ";"))
    error("expected \";\"");

  node = new_unary(ND_EXPR_STMT, node);
  *rest = tok->next;
  return node;
}

// expr = assign
Node *expr(Token **rest, Token *tok) {
  return assign(rest, tok);
}

// assign = equality ("=" equality)?
Node *assign(Token **rest, Token *tok) {
  Node *node = equality(&tok, tok);

  if (equal(tok, "=")) {
    Node *binary = new_node(ND_ASSIGN);
    binary->lhs = node;
    binary->rhs = equality(&tok, tok->next);
    node = binary;
  }

  *rest = tok;
  return node;
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

// mul = primary ("*" primary | "/" primary)*
Node *mul(Token **rest, Token *tok) {
  Node *node = primary(&tok, tok);

  for (;;) {
    if (equal(tok, "*")) {
      Node *binary = new_node(ND_MUL);
      binary->lhs = node;
      binary->rhs = primary(&tok, tok->next);
      node = binary;
      continue;
    }

    if (equal(tok, "/")) {
      Node *binary = new_node(ND_DIV);
      binary->lhs = node;
      binary->rhs = primary(&tok, tok->next);
      node = binary;
      continue;
    }

    break;
  }

  *rest = tok;
  return node;
}

// primary = num | ident
Node *primary(Token **rest, Token *tok) {
  if (tok->kind == TK_NUM) {
    Node *node = new_node(ND_NUM);
    node->val = atoi(tok->loc);
    *rest = tok->next;
    return node;
  }

  if (tok->kind == TK_IDENT) {
    Node *node = new_node(ND_VAR);
    node->name = *tok->loc;
    *rest = tok->next;
    return node;
  }

  error("unknown primary");
}

// program = stmt*
Node *parse(Token *tok) {
  Node head;
  Node *cur = &head;

  for (; tok->kind != TK_EOF;)
    cur = cur->next = stmt(&tok, tok);

  return head.next;
}
