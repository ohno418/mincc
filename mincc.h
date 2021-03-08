#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* tokenize.c */
typedef enum {
  TK_NUM,   // number
  TK_PUNCT, // Puctuatos
  TK_EOF,   // End-of-file
} TokenKind;

typedef struct Token Token;
struct Token {
  Token *next;
  TokenKind kind;
  char *loc;
  int len;
};

void error(char *msg);
Token *tokenize(char *p);


/* parse.c */
typedef enum {
  ND_NUM,
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_EQ,  // ==
  ND_NEQ, // !=
} NodeKind;

typedef struct Node Node;
struct Node {
  Node *next;
  NodeKind kind;

  // ND_NUM
  int val;

  Node *lhs; // left-hand side
  Node *rhs; // right-hand side
};

Node *parse(Token *tok);


/* codegen.c */
void codegen(Node *node);
