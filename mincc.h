#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* tokenize.c */
typedef enum {
  TK_NUM,
} TokenKind;

typedef struct Token Token;
struct Token {
  Token *next;
  TokenKind kind;
  char *loc;
  int len;
};

Token *tokenize(char *p);


/* parse.c */
typedef enum {
  ND_NUM,
} NodeKind;

typedef struct Node Node;
struct Node {
  Node *next;
  NodeKind kind;

  // ND_NUM
  int val;
};

Node *parse(Token *tok);


/* codegen.c */
void codegen(Node *node);
