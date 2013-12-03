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

#if !defined(AFX_PROPERTYITEMCOMBO_H__9916BC6F_751F_4B15_996F_3C9F6334A259__INCLUDED_)
#define AFX_PROPERTYITEMCOMBO_H__9916BC6F_751F_4B15_996F_3C9F6334A259__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PropTreeItem.h"

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyItemCombo window

#include "SVNotifyButton.h"

class SVRPropertyItemCombo : public CComboBox, public SVRPropertyItem
{
// Construction
public:
	SVRPropertyItemCombo();
	virtual ~SVRPropertyItemCombo();

// Attributes
public:
	// The attribute area needs drawing
	virtual void DrawAttribute(CDC* pDC, const RECT& rc);

	// Retrieve the item's attribute value
	virtual bool GetItemValue( long& l_Value );
	virtual bool GetItemValue( UINT& p_Value );
    virtual bool GetItemValue(VARIANT& vtVal);

	// Set the item's attribute value
	virtual bool SetItemValue(const LPARAM lparam);

	// Called when attribute area has changed size
	virtual void OnMove();

	// Called when the item needs to refresh its data
	virtual void OnRefresh();

	// Called when the item needs to commit its changes
	virtual void OnCommit();

	// Called to activate the item
	virtual void OnActivate();

	// Create your combo box with your specified styles
	BOOL CreateComboBox(DWORD dwStyle = WS_CHILD|WS_VSCROLL|WS_TABSTOP|CBS_SORT|CBS_DROPDOWNLIST);

	// Create combo box with TRUE/FALSE selections
	BOOL CreateComboBoxBool();

	// Set the height for the dropdown combo box
	void SetDropDownHeight(LONG nDropHeight);

	// Get the height of the dropdown combo box
	LONG GetDropDownHeight();

	void SetButtonText( const CString& sText );

	void EnableButton( bool bEnable = true );	// must be done before Create

protected:
//	CVariantObj	m_vtData;
	LPARAM  m_lComboData;
	LONG    m_nDropHeight;
	CString m_strButtonText;

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

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult );
	void StoreItemData();

	void DisplayButton();


	DECLARE_MESSAGE_MAP()

private:

	SVNotifyButton m_btnDots;

	bool m_bShowButton;
	bool m_bEnableButton;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPTREEITEMCOMBO_H__9916BC6F_751F_4B15_996F_3C9F6334A259__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\PropertyTree\PropTreeItemCombo.h_v  $
 * 
 *    Rev 1.2   30 Oct 2013 10:21:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added GetItemValue( UINT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Sep 2013 14:07:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 16:42:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Sep 2008 10:56:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected Focus and Change Notification logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Jun 2007 12:20:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 May 2005 14:34:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   allowed the right-side button to be completely removed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Aug 2003 13:37:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved SVNotfyButton to its own file
 * added ability to set button text
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jul 2003 08:51:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added conditional "..." button
 * cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 15:42:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/