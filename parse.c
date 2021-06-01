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

// mul = num ("*" | "/" num)*
Node *mul(Token *tok, Token **rest) {
  Node *node = new_num_node(tok, &tok);

  for (;;) {
    if (equal(tok, "*")) {
      Node *rhs = new_num_node(tok->next, &tok);
      node = new_binary_node(ND_MUL, node, rhs);
      continue;
    }

    if (equal(tok, "/")) {
      Node *rhs = new_num_node(tok->next, &tok);
      node = new_binary_node(ND_DIV, node, rhs);
      continue;
    }

    break;
  }

  *rest = tok;
  return node;
}

// add = mul ("+" | "-" mul)*
Node *add(Token *tok, Token **rest) {
  Node *node = mul(tok, &tok);

  for (;;) {
    if (equal(tok, "+")) {
      Node *rhs = mul(tok->next, &tok);
      node = new_binary_node(ND_ADD, node, rhs);
      continue;
    }

    if (equal(tok, "-")) {
      Node *rhs = mul(tok->next, &tok);
      node = new_binary_node(ND_SUB, node, rhs);
      continue;
    }

    break;
  }

  *rest = tok;
  return node;
}

// expr = add
Node *expr(Token *tok, Token **rest) {
  return add(tok, rest);
}

// expr_stmt = expr ";"
Node *expr_stmt(Token *tok, Token **rest) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_EXPR_STMT;
  node->lhs = expr(tok, &tok);

  if (!equal(tok, ";")) {
    fprintf(stderr, "\";\" expected");
    exit(1);
  }
  *rest = tok->next;
  return node;
}

// program = expr_stmt*
Node *parse(Token *tok) {
  Node head;
  Node *cur = &head;
  for (; tok->kind != TK_EOF;) {
    cur->next = expr_stmt(tok, &tok);
    cur = cur->next;
  }
  return head.next;
}
