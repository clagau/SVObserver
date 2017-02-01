//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file RRSView.h 
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
class CRRSViewApp : public CWinApp
{
public:
	CRRSViewApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CRRSViewApp theApp;