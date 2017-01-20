//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilitiesClass
//* .File Name       : $Workfile:   SVUtilities.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:57:10  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <deque>
#include "SVUtilityLibrary/SVString.h"
#include "SVSecurity/SVSecurityManager.h"
#pragma endregion Includes

class SVUtilitiesClass  
{
public:
	CMenu *FindSubMenuByName(CMenu *pMenu, LPCTSTR Name);
	BOOL LoadMenu (CMenu *pMenu);
	BOOL SetupUtilities (CMenu *pMenu);
	void RunUtility (SVSecurityManager* pAccess, UINT uiUtilityId);
	SVUtilitiesClass();
	virtual ~SVUtilitiesClass();
	BOOL LoadMenuFromINI(CMenu *pMenu);

	BOOL CleanupIni();

	BOOL UpdateIni();

private:
	BOOL ClearMenu (CMenu *pMenu);
};

