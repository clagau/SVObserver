//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecurityApp.h
//* .File Name       : $Workfile:   SVSecurityApp.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:01:48  $
//******************************************************************************

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

/////////////////////////////////////////////////////////////////////////////
// SVSecurityApp
// See SVSecurity.cpp for the implementation of this class
//

class SVSecurityApp : public CWinApp
{
public:
	SVSecurityApp();
	virtual ~SVSecurityApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVSecurityApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(SVSecurityApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


