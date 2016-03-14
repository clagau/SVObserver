//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SubSystemTestMFC
//* .File Name       : $Workfile:   SubSystemTestMFC.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:15:58  $
//******************************************************************************

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSubSystemTestMFCApp:
// See SubSystemTestMFC.cpp for the implementation of this class
//

class CSubSystemTestMFCApp : public CWinApp
{
public:
	CSubSystemTestMFCApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSubSystemTestMFCApp theApp;

