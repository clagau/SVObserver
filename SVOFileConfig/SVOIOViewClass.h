// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIOViewClass
// * .File Name       : $Workfile:   SVOIOViewClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:34:26  $
// ******************************************************************************

#pragma once

/////////////////////////////////////////////////////////////////////////////
// SVOIOViewClass view

class SVOIOViewClass : public CView
{
protected:
	SVOIOViewClass();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(SVOIOViewClass)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOIOViewClass)
	protected:
	virtual void OnDraw(CDC* pDC) override;      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~SVOIOViewClass();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(SVOIOViewClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

