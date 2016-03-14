//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOIOFrameWndClass
//* .File Name       : $Workfile:   SVOIOFrameWndClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:06:58  $
//******************************************************************************

#if !defined(AFX_SVOIOFRAMEWNDCLASS_H__CDF9EB40_A939_4B1A_AF5C_D45CE408BB2A__INCLUDED_)
#define AFX_SVOIOFRAMEWNDCLASS_H__CDF9EB40_A939_4B1A_AF5C_D45CE408BB2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVOIOFrameWndClass.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SVOIOFrameWndClass frame

class SVOIOFrameWndClass : public CFrameWnd
{
	DECLARE_DYNCREATE(SVOIOFrameWndClass)
protected:
	SVOIOFrameWndClass();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOIOFrameWndClass)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~SVOIOFrameWndClass();

	// Generated message map functions
	//{{AFX_MSG(SVOIOFrameWndClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOIOFRAMEWNDCLASS_H__CDF9EB40_A939_4B1A_AF5C_D45CE408BB2A__INCLUDED_)

