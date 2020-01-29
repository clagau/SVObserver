%{
#pragma warning( push )
#pragma warning( disable : 4102 4013 ) 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
%}

%{
#ifndef Y_TAB_H
#define Y_TAB_H

/**
@SVObjectName Data Tracking Union

@SVObjectOverview This object tracks the data index, value, and/or List Position.

@SVObjectOperations This object has no operators.

*/
%}
%union {
int index;
double val;
double* listPtr;
}
%{
#endif
%}

%token  SV_IDENTIFIER
%token  SV_NUMBER
%token  SV_PLUS SV_MINUS SV_TIMES SV_DIVIDE
%token  SV_LEFT_PARENTHESIS SV_RIGHT_PARENTHESIS
%token  SV_COMMA
%token	SV_EQ SV_NE SV_LT SV_LTE SV_GT SV_GTE
%token	SV_AND SV_OR SV_XOR SV_NOT
%token	SV_SINE SV_COSINE SV_TANGENT SV_ARCSINE SV_ARCCOSINE SV_ARCTANGENT
%token	SV_MODULUS SV_POW SV_ABSOLUTE SV_SQUARE SV_SQUAREROOT SV_TRUNCATE
%token	SV_MINIMUM SV_MAXIMUM SV_AVERAGE SV_MEDIAN
%token	SV_IF SV_THEN SV_ELSE
%token	SV_SUM SV_STDDEV
%token  SV_STARTSUBSCRIPT SV_ENDSUBSCRIPT
%token  SV_STARTCURLYBRACE SV_ENDCURLYBRACE
%token  SV_END
%type <val> SV_NUMBER Expression ArrayIndexIdentifier
%type <listPtr> ExpressionList ArrayIdentifier
%type <index> SV_IDENTIFIER 

%right  SV_EQ SV_NE SV_LT SV_LTE SV_GT SV_GTE

%left	SV_AND SV_OR SV_XOR SV_NOT

%left   SV_PLUS SV_MINUS
%left   SV_TIMES SV_DIVIDE
%left   SV_NEG

%start Input

%%

Input:
          /* Empty */
        | Input Line
        ;

Line:
          SV_END
        | Expression SV_END					{ equationResult = $1; }
        | error SV_END						{ yyerrok; }
        ;

Expression:
          SV_NUMBER							{ $$=$1; }
		| SV_IDENTIFIER						{ $$=GetPropertyValue($1); }
		| ArrayIndexIdentifier				{ $$=$1; }
		| Expression SV_AND Expression		{ $$=($1 && $3); }
		| Expression SV_OR Expression		{ $$=($1 || $3); }
		| Expression SV_XOR Expression		{ $$=(($1-$3) ? 1.0 : 0.0); }

        | Expression SV_PLUS Expression		{ $$=$1+$3; }
        | Expression SV_MINUS Expression	{ $$=$1-$3; }
        | Expression SV_TIMES Expression	{ $$=$1*$3; }
        | Expression SV_DIVIDE Expression	{ $$=(($3) ? ($1/$3) : (($1 >= 0.0) ? HUGE_VAL : -HUGE_VAL)); }

        | SV_MINUS Expression %prec SV_NEG  { $$=-$2; }

        | Expression SV_EQ Expression		{ $$=($1==$3); }
		| Expression SV_NE Expression		{ $$=($1!=$3); }
		| Expression SV_LT Expression		{ $$=($1<$3); }
		| Expression SV_LTE Expression		{ $$=($1<=$3); }
		| Expression SV_GT Expression		{ $$=($1>$3); }
		| Expression SV_GTE Expression		{ $$=($1>=$3); }
		
		| SV_NOT Expression					{ $$=(!$2); }

		| SV_MODULUS SV_LEFT_PARENTHESIS Expression SV_COMMA Expression SV_RIGHT_PARENTHESIS { $$=fmod($3,$5); }
		| SV_POW SV_LEFT_PARENTHESIS Expression SV_COMMA Expression SV_RIGHT_PARENTHESIS { $$=pow($3,$5); }
		| SV_ABSOLUTE SV_LEFT_PARENTHESIS Expression SV_RIGHT_PARENTHESIS		{ $$=fabs($3); }
		| SV_SQUARE SV_LEFT_PARENTHESIS Expression SV_RIGHT_PARENTHESIS		{ $$=($3*$3); }
		| SV_SQUAREROOT SV_LEFT_PARENTHESIS Expression SV_RIGHT_PARENTHESIS	{ $$=sqrt($3); }
		| SV_TRUNCATE SV_LEFT_PARENTHESIS Expression SV_RIGHT_PARENTHESIS		{ $$=(double)((int)($3)); }

		| SV_SINE SV_LEFT_PARENTHESIS Expression SV_RIGHT_PARENTHESIS		{ $$=sin(($3 * svDegreesToRadians)); }
		| SV_ARCSINE SV_LEFT_PARENTHESIS Expression SV_RIGHT_PARENTHESIS	{ $$=(asin($3) * svRadiansToDegrees); }
		| SV_COSINE SV_LEFT_PARENTHESIS Expression SV_RIGHT_PARENTHESIS		{ $$=cos(($3 * svDegreesToRadians)); }
		| SV_ARCCOSINE SV_LEFT_PARENTHESIS Expression SV_RIGHT_PARENTHESIS	{ $$=(acos($3) * svRadiansToDegrees); }
		| SV_TANGENT SV_LEFT_PARENTHESIS Expression SV_RIGHT_PARENTHESIS	{ $$=(($3 == 90 || $3 == 270) ? 0.0 : tan(($3 * svDegreesToRadians))); }
		| SV_ARCTANGENT SV_LEFT_PARENTHESIS Expression SV_RIGHT_PARENTHESIS { $$=(atan($3) * svRadiansToDegrees); }
		| SV_IF SV_LEFT_PARENTHESIS Expression SV_RIGHT_PARENTHESIS SV_THEN SV_STARTCURLYBRACE Expression SV_ENDCURLYBRACE SV_ELSE SV_STARTCURLYBRACE Expression SV_ENDCURLYBRACE { $$=($3?$7:$11);}
		
		| SV_LEFT_PARENTHESIS Expression SV_RIGHT_PARENTHESIS { $$=$2; }

		| SV_MINIMUM SV_LEFT_PARENTHESIS { if ( g_bUseCorrectListRecursion ) ++current_recursive_depth; InitCurrentList(); }
					ExpressionListOrArrayIdentifier SV_RIGHT_PARENTHESIS { $$=CalcMin(); if ( g_bUseCorrectListRecursion ) --current_recursive_depth; }
		| SV_MAXIMUM SV_LEFT_PARENTHESIS { if ( g_bUseCorrectListRecursion ) ++current_recursive_depth; InitCurrentList(); }
					ExpressionListOrArrayIdentifier SV_RIGHT_PARENTHESIS { $$=CalcMax(); if ( g_bUseCorrectListRecursion ) --current_recursive_depth; }
		| SV_AVERAGE SV_LEFT_PARENTHESIS { if ( g_bUseCorrectListRecursion ) ++current_recursive_depth; InitCurrentList(); }
					ExpressionListOrArrayIdentifier SV_RIGHT_PARENTHESIS { $$=CalcAverage(); if ( g_bUseCorrectListRecursion ) --current_recursive_depth; }
		| SV_MEDIAN SV_LEFT_PARENTHESIS { if ( g_bUseCorrectListRecursion ) ++current_recursive_depth; InitCurrentList(); }
					ExpressionListOrArrayIdentifier SV_RIGHT_PARENTHESIS { $$=CalcMedian(); if ( g_bUseCorrectListRecursion ) --current_recursive_depth; }
		| SV_SUM    SV_LEFT_PARENTHESIS { if ( g_bUseCorrectListRecursion ) ++current_recursive_depth; InitCurrentList(); }
					ExpressionListOrArrayIdentifier SV_RIGHT_PARENTHESIS { $$=CalcSum(); if ( g_bUseCorrectListRecursion ) --current_recursive_depth; }
		| SV_STDDEV SV_LEFT_PARENTHESIS { if ( g_bUseCorrectListRecursion ) ++current_recursive_depth; InitCurrentList(); }
					ExpressionListOrArrayIdentifier SV_RIGHT_PARENTHESIS { $$=CalcStdDeviation(); if ( g_bUseCorrectListRecursion ) --current_recursive_depth; }
        ;

ArrayIdentifier:
		  SV_IDENTIFIER SV_STARTSUBSCRIPT SV_ENDSUBSCRIPT { PushValues($1); }
		;

ArrayIndexIdentifier:
		  SV_IDENTIFIER SV_STARTSUBSCRIPT Expression SV_STARTCURLYBRACE Expression SV_ENDCURLYBRACE SV_ENDSUBSCRIPT { $$=GetAt($1,$3,$5); }
		| SV_IDENTIFIER SV_STARTSUBSCRIPT Expression SV_ENDSUBSCRIPT { $$=GetAt($1,$3); }
		;

ExpressionList:
		Expression SV_COMMA Expression { if(!AddToList($1))
											YYABORT; 
										if(!AddToList($3))
											YYABORT;
									   }
		| ArrayIdentifier SV_COMMA Expression { if (!AddToList($3))
													YYABORT;	}
		| Expression SV_COMMA ArrayIdentifier { if (!AddToList($1))
													YYABORT;	}
		| ArrayIdentifier SV_COMMA ArrayIdentifier { }
		| ExpressionList SV_COMMA Expression { if(!AddToList($3))
													YYABORT;
												$$=$1;	 }
		| ExpressionList SV_COMMA ArrayIdentifier { $$=$1; }
		;

ExpressionListOrArrayIdentifier:
		ExpressionList
		| ArrayIdentifier
		;

%%
