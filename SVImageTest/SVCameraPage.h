// ******************************************************************************
// * COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCameraPage
// * .File Name       : $Workfile:   SVCameraPage.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 08:25:58  $
// ******************************************************************************

#ifndef SVCAMERAPAGE_H
#define SVCAMERAPAGE_H

#include "DisplayPicture.h"

class SVCameraPage;

struct SVCameraPageBufferInfoStruct
{
	SVCameraPage *m_psvDlg;
	long m_lIndex;
	unsigned long m_ulBufferIndex;

	SVCameraPageBufferInfoStruct()
	{
		m_psvDlg = NULL;
		m_lIndex = -1;
		m_ulBufferIndex = 0;
	};
};

/////////////////////////////////////////////////////////////////////////////
// SVCameraPage dialog

class SVCameraPage : public CPropertyPage
{
	DECLARE_DYNCREATE(SVCameraPage)

	public:
	HRESULT GetNextBuffer( SVImageBufferInterface& p_rBuffer );
	HRESULT UpdateWithCompletedBuffer( const SVImageBufferInterface& p_rBuffer );

	//{{AFX_DATA(SVCameraPage)
	enum { IDD = IDD_CAMERA_PAGE };
	CDisplayPicture	m_CameraImage;
	CButton	m_StartStopButton;
	CStatic	m_StartFrameCount;
	CButton	m_ProperitiesButton;
	CStatic	m_EndFrameCount;
	CButton	m_DetailsButton;
	CStatic	m_CameraFileName;
	CButton	m_AdvancedButton;
	CButton	m_CameraFileBrowseButton;
	//}}AFX_DATA

	long m_triggerPeriod;
	BOOL m_bUseSoftwareTrigger;

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVCameraPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	protected:
	// Generated message map functions
	//{{AFX_MSG(SVCameraPage)
	afx_msg void OnAdvancedButtonClick();
	afx_msg void OnCameraFileBrowseButtonClick();
	afx_msg void OnDetailsButtonClick();
	afx_msg void OnProperitiesButtonClick();
	afx_msg void OnStartStopButtonClick();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVCameraPage();
	virtual ~SVCameraPage();

	void SetTabText( LPCTSTR p_szText );
	void SelectCamera( long p_SelectedCamera );
	void StartAcquire();
	void StopAcquire();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

	SVTestAcquisitionClass* m_pAcquisition;
	CString m_csFileName;
	long m_lSelectedCamera;
	long m_lStartCount;
	long m_lEndCount;
	bool m_bStarted;

	SVCameraPageBufferInfoStruct m_svInfoArray[10];

private:
	void ResetCount();
	void ResetCameraFilename();

	void LoadCameraFiles(bool bAnalogDigitizer);
	void LoadSVCameraFiles();
	void LoadCorecoCameraFiles();

	void CreateCameraImage();
	void SetGigePacketSizeDeviceParam(SVDeviceParamCollection* pDeviceParams);

	void EnableViewCameraFileButton(bool bAnalogDigitizer);

	UINT_PTR m_timerID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVCAMERAPAGE_H__0F2C8D2E_CE74_4048_A054_C2B833166EC5__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVCameraPage.h_v  $
 * 
 *    Rev 1.1   01 Oct 2013 08:25:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add X64 Platform
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 11:08:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Jul 2009 15:53:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   17 Jun 2009 08:29:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support Gige System
 * Revised to support loading of Gige camera files
 * Revised to check for valid host buffer address
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   03 Oct 2008 17:38:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed Frequency to Period for the Software Timer Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Sep 2008 14:06:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected m_bSoftwareTrigger variable for use with DDX
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Sep 2008 14:00:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised for Software Trigger support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Mar 2008 09:35:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  622
 * SCR Title:  Update SVImageTest Application to Acquire on Multiple Cameras Simultaneously
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added PVCS Headers to files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
