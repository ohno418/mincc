#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Type Type;

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
  Type *ty;

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
  ND_LT,        // <
  ND_RETURN,    // "return"
  ND_IF,        // "if"
  ND_FOR,       // "for"
  ND_EXPR_STMT, // expression statement
  ND_BLOCK,     // { ... }
  ND_FUNCALL,   // function call
  ND_ASSIGN,    // assignment
  ND_ADDR,      // &
  ND_DEREF,     // *
  ND_VAR,       // variable
} NodeKind;

typedef struct Node Node;
struct Node {
  Node *next;
  NodeKind kind;
  Type *ty;

  Node *lhs; // left-hand side
  Node *rhs; // right-hand side

  // ND_NUM
  int val;

  // ND_IF:  cond, then, els
  // ND_FOR: init, cond, inc, then
  Node *cond;
  Node *then;
  Node *els;
  Node *init;
  Node *inc;

  // ND_BLOCK
  Node *body;

  // funcation call
  char *funcname;
  Node *args;

  // ND_VAR
  Var *var;
};

typedef struct Function Function;
struct Function {
  Function *next;
  char *name;
  Var *params;
  Node *body;
  Var *lvars;
};

bool equal(Token *tok, char *str);
Function *parse(Token *tok);


/* codegen.c */
void codegen(Function *prog);


/* type.c */
typedef enum {
  TY_INT,
  TY_PTR,
  TY_ARRAY,
} TypeKind;

struct Type {
  TypeKind kind;
  int size;

  // pointer-to or array-of type
  Type *base;

  // declaration
  char *name;

  // array
  int array_len;
};

Type *ty_int();
Type *ty_ptr(Type *base);
Type *ty_array(Type *base, int len);
void add_type(Node *node);
