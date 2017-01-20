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

#pragma once

#pragma region Includes
#include "DisplayPicture.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVCameraPage;

struct SVCameraPageBufferInfoStruct
{
	SVCameraPage *m_psvDlg;
	long m_lIndex;
	unsigned long m_ulBufferIndex;

	SVCameraPageBufferInfoStruct()
	{
		m_psvDlg = nullptr;
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
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

	protected:
	// Generated message map functions
	//{{AFX_MSG(SVCameraPage)
	afx_msg void OnAdvancedButtonClick();
	afx_msg void OnCameraFileBrowseButtonClick();
	afx_msg void OnDetailsButtonClick();
	afx_msg void OnProperitiesButtonClick();
	afx_msg void OnStartStopButtonClick();
	virtual BOOL OnInitDialog() override;
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
	SVString m_FileName;
	long m_lSelectedCamera;
	long m_lStartCount;
	long m_lEndCount;
	bool m_bStarted;

	SVCameraPageBufferInfoStruct m_svInfoArray[10];

private:
	void ResetCount();

	void LoadSVCameraFiles();

	void CreateCameraImage();
	void SetGigePacketSizeDeviceParam(SVDeviceParamCollection* pDeviceParams);

	void EnableViewCameraFileButton();

	UINT_PTR m_timerID;
};
