#include "mincc.h"

// local variables
Var *lvars;

bool equal(Token *tok, char *str) {
  return tok->len == strlen(str) &&
    strncmp(tok->loc, str, tok->len) == 0;
}

Var *find_lvar(char *name) {
  for (Var *v = lvars; v; v = v->next)
    if (strcmp(name, v->name) == 0)
      return v;

  return NULL;
}

Var *new_lvar(char *name, Type *ty) {
  Var *var = calloc(1, sizeof(Var));
  var->name = name;
  var->ty = ty;

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

Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

Node *declaration(Token **rest, Token *tok);
Node *stmt(Token **rest, Token *tok);
Node *compound_stmt(Token **rest, Token *tok);
Node *expr_stmt(Token **rest, Token *tok);
Node *expr(Token **rest, Token *tok);
Node *assign(Token **rest, Token *tok);
Node *equality(Token **rest, Token *tok);
Node *relational(Token **rest, Token *tok);
Node *add(Token **rest, Token *tok);
Node *mul(Token **rest, Token *tok);
Node *unary(Token **rest, Token *tok);
Node *primary(Token **rest, Token *tok);

// declspec = "int"
Type *declspec(Token **rest, Token *tok) {
  if (equal(tok, "int")) {
    *rest = tok->next;
    return ty_int();
  }

  error("unknown declspec");
}

// type_suffix = "[" num "]"
//             | ε
Type *type_suffix(Token **rest, Token *tok, Type *ty) {
  if (equal(tok, "[")) {
    tok = tok->next;

    if (tok->kind != TK_NUM)
      error("expected a number");
    ty = ty_array(ty, atoi(strndup(tok->loc, tok->len)));
    tok = tok->next;

    if (!equal(tok, "]"))
      error("expected \"]\"");

    *rest = tok->next;
    return ty;
  }

  *rest = tok;
  return ty;
}

// declarator = "*"* ident type_suffix
Type *declarator(Token **rest, Token *tok, Type *ty) {
  for (; equal(tok, "*"); tok = tok->next)
    ty = ty_ptr(ty);

  if (tok->kind != TK_IDENT)
    error("expected identifier");
  char *name = strndup(tok->loc, tok->len);

  ty = type_suffix(rest, tok->next, ty);
  ty->name = name;
  return ty;
}

// declaration = declspec declarator ("=" expr)? ";"
Node *declaration(Token **rest, Token *tok) {
  Type *base_ty = declspec(&tok, tok);
  Type *ty = declarator(&tok, tok, base_ty);

  Var *var = new_lvar(ty->name, ty);
  Node *node = new_node(ND_VAR);
  node->var = var;

  if (equal(tok, "="))
    node = new_binary(ND_ASSIGN, node, expr(&tok, tok->next));

  if (!equal(tok, ";"))
    error("expected \";\"");
  *rest = tok->next;

  Node *block = new_node(ND_BLOCK);
  block->body = new_unary(ND_EXPR_STMT, node);
  return block;
}

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

bool is_typename(Token *tok) {
  return equal(tok, "int");
}

// compound_stmt = (declaration | stmt)* "}"
Node *compound_stmt(Token **rest, Token *tok) {
  Node head;
  Node *cur = &head;
  for (; !equal(tok, "}");) {
    if (is_typename(tok))
      cur = cur->next = declaration(&tok, tok);
    else
      cur = cur->next = stmt(&tok, tok);
    add_type(cur);
  }

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

Node *new_add(Token **rest, Token *tok, Node *lhs) {
  add_type(lhs);

  if (lhs->ty->kind == TY_INT)
    return new_binary(ND_ADD, lhs, mul(rest, tok->next));

  if (lhs->ty->base)
    return new_binary(
      ND_ADD,
      lhs,
      new_binary(
        ND_MUL,
        mul(rest, tok->next),
        new_num(8)
      )
    );

  error("invalid operand");
}

Node *new_sub(Token **rest, Token *tok, Node *lhs) {
  add_type(lhs);

  if (lhs->ty->kind == TY_INT)
    return new_binary(ND_SUB, lhs, mul(rest, tok->next));

  if (lhs->ty->base)
    return new_binary(
      ND_SUB,
      lhs,
      new_binary(
        ND_MUL,
        mul(rest, tok->next),
        new_num(8)
      )
    );

  error("invalid operand");
}

// add = mul ("+" mul | "-" mul)*
Node *add(Token **rest, Token *tok) {
  Node *node = mul(&tok, tok);

  for (;;) {
    if (equal(tok, "+")) {
      node = new_add(&tok, tok, node);
      continue;
    }

    if (equal(tok, "-")) {
      node = new_sub(&tok, tok, node);
      continue;
    }

    break;
  }

  *rest = tok;
  return node;
}

// mul = unary ("*" unary | "/" unary)*
Node *mul(Token **rest, Token *tok) {
  Node *node = unary(&tok, tok);

  for (;;) {
    if (equal(tok, "*")) {
      node = new_binary(ND_MUL, node, unary(&tok, tok->next));
      continue;
    }

    if (equal(tok, "/")) {
      node = new_binary(ND_DIV, node, unary(&tok, tok->next));
      continue;
    }

    break;
  }

  *rest = tok;
  return node;
}

// unary = ("&" | "*") unary
//       | primary
Node *unary(Token **rest, Token *tok) {
  if (equal(tok, "&"))
    return new_unary(ND_ADDR, unary(rest, tok->next));

  if (equal(tok, "*"))
    return new_unary(ND_DEREF, unary(rest, tok->next));

  return primary(rest, tok);
}

// primary = num
//         | ident ("(" args ")")?
//         | "(" expr ")"
//         | "sizeof" unary
// args = expr ("," expr)*
Node *primary(Token **rest, Token *tok) {
  if (tok->kind == TK_NUM) {
    Node *node = new_num(atoi(tok->loc));
    *rest = tok->next;
    return node;
  }

  if (tok->kind == TK_IDENT) {
    // function call
    if (equal(tok->next, "(")) {
      Node *node = new_node(ND_FUNCALL);
      node->funcname = strndup(tok->loc, tok->len);

      tok = tok->next->next;

      Node head;
      Node *cur = &head;
      for (int i = 0; !equal(tok, ")"); i = i + 1) {
        if (i != 0) {
          if (!equal(tok, ","))
            error("expected \",\"");
          tok = tok->next;
        }

        cur = cur->next = expr(&tok, tok);
      }
      node->args = head.next;

      *rest = tok->next;
      return node;
    }

    // variable
    Var *var = find_lvar(strndup(tok->loc, tok->len));
    if (!var)
      error("an unknown variable");

    Node *node = new_node(ND_VAR);
    node->var = var;
    *rest = tok->next;
    return node;
  }

  if (equal(tok, "(")) {
    Node *node = expr(&tok, tok->next);
    if (!equal(tok, ")"))
      error("expected \")\"");
    *rest = tok->next;
    return node;
  }

  if (equal(tok, "sizeof")) {
    Node *node = unary(rest, tok->next);
    add_type(node);
    return new_num(node->ty->size);
  }

  error("unknown primary");
}

// function = declspec declarator "(" func_params? ")" "{" compound_stmt
// func_params = declspec declarator ("," declspec declarator)*
Function *function(Token **rest, Token *tok) {
  lvars = NULL;

  Function *fn = calloc(1, sizeof(Function));

  Type *base_ty = declspec(&tok, tok);
  Type *ty = declarator(&tok, tok, base_ty);
  fn->name = ty->name;

  if (!equal(tok, "("))
    error("expected \"(\"");
  tok = tok->next;

  // params
  for (int i = 0; !equal(tok, ")"); i = i + 1) {
    if (i != 0) {
      if (!equal(tok, ","))
        error("expected \",\"");
      tok = tok->next;
    }

    Type *ty = declspec(&tok, tok);
    ty = declarator(&tok, tok, ty);
    new_lvar(ty->name, ty);
  }
  fn->params = lvars;

  if (!equal(tok->next, "{"))
    error("expected \"{\"");
  tok = tok->next->next;

  fn->body = compound_stmt(rest, tok);
  fn->lvars = lvars;

  // Assign offsets to local variables.
  int offset = 0;
  for (Var *v = lvars; v; v = v->next) {
    offset = offset + v->ty->size;
    v->offset = offset;
  }

  return fn;
}

Function *parse(Token *tok) {
  Function head;
  Function *cur = &head;
  for (; tok->kind != TK_EOF;)
    cur = cur->next = function(&tok, tok);
  return head.next;
}
