//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOIPFrameWndClass
//* .File Name       : $Workfile:   SVOIPFrameWndClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:08:04  $
//******************************************************************************

#pragma once

/////////////////////////////////////////////////////////////////////////////
// SVOIPFrameWndClass frame

class SVOIPFrameWndClass : public CFrameWnd
{
	DECLARE_DYNCREATE(SVOIPFrameWndClass)
protected:
	SVOIPFrameWndClass();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOIPFrameWndClass)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~SVOIPFrameWndClass();

	// Generated message map functions
	//{{AFX_MSG(SVOIPFrameWndClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
