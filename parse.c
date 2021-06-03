#include "mincc.h"

Var *lvars;

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

char *get_ident(Token *tok) {
  if (tok->kind != TK_IDENT) {
    fprintf(stderr, "expected identifier");
    exit(1);
  }

  return strndup(tok->loc, tok->len);
}

Var *find_lvar(char *name) {
  for (Var *v = lvars; v; v = v->next) {
    if (strlen(v->name) == strlen(name) &&
        strncmp(v->name, name, strlen(name)) == 0) {
      return v;
    }
  }

  return NULL;
}

Var *create_lvar(char *name) {
  Var *var = calloc(1, sizeof(Var));
  var->name = name;

  // Register lvar.
  var->next = lvars;
  lvars = var;
  return var;
}

// primary = num | ident
Node *primary(Token *tok, Token **rest) {
  if (tok->kind == TK_NUM)
    return new_num_node(tok, rest);

  if (tok->kind == TK_IDENT) {
    // variable
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_VAR;
    node->var = find_lvar(get_ident(tok));
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

// relational = add (">" add | "<" add | "<=" add | ">= add)?
Node *relational(Token *tok, Token **rest) {
  Node *node = add(tok, &tok);

  if (equal(tok, "<")) {
    Node *rhs = add(tok->next, &tok);
    node = new_binary_node(ND_LT, node, rhs);
  }

  if (equal(tok, ">")) {
    Node *lhs = add(tok->next, &tok);
    node = new_binary_node(ND_LT, lhs, node);
  }

  if (equal(tok, "<=")) {
    Node *rhs = add(tok->next, &tok);
    node = new_binary_node(ND_LTE, node, rhs);
  }

  if (equal(tok, ">=")) {
    Node *lhs = add(tok->next, &tok);
    node = new_binary_node(ND_LTE, lhs, node);
  }

  *rest = tok;
  return node;
}

// assign = relational ("=" assign)?
Node *assign(Token *tok, Token **rest) {
  Token *start = tok;
  Node *node = relational(tok, &tok);

  if (equal(tok, "=")) {
    if (node->kind != ND_VAR) {
      fprintf(stderr, "assign to a non-variable");
      exit(1);
    }
    tok = tok->next;

    Var *var = create_lvar(get_ident(start));
    node->var = var;
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

// function = ident "(" ")" "{" expr_stmt* "}"
Function *function(Token *tok, Token **rest) {
  Function *fn = calloc(1, sizeof(Function));
  fn->name = get_ident(tok);
  tok = tok->next;
  lvars = NULL;

  consume(tok, &tok, "(");
  consume(tok, &tok, ")");
  consume(tok, &tok, "{");

  Node head = {};
  Node *cur = &head;
  for (; !equal(tok, "}");) {
    cur->next = expr_stmt(tok, &tok);
    cur = cur->next;
  }
  *rest = tok->next;

  fn->body = head.next;
  fn->lvars = lvars;
  return fn;
}

// program = function
Function *parse(Token *tok) {
  Function *fn = function(tok, &tok);
  if (tok->kind != TK_EOF) {
    fprintf(stderr, "extra token");
    exit(1);
  }
  return fn;
}
