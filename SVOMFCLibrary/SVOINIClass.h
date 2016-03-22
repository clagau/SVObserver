// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOINIClass
// * .File Name       : $Workfile:   SVOINIClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:08:22  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <io.h>

class SVOINIClass  
{
public:
	SVOINIClass();
	~SVOINIClass();

	HRESULT SetFile( LPCTSTR p_szFile );

	std::string GetValueString( LPCTSTR p_szSection,
	                  LPCTSTR p_szKey,
	                  LPCTSTR p_szDefault,
	                  LPCTSTR p_szFileName = NULL );

	HRESULT GetValue( LPCTSTR p_szSection,
	                  LPCTSTR p_szKey,
	                  LPCTSTR p_szDefault,
	                  BSTR*   p_pbstrValue,
	                  LPCTSTR p_szFileName = NULL );

	HRESULT GetValue( LPCTSTR p_szSection,
	                  LPCTSTR p_szKey,
	                  int     p_iDefault,
	                  int*    p_piValue,
	                  LPCTSTR p_szFileName = NULL );

	int GetValueInt( LPCTSTR p_szSection,
	                     LPCTSTR p_szKey,
	                     int     p_iDefault,
	                     LPCTSTR p_szFileName = NULL );

	HRESULT SetValue( LPCTSTR p_szSection,
	                  LPCTSTR p_szKey,
	                  LPCTSTR p_szValue,
	                  LPCTSTR p_szFileName = NULL );

	HRESULT SetValue( LPCTSTR p_szSection,
	                  LPCTSTR p_szKey,
	                  int     p_iValue,
	                  LPCTSTR p_szFileName = NULL );
protected:
	LPTSTR	m_tszFile;
};

#include "SVOINIClass.inl"

