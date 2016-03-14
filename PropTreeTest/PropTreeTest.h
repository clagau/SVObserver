//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PropTreeTest
//* .File Name       : $Workfile:   PropTreeTest.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   03 Oct 2013 14:43:26  $
//******************************************************************************

// PropTreeTest.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPropTreeTestApp:
// See PropTreeTest.cpp for the implementation of this class
//

class CPropTreeTestApp : public CWinApp
{
public:
	CPropTreeTestApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CPropTreeTestApp theApp;

