/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:35
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-12 21:47:35
 * @FilePath: /C-Minus-Compiler/src/ast.hpp
 * @Description: Convert the parse tree to AST for subsequent LLVM operations.
 *
 * Copyright (c) 2022 by SiO-2, All Rights Reserved.
 */

/**
 * TODO：
 *  1. 在使用 vector 记录子节点的 class 中，如何记录节点（语句块）的顺序？
 */

#ifndef CMinusCompiler_AST_H_
#define CMinusCompiler_AST_H_

#include "node.h"

#include <vector>
#include <string>
#include <variant>

using namespace std;

extern const char *NodeNames[];

/**
 * @brief Number has two types, int and float.
 */
union ASTNUM
{
    int intNum;
    float floatNum;
};

/**
 * @brief Support three types void, int, real (ie float), respectively ASTVOID = 0, ASTINT = 1, ASTFLOAT = 2.
 */
enum ASTTypeSpec
{
    ASTVOID = 0,
    ASTINT,
    ASTREAL
};

/**
 * @brief ASTNODE, ASTPROGRAM, ASTVARDECL, ASTFUNDECL, ASTPARAM, ASTCOMPOUNDSTMT,
 *  ASTEXPR, ASTVAR, ASTSIMPLEEXPR, ASTADDEXPR, ASTTERM, ASTFACTOR, ASTCALL,
 *  ASTSELECTSTMT, ASTWHILESTMT, ASTFORSTMT, ASTRETURNSTMT;
 */
enum ASTNodeType
{
    ASTNODE = 0,
    ASTPROGRAM,
    ASTVARDECL,
    ASTFUNDECL,
    ASTPARAM,
    ASTCOMPOUNDSTMT,
    ASTEXPR,
    ASTVAR,
    ASTSIMPLEEXPR,
    ASTADDEXPR,
    ASTTERM,
    ASTFACTOR,
    ASTCALL,
    ASTSELECTSTMT,
    ASTWHILESTMT,
    ASTFORSTMT,
    ASTRETURNSTMT
};

/**
 * @brief ASTLE, ASTLT, ASTGT, ASTGE, ASTEQ, ASTNE.
 */
enum ASTRelOp
{
    ASTLE = 0,
    ASTLT,
    ASTGT,
    ASTGE,
    ASTEQ,
    ASTNE
};

/**
 * @brief ASTADD, ASTMINUS.
 */
enum ASTAddOp
{
    ASTADD = 0,
    ASTMINUS
};

/**
 * @brief ASTNUL, ASTDIV.
 */
enum ASTMulOp
{
    ASTMUL = 0,
    ASTDIV
};

/**
 * @brief Base class for all expression nodes. The target AST is a LC-RS binary tree.
 * @param {int} lineno - initialized to -1.
 * @param {int} colume - initialized to -1.
 * @param {ASTNodeType} astNodeType - initialized to ASTNODE.
 */
class ASTNode
{
    int lineno, column;
    ASTNodeType nodeType;
    // ASTNode *leftChild;
    // ASTNode *rightSibling;

public:
    ASTNode(int lineno = -1, int column = -1, ASTNodeType nodeType = ASTNODE)
        : lineno(lineno), column(column), nodeType(nodeType) {}
    virtual ~ASTNode() {}

    void SetLineno(int setLineno) { lineno = setLineno; }
    int GetLineno() { return lineno; }

    void SetColumn(int setColumn) { column = setColumn; }
    int GetColumn() { return column; }

    /**
     * @brief ASTNodeType - ASTNODE, ASTPROGRAM, ASTVARDECL, ASTFUNDECL, ASTPARAM, ASTCOMPOUNDSTMT,
     *  ASTEXPR, ASTVAR, ASTSIMPLEEXPR, ASTADDEXPR, ASTTERM, ASTFACTOR, ASTCALL, ASTSELECTSTMT,
     *  ASTWHILESTMT, ASTFORSTMT, ASTRETURNSTMT;
     */
    void SetNodeType(ASTNodeType setNodeType) { nodeType = setNodeType; }
    ASTNodeType GetNodeType() { return nodeType; }

    /**
     * @brief To set the basic information of a node, it needs to be called manually every time a node is created.
     * @param {int} lineno
     * @param {int} column
     * @param {ASTNodeType} nodeType - ASTNODE, ASTPROGRAM, ASTVARDECL, ASTFUNDECL, ASTPARAM,
     *  ASTCOMPOUNDSTMT, ASTEXPR, ASTVAR, ASTSIMPLEEXPR, ASTADDEXPR, ASTTERM, ASTFACTOR, ASTCALL,
     *  ASTSELECTSTMT, ASTWHILESTMT, ASTFORSTMT, ASTRETURNSTMT;
     */
    void SetASTNodeData(int lineno, int column, ASTNodeType nodeType)
    {
        ASTNode::lineno = lineno;
        ASTNode::column = column;
        ASTNode::nodeType = nodeType;
    }

    // void SetLeftChild(ASTNode *setLeftChild) { leftChild = setLeftChild; }
    // ASTNode *GetLeftChild() { return leftChild; }
    //
    // void SetRightSibling(ASTNode *setRightSibling) { rightSibling = setRightSibling; }
    // ASTNode *GetRightSibling() { return rightSibling; }
};

/**
 * @brief A program consists of a list (or sequence) of declarations,
 *  which may be function or variable declarations, in any order.
 * @param {vector<ASTNode*>} Decl - There must be at least one declaration.
 *  The last declaration in a program must be a function declaration of the form void main(void).
 */
class Program : public ASTNode
{
    vector<ASTNode *> DeclList;

public:
    Program() {}
    void AddDecl(ASTNode *decl) { DeclList.push_back(decl); }
};

/**
 * @brief A variable declaration declares either a simple variable of integer type or an array
 *  variable whose base type is integer or float, and whose indices range from 0 ... num-1.
 * @param {ASTTypeSpec} typeSpec - int, real or void.
 * @param {string} id - identifier.
 * @param {bool} isArray - by default is not an array.
 * @param {int} arrayLength - array length.
 */
class VarDecl : public ASTNode
{
    ASTTypeSpec typeSpec;
    string id;
    bool isArray;
    int arrayLength;

public:
    VarDecl(ASTTypeSpec typeSpec, string id)
        : typeSpec(typeSpec), id(id), isArray(false) {}

    VarDecl(ASTTypeSpec typeSpec, string id, int arrayLength)
        : typeSpec(typeSpec), id(id), isArray(true), arrayLength(arrayLength) {}
};

/**
 * @brief Function declaration.
 * @param {ASTTypeSpec} typeSpec - return type specifier.
 * @param {string} id - identifier.
 * @param {bool} haveParam - flag of parameters. By default there are no parameters.
 * @param {vector<Param*>} paramList - comma-separated list of parameters inside parentheses.
 * @param {vector<ASTNode*>} compoundStmt - compound statement consists of curly brackets
 *  surrounding a set of declarations and statements.
 */
class FunDecl : public ASTNode
{
    ASTTypeSpec typeSpec;
    string id;
    bool haveParam; // Is there an param.
    vector<Param *> paramList;
    vector<ASTNode *> compoundStmt;

public:
    FunDecl(ASTTypeSpec typeSpec, string id, bool haveParam = false)
        : typeSpec(typeSpec), id(id), haveParam(haveParam) {}

    void AddParam(Param *param)
    {
        if (!haveParam)
            haveParam = true;
        paramList.push_back(param);
    }

    void AddStmt(ASTNode *stmt) { compoundStmt.push_back(stmt); };
};

/**
 * @brief Parameters of a function are either void (i.e., there  *  are no parameters) or
 *  a list representing the function's parameters. Parameters followed by brackets are array
 *  parameters whose size can vary. Simple integer parameters are passed by value. Array
 *  parameters are passed by reference (i.e., as pointers) and must be matched by an array
 *  variable during a call.
 * @param {ASTTypeSpec} typeSpec - int, real or void.
 * @param {string} id - identifier.
 * @param {bool} isArray - by default is not an array.
 */
class Param : public ASTNode
{
    ASTTypeSpec typeSpec;
    string id;
    bool isArray;

public:
    Param(ASTTypeSpec typeSpec, string id, bool isArray = false)
        : typeSpec(typeSpec), id(id), isArray(isArray) {}
};

//
/**
 * @brief An expression is a yariable reference followed by an assignment symbol (equal sign)
 *  and an expression, or just a simple expression.
 * @param {bool} isAssignStmt - false by default. it's assignment or simple expression.
 * @param {Var*} var - pointer to the variable.
 * @param {Expr*} expr - pointer to the expression to evaluate.
 * @param {SimpleExpr*} simpleExpr - pointer to the simple expression.
 */
class Expr : public ASTNode
{
    bool isAssignStmt; // Is it an assignment statement. By default it is not.
    Var *var;
    Expr *expr;
    SimpleExpr *simpleExpr;

public:
    Expr(SimpleExpr *simpleExpr)
        : isAssignStmt(false), var(NULL), expr(NULL), simpleExpr(simpleExpr) {}

    Expr(Var *var, Expr *expr)
        : isAssignStmt(true), var(var), expr(expr), simpleExpr(NULL) {}
};

/**
 * @brief A var is either a simple (integer) variable or a subscripted array variable.
 *  A negative subscript causes the program to halt (unlike C).
 * @param {string} id - identifier.
 * @param {bool} haveSubscript - no subscript by default.
 * @param {Expr*} subscript - pointer to the expression that evaluates the subscript.
 */
class Var : public ASTNode
{
    string id;
    bool haveSubscript;
    Expr *subscript;

public:
    Var(string id) : id(id), haveSubscript(false), subscript(NULL) {}
    Var(string id, Expr *subscript) : id(id), haveSubscript(true), subscript(subscript) {}
};

/**
 * @brief A simple expression consists of relational operators. The value of a simple expression
 *  is either the value of its additive expression if it contains no relational operators, or 1
 *  if the relational operator evaluates to true, or 0 if it evaluates to false.
 * @param {AddExpr*} leftAddExpr - left additive-expression.
 * @param {bool} haveRightAddExpr - false by default.
 * @param {ASTRelOp} relOp - relational operator.
 * @param {AddExpr*} rightAddExpr - right additive-expression.
 */
class SimpleExpr : public ASTNode
{
    AddExpr *leftAddExpr;
    bool haveRightAddExpr; // Is there a right additive-expression? By default it's false.
    ASTRelOp relOp;
    AddExpr *rightAddExpr;

public:
    SimpleExpr(AddExpr *addExpr) : leftAddExpr(addExpr), haveRightAddExpr(false) { rightAddExpr = NULL; }
    SimpleExpr(AddExpr *leftAddExpr, ASTRelOp relOp, AddExpr *rightAddExpr)
        : leftAddExpr(leftAddExpr), haveRightAddExpr(true), relOp(relOp), rightAddExpr(rightAddExpr) {}
};

/**
 * @brief Additive expressions and terms represent the typical associativity and precedence of the arithmetic operators.
 * @param {Term*} firstTerm - first term.
 * @param {bool} areMultipleTerms - false by default.
 * @param {vector<ASTAddOp>} addOpList - addition operator list.
 * @param {vector<Term*>} termList - term list.
 */
class AddExpr : public ASTNode
{
    Term *firstTerm;
    bool areMultipleTerms; // Are there multiple terms? By default it's false.
    vector<ASTAddOp> addOpList;
    vector<Term *> termList;

public:
    AddExpr(Term *firstTerm) : firstTerm(firstTerm), areMultipleTerms(false) {}

    void AddTerm(ASTAddOp addOp, Term *term)
    {
        if (!areMultipleTerms)
            areMultipleTerms = true;
        addOpList.push_back(addOp);
        termList.push_back(term);
    }
};

/**
 * @brief Additive expressions and terms represent the typical associativity and precedence of the arithmetic operators.
 * @param {Factor*} firstFactor - first factor.
 * @param {bool} areMultipleFactors - false by default.
 * @param {vector<ASTMulOp>} mulOpList - multiplication operator list.
 * @param {vector<Factor*>} factorList - factor list.
 */
class Term : public ASTNode
{

    Factor *firstFactor;
    bool areMultipleFactors; // Are there multiple factors? By default it's false.
    vector<ASTMulOp> mulOpList;
    vector<Factor *> factorList;

public:
    Term(Factor *firstFactor) : firstFactor(firstFactor), areMultipleFactors(false) {}

    void AddTerm(ASTMulOp mulOp, Factor *factor)
    {
        if (!areMultipleFactors)
            areMultipleFactors = true;
        mulOpList.push_back(mulOp);
        factorList.push_back(factor);
    }
};

/**
 * @brief A factor is an expression enclosed in parentheses, a variable, which evaluates
 *  to the value of its variable; a call of a function, which evaluates to the returned
 *  value of the function; or a NUM, whose value is computed by the scanner.
 * @param {Expr*} expr - NULL if is not expr.
 * @param {Var*} var - NULL if is not var.
 * @param {Call*} callExpr - NULL if is not callExpr.
 * @param {bool} isNum - false if is not num.
 * @param {ASTNUM} num - store the value of num.
 */
class Factor : public ASTNode
{
    Expr *expr;
    Var *var;
    Call *callExpr;
    bool isNum;
    ASTNUM num;

public:
    Factor(Expr *expr) : expr(expr), var(NULL), callExpr(NULL), isNum(false) {}
    Factor(Var *var) : expr(NULL), var(var), callExpr(NULL), isNum(false) {}
    Factor(Call *callExpr) : expr(NULL), var(NULL), callExpr(callExpr), isNum(false) {}
    Factor(ASTNUM num) : expr(NULL), var(NULL), callExpr(NULL), isNum(true), num(num) {}
};

/**
 * @brief A function call consists of an ID (the name of the function), followed by its arguments.
 * @param {string} id - identifier of callee.
 * @param {vector<Expr*>} argList - argument list.
 */
class Call : public ASTNode
{
    string id;
    vector<Expr *> argList;

public:
    Call() {}
    Call(string id) : id(id) {}
    void AddArg(Expr *arg) { argList.push_back(arg); }
};

/**
 * @brief The if-statement has the usual semantics: the expression is evaluated; a nonzero
 *  value causes execution of the first statement; a zero value causes execution of the
 *  second statement, if it exists.
 * @param {Expr*} expr - the expression to evaluate.
 * @param {vector<ASTNode*>} trueCompoundStmt - statement list for true.
 * @param {bool} haveElse - false by default.
 * @param {vector<ASTNode*>} falseCompoundStmt - statement list for false.
 */
class SelectStmt : public ASTNode
{
    Expr *expr;
    vector<ASTNode *> trueCompoundStmt;
    bool haveElse;
    vector<ASTNode *> falseCompoundStmt;

public:
    SelectStmt(Expr *expr = NULL) : expr(expr) { haveElse = false; }
    void AddTrueStmt(ASTNode *stmt) { trueCompoundStmt.push_back(stmt); }
    void AddFalseStmt(ASTNode *stmt)
    {
        haveElse = true;
        falseCompoundStmt.push_back(stmt);
    }
};

/**
 * @brief The while-statement is executed by repeatedly evaluating the expression and then
 *  executing the statement if the expression evaluates to a nonzero value, ending when the
 *  expression evaluates to 0.
 * @param {Expr*} expr - the expression to evaluate.
 * @param {vector<ASTNode*>} compoundStmt - statement list.
 */
class WhileStmt : public ASTNode
{
    Expr *expr;
    vector<ASTNode *> compoundStmt;

public:
    WhileStmt(Expr *expr = NULL) : expr(expr) {}
    void AddStmt(ASTNode *stmt) { compoundStmt.push_back(stmt); }
};

class ForStmt : public ASTNode
{
    bool haveForParam1; // Is there an For_param1. false by default.
    Var *var1;
    Expr *expr1;

    Expr *forParam2;

    bool haveForParam3; // Is there an For_param3. false by default.
    Var *var3;
    Expr *expr3;

    vector<ASTNode *> compoundStmt;

public:
    ForStmt() : haveForParam1(false), var1(NULL), expr1(NULL), forParam2(NULL),
                haveForParam3(false), var3(NULL), expr3(NULL) {}

    void AddForParam1(Var *var1, Expr *expr1)
    {
        if (!haveForParam1)
        {
            haveForParam1 = true;
            ForStmt::var1 = var1;
            ForStmt::expr1 = expr1;
        }
    }

    void AddForParam2(Expr *forParam2) { ForStmt::forParam2 = forParam2; }

    void AddForParam3(Var *var3, Expr *expr3)
    {
        if (!haveForParam3)
        {
            haveForParam3 = true;
            ForStmt::var3 = var3;
            ForStmt::expr3 = expr3;
        }
    }

    void AddStmt(ASTNode *stmt) { compoundStmt.push_back(stmt); }
};

//A return statement may either return a value or not. Functions not declared void\
must return values. Functions declared void must not return values.

class ReturnStmt : public ASTNode
{
    bool isVoid; // Whether the function type is void. True by default.
    Expr *expr;

public:
    ReturnStmt(bool isVoid = true) : isVoid(isVoid), expr(NULL) {}
    void AddExpr(Expr *expr)
    {
        if (isVoid)
            isVoid = false;
        ReturnStmt::expr = expr;
    }
};

#endif