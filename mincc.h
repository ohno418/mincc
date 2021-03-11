#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* tokenize.c */
typedef enum {
  TK_NUM,   // number
  TK_IDENT, // identifier
  TK_PUNCT, // puctuatos
  TK_EOF,   // end-of-file
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
  ND_NUM,       // integer
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_EQ,        // ==
  ND_NEQ,       // !=
  ND_EXPR_STMT, // expression statement
  ND_ASSIGN,    // assignment
  ND_VAR,       // variable
} NodeKind;

typedef struct Node Node;
struct Node {
  Node *next;
  NodeKind kind;

  Node *lhs; // left-hand side
  Node *rhs; // right-hand side

  // ND_NUM
  int val;

  // ND_VAR
  char name;
};

Node *parse(Token *tok);


/* codegen.c */
void codegen(Node *node);
