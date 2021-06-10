#include "mincc.h"

int get_puct_len(char *p) {
  if (strncmp(p, "<=", 2) == 0 || strncmp(p, ">=", 2) == 0 ||
      strncmp(p, "+=", 2) == 0 || strncmp(p, "-=", 2) == 0 ||
      strncmp(p, "++", 2) == 0 || strncmp(p, "--", 2) == 0) {
    return 2;
  }

  return 1;
}

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
        *p == '(' || *p == ')' || *p == '{' || *p == '}' ||
        *p == ',' || *p == '&' || *p == ':') {
      int len = get_puct_len(p);
      Token *tok = calloc(1, sizeof(Token));
      tok->kind = TK_PUCT;
      tok->loc = p;
      tok->len = len;
      cur->next = tok;
      cur = cur->next;
      p += len;
      continue;
    }

    // identifier
    if ('a' <= *p && *p <= 'z') {
      char *start = p;
      for (
          ;
          ('a' <= *p && *p <= 'z') ||
            ('0' <= *p && *p <= '9') ||
            *p == '_';
          p++
      );

      Token *tok = calloc(1, sizeof(Token));
      tok->kind = TK_IDENT;
      tok->loc = start;
      tok->len = p - start;
      cur->next = tok;
      cur = cur->next;
      continue;
    }

    fprintf(stderr, "Unknown token: %s\n", p);
    exit(1);
  }

  Token *eof = calloc(1, sizeof(Token));
  eof->kind = TK_EOF;
  eof->loc = p;
  eof->len = 0;
  cur->next = eof;

  return head.next;
}
