#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* tokenize.c */
typedef enum {
  TK_NUM,  // nunber
  TK_PUCT, // puctuator
  TK_EOF,  // end of file
} TokenKind;

typedef struct Token Token;
struct Token {
  Token *next;
  TokenKind kind;
  char *loc;
  int len;
};

Token *tokenize(char *input);

/* parse.c */
typedef enum {
  ND_NUM, // number
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;

  int num; // ND_NUM

  Node *lhs;
  Node *rhs;
};

Node *parse(Token *tok);

/* codegen.c */
void codegen(Node *node);
