//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEquationYaccClass
//* .File Name       : $Workfile:   SVEquationYacc.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:23:08  $
//******************************************************************************

#pragma once

#include "SVEquationGlobals.h"
#include <stack>


class SVEquationBase;
struct RElement;
/**
@SVObjectName Equation YACC

@SVObjectOverview This object uses YACC to generate its semantic operation based on the provided unparsed string.  Once the string is parsed, this object collects the data and executes operations based on the provided string.

@SVObjectOperations This object maintains operations to create tokens for the defined elements in the string and operations upon those tokens.  Other operations are available to define and perform specialized functionality.  The remaining operators are defined to collect the data for each token when the operators are performed.

*/
class SVEquationYaccClass
{
public:
	SVEquationYaccClass();
	~SVEquationYaccClass();

	int yyerror(const char*);
	int yylex();
	int yyparse();

	void setEquation(SVEquationBase* pEquation) { m_pEquation = pEquation; }

public:
	// Variables needed
	int yacc_err;
	int sIndex;
	int numTokens = 0;
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
	SVEquationBase* m_pEquation = nullptr;
	
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
	double GetAt(int symbolIndex, int index, double dDefault=0.0);
	
	int PushValues(int index);
	int indexValue() const;
	
	
	RElement* AddToRanges(double value);
	RElement* AddToRanges(double first, double last);
	int PushRangeToList(); 
	
	
	void pushRangeStack(int id); //new actual  range
	
	bool emptyRangeStack(void);  //are there any actual ranges?
	void popRangeStack(void);  //delete actual range;
	
	double CalcMin();
	double CalcMax();
	double CalcAverage();
	double CalcMedian();
	double CalcSum();
	double CalcStdDeviation();
	int AddToList(double val);

	void InitCurrentList();

	
	struct Rangestruct
	{
		explicit Rangestruct(int ID) :id(ID)
		{;};
		int id = {-1};
		std::vector<RElement> m_Indices;
	};
	std::stack<Rangestruct> m_RangeStack;

}; // end class SVEquationYaccClass

