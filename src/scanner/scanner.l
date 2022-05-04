/*
 * @Author: SiO-2
 * @Date: 2022-04-19 22:36:55
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-04-20 21:14:51
 * @FilePath: /C-Minus-Compiler/src/scanner/scanner.l
 * @Description: 
 * 
 * Copyright (c) 2022 by SiO-2, All Rights Reserved. 
 */
%{
#include "syntax.tab.h"
//#define DEBUGNOW
//#define DEBUGBISONNOW
int lexError=0;
extern int emptyflag;

typedef enum {
    LEX_INT=0,
    LEX_FLOAT=1,
    LEX_ID=2,
    LEX_TYPE=3,
    OTHERS
}NODE_TYPE;

void infoDebug(char *str){
#ifdef DEBUGNOW
    printf("debug:Line %d :input  %s : %s\n",yylineno,str,yytext);
#endif
}

// 参考结构 https://segmentfault.com/a/1190000002451356
struct Node{
    struct Node *child;//第一个孩子;
    struct Node* next_sib;//下一个兄弟节点
    char name[32];//节点名称
    union{
        int int_contant;
        float float_contant;
        char string_contant[32];//包含的内容
    };
    int place;//节点的类型,0表示lexical里面的,1表示syntax里面的;
    int column;//表示行数;
    NODE_TYPE type;// 0 表示int,1表示float,2表示ID,3表示type;
};
struct Node* root;

// 注意%locations应该要提前在.y文件注明https://stackoverflow.com/questions/43419585/how-to-resolve-a-yylloc-undeclared-error
void addLexNode(char * Name,NODE_TYPE Type);
    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
%}
%option yylineno
letter_         [A-Za-z_]
digit           [0-9]
digits          [0-9]+
OCT             0[0-7]+
OCTERROR        0[0-7]*[8-9]+[0-9]*
HEX             0[xX][0-9a-fA-F]+
HEXERROR        0[xX][0-9a-fA-F]*[g-zG-Z]+[0-9a-zA-Z]*
DEC             0|([1-9][0-9]*)
DECERROR        [0-9]+[a-wA-Wy-zY-Z]+[0-9a-dA-Df-zF-Z]*|[0-9]+[0-9]+[a-dA-Df-zF-Z]+[0-9]*[0-9a-dA-Df-zF-Z]*
FLOAT           {digits}\.{digits}|(\.{digits}|{digits}\.|{digits}\.{digits}|{digits})([eE][+-]?{digits})
FLOATERROR      (\.{digits}|{digits}\.|{digits}\.{digits})([eE][+-]?)
ID              {letter_}({letter_}|{digit})*
TYPE            int|float
LP              "("
RP              ")"
LB              "["
RB              "]"
LC              "{"
RC              "}"
LCOMMENT        "/*"
COMMENT         "//"
RELOP           >|<|>=|<=|==|!=
WhiteSpace      [ \t\r\n]+
%%
{HEX} 		{
			infoDebug("HEX");
                	addLexNode("INT",LEX_INT);
                	sscanf(yytext,"%x",&yylval.node->int_contant);
                	// printf("%d,0x%x\n",yylval.node->int_contant,yylval.node->int_contant);
                	emptyflag=1;
               		return INT;
		}
{HEXERROR}      {
                        printf("Error type A at Line %d: Illegal hexadecimal number\'%s\'.\n",yylineno, yytext);
                        lexError+=1;
                        addLexNode("INT",LEX_INT);
                        yylval.node->int_contant=0;//暂时捕获成0,反正是错的
                        emptyflag=1;
                        return INT;
                }
{OCT}           {
                        infoDebug("OCT");
                        addLexNode("INT",LEX_INT);
                        sscanf(yytext,"%o",&yylval.node->int_contant);
                        // printf("%d,%o\n",yylval.node->int_contant,yylval.node->int_contant);
                        emptyflag=1;
                        return INT;
                }
{OCTERROR}      {
                        printf("Error type A at Line %d: Illegal octal number\'%s\'.\n",yylineno, yytext);
                        lexError+=1;
                        addLexNode("INT",LEX_INT);
                        yylval.node->int_contant=0;     // 暂时捕获成0,反正是错的
                        emptyflag=1;
                        return INT;
                }
{DEC}           {
                        infoDebug("DEC");
                        addLexNode("INT",LEX_INT);
                        sscanf(yytext,"%d",&yylval.node->int_contant);
                        emptyflag=1;
                        return INT;
                }
{DECERROR}      {
                        printf("Error type A at Line %d: Illegal number\'%s\'.\n",yylineno, yytext);
                        lexError+=1;
                        addLexNode("INT",LEX_INT);
                        yylval.node->int_contant=0;
                        emptyflag=1;
                        return INT;
                }
{FLOAT}         {
                        infoDebug("FLOAT");
                        addLexNode("FLOAT",LEX_FLOAT);
                        sscanf(yytext,"%f",&yylval.node->float_contant);
                        // printf("%f,%s\n",yylval.node->float_contant,yytext);
                        emptyflag=1;
                        return FLOAT;
                }
{FLOATERROR}    {
                        printf("Error type A at Line %d: Illegal floating point number\'%s\'.\n",yylineno, yytext);
                        addLexNode("FLOAT",LEX_FLOAT);
                        yylval.node->float_contant=0;
                        lexError+=1;
                        emptyflag=1;
                        return FLOAT;
                        // printf("To be done !FLOATERROR\n");
                }
{LCOMMENT}      { 
                        // printf("LCOMMENT\n");
                        // printf("%d",yylloc.first_line);
                        // if(yylloc.first_line==1){
                        //      // printf("here");
                        //      yylloc.first_line=yylineno;
                        // }
                        char a=input();
                        char b=input();
                        while(a!='*'||b!='/'){
                                // printf("%c,%c\n",a,b);
                                a=b;
                                b=input();
                                if(b=='\0'){ 
                                        printf("Error type A at Line %d: Missing */.\n",yylineno);
                                        break;
                                }
                        }
                        // printf("%d",yylloc.first_line);    
                }
{COMMENT}       {
                        char a=input();
                        while(a!='\n')
                                a=input();
                        // printf("COMMENT\n");
                }
";"             {
                        infoDebug("SEMI");
                        addLexNode("SEMI",OTHERS);    
                        emptyflag=1;
                        return SEMI;
                }
","             {
                        infoDebug("comma");
                        addLexNode("COMMA",OTHERS);
                        emptyflag=1;
                        return COMMA;    
                }
"="             {
                        infoDebug("assignop");
                        addLexNode("ASSIGNOP",OTHERS);
                        emptyflag=1;
                        return ASSIGNOP;
                }
{RELOP}         {
                        infoDebug("RELOP");
                        addLexNode("RELOP",OTHERS);
                        emptyflag=1;
                        return RELOP;
                }
"+"             {
                        infoDebug("plus");
                        addLexNode("PLUS",OTHERS);
                        emptyflag=1;
                        return PLUS;
                }
"-"             {
                        infoDebug("minus");
                        addLexNode("MINUS",OTHERS);
                        emptyflag=1;
                        return MINUS;
                }
"*"             {
                        infoDebug("star");
                        addLexNode("STAR",OTHERS);
                        emptyflag=1;
                        return STAR;
                }
"/"             {
                        infoDebug("div");
                        addLexNode("DIV",OTHERS);
                        emptyflag=1;
                        return DIV;
                }
"&&"            {
                        infoDebug("and");
                        addLexNode("AND",OTHERS);
                        emptyflag=1;
                        return AND;
                }
"||"            {
                        infoDebug("or");
                        addLexNode("OR",OTHERS);
                        emptyflag=1;
                        return OR;
                }
"."             {
                        infoDebug("dot");
                        addLexNode("DOT",OTHERS);
                        emptyflag=1;
                        return DOT;
                }
"!"             {
                        infoDebug("not");
                        addLexNode("NOT",OTHERS);
                        emptyflag=1;
                        return NOT;
                }
{TYPE}          {
                        infoDebug("type");
                        emptyflag=1;
                        addLexNode("TYPE",LEX_TYPE);
                        strcpy(yylval.node->string_contant,yytext);
                        // printf("%s\n",yylval.node->string_contant);
                        return TYPE;
                }
{LP}            {
                        infoDebug("LP");
                        emptyflag=1;
                        addLexNode("LP",OTHERS);
                        return LP;
                }
{RP}            {
                        infoDebug("RP");
                        emptyflag=1;
                        addLexNode("RP",OTHERS);
                        return RP;
                }
{LB}            {
                        infoDebug("LB");
                        emptyflag=1;
                        addLexNode("LB",OTHERS);
                        return LB;
                }
{RB}            {
                        infoDebug("RB");
                        emptyflag=1;
                        addLexNode("RB",OTHERS);
                        return RB;
                }
{LC}            {
                        infoDebug("LC");
                        emptyflag=1;
                        addLexNode("LC",OTHERS);
                        return LC;
                }
{RC}            {
                        infoDebug("RC");
                        emptyflag=1;
                        addLexNode("RC",OTHERS);
                        return RC;
                }
"struct"        {
                        infoDebug("STRUCT");
                        emptyflag=1;
                        addLexNode("STRUCT",OTHERS);
                        return STRUCT;
                }
"return"        {
                        infoDebug("return");
                        emptyflag=1;
                        addLexNode("RETURN",OTHERS);
                        return RETURN;
                }
"if"            {
                        infoDebug("if");
                        emptyflag=1;
                        addLexNode("IF",OTHERS);
                        return IF;
                }
"else"          {
                        infoDebug("else");
                        emptyflag=1;
                        addLexNode("ELSE",OTHERS);
                        return ELSE;
                }
"while"         {
                        infoDebug("while");
                        emptyflag=1;
                        addLexNode("WHILE",OTHERS);
                        return WHILE;
                }
{ID}            {
                        infoDebug("ID");
                        emptyflag=1;
                        addLexNode("ID",LEX_ID);
                        strcpy(yylval.node->string_contant,yytext);
                        // printf("%s\n",yylval.node->string_contant);
                        return ID;
                }
"\n"            {
                        yycolumn=1;
                }
{WhiteSpace}+   {   
                        ; /*Let it go!*/      
                }
.               {
                        lexError+=1;
                        printf("Error type A at Line %d: Mysterious characters \'%s\'.\n",yylineno, yytext);
                }
%%
int yywrap()
{ 
        return 1;
}
void addLexNode(char * Name,NODE_TYPE Type){
        // printf("In add lexnode!!\n");
        yylval.node=(struct Node*)malloc(sizeof(struct Node));
        yylval.node->child=NULL;
        yylval.node->next_sib=NULL;
        yylval.node->column=yylineno;
        strcpy(yylval.node->name,Name);
        yylval.node->place=0;
        yylval.node->type=Type;
}