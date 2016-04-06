// ******************************************************************************
// * COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCameraPage
// * .File Name       : $Workfile:   SVCameraPage.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 08:25:58  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "svimagetest.h"
#include "SVCameraPage.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVCameraPropDlgClass.h"
#include "SVCameraFilePropertyPageDlg.h"
#include "SVTestAcquisitionClass.h"
#include "SVTestAcquisitionSubsystem.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVMessage/SVMessage.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString SVGigeCameraFileDefExt = ".ogc";
const CString SVGigeCameraFileFilter = "Digitizer Files (*.ogc)|*.ogc||";

const static int AcqImageBufferCnt = 10;

// SEJ - silly factory register for device param names...
//REGISTER_DEVICE_PARAM( DeviceParamCameraFormats,       DeviceParamCameraFormats_String,  SVCameraFormatsDeviceParam );
// SEJ - need to instantiate one of these first, so the factory is updated...
SVCameraFormatsDeviceParam gDummySVCameraFormatsDeviceParam;
SVLutDeviceParam gDummySVLutDeviceParam;
SVLightReferenceDeviceParam gDummySVLightReferenceDeviceParam;

/////////////////////////////////////////////////////////////////////////////
// SVCameraPage property page

IMPLEMENT_DYNCREATE(SVCameraPage, CPropertyPage)

void SVCameraPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVCameraPage)
	DDX_Control(pDX, IDC_CAMERA_IMAGE, m_CameraImage);
	DDX_Control(pDX, IDC_START_STOP_BUTTON, m_StartStopButton);
	DDX_Control(pDX, IDC_START_FRAME_COUNT, m_StartFrameCount);
	DDX_Control(pDX, IDC_PROPERITIES, m_ProperitiesButton);
	DDX_Control(pDX, IDC_END_FRAME_COUNT, m_EndFrameCount);
	DDX_Control(pDX, IDC_DETAILS, m_DetailsButton);
	DDX_Control(pDX, IDC_CAMERA_FILE_NAME, m_CameraFileName);
	DDX_Control(pDX, IDC_ADVANCED, m_AdvancedButton);
	DDX_Control(pDX, IDC_CAMERA_FILE_BROWSE, m_CameraFileBrowseButton);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_PERIOD, m_triggerPeriod);
	DDX_Check(pDX, IDC_USE_SOFTWARETRIGGER, m_bUseSoftwareTrigger);
}


BEGIN_MESSAGE_MAP(SVCameraPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVCameraPage)
	ON_BN_CLICKED(IDC_ADVANCED, OnAdvancedButtonClick)
	ON_BN_CLICKED(IDC_CAMERA_FILE_BROWSE, OnCameraFileBrowseButtonClick)
	ON_BN_CLICKED(IDC_DETAILS, OnDetailsButtonClick)
	ON_BN_CLICKED(IDC_PROPERITIES, OnProperitiesButtonClick)
	ON_BN_CLICKED(IDC_START_STOP_BUTTON, OnStartStopButtonClick)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

HRESULT SVCameraPage::GetNextBuffer( SVImageBufferInterface& p_rBuffer )
{
	m_lStartCount++;
	m_lEndCount++;

	return m_CameraImage.GetNextBuffer( p_rBuffer );
}

HRESULT SVCameraPage::UpdateWithCompletedBuffer( const SVImageBufferInterface& p_rBuffer )
{
	CString l_csCount;

	l_csCount.Format( "%d", m_lStartCount );
	m_StartFrameCount.SetWindowText( l_csCount );

	l_csCount.Format( "%d", m_lEndCount );
	m_EndFrameCount.SetWindowText( l_csCount );

	m_CameraImage.Invalidate();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// SVCameraPage message handlers

void SVCameraPage::OnAdvancedButtonClick() 
{
	// Show camera file editor dialog
	SVCameraFilePropertyPageDlg dlg;	

	dlg.m_pAcquisition = m_pAcquisition;

	if ( dlg.DoModal() == IDOK )
	{
		if (m_pAcquisition)
		{
			// refresh parameters ?
			SVDeviceParamCollection deviceParams;
			m_pAcquisition->GetDeviceParameters(deviceParams);

			HRESULT hr = m_pAcquisition->SetDeviceParameters(deviceParams);
			if (hr != S_OK)
			{
				if (hr == SVMSG_SVO_20_INCORRECT_CAMERA_FILE)
				{
					AfxMessageBox("Error - Camera File does not Match Camera", MB_OK);
				}
				else
				{
					// be silent?
					//AfxMessageBox("Error Loading Camera File", MB_OK);
				}
			}
		}
	}
}

void SVCameraPage::OnCameraFileBrowseButtonClick()
{
	CString cameraFileFilter;
	CString cameraFileDefaultExt;

	CSVImageTestApp* pApp = (CSVImageTestApp *)AfxGetApp();
	// check for Gige...
	if (pApp->IsGigeSystem())
	{
		cameraFileFilter = SVGigeCameraFileFilter;
		cameraFileDefaultExt = SVGigeCameraFileDefExt;
	}

	CFileDialog dlg( TRUE, 
						cameraFileDefaultExt,
						m_csFileName, 
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOCHANGEDIR,
						cameraFileFilter,
						NULL );

	dlg.m_ofn.lpstrTitle = "Select Camera File";
	dlg.m_ofn.lpstrInitialDir = "C:\\CAM";

	if ( dlg.DoModal() == IDOK )
	{
		m_csFileName = dlg.GetPathName();

		LoadSVCameraFiles();
		m_CameraFileName.SetWindowText( m_csFileName );

		// Reset Camera Image
		CreateCameraImage();
	}
}

void SVCameraPage::OnDetailsButtonClick() 
{
}

void SVCameraPage::OnProperitiesButtonClick() 
{
	SVCameraPropDlgClass l_svDialog;

	if (m_pAcquisition)
	{
		unsigned long l_ulHandle = 0;

		m_pAcquisition->m_rSubsystem.m_svDigitizers.GetHandle( &l_ulHandle, m_lSelectedCamera );

		l_svDialog.m_psvDigitizers = &m_pAcquisition->m_rSubsystem.m_svDigitizers;
		l_svDialog.m_ulHandle = l_ulHandle;

		l_svDialog.DoModal();

		// Reset Camera Image
		CreateCameraImage();
	}
}

void SVCameraPage::OnStartStopButtonClick() 
{
	UpdateData();

	if( ! m_bStarted )
	{
		StartAcquire();
	}
	else
	{
		StopAcquire();
	}
}

BOOL SVCameraPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_StartFrameCount.SetWindowText( "0" );
	m_EndFrameCount.SetWindowText( "0" );
	m_CameraFileName.SetWindowText( "" );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

SVCameraPage::SVCameraPage()
: CPropertyPage(SVCameraPage::IDD),
	m_pAcquisition( NULL ),
	m_csFileName(),
	m_lSelectedCamera( -1L ),
	m_lStartCount( 0L ),
	m_lEndCount( 0L ),
	m_bStarted( false ),
	m_timerID(0),
	m_bUseSoftwareTrigger(false),
	m_triggerPeriod(200)
{
	//{{AFX_DATA_INIT(SVCameraPage)
	//}}AFX_DATA_INIT

	for (int i = 0; i < AcqImageBufferCnt; i++ )
	{
		m_svInfoArray[i].m_lIndex = i;
		m_svInfoArray[i].m_psvDlg = this;
	}
}

SVCameraPage::~SVCameraPage()
{
}

void SVCameraPage::SetTabText( LPCTSTR p_szText )
{
	m_strCaption = p_szText;
	m_psp.dwFlags |= PSP_USETITLE;
	m_psp.pszTitle = m_strCaption;
}

void SVCameraPage::SelectCamera( long p_SelectedCamera )
{
	m_lSelectedCamera = p_SelectedCamera;

	if( 0 <= m_lSelectedCamera && m_pAcquisition != NULL )
	{
		CreateCameraImage();
	}
}

void SVCameraPage::CreateCameraImage()
{
	if( 0 <= m_lSelectedCamera && m_pAcquisition != NULL )
	{
		unsigned long l_ulHandle( 0L );

		m_pAcquisition->m_rSubsystem.m_svDigitizers.GetHandle( &l_ulHandle, m_lSelectedCamera );

		unsigned long bufWidth = 640;
		unsigned long bufHeight = 480;
		int iFormat = SVImageFormatUnknown;

		m_pAcquisition->m_rSubsystem.m_svDigitizers.GetBufferHeight( l_ulHandle, &bufHeight );
		m_pAcquisition->m_rSubsystem.m_svDigitizers.GetBufferWidth( l_ulHandle, &bufWidth );
		m_pAcquisition->m_rSubsystem.m_svDigitizers.GetBufferFormat( l_ulHandle, &iFormat );

		m_CameraImage.UpdateDisplayBufferInfo( bufWidth, bufHeight, iFormat );
		m_CameraImage.Invalidate();
	}
}

void SVCameraPage::ResetCount()
{
	m_lStartCount = 0;
	m_lEndCount = 0;

	m_StartFrameCount.SetWindowText( "0" );
	m_EndFrameCount.SetWindowText( "0" );
}

void SVCameraPage::ResetCameraFilename()
{
	m_csFileName.Empty();
	m_CameraFileName.SetWindowText(m_csFileName);
}

void SVCameraPage::LoadSVCameraFiles()
{
	if (m_pAcquisition)
	{
		HRESULT hr = m_pAcquisition->ReadCameraFile(m_csFileName);

		if (hr == S_OK)
		{
			// do camera file test/validation?

			SVDeviceParamCollection cameraFileParams;
			m_pAcquisition->GetCameraFileParameters(cameraFileParams);

			if (m_pAcquisition->IsValidCameraFileParameters(cameraFileParams) != S_OK)
			{
				AfxMessageBox("Error - Incorrect Camera File", MB_OK);
			}
		}
		else
		{
			if (hr == SVMSG_SVO_20_INCORRECT_CAMERA_FILE)
			{
				AfxMessageBox("Error - Camera File does not Match Camera", MB_OK);
			}
			else
			{
				AfxMessageBox("Error Loading Camera File", MB_OK);
			}
		}
	}
	else
	{
		AfxMessageBox("No Acquisition Device", MB_OK);
	}
}

void SVCameraPage::SetGigePacketSizeDeviceParam(SVDeviceParamCollection* pDeviceParams)
{
	CSVImageTestApp* pApp = (CSVImageTestApp *)AfxGetApp();

	// check if Packet Size Device Param exists
	SVDeviceParam* l_pGigePacketSize = pDeviceParams->GetParameter( DeviceParamGigePacketSize );
	// if found - update it
	// else - add it
	if ( l_pGigePacketSize != NULL )
	{
		l_pGigePacketSize->SetValue(_variant_t(pApp->m_iniLoader.m_gigePacketSize));
	}
	else // add it
	{
		pDeviceParams->SetParameter( DeviceParamGigePacketSize, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( DeviceParamGigePacketSize )) );
		SVLongValueDeviceParam* pParam = pDeviceParams->GetParameter( DeviceParamGigePacketSize ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->lValue = pApp->m_iniLoader.m_gigePacketSize;
		pParam->SetName(DeviceParamGigePacketSize_String);
	}
}

void SVCameraPage::StartAcquire()
{
	if (m_bUseSoftwareTrigger)
	{
		unsigned long l_ulHandle = 0;
		m_pAcquisition->m_rSubsystem.m_svDigitizers.GetHandle( &l_ulHandle, m_lSelectedCamera );

		m_pAcquisition->m_rSubsystem.m_svDigitizers.InternalTriggerEnable(l_ulHandle);

		if (m_timerID == 0)
		{
			m_timerID = SetTimer(1, m_triggerPeriod, NULL);
		}
	}
	SVCallbackStruct l_svCallback;

	l_svCallback.m_pOwner = this;

	ResetCount();

	m_pAcquisition->StartAcquire(*this);

	m_bStarted = true;

	::SetWindowText(::GetDlgItem(m_hWnd, IDC_START_STOP_BUTTON), "Stop");
}

void SVCameraPage::StopAcquire()
{
	m_pAcquisition->StopAcquire();

	m_bStarted = false;

	::SetWindowText(::GetDlgItem(m_hWnd, IDC_START_STOP_BUTTON), "Start");

	if (m_timerID != 0)
	{
		::KillTimer(m_hWnd, m_timerID);
		m_timerID = 0;
	}
}

void SVCameraPage::EnableViewCameraFileButton()
{
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_VIEW_CAMERA_FILE), true);
}

void SVCameraPage::OnTimer(UINT_PTR nIDEvent)
{
	if( m_pAcquisition != NULL && m_bStarted )
	{
		m_pAcquisition->FireSoftwareTrigger();
	}
}

void SVCameraPage::OnDestroy()
{
	if (m_timerID != 0)
	{
		::KillTimer(m_hWnd, m_timerID);
		m_timerID = 0;
	}
	CDialog::OnDestroy();
}


