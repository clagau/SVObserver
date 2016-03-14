//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEquationYaccClass
//* .File Name       : $Workfile:   SVEquationYacc.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:23:08  $
//******************************************************************************

#ifndef SVEQUATIONYACC_H
#define SVEQUATIONYACC_H

#include "SVEquationGlobals.h"

class SVEquationBase;

/**
@SVObjectName Equation YACC

@SVObjectOverview This object uses YACC to generate its semantic operation based on the provided unparsed string.  Once the string is parsed, this object collects the data and executes operations based on the provided string.

@SVObjectOperations This object maintains operations to create tokens for the defined elements in the string and operations upon those tokens.  Other operations are available to define and perform specialized functionality.  The remaining operators are defined to collect the data for each token when the operators are performed.

*/
class SVEquationYaccClass
{
	friend class SVEquationClass;

public:
	SVEquationYaccClass();
	~SVEquationYaccClass();

	int yyerror(char *s);
	int yylex();
	int yyparse();

public:
	// Variables needed
	int yacc_err;
	int sIndex;
	int numTokens;
	SVLEX_STACK lex_stack[4096];
	double equationResult;

	// from original y.yab.c
	int      yydebug;
	int      yynerrs;
	int      yyerrflag;
	int      yychar;
	short*   yyssp;
	YYSTYPE* yyvsp;
	YYSTYPE  yyval;
	YYSTYPE  yylval;

	short   yyss[YYSTACKSIZE];
	YYSTYPE yyvs[YYSTACKSIZE];

	HRESULT m_StatusCode;

protected:
	SVEquationBase* pEquation;
	
	enum SVYaccLimitsEnum
	{
		RECURSION_LIMIT = 21,
		LIST_LIMIT      = 1000,
	};
	int current_recursive_depth;
	int iNumListEntries[ RECURSION_LIMIT ];
	double dValueList[ RECURSION_LIMIT ][ LIST_LIMIT ];

protected:
	double GetPropertyValue(int symbolIndex);
	double GetAt(int symbolIndex, int index, double dDefault);
	double GetAt(int symbolIndex, int index);
	int PushValues(int index);

	double CalcMin();
	double CalcMax();
	double CalcAverage();
	double CalcMedian();
	double CalcSum();
	double CalcStdDeviation();
	int AddToList(double val);
	void InitCurrentList();

}; // end class SVEquationYaccClass

#endif

