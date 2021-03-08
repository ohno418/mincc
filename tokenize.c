#include "mincc.h"

void error(char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

Token *new_token(TokenKind kind, char *loc, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->loc = loc;
  tok->len = len;
  return tok;
}

Token *tokenize(char *p) {
  Token head = {};
  Token *cur = &head;

  for (;;) {
    if (isspace(*p)) {
      p = p + 1;
      continue;
    }

    if (isdigit(*p)) {
      char *start = p;
      for (; isdigit(*p); p = p + 1);
      cur = cur->next = new_token(TK_NUM, start, p - start);
      continue;
    }

    if (ispunct(*p)) {
      char *start = p;
      p = p + 1;
      for (; ispunct(*p); p = p + 1);
      cur = cur->next = new_token(TK_PUNCT, start, p - start);
      continue;
    }

    break;
  }

  cur->next = new_token(TK_EOF, p, 0);
  return head.next;
}
