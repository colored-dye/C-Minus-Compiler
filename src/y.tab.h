/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    MYID = 258,
    MYNUM = 259,
    MYINT = 260,
    MYVOID = 261,
    MYREAL = 262,
    MYCHAR = 263,
    MYRETURN = 264,
    MYIF = 265,
    MYELSE = 266,
    MYWHILE = 267,
    MYFOR = 268,
    MYGT = 269,
    MYLT = 270,
    MYGE = 271,
    MYLE = 272,
    MYNE = 273,
    MYEQ = 274,
    MYADD = 275,
    MYSUB = 276,
    MYMUL = 277,
    MYDIV = 278,
    MYLP = 279,
    MYRP = 280,
    MYLB = 281,
    MYRB = 282,
    MYLC = 283,
    MYRC = 284,
    MYSEMI = 285,
    MYCOMMA = 286,
    MYASSIGN = 287,
    MYERROR = 288,
    LOWER_THAN_ELSE = 289,
    ELSE = 290
  };
#endif
/* Tokens.  */
#define MYID 258
#define MYNUM 259
#define MYINT 260
#define MYVOID 261
#define MYREAL 262
#define MYCHAR 263
#define MYRETURN 264
#define MYIF 265
#define MYELSE 266
#define MYWHILE 267
#define MYFOR 268
#define MYGT 269
#define MYLT 270
#define MYGE 271
#define MYLE 272
#define MYNE 273
#define MYEQ 274
#define MYADD 275
#define MYSUB 276
#define MYMUL 277
#define MYDIV 278
#define MYLP 279
#define MYRP 280
#define MYLB 281
#define MYRB 282
#define MYLC 283
#define MYRC 284
#define MYSEMI 285
#define MYCOMMA 286
#define MYASSIGN 287
#define MYERROR 288
#define LOWER_THAN_ELSE 289
#define ELSE 290

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 18 "parser.y"

  struct Node* node;
  int integer;
  float real;

#line 133 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
