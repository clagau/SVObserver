//******************************************************************************
//* COPYRIGHT (c) 2007 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVParserSCriptsGlobals
//* .File Name       : $Workfile:   SVEquationGlobals.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:22:56  $
//******************************************************************************

#pragma once

#ifndef Y_TAB_H
#define Y_TAB_H
	#include "SVPARSERSCRIPTS/y.tab.h"
#endif // #ifndef Y_TAB_H

#ifdef YYSTACKSIZE
	#undef YYMAXDEPTH
	#define YYMAXDEPTH YYSTACKSIZE
#else
	#ifdef YYMAXDEPTH
		#define YYSTACKSIZE YYMAXDEPTH
	#else
		#define YYSTACKSIZE 500
		#define YYMAXDEPTH 500
	#endif
#endif

#define yystacksize YYSTACKSIZE

/**
@SVObjectName Lex Stack

@SVObjectOverview This object maintains the data for the scanning operations of the lexical grammar.

@SVObjectOperations This object has no operations.

*/
struct SVLEX_STACK
{
	int position;
	int token;
	YYSTYPE value;
};

const double PI = 3.1415926535;
const double svDegreesToRadians = PI / 180.0;
const double svRadiansToDegrees = 180.0 / PI;

