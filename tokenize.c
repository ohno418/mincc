#include "mincc.h"

Token *new_token(TokenKind kind, char *loc, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->loc = loc;
  tok->len = len;
  return tok;
}

Token *tokenize(char *p) {
  return new_token(TK_NUM, p, strlen(p));
}
