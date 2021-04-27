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

#pragma region Includes
#include "SVPPQEntryDialogDigInPage.h"
#include "SVPPQEntryDialogRemotePage.h"
#pragma endregion Includes

class SVPPQObject;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCameraPageInfoStruct
// -----------------------------------------------------------------------------
// .Description : 
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

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVPPQEntryDialogCameraPageClass
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
class SVPPQEntryDialogCameraPageClass : public CPropertyPage
{

	DECLARE_DYNCREATE(SVPPQEntryDialogCameraPageClass)

//******************************************************************************
protected:
	// Generated message map functions
	//{{AFX_MSG(SVPPQEntryDialogCameraPageClass)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnAddButton();
	afx_msg void OnRemoveButton();
	virtual void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class SVPPQEntryDialogPropertySheetClass;

public:
	SVPPQEntryDialogCameraPageClass();
	virtual ~SVPPQEntryDialogCameraPageClass();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVPPQEntryDialogCameraPageClass)
	public:
	virtual BOOL OnApply() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL


//******************************************************************************
// Data Element(s):
//******************************************************************************
public:

// Dialog Data
	//{{AFX_DATA(SVPPQEntryDialogCameraPageClass)
	enum { IDD = IDD_PPQENTRY_CAMERA_PAGE };
	CListBox	m_SelectedList;
	CListBox	m_AvailableList;
	CString		m_CurrentPos;
	//}}AFX_DATA

protected:
	SVPPQEntryDialogPropertySheetClass* m_pSheet;
	BOOL								m_bIsTaken;
};

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVPPQEntryDialogPropertySheetClass
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
class SVPPQEntryDialogPropertySheetClass : public CPropertySheet
{
// Construction
public:
	explicit SVPPQEntryDialogPropertySheetClass(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	explicit SVPPQEntryDialogPropertySheetClass(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	virtual ~SVPPQEntryDialogPropertySheetClass();

protected:
	void addPages();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPPQEntryDialogPropertySheetClass)
	public:
	virtual INT_PTR DoModal() override;
	//}}AFX_VIRTUAL
	BOOL OkToAdd();

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

