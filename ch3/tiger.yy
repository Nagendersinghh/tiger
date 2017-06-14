%{
#include <stdio.h>
#include "util.h"
#include "errormsg.h"

int yylex(void); /* function prototype */

void yyerror(char *s) {
	EM_error(EM_tokPos, "%s", s);
}
%}


%union {
	int pos;
	int ival;
	string sval;
	}

%token <sval> ID STRING
%token <ival> INT

%token
	COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK
	LBRACE RBRACE DOT
	PLUS MINUS TIMES DIVIDE EQ NEQ LT LE GT GE
	AND OR ASSIGN
	ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF
	BREAK NIL
	FUNCTION VAR TYPE

%start program

%nonassoc DO OF
%nonassoc THEN /* ELSE must come after THEN! */
%nonassoc ELSE
%left SEMICOLON
%left ASSIGN
%left OR
%left AND
%nonassoc EQ NEQ GT LT GE LE
%left PLUS MINUS
%left TIMES DIVIDE
%left UMINUS

%%

program: expression

expression
	: INT
	| STRING
	| lvalue
	| NIL
	| LPAREN expression_sequence RPAREN
	| arithmetic_expression
	| selection
	| comparison
	| boolean
	| MINUS expression %prec UMINUS
	| function_call
	| record_creation
	| array_creation
	| assignment
	| loop
	| BREAK
	| LPAREN RPAREN
	| LET declaration_sequence IN END
	| LET declaration_sequence IN expression_sequence END

declaration_sequence
	:
	| declaration declaration_sequence

declaration
	: type_declaration
	| var_declaration
	| function_declaration

type_declaration
	: TYPE ID EQ type

type
	: ID
	| LBRACE tyfields RBRACE
	| ARRAY OF ID

tyfields
	:
	| type_field_sequence

type_field_sequence
	: ID COLON ID
	| type_field_sequence COMMA ID COLON ID

var_declaration
	: VAR ID ASSIGN expression
	| VAR ID COLON ID ASSIGN expression

function_declaration
	: FUNCTION ID LPAREN tyfields RPAREN EQ expression
	| FUNCTION ID LPAREN tyfields RPAREN COLON ID EQ expression

lvalue
	: ID lvalue_ext
lvalue_ext
	:
	| DOT ID lvalue_ext
	| LBRACK expression RBRACK lvalue_ext


expression_sequence
	: expression
	| expression_sequence SEMICOLON expression


arithmetic_expression
	: expression PLUS expression
	| expression MINUS expression
	| expression TIMES expression
	| expression DIVIDE expression

comparison
	: expression EQ expression
	| expression NEQ expression
	| expression GT expression
	| expression LT expression
	| expression GE expression
	| expression LE expression

boolean
	: expression OR expression
	| expression AND expression

selection
	: IF expression THEN expression ELSE expression
	| IF expression THEN expression

function_call
	: ID LPAREN expression_list RPAREN

expression_list
	:
	| expression exp_list_ext

exp_list_ext
	:
	| COMMA expression exp_list_ext

record_creation
	: ID LBRACE field_list RBRACE

field_list
	:
	| ID EQ expression
	| field_list COMMA ID EQ expression

array_creation
	: ID LBRACK expression RBRACK OF expression

assignment
	: lvalue ASSIGN expression

loop
	: WHILE expression DO expression
	| FOR ID ASSIGN expression TO expression DO expression
