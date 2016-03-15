// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIPFrameWndClass
// * .File Name       : $Workfile:   SVOIPFrameWndClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:35:28  $
// ******************************************************************************

#if !defined(AFX_SVOIPFRAMEWNDCLASS_H__CDF9EB40_A939_4B1A_AF5C_D45CE408BB2A__INCLUDED_)
#define AFX_SVOIPFRAMEWNDCLASS_H__CDF9EB40_A939_4B1A_AF5C_D45CE408BB2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

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

#endif // !defined(AFX_SVOIOFRAMEWNDCLASS_H__CDF9EB40_A939_4B1A_AF5C_D45CE408BB2A__INCLUDED_)

