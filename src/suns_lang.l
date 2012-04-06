/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_lang.l
 * $Id: $
 *
 * flex (lex) lexer for the sunspec model description language.
 *
 * Copyright (c) 2011, John D. Blair <jdb@moship.net>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *     * Neither the name of John D. Blair nor his lackeys may be used
 *       to endorse or promote products derived from this software
 *       without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * JOHN D. BLAIR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


%{
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "trx/string.h"
#include "suns_lang.tab.h"
#include "suns_parser.h"

extern int line_no;
    
    
%}


%%
model             { return MODELTOK; }
name              { return NAMETOK; }
did               { return DIDTOK; }
len               { return LENTOK; }
datapoints        { return DPTOK; }
define            { return DEFINETOK; }
enum              { return ENUMTOK; }
enumerated        { return ENUMTOK; }
bitfield          { return BFTOK; }
comment		      { return COMMENTTOK; }
data              { return DATATOK; }
\"[^\"\n]*\"      {
                    yylval.string = string_trim_quotes(strdup(yytext));
                    return STRING;
                  }
\'[^\'\n]*\'      {
                    yylval.string = string_trim_quotes(strdup(yytext));
                    return CHARS;
                  }
[\-+][0-9]+       {
                    if (sscanf(yytext, "%lld", &(yylval.number_i)) != 1) {
                      yyerror("'%s' is not a valid integer");
                    }
                    return INT;
                  }
[0-9]+            {
                    if (sscanf(yytext, "%llu", &(yylval.number_u)) != 1) {
                      yyerror("'%s' is not a valid unsigned integer");
                    }
                    return UINT;
                  }
0x[0-9A-Fa-f]+    {
                    if (sscanf(yytext, "%llx", &(yylval.number_u)) != 1) {
                      yyerror("'%s' is not a valid unsigned integer");
                    }
                    return UINT;
                  }
[\-+]?[0-9]+\.[0-9]+ {
                    if (sscanf(yytext, "%lf", &(yylval.number_f)) != 1) {
                      yyerror("'%s' is not a valid float");
                    }
                    return FLOAT;
                  }
[a-zA-Z][a-zA-Z0-9_\-,]*     { yylval.string = strdup(yytext); return NAME; }
\{                { return OBRACE; }
\}                { return EBRACE; }
\(                { return OPAREN; }
\)                { return EPAREN; }
#[^\n]*\n         /* ignore comments */;
\:                { return COLON; }
=                 { return EQUAL; }
\;                { return SEMICOLON; }
\.                { return DOT; }
<<EOF>>           { return EOF; }
\n                { line_no++; }
[ \t]+            /* ignore whitespace */;
%%

