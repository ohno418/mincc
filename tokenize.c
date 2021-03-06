#include "mincc.h"

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
      p++;
      continue;
    }

    if (isdigit(*p)) {
      char *start = p;
      for (; isdigit(*p); p++);
      cur = cur->next = new_token(TK_NUM, start, p - start);
      continue;
    }

    if (ispunct(*p)) {
      cur = cur->next = new_token(TK_PUNCT, p, 1);
      p++;
      continue;
    }

    break;
  }

  cur->next = new_token(TK_EOF, p, 0);
  return head.next;
}
