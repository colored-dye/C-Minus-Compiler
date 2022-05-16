/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:35
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-16 23:37:07
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
class ASTProgram;
class ASTVarDecl;
class ASTFunDecl;
class ASTParam;
class ASTCompoundStmt;
class ASTExpr;
class ASTSelectStmt;
class ASTWhileStmt;
class ASTForStmt;
class ASTReturnStmt;
class ASTVar;
class ASTSimpleExpr;
class ASTAddExpr;
class ASTTerm;
class ASTFactor;
class ASTCall;

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
class ASTProgram : public ASTNode
{
    vector<ASTNode *> declList;

public:
    ASTProgram() { SetNodeType(ASTPROGRAM); }
    ~ASTProgram() {}

    void AddDecl(ASTNode *decl) { declList.push_back(decl); }
    const vector<ASTNode *> &GetDeclList() const { return declList; }
};

/**
 * @brief A variable declaration declares either a simple variable of integer type or an array
 *  variable whose base type is integer or float, and whose indices range from 0 ... num-1.
 * @param {ASTTypeSpec} typeSpec - int, real.
 * @param {string} id - identifier.
 * @param {bool} isArray - by default is not an array.
 * @param {int} arrayLength - array length.
 * @param {bool} isGlobal - global variables: true; local variables: false
 */
class ASTVarDecl : public ASTNode
{
    ASTTypeSpec typeSpec; // int or real
    string id;
    bool isArray;
    int arrayLength;

public:
    bool isGlobal; // 全局变量: true; 局部变量: false

    ASTVarDecl(ASTTypeSpec typeSpec, string id)
        : typeSpec(typeSpec), id(id), isArray(false) { SetNodeType(ASTVARDECL); }

    ASTVarDecl(ASTTypeSpec typeSpec, string id, int arrayLength)
        : typeSpec(typeSpec), id(id), isArray(true), arrayLength(arrayLength) { SetNodeType(ASTVARDECL); }

    ~ASTVarDecl(){};

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
 * @param {vector<ASTParam*>} paramList - comma-separated list of parameters inside parentheses.
 * @param {ASTCompoundStmt*>} compoundStmt - compound statement consists of curly brackets
 *  surrounding a set of declarations and statements.
 */
class ASTFunDecl : public ASTNode
{
    ASTTypeSpec typeSpec;
    string id;
    bool haveParam; // Is there an param.
    vector<ASTParam *> paramList;
    ASTCompoundStmt *compoundStmt;

public:
    ASTFunDecl(ASTTypeSpec typeSpec, string id, bool haveParam = false)
        : typeSpec(typeSpec), id(id), haveParam(haveParam)
    {
        compoundStmt = NULL;
        SetNodeType(ASTFUNDECL);
    }

    ~ASTFunDecl() {}

    void AddParam(ASTParam *param)
    {
        if (!haveParam)
            haveParam = true;
        paramList.push_back(param);
    }

    void AddCompoundStmt(ASTCompoundStmt *compoundStmt) { ASTFunDecl::compoundStmt = compoundStmt; };

    ASTTypeSpec GetTypeSpec() const { return typeSpec; }
    const string GetId() const { return id; }
    bool HaveParam() const { return haveParam; }
    const vector<ASTParam *> &GetParamList() const { return paramList; }
    const ASTCompoundStmt *GetCompoundStmt() const { return compoundStmt; }
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
class ASTParam : public ASTNode
{
    ASTTypeSpec typeSpec;
    string id;
    bool isArray;

public:
    ASTParam(ASTTypeSpec typeSpec, string id, bool isArray = false)
        : typeSpec(typeSpec), id(id), isArray(isArray) { SetNodeType(ASTPARAM); }

    ~ASTParam(){};

    ASTTypeSpec GetTypeSpec() const { return typeSpec; }
    const string GetId() const { return id; }
    bool IsArray() const { return isArray; }
};

/**
 * @brief compound statement consists of curly brackets surrounding a set of declarations and statements.
 * @param {vector<ASTNode*>} declStmtList
 */
class ASTCompoundStmt : public ASTNode
{
    vector<ASTNode *> declStmtList;

public:
    ASTCompoundStmt() { SetNodeType(ASTCOMPOUNDSTMT); }
    ~ASTCompoundStmt() {}

    void AddDecl(ASTNode *decl) { declStmtList.push_back(decl); };
    void AddStmt(ASTNode *stmt) { declStmtList.push_back(stmt); };
};

/**
 * @brief An expression is a yariable reference followed by an assignment symbol (equal sign)
 *  and an expression, or just a simple expression.
 * @param {bool} isAssignStmt - false by default. it's assignment or simple expression.
 * @param {ASTVar*} var - pointer to the variable.
 * @param {ASTExpr*} expr - pointer to the expression to evaluate.
 * @param {ASTSimpleExpr*} simpleExpr - pointer to the simple expression.
 */
class ASTExpr : public ASTNode
{
    bool isAssignStmt; // Is it an assignment statement. By default it is not.
    ASTVar *var;
    ASTExpr *expr;
    ASTSimpleExpr *simpleExpr;

public:
    ASTExpr(ASTSimpleExpr *simpleExpr)
        : isAssignStmt(false), var(NULL), expr(NULL), simpleExpr(simpleExpr) { SetNodeType(ASTEXPR); }

    ASTExpr(ASTVar *var, ASTExpr *expr)
        : isAssignStmt(true), var(var), expr(expr), simpleExpr(NULL) { SetNodeType(ASTEXPR); }

    ~ASTExpr() {}

    bool IsAssignStmt() const { return isAssignStmt; }
    const ASTVar *GetVar() const { return var; }
    const ASTExpr *GetExpr() const { return expr; }
    const ASTSimpleExpr *GetSimpleExpr() const { return simpleExpr; }
};

/**
 * @brief The if-statement has the usual semantics: the expression is evaluated; a nonzero
 *  value causes execution of the first statement; a zero value causes execution of the
 *  second statement, if it exists.
 * @param {ASTExpr*} expr - the expression to evaluate.
 * @param {ASTNode*} trueStmt - statement for true.
 * @param {bool} haveElse - false by default.
 * @param {ASTNode*} falseStmt - statement for false.
 */
class ASTSelectStmt : public ASTNode
{
    ASTExpr *expr;
    ASTNode *trueStmt;
    bool haveElse;
    ASTNode *falseStmt;

public:
    ASTSelectStmt(ASTExpr *expr, ASTNode *trueStmt)
        : expr(expr), trueStmt(trueStmt), haveElse(false), falseStmt(NULL) { SetNodeType(ASTSELECTSTMT); }
    ASTSelectStmt(ASTExpr *expr, ASTNode *trueStmt, ASTNode *falseStmt)
        : expr(expr), trueStmt(trueStmt), haveElse(true), falseStmt(falseStmt) { SetNodeType(ASTSELECTSTMT); }
    ~ASTSelectStmt() {}

    const ASTExpr *GetExpr() const { return expr; }
    const ASTNode *GetTrueStmt() const { return trueStmt; }
    bool HaveElse() const { return haveElse; }
    const ASTNode *GetFalseStmt() const { return falseStmt; }
};

/**
 * @brief The while-statement is executed by repeatedly evaluating the expression and then
 *  executing the statement if the expression evaluates to a nonzero value, ending when the
 *  expression evaluates to 0.
 * @param {ASTExpr*} expr - the expression to evaluate.
 * @param {ASTNode*} stmt - statement.
 */
class ASTWhileStmt : public ASTNode
{
    ASTExpr *expr;
    ASTNode *stmt;

public:
    ASTWhileStmt(ASTExpr *expr, ASTNode *stmt) : expr(expr), stmt(stmt) { SetNodeType(ASTWHILESTMT); }
    ~ASTWhileStmt() {}

    const ASTExpr *GetExpr() const { return expr; }
    const ASTNode *GetStmt() const { return stmt; }
};

/**
 * @brief The for-statement.
 * @param {bool} haveForParam1 - false by default.
 * @param {ASTVar*} var1
 * @param {ASTExpr*} expr1
 * @param {ASTExpr*} expr2
 * @param {bool} haveForParam3 - false by default.
 * @param {ASTVar*} var3
 * @param {ASTExpr*} expr3
 * @param {ASTNode*} stmt
 */
class ASTForStmt : public ASTNode
{
    bool haveForParam1; // Is there an For_param1. false by default.
    ASTVar *var1;
    ASTExpr *expr1;

    ASTExpr *expr2;

    bool haveForParam3; // Is there an For_param3. false by default.
    ASTVar *var3;
    ASTExpr *expr3;

    ASTNode *stmt;

public:
    ASTForStmt(ASTExpr *expr2, ASTNode *stmt)
        : haveForParam1(false), var1(NULL), expr1(NULL), expr2(expr2),
          haveForParam3(false), var3(NULL), expr3(NULL), stmt(stmt) { SetNodeType(ASTFORSTMT); }
    ~ASTForStmt() {}

    void AddForParam1(ASTVar *var1, ASTExpr *expr1)
    {
        if (!haveForParam1)
        {
            haveForParam1 = true;
            ASTForStmt::var1 = var1;
            ASTForStmt::expr1 = expr1;
        }
    }

    void AddForParam3(ASTVar *var3, ASTExpr *expr3)
    {
        if (!haveForParam3)
        {
            haveForParam3 = true;
            ASTForStmt::var3 = var3;
            ASTForStmt::expr3 = expr3;
        }
    }

    bool HaveForParam1() const { return haveForParam1; }
    const ASTVar *GetVar1() const { return var1; }
    const ASTExpr *GetExpr1() const { return expr1; }
    const ASTExpr *GetExpr2() const { return expr2; }
    bool HaveForParam3() const { return haveForParam3; }
    const ASTVar *GetVar3() const { return var3; }
    const ASTExpr *GetExpr3() const { return expr3; }
    const ASTNode *GetStmt() const { return stmt; }
};

/**
 * @brief A return statement may either return a value or not. Functions not declared void
 *  must return values. Functions declared void must not return values.
 * @param {bool} isVoid - true by default.
 * @param {ASTExpr*} expr
 */
class ASTReturnStmt : public ASTNode
{
    bool isVoid; // Whether the function type is void. True by default.
    ASTExpr *expr;

public:
    ASTReturnStmt() : isVoid(true), expr(NULL) { SetNodeType(ASTRETURNSTMT); }
    ~ASTReturnStmt() {}

    void AddExpr(ASTExpr *expr)
    {
        if (isVoid)
            isVoid = false;
        ASTReturnStmt::expr = expr;
    }

    bool IsVoid() const { return isVoid; }
    const ASTExpr *GetExpr() const { return expr; }
};

/**
 * @brief A var is either a simple (integer) variable or a subscripted array variable.
 *  A negative subscript causes the program to halt (unlike C).
 * @param {string} id - identifier.
 * @param {bool} haveSubscript - no subscript by default.
 * @param {ASTExpr*} subscript - pointer to the expression that evaluates the subscript.
 */
class ASTVar : public ASTNode
{
    string id;
    bool haveSubscript;
    ASTExpr *subscript;

public:
    ASTVar(string id) : id(id), haveSubscript(false), subscript(NULL) { SetNodeType(ASTVAR); }
    ASTVar(string id, ASTExpr *subscript) : id(id), haveSubscript(true), subscript(subscript) { SetNodeType(ASTVAR); }

    ~ASTVar() {}

    const string GetId() const { return id; }
    bool HaveSubscript() const { return haveSubscript; }
    const ASTExpr *GetExpr() const { return subscript; }
};

/**
 * @brief A simple expression consists of relational operators. The value of a simple expression
 *  is either the value of its additive expression if it contains no relational operators, or 1
 *  if the relational operator evaluates to true, or 0 if it evaluates to false.
 * @param {ASTAddExpr*} leftAddExpr - left additive-expression.
 * @param {bool} haveRightAddExpr - false by default.
 * @param {ASTRelOp} relOp - relational operator.
 * @param {ASTAddExpr*} rightAddExpr - right additive-expression.
 */
class ASTSimpleExpr : public ASTNode
{
    ASTAddExpr *leftAddExpr;
    bool haveRightAddExpr; // Is there a right additive-expression? By default it's false.
    ASTRelOp relOp;
    ASTAddExpr *rightAddExpr;

public:
    ASTSimpleExpr(ASTAddExpr *addExpr) : leftAddExpr(addExpr), haveRightAddExpr(false)
    {
        rightAddExpr = NULL;
        SetNodeType(ASTSIMPLEEXPR);
    }
    ASTSimpleExpr(ASTAddExpr *leftAddExpr, ASTRelOp relOp, ASTAddExpr *rightAddExpr)
        : leftAddExpr(leftAddExpr), haveRightAddExpr(true), relOp(relOp), rightAddExpr(rightAddExpr) { SetNodeType(ASTSIMPLEEXPR); }

    ~ASTSimpleExpr() {}

    const ASTAddExpr *GetLeftAddExpr() const { return leftAddExpr; }
    bool HaveRightAddExpr() const { return haveRightAddExpr; }
    ASTRelOp GetRelOp() const { return relOp; }
    const ASTAddExpr *GetRightAddExpr() const { return rightAddExpr; }
};

/**
 * @brief Additive expressions and terms represent the typical associativity and precedence of the arithmetic operators.
 * @param {ASTTerm*} firstTerm - first term.
 * @param {bool} areMultipleTerms - false by default.
 * @param {vector<ASTAddOp>} addOpList - addition operator list.
 * @param {vector<ASTTerm*>} termList - term list.
 */
class ASTAddExpr : public ASTNode
{
    ASTTerm *firstTerm;
    bool areMultipleTerms; // Are there multiple terms? By default it's false.
    vector<ASTAddOp> addOpList;
    vector<ASTTerm *> termList;

public:
    ASTAddExpr(ASTTerm *firstTerm) : firstTerm(firstTerm), areMultipleTerms(false) { SetNodeType(ASTADDEXPR); }
    ~ASTAddExpr() {}

    void AddTerm(ASTAddOp addOp, ASTTerm *term)
    {
        if (!areMultipleTerms)
            areMultipleTerms = true;
        addOpList.push_back(addOp);
        termList.push_back(term);
    }

    const ASTTerm *GetFirstTerm() const { return firstTerm; }
    bool AreMultipleTerms() const { return areMultipleTerms; }
    const vector<ASTAddOp> &GetAddOpList() const { return addOpList; }
    const vector<ASTTerm *> &GetTermList() const { return termList; }
};

/**
 * @brief Additive expressions and terms represent the typical associativity and precedence of the arithmetic operators.
 * @param {ASTFactor*} firstFactor - first factor.
 * @param {bool} areMultipleFactors - false by default.
 * @param {vector<ASTMulOp>} mulOpList - multiplication operator list.
 * @param {vector<ASTFactor*>} factorList - factor list.
 */
class ASTTerm : public ASTNode
{

    ASTFactor *firstFactor;
    bool areMultipleFactors; // Are there multiple factors? By default it's false.
    vector<ASTMulOp> mulOpList;
    vector<ASTFactor *> factorList;

public:
    ASTTerm(ASTFactor *firstFactor) : firstFactor(firstFactor), areMultipleFactors(false) { SetNodeType(ASTTERM); }
    ~ASTTerm() {}

    void AddFactor(ASTMulOp mulOp, ASTFactor *factor)
    {
        if (!areMultipleFactors)
            areMultipleFactors = true;
        mulOpList.push_back(mulOp);
        factorList.push_back(factor);
    }

    const ASTFactor *GetFirstFactor() const { return firstFactor; }
    bool AreMultipleFactors() const { return areMultipleFactors; }
    const vector<ASTMulOp> &GetMulOpList() const { return mulOpList; }
    const vector<ASTFactor *> &GetFactorList() const { return factorList; }
};

/**
 * @brief A factor is an expression enclosed in parentheses, a variable, which evaluates
 *  to the value of its variable; a call of a function, which evaluates to the returned
 *  value of the function; or a NUM, whose value is computed by the scanner.
 * @param {ASTExpr*} expr - NULL if is not expr.
 * @param {ASTVar*} var - NULL if is not var.
 * @param {ASTCall*} callExpr - NULL if is not callExpr.
 * @param {bool} isNum - false if is not num.
 * @param {ASTNUM} num - store the value of num.
 */
class ASTFactor : public ASTNode
{
    ASTExpr *expr;
    ASTVar *var;
    ASTCall *callExpr;
    bool isInt;
    int numInt;
    bool isReal;
    float numReal;

public:
    ASTFactor(ASTExpr *expr) : expr(expr), var(NULL), callExpr(NULL), isInt(false)
    {
        isReal = false;
        SetNodeType(ASTFACTOR);
    }
    ASTFactor(ASTVar *var) : expr(NULL), var(var), callExpr(NULL), isInt(false)
    {
        isReal = false;
        SetNodeType(ASTFACTOR);
    }
    ASTFactor(ASTCall *callExpr) : expr(NULL), var(NULL), callExpr(callExpr), isInt(false)
    {
        isReal = false;
        SetNodeType(ASTFACTOR);
    }
    ASTFactor(int num) : expr(NULL), var(NULL), callExpr(NULL), isInt(true), numInt(num), isReal(false) { SetNodeType(ASTFACTOR); }
    ASTFactor(float num) : expr(NULL), var(NULL), callExpr(NULL), isInt(false)
    {
        isReal = true;
        numReal = num;
        SetNodeType(ASTFACTOR);
    }

    ~ASTFactor() {}

    const ASTExpr *GetExpr() const { return expr; }
    const ASTVar *GetVar() const { return var; }
    const ASTCall *GetCallExpr() const { return callExpr; }
    bool IsInt() const { return isInt; }
    int GetNumInt() const { return numInt; }
    bool IsReal() const { return isReal; }
    float GetNumReal() const { return numReal; }
};

/**
 * @brief A function call consists of an ID (the name of the function), followed by its arguments.
 * @param {string} id - identifier of callee.
 * @param {vector<ASTExpr*>} argList - argument list.
 */
class ASTCall : public ASTNode
{
    string id;
    vector<ASTExpr *> argList;

public:
    ASTCall(string id) : id(id) { SetNodeType(ASTCALL); }
    ~ASTCall() {}

    void AddArg(ASTExpr *arg) { argList.push_back(arg); }

    const string GetId() const { return id; }
    const vector<ASTExpr *> &GetArgList() const { return argList; }
};

void PrintASTNode(const ASTNode *curNode);

/**
 * @brief Function call interface for printing AST
 * @param {ASTProgram*} program
 */
void PrintAST(const ASTProgram *program);

#endif