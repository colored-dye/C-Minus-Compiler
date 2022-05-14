/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:35
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-15 03:52:59
 * @FilePath: /C-Minus-Compiler/src/ast.hpp
 * @Description: AST for subsequent LLVM operations.
 *
 * Copyright (c) 2022 by SiO-2, All Rights Reserved.
 */

#ifndef CMinusCompiler_AST_H_
#define CMinusCompiler_AST_H_

#include <vector>
#include <string>

using namespace std;

extern const char *NodeNames[];

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
    ASTSELECTSTMT,
    ASTWHILESTMT,
    ASTFORSTMT,
    ASTRETURNSTMT,
    ASTVAR,
    ASTSIMPLEEXPR,
    ASTADDEXPR,
    ASTTERM,
    ASTFACTOR,
    ASTCALL
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

class ASTNode;
class Program;
class VarDecl;
class FunDecl;
class Param;
class CompoundStmt;
class Expr;
class SelectStmt;
class WhileStmt;
class ForStmt;
class ReturnStmt;
class Var;
class SimpleExpr;
class AddExpr;
class Term;
class Factor;
class Call;

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

public:
    ASTNode(int lineno = -1, int column = -1, ASTNodeType nodeType = ASTNODE)
        : lineno(lineno), column(column), nodeType(nodeType) {}
    virtual ~ASTNode() {}

    /**
     * @brief To set the coordinate of a node, it needs to be called manually every time a node is created.
     * @param {int} lineno
     * @param {int} column
     */
    void SetCoordinate(int lineno, int column)
    {
        ASTNode::lineno = lineno;
        ASTNode::column = column;
    }

    int GetLineno() const { return lineno; }
    int GetColumn() const { return column; }

    /**
     * @brief ASTNodeType - ASTNODE, ASTPROGRAM, ASTVARDECL, ASTFUNDECL, ASTPARAM, ASTCOMPOUNDSTMT,
     *  ASTEXPR, ASTVAR, ASTSIMPLEEXPR, ASTADDEXPR, ASTTERM, ASTFACTOR, ASTCALL, ASTSELECTSTMT,
     *  ASTWHILESTMT, ASTFORSTMT, ASTRETURNSTMT;
     */
    void SetNodeType(ASTNodeType setNodeType) { nodeType = setNodeType; }
    ASTNodeType GetNodeType() const { return nodeType; }
};

/**
 * @brief A program consists of a list (or sequence) of declarations,
 *  which may be function or variable declarations, in any order.
 * @param {vector<ASTNode*>} Decl - There must be at least one declaration.
 *  The last declaration in a program must be a function declaration of the form void main(void).
 */
class Program : public ASTNode
{
    vector<ASTNode *> declList;

public:
    Program() { SetNodeType(ASTPROGRAM); }
    ~Program() {}

    void AddDecl(ASTNode *decl) { declList.push_back(decl); }
    const vector<ASTNode *> &GetDeclList() const { return declList; }
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
        : typeSpec(typeSpec), id(id), isArray(false) { SetNodeType(ASTVARDECL); }

    VarDecl(ASTTypeSpec typeSpec, string id, int arrayLength)
        : typeSpec(typeSpec), id(id), isArray(true), arrayLength(arrayLength) { SetNodeType(ASTVARDECL); }

    ~VarDecl(){};

    ASTTypeSpec GetTypeSpec() const { return typeSpec; }
    const string GetId() const { return id; }
    bool IsArray() const { return isArray; }
    int GetArrayLength() const { return arrayLength; }
};

/**
 * @brief Function declaration.
 * @param {ASTTypeSpec} typeSpec - return type specifier.
 * @param {string} id - identifier.
 * @param {bool} haveParam - flag of parameters. By default there are no parameters.
 * @param {vector<Param*>} paramList - comma-separated list of parameters inside parentheses.
 * @param {CompoundStmt*>} compoundStmt - compound statement consists of curly brackets
 *  surrounding a set of declarations and statements.
 */
class FunDecl : public ASTNode
{
    ASTTypeSpec typeSpec;
    string id;
    bool haveParam; // Is there an param.
    vector<Param *> paramList;
    CompoundStmt *compoundStmt;

public:
    FunDecl(ASTTypeSpec typeSpec, string id, bool haveParam = false)
        : typeSpec(typeSpec), id(id), haveParam(haveParam)
    {
        compoundStmt = NULL;
        SetNodeType(ASTCOMPOUNDSTMT);
    }

    ~FunDecl() {}

    void AddParam(Param *param)
    {
        if (!haveParam)
            haveParam = true;
        paramList.push_back(param);
    }

    void AddCompoundStmt(CompoundStmt *compoundStmt) { FunDecl::compoundStmt = compoundStmt; };

    ASTTypeSpec GetTypeSpec() const { return typeSpec; }
    const string GetId() const { return id; }
    bool HaveParam() const { return haveParam; }
    const vector<Param *> &GetParamList() const { return paramList; }
    const CompoundStmt *GetCompoundStmt() const { return compoundStmt; }
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
        : typeSpec(typeSpec), id(id), isArray(isArray) { SetNodeType(ASTPARAM); }

    ~Param(){};

    ASTTypeSpec GetTypeSpec() const { return typeSpec; }
    const string GetId() const { return id; }
    bool IsArray() const { return isArray; }
};

/**
 * @brief compound statement consists of curly brackets surrounding a set of declarations and statements.
 * @param {vector<ASTNode*>} declStmtList
 */
class CompoundStmt : public ASTNode
{
    vector<ASTNode *> declStmtList;

public:
    CompoundStmt() { SetNodeType(ASTCOMPOUNDSTMT); }
    ~CompoundStmt() {}

    void AddDecl(ASTNode *decl) { declStmtList.push_back(decl); };
    void AddStmt(ASTNode *stmt) { declStmtList.push_back(stmt); };
};

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
        : isAssignStmt(false), var(NULL), expr(NULL), simpleExpr(simpleExpr) { SetNodeType(ASTEXPR); }

    Expr(Var *var, Expr *expr)
        : isAssignStmt(true), var(var), expr(expr), simpleExpr(NULL) { SetNodeType(ASTEXPR); }

    ~Expr() {}

    bool IsAssignStmt() const { return isAssignStmt; }
    const Var *GetVar() const { return var; }
    const Expr *GetExpr() const { return expr; }
    const SimpleExpr *GetSimpleExpr() const { return simpleExpr; }
};

/**
 * @brief The if-statement has the usual semantics: the expression is evaluated; a nonzero
 *  value causes execution of the first statement; a zero value causes execution of the
 *  second statement, if it exists.
 * @param {Expr*} expr - the expression to evaluate.
 * @param {ASTNode*} trueStmt - statement for true.
 * @param {bool} haveElse - false by default.
 * @param {ASTNode*} falseStmt - statement for false.
 */
class SelectStmt : public ASTNode
{
    Expr *expr;
    ASTNode *trueStmt;
    bool haveElse;
    ASTNode *falseStmt;

public:
    SelectStmt(Expr *expr, ASTNode *trueStmt)
        : expr(expr), trueStmt(trueStmt), haveElse(false), falseStmt(NULL) { SetNodeType(ASTSELECTSTMT); }
    SelectStmt(Expr *expr, ASTNode *trueStmt, ASTNode *falseStmt)
        : expr(expr), trueStmt(trueStmt), haveElse(true), falseStmt(falseStmt) { SetNodeType(ASTSELECTSTMT); }
    ~SelectStmt() {}

    const Expr *GetExpr() const { return expr; }
    const ASTNode *GetTrueStmt() const { return trueStmt; }
    bool HaveElse() const { return haveElse; }
    const ASTNode *GetFalseStmt() const { return falseStmt; }
};

/**
 * @brief The while-statement is executed by repeatedly evaluating the expression and then
 *  executing the statement if the expression evaluates to a nonzero value, ending when the
 *  expression evaluates to 0.
 * @param {Expr*} expr - the expression to evaluate.
 * @param {ASTNode*} stmt - statement.
 */
class WhileStmt : public ASTNode
{
    Expr *expr;
    ASTNode *stmt;

public:
    WhileStmt(Expr *expr, ASTNode *stmt) : expr(expr), stmt(stmt) { SetNodeType(ASTWHILESTMT); }
    ~WhileStmt() {}

    const Expr *GetExpr() const { return expr; }
    const ASTNode *GetStmt() const { return stmt; }
};

/**
 * @brief The for-statement.
 * @param {bool} haveForParam1 - false by default.
 * @param {Var*} var1
 * @param {Expr*} expr1
 * @param {Expr*} expr2
 * @param {bool} haveForParam3 - false by default.
 * @param {Var*} var3
 * @param {Expr*} expr3
 * @param {ASTNode*} stmt
 */
class ForStmt : public ASTNode
{
    bool haveForParam1; // Is there an For_param1. false by default.
    Var *var1;
    Expr *expr1;

    Expr *expr2;

    bool haveForParam3; // Is there an For_param3. false by default.
    Var *var3;
    Expr *expr3;

    ASTNode *stmt;

public:
    ForStmt(Expr *expr2, ASTNode *stmt)
        : haveForParam1(false), var1(NULL), expr1(NULL), expr2(expr2),
          haveForParam3(false), var3(NULL), expr3(NULL), stmt(stmt) { SetNodeType(ASTFORSTMT); }
    ~ForStmt() {}

    void AddForParam1(Var *var1, Expr *expr1)
    {
        if (!haveForParam1)
        {
            haveForParam1 = true;
            ForStmt::var1 = var1;
            ForStmt::expr1 = expr1;
        }
    }

    void AddForParam3(Var *var3, Expr *expr3)
    {
        if (!haveForParam3)
        {
            haveForParam3 = true;
            ForStmt::var3 = var3;
            ForStmt::expr3 = expr3;
        }
    }

    bool HaveForParam1() const { return haveForParam1; }
    const Var *GetVar1() const { return var1; }
    const Expr *GetExpr1() const { return expr1; }
    const Expr *GetExpr2() const { return expr2; }
    bool HaveForParam3() const { return haveForParam3; }
    const Var *GetVar3() const { return var3; }
    const Expr *GetExpr3() const { return expr3; }
    const ASTNode *GetStmt() const { return stmt; }
};

/**
 * @brief A return statement may either return a value or not. Functions not declared void
 *  must return values. Functions declared void must not return values.
 * @param {bool} isVoid - true by default.
 * @param {Expr*} expr
 */
class ReturnStmt : public ASTNode
{
    bool isVoid; // Whether the function type is void. True by default.
    Expr *expr;

public:
    ReturnStmt() : isVoid(true), expr(NULL) { SetNodeType(ASTRETURNSTMT); }
    ~ReturnStmt() {}

    void AddExpr(Expr *expr)
    {
        if (isVoid)
            isVoid = false;
        ReturnStmt::expr = expr;
    }

    bool IsVoid() const { return isVoid; }
    const Expr *GetExpr() const { return expr; }
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
    Var(string id) : id(id), haveSubscript(false), subscript(NULL) { SetNodeType(ASTVAR); }
    Var(string id, Expr *subscript) : id(id), haveSubscript(true), subscript(subscript) { SetNodeType(ASTVAR); }

    ~Var() {}

    const string GetId() const { return id; }
    bool HaveSubscript() const { return haveSubscript; }
    const Expr *GetExpr() const { return subscript; }
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
    SimpleExpr(AddExpr *addExpr) : leftAddExpr(addExpr), haveRightAddExpr(false)
    {
        rightAddExpr = NULL;
        SetNodeType(ASTSIMPLEEXPR);
    }
    SimpleExpr(AddExpr *leftAddExpr, ASTRelOp relOp, AddExpr *rightAddExpr)
        : leftAddExpr(leftAddExpr), haveRightAddExpr(true), relOp(relOp), rightAddExpr(rightAddExpr) { SetNodeType(ASTSIMPLEEXPR); }

    ~SimpleExpr() {}

    const AddExpr *GetLeftAddExpr() const { return leftAddExpr; }
    bool HaveRightAddExpr() const { return haveRightAddExpr; }
    ASTRelOp GetRelOp() const { return relOp; }
    const AddExpr *GetRightAddExpr() const { return rightAddExpr; }
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
    AddExpr(Term *firstTerm) : firstTerm(firstTerm), areMultipleTerms(false) { SetNodeType(ASTADDEXPR); }
    ~AddExpr() {}

    void AddTerm(ASTAddOp addOp, Term *term)
    {
        if (!areMultipleTerms)
            areMultipleTerms = true;
        addOpList.push_back(addOp);
        termList.push_back(term);
    }

    const Term *GetFirstTerm() const { return firstTerm; }
    bool AreMultipleTerms() const { return areMultipleTerms; }
    const vector<ASTAddOp> &GetAddOpList() const { return addOpList; }
    const vector<Term *> &GetTermList() const { return termList; }
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
    Term(Factor *firstFactor) : firstFactor(firstFactor), areMultipleFactors(false) { SetNodeType(ASTTERM); }
    ~Term() {}

    void AddFactor(ASTMulOp mulOp, Factor *factor)
    {
        if (!areMultipleFactors)
            areMultipleFactors = true;
        mulOpList.push_back(mulOp);
        factorList.push_back(factor);
    }

    const Factor *GetFirstFactor() const { return firstFactor; }
    bool AreMultipleFactors() const { return areMultipleFactors; }
    const vector<ASTMulOp> &GetMulOpList() const { return mulOpList; }
    const vector<Factor *> &GetFactorList() const { return factorList; }
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
    bool isInt;
    int numInt;
    bool isReal;
    float numReal;

public:
    Factor(Expr *expr) : expr(expr), var(NULL), callExpr(NULL), isInt(false)
    {
        isReal = false;
        SetNodeType(ASTFACTOR);
    }
    Factor(Var *var) : expr(NULL), var(var), callExpr(NULL), isInt(false)
    {
        isReal = false;
        SetNodeType(ASTFACTOR);
    }
    Factor(Call *callExpr) : expr(NULL), var(NULL), callExpr(callExpr), isInt(false)
    {
        isReal = false;
        SetNodeType(ASTFACTOR);
    }
    Factor(int num) : expr(NULL), var(NULL), callExpr(NULL), isInt(true), numInt(num), isReal(false) { SetNodeType(ASTFACTOR); }
    Factor(float num) : expr(NULL), var(NULL), callExpr(NULL), isInt(false)
    {
        isReal = true;
        numReal = num;
        SetNodeType(ASTFACTOR);
    }

    ~Factor() {}

    const Expr *GetExpr() const { return expr; }
    const Var *GetVar() const { return var; }
    const Call *GetCallExpr() const { return callExpr; }
    bool IsInt() const { return isInt; }
    int GetNumInt() const { return numInt; }
    bool IsReal() const { return isReal; }
    float GetNumReal() const { return numReal; }
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
    Call(string id) : id(id) { SetNodeType(ASTCALL); }
    ~Call() {}

    void AddArg(Expr *arg) { argList.push_back(arg); }

    const string GetId() const { return id; }
    const vector<Expr *> &GetArgList() const { return argList; }
};

#endif