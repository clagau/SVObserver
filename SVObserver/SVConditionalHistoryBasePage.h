//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistoryBasePage
//* .File Name       : $Workfile:   SVConditionalHistoryBasePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:00:22  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOGui\ISVPropertyPageDialog.h"
#include "SVOutputInfoListTreeCtrl.h"
#pragma endregion Includes

#pragma region Declarations
class SVConditionalHistorySheet;
class SVInspectionProcess;
#pragma endregion Declarations

class SVConditionalHistoryBasePage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
public:
#pragma region Constructor
	SVConditionalHistoryBasePage( SVConditionalHistorySheet* pParentSheet, const CString& szCaption, int id );
	virtual ~SVConditionalHistoryBasePage();
#pragma endregion Constructor

	// ISVPropertyPageDialog
	virtual bool QueryAllowExit();

	// output of OnOK
	bool m_bObjectsChanged; // @WARNING:  bad practice making members public

// Dialog Data
	//{{AFX_DATA(SVConditionalHistoryBasePage)
	enum { IDD = IDD_CONDITIONAL_HISTORY_PAGE };
	SVOutputInfoListTreeCtrlClass m_Tree; // @WARNING:  bad practice making members public
	CEdit       m_ebMaxHistory; // @WARNING:  bad practice making members public
	CButton     m_btnClearAll; // @WARNING:  bad practice making members public
	CButton     m_btnClear; // @WARNING:  bad practice making members public
	CComboBox   m_cbInspection; // @WARNING:  bad practice making members public
	CListBox    m_lbSelected; // @WARNING:  bad practice making members public
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVConditionalHistoryBasePage)
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions
	//{{AFX_MSG(SVConditionalHistoryBasePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeListSelected_Base();
	afx_msg void OnBtnClear();
	afx_msg void OnBtnClearAll();
	afx_msg void OnCheckOverwrite();
	afx_msg void OnSelChangeComboInspection_Base();
	afx_msg void OnChangeEditMaxHistory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool CanSelectObjectCallback( SVObjectReference refObject, bool bCurrentState, int iIndex );

	virtual void OnSelChangeListSelected() {};
	virtual void OnSelChangeComboInspection() {};

	virtual void InitPage( SVInspectionProcess* pInspection ) = 0;

	SVConditionalHistorySheet* m_pSheet;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditionalHistoryBasePage.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:00:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Nov 2010 13:30:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Jan 2006 12:36:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added object list functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jan 2006 14:55:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed property updating
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