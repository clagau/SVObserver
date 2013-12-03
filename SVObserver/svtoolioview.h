//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolIOView
//* .File Name       : $Workfile:   svtoolioview.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:45:40  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#ifndef SVTOOLIOVIEW_H
#define SVTOOLIOVIEW_H

//#if !defined(AFX_SVTOOLIOVIEW_H__A02486C1_8B4D_11D1_B749_97CA47472392__INCLUDED_)
//#define AFX_SVTOOLIOVIEW_H__A02486C1_8B4D_11D1_B749_97CA47472392__INCLUDED_

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
// .Title       : SVToolIOView
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
class SVToolIOView : public CListView
{

//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:

//******************************************************************************
// MFC Declarations:
//******************************************************************************
protected: 
	DECLARE_DYNCREATE( SVToolIOView )

//******************************************************************************
// Manual Generated Message Map Entries
//******************************************************************************

//******************************************************************************
// Class Wizard Generated Message Map Entries
//******************************************************************************
public:
protected:
// Generated message map functions
	//{{AFX_MSG(SVToolIOView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
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
	SVToolIOView();

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
// Standard destructor
	virtual ~SVToolIOView();

protected:
	CImageList ImageList;             // 20 Apr 1999 - frb.
	CImageList StateImageList;        // 20 Apr 1999 - frb.


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

	//{{AFX_VIRTUAL(SVToolIOView)
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

#ifndef _DEBUG  // Testversion in SVToolIOView.cpp
inline SVIODoc* SVToolIOView::GetDocument()
   { return (SVIODoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

/**************************************************************************/
/* INCLUDE CONTROL:                                                        */
/**************************************************************************/
#endif	// SVTOOLIOVIEW_H

//#endif // !defined(AFX_SVTOOLIOVIEW_H__A02486C1_8B4D_11D1_B749_97CA47472392__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\svtoolioview.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:45:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.5   18 Aug 2003 16:06:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed SVDigitalIOControlClass from class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.4   22 Apr 2003 17:07:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */