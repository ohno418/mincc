#include "mincc.h"

void error(char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

bool is_keyword(Token *tok) {
  return equal(tok, "return") || equal(tok, "if") || equal(tok, "else");
}

void convert_keywords(Token *tok) {
  for (Token *t = tok; t; t = t->next)
    if (t->kind == TK_IDENT && is_keyword(tok))
      t->kind = TK_KEYWORD;
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

  for (; *p;) {
    // Skip spaces
    if (isspace(*p)) {
      p = p + 1;
      continue;
    }

    // number
    if (isdigit(*p)) {
      char *start = p;
      for (; isdigit(*p); p = p + 1);
      cur = cur->next = new_token(TK_NUM, start, p - start);
      continue;
    }

    // puctuator
    if (ispunct(*p)) {
      char *start = p;
      p = p + 1;
      for (; ispunct(*p); p = p + 1);
      cur = cur->next = new_token(TK_PUNCT, start, p - start);
      continue;
    }

    // identifier or keyword
    if (isalpha(*p)) {
      char *start = p;
      for (; isalpha(*p); p = p + 1);
      cur = cur->next = new_token(TK_IDENT, start, p - start);
      continue;
    }

    error("invalid token");
  }

  cur->next = new_token(TK_EOF, p, 0);
  convert_keywords(head.next);
  return head.next;
}
