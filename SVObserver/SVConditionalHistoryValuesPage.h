//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistoryValuesPage
//* .File Name       : $Workfile:   SVConditionalHistoryValuesPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:02:28  $
//******************************************************************************

#if !defined(AFX_SVCONDITIONALHISTORYVALUESPAGE_H__704459BA_6EC0_442F_9017_4899FA61C649__INCLUDED_)
#define AFX_SVCONDITIONALHISTORYVALUESPAGE_H__704459BA_6EC0_442F_9017_4899FA61C649__INCLUDED_
#pragma once

#include "ISVPropertyPageDialog.h"
#include "SVOutputInfoListTreeCtrl.h"
#include "SVConditionalHistoryBasePage.h"

/////////////////////////////////////////////////////////////////////////////
// SVConditionalHistoryValuesPage dialog

class SVConditionalHistorySheet;

class SVConditionalHistoryValuesPage : public SVConditionalHistoryBasePage
{
public:
	SVConditionalHistoryValuesPage( SVConditionalHistorySheet* pParentSheet, const CString& szCaption, int id = IDD );
	~SVConditionalHistoryValuesPage();

	// ISVPropertyPageDialog
	virtual bool QueryAllowExit();
	virtual void OnOK();

// Dialog Data
	//{{AFX_DATA(SVConditionalHistoryValuesPage)
	enum { IDD = IDD_CONDITIONAL_HISTORY_PAGE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVConditionalHistoryValuesPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVConditionalHistoryValuesPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnSelchangeListSelected();

	virtual void InitPage( SVInspectionProcess* pInspection );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVCONDITIONALHISTORYVALUESPAGE_H__704459BA_6EC0_442F_9017_4899FA61C649__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditionalHistoryValuesPage.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:02:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:19:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/