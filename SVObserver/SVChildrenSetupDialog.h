//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVChildrenSetupDialog
//* .File Name       : $Workfile:   SVChildrenSetupDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Jul 2014 18:19:34  $
//******************************************************************************

#pragma once
#pragma region Includes
#include "InspectionEngine/SVClassInfoStruct.h"
#pragma endregion Includes

namespace SvIe
{
class SVTaskObjectClass;
class SVTaskObjectListClass;
}

class SVIPDoc;
class SVObjectClass;

class SVChildrenSetupDialog : public CDialog
{
public:
	SVChildrenSetupDialog( CWnd* pParent = nullptr );
	virtual ~SVChildrenSetupDialog();

//******************************************************************************
// Message Handler(s):
//******************************************************************************
protected:
	//{{AFX_MSG(SVChildrenSetupDialog)
	afx_msg void OnAddButton();
	bool CreateSelectedResults(SvIe::SVClassInfoStruct& rChildInfo);
	afx_msg void OnRemoveButton();
	virtual BOOL OnInitDialog() override;
	virtual void OnCancel() override;
	virtual void OnOK() override;
	afx_msg void OnSetupButton();
	afx_msg void OnPublishButton();
	afx_msg void OnItemChangedChildrenList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedAvailableChildrenList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


//******************************************************************************
// Virtual(s):
//******************************************************************************

	//{{AFX_VIRTUAL(SVChildrenSetupDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************
protected:
	void redrawLists();
	BOOL checkOkToDelete( SvIe::SVTaskObjectClass* pTaskObject );

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	SvIe::SVClassInfoStructVector* m_pAvailableChildrenList;
	SvIe::SVTaskObjectListClass* m_pParentObject;
	std::string					m_Title;
	BOOL						m_AllowMultipleChildrenInstances;
	SVIPDoc*					m_pDocument;

protected:
	SVObjectClass* m_pParentOwner;

	//{{AFX_DATA(SVChildrenSetupDialog)
	enum { IDD = IDD_CHILDREN_DIALOG };
	CListCtrl m_ChildrenListCtrl;
	CListCtrl m_AvailableChildrenListCtrl;
	//}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

