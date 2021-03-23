#include "mincc.h"

Type *ty_int() {
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = TY_INT;
  ty->size = 8;
  return ty;
}

Type *ty_ptr(Type *base) {
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = TY_PTR;
  ty->size = 8;
  ty->base = base;
  return ty;
}

Type *ty_array(Type *base, int len) {
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = TY_ARRAY;
  ty->base = base;
  ty->array_len = len;
  ty->size = base->size * len;
  return ty;
}

void add_type(Node *node) {
  if (!node || node->ty)
    return;

  add_type(node->lhs);
  add_type(node->rhs);
  add_type(node->cond);
  add_type(node->then);
  add_type(node->els);
  add_type(node->init);
  add_type(node->inc);

  for (Node *n = node->body; n; n = n->next)
    add_type(n);

  if (node->kind == ND_NUM) {
    node->ty = ty_int();
    return;
  }

  if (node->kind == ND_ADD) {
    node->ty = node->lhs->ty;
    return;
  }
  if (node->kind == ND_SUB) {
    node->ty = node->lhs->ty;
    return;
  }
  if (node->kind == ND_MUL) {
    node->ty = node->lhs->ty;
    return;
  }
  if (node->kind == ND_DIV) {
    node->ty = node->lhs->ty;
    return;
  }

  if (node->kind == ND_VAR) {
    node->ty = node->var->ty;
    return;
  }

  if (node->kind == ND_EQ) {
    node->ty = ty_int();
    return;
  }

  if (node->kind == ND_NEQ) {
    node->ty = ty_int();
    return;
  }

  if (node->kind == ND_LT) {
    node->ty = ty_int();
    return;
  }

  if (node->kind == ND_RETURN) {
    node->ty = node->lhs->ty;
    return;
  }

  if (node->kind == ND_FUNCALL) {
    node->ty = ty_int();
    return;
  }

  if (node->kind == ND_ASSIGN) {
    node->ty = node->lhs->ty;
    return;
  }

  if (node->kind == ND_ADDR) {
    node->ty = ty_ptr(node->lhs->ty);
    return;
  }

  if (node->kind == ND_DEREF) {
    if (!node->lhs->ty->base)
      error("invalid pointer dereference");

    node->ty = node->lhs->ty->base;
    return;
  }

  if (node->kind == ND_EXPR_STMT)
    return;

  if (node->kind == ND_BLOCK)
    return;

  if (node->kind == ND_IF)
    return;

  if (node->kind == ND_FOR)
    return;

  fprintf(stderr, "@ %d\n", node->kind);
  error("unknown type");
}
