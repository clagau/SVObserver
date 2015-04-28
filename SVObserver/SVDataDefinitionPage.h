//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataDefinitionPage
//* .File Name       : $Workfile:   SVDataDefinitionPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:04:20  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVDataDefinitionSheet.h"
#include "SVOutputInfoListTreeCtrl.h"
#pragma endregion Includes

class SVDataDefinitionPage : public CPropertyPage
{
public:
#pragma region Constructor
	SVDataDefinitionPage(SVDataDefinitionSheet* pParent, CString p_szCaption );   // standard constructor
	virtual ~SVDataDefinitionPage();
#pragma endregion Constructor

// Dialog Data
	//{{AFX_DATA(SVDataDefinitionPage)
	enum { IDD = IDD_DATA_DEF_LIST_PAGE };
	//}}AFX_DATA

	virtual void OnOK();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVDataDefinitionPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void InitPage( SVInspectionProcess* pInspection );
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
	bool CanSelectObjectCallback( SVObjectReference refObject, bool bCurrentState, int iIndex );

	// Generated message map functions
	//{{AFX_MSG(SVDataDefinitionPage)
	afx_msg void OnBtnClear();
	afx_msg void OnBtnClearAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

#pragma region Member Variables
private:
	CListBox m_lbSelected;
	SVOutputInfoListTreeCtrlClass m_Tree;
	CButton m_btnClearAll;
	CButton m_btnClear;
	SVDataDefinitionSheet* m_pSheet;
	bool m_bIsImagesPage;
#pragma endregion Member Variables
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDataDefinitionPage.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:04:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Mar 2008 09:22:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New Property Sheet for Data and image Definition lists
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/