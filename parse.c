#include "mincc.h"

// local variables
Var *lvars;

bool equal(Token *tok, char *str) {
  return tok->len == strlen(str) &&
    strncmp(tok->loc, str, tok->len) == 0;
}

Var *find_lvar(char *name, int len) {
  for (Var *v = lvars; v; v = v->next)
    if (len == v->len && strncmp(name, v->name, len) == 0)
      return v;

  return NULL;
}

Var *new_var(char *name, int len) {
  Var *var = calloc(1, sizeof(Var));
  var->name = name;
  var->len = len;

  // List it in lvars.
  var->next = lvars;
  lvars = var;

  return var;
}

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_unary(NodeKind kind, Node *lhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  return node;
}

Node *stmt(Token **rest, Token *tok);
Node *compound_stmt(Token **rest, Token *tok);
Node *expr_stmt(Token **rest, Token *tok);
Node *expr(Token **rest, Token *tok);
Node *assign(Token **rest, Token *tok);
Node *equality(Token **rest, Token *tok);
Node *relational(Token **rest, Token *tok);
Node *add(Token **rest, Token *tok);
Node *mul(Token **rest, Token *tok);
Node *primary(Token **rest, Token *tok);

// stmt = "return" expr ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      | "{" compound_stmt
//      | expr_stmt
Node *stmt(Token **rest, Token *tok) {
  if (equal(tok, "return")) {
    Node *node = expr(&tok, tok->next);
    if (!equal(tok, ";"))
      error("expected \";\"");
    *rest = tok->next;
    return new_unary(ND_RETURN, node);
  }

  if (equal(tok, "if")) {
    Node *node = new_node(ND_IF);
    if (!equal(tok->next, "("))
      error("expected \"(\"");
    node->cond = expr(&tok, tok->next->next);
    if (!equal(tok, ")"))
      error("expected \")\"");
    node->then = stmt(&tok, tok->next);
    if (equal(tok, "else"))
      node->els = stmt(&tok, tok->next);
    *rest = tok;
    return node;
  }

  if (equal(tok, "for")) {
    Node *node = new_node(ND_FOR);
    if (!equal(tok->next, "("))
      error("expected \"(\"");
    tok = tok->next->next;

    if (!equal(tok, ";"))
      node->init = expr(&tok, tok);
    if (!equal(tok, ";"))
      error("expected \";\"");
    tok = tok->next;

    if (!equal(tok, ";"))
      node->cond = expr(&tok, tok);
    if (!equal(tok, ";"))
      error("expected \";\"");
    tok = tok->next;

    if (!equal(tok, ")"))
      node->inc = expr(&tok, tok);
    if (!equal(tok, ")"))
      error("expected \")\"");
    tok = tok->next;

    node->then = stmt(rest, tok);
    return node;
  }

  if (equal(tok, "{"))
    return compound_stmt(rest, tok->next);

  return expr_stmt(rest, tok);
}

// compound_stmt = stmt* "}"
Node *compound_stmt(Token **rest, Token *tok) {
  Node head;
  Node *cur = &head;
  for (; !equal(tok, "}");)
    cur = cur->next = stmt(&tok, tok);

  Node *node = new_node(ND_BLOCK);
  node->body = head.next;
  *rest = tok->next;
  return node;
}

// expr_stmt = expr? ";"
Node *expr_stmt(Token **rest, Token *tok) {
  // null statement
  if (equal(tok, ";")) {
    *rest = tok->next;
    return new_node(ND_BLOCK);
  }

  Node *node = new_unary(ND_EXPR_STMT, expr(&tok, tok));
  if (!equal(tok, ";"))
    error("expected \";\"");
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

  if (equal(tok, "="))
    node = new_binary(ND_ASSIGN, node, equality(&tok, tok->next));

  *rest = tok;
  return node;
}

// equality = relational ("==" relational | "!=" relational)?
Node *equality(Token **rest, Token *tok) {
  Node *node = relational(&tok, tok);

  if (equal(tok, "=="))
    node = new_binary(ND_EQ, node, relational(&tok, tok->next));

  if (equal(tok, "!="))
    node = new_binary(ND_NEQ, node, relational(&tok, tok->next));

  *rest = tok;
  return node;
}

// relational = add ("<" add | ">" add)?
Node *relational(Token **rest, Token *tok) {
  Node *node = add(&tok, tok);

  if (equal(tok, "<"))
    node = new_binary(ND_LT, node, add(&tok, tok->next));

  if (equal(tok, ">"))
    node = new_binary(ND_LT, add(&tok, tok->next), node);

  *rest = tok;
  return node;
}

// add = mul ("+" mul | "-" mul)*
Node *add(Token **rest, Token *tok) {
  Node *node = mul(&tok, tok);

  for (;;) {
    if (equal(tok, "+")) {
      node = new_binary(ND_ADD, node, mul(&tok, tok->next));
      continue;
    }

    if (equal(tok, "-")) {
      node = new_binary(ND_SUB, node, mul(&tok, tok->next));
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
      node = new_binary(ND_MUL, node, primary(&tok, tok->next));
      continue;
    }

    if (equal(tok, "/")) {
      node = new_binary(ND_DIV, node, primary(&tok, tok->next));
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
    // Find or create lvar.
    Var *var = find_lvar(tok->loc, tok->len);
    if (!var)
      var = new_var(tok->loc, tok->len);

    Node *node = new_node(ND_VAR);
    node->var = var;
    *rest = tok->next;
    return node;
  }

  error("unknown primary");
}

// function = func-name "(" ")" "{" compound_stmt
Function *function(Token *tok) {
  Function *func = calloc(1, sizeof(Function));
  func->name = strndup(tok->loc, tok->len);

  if (!equal(tok->next, "("))
    error("expected \"(\"");
  if (!equal(tok->next->next, ")"))
    error("expected \")\"");
  if (!equal(tok->next->next->next, "{"))
    error("expected \"{\"");
  tok = tok->next->next->next->next;

  func->body = compound_stmt(&tok, tok);
  func->lvars = lvars;

  // Assign offsets to local variables.
  int offset = 0;
  for (Var *v = lvars; v; v = v->next) {
    offset = offset + 8;
    v->offset = offset;
  }

  if (tok->kind != TK_EOF)
    error("extra token");
  return func;
}

Function *parse(Token *tok) {
  return function(tok);
}
