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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVEquationLibrary\SVEquationYacc.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:23:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 Jun 2007 10:44:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * Source moved from original location in SVObserver project and SVPARSERSCRIPTS project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   17 Jul 2006 11:43:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  570
 * SCR Title:  Fix Bug in math tools that casues nested commands to evaluate incorrectly
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified code on math equation functions MIN, MAX, MED, STD DEV, AVG, and SUM to allow recursion.  Also added code to allow legacy behavior.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   19 Jul 2005 14:30:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added syntax for accessing an array element without specifying a default value (uses value object default value)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Jun 2005 08:51:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented SUM, STDDEV, array value object parsing
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 09:25:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
