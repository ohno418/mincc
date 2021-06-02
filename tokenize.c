#include "mincc.h"

Token *tokenize(char *input) {
  Token head;
  Token *cur = &head;

  char *p = input;
  for (; *p;) {
    // Skip spaces
    if (isspace(*p)) {
      p++;
      continue;
    }

    // number
    if (isdigit(*p)) {
      char *start = p;
      for (;; p++)
        if (!isdigit(*p))
          break;

      Token *tok = calloc(1, sizeof(Token));
      tok->kind = TK_NUM;
      tok->loc = start;
      tok->len = p - start;
      cur->next = tok;
      cur = cur->next;
      continue;
    }

    // puctuator
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' ||
        *p == '=' || *p == ';' || *p == '>' || *p == '<' ||
        *p == '(' || *p == ')' || *p == '{' || *p == '}') {
      Token *tok = calloc(1, sizeof(Token));
      tok->kind = TK_PUCT;
      tok->loc = p;
      tok->len = 1;
      cur->next = tok;
      cur = cur->next;
      p++;
      continue;
    }

    // identifier
    if ('a' <= *p && *p <= 'z') {
      char *start = p;
      for (; ('a' <= *p && *p <= 'z') || *p == '_'; p++);

      Token *tok = calloc(1, sizeof(Token));
      tok->kind = TK_IDENT;
      tok->loc = start;
      tok->len = p - start;
      cur->next = tok;
      cur = cur->next;
      continue;
    }

    fprintf(stderr, "Unknown token: %s", p);
    exit(1);
  }

  Token *eof = calloc(1, sizeof(Token));
  eof->kind = TK_EOF;
  eof->loc = p;
  eof->len = 0;
  cur->next = eof;

  return head.next;
}
