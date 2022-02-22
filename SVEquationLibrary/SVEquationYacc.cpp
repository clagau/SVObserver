//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEquationYaccClass
//* .File Name       : $Workfile:   SVEquationYacc.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:23:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVEquationYacc.h"
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <math.h>
//Moved to precompiled header: #include <vector>
#include "SVEquationBase.h"
#include "SVMessage/SVMessage.h"

#pragma warning( push )
#pragma warning( disable : 4102) 
#pragma warning( disable : 4013) 
#pragma warning( disable : 4244) 
#pragma warning( disable : 4706) 

bool g_bUseCorrectListRecursion = true;

SVEquationYaccClass::SVEquationYaccClass()
{
	m_StatusCode = S_OK;
	yacc_err = 0;
	sIndex = 0;
	memset( lex_stack,'\0', sizeof( lex_stack ) );
	equationResult = 0.0;	

	// from original y.yab.c
	yydebug = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = 0;
	yyssp = nullptr;
	yyvsp = nullptr;
	
	memset(yyss,'\0',YYSTACKSIZE);
	memset(yyvs,'\0',YYSTACKSIZE);

	memset( &(iNumListEntries[0]), 0, sizeof(iNumListEntries[0]) * RECURSION_LIMIT );
	memset( &(dValueList[0][0]), 0, sizeof(dValueList[0][0]) * RECURSION_LIMIT * LIST_LIMIT );
	current_recursive_depth = 0;
	yylval.listPtr = &(dValueList[0][0]);	// THIS ISN'T EVEN REALLY USED!?
}

SVEquationYaccClass::~SVEquationYaccClass()
{
}

int SVEquationYaccClass::yyerror(const char*) 
{ 
	yacc_err = 1;
	current_recursive_depth = 0;	// reset this var on error
	return(-1);
}

int SVEquationYaccClass::yylex()
{
	if( lex_stack[sIndex].token == SV_IDENTIFIER )
		yylval.index = lex_stack[sIndex].value.index;
	else
		yylval.val = lex_stack[sIndex].value.val;
	return lex_stack[sIndex++].token;
}

double SVEquationYaccClass::GetPropertyValue(int symbolIndex )
{
	return m_pEquation->GetPropertyValue( symbolIndex );
}

double SVEquationYaccClass::GetAt(int symbolIndex, int index, double dDefault /*=0.0*/)
{
	return m_pEquation->GetSubscriptedPropertyValue( symbolIndex, index-1, dDefault );	// indexes in the Equation are 1-based
}

int SVEquationYaccClass::PushValues(int iSymbolIndex)
{
	std::vector<double> values;
	/*HRESULT hr = */m_pEquation->GetArrayValues(iSymbolIndex, values);
	int iNum =0;
	for (size_t i = 0; i < values.size(); i++)
	{
		iNum = AddToList(values[i]);
	}
	return iNum;
}

int SVEquationYaccClass::indexValue() const
{
	return m_pEquation->GetIndexValue();
}

void SVEquationYaccClass::InitCurrentList()
{
	iNumListEntries[current_recursive_depth] = 0;
}

int SVEquationYaccClass::AddToList(double val)
{
	// Check if at max
	if ( current_recursive_depth < RECURSION_LIMIT )
	{
		int& rCurrentNumListEntries = iNumListEntries[current_recursive_depth];
		if ( (rCurrentNumListEntries < LIST_LIMIT)  )
		{
			dValueList[current_recursive_depth][rCurrentNumListEntries++] = val;
			return rCurrentNumListEntries;
		}
		else
		{
			m_StatusCode = SVMSG_TOO_MANY_VARIABLES;
			yyerror(nullptr);
		}
	}
	else
	{
		m_StatusCode = SVMSG_TOO_MANY_VARIABLES;
		yyerror(nullptr);
		current_recursive_depth = 0;	// reset this var on error
	}

	return 0;
}

double SVEquationYaccClass::CalcMin()
{
	double val = 0.0;
	const int& rCurrentNumListEntries = iNumListEntries[current_recursive_depth];

	if (rCurrentNumListEntries)
	{
		val = dValueList[current_recursive_depth][0];
		
		for(int i = 1;i < rCurrentNumListEntries; i++)
		{
			val = __min(dValueList[current_recursive_depth][i],val);
		}
	}
	return val;

}

double SVEquationYaccClass::CalcMax()
{
	double val = 0.0;
	const int& rCurrentNumListEntries = iNumListEntries[current_recursive_depth];

	if (rCurrentNumListEntries)
	{
		val = dValueList[current_recursive_depth][0];
	
		for (int i = 1;i < rCurrentNumListEntries; i++)
		{
			val = __max(dValueList[current_recursive_depth][i],val);
		}
	}
	
	return val;
}

double SVEquationYaccClass::CalcAverage()
{
	double val = 0.0;
	const int& rCurrentNumListEntries = iNumListEntries[current_recursive_depth];

	if (rCurrentNumListEntries)
	{
		for (int i = 0;i < rCurrentNumListEntries; i++)
		{
			val += dValueList[current_recursive_depth][i];
		}
		val /= (double) rCurrentNumListEntries;
	}
	return val;
}

double SVEquationYaccClass::CalcMedian()
{
	double val = 0.0;
	const int& rCurrentNumListEntries = iNumListEntries[current_recursive_depth];

	if (rCurrentNumListEntries)
	{
		if ( g_bUseCorrectListRecursion )
		{
			std::sort(&(dValueList[current_recursive_depth][0]), &(dValueList[current_recursive_depth][rCurrentNumListEntries]));

			int index = (rCurrentNumListEntries - 1) / 2;

			val = dValueList[current_recursive_depth][index];

			if ((rCurrentNumListEntries % 2) == 0) // if even
			{
				val = (val + dValueList[current_recursive_depth][index+1]) / 2.0;
			}
		}
		else
		{
			std::sort(&(dValueList[current_recursive_depth][0]), &(dValueList[current_recursive_depth][rCurrentNumListEntries-1]));

			int index = rCurrentNumListEntries / 2;

			val = dValueList[current_recursive_depth][index];

			if ((rCurrentNumListEntries % 2) == 0) // if even
			{
				val = (val + dValueList[current_recursive_depth][index-1]) / 2.0;
			}
		}
	}
	return val;
}

double SVEquationYaccClass::CalcSum()
{
	double val = 0.0;
	const int& rCurrentNumListEntries = iNumListEntries[current_recursive_depth];
	
	if (rCurrentNumListEntries)
	{
		for (int i = 0;i < rCurrentNumListEntries; i++)
		{
			val += dValueList[current_recursive_depth][i];
		}
	}
	return val;
}

double SVEquationYaccClass::CalcStdDeviation()
{
	double val = 0.0;
	const int& rCurrentNumListEntries = iNumListEntries[current_recursive_depth];

	if ( rCurrentNumListEntries )
	{
		double avg = CalcAverage();
		for (int i = 0;i < rCurrentNumListEntries; i++)
		{
			double dDistance = dValueList[current_recursive_depth][i] - avg;
			val += dDistance * dDistance;
		}
		val /= rCurrentNumListEntries;
		val = sqrt(val);

	}
	return val;
}

#define yyparse SVEquationYaccClass::yyparse

#include "y.tab.c"

#pragma warning( pop )

