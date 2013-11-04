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

#ifndef SVPARSERSCRIPTSGLOBALS_H
#define SVPARSERSCRIPTSGLOBALS_H

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

#endif // #ifndef SVPARSERSCRIPTSGLOBALS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVEquationLibrary\SVEquationGlobals.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:22:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 Jun 2007 10:44:56   Joe
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
*/
