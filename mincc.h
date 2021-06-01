#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* tokenize.c */
typedef enum {
  TK_NUM,   // nunber
  TK_PUCT,  // puctuator
  TK_IDENT, // identifier
  TK_EOF,   // end of file
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
typedef struct Var Var;
struct Var {
  Var *next;
  char *name;
  int offset;
};

typedef enum {
  ND_EXPR_STMT, // expression statement
                // (lhs has its expression.)
  ND_NUM,       // number
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_ASSIGN,    // =
  ND_VAR,       // variable
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *next;

  Node *lhs;
  Node *rhs;

  int num;  // ND_NUM
  Var *var; // ND_VAR
};

typedef struct Function {
  char *name;
  Var *lvars;
  Node *body;
} Function;

Function *parse(Token *tok);

/* codegen.c */
void codegen(Function *fn);
