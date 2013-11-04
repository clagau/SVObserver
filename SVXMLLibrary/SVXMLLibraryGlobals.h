//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLLibraryGlobals
//* .File Name       : $Workfile:   SVXMLLibraryGlobals.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:52:22  $
//******************************************************************************

#ifndef SVXMLLIBRARYGLOBALS_H
#define SVXMLLIBRARYGLOBALS_H

// CalculateNodeCheckSums
extern const long g_lXMLCheckSumCreate;
extern const long g_lXMLCheckSumCompare;

// Other
extern const char*	g_csSVR00001SchemaName;
extern const WCHAR* g_wcsFromParent;

// Node Titles
extern const WCHAR*	g_wcsData;
extern const WCHAR*	g_wcsNode;
extern const char*	g_csSchema;
extern const WCHAR*	g_wcsEncryption;
extern const char*  g_csRevisionHistory;
extern const WCHAR* g_wcsRevision;

// Attribute Titles
extern const WCHAR*	g_wcsFormat;
extern const WCHAR*	g_wcsIsActive;
extern const WCHAR* g_wcsFormatVersion;
extern const WCHAR* g_wcsRevisionAtt;
extern const char*  g_csCheckSum;

// Other

#endif // #ifndef SVXMLLIBRARYGLOBALS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXMLLibraryGlobals.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:52:22   bWalter
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

