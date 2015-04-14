//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgRemoteInputToolPage
//* .File Name       : $Workfile:   SVTADlgRemoteInputToolPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:17:26  $
//******************************************************************************

#ifndef SVTADLGREMOTEINPUTTOOLPAGE_H
#define SVTADLGREMOTEINPUTTOOLPAGE_H

#include "SVValueObjectImpl.h"
#include "afxcmn.h"
#include "afxwin.h"

class SVRemoteInputTool;
class SVToolAdjustmentDialogSheetClass;

class SVTADlgRemoteInputToolPage : public CPropertyPage
{
	// Generated message map functions
	public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedClearInputButton();
	afx_msg void OnBnClickedSelectInputButton();
	DECLARE_MESSAGE_MAP()

	// ClassWizard generated virtual function overrides
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	enum { IDD = IDD_TA_REMOTE_INPUT_PAGE };

	SVTADlgRemoteInputToolPage( SVToolAdjustmentDialogSheetClass* Parent, int id = IDD );

protected:
	void RefreshSelectedInputName();

	CStatic m_RemoteInputNameCtrl;
	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	SVRemoteInputTool* m_pTool;
	CString m_InputName;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgRemoteInputToolPage.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:17:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jan 2012 10:03:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated source code based on gui changes desired by application users.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2012 15:42:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added tool adjustment functionality for the new remote input tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
