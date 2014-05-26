// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropTree.h
// * .File Name       : $Workfile:   PROPTREE.H  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   26 May 2014 10:16:02  $
// ******************************************************************************

// PropertyTreeCtrl.h : header file
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

#if !defined(AFX_PROPT_H__386AA426_6FB7_4B4B_9563_C4CC045BB0C9__INCLUDED_)
#define AFX_PROPT_H__386AA426_6FB7_4B4B_9563_C4CC045BB0C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PropTreeCtrl.h"
#include "PropTreeInfo.h"

#include "PropTreeItem.h"
#include "PropTreeItemStatic.h"
#include "PropTreeItemEdit.h"
#include "PropTreeItemCombo.h"
#include "PropTreeItemFile.h"
#include "SVRPropTreeState.h"
class SVRPropTree;

typedef bool (CALLBACK* ENUMPROPITEMPROC)(SVRPropTree*, SVRPropertyItem*, LPARAM);

// SVRPropTree window styles
enum { PTS_NOTIFY					= 0x00000001 };

// SVRPropTree HitTest return codes
enum
{
	HTPROPFIRST     = 50,

	HTLABEL         = HTPROPFIRST + 0,
	HTCOLUMN        = HTPROPFIRST + 1,
	HTEXPAND        = HTPROPFIRST + 2,
	HTATTRIBUTE     = HTPROPFIRST + 3,
	HTCHECKBOX      = HTPROPFIRST + 4,
};

const int g_buttonSpace = 4;

// SVRPropTree WM_NOTIFY notification structure
typedef struct _NMPROPTREE
{
	NMHDR            hdr;
	SVRPropertyItem* pItem;
} NMPROPTREE, *PNMPROPTREE, FAR *LPNMPROPTREE;

// SVRPropTree specific Notification Codes
enum
{
	PTN_FIRST               = -1100,

	PTN_INSERTITEM          = PTN_FIRST - 1,
	PTN_DELETEITEM          = PTN_FIRST - 2,
	PTN_DELETEALLITEMS      = PTN_FIRST - 3,
	PTN_ITEMCHANGED         = PTN_FIRST - 5,
	PTN_ITEMBUTTONCLICK     = PTN_FIRST - 6,
	PTN_SELCHANGE           = PTN_FIRST - 7,
	PTN_ITEMEXPANDING       = PTN_FIRST - 8,
	PTN_COLUMNCLICK         = PTN_FIRST - 9,
	PTN_PROPCLICK           = PTN_FIRST - 10,
	PTN_CHECKCLICK          = PTN_FIRST - 12,
	PTN_QUERY_SHOW_BUTTON   = PTN_FIRST - 13,
};
/////////////////////////////////////////////////////////////////////////////
// SVRPropTree window

class SVRPropTree : public CWnd
{
// Construction
public:
	SVRPropTree();
	virtual ~SVRPropTree();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	void EnableListBoxStyle(bool bEnable = true);

// Attributes/Operations
	static CFont* GetNormalFont();
	static CFont* GetBoldFont();

	// Returns the root item of the tree
	SVRPropertyItem* GetRootItem();

	// Returns the focused item or NULL for none
	SVRPropertyItem* GetFocusedItem();

	// Enumerates an item and all its child items
	bool EnumItems(SVRPropertyItem* pItem, ENUMPROPITEMPROC proc, LPARAM lParam = 0L);

	// Insert a created SVRPropertyItem into the control
	SVRPropertyItem* InsertItem(SVRPropertyItem* pItem, SVRPropertyItem* pParent = NULL);

	// Delete an item and ALL its children
	void DeleteItem(SVRPropertyItem* pItem);

	// Delete all items from the tree
	void DeleteAllItems();

	// Return the splitter position
	LONG GetColumn();

	// Set the splitter position
	void SetColumn(LONG nColumn);

	// Sets the focused item
	void SetFocusedItem(SVRPropertyItem* pItem);

	// Show or hide the info text
	void ShowInfoText(bool bShow = true);

	// Returns TRUE if the item is visible (its parent is expanded)
	bool IsItemVisible(SVRPropertyItem* pItem);

	// Ensures that an item is visible
	void EnsureVisible(SVRPropertyItem* pItem);

	// do a hit test on the control (returns a HTxxxx code)
	LONG HitTest(const POINT& pt);

	// find an item by a location
	SVRPropertyItem* FindItem(const POINT& pt);

	// find an item by item id
	SVRPropertyItem* FindItem(UINT nCtrlID);

	//
	// functions used by SVRPropertyItem (you normally dont need to call these directly)
	//

	void AddToVisibleList(SVRPropertyItem* pItem);
	void ClearVisibleList();

	void SetOriginOffset(LONG nOffset);
	void UpdatedItems();
	void UpdateMoveAllItems();
	void RefreshItems(SVRPropertyItem* pItem = NULL);

	// enable or disable tree input
	void DisableInput(bool bDisable = true);
	bool IsDisableInput();

	bool IsSingleSelection();

	SVRPropertyItem* GetVisibleList();
	CWnd* GetCtrlParent();

	const POINT& GetOrigin();

	void SelectItems(SVRPropertyItem* pItem, bool bSelect = true);

	// Focus on the first visible item
	SVRPropertyItem* FocusFirst();

	// Focus on the last visible item
	SVRPropertyItem* FocusLast();

	// Focus on the previous item
	SVRPropertyItem* FocusPrev();
	
	// Focus on the next item
	SVRPropertyItem* FocusNext();

	LRESULT SendNotify(UINT nNotifyCode, SVRPropertyItem* pItem = NULL);

	bool SaveState(SVRPropTreeState& rState);
	bool RestoreState(const SVRPropTreeState& rState);

	void SetExpandColumnSize(int iSize=16);


	int PROPTREEITEM_EXPANDCOLUMN;//         = 16			// width of the expand column
	static const int PROPTREEITEM_COLRNG;//        = 5			// width of splitter
	static const int PROPTREEITEM_DEFHEIGHT;//     = 21,			// default heigt of an item
	static const int PROPTREEITEM_SPACE;//         = 5, 			// default horz spacing
	static const int PROPTREEITEM_EXPANDBOX;//     = 9, 			// size of the expand box
	static const int PROPTREEITEM_CHECKBOX;//      = 14,			// size of the check box
	static const int PNINDENT;//                   = 16,			// child level indent
	static const int PROPTREEITEM_EXPANDBOXHALF;// = (PROPTREEITEM_EXPANDBOX / 2)

protected:

	bool SaveState(CString sItemName, SVRPropertyItem* pItem, SVRPropTreeState& rState);
	bool RestoreState(CString sItemName, SVRPropertyItem* pItem, const SVRPropTreeState& rState);

	// Resize the child windows to fit the exact dimensions the SVRPropTree control
	void ResizeChildWindows(int cx, int cy);

	// Initialize global resources, brushes, fonts, etc.
	void InitGlobalResources();

	// Free global resources, brushes, fonts, etc.
	void FreeGlobalResources();

	// Recursive version of DeleteItem
	void Delete(SVRPropertyItem* pItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVRPropTree)
	//}}AFX_VIRTUAL

protected:
	// Actual tree control
	SVRPropTreeCtrl	m_List;

	// Descriptive control
	SVRPropertyInfo	m_Info;

	// TRUE to show info control
	bool					m_bShowInfo;

	// Height of the info control
	LONG					m_nInfoHeight;

	// Root level tree item
	SVRPropertyItem	m_Root;

	// Linked list of visible items
	SVRPropertyItem*	m_pVisbleList;

	// Pointer to the focused item (selected)
	SVRPropertyItem*	m_pFocus;

	// PropTree scroll position. x = splitter position, y = vscroll position
	CPoint				m_Origin;

	// auto generated last created ID
	UINT					m_nLastUID;

	// Number of SVRPropTree controls in the current application
	static UINT			s_nInstanceCount;

	static CFont*		s_pNormalFont;
	static CFont*		s_pBoldFont;

	bool					m_bDisableInput;

	// Used for enumeration
	static SVRPropertyItem*	s_pFound;

	UINT	m_nID;

	bool m_bLockWindowUpdate;

public:
	bool m_bShift;

// Implementation
private:
	static bool CALLBACK EnumFindItem(SVRPropTree* pProp, SVRPropertyItem* pItem, LPARAM lParam);
	static bool CALLBACK EnumSelectAll(SVRPropTree*, SVRPropertyItem* pItem, LPARAM lParam);
	static bool CALLBACK EnumMoveAll(SVRPropTree*, SVRPropertyItem* pItem, LPARAM);
	static bool CALLBACK EnumRefreshAll(SVRPropTree*, SVRPropertyItem* pItem, LPARAM);

	// Generated message map functions
protected:
	//{{AFX_MSG(SVRPropTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPT_H__386AA426_6FB7_4B4B_9563_C4CC045BB0C9__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\PropertyTree\PROPTREE.H_v  $
 * 
 *    Rev 1.3   26 May 2014 10:16:02   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  894
 * SCR Title:  Enhancements to Adjust Tool Position Dialog
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   added constant g_buttonSpace and used it instead of magic numbers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Jan 2014 08:42:02   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  874
 * SCR Title:  Cleanup Visual Studio Project Files
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed unnecessary include PropTreeItemColor.h.
 * Removed dead (commented out) code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Jul 2013 16:26:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   06 Jun 2013 08:02:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix keyboard navigation using tab to move from field to field.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 16:42:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 May 2005 14:32:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   centralized constants
 * fixed potential crash
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Jul 2004 16:16:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented Save / Restore state
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jul 2003 08:41:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added PTN_QUERY_SHOW_BUTTON
 * changed #define to enum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 15:42:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/