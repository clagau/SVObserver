//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistoryImagesPage
//* .File Name       : $Workfile:   SVConditionalHistoryImagesPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:59:18  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOutputInfoListTreeCtrl.h"
#include "SVConditionalHistoryBasePage.h"
#pragma endregion Includes

/////////////////////////////////////////////////////////////////////////////
// SVConditionalHistoryImagesPage dialog

class SVConditionalHistorySheet;

class SVConditionalHistoryImagesPage : public SVConditionalHistoryBasePage
{
public:
#pragma region Constructor
	SVConditionalHistoryImagesPage( SVConditionalHistorySheet* pParentSheet, const CString& szCaption, int id = IDD );
	virtual ~SVConditionalHistoryImagesPage();
#pragma endregion Constructor

	// ISVPropertyPageDialog
	virtual bool QueryAllowExit();
	virtual void OnOK();

// Dialog Data
	//{{AFX_DATA(SVConditionalHistoryImagesPage)
	enum { IDD = IDD_CONDITIONAL_HISTORY_PAGE };
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVConditionalHistoryImagesPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions
	//{{AFX_MSG(SVConditionalHistoryImagesPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnSelChangeListSelected();

	virtual void InitPage( SVInspectionProcess* pInspection );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditionalHistoryImagesPage.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:59:18   bWalter
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