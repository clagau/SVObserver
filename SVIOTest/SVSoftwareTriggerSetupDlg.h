//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerSetupDlg
//* .File Name       : $Workfile:   SVSoftwareTriggerSetupDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 12:50:44  $
//******************************************************************************
#pragma once

// SVSoftwareTriggerSetupDlg dialog

class SVSoftwareTriggerSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(SVSoftwareTriggerSetupDlg)

public:
	SVSoftwareTriggerSetupDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVSoftwareTriggerSetupDlg();

// Dialog Data
	enum { IDD = IDD_SOFTWARETRIGGER_SETUP_DLG };

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	long m_period1;
	long m_period2;
	long m_period3;
	long m_period4;
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOTest\SVSoftwareTriggerSetupDlg.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:50:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Oct 2008 17:37:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed Frequency to Period for the Software Timer Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Sep 2008 08:14:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
