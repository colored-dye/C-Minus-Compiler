/*
 * @Author: colored-dye
 * @Date: 2022-05-08 14:51:44
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-17 15:01:59
 * @FilePath: /C-Minus-Compiler/src/node.cpp
 * @Description:
 *
 * Copyright (c) 2022 by colored-dye, All Rights Reserved.
 */

#include "node.hpp"
#include "ast.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

const char *NodeNames[] = {
    "Program",        // 0  entire program
    "GlobalDeclList", // 1  list of global declarations
    "Decl",           // 2  abstract declarations
    "VarDecl",        // 3  specific variable declaration
    "FunDecl",        // 4  specific function declaration
    "TypeSpec",       // 5  type specification
    "Params",         // 6
    "ParamList",      // 7
    "Param",          // 8
    "CompoundStmt",   // 9
    "LocalDecl",      // 10
    "StmtList",       // 11
    "Stmt",           // 12
    "ExprStmt",       // 13
    "Expr",           // 14
    "SelectionStmt",  // 15
    "WhileStmt",      // 16
    "ForStmt",        // 17
    "For_param1",     // 18
    "For_param2",     // 19
    "For_param3",     // 20
    "ReturnStmt",     // 21
    "Var",            // 22
    "SimpleExpr",     // 23
    "AddExpr",        // 24
    "RelOp",          // 25
    "AddOp",          // 26
    "Term",           // 27
    "Factor",         // 28
    "MulOp",          // 29
    "Call",           // 30
    "Args",           // 31
    "ArgList",        // 32
};

struct Node *createNode(void)
{
  struct Node *ret = (struct Node *)malloc(sizeof(struct Node));
  if (!ret)
  {
    fprintf(stderr, "createNode(): Out of memory\n");
    return NULL;
  }
  ret->child = ret->next_sib = NULL;
  ret->is_terminal = 0;
  return ret;
}

void printTree(struct Node *root)
{
  static int indent = 0;
  if (!root)
  {
    return;
  }
  for (int i = 0; i < indent << 1; i++)
    putchar(' ');

  if (root->is_terminal)
  {
    switch (root->termKind)
    {
    case TermKType:
      printf("  [Type: %s]\n", root->str_term);
      break;
    case TermKNum:
      printf("NUM");
      if (root->is_int)
        printf("  [Num: %d]\n", root->int_term);
      else
        printf("  [Num: %.3f]\n", root->real_term);
      break;
    case TermKId:
      printf("ID");
      printf("  [Id: %s]\n", root->str_term);
      break;
    case TermKOp:
      printf("  [Op: %d]\n", root->int_term);
      break;
    case TermKBracket:
      printf("  [Bracket: %s]\n", root->str_term);
      break;
    }
  }
  else
  {
    printf("%s (%d)\n", root->name, root->lineno);
  }
  indent++;
  printTree(root->child);
  indent--;
  printTree(root->next_sib);
}

void addChild(int n, struct Node *parent, ...)
{
  va_list arg_list;
  va_start(arg_list, parent);
  struct Node *p, *child;
  int i = 0;
  if (!parent->child)
  {
    child = va_arg(arg_list, struct Node *);
    parent->child = child;
    p = child;
    i = 1;
  }
  else
  {
    p = parent->child;
    while (p->next_sib)
    {
      p = p->next_sib;
    }
  }

  for (; i < n; i++)
  {
    child = va_arg(arg_list, struct Node *);
    p->next_sib = child;
    p = child;
  }
  va_end(arg_list);
}

struct Node *makeNode(const char *name)
{
  struct Node *ret = createNode();
  strncpy(ret->name, name, NAME_LENGTH);
  return ret;
}

ASTNode *ParserTreeToAST(struct Node *parserNode)
{
  if (parserNode == NULL)
    return NULL;

  ASTNode *curASTNode = NULL;
  if (strncasecmp(parserNode->name, "Program", 7) == 0)
  {
    /*
    Program -> GlobalDeclList --> Decl - VarDecl
                                       - FunDecl
     */
    curASTNode = new ASTProgram();
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
    struct Node *decl = parserNode->child->child; // Skip GlobalDeclList that is not an ASTNode.
    while (decl != NULL)
    {
      if (!strcasecmp(decl->child->name, NodeNames[3])) // VarDecl 3
      {
        ASTNode *varDecl = ParserTreeToAST(decl->child);
        ((ASTProgram *)curASTNode)->AddDecl(varDecl);
        ((ASTVarDecl *)varDecl)->isGlobal = true;
      }
      else if (!strcasecmp(decl->child->name, NodeNames[4])) // FunDecl 4
      {
        ASTNode *funDecl = ParserTreeToAST(decl->child);
        ((ASTProgram *)curASTNode)->AddDecl(funDecl);
      }
      decl = decl->next_sib;
    }
  }
  else if (!strncasecmp(parserNode->name, "VarDecl", 7))
  {
    /*
    VarDecl -> TypeSpec -> Type
            -> ID
            -- NUM
     */
    struct Node *pType = parserNode->child->child;
    struct Node *pId = parserNode->child->next_sib;
    struct Node *pNum = pId->next_sib;
    ASTTypeSpec typeSpec = (!strcasecmp(pType->str_term, "INT")) ? ASTINT : ASTREAL;
    string id = string(pId->str_term);

    if (pNum == NULL)
      curASTNode = new ASTVarDecl(typeSpec, id);
    else
    {
      int arrayLength = pNum->int_term;
      curASTNode = new ASTVarDecl(typeSpec, id, arrayLength);
    }
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
  }
  else if (strncasecmp(parserNode->name, "FunDecl", 7) == 0)
  {
    /*
    FunDecl -> TypeSpec -> Type
            -> ID
            -> Params - ParamList --> Param
                      - Void
            -> CompoundStmt
     */
    struct Node *pType = parserNode->child->child;
    struct Node *pId = parserNode->child->next_sib;
    struct Node *pParams = pId->next_sib;
    struct Node *pCompoundStmt = pParams->next_sib;

    ASTTypeSpec typeSpec;
    if (!strcasecmp(pType->str_term, "INT"))
      typeSpec = ASTINT;
    else if (!strcasecmp(pType->str_term, "REAL"))
      typeSpec = ASTREAL;
    else if (!strcasecmp(pType->str_term, "VOID"))
      typeSpec = ASTVOID;
    string id = string(pId->str_term);
    curASTNode = new ASTFunDecl(typeSpec, id);
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);

    if (strcasecmp(pParams->child->str_term, "VOID"))
    {
      struct Node *pParam = pParams->child->child; // Skip ParamList that is not an ASTNode.
      while (pParam != NULL)
      {
        ASTNode *param = ParserTreeToAST(pParam);
        ((ASTFunDecl *)curASTNode)->AddParam((ASTParam *)param);
        pParam = pParam->next_sib;
      }
    }

    ASTCompoundStmt *compoundStmt = (ASTCompoundStmt *)ParserTreeToAST(pCompoundStmt);
    ((ASTFunDecl *)curASTNode)->AddCompoundStmt(compoundStmt);
  }
  else if (strncasecmp(parserNode->name, "Param", 5) == 0)
  {
    /*
    Param -> TypeSpec -> Type
          -> ID
          -- []
     */
    struct Node *pType = parserNode->child->child;
    struct Node *pId = parserNode->child->next_sib;
    struct Node *pBracket = pId->next_sib;

    ASTTypeSpec typeSpec;
    if (!strcasecmp(pType->str_term, "INT"))
      typeSpec = ASTINT;
    else if (!strcasecmp(pType->str_term, "REAL"))
      typeSpec = ASTREAL;
    else if (!strcasecmp(pType->str_term, "VOID"))
      typeSpec = ASTVOID;
    string id = string(pId->str_term);
    if (pBracket == NULL)
      curASTNode = new ASTParam(typeSpec, id);
    else
      curASTNode = new ASTParam(typeSpec, id, true);

    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
  }
  else if (strncasecmp(parserNode->name, "CompoundStmt", 12) == 0)
  {
    /*
    CompoundStmt -> LocalDecl --> VarDecl
                 -> StmtList --> Stmt - CompoundStmt
                                      - ExprStmt - Expr;
                                                 - ;
                                      - SelectStmt
                                      - WhileStmt
                                      - ForStmt
                                      - ReturnStmt
     */

    struct Node *pVarDecl = parserNode->child->child;
    curASTNode = new ASTCompoundStmt();
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);

    while (pVarDecl != NULL)
    {
      ASTNode *varDecl = ParserTreeToAST(pVarDecl);
      ((ASTCompoundStmt *)curASTNode)->AddDecl((ASTVarDecl *)varDecl);
      ((ASTVarDecl *)varDecl)->isGlobal = false;
      pVarDecl = pVarDecl->next_sib;
    }

    struct Node *pStmtList = parserNode->child->next_sib;
    if (pStmtList != NULL)
    {
      struct Node *pStmt = pStmtList->child;
      while (pStmt != NULL)
      {
        ASTNode *stmt;
        if (strncasecmp(pStmt->child->name, "ExprStmt", 8) == 0)
        {
          stmt = ParserTreeToAST(pStmt->child->child);
        }
        else
        {
          // stmt = ParserTreeToAST(pStmt->child);
        }

        ((ASTCompoundStmt *)curASTNode)->AddStmt(stmt);
        pStmt = pStmt->next_sib;
      }
    }
  }
  else if (strncasecmp(parserNode->name, "Expr", 4) == 0)
  {
    /*
    Expr - SimpleExpr
         - Assign -> Var
                  -> Expr
     */
    if (strncasecmp(parserNode->child->name, "SimpleExpr", 10) == 0)
    {
      struct Node *pSimpleExpr = parserNode->child;
      ASTNode *simpleExpr = ParserTreeToAST(pSimpleExpr);
      curASTNode = new ASTExpr((ASTSimpleExpr *)simpleExpr);
    }
    else if (strncasecmp(parserNode->child->name, "Assign", 6) == 0)
    {
      struct Node *pVar = parserNode->child->child;
      ASTNode *var = ParserTreeToAST(pVar);
      struct Node *pExpr = pVar->next_sib;
      ASTNode *expr = ParserTreeToAST(pExpr);
      curASTNode = new ASTExpr((ASTVar *)var, (ASTExpr *)expr);
    }

    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
  }
  else if (strncasecmp(parserNode->name, "SelectionStmt", 13) == 0)
  {
    /*
    SelectStmt -> Expr
               -> Stmt - CompoundStmt
                       - Expr
                       - SelectStmt
                       - WhileStmt
                       - ForStmt
                       - ReturnStmt
               -- Stmt
     */
    struct Node *pExpr = parserNode->child;
    ASTNode *expr = ParserTreeToAST(pExpr);
    struct Node *pTrueStmt = pExpr->next_sib;
    ASTNode *trueStmt = ParserTreeToAST(pTrueStmt->child);
    struct Node *pFalseStmt = pTrueStmt->next_sib;
    if (pFalseStmt == NULL)
      curASTNode = new ASTSelectStmt((ASTExpr *)expr, (ASTNode *)trueStmt);
    else
    {
      ASTNode *falseStmt = ParserTreeToAST(pFalseStmt->child);
      curASTNode = new ASTSelectStmt((ASTExpr *)expr, (ASTNode *)trueStmt, (ASTNode *)falseStmt);
    }
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
  }
  else if (strncasecmp(parserNode->name, "WhileStmt", 9) == 0)
  {
    /*
    WhileStmt -> Expr
              -> Stmt - CompoundStmt
                      - Expr
                      - SelectStmt
                      - WhileStmt
                      - ForStmt
                      - ReturnStmt
     */
    struct Node *pExpr = parserNode->child;
    ASTNode *expr = ParserTreeToAST(pExpr);
    struct Node *pStmt = pExpr->next_sib;
    ASTNode *stmt = ParserTreeToAST(pStmt->child);
    curASTNode = new ASTWhileStmt((ASTExpr *)expr, (ASTNode *)stmt);
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
  }
  else if (strncasecmp(parserNode->name, "ForStmt", 7) == 0)
  {
    /*
    ForStmt -> For_param1 -> Var
                          -> Expr
            -> For_param2 -> Expr
            -> For_param3 -> Var
                          -> Expr
            -> Stmt - CompoundStmt
                    - Expr
                    - SelectStmt
                    - WhileStmt
                    - ForStmt
                    - ReturnStmt
     */
  }
  else if (strncasecmp(parserNode->name, "ReturnStmt", 10) == 0)
  {
    /*
    ReturnStmt -- Expr
     */
    curASTNode = new ASTReturnStmt();
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
    struct Node *pExpr = parserNode->child;
    if (pExpr != NULL)
    {
      ASTNode *expr = ParserTreeToAST(pExpr);
      ((ASTReturnStmt *)curASTNode)->AddExpr((ASTExpr *)expr);
    }
  }
  else if (strncasecmp(parserNode->name, "Var", 3) == 0)
  {
    /*
    Var -> ID
        -- Expr
     */
    struct Node *pId = parserNode->child;
    struct Node *pExpr = pId->next_sib;
    string id = string(pId->str_term);
    ASTNode *expr = ParserTreeToAST(pExpr);
    if (expr == NULL)
      curASTNode = new ASTVar(id);
    else
    {
      ASTNode *expr = ParserTreeToAST(pExpr);
      curASTNode = new ASTVar(id, (ASTExpr *)expr);
    }
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
  }
  else if (strncasecmp(parserNode->name, "SimpleExpr", 10) == 0)
  {
    /*
    SimpleExpr -> AddExpr
               -- RelOp
               -- AddExpr
     */
    struct Node *pLeftAddExpr = parserNode->child;
    ASTNode *leftAddExpr = ParserTreeToAST(pLeftAddExpr);
    struct Node *pRelOp = pLeftAddExpr->next_sib;
    if (pRelOp == NULL)
      curASTNode = new ASTSimpleExpr((ASTAddExpr *)leftAddExpr);
    else
    {
      ASTRelOp relOp;
      switch (pRelOp->int_term)
      {
      case LE:
        relOp = ASTLE;
        break;
      case LT:
        relOp = ASTLT;
        break;
      case GT:
        relOp = ASTGT;
        break;
      case GE:
        relOp = ASTGE;
        break;
      case EQ:
        relOp = ASTEQ;
        break;
      case NE:
        relOp = ASTNE;
        break;

      default:
        break;
      }
      struct Node *pRightAddExpr = pRelOp->next_sib;
      ASTNode *rightAddExpr = ParserTreeToAST(pRightAddExpr);
      curASTNode = new ASTSimpleExpr((ASTAddExpr *)leftAddExpr, relOp, (ASTAddExpr *)rightAddExpr);
    }
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
  }
  else if (strncasecmp(parserNode->name, "AddExpr", 7) == 0)
  {
    /*
    AddExpr -- AddExpr
            -- AddOp
            -> Term
     */
    if (strncasecmp(parserNode->child->name, "Term", 4) == 0)
    {
      struct Node *pTerm = parserNode->child;
      ASTNode *term = ParserTreeToAST(pTerm);
      curASTNode = new ASTAddExpr((ASTTerm *)term);
    }
    else if (strncasecmp(parserNode->child->name, "AddExpr", 7) == 0)
    {
      struct Node *pAddExpr = parserNode->child;
      ASTNode *firstTerm = ParserTreeToAST(pAddExpr->child);
      curASTNode = new ASTAddExpr((ASTTerm *)firstTerm);

      struct Node *pAddOp = pAddExpr->next_sib;
      ASTAddOp addOp = (pAddOp->int_term == ADD) ? ASTADD : ASTMINUS;
      struct Node *pTerm = parserNode->child;
      ASTNode *term = ParserTreeToAST(pTerm);
      ((ASTAddExpr *)curASTNode)->AddTerm(addOp, (ASTTerm *)term);
    }
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
  }
  else if (strncasecmp(parserNode->name, "Term", 4) == 0)
  {
    /*
    Term -- Term
         -- MulOp
         -> Factor
     */
    if (strncasecmp(parserNode->child->name, "Factor", 6) == 0)
    {
      struct Node *pFactor = parserNode->child;
      ASTNode *factor = ParserTreeToAST(pFactor);
      curASTNode = new ASTTerm((ASTFactor *)factor);
    }
    else if (strncasecmp(parserNode->child->name, "Term", 4) == 0)
    {
      struct Node *pTerm = parserNode->child;
      ASTNode *firstFactor = ParserTreeToAST(pTerm->child);
      curASTNode = new ASTTerm((ASTFactor *)firstFactor);

      struct Node *pMulOp = pTerm->next_sib;
      ASTMulOp addOp = (pMulOp->int_term == MUL) ? ASTMUL : ASTDIV;
      struct Node *pFactor = parserNode->child;
      ASTNode *factor = ParserTreeToAST(pFactor);
      ((ASTTerm *)curASTNode)->AddFactor(addOp, (ASTFactor *)factor);
    }
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
  }
  else if (strncasecmp(parserNode->name, "Factor", 6) == 0)
  {
    /*
    Factor - Expr
           - Var
           - Call
           - NUM
     */
    if (!strncasecmp(parserNode->child->name, "Expr", 4))
    {
      curASTNode = new ASTFactor((ASTExpr *)(ParserTreeToAST(parserNode->child)));
    }
    else if (!strncasecmp(parserNode->child->name, "Var", 3))
    {
      curASTNode = new ASTFactor((ASTVar *)ParserTreeToAST(parserNode->child));
    }
    else if (!strncasecmp(parserNode->child->name, "Call", 4))
    {
      curASTNode = new ASTFactor((ASTCall *)ParserTreeToAST(parserNode->child));
    }
    else if (parserNode->child->termKind == TermKNum)
    {
      if (parserNode->child->is_int)
      {
        curASTNode = new ASTFactor(parserNode->child->int_term);
      }
      else
      {
        curASTNode = new ASTFactor(parserNode->child->real_term);
      }
    }
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
  }
  else if (strncasecmp(parserNode->name, "Call", 4) == 0)
  {
    /*
    Call -> ID
         -> Args -- ArgList --> Expr
     */
    curASTNode = new ASTCall(parserNode->child->str_term);
    struct Node *p = parserNode->child->next_sib;
    while (p)
    {
      ((ASTCall *)curASTNode)->AddArg((ASTExpr *)ParserTreeToAST(p));
      p = p->next_sib;
    }
    curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
  }
  else // other node
  {
  }

  return curASTNode;
}