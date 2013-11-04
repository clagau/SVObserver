// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItem.h
// * .File Name       : $Workfile:   PropTreeItem.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   07 May 2013 07:48:30  $
// ******************************************************************************

// PropertyItem.h
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

#if !defined(AFX_PROPERTYITEM_H__22BD9C18_A68C_4BB8_B7FC_C4A7DA0E1EBF__INCLUDED_)
#define AFX_PROPERTYITEM_H__22BD9C18_A68C_4BB8_B7FC_C4A7DA0E1EBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class SVRPropTree;

class SVRPropertyItem
{
// Construction
public:
	SVRPropertyItem();
	virtual ~SVRPropertyItem();

// Attributes/Operations
public:
	// @cmember Property Item states.
	bool IsExpanded();
	bool IsSelected();
	bool IsChecked();
	bool IsReadOnly();
	bool IsActivated();
	bool IsHidden();
	bool CanShrink();
	bool CanHighlight();
	bool IsBold();

	void Select(bool bSelect = true);
	void Expand(bool bExpand = true);
	void Check(bool bCheck = true);
	void ReadOnly(bool bReadOnly = true);
	void HideItem(bool bHide = true);
	void SetCanShrink(bool bShrinkable = true);
	void SetCanHighlight(bool bCanHighlight);
	void SetBold(bool bBold = true);

	COLORREF GetForeColor();
	COLORREF GetBackColor();
	COLORREF SetForeColor(COLORREF rgb);
	COLORREF SetBackColor(COLORREF rgb);

	// Returns true if the item has a checkbox
	bool IsCheckBox();

	// Pass in true, for the item to have a checkbox
	void HasCheckBox(bool bCheckbox = true);

	// Returns TRUE if the point is on the expand button
	bool HitExpand(const POINT& pt);

	// Returns TRUE if the point is on the check box
	bool HitCheckBox(const POINT& pt);

	// Returns TRUE if the item is on the root level. Root level items don't have attribute areas
	bool IsRootLevel();

	// Returns the total height of the item and all its children
	LONG GetTotalHeight();

	// Set the items label text
	void SetLabelText(LPCTSTR sLabel);

	// Return the items label text
	LPCTSTR GetLabelText();

	// Set the items info (description) text
	void SetInfoText(LPCTSTR sInfo);

	// Get the items info (description) text
	LPCTSTR GetInfoText();

	// Set the item's ID
	void SetCtrlID(UINT nCtrlID);

	// Return the item's ID
	UINT GetCtrlID();

	// draw the item's non attribute area
	LONG UpdatePosition(const RECT& rc, LONG x, LONG y);

	// draw the item's non attribute area
	LONG DrawItem(CDC* pDC, const RECT& rc, LONG x, LONG y);

	// call to mark attribute changes
	void CommitChanges();

	// call to activate item attribute
	void Activate();

	//
	// Overrideables
	//

	// The attribute area needs drawing
	virtual void DrawAttribute(CDC* pDC, const RECT& rc);

	// Return the height of the item
	virtual LONG GetHeight();

	// Sets the height of the item
	virtual void SetHeight(long lHeight);

	// Retrieve the item's attribute type
	virtual int GetItemType(void);

	// Retrieve the item's attribute value
	virtual bool GetItemValue(bool& boolVal);
	virtual bool GetItemValue(BYTE& bVal);
	virtual bool GetItemValue(short& iVal);
	virtual bool GetItemValue(USHORT& uiVal);
	virtual bool GetItemValue(long& lVal);
	virtual bool GetItemValue(ULONG& ulVal);
	virtual bool GetItemValue(int& intVal);
	virtual bool GetItemValue(UINT& uintVal);
	virtual bool GetItemValue(float& fltVal);
	virtual bool GetItemValue(double& dblVal);
	virtual bool GetItemValue(CString& strVal);
    virtual bool GetItemValue(VARIANT& vtVal);

	// Set the item's attribute value
	virtual bool SetItemValue(const bool boolVal);
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

	// Set the item's attribute reference
	virtual bool SetItemValuePtr(BYTE& bVal);
	virtual bool SetItemValuePtr(short& iVal);
	virtual bool SetItemValuePtr(USHORT& uiVal);
	virtual bool SetItemValuePtr(long& lVal);
	virtual bool SetItemValuePtr(ULONG& ulVal);
	virtual bool SetItemValuePtr(int& intVal);
	virtual bool SetItemValuePtr(UINT& uintVal);
	virtual bool SetItemValuePtr(float& fltVal);
	virtual bool SetItemValuePtr(double& dblVal);
	virtual bool SetItemValuePtr(CString& strVal);

	// Called when attribute area has changed size
	virtual void OnMove();

	// Called when the item needs to refresh its data
	virtual void OnRefresh();

	// Called when the item needs to commit its changes
	virtual void OnCommit();

	// Called to undo committed changes
	virtual void OnUndoCommit();

	// Called to activate the item
	virtual void OnActivate();

	//
	// Usually only SVRPropTree should call these
	//

	void SetPropOwner(SVRPropTree* pProp);

	// Return the location of the PropItem
	const POINT& GetLocation();

	// TreeItem link pointer access
	SVRPropertyItem* GetParent();
	SVRPropertyItem* GetSibling();
	SVRPropertyItem* GetChild();
	SVRPropertyItem* GetNextVisible();

	void SetParent(SVRPropertyItem* pParent);
	void SetSibling(SVRPropertyItem* pSibling);
	void SetChild(SVRPropertyItem* pChild);
	void SetNextVisible(SVRPropertyItem* pVis);

	SVRPropTree* GetPropTree();
protected:
	// SVRPropTree class that this class belongs
	SVRPropTree*	m_pProp;

	// TreeItem label name
	CString			m_sLabel;

	// Descriptive info text
	CString			m_sInfo;

	// TreeItem location
	CPoint			m_loc;

	// TreeItem attribute size
	CRect				m_rc;

	// ID of control item (should be unique)
	UINT				m_nCtrlID;

protected:
	enum ItemStates
	{
		ItemSelected  = 0x00000001,
		ItemExpanded  = 0x00000002,
		ItemCheckbox  = 0x00000004,
		ItemChecked   = 0x00000008,
		ItemActivated = 0x00000010,
		ItemReadOnly  = 0x00000020,
		ItemHidden    = 0x00000040,
	};

	// @cmember Item state
	DWORD				m_dwState;

	// @cmember TRUE if item is activated.
	bool				m_bActivated;

	// @cmember TRUE if item has been commited once (activation).
	bool				m_bCommitOnce;

	bool				m_bCanShrink;

	// Rectangle position of the expand button (if contains one)
	CRect				m_rcExpand;

	// Rectangle position of the check box (if contains one)
	CRect				m_rcCheckbox;

	// link pointers
	SVRPropertyItem*		m_pParent;
	SVRPropertyItem*		m_pSibling;
	SVRPropertyItem*		m_pChild;
	SVRPropertyItem*		m_pVis;

	COLORREF m_rgbForeColor;
	COLORREF m_rgbBackColor;
	bool m_bCanHighlight;
	bool m_bBold;
	long m_lHeight;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYITEM_H__22BD9C18_A68C_4BB8_B7FC_C4A7DA0E1EBF__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\PropertyTree\PropTreeItem.h_v  $
 * 
 *    Rev 1.1   07 May 2013 07:48:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   19 Apr 2013 10:03:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added support for SVAdjustToolSizePositionDialog so the tab key will work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 16:42:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Feb 2005 10:09:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update SetHeight and SetWidth Methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   23 Jul 2004 07:58:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Add new method UpdatePosition to caculate the new window positions for the item and its children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Jul 2004 16:17:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added ForeColor, BackColor, Highlight, Bold
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   06 Oct 2003 09:09:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for denial (undo) of commitchanges
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Aug 2003 13:32:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added CanShrink option
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jul 2003 08:43:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed private items to protected
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 15:42:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/