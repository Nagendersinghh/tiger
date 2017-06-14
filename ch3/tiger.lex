%{
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "y.tab.h"
#include "errormsg.h"

int charPos=1;

int commentNesting = 0;

int yywrap(void) {
	charPos=1;
	return 1;
}

void adjust(void) {
	EM_tokPos=charPos;
	charPos+=yyleng;
}
/* Helper variables for string operations */
const int INITIAL_BUFFER_LENGTH = 32;
char *string_buffer;
unsigned int string_buffer_capacity;
void init_string_buffer() {
	string_buffer = checked_malloc(INITIAL_BUFFER_LENGTH);
	string_buffer[0] = 0;
	string_buffer_capacity = INITIAL_BUFFER_LENGTH;
}

void append_char_to_stringbuffer(char ch) {
	size_t len = strlen(string_buffer) + 1; // +1 for NULL
	// Check if we have enough memory to store the character, if not
	// allocate memory again.
	if (len == string_buffer_capacity) {
		char *temp;
		string_buffer_capacity *= 2;
		temp = checked_malloc(string_buffer_capacity);
		memcpy(temp, string_buffer, len);
		free(string_buffer);
		string_buffer = temp;
	}
	string_buffer[len -1] = ch;
	string_buffer[len] = 0;
}

%}
%x COMMENT STRING_STATE
%%
	/* skip spaces, tabs and carriage returns */
[ \r\t]		{adjust(); continue;}
<INITIAL,COMMENT>\n	{
		adjust();
		EM_newline();
		continue;
		}
	/* Reserved words of the language. */
while     {adjust(); return WHILE;}
for       {adjust(); return FOR;}
to        {adjust(); return TO;}
break     {adjust(); return BREAK;}
let       {adjust(); return LET;}
in        {adjust(); return IN;}
end       {adjust(); return END;}
function  {adjust(); return FUNCTION;}
var       {adjust(); return VAR;}
type      {adjust(); return TYPE;}
array     {adjust(); return ARRAY;}
if        {adjust(); return IF;}
then      {adjust(); return THEN;}
else      {adjust(); return ELSE;}
do        {adjust(); return DO;}
of        {adjust(); return OF;}
nil       {adjust(); return NIL;}

	/* Punctuation symbols of the language. */
","   {adjust(); return COMMA;}
":"   {adjust(); return COLON;}
";"   {adjust(); return SEMICOLON;}
"("   {adjust(); return LPAREN;}
")"   {adjust(); return RPAREN;}
"["   {adjust(); return LBRACK;}
"]"   {adjust(); return RBRACK;}
"{"   {adjust(); return LBRACE;}
"}"   {adjust(); return RBRACE;}
"."   {adjust(); return DOT;}
"+"   {adjust(); return PLUS;}
"-"   {adjust(); return MINUS;}
"*"   {adjust(); return TIMES;}
"/"   {adjust(); return DIVIDE;}
"="   {adjust(); return EQ;}
"<>"  {adjust(); return NEQ;}
"<"   {adjust(); return LT;}
"<="  {adjust(); return LE;}
">"   {adjust(); return GT;}
">="  {adjust(); return GE;}
"&"   {adjust(); return AND;}
"|"   {adjust(); return OR;}
":="  {adjust(); return ASSIGN;}

	/* Identifiers. */
[a-zA-Z]+[_0-9a-zA-Z]*	{
		adjust();
		yylval.sval = strdup(yytext);
		return ID;
	}

	/* Unsigned integers. */
[0-9]+	{
	adjust();
	yylval.ival = atoi(yytext);
	return INT;
	}
"/*"	{
	adjust();
	commentNesting++;
	BEGIN COMMENT;
	}
	/* End of a comment before it even started */
"*/"	{
	adjust();
	EM_error(EM_tokPos, "Closing comment tag without an opening one!");
	yyterminate();
	}
	/* Start of a string */
\"	{
	adjust();
	init_string_buffer();
	BEGIN STRING_STATE;
	}
	/* Anything else that's not matched yet is an illegal token. */
.	{
	adjust();
	EM_error(EM_tokPos, "Illegal token!");
	yyterminate();
	}

<STRING_STATE>{
	/* Closing Quote: terminate the string */
	\"	{
		adjust();
		BEGIN INITIAL;
		yylval.sval = strdup(string_buffer);
		return STRING;
	}
	/* newline character in middle of the string -> Error */
	\n	{
		adjust();
		EM_error(EM_tokPos, "Unterminated string literal!");
		yyterminate();
	}
	/* single character with ascii code ddd */
	\\[0-9][0-9][0-9]	{
		adjust();
		int value;
		sscanf(yytext + 1, "%d", &value); // Remove '/'
		if (value >= 0xff) {
			EM_error(EM_tokPos, "ASCII decimal value out of bounds!");
			yyterminate();
		}
		append_char_to_stringbuffer(value);

	}
	/* Any other ASCII codes are invalid */
	\\[0-9]+	{
		adjust();
		EM_error(EM_tokPos, "Bad ASCII sequence!");
		yyterminate();
	}
	/* Double quote */
	\\\"	{
		adjust();
		append_char_to_stringbuffer('"');
	}
	/* Backslash character */
	\\\\	{
		adjust();
		append_char_to_stringbuffer('\\');
	}
	\\[ \f\n\t]+\\	{
		adjust();
		int i;
		for (i = 0; yytext[i]; i++) {
			if (yytext[i] == '\n')
				EM_newline();
		}
		continue;
	}
	/* tabs */
	\\t	{
		adjust();
		append_char_to_stringbuffer('\t');
	}
	/* newline */
	\\n	{
		adjust();
		append_char_to_stringbuffer('\n');
	}
	/* Control characters */
	"\^"[@A-Z[^\\]_?]	{
		adjust();
		append_char_to_stringbuffer(yytext[1] - '@');
	}
	<<EOF>>		{
		EM_error(EM_tokPos, "Unterminated string at end of file!");
		yyterminate();
	}
	/* Normal Text */
	[^\n\f\\"]*	{
		adjust();
		printf("%s", yytext);
		char *temp = yytext;
		while (*temp)
			append_char_to_stringbuffer(*temp++);
	}
}
<COMMENT>{
	/*
	 * Starting of another comment inside a comment.
	 * Increase the nesting and continue.
	 */
	"/*"	{
		adjust();
		commentNesting++;
		continue;
		}
	/*
	 * Closing a comment. Check if possible i.e. if we actually have a
	 * comment open. If nesting equals 0, that means we are no longer in a
	 * comment and change the state accordingly.
	 */
	"*/"	{
		adjust();
		if (--commentNesting == 0)
			BEGIN INITIAL;
		}
	<<EOF>>	{
		EM_error(EM_tokPos, "Comment still open at the end of file!");
		yyterminate();
	}
	/* Anything else except newline; consume */
	.	{adjust();}
}
