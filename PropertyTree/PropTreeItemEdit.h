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

#include "PropTreeItem.h"
#include "VariantObj.h"
#include "SVNotifyButton.h"

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
	virtual void DrawAttribute(CDC* pDC, const RECT& rc);

	// Retrieve the item's attribute type
	virtual int GetItemType(void);

	// Retrieve the item's attribute value
	virtual bool GetItemValue(BYTE& pbVal);
	virtual bool GetItemValue(short& piVal);
	virtual bool GetItemValue(USHORT& puiVal);
	virtual bool GetItemValue(long& plVal);
	virtual bool GetItemValue(ULONG& pulVal);
	virtual bool GetItemValue(int& pintVal);
	virtual bool GetItemValue(UINT& puintVal);
	virtual bool GetItemValue(float& pfltVal);
	virtual bool GetItemValue(double& pdblVal);
	virtual bool GetItemValue(CString& lpszVal);
    virtual bool GetItemValue(VARIANT& vtVal);
	virtual bool GetItemValue(bool& bVal);

	// Set the item's attribute value
	virtual bool SetItemValue(const BYTE bVal);
	virtual bool SetItemValue(const short iVal);
	virtual bool SetItemValue(const USHORT uiVal);
	virtual bool SetItemValue(const long lVal);
	virtual bool SetItemValue(const ULONG ulVal);
	virtual bool SetItemValue(const int intVal);
	virtual bool SetItemValue(const UINT uintVal);
	virtual bool SetItemValue(const float fltVal);
	virtual bool SetItemValue(const double dblVal);
	virtual bool SetItemValue(LPCTSTR pstrVal);
	virtual bool SetItemValue(const bool bVal);

	// Set the item's attribute reference
	virtual bool SetItemValuePtr(BYTE& bVal);
	virtual bool SetItemValuePtr(short& iVal);
	virtual bool SetItemValuePtr(USHORT& puiVal);
	virtual bool SetItemValuePtr(long& plVal);
	virtual bool SetItemValuePtr(ULONG& pulVal);
	virtual bool SetItemValuePtr(int& pintVal);
	virtual bool SetItemValuePtr(UINT& puintVal);
	virtual bool SetItemValuePtr(float& pfltVal);
	virtual bool SetItemValuePtr(double& pdblVal);
	virtual bool SetItemValuePtr(CString& strVal);

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

	void SetButtonText( const CString& sText );

protected:
	CVariantObj	m_vtData;
	CString*    m_pstr;
	CString     m_strPrevious;
	CString     m_strButtonText;

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

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult );
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

