//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQEntryDialog
//* .File Name       : $Workfile:   SVPPQEntryDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   28 Feb 2014 08:31:08  $
//******************************************************************************

#pragma once

#include "SVPPQEntryDialogDigInPage.h"
#include "SVPPQEntryDialogRemotePage.h"

class SVPPQObject;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCameraPageInfoStruct
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Export
//	 Type				Name				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :04.08.1999 RO 			First Implementation
////////////////////////////////////////////////////////////////////////////////
struct SVCameraPageInfoStruct
{
	SVCameraPageInfoStruct()
	{
		Channel = 0;
	};

//******************************************************************************
// Data Element(s):
//******************************************************************************

	DWORD		  Channel;
};



class SVPPQEntryDialogPropertySheetClass;

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVPPQEntryDialogCameraPageClass
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVPPQEntryDialogCameraPageClass : public CPropertyPage
{
//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************

//******************************************************************************
// MFC Declarations:
//******************************************************************************

	DECLARE_DYNCREATE(SVPPQEntryDialogCameraPageClass)

//******************************************************************************
// Manual Generated Message Map Entries
//******************************************************************************

//******************************************************************************
// Class Wizard Generated Message Map Entries
//******************************************************************************
protected:
	// Generated message map functions
	//{{AFX_MSG(SVPPQEntryDialogCameraPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnRemoveButton();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

	friend class SVPPQEntryDialogPropertySheetClass;

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVPPQEntryDialogCameraPageClass();

//******************************************************************************
// Destructor(s):
//******************************************************************************

	~SVPPQEntryDialogCameraPageClass();

//******************************************************************************
// Operator(s):
//******************************************************************************

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Process;
//******************************************************************************

//******************************************************************************
// Operation(s) Of Visual Representation:
//******************************************************************************

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVPPQEntryDialogCameraPageClass)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

//******************************************************************************
// Debug Operation(s):
//******************************************************************************

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:

// Dialog Data
	//{{AFX_DATA(SVPPQEntryDialogCameraPageClass)
	enum { IDD = IDD_PPQENTRY_CAMERA_PAGE };
	CListBox	selectedList;
	CListBox	availableList;
	CString		StrCurPos;
	//}}AFX_DATA


protected:
	SVPPQEntryDialogPropertySheetClass* m_pSheet;
	BOOL								m_bIsTaken;
};

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVPPQEntryDialogPropertySheetClass
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVPPQEntryDialogPropertySheetClass : public CPropertySheet
{
	DECLARE_DYNAMIC(SVPPQEntryDialogPropertySheetClass)

// Construction
public:
	SVPPQEntryDialogPropertySheetClass(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVPPQEntryDialogPropertySheetClass(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~SVPPQEntryDialogPropertySheetClass();

// Attributes
public:

// Operations
public:
protected:

	void addPages();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPPQEntryDialogPropertySheetClass)
	public:
	virtual INT_PTR DoModal();
	//}}AFX_VIRTUAL
	BOOL OkToAdd();

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(SVPPQEntryDialogPropertySheetClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	SVPPQObject* m_pPPQ;
	long		m_lCurrentPosition;
	BOOL		m_bIsTaken;

protected:

	SVPPQEntryDialogCameraPageClass		m_oCameraPage;
	SVPPQEntryDialogDigInPageClass		m_oDigitalPage;
	SVPPQEntryDialogRemotePageClass		m_oRemotePage;
};

