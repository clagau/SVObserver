// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemEdit.h
// * .File Name       : $Workfile:   PropTreeItemEdit.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   05 Jul 2013 14:35:50  $
// ******************************************************************************

// PropertyItemEdit.h : header file
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#include "SVRPropTreeItem.h"
#include "SVNotifyButton.h"
#pragma endregion Includes

#define MSG_TO_PARENT WM_USER+10

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyItemEdit window

class SVRPropertyItemEdit : public CEdit, public SVRPropertyItem
{
// Construction
public:
	SVRPropertyItemEdit();
	virtual ~SVRPropertyItemEdit();

// Attributes
public:
	// The attribute area needs drawing
	virtual void DrawAttribute(CDC* pDC, const RECT& rc) override;

	// Retrieve the item's attribute type
	virtual int GetItemType(void) override;

	// Retrieve the item's attribute value
	virtual bool GetItemValue(BYTE& pbVal) override;
	virtual bool GetItemValue(short& piVal) override;
	virtual bool GetItemValue(USHORT& puiVal) override;
	virtual bool GetItemValue(long& plVal) override;
	virtual bool GetItemValue(ULONG& pulVal) override;
	virtual bool GetItemValue(int& pintVal) override;
	virtual bool GetItemValue(UINT& puintVal) override;
	virtual bool GetItemValue(float& pfltVal) override;
	virtual bool GetItemValue(double& pdblVal) override;
	virtual bool GetItemValue(SVString& rVal) override;
	virtual bool GetItemValue(VARIANT& vtVal) override;
	virtual bool GetItemValue(bool& bVal) override;

	// Set the item's attribute value
	virtual bool SetItemValue(const BYTE bVal) override;
	virtual bool SetItemValue(const short iVal) override;
	virtual bool SetItemValue(const USHORT uiVal) override;
	virtual bool SetItemValue(const long lVal) override;
	virtual bool SetItemValue(const ULONG ulVal) override;
	virtual bool SetItemValue(const int intVal) override;
	virtual bool SetItemValue(const UINT uintVal) override;
	virtual bool SetItemValue(const float fltVal) override;
	virtual bool SetItemValue(const double dblVal) override;
	virtual bool SetItemValue(LPCTSTR pVal) override;
	virtual bool SetItemValue(const bool bVal) override;

	// Set the item's attribute reference
	virtual bool SetItemValuePtr(BYTE& bVal) override;
	virtual bool SetItemValuePtr(short& iVal) override;
	virtual bool SetItemValuePtr(USHORT& puiVal) override;
	virtual bool SetItemValuePtr(long& plVal) override;
	virtual bool SetItemValuePtr(ULONG& pulVal) override;
	virtual bool SetItemValuePtr(int& pintVal) override;
	virtual bool SetItemValuePtr(UINT& puintVal) override;
	virtual bool SetItemValuePtr(float& pfltVal) override;
	virtual bool SetItemValuePtr(double& pdblVal) override;
	virtual bool SetItemValuePtr(SVString& rVal) override;

	// @cmember Called when attribute area has changed size.
	virtual void OnMove() override;

	// @cmember Called when the item needs to refresh its data.
	virtual void OnRefresh() override;

	// @cmember Called when the item needs to commit its changes.
	virtual void OnCommit() override;
	
	virtual void OnUndoCommit() override;

	// @cmember Called to activate the item.
	virtual void OnActivate() override;
	
	// Called to tell the control that is will loss the focus and no new focus will be set. 
	//(Will be set a new focus, OnKillFocus will be fired automatically.)
	virtual void OnLossFocus() override;

	void SetButtonText( LPCTSTR Text );

protected:
	_variant_t	m_vtData;
	SVString*   m_pValue;
	SVString    m_Previous;
	SVString    m_ButtonText;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVRPropertyItemEdit)
	//}}AFX_VIRTUAL

// Implementation
public:

	BOOL CreateEdit(DWORD dwStyle = WS_CHILD|ES_AUTOHSCROLL);

	// Generated message map functions
protected:
	//{{AFX_MSG(SVRPropertyItemEdit)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillfocusReflect();
	afx_msg void OnKillFocus(CWnd *pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult ) override;
	void ButtonClicked();
	void DisplayButton();


	DECLARE_MESSAGE_MAP()

private:

	SVNotifyButton m_btnDots;

	bool m_bShowButton;
	bool m_bKillFocusWorkaround;

public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

