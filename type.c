#include "mincc.h"

Type *ty_int() {
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = TY_INT;
  return ty;
}

Type *ty_ptr(Type *base) {
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = TY_PTR;
  ty->base = base;
  return ty;
}

void add_type(Node *node) {
  if (!node)
    return;

  add_type(node->lhs);
  add_type(node->rhs);

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
    // TODO
    node->ty = ty_int();
    return;
  }

  if (node->kind == ND_FUNCALL) {
    // TODO
    node->ty = ty_int();
    return;
  }

  if (node->kind == ND_ADDR) {
    add_type(node->lhs);
    node->ty = ty_ptr(node->lhs->ty);
    return;
  }

  fprintf(stderr, "@ %d\n", node->kind);
  error("unknown type");
}
