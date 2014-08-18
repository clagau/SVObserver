//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : NameSelectionTreeCtrl
//* .File Name       : $Workfile:   NameSelectionTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 Aug 2014 15:28:46  $
//******************************************************************************

#pragma once
#include <boost/function.hpp>
#include <map>
#include "NameSelectionList.h"

class CTreeCtrl;

// this callback occurs when the user attempts to select or deselect an item.
// the first parameter is the current state of the item. 
// The second parameter is the index of the item in the tree
// the return value indicates if the object can be selected/deselected.
typedef boost::function<bool (bool, int) > CanSelectObjectCallbackFn;

class NameSelectionTreeCtrl : public CTreeCtrl
{
	typedef std::map <int, HTREEITEM> MapIndexToHandleType;
	
	DECLARE_DYNCREATE(NameSelectionTreeCtrl)

public:
	NameSelectionTreeCtrl();
	virtual ~NameSelectionTreeCtrl();
	
	void SetSingleSelect(bool bSingle);
	void SetAllowWholeArray();
	void SetCanSelectObjectCallback(CanSelectObjectCallbackFn fn);
	void AllowNodeItemChecks(bool p_bAllow);
	void InitTreeCtrl();
	void Populate(const NameSelectionList& rList);
	bool SelectItemByIndex(int index);
	void DeselectAll();

protected:
	void SetBranchChecks(HTREEITEM hItem , bool p_bNewBranchState);

	bool GetCheckState(HTREEITEM hItem);
	bool SetCheckState(HTREEITEM hItem, bool bCheck, bool bSelect = true);
	bool Click(HTREEITEM hItem);
	void UpdateNodeStateColor(HTREEITEM hItem, long& p_rlFlags);

// Attributes
	CImageList m_StateImageList;

	HTREEITEM m_hLastChecked;
	bool m_bSingleSelect;
	bool m_bAllowWholeArray;
	bool m_bAllowBranchCheck;
	MapIndexToHandleType m_mapIndexes;
	CanSelectObjectCallbackFn m_fnCanSelectObject;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NameSelectionTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation

	// Generated message map functions
protected:
	//{{AFX_MSG(NameSelectionTreeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\NameSelectionTreeCtrl.h_v  $
 * 
 *    Rev 1.2   15 Aug 2014 15:28:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed DeselectItems
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Aug 2014 13:27:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added DeselectAll method.
 * Added DeselectItems method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Apr 2014 16:26:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
