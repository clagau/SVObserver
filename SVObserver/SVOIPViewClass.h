//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOIPViewClass
//* .File Name       : $Workfile:   SVOIPViewClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:08:26  $
//******************************************************************************

#pragma once

/////////////////////////////////////////////////////////////////////////////
// SVOIPViewClass view

class SVOIPViewClass : public CView
{
protected:
	SVOIPViewClass();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(SVOIPViewClass)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOIPViewClass)
	protected:
	virtual void OnDraw(CDC* pDC) override;      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~SVOIPViewClass();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(SVOIPViewClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
