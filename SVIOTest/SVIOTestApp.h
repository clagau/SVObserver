// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOTEST
// * .File Name       : $Workfile:   SVIOTEST.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:50:16  $
// ******************************************************************************

#pragma once

#pragma region Includes
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Triggering/SVIOTriggerLoadLibraryClass.h"
#pragma endregion Includes

static int SVMEBoardNumber = 0;

class SVIOTestApp : public CWinApp
{
public:
	SVIOTestApp();
	virtual ~SVIOTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVIOTestApp)
	public:
	virtual BOOL InitInstance() override;
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(SVIOTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	SvTrig::SVIOTriggerLoadLibraryClass m_svTriggers;
};
