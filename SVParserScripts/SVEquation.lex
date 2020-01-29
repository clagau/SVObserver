%{
#pragma warning( disable : 4102 4013 )  

#include <stdlib.h>
#include <string.h>
%}

%option c++
%option noyywrap
%option yyclass="SVEquationLexClass"

%s SYNTAX_CHECK

white           [ \t]+
crlf			"\x0d\x0a"
digit2			[0-1]
digit10         [0-9]
digit16			[0-9A-Fa-f]
letter          [A-Za-z]+
alphanumeric	[A-Za-z0-9 _#\203\212\214\216\232\234\236\237\252\265\272\300-\326\330-\366\370-\377]+
delimiter		\"
binary			{digit2}+("b")
integer         {digit10}+
hex				{digit16}+("h")
float			{integer}("."{integer})
name			{alphanumeric}
identifier      {delimiter}{name}("."{name})*{delimiter}
%%

	if ( syntax_check )
		BEGIN(SYNTAX_CHECK);
	
{white}         { /* We ignore white space characters */
					currentPos++;
				}
				
{crlf}			{ /* Ignore carrage return line feed also */
					currentPos+=2;
				}

{float}			{
					position = currentPos;
					currentPos += strlen(yytext);
					yylval.val=atof(yytext); 
					return(SV_NUMBER);
				}

{integer}		{
					position = currentPos;
					currentPos += strlen(yytext);
					yylval.val=(double)(atoi(yytext));
					return(SV_NUMBER);
				}

{hex}			{	
					char *endptr;
					position = currentPos;
					currentPos += strlen(yytext);
					yylval.val=strtoul(yytext,&endptr,16);
					return(SV_NUMBER);
				}

{binary}		{
					char *endptr;
					position = currentPos;
					currentPos += strlen(yytext);
					yylval.val=strtoul(yytext,&endptr,2);
					return(SV_NUMBER);
				}

{identifier}	{
					position = currentPos;
					currentPos += strlen(yytext);
					yylval.index = addSymbol(yytext);
					if (yylval.index == -1)
					{
						YY_FATAL_ERROR("Invalid Identifier");
						yyterminate();
					}
					return(SV_IDENTIFIER);
				}

"+"				{
					position = currentPos;
					currentPos++;
					return(SV_PLUS);
				}
				
"-"				{
					position = currentPos;
					currentPos++;
					return(SV_MINUS);
				}
				
"*"				{
					position = currentPos;
					currentPos++;
					return(SV_TIMES);
				}
				
"/"				{
					position = currentPos;
					currentPos++;
					return(SV_DIVIDE);
				}
				
"EQ"	| 
"eq"	|
"="		{
					position = currentPos;
					currentPos += 2;
					return(SV_EQ);
				}

"NE"	| 
"ne"	|
"<>"			{
					position = currentPos;
					currentPos += 2;
					return(SV_NE);
				}

"LT"	| 
"lt"	|
"<"			{
					position = currentPos;
					currentPos += 2;
					return(SV_LT);
				}

"LTE"	| 
"lte"	|
"<="		{
					position = currentPos;
					currentPos += 2;
					return(SV_LTE);
				}

"GT"	| 
"gt"	|
">"			{
					position = currentPos;
					currentPos += 2;
					return(SV_GT);
				}

"GTE"	| 
"gte"	|
">="			{
					position = currentPos;
					currentPos += 2;
					return(SV_GTE);
				}

"AND"	| 
"and"			{
					position = currentPos;
					currentPos += 3;
					return(SV_AND);
				}

"OR"	| 
"or"			{
					position = currentPos;
					currentPos += 3;
					return(SV_OR);
				}

"NOT"	| 
"not"			{
					position = currentPos;
					currentPos += 3;
					return(SV_NOT);
				}

"XOR"	| 
"xor"			{
					position = currentPos;
					currentPos += 3;
					return(SV_XOR);				
				}

"SIN"	| 
"sin"			{
					position = currentPos;
					currentPos += 3;
					return(SV_SINE);
				}

"COS"	| 
"cos"			{
					position = currentPos;
					currentPos += 3;
					return(SV_COSINE);
				}

"TAN"	| 
"tan"			{
					position = currentPos;
					currentPos += 3;
					return(SV_TANGENT);
				}

"ASIN"	| 
"asin"			{
					position = currentPos;
					currentPos += 3;
					return(SV_ARCSINE);
				}

"ACOS"	| 
"acos"			{
					position = currentPos;
					currentPos += 3;
					return(SV_ARCCOSINE);
				}

"ATAN"	| 
"atan"			{
					position = currentPos;
					currentPos += 3;
					return(SV_ARCTANGENT);
				}

"MOD"	| 
"mod"			{
					position = currentPos;
					currentPos += 3;
					return(SV_MODULUS);
				}

"POW"	| 
"pow"			{
					position = currentPos;
					currentPos += 3;
					return(SV_POW);
				}

"ABS"	| 
"abs"			{
					position = currentPos;
					currentPos += 3;
					return(SV_ABSOLUTE);
				}

"SQR"	| 
"sqr"			{
					position = currentPos;
					currentPos += 3;
					return(SV_SQUARE);
				}

"SQRT"	| 
"sqrt"			{	
					position = currentPos;
					currentPos += 4;
					return(SV_SQUAREROOT);
				}

"TRUNC"	| 
"trunc"			{	
					position = currentPos;
					currentPos += 5;
					return(SV_TRUNCATE);
				}

"MIN"	|
"min"			{
					position = currentPos;
					currentPos += 3;
					return(SV_MINIMUM);
				}

"MAX"	|
"max"			{
					position = currentPos;
					currentPos += 3;
					return(SV_MAXIMUM);
				}

"AVG"	|
"avg"			{
					position = currentPos;
					currentPos += 3;
					return(SV_AVERAGE);
				}

"MED"	|
"med"			{
					position = currentPos;
					currentPos += 3;
					return(SV_MEDIAN);
				}

"SUM"	|
"sum"			{
					position = currentPos;
					currentPos += 3;
					return(SV_SUM);
				}

"IF"	|
"if"			{
					position = currentPos;
					currentPos += 2;
					return(SV_IF);
				}

"THEN"	|
"then"			{
					position = currentPos;
					currentPos += 4;
					return(SV_THEN);
				}

"ELSE"	|
"else"			{
					position = currentPos;
					currentPos += 4;
					return(SV_ELSE);
				}

"STDDEV"	|
"stddev"	|
"StdDev"		{
					position = currentPos;
					currentPos += 6;
					return(SV_STDDEV);
				}

"("				{	
					position = currentPos;
					currentPos++;
					paren++;
					return(SV_LEFT_PARENTHESIS);
				}
				
")"				{	
					position = currentPos;
					currentPos++;
					paren--;
					return(SV_RIGHT_PARENTHESIS);
				}
				
","				{	
					position = currentPos;
					currentPos++;

					return(SV_COMMA);
				}

"["				{	
					position = currentPos;
					currentPos++;

					return(SV_STARTSUBSCRIPT);
				}

"]"				{	
					position = currentPos;
					currentPos++;

					return(SV_ENDSUBSCRIPT);
				}

"{"				{	
					position = currentPos;
					currentPos++;

					return(SV_STARTCURLYBRACE);
				}

"}"				{	
					position = currentPos;
					currentPos++;

					return(SV_ENDCURLYBRACE);
				}

"\n"			{ return(SV_END); }


<SYNTAX_CHECK>{
"++"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"+*"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"+/"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"+)"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"+,"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }

"-+"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"-*"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"-/"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"-)"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"-,"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }

"**"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"*/"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"*+"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"*)"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"*,"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }

"//"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"/*"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"/+"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"/)"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"/,"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }

",+"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
",*"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
",/"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
",,"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
",)"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }

"(+"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"(*"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"(/"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"(,"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }
"()"		{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }

.			{ YY_FATAL_ERROR("Invalid Token"); yyterminate(); }

}

%%
