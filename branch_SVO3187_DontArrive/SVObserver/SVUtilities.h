//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilities
//* .File Name       : $Workfile:   SVUtilities.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:57:10  $
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

class SVSecurityManager;

class SVUtilities  
{
public:
	CMenu *FindSubMenuByName(CMenu *pMenu, LPCTSTR Name);
	void LoadMenu (CMenu *pMenu);
	void SetupUtilities (CMenu *pMenu);
	void RunUtility (SVSecurityManager* pAccess, UINT uiUtilityId);
	SVUtilities();
	virtual ~SVUtilities();
	bool LoadMenuFromINI(CMenu *pMenu);

	void CleanupIni();

	void UpdateIni();

private:
	void ClearMenu (CMenu *pMenu);
};

