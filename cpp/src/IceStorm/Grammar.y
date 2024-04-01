%code top{

//
// Copyright (c) ZeroC, Inc. All rights reserved.
//

}

%code requires{

// I must set the initial stack depth to the maximum stack depth to
// disable bison stack resizing. The bison stack resizing routines use
// simple malloc/alloc/memcpy calls, which do not work for the
// YYSTYPE, since YYSTYPE is a C++ type, with constructor, destructor,
// assignment operator, etc.
#define YYMAXDEPTH  10000      // 10000 should suffice. Bison default is 10000 as maximum.
#define YYINITDEPTH YYMAXDEPTH // Initial depth is set to max depth, for the reasons described above.

// Newer bison versions allow to disable stack resizing by defining yyoverflow.
#define yyoverflow(a, b, c, d, e, f) yyerror(a)

}

%code{

// Forward declaration of the lexing function generated by flex, so bison knows about it.
// This must match the definition of 'yylex' in the generated scanner.
int yylex(YYSTYPE* yylvalp);

}

%{

#include "Ice/Ice.h"
#include "Parser.h"

#ifdef _MSC_VER
// warning C4102: 'yyoverflowlab' : unreferenced label
#   pragma warning(disable:4102)
#endif

// Avoid old style cast warnings in generated grammar
#ifdef __GNUC__
#    pragma GCC diagnostic ignored "-Wold-style-cast"
#    pragma GCC diagnostic ignored "-Wunused-label"
#endif

// Avoid clang warnings in generated grammar
#if defined(__clang__)
#    pragma clang diagnostic ignored "-Wunused-but-set-variable"
#    pragma clang diagnostic ignored "-Wunused-label"
#endif

using namespace std;
using namespace Ice;
using namespace IceStorm;

void
yyerror(const char* s)
{
    parser->error(s);
}

%}

// Directs Bison to generate a re-entrant parser.
%define api.pure
// Specifies what type to back the tokens with (their semantic values).
%define api.value.type {std::list<std::string>}

// All keyword tokens. Make sure to modify the "keyword" rule in this
// file if the list of keywords is changed. Also make sure to add the
// keyword to the keyword table in Scanner.l.
%token ICE_STORM_HELP
%token ICE_STORM_EXIT
%token ICE_STORM_CURRENT
%token ICE_STORM_CREATE
%token ICE_STORM_DESTROY
%token ICE_STORM_LINK
%token ICE_STORM_UNLINK
%token ICE_STORM_LINKS
%token ICE_STORM_TOPICS
%token ICE_STORM_REPLICA
%token ICE_STORM_SUBSCRIBERS
%token ICE_STORM_STRING

%%

// ----------------------------------------------------------------------
start
// ----------------------------------------------------------------------
: commands
{
}
| %empty
{
}
;

// ----------------------------------------------------------------------
commands
// ----------------------------------------------------------------------
: commands command
{
}
| command
{
}
;

// ----------------------------------------------------------------------
command
// ----------------------------------------------------------------------
: ICE_STORM_HELP ';'
{
    parser->usage();
}
| ICE_STORM_EXIT ';'
{
    return 0;
}
| ICE_STORM_CREATE strings ';'
{
    parser->create($2);
}
| ICE_STORM_CURRENT strings ';'
{
    parser->current($2);
}
| ICE_STORM_DESTROY strings ';'
{
    parser->destroy($2);
}
| ICE_STORM_LINK strings ';'
{
    parser->link($2);
}
| ICE_STORM_UNLINK strings ';'
{
    parser->unlink($2);
}
| ICE_STORM_LINKS strings ';'
{
    parser->links($2);
}
| ICE_STORM_TOPICS strings ';'
{
    parser->topics($2);
}
| ICE_STORM_REPLICA strings ';'
{
    parser->replica($2);
}
| ICE_STORM_SUBSCRIBERS strings ';'
{
    parser->subscribers($2);
}
| ICE_STORM_STRING error ';'
{
    parser->invalidCommand("unknown command `" + $1.front() + "' (type `help' for more info)");
}
| error ';'
{
    yyerrok;
}
| ';'
{
}
;

// ----------------------------------------------------------------------
strings
// ----------------------------------------------------------------------
: ICE_STORM_STRING strings
{
    $$ = $2;
    $$.push_front($1.front());
}
| keyword strings
{
    $$ = $2;
    $$.push_front($1.front());
}
| %empty
{
    $$ = YYSTYPE();
}
;

// ----------------------------------------------------------------------
keyword
// ----------------------------------------------------------------------
: ICE_STORM_HELP
{
}
| ICE_STORM_EXIT
{
}
| ICE_STORM_CURRENT
{
}
| ICE_STORM_CREATE
{
}
| ICE_STORM_DESTROY
{
}
| ICE_STORM_LINK
{
}
| ICE_STORM_UNLINK
{
}
| ICE_STORM_LINKS
{
}
| ICE_STORM_TOPICS
{
}

%%
