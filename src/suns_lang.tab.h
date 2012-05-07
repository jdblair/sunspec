
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     OBRACE = 258,
     EBRACE = 259,
     OPAREN = 260,
     EPAREN = 261,
     EOL = 262,
     COLON = 263,
     SEMICOLON = 264,
     COMMENT = 265,
     DOT = 266,
     EQUAL = 267,
     MODELTOK = 268,
     NAMETOK = 269,
     DIDTOK = 270,
     DPTOK = 271,
     LENTOK = 272,
     ENUMTOK = 273,
     BFTOK = 274,
     DEFINETOK = 275,
     COMMENTTOK = 276,
     DATATOK = 277,
     CHARS = 278,
     UINT = 279,
     INT = 280,
     FLOAT = 281,
     STRING = 282,
     NAME = 283,
     INT16TOK = 284,
     UINT16TOK = 285,
     INT32TOK = 286,
     UINT32TOK = 287,
     INT64TOK = 288,
     UINT64TOK = 289,
     FLOAT32TOK = 290,
     FLOAT64TOK = 291,
     ENUM16TOK = 292,
     BITFIELD16TOK = 293,
     BITFIELD32TOK = 294,
     SFTOK = 295,
     STRINGTOK = 296
   };
#endif
/* Tokens.  */
#define OBRACE 258
#define EBRACE 259
#define OPAREN 260
#define EPAREN 261
#define EOL 262
#define COLON 263
#define SEMICOLON 264
#define COMMENT 265
#define DOT 266
#define EQUAL 267
#define MODELTOK 268
#define NAMETOK 269
#define DIDTOK 270
#define DPTOK 271
#define LENTOK 272
#define ENUMTOK 273
#define BFTOK 274
#define DEFINETOK 275
#define COMMENTTOK 276
#define DATATOK 277
#define CHARS 278
#define UINT 279
#define INT 280
#define FLOAT 281
#define STRING 282
#define NAME 283
#define INT16TOK 284
#define UINT16TOK 285
#define INT32TOK 286
#define UINT32TOK 287
#define INT64TOK 288
#define UINT64TOK 289
#define FLOAT32TOK 290
#define FLOAT64TOK 291
#define ENUM16TOK 292
#define BITFIELD16TOK 293
#define BITFIELD32TOK 294
#define SFTOK 295
#define STRINGTOK 296




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 82 "suns_lang.y"

    double number_f;
    int64_t number_i;
    uint64_t number_u;
    char *string;
    struct suns_type_pair *type_pair;
    struct suns_dp *dp;
    struct list *list;
    struct buffer *buffer;
    struct suns_model *model;
    struct suns_data *data;
    struct suns_value *value;
    struct suns_model_did *did;
    struct suns_define_block *define_block;
    struct suns_define *define;
    struct suns_dp_block *datapoints_block;
    struct suns_data_block *data_block;
    struct suns_attribute *attribute;



/* Line 1676 of yacc.c  */
#line 156 "suns_lang.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


