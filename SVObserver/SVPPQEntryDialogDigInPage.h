//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQEntryDialogDigInPage
//* .File Name       : $Workfile:   SVPPQEntryDialogDigInPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:31:02  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVLibrary/SVDataItemManagerTemplate.h"
#include "SVLibrary/SVIOEntryHostStruct.h"
#pragma endregion Includes

class SVPPQEntryDialogPropertySheetClass;

class SVPPQEntryDialogDigInPageClass : public CPropertyPage
{
	DECLARE_DYNCREATE(SVPPQEntryDialogDigInPageClass)

	//{{AFX_MSG(SVPPQEntryDialogDigInPageClass)
	protected:
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	afx_msg void OnAddButton();
	afx_msg void OnRemoveButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVPPQEntryDialogDigInPageClass)
	public:
	virtual BOOL OnApply() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	//}}AFX_VIRTUAL

	//{{AFX_DATA(SVPPQEntryDialogDigInPageClass)
	public:
	enum { IDD = IDD_PPQENTRY_DIGITAL_INPUT_PAGE };
	CListBox m_SelectedInputCtrl;
	CListBox m_AvailableInputCtrl;
	CString m_CurrentPos;
	//}}AFX_DATA

	friend class SVPPQEntryDialogPropertySheetClass;

public:
	SVPPQEntryDialogDigInPageClass();
	virtual ~SVPPQEntryDialogDigInPageClass();

protected:
	SVPPQEntryDialogPropertySheetClass* m_pSheet;
	BOOL m_bIsTaken;

private:
	typedef SVDataItemManagerTemplate< SVIOEntryHostStructPtr > SVDataItemManager;

	SVDataItemManager m_AvailableItems;
	SVDataItemManager m_SelectedItems;
};

