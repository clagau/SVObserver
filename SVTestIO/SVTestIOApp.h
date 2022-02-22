//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestIOApp
//* .File Name       : $Workfile:   SVTestIO.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:28:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#pragma endregion Includes

class SVTestIODlg;

class SVTestIOApp : public CWinApp
{
public:
	SVTestIOApp();
	virtual ~SVTestIOApp();

// Overrides
public:
	virtual BOOL InitInstance() override;
	virtual BOOL ExitInstance() override;

	SVTestIODlg* m_pTestIODlg;

	DECLARE_MESSAGE_MAP()
};

