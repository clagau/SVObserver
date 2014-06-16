//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetTreeCtrl
//* .File Name       : $Workfile:   SVToolSetTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Jun 2014 16:15:48  $
//******************************************************************************

#pragma once

class SVTaskObjectListClass;
class SVTaskObjectClass;
class SVToolSetTabViewClass;

class SVToolSetTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNCREATE(SVToolSetTreeCtrl)

protected:
	SVTaskObjectListClass* m_pTaskList;

	SVToolSetTabViewClass* GetView();
	const SVToolSetTabViewClass* GetView() const;

	HTREEITEM InsertTaskItem(HTREEITEM HParent, SVTaskObjectClass* pTask);
	HTREEITEM InsertToolSet(HTREEITEM HParent, SVTaskObjectListClass* pTask);

public:
	SVToolSetTreeCtrl();
	virtual ~SVToolSetTreeCtrl();
	bool SetTaskObjectList(SVTaskObjectListClass* pTaskList);
	void SetHasButtonsStyle();
	void SetNoEditLabelsStyle();
	void Rebuild();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVToolSetTreeCtrl)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(SVToolSetTreeCtrl)
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolSetTreeCtrl.h_v  $
 * 
 *    Rev 1.0   12 Jun 2014 16:15:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
