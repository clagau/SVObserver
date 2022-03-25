//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SMViewApp.h 
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

//! Applicationclass CShareViewApp:
class SMViewApp : public CWinApp
{
public:
	SMViewApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern SMViewApp theApp;