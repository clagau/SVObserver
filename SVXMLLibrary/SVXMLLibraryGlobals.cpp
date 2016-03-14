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

