//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDriveInitDlg
//* .File Name       : $Workfile:   SVDriveInitDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Sep 2014 15:03:26  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVProductId.h"
#pragma endregion Includes

class SVDriveInitDlg : public CDialog
{
public:
	SVDriveInitDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
	//{{AFX_DATA(SVDriveInitDlg)
	enum { IDD = IDD_SVDRIVEINIT_DIALOG };
	CButton	m_cb1394Hub;
	CButton m_SingleCamera;
	CEdit	m_model_number;
	CComboBox	m_type;
	CEdit	m_cdkey5;
	CEdit	m_cdkey4;
	CEdit	m_cdkey3;
	CEdit	m_cdkey2;
	CEdit	m_cdkey1;
	CEdit	m_date;
	CEdit	m_serviced_by;
	CEdit	m_serial_number;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVDriveInitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(SVDriveInitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeCdkey1();
	afx_msg void OnChangeCdkey2();
	afx_msg void OnChangeCdkey3();
	afx_msg void OnChangeCdkey4();
	afx_msg void OnChangeCdkey5();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void OnChangeCdkey(CEdit& rEdit);
	bool IsValidModelNumber() const;

	bool GetProductId() const;
	
	bool GetOEMInfo();
	bool GetSysPrepInfo();

	bool UpdateRegistryInfo();
	bool UpdateOEMInfo();
	bool UpdateSVIMInfo();
	bool UpdateSysPrepInfo();

	HRESULT BackupBootIni();

	HRESULT UpdateIOInterfaceDLL(LPCTSTR p_szIOBoard);

	HRESULT UpdateMatrox(LPCTSTR p_szDigitizer);
	HRESULT UpdateMatroxRegistryMaxMem(size_t& p_rMaxSize, size_t p_MILSize);
	HRESULT AddMatroxBootIniMaxMem(size_t p_MILSize);
	HRESULT RemoveMatroxBootIniMaxMem();
	HRESULT AddCommandBeforeSVObserver(LPCTSTR p_strNewCommand, bool p_bRemove);

	SVProductId m_CDKey;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVDriveInit\SVDriveInitDlg.h_v  $
 * 
 *    Rev 1.2   17 Sep 2014 15:03:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  943
 * SCR Title:  Parallel Port I/O for 64 Bit and Windows 7
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support for SVLptIO.
 * Renamed UpdateTVicLPT to UpdateIOInterfaceDLL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 14:06:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   12 Apr 2013 14:28:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated issues  with PVCS log entries.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 *    Rev 1.0   22 Apr 2013 08:33:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   11 Dec 2012 18:17:02   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (SVDriveInitDlg class) Added m_SingleCamera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   10 Sep 2012 09:47:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  788
 * SCR Title:  Modify SVDriveInit so it adds net start to GigE images
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a function to add/remove a command before SVObserver.exe.  Use the new function to add a net start command before SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9.1.1   11 Dec 2012 17:19:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (SVDriveInitDlg class) Added m_SingleCamera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9.1.0   08 Nov 2012 15:08:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  788
 * SCR Title:  Modify SVDriveInit so it adds net start to GigE images
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a function to add/remove a command before SVObserver.exe.  Use the new function to add a net start command before SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   28 Jun 2012 15:00:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removing Shared Memory for the webserver from the SVObserver application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   19 Dec 2011 14:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Drive Init program to enable and configure the shared memory functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Nov 2009 18:46:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated drive init program to fix issues associated with new functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   12 Feb 2009 14:09:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated to use fbreseal instead of sysprep
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Mar 2007 07:35:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  593
 * SCR Title:  Remove HUB product types from SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed checkbox for HUB / Shared bandwidth.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Jan 2006 07:04:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  538
 * SCR Title:  Remove instances when displaying RAID error message on event log read errors
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add start-up support for 1394 Shared Bandwidth/Hub configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Jan 2004 11:34:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated dialog to not run sysprep program unless the OEM number change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2003 14:26:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new SVIM Color flag to the dialog to remove the need for adjusting color flag with registry files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Aug 2003 10:58:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
