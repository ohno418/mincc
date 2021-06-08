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
typedef struct Type Type;
typedef struct Var Var;
struct Var {
  Var *next;
  char *name;
  Type *ty;
  int offset;
};

typedef enum {
  ND_NUM,       // number
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_LT,        // <
  ND_LTE,       // <=
  ND_ASSIGN,    // =
  ND_VAR,       // variable
  ND_FUNCALL,   // function call

  // statements:
  //   `lhs` has its expression
  ND_RETURN,    // return statement
  ND_BLOCK,     // block statement
  ND_IF,        // if statement
  ND_FOR,       // for statement
  ND_EXPR_STMT, // expression statement
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *next;

  Node *lhs;
  Node *rhs;

  int num;       // ND_NUM
  Var *var;      // ND_VAR

  // ND_FUNCALL
  char *fn_name;
  Node *args;

  // ND_BLOCK
  Node *body;

  // ND_IF, ND_FOR
  Node *init;
  Node *cond;
  Node *inc;
  Node *then;
  Node *els;
};

typedef struct Function Function;
struct Function {
  char *name;
  // TODO
  // Type *ty;
  Var *params;
  Var *locals;
  Node *body;
  int stack_size;
  Function *next;
};

Function *parse(Token *tok);

/* codegen.c */
void codegen(Function *fn);

/* type.c */
typedef enum {
  TY_INT,
} TypeKind;

struct Type {
  TypeKind kind;
  int size;
};

Type *ty_int();
