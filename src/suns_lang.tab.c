
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 42 "suns_lang.y"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "trx/macros.h"
#include "trx/debug.h"
#include "trx/list.h"
#include "trx/buffer.h"
#include "suns_model.h"
#include "suns_parser.h"

    extern suns_parser_state_t _sps;

    suns_parser_state_t *sps = &_sps;

    int line_no = 1;
    char yyerror_buf[BUFFER_SIZE];

    void yyerror(const char *str)
    {
        error("line %d: %s\n", line_no, str);
    }
    
    int yywrap()
    {
        return 1;
    }



/* Line 189 of yacc.c  */
#line 105 "suns_lang.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 214 of yacc.c  */
#line 79 "suns_lang.y"

    unsigned int number_u;
    int number_i;
    float number_f;
    long int number_li;
    unsigned long int number_lu;
    double number_d;
    char *string;
    struct suns_type_pair *type_pair;
    struct suns_dp *dp;
    struct list *list;
    struct buffer *buffer;
    struct suns_model *model;
    struct suns_data *data;
    struct suns_value *value;
    struct suns_model_did *did;
    struct named_list *named_list;
    struct suns_define_block *define_block;
    struct suns_define *define;
    struct suns_dp_block *datapoints_block;



/* Line 214 of yacc.c  */
#line 247 "suns_lang.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 259 "suns_lang.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   63

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  42
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  42
/* YYNRULES -- Number of states.  */
#define YYNSTATES  75

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   296

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    13,    17,    20,
      23,    29,    30,    33,    36,    39,    42,    45,    50,    56,
      57,    60,    66,    73,    78,    84,    88,    92,    96,    98,
     100,   102,   104,   108,   112,   116,   120,   121,   124,   130,
     136,   142,   143
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      43,     0,    -1,    44,    -1,    -1,    44,    45,    -1,    49,
      -1,    57,    -1,    15,    24,    27,    -1,    17,    24,    -1,
      14,    27,    -1,    13,    28,     3,    50,     4,    -1,    -1,
      50,    51,    -1,    50,    46,    -1,    50,    47,    -1,    50,
      48,    -1,    50,    58,    -1,    16,     3,    52,     4,    -1,
      16,    28,     3,    52,     4,    -1,    -1,    52,    53,    -1,
      28,     3,    24,    54,     4,    -1,    28,     3,    24,    54,
      28,     4,    -1,    28,     3,    54,     4,    -1,    28,     3,
      54,    28,     4,    -1,    28,    11,    28,    -1,    28,    11,
      24,    -1,    28,    11,    25,    -1,    28,    -1,    24,    -1,
      25,    -1,    26,    -1,    54,     8,    25,    -1,    54,     8,
      24,    -1,    54,     8,    26,    -1,    54,     8,    27,    -1,
      -1,    56,    55,    -1,    22,    28,     3,    56,     4,    -1,
      20,    28,     3,    60,     4,    -1,    28,     3,    24,    27,
       4,    -1,    -1,    60,    59,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   129,   129,   131,   132,   134,   134,   136,   143,   148,
     153,   164,   167,   171,   179,   183,   187,   193,   201,   214,
     218,   224,   231,   237,   243,   260,   271,   288,   291,   305,
     311,   317,   330,   368,   406,   417,   427,   430,   443,   455,
     463,   472,   475
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OBRACE", "EBRACE", "OPAREN", "EPAREN",
  "EOL", "COLON", "SEMICOLON", "COMMENT", "DOT", "EQUAL", "MODELTOK",
  "NAMETOK", "DIDTOK", "DPTOK", "LENTOK", "ENUMTOK", "BFTOK", "DEFINETOK",
  "COMMENTTOK", "DATATOK", "CHARS", "UINT", "INT", "FLOAT", "STRING",
  "NAME", "INT16TOK", "UINT16TOK", "INT32TOK", "UINT32TOK", "INT64TOK",
  "UINT64TOK", "FLOAT32TOK", "FLOAT64TOK", "ENUM16TOK", "BITFIELD16TOK",
  "BITFIELD32TOK", "SFTOK", "STRINGTOK", "$accept", "config", "blocks",
  "block", "did", "len", "name", "model_block", "model_elmts",
  "datapoints_block", "suns_dp_list", "suns_dp", "suns_type", "suns_value",
  "register_list", "data_block", "define_block", "define", "defines", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    42,    43,    44,    44,    45,    45,    46,    47,    48,
      49,    50,    50,    50,    50,    50,    50,    51,    51,    52,
      52,    53,    53,    53,    53,    54,    54,    54,    54,    55,
      55,    55,    55,    55,    55,    55,    56,    56,    57,    58,
      59,    60,    60
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     3,     2,     2,
       5,     0,     2,     2,     2,     2,     2,     4,     5,     0,
       2,     5,     6,     4,     5,     3,     3,     3,     1,     1,
       1,     1,     3,     3,     3,     3,     0,     2,     5,     5,
       5,     0,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,     4,     5,     6,     0,
       0,    11,    36,     0,     0,    10,     0,     0,     0,     0,
       0,    13,    14,    15,    12,    16,    38,    29,    30,    31,
      28,     0,    37,     9,     0,    19,     0,     8,     0,     0,
       0,     7,     0,    19,    41,    26,    27,    25,    33,    32,
      34,    35,    17,     0,    20,     0,     0,     0,    18,    39,
       0,    42,     0,     0,     0,     0,    23,     0,     0,    21,
       0,    24,     0,    22,    40
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,     6,    21,    22,    23,     7,    13,    24,
      42,    54,    31,    32,    14,     8,    25,    61,    56
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -50
static const yytype_int8 yypact[] =
{
     -50,    16,   -12,   -50,   -19,   -11,   -50,   -50,   -50,    17,
      18,   -50,   -50,    28,    -2,   -50,    -8,    13,    -3,    14,
      11,   -50,   -50,   -50,   -50,   -50,   -50,   -50,   -50,   -50,
      29,    33,   -50,   -50,    19,   -50,    44,   -50,    46,   -13,
       9,   -50,    -1,   -50,   -50,   -50,   -50,   -50,   -50,   -50,
     -50,   -50,   -50,    47,   -50,     0,     1,   -10,   -50,   -50,
      48,   -50,    24,     2,    30,     3,   -50,    49,    31,   -50,
      51,   -50,    52,   -50,   -50
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -50,   -50,   -50,   -50,   -50,   -50,   -50,   -50,   -50,   -50,
      20,   -50,   -49,   -50,   -50,   -50,   -50,   -50,   -50
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      35,     4,    26,    52,    58,    59,    66,    69,    63,     9,
       5,    45,    46,    65,    62,    47,     3,    10,    30,    33,
      11,    12,    27,    28,    29,    36,    30,    53,    53,    60,
      67,    70,    15,    48,    49,    50,    51,    34,    37,    38,
      39,    40,    16,    17,    18,    19,    41,    43,    20,    44,
      57,    64,    30,    71,    68,    73,    74,     0,    72,     0,
       0,     0,     0,    55
};

static const yytype_int8 yycheck[] =
{
       3,    13,     4,     4,     4,     4,     4,     4,    57,    28,
      22,    24,    25,    62,    24,    28,     0,    28,    28,    27,
       3,     3,    24,    25,    26,    28,    28,    28,    28,    28,
      28,    28,     4,    24,    25,    26,    27,    24,    24,    28,
      11,     8,    14,    15,    16,    17,    27,     3,    20,     3,
       3,     3,    28,     4,    24,     4,     4,    -1,    27,    -1,
      -1,    -1,    -1,    43
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    43,    44,     0,    13,    22,    45,    49,    57,    28,
      28,     3,     3,    50,    56,     4,    14,    15,    16,    17,
      20,    46,    47,    48,    51,    58,     4,    24,    25,    26,
      28,    54,    55,    27,    24,     3,    28,    24,    28,    11,
       8,    27,    52,     3,     3,    24,    25,    28,    24,    25,
      26,    27,     4,    28,    53,    52,    60,     3,     4,     4,
      28,    59,    24,    54,     3,    54,     4,    28,    24,     4,
      28,     4,    27,     4,     4
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 7:

/* Line 1455 of yacc.c  */
#line 137 "suns_lang.y"
    {
    (yyval.did) = malloc(sizeof(suns_model_did_t));
    (yyval.did)->did = (yyvsp[(2) - (3)].number_u);
    (yyval.did)->name = (yyvsp[(3) - (3)].string);
}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 144 "suns_lang.y"
    {
    (yyval.number_u) = (yyvsp[(2) - (2)].number_u);
}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 149 "suns_lang.y"
    {
    (yyval.string) = (yyvsp[(2) - (2)].string);
}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 154 "suns_lang.y"
    {
    /* model_blocks are the same struct as model_elmts */
    (yyval.model) = (yyvsp[(4) - (5)].model);
    (yyval.model)->type = (yyvsp[(2) - (5)].string);

    /* add the model to the global model list */
    list_node_add(sps->model_list, list_node_new((yyval.model)));
}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 164 "suns_lang.y"
    {
    (yyval.model) = suns_model_new();
}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 168 "suns_lang.y"
    {
    list_node_add((yyval.model)->dp_blocks, list_node_new((yyvsp[(2) - (2)].datapoints_block)));
}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 172 "suns_lang.y"
    {
    (yyvsp[(2) - (2)].did)->model = (yyval.model);
    /* add the did to the global did_list */
    list_node_add(sps->did_list, list_node_new((yyvsp[(2) - (2)].did)));
    /* add the did to the list in the model */
    list_node_add((yyval.model)->did_list, list_node_new((yyvsp[(2) - (2)].did)));
}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 180 "suns_lang.y"
    {
    (yyval.model)->len = (yyvsp[(2) - (2)].number_u);
}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 184 "suns_lang.y"
    {
    (yyval.model)->name = (yyvsp[(2) - (2)].string);
}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 188 "suns_lang.y"
    {
    list_node_add((yyval.model)->defines, list_node_new((yyvsp[(2) - (2)].define_block)));
}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 194 "suns_lang.y"
    {
    /* FIXME: write suns_dp_block_new() */
    (yyval.datapoints_block) = malloc(sizeof(suns_dp_block_t));
    (yyval.datapoints_block)->feature = NULL;
    (yyval.datapoints_block)->repeating = 0;
    (yyval.datapoints_block)->dp_list = (yyvsp[(3) - (4)].list);
}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 202 "suns_lang.y"
    {
    (yyval.datapoints_block) = malloc(sizeof(suns_dp_block_t));
    /* FIXME: abstract this operation so we're not doing
       strcmp() here in the parser */
    if (strcmp((yyvsp[(2) - (5)].string), "repeating") == 0) {
        (yyval.datapoints_block)->repeating = 1;
    }
    (yyval.datapoints_block)->feature = (yyvsp[(2) - (5)].string);
    (yyval.datapoints_block)->dp_list = (yyvsp[(4) - (5)].list);
}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 214 "suns_lang.y"
    {
    /* create a new empty list */
    (yyval.list) = list_new();
}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 219 "suns_lang.y"
    {
    /* add the data point to the list */
    list_node_add((yyval.list), list_node_new((yyvsp[(2) - (2)].dp)));
}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 225 "suns_lang.y"
    {
    (yyval.dp)->name = strdup((yyvsp[(1) - (5)].string));
    (yyval.dp)->offset = (yyvsp[(3) - (5)].number_u);
    (yyval.dp)->type_pair = (yyvsp[(4) - (5)].type_pair);
}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 232 "suns_lang.y"
    {
    (yyval.dp)->name = strdup((yyvsp[(1) - (6)].string));
    (yyval.dp)->offset = (yyvsp[(3) - (6)].number_u);
    (yyval.dp)->type_pair = (yyvsp[(4) - (6)].type_pair);
}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 238 "suns_lang.y"
    {
    (yyval.dp)->name = strdup((yyvsp[(1) - (4)].string));
    (yyval.dp)->offset = 0;
    (yyval.dp)->type_pair = (yyvsp[(3) - (4)].type_pair);
}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 244 "suns_lang.y"
    {
    (yyval.dp)->name = strdup((yyvsp[(1) - (5)].string));
    (yyval.dp)->offset = 0;
    (yyval.dp)->type_pair = (yyvsp[(3) - (5)].type_pair);
}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 261 "suns_lang.y"
    {
    /* type with name subscript */
    (yyval.type_pair) = malloc(sizeof(suns_type_pair_t)); 
    (yyval.type_pair)->type = suns_type_from_name((yyvsp[(1) - (3)].string));
    if ((yyval.type_pair)->type == SUNS_UNDEF) {
        yyerror("invalid suns type");
        YYERROR;
    }
    (yyval.type_pair)->sub.name = (yyvsp[(3) - (3)].string);
}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 272 "suns_lang.y"
    {
    /* type with uint subscript - could be string or numeric type */
    (yyval.type_pair) = malloc(sizeof(suns_type_pair_t)); 
    (yyval.type_pair)->type = suns_type_from_name((yyvsp[(1) - (3)].string));
    if ((yyval.type_pair)->type == SUNS_UNDEF) {
        yyerror("invalid suns type");
        YYERROR;
    }
    switch ((yyval.type_pair)->type) {
    case SUNS_STRING:
        (yyval.type_pair)->sub.len = (yyvsp[(3) - (3)].number_u);
        break;
    default:
        (yyval.type_pair)->sub.sf  = (yyvsp[(3) - (3)].number_u);
    }
}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 289 "suns_lang.y"
    {
}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 292 "suns_lang.y"
    {
    /* simple type with no subscript */
    (yyval.type_pair) = malloc(sizeof(suns_type_pair_t));
    (yyval.type_pair)->type = suns_type_from_name((yyvsp[(1) - (1)].string));
    if ((yyval.type_pair)->type == SUNS_UNDEF) {
        yyerror("invalid suns type");
        YYERROR;
    }
    (yyval.type_pair)->sub.name = NULL;
}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 306 "suns_lang.y"
    {
    debug("UINT %d", (yyvsp[(1) - (1)].number_u));
    (yyval.value) = suns_value_new();
    suns_value_set_uint16((yyval.value), (yyvsp[(1) - (1)].number_u));
}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 312 "suns_lang.y"
    {
    debug("INT %d", (yyvsp[(1) - (1)].number_i));
    (yyval.value) = suns_value_new();
    suns_value_set_int16((yyval.value), (yyvsp[(1) - (1)].number_i));
}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 318 "suns_lang.y"
    {
    debug("FLOAT %f", (yyvsp[(1) - (1)].number_f));
    (yyval.value) = suns_value_new();
    suns_value_set_float32((yyval.value), (yyvsp[(1) - (1)].number_f));
}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 331 "suns_lang.y"
    {
    (yyval.value) = suns_value_new();
    switch ((yyvsp[(1) - (3)].type_pair)->type) {
    case SUNS_INT16:
        suns_value_set_int16((yyval.value), (yyvsp[(3) - (3)].number_i));
        break;
    case SUNS_INT32:
        suns_value_set_int32((yyval.value), (yyvsp[(3) - (3)].number_i));
        break;
    case SUNS_UINT16:
        suns_value_set_uint16((yyval.value), (yyvsp[(3) - (3)].number_i));
        break;
    case SUNS_UINT32:
        suns_value_set_uint32((yyval.value), (yyvsp[(3) - (3)].number_i));
        break;
    case SUNS_FLOAT32:
        suns_value_set_float32((yyval.value), (yyvsp[(3) - (3)].number_i));
        break;
    case SUNS_ENUM16:
        suns_value_set_enum16((yyval.value), (yyvsp[(3) - (3)].number_i));
        break;
    case SUNS_BITFIELD16:
        suns_value_set_bitfield16((yyval.value), (yyvsp[(3) - (3)].number_i));
        break;
    case SUNS_BITFIELD32:
        suns_value_set_bitfield32((yyval.value), (yyvsp[(3) - (3)].number_i));
        break;
    case SUNS_SF:
        suns_value_set_sunssf((yyval.value), (yyvsp[(3) - (3)].number_i));
        break;
    default:
        snprintf(yyerror_buf, BUFFER_SIZE,
                 "unsupported type with int literal: %s",
                 suns_type_string((yyvsp[(1) - (3)].type_pair)->type));
        yyerror(yyerror_buf);
    }
}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 369 "suns_lang.y"
    {
    (yyval.value) = suns_value_new();
    switch ((yyvsp[(1) - (3)].type_pair)->type) {
    case SUNS_INT16:
        suns_value_set_int16((yyval.value), (yyvsp[(3) - (3)].number_u));
        break;
    case SUNS_INT32:
        suns_value_set_int32((yyval.value), (yyvsp[(3) - (3)].number_u));
        break;
    case SUNS_UINT16:
        suns_value_set_uint16((yyval.value), (yyvsp[(3) - (3)].number_u));
        break;
    case SUNS_UINT32:
        suns_value_set_uint32((yyval.value), (yyvsp[(3) - (3)].number_u));
        break;
    case SUNS_FLOAT32:
        suns_value_set_float32((yyval.value), (yyvsp[(3) - (3)].number_u));
        break;
    case SUNS_ENUM16:
        suns_value_set_enum16((yyval.value), (yyvsp[(3) - (3)].number_u));
        break;
    case SUNS_BITFIELD16:
        suns_value_set_bitfield16((yyval.value), (yyvsp[(3) - (3)].number_u));
        break;
    case SUNS_BITFIELD32:
        suns_value_set_bitfield32((yyval.value), (yyvsp[(3) - (3)].number_u));
        break;
    case SUNS_SF:
        suns_value_set_sunssf((yyval.value), (yyvsp[(3) - (3)].number_u));
        break;
    default:
        snprintf(yyerror_buf, BUFFER_SIZE,
                 "unsupported type with int literal: %s",
                 suns_type_string((yyvsp[(1) - (3)].type_pair)->type));
        yyerror(yyerror_buf);
    }
}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 407 "suns_lang.y"
    {
    (yyval.value) = suns_value_new();
    switch ((yyvsp[(1) - (3)].type_pair)->type) {
    case SUNS_FLOAT32:
        suns_value_set_float32((yyval.value), (yyvsp[(3) - (3)].number_f));
        break;
    default:
        yyerror("can only use float32 with a float literal");
    }
}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 418 "suns_lang.y"
    {
    (yyval.value) = suns_value_new();
    if ((yyvsp[(1) - (3)].type_pair)->type != SUNS_STRING)
        yyerror("string literal can only be declared as a string value");
    suns_value_set_string((yyval.value), (yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].type_pair)->sub.len);
}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 427 "suns_lang.y"
    {
    (yyval.buffer) = buffer_new(BUFFER_SIZE);
}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 431 "suns_lang.y"
    {
    debug("suns_value type = %s(%d)",
          suns_type_string((yyvsp[(2) - (2)].value)->tp.type), (yyvsp[(2) - (2)].value)->tp.type);
    debug("buffer_space($$) = %d, suns_type_size($2->tp.type) = %d", buffer_space((yyval.buffer)), suns_type_size((yyvsp[(2) - (2)].value)->tp.type));
    if (buffer_space((yyval.buffer)) < suns_type_pair_size(&((yyvsp[(2) - (2)].value)->tp))) {
        debug("resizing buffer!");
        buffer_resize((yyval.buffer), buffer_len((yyval.buffer)) + BUFFER_SIZE);
    }
    suns_value_to_buffer((yyval.buffer), (yyvsp[(2) - (2)].value));
}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 444 "suns_lang.y"
    {
    suns_data_block_t *d = malloc(sizeof(suns_data_block_t));

    d->name = strdup((yyvsp[(2) - (5)].string));
    d->data = (yyvsp[(4) - (5)].buffer);
    
    /* add the data block to the global list */
    list_node_add(sps->data_block_list, list_node_new(d));
}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 456 "suns_lang.y"
    {
    (yyval.define_block) = malloc(sizeof(suns_define_block_t));
    (yyval.define_block)->name = (yyvsp[(2) - (5)].string);
    (yyval.define_block)->list = (yyvsp[(4) - (5)].list);
}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 464 "suns_lang.y"
    {
    (yyval.define) = malloc(sizeof(suns_define_t));
    (yyval.define)->name = (yyvsp[(1) - (5)].string);
    (yyval.define)->value = (yyvsp[(3) - (5)].number_u);
    (yyval.define)->string = (yyvsp[(4) - (5)].string);
}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 472 "suns_lang.y"
    {
    (yyval.list) = list_new();
}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 476 "suns_lang.y"
    {
    list_node_add((yyval.list), list_node_new((yyvsp[(2) - (2)].define)));
}
    break;



/* Line 1455 of yacc.c  */
#line 2001 "suns_lang.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 483 "suns_lang.y"



