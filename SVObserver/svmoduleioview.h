//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVModuleIOView
//* .File Name       : $Workfile:   svmoduleioview.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:36:56  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#ifndef SVMODULEIOVIEW_H
#define SVMODULEIOVIEW_H

//#if !defined(AFX_SVMODULEIOVIEW_H__A02486C1_8B4D_11D1_B749_97CA47472392__INCLUDED_)
//#define AFX_SVMODULEIOVIEW_H__A02486C1_8B4D_11D1_B749_97CA47472392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

class SVIODoc;
////////////////////////////////////////////////////////////////////////////////
// Includes which are necessary to declare or define types, constants and macros
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//* CONSTANT(S):
//******************************************************************************

//******************************************************************************
//* MACRO(S):
//******************************************************************************

//******************************************************************************
//* TYPE(S):
//******************************************************************************

//******************************************************************************
//* FUNCTION DECLARATION(S):
//******************************************************************************

//******************************************************************************
//* CONSTANT VARIABLE(S):
//******************************************************************************

//******************************************************************************
//* VARIABLE(S):
//******************************************************************************

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVModuleIOView
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
class SVModuleIOView : public CListView
{

//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:

//******************************************************************************
// MFC Declarations:
//******************************************************************************
protected:
	DECLARE_DYNCREATE( SVModuleIOView )

//******************************************************************************
// Manual Generated Message Map Entries
//******************************************************************************

//******************************************************************************
// Class Wizard Generated Message Map Entries
//******************************************************************************
public:
protected:
// Generated message map functions
	//{{AFX_MSG(SVModuleIOView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
protected:
// Create from serialization only
	SVModuleIOView();

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
// Standard destructor
	virtual ~SVModuleIOView();

protected:
	CImageList ImageList;                     // 20 Apr 1999 - frb.
	CImageList StateImageList;                // 20 Apr 1999 - frb.

//******************************************************************************
// Operator(s):
//******************************************************************************

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:
	SVIODoc* GetDocument();
	// m_pSelection enthält die Auswahl des aktuellen CSVIOCntrItem-Objekts.
	// Für viele Anwendungen ist eine derartige Member-Variable nicht angemessen, um
	//  z.B. eine Mehrfachauswahl oder eine Auswahl von Objekten zu repräsentieren,
	//  die keine CSVIOCntrItem-Objekte sind. Dieser Auswahlmechanismus
	//  dient nur dazu, Ihnen bei den ersten Schritten zu helfen.

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(SVModuleIOView)
	public:
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnInitialUpdate(); // das erste mal nach der Konstruktion aufgerufen
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL


//******************************************************************************
// Debug Operation(s):
//******************************************************************************
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//******************************************************************************
// Data Element(s):
//******************************************************************************
};

#ifndef _DEBUG  // Testversion in SVModuleIOView.cpp
inline SVIODoc* SVModuleIOView::GetDocument()
   { return (SVIODoc*)m_pDocument; }
#endif


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

/**************************************************************************/
/* INCLUDE CONTROL:                                                        */
/**************************************************************************/
#endif	// SVMODULEIOVIEW_H

//#endif // !defined(AFX_SVMODULEIOVIEW_H__A02486C1_8B4D_11D1_B749_97CA47472392__INCLUDED_)

