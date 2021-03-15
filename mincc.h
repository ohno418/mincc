#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* tokenize.c */
typedef enum {
  TK_NUM,     // number
  TK_PUNCT,   // puctuatos
  TK_IDENT,   // identifier
  TK_KEYWORD, // keyword
  TK_EOF,     // end-of-file
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
typedef struct Var Var;
struct Var {
  Var *next;
  char *name;
  int len;

  // offset from RBP
  int offset;
};

typedef enum {
  ND_NUM,       // integer
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_EQ,        // ==
  ND_NEQ,       // !=
  ND_RETURN,    // "return"
  ND_IF,        // "if"
  ND_EXPR_STMT, // expression statement
  ND_BLOCK,     // { ... }
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

  // ND_IF
  Node *cond;
  Node *then;
  Node *els;

  // ND_BLOCK
  Node *body;

  // ND_VAR
  Var *var;
};

typedef struct Function {
  Node *body;
  Var *lvars;
} Function;

bool equal(Token *tok, char *str);
Function *parse(Token *tok);


/* codegen.c */
void codegen(Function *prog);
