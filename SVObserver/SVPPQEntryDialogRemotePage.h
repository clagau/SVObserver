//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVIOLibrary/SVIOEntryHostStruct.h"
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
	CListBox	m_selectedInputCtrl;
	CListBox	m_availableInputCtrl;
	CString	StrCurPos;
	//}}AFX_DATA

	friend class SVPPQEntryDialogPropertySheetClass;

public:
	SVPPQEntryDialogRemotePageClass();

	virtual ~SVPPQEntryDialogRemotePageClass() = default;

protected:
	SVPPQEntryDialogPropertySheetClass* m_pSheet{nullptr};
	bool m_bIsTaken{false};

private:
	std::map<std::string, SVIOEntryHostStructPtr> m_AvailableItems;
	std::map<std::string, SVIOEntryHostStructPtr> m_SelectedItems;
};

