#ifndef _NODE_H_
#define _NODE_H_

#include <stdlib.h>
#include "y.tab.h"

#define NAME_LENGTH 32
#define STRING_LENGTH 128

// typedef enum { NodeKStmt, NodeKGlobalDecl, NodeKExpr } NodeKind;
// typedef enum { StmtExpr, StmtCompound, StmtSelection, StmtWhile, StmtFor, StmtReturn } StmtKind;
// typedef enum { GlobalDeclVar, GlobalDeclFunc } GlobalDeclKind;
// typedef enum { ExprAssign, ExprSimple } ExprKind;

typedef enum { TermKType, TermKNum, TermKId, TermKOp, TermKBracket } TermKind;

struct Node{
    struct Node* child;//第一个孩子;
    struct Node* next_sib;//兄弟节点
    char name[NAME_LENGTH];//节点名称

    // Non-terminal只需关注其名称或类别
    // 而terminal需要支持所有的原生数据类型
    int is_terminal;
    TermKind termKind;
    union {
        struct {
            union {
                int int_term;
                float real_term;
            };
            int is_int;
        };
        char str_term[STRING_LENGTH];
    };

    int column;//列数;
    int lineno;
};

struct Node* createNode(void);

struct Node* makeNode(const char* name);

void addChild(int n, struct Node* parent, ...);

void printTree(struct Node *root);

#endif