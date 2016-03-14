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

class SVIPDoc;
class SVTaskObjectClass;
class SVClassInfoStructListClass;
class SVTaskObjectListClass;
class SVObjectClass;

class SVChildrenSetupDialogClass : public CDialog
{
public:
	SVChildrenSetupDialogClass( CWnd* pParent = NULL );

//******************************************************************************
// Message Handler(s):
//******************************************************************************
protected:
	//{{AFX_MSG(SVChildrenSetupDialogClass)
	afx_msg void OnAddButton();
	afx_msg void OnRemoveButton();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSetupButton();
	afx_msg void OnPublishButton();
	afx_msg void OnItemChangedChildrenList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedAvailableChildrenList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


//******************************************************************************
// Virtual(s):
//******************************************************************************

	//{{AFX_VIRTUAL(SVChildrenSetupDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************
protected:
	void redrawLists();
	BOOL checkOkToDelete( SVTaskObjectClass* pTaskObject );

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	SVClassInfoStructListClass*	m_pAvailableChildrenList;
	SVTaskObjectListClass*		m_pParentObject;
	CString StrTitle;
	BOOL						m_AllowMultipleChildrenInstances;
	SVIPDoc*					m_pDocument;

protected:
	SVObjectClass* m_pParentOwner;

	//{{AFX_DATA(SVChildrenSetupDialogClass)
	enum { IDD = IDD_CHILDREN_DIALOG };
	CListCtrl m_ChildrenListCtrl;
	CListCtrl m_AvailableChildrenListCtrl;
	//}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

