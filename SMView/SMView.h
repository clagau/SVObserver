//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SMView.h 
/// All Rights Reserved 
//*****************************************************************************

///  This is the main header file for the application.  It includes other
//project specific headers (including Resource.h) and declares the
//CShareViewApp application class.
//******************************************************************************

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


//! Applicationclass CShareViewApp:
class CSMViewApp : public CWinApp
{
public:
	CSMViewApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSMViewApp theApp;