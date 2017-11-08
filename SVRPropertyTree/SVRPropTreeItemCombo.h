// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemCombo.h
// * .File Name       : $Workfile:   PropTreeItemCombo.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   30 Oct 2013 10:21:56  $
// ******************************************************************************

// PropertyItemCombo.h : header file
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

#include "SVRPropTreeItem.h"
#include "SVNotifyButton.h"
#pragma endregion Includes

class SVRPropertyItemCombo : public CComboBox, public SVRPropertyItem
{
// Construction
public:
	SVRPropertyItemCombo();
	virtual ~SVRPropertyItemCombo();

// Attributes
public:
	// The attribute area needs drawing
	virtual void DrawAttribute(CDC* pDC, const RECT& rc) override;

	// Retrieve the item's attribute value
	virtual bool GetItemValue( long& rValue ) override;
	virtual bool GetItemValue( UINT& rValue ) override;
	virtual bool GetItemValue(_variant_t& rValue) override;

	// Set the item's attribute value
	virtual bool SetItemValue(const LPARAM lparam);

	// Called when attribute area has changed size
	virtual void OnMove() override;

	// Called when the item needs to refresh its data
	virtual void OnRefresh() override;

	// Called when the item needs to commit its changes
	virtual void OnCommit() override;

	// Called to activate the item
	virtual void OnActivate() override;

	// Called to tell the control that is will loss the focus and no new focus will be set. 
	//(Will be set a new focus, OnKillFocus will be fired automatically.)
	virtual void OnLossFocus() override;

	// Create your combo box with your specified styles
	BOOL CreateComboBox(DWORD dwStyle = WS_CHILD|WS_VSCROLL|WS_TABSTOP|CBS_SORT|CBS_DROPDOWNLIST);

	// Create combo box with TRUE/FALSE selections
	BOOL CreateComboBoxBool();

	// Set the height for the dropdown combo box
	void SetDropDownHeight(LONG nDropHeight);

	// Get the height of the dropdown combo box
	LONG GetDropDownHeight();

	void SetButtonText( LPCTSTR Text );

	void EnableButton( bool bEnable = true );	// must be done before Create

protected:
//	CVariantObj	m_vtData;
	LPARAM  m_lComboData;
	LONG    m_nDropHeight;
	std::string m_ButtonText;

// Operations
protected:
	void Initialize(void);
	LONG FindCBData(LPARAM lParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVRPropertyItemCombo)
	//}}AFX_VIRTUAL

	
	void ButtonClicked();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVRPropertyItemCombo)
	afx_msg void OnKillfocusReflect();
	afx_msg void OnKillFocus(CWnd*);
	afx_msg void OnSelendok();
	afx_msg void OnSelendcancel();
	//}}AFX_MSG
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult ) override;
	void StoreItemData();

	void DisplayButton();


	DECLARE_MESSAGE_MAP()

private:

	SVNotifyButton m_btnDots;

	bool m_bShowButton;
	bool m_bEnableButton;

};

