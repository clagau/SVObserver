//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQEntryDialogRemotePageClass
//* .File Name       : $Workfile:   SVPPQEntryDialogRemotePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:31:06  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVLibrary/SVDataItemManagerTemplate.h"
#include "SVObjectLibrary/SVIOEntryHostStruct.h"
#pragma endregion Includes

class SVPPQEntryDialogPropertySheetClass;

class SVPPQEntryDialogRemotePageClass : public CPropertyPage
{
	DECLARE_DYNCREATE(SVPPQEntryDialogRemotePageClass)

	//{{AFX_MSG(SVPPQEntryDialogRemotePageClass)
	protected:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnAddButton();
	afx_msg void OnRemoveButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVPPQEntryDialogRemotePageClass)
	public:
	virtual void OnOK() override;
	virtual BOOL OnApply() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

	//{{AFX_DATA(SVPPQEntryDialogRemotePageClass)
	public:
	enum { IDD = IDD_PPQENTRY_REMOTE_INPUT_PAGE };
	CListBox	selectedInputListCtrl;
	CListBox	availableInputListCtrl;
	CString	StrCurPos;
	//}}AFX_DATA

	friend class SVPPQEntryDialogPropertySheetClass;

public:
	SVPPQEntryDialogRemotePageClass();

	virtual ~SVPPQEntryDialogRemotePageClass();

protected:
	SVPPQEntryDialogPropertySheetClass* m_pSheet;
	BOOL m_bIsTaken;

private:
	typedef SVDataItemManagerTemplate< SVIOEntryHostStructPtr > SVDataItemManager;

	SVDataItemManager m_AvailableItems;
	SVDataItemManager m_SelectedItems;
};

