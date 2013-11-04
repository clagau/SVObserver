//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistoryConditionalPage
//* .File Name       : $Workfile:   SVConditionalHistoryConditionalPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:00:36  $
//******************************************************************************

#if !defined(AFX_SVCONDITIONALHISTORYCONDITIONALPAGE_H__66E27060_797F_43D7_B929_EF2C2ACE15C3__INCLUDED_)
#define AFX_SVCONDITIONALHISTORYCONDITIONALPAGE_H__66E27060_797F_43D7_B929_EF2C2ACE15C3__INCLUDED_
#pragma once

#include "ISVPropertyPageDialog.h"
#include "SVOutputInfoListTreeCtrl.h"
#include "SVConditionalHistoryBasePage.h"

/////////////////////////////////////////////////////////////////////////////
// SVConditionalHistoryConditionalPage dialog

class SVConditionalHistorySheet;

class SVConditionalHistoryConditionalPage : public SVConditionalHistoryBasePage
{
public:
	SVConditionalHistoryConditionalPage( SVConditionalHistorySheet* pParentSheet, const CString& szCaption, int id = IDD );
	~SVConditionalHistoryConditionalPage();

	// ISVPropertyPageDialog
	virtual bool QueryAllowExit();
	virtual void OnOK();

// Dialog Data
	//{{AFX_DATA(SVConditionalHistoryConditionalPage)
	enum { IDD = IDD_CONDITIONAL_HISTORY_PAGE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVConditionalHistoryConditionalPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVConditionalHistoryConditionalPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnSelchangeListSelected();

	virtual void InitPage( SVInspectionProcess* pInspection );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVCONDITIONALHISTORYCONDITIONALPAGE_H__66E27060_797F_43D7_B929_EF2C2ACE15C3__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditionalHistoryConditionalPage.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:00:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:19:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/