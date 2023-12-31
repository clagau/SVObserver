// ******************************************************************************
// * COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved, Harrisburg
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
#include "SVImageTestApp.h"
#include "SVCameraPage.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "Definitions/SVGigeEnums.h"
#include "Definitions/TriggerType.h"
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

constexpr const char* cGigeCameraFileDefExt = _T(".ogc");
constexpr const char* cGigeCameraFileFilter = _T("Digitizer Files (*.ogc)|*.ogc||");

const static int AcqImageBufferCnt = 10;

// Need to instantiate one of these first, so the factory is updated...
SVCameraFormatsDeviceParam gDummySVCameraFormatsDeviceParam;
SVLutDeviceParam gDummySVLutDeviceParam;
SVLightReferenceDeviceParam gDummySVLightReferenceDeviceParam;

SVCreatePtr SVCameraPage::m_pCreate {nullptr};
SVDestroyPtr SVCameraPage::m_pDestroy {nullptr};
SVTriggerStartPtr SVCameraPage::m_pStart {nullptr};
SVTriggerStopPtr SVCameraPage::m_pStop {nullptr};
bool SVCameraPage::m_plcStarted {false};


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

HRESULT SVCameraPage::UpdateWithCompletedBuffer( const SvOi::ITRCImagePtr& )
{
	std::string Count;

	Count = std::format( _T("{}"), ++m_lStartCount );
	m_StartFrameCount.SetWindowText( Count.c_str() );

	Count = std::format( _T("{}"), ++m_lEndCount );
	m_EndFrameCount.SetWindowText( Count.c_str() );

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
			m_pAcquisition->GetDeviceParameters(m_cameraFileParams);
		}
	}
}

void SVCameraPage::OnCameraFileBrowseButtonClick()
{
	std::string cameraFileFilter;
	std::string cameraFileDefaultExt;

	SVImageTestApp* pApp = (SVImageTestApp *)AfxGetApp();
	// check for Gige...
	if( nullptr != pApp && pApp->IsGigeSystem())
	{
		cameraFileFilter = cGigeCameraFileFilter;
		cameraFileDefaultExt = cGigeCameraFileDefExt;
	}

	//Any leading dot will be removed from the default extension because otherwise two consecutive dots after the filename may occur in certain cases
	size_t positionOfDot = cameraFileDefaultExt.find(".");
	if (0 == positionOfDot)
	{
		cameraFileDefaultExt = cameraFileDefaultExt.substr(1);
	}

	CFileDialog dlg( true, 
						cameraFileDefaultExt.c_str(),
						m_FileName.c_str(), 
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOCHANGEDIR,
						cameraFileFilter.c_str(),
						nullptr );

	dlg.m_ofn.lpstrTitle = _T("Select Camera File");
	dlg.m_ofn.lpstrInitialDir = _T("C:\\CAM");

	if ( dlg.DoModal() == IDOK )
	{
		m_FileName = dlg.GetPathName();

		LoadSVCameraFiles();
		m_CameraFileName.SetWindowText( m_FileName.c_str() );

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
		unsigned long digitizerHandle = m_pAcquisition->m_rSubsystem.m_svDigitizers.GetHandle(m_lSelectedCamera);

		l_svDialog.m_psvDigitizers = &m_pAcquisition->m_rSubsystem.m_svDigitizers;
		l_svDialog.m_digitizerHandle = digitizerHandle;

		l_svDialog.DoModal();

		// Reset Camera Image
		CreateCameraImage();
	}
}

void SVCameraPage::OnStartStopButtonClick() 
{
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
	m_pAcquisition( nullptr ),
	m_FileName(),
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

	if( 0 <= m_lSelectedCamera && nullptr != m_pAcquisition )
	{
		CreateCameraImage();
	}
}

void SVCameraPage::CreateCameraImage()
{
	if( 0 <= m_lSelectedCamera && nullptr != m_pAcquisition )
	{
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

void SVCameraPage::LoadSVCameraFiles()
{
	if (m_pAcquisition)
	{
		HRESULT hr = m_pAcquisition->ReadCameraFile(std::string(m_FileName));

		if (S_OK == hr)
		{
			// do camera file test/validation?

			m_pAcquisition->GetCameraFileParameters(m_cameraFileParams);

			if (S_OK != m_pAcquisition->IsValidCameraFileParameters(m_cameraFileParams))
			{
				AfxMessageBox("Error - Incorrect Camera File", MB_OK);
			}
		}
		else
		{
			if (SVMSG_SVO_20_INCORRECT_CAMERA_FILE == hr)
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
	SVImageTestApp* pApp = (SVImageTestApp *)AfxGetApp();

	// check if Packet Size Device Param exists
	SVDeviceParam* l_pGigePacketSize = pDeviceParams->GetParameter( DeviceParamGigePacketSize );
	// if found - update it
	// else - add it
	if ( nullptr != l_pGigePacketSize )
	{
		l_pGigePacketSize->SetValue(_variant_t(pApp->m_iniLoader.GetInitialInfo().m_gigePacketSize));
	}
	else // add it
	{
		pDeviceParams->SetParameter( DeviceParamGigePacketSize, std::unique_ptr<SVDeviceParam>(SVDeviceParam::Create(DeviceParamGigePacketSize)).get());
		SVLongValueDeviceParam* pParam = pDeviceParams->GetParameter( DeviceParamGigePacketSize ).DerivedValue(pParam);
		Log_Assert( pParam );
		pParam->lValue = pApp->m_iniLoader.GetInitialInfo().m_gigePacketSize;
		pParam->SetName(DeviceParamGigePacketSize_String);
	}
}

void SVCameraPage::StartAcquire()
{
	UpdateData(true);
	HRESULT hr = m_pAcquisition->SetDeviceParameters(m_cameraFileParams);
	if (S_OK != hr)
	{
		if (SVMSG_SVO_20_INCORRECT_CAMERA_FILE == hr)
		{
			AfxMessageBox("Error - Camera File does not Match Camera", MB_OK);
			return;
		}
	}
	if (m_bUseSoftwareTrigger)
	{
		unsigned long digitizerHandle = m_pAcquisition->m_rSubsystem.m_svDigitizers.GetHandle(m_lSelectedCamera);
		_variant_t value {static_cast<long> (SvDef::TriggerType::SoftwareTrigger)};
		m_pAcquisition->m_rSubsystem.m_svDigitizers.ParameterSetValue(digitizerHandle, SvDef::SVTriggerType, value);
		if (m_timerID == 0)
		{
			m_timerID = SetTimer(1, m_triggerPeriod, nullptr);
		}
	}
	else
	{
		if (nullptr != m_pCreate && nullptr != m_pStart && false == m_plcStarted)
		{
			m_pCreate();
			m_pStart(1);
			m_plcStarted = true;
		}
	}

	ResetCount();

	m_pAcquisition->StartAcquire(*this);

	m_bStarted = true;

	::SetWindowText(::GetDlgItem(m_hWnd, IDC_START_STOP_BUTTON), "Stop");
}

void SVCameraPage::StopAcquire()
{
	UpdateData(true);
	m_pAcquisition->StopAcquire();

	m_bStarted = false;

	::SetWindowText(::GetDlgItem(m_hWnd, IDC_START_STOP_BUTTON), "Start");

	if (m_timerID != 0)
	{
		::KillTimer(m_hWnd, m_timerID);
		m_timerID = 0;
	}
	if (nullptr != m_pDestroy && nullptr != m_pStop && true == m_plcStarted)
	{
		m_pStop(1);
		m_pDestroy();
		m_plcStarted = false;
	}
}

void SVCameraPage::EnableViewCameraFileButton()
{
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_VIEW_CAMERA_FILE), true);
}

void SVCameraPage::OnTimer(UINT_PTR)
{
	if( nullptr != m_pAcquisition && m_bStarted )
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
	CPropertyPage::OnDestroy();
}


