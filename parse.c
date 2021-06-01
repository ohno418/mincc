#include "mincc.h"

_Bool equal(Token *tok, char *str) {
  return
    tok->len == strlen(str) &&
    strncmp(tok->loc, str, tok->len) == 0;
}

void consume(Token *tok, Token **rest, char *str) {
  if (!equal(tok, str)) {
    fprintf(stderr, "expected \"%s\"", str);
    exit(1);
  }

  *rest = tok->next;
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

// primary = num | ident
Node *primary(Token *tok, Token **rest) {
  if (tok->kind == TK_NUM)
    return new_num_node(tok, rest);

  if (tok->kind == TK_IDENT) {
    Var *var = calloc(1, sizeof(Var));
    var->name = *tok->loc;
    var->offset = (var->name - 'a' + 1) * 8;

    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_VAR;
    node->var = var;
    *rest = tok->next;
    return node;
  }

  fprintf(stderr, "unknown primary: %s", tok->loc);
  exit(1);
}

// mul = primary ("*" | "/" primary)*
Node *mul(Token *tok, Token **rest) {
  Node *node = primary(tok, &tok);

  for (;;) {
    if (equal(tok, "*")) {
      Node *rhs = primary(tok->next, &tok);
      node = new_binary_node(ND_MUL, node, rhs);
      continue;
    }

    if (equal(tok, "/")) {
      Node *rhs = primary(tok->next, &tok);
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

// assign = add ("=" assign)?
Node *assign(Token *tok, Token **rest) {
  Node *node = add(tok, &tok);

  if (equal(tok, "=")) {
    if (node->kind != ND_VAR) {
      fprintf(stderr, "assign to a non-variable");
      exit(1);
    }
    tok = tok->next;

    Node *rhs = assign(tok, &tok);
    node = new_binary_node(ND_ASSIGN, node, rhs);
    *rest = tok;
  }

  *rest = tok;
  return node;
}

// expr = assign
Node *expr(Token *tok, Token **rest) {
  return assign(tok, rest);
}

// expr_stmt = expr ";"
Node *expr_stmt(Token *tok, Token **rest) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_EXPR_STMT;
  node->lhs = expr(tok, &tok);

  if (!equal(tok, ";")) {
    fprintf(stderr, "\";\" expected %s", tok->loc);
    exit(1);
  }
  *rest = tok->next;
  return node;
}

// function = "main" "(" ")" "{" expr_stmt* "}"
Node *function(Token *tok, Token **rest) {
  consume(tok, &tok, "main");
  consume(tok, &tok, "(");
  consume(tok, &tok, ")");
  consume(tok, &tok, "{");

  Node head;
  Node *cur = &head;
  for (; !equal(tok, "}");) {
    cur->next = expr_stmt(tok, &tok);
    cur = cur->next;
  }
  *rest = tok->next;
  return head.next;
}

// program = function
Node *parse(Token *tok) {
  Node *node = function(tok, &tok);
  if (tok->kind != TK_EOF) {
    fprintf(stderr, "extra token");
    exit(1);
  }
  return node;
}
