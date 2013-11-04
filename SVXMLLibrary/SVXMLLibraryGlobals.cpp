//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLLibraryGlobals
//* .File Name       : $Workfile:   SVXMLLibraryGlobals.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:52:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVXMLLibraryGlobals.h"

// CalculateNodeCheckSums () mode values.  Keep bit level so that we can 
// possibly set multiple.
const long g_lXMLCheckSumCreate = 1;
const long g_lXMLCheckSumCompare = 2;

// Other
const char*		g_csSVR00001SchemaName			= "SVR00001";
const WCHAR*	g_wcsFromParent					= L"<FromParent>";

// Node Titles
const WCHAR*	g_wcsData				= L"DATA";
const WCHAR*	g_wcsNode				= L"NODE";
const char*		g_csSchema				= "Schema";
const WCHAR*	g_wcsEncryption		= L"Encryption";
const	char*		g_csRevisionHistory	= "RevisionHistory";
const WCHAR*	g_wcsRevision			= L"Revision";

// Attribute Titles
const WCHAR*	g_wcsFormat				= L"Format";
const WCHAR*	g_wcsIsActive			= L"IsActive";
const WCHAR*	g_wcsFormatVersion	= L"FormatVersion";
const WCHAR*	g_wcsRevisionAtt		= L"Revision";
const char*		g_csCheckSum			= "CheckSum";

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXMLLibraryGlobals.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:52:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 18:12:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code is the refacting of the items from the SVLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
