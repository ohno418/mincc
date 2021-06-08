#include "mincc.h"

static Function *current_fn;
// local variables
Var *locals;

_Bool equal(Token *tok, char *str) {
  return
    tok->len == strlen(str) &&
    strncmp(tok->loc, str, tok->len) == 0;
}

void consume(Token *tok, Token **rest, char *str) {
  if (!equal(tok, str)) {
    fprintf(stderr, "expected \"%s\": %s\n", str, tok->loc);
    exit(1);
  }

  *rest = tok->next;
}

Node *new_num_node(Token *tok, Token **rest) {
  if (tok->kind != TK_NUM) {
    fprintf(stderr, "expected number token\n");
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
    fprintf(stderr, "expected identifier\n");
    exit(1);
  }

  return strndup(tok->loc, tok->len);
}

Var *find_lvar(char *name) {
  for (Var *v = current_fn->params; v; v = v->next) {
    if (strlen(v->name) == strlen(name) &&
        strncmp(v->name, name, strlen(name)) == 0) {
      return v;
    }
  }
  for (Var *v = locals; v; v = v->next) {
    if (strlen(v->name) == strlen(name) &&
        strncmp(v->name, name, strlen(name)) == 0) {
      return v;
    }
  }
  return NULL;
}

Var *create_lvar(char *name, Type *ty) {
  Var *var = calloc(1, sizeof(Var));
  var->name = name;
  var->ty = ty;

  // Register lvar.
  var->next = locals;
  locals = var;
  return var;
}

Node *expr(Token *tok, Token **rest);

// primary = num
//         | "int" ident
//         | "sizeof" "(" ident ")"
//         | ident "(" (expr ("," expr)*)? ")"
//         | ident ("++" | "--")
//         | ident
Node *primary(Token *tok, Token **rest) {
  if (tok->kind == TK_NUM)
    return new_num_node(tok, rest);

  // new variable
  if (equal(tok, "int") && tok->next->kind == TK_IDENT) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_VAR;
    node->var = create_lvar(get_ident(tok->next), ty_int());;
    *rest = tok->next->next;
    return node;
  }

  // sizeof
  if (equal(tok, "sizeof")) {
    consume(tok->next, &tok, "(");
    Var *var = find_lvar(get_ident(tok));
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->num = var->ty->size;
    consume(tok->next, &tok, ")");
    *rest = tok;
    return node;
  }

  // function call
  if (tok->kind == TK_IDENT && equal(tok->next, "(")) {
    char *fn_name = get_ident(tok);
    tok = tok->next->next;

    Node head = {};
    Node *cur = &head;
    for (int i = 0; !equal(tok, ")"); i++) {
      if (i != 0)
        consume(tok, &tok, ",");

      cur->next = expr(tok, &tok);
      cur = cur->next;
    }
    Node *args = head.next;
    tok = tok->next;

    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_FUNCALL;
    node->fn_name = fn_name;
    node->args = args;
    *rest = tok;
    return node;
  }

  // increment/decrement operation
  if (tok->kind == TK_IDENT && (equal(tok->next, "++") || equal(tok->next, "--"))) {
    /*
     * assign
     *   |-- var
     *   |-- add
     *         |-- var
     *         |-- num = 1
     */
    Node *var_node = calloc(1, sizeof(Node));
    var_node->kind = ND_VAR;
    var_node->var = find_lvar(get_ident(tok));

    Node *num_node = calloc(1, sizeof(Node));
    num_node->kind = ND_NUM;
    num_node->num = 1;

    Node *rhs;
    if (equal(tok->next, "++"))
      rhs = new_binary_node(ND_ADD, var_node, num_node);
    if (equal(tok->next, "--"))
      rhs = new_binary_node(ND_SUB, var_node, num_node);

    Node *node = new_binary_node(
        ND_ASSIGN,
        var_node,
        rhs
    );

    *rest = tok->next->next;
    return node;
  }

  // existing variable
  if (tok->kind == TK_IDENT) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_VAR;
    node->var = find_lvar(get_ident(tok));
    if (!node->var) {
      fprintf(stderr, "unknown variable: %s\n", tok->loc);
      exit(1);
    }
    *rest = tok->next;
    return node;
  }

  fprintf(stderr, "unknown primary: %s\n", tok->loc);
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

// assign    = relational (assign-op assign)?
// assign-op = "=" | "+=" | "-="
Node *assign(Token *tok, Token **rest) {
  Token *start = tok;
  Node *node = relational(tok, &tok);

  if (equal(tok, "=")) {
    if (node->kind != ND_VAR) {
      fprintf(stderr, "assign to a non-variable\n");
      exit(1);
    }
    tok = tok->next;

    Node *rhs = assign(tok, &tok);
    node = new_binary_node(ND_ASSIGN, node, rhs);
    *rest = tok;
  }

  if (equal(tok, "+=")) {
    /*
     * assign
     *   |-- var
     *   |-- add
     *         |-- var
     *         |-- (lhs)
     */
    if (node->kind != ND_VAR) {
      fprintf(stderr, "assign to a non-variable\n");
      exit(1);
    }
    tok = tok->next;

    Node *var_node = calloc(1, sizeof(Node));
    var_node->kind = ND_VAR;
    var_node->var = node->var;

    Node *rhs = calloc(1, sizeof(Node));
    rhs->kind = ND_ADD;
    rhs->lhs = var_node;
    rhs->rhs = assign(tok, &tok);

    node = new_binary_node(ND_ASSIGN, node, rhs);
    *rest = tok;
  }

  if (equal(tok, "-=")) {
    /*
     * assign
     *   |-- var
     *   |-- sub
     *         |-- var
     *         |-- (lhs)
     */
    if (node->kind != ND_VAR) {
      fprintf(stderr, "assign to a non-variable\n");
      exit(1);
    }
    tok = tok->next;

    Var *var = find_lvar(get_ident(start));
    if (!var) {
      fprintf(stderr, "variable not found\n");
      exit(1);
    }
    node->var = var;

    Node *var_node = calloc(1, sizeof(Node));
    var_node->kind = ND_VAR;
    var_node->var = var;

    Node *rhs = calloc(1, sizeof(Node));
    rhs->kind = ND_SUB;
    rhs->lhs = var_node;
    rhs->rhs = assign(tok, &tok);

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
    fprintf(stderr, "\";\" expected %s\n", tok->loc);
    exit(1);
  }
  *rest = tok->next;
  return node;
}

// stmt = "return" expr ";"
//      | "{" stmt* "}"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "for" "(" expr ";" expr ";" expr ")" stmt
//      | expr_stmt
Node *stmt(Token *tok, Token **rest) {
  // return statement
  if (equal(tok, "return")) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr(tok->next, &tok);

    if (!equal(tok, ";")) {
      fprintf(stderr, "\";\" expected %s\n", tok->loc);
      exit(1);
    }
    *rest = tok->next;
    return node;
  }

  // block statement
  if (equal(tok, "{")) {
    tok = tok->next;

    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;

    Node head = {};
    Node *cur = &head;
    for (; !equal(tok, "}");) {
      cur->next = stmt(tok, &tok);
      cur = cur->next;
    }
    node->body = head.next;
    *rest = tok->next;
    return node;
  }

  // if statement
  if (equal(tok, "if")) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_IF;

    consume(tok->next, &tok, "(");
    node->cond = expr(tok, &tok);
    consume(tok, &tok, ")");
    node->then = stmt(tok, &tok);

    if (equal(tok, "else"))
      node->els = stmt(tok->next, &tok);

    *rest = tok;
    return node;
  }

  if (equal(tok, "for")) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;

    consume(tok->next, &tok, "(");
    if (!equal(tok, ";"))
      node->init = expr(tok, &tok);
    consume(tok, &tok, ";");
    if (!equal(tok, ";"))
      node->cond = expr(tok, &tok);
    consume(tok, &tok, ";");
    if (!equal(tok, ")"))
      node->inc = expr(tok, &tok);
    consume(tok, &tok, ")");

    node->then = stmt(tok, &tok);
    *rest = tok;
    return node;
  }

  return expr_stmt(tok, rest);
}

// function = "int" ident "(" params? ")" "{" stmt* "}"
// params   = param ("," param)*
// param    = "int" ident
Function *function(Token *tok, Token **rest) {
  Function *fn = calloc(1, sizeof(Function));
  current_fn = fn;
  locals = NULL;

  // type
  if (!equal(tok, "int")) {
    fprintf(stderr, "\"int\" keyword is required\n");
    exit(1);
  }
  fn->ty = ty_int();
  tok = tok->next;

  // name
  fn->name = get_ident(tok);
  tok = tok->next;

  // params
  consume(tok, &tok, "(");
  if (!equal(tok, ")")) {
    Var head = {};
    Var *cur = &head;
    for (int i = 0; !equal(tok, ")"); i++) {
      if (i != 0)
        consume(tok, &tok, ",");
      consume(tok, &tok, "int");

      Var *var = calloc(1, sizeof(Var));
      var->name = get_ident(tok);
      var->ty = ty_int();
      tok = tok->next;

      cur->next = var;
      cur = cur->next;
    }
    fn->params = head.next;
  }
  tok = tok->next;
  consume(tok, &tok, "{");

  Node head = {};
  Node *cur = &head;
  for (; !equal(tok, "}");) {
    cur->next = stmt(tok, &tok);
    cur = cur->next;
  }
  *rest = tok->next;

  fn->body = head.next;
  fn->locals = locals;

  current_fn = NULL;
  return fn;
}

// program = function
Function *parse(Token *tok) {
  Function head;
  Function *cur = &head;

  for (; tok->kind != TK_EOF;) {
    cur->next = function(tok, &tok);
    cur = cur->next;
  }

  return head.next;
}
