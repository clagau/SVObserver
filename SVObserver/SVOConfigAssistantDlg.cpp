//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOConfigAssistantDlg
//* .File Name       : $Workfile:   SVOConfigAssistantDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.13  $
//* .Check In Date   : $Date:   07 Mar 2014 18:22:14  $
//******************************************************************************

#include "stdafx.h"
#include <iterator>
#include <boost/assign.hpp>
#include <sys/stat.h>
#include <io.h>
#include "SVOConfigAssistantDlg.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVFileAcquisitionLoadingModeEnum.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObserver.h"
#include "SVOPPQObj.h"
#include "SVOInspectionObj.h"
#include "SVOTriggerObj.h"
#include "SVOCameraObj.h"
#include "SVInputObjectList.h"
#include "SVOutputObjectList.h"
#include "SVImageProcessingClass.h"
#include "SVAcquisitionClass.h"
#include "SVIPDoc.h"
#include "SVTriggerClass.h"
#include "SVMessage/SVMessage.h"
#include "SVTriggerProcessingClass.h"
#include "SVInspectionImporter.h"
#include "SVImportProgress.h"
#include "SVHardwareManifest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	int MAX_NUM_DIGITIZERS_1394_HUB=3;

	enum SystemTypeEnum
	{
		NEW_SYSTEM_ENUM,
		SYSTEM_SVIM_FULL_ENUM,
		SYSTEM_SVIM_05_ENUM,
		SYSTEM_SVIM_RGB_ENUM,
		SYSTEM_SVIM_COLOR_ENUM,
		SYSTEM_SVIM_D1_ENUM,
		SYSTEM_SVIM_D2_ENUM,
		SYSTEM_SVIM_D3_ENUM,
		SYSTEM_SVIM_X1_ENUM,
		SYSTEM_SVIM_X2_ENUM,
		SYSTEM_SVIM_X3_ENUM,
		SYSTEM_SVIM_D1_COLOR_ENUM,
		SYSTEM_SVIM_D2_COLOR_ENUM,
		SYSTEM_SVIM_D3_COLOR_ENUM,
		SYSTEM_SVIM_X1_COLOR_ENUM,
		SYSTEM_SVIM_X2_COLOR_ENUM,
		SYSTEM_SVIM_X3_COLOR_ENUM,
		SYSTEM_SVIM_D1_HUB_ENUM,
		SYSTEM_SVIM_D2_HUB_ENUM,
		SYSTEM_SVIM_D3_HUB_ENUM,
		SYSTEM_SVIM_X1_HUB_ENUM,
		SYSTEM_SVIM_X2_GD1A_ENUM,
		SYSTEM_SVIM_X2_GD1A_COLOR_ENUM,
		SYSTEM_SVIM_X2_GD2A_ENUM,
		SYSTEM_SVIM_X2_GD2A_COLOR_ENUM,
		SYSTEM_SVIM_X2_GD4A_ENUM,
		SYSTEM_SVIM_X2_GD4A_COLOR_ENUM,
		SYSTEM_SVIM_X2_GD8A_ENUM,
		SYSTEM_SVIM_X2_GD8A_COLOR_ENUM,
		SYSTEM_SVIM_X2_GD8A_NONIO_ENUM,
		SYSTEM_SVIM_X2_GD8A_NONIO_COLOR_ENUM,
		LAST_SYSTEM_ENUM // add new system enums prior to this entry
	};
}

// Defines for Camera File extensions
static const CString CorecoCameraFileDefExt = ".cca";
static const CString CorecoVicCameraFileDefExt = ".cvi";
static const CString SV1394CameraFileDefExt = ".odc";
static const CString SVGigeCameraFileDefExt = ".ogc";

static const CString CAMERA_FIXED_NAME   ( _T("Camera_") );
static const CString INSPECT_FIXED_NAME  ( _T("Inspection_") );
static const CString PPQ_FIXED_NAME      ( _T("PPQ_") );

static const CString SYSTEM_SVIM_FULL    ( _T("SVIM") );
static const CString SYSTEM_SVIM_05      ( _T("SVIM .5") );
static const CString SYSTEM_SVIM_RGB     ( _T("SVIM RGB (mono)") );
static const CString SYSTEM_SVIM_COLOR   ( _T("SVIM COLOR") );
static const CString SYSTEM_SVIM_1394    ( _T("SVIM 1394") );
static const CString SYSTEM_SVIM_UNKNOWN ( _T("Unknown SVIM") );

//const strings for 1394 product types
static const CString SYSTEM_SVIM_D1		  ( _T("SVIM D1") );
static const CString SYSTEM_SVIM_X1		  ( _T("SVIM X1") );
static const CString SYSTEM_SVIM_X2		  ( _T("SVIM X2") );
static const CString SYSTEM_SVIM_X3		  ( _T("SVIM X3") );
static const CString SYSTEM_SVIM_D2		  ( _T("SVIM D2") );
static const CString SYSTEM_SVIM_D3		  ( _T("SVIM D3") );
static const CString SYSTEM_SVIM_D1_COLOR	  ( _T("SVIM D1 Color") );
static const CString SYSTEM_SVIM_X1_COLOR	  ( _T("SVIM X1 Color") );
static const CString SYSTEM_SVIM_X2_COLOR	  ( _T("SVIM X2 Color") );
static const CString SYSTEM_SVIM_X3_COLOR	  ( _T("SVIM X3 Color") );
static const CString SYSTEM_SVIM_D2_COLOR	  ( _T("SVIM D2 Color") );
static const CString SYSTEM_SVIM_D3_COLOR	  ( _T("SVIM D3 Color") );

static const CString SYSTEM_SVIM_D1_HUB_STRING	( _T("SVIM D1 HUB") );
static const CString SYSTEM_SVIM_X1_HUB_STRING	( _T("SVIM X1 Shared Bandwidth") );
static const CString SYSTEM_SVIM_D2_HUB_STRING	( _T("SVIM D2 HUB") );
static const CString SYSTEM_SVIM_D3_HUB_STRING	( _T("SVIM D3 HUB") );

static const CString SYSTEM_SVIM_X2_GD1A		( _T("SVIM X2-GD1A") );
static const CString SYSTEM_SVIM_X2_GD1A_COLOR	( _T("SVIM X2-GD1A Color") );
static const CString SYSTEM_SVIM_X2_GD2A		( _T("SVIM X2-GD2A") );
static const CString SYSTEM_SVIM_X2_GD2A_COLOR	( _T("SVIM X2-GD2A Color") );
static const CString SYSTEM_SVIM_X2_GD4A		( _T("SVIM X2-GD4A") );
static const CString SYSTEM_SVIM_X2_GD4A_COLOR	( _T("SVIM X2-GD4A Color") );
static const CString SYSTEM_SVIM_X2_GD8A		( _T("SVIM X2-GD8A") );
static const CString SYSTEM_SVIM_X2_GD8A_COLOR	( _T("SVIM X2-GD8A Color") );

static const CString SYSTEM_SVIM_X2_GD8A_NONIO		( _T("SVIM Non I/O") );
static const CString SYSTEM_SVIM_X2_GD8A_NONIO_COLOR	( _T("SVIM Non I/O Color") );

static const CString SVIM_BOARD_QUAD_STRING     ( _T("Viper_Quad_1") );
static const CString SVIM_BOARD_RGB_STRING      ( _T("Viper_RGB_1") );
static const CString SVIM_BOARD_1394_STRING     ( _T("Matrox_1394") );
static const CString SVIM_BOARD_FILEACQUISITION_STRING     ( _T("File") );
static const CString SVIM_BOARD_MATROX_GIGE	( _T("Matrox_GIGE") );

static const CString SVIM_DIG_NAME_STRING       ( _T(".Dig_") );
static const CString SVIM_CHN_NAME_STRING       ( _T(".Ch_") );
static const CString SVIM_CHN_ALL_STRING        ( _T(".Ch_All") );

static const CString ERR_STR                 ( _T( "**ERROR**   " ) );
static const CString WARNING_STR             ( _T( "**WARNING** " ) );
static const CString CAMERA_INVALID_FILES    ( _T( "-Camera files are invalid" ) );
static const CString CAMERA_NONE_DEFINED     ( _T( "-No cameras defined" ) );
static const CString TRIGGER_NONE_DEFINED    ( _T( "-No trigger defined" ) );
static const CString INSPECT_NONE_DEFINED    ( _T( "-No inspection defined" ) );
static const CString PPQ_NONE_DEFINED        ( _T( "-No PPQ defined" ) );
static const CString PPQ_NO_TRIGGER          ( _T( "-No Trigger attached to PPQ" ) );
static const CString PPQ_NO_CAMERA           ( _T( "-No Camera attached to PPQ" ) );
static const CString PPQ_NO_INSPECT          ( _T( "-No Inspection attached to PPQ" ) );
static const CString CONFIG_NAME_ERR         ( _T( " No Configuration name" ) );
static const CString INVALID_CAMERA          ( _T( "-Camera is invalid for current product" ) );
static const CString INVALID_TRIGGER         ( _T( "-Trigger is invalid for current product" ) );
static const CString INSPECTION_ERROR        ( _T( "-Inspection is not attached to a PPQ" ) );
static const CString TOOLSET_IMAGE_ERROR     ( _T( "-Inspection has no Toolset Image" ) );
static const CString PPQ_PROP_SRC_IMG_ERROR  ( _T( "- Property -'Maintain souce image' set to TRUE, PPQ length > ") );
static const CString PPQ_CONDITIONAL_OUTPUT_INVALID ( _T( "-Conditional Output for PPQ is not valid" ) );
static const CString MESSAGE_UNSUPPORTED_CAM_FILE ( _T("The camera file you have selected is not an SVResearch supported file.") );
static const CString MESSAGE_INCORRECT_CAM_FILE   ( _T("The selected camera file does not match the physical camera.") );
static const CString MESSAGE_NOT_COLOR_CAM_FILE   ( _T("The selected camera file is not a color camera file.") );
static const CString MESSAGE_NOT_COLOR_CAM        ( _T("The physical camera is not a color camera.") );
static const CString MESSAGE_ONE_INVERT_CONTROL   ( _T("There is only one Invert control on a SVIM X-Series for all triggers and strobes.") );
static const CString MESSAGE_SOFTWARE_TRIGGER_NOT_ALLOWED ( _T("The camera does not support Software Triggering.") );
static const CString MESSAGE_FILE_ACQUISITION_NOT_ALLOWED ( _T("File Acquisition is not allowed.") );
static const CString MESSAGE_FILE_ACQUISITION_INVALID_FILE ( _T("The Image Filename specified is Invalid.") );
static const CString MESSAGE_FILE_ACQUISITION_INVALID_DIRECTORY ( _T("The Image Directory specified is Invalid.") );
static const CString MESSAGE_FILE_ACQUISITION_MISSING_FILENAME ( _T("The Image Filename must be specified.") );
static const CString MESSAGE_FILE_ACQUISITION_MISSING_DIRECTORY ( _T("The Image Directory must be specified.") );
static const CString MESSAGE_FILE_ACQUISITION_INVALID_IMAGE_SIZE ( _T("The Image Size is Invalid.") );

const CSVOConfigAssistantDlg::SVProductStringMap CSVOConfigAssistantDlg::m_ProductStringMap = boost::assign::map_list_of< SVIMProductEnum, CString >
	// SYSTEM ID, DISPLAY NAME
	( SVIM_PRODUCT_FULL, SYSTEM_SVIM_FULL )
	( SVIM_PRODUCT_05, SYSTEM_SVIM_05 )
	( SVIM_PRODUCT_RGB_MONO, SYSTEM_SVIM_RGB )
	( SVIM_PRODUCT_RGB_COLOR, SYSTEM_SVIM_COLOR )
	( SVIM_PRODUCT_D3, SYSTEM_SVIM_D3 )
	( SVIM_PRODUCT_D2, SYSTEM_SVIM_D2 )
	( SVIM_PRODUCT_D1, SYSTEM_SVIM_D1 )
	( SVIM_PRODUCT_X1, SYSTEM_SVIM_X1 )
	( SVIM_PRODUCT_X2, SYSTEM_SVIM_X2 )
	( SVIM_PRODUCT_X3, SYSTEM_SVIM_X3 )
	( SVIM_PRODUCT_D3_COLOR, SYSTEM_SVIM_D3_COLOR )
	( SVIM_PRODUCT_D2_COLOR, SYSTEM_SVIM_D2_COLOR )
	( SVIM_PRODUCT_D1_COLOR, SYSTEM_SVIM_D1_COLOR )
	( SVIM_PRODUCT_X1_COLOR, SYSTEM_SVIM_X1_COLOR )
	( SVIM_PRODUCT_X2_COLOR, SYSTEM_SVIM_X2_COLOR )
	( SVIM_PRODUCT_X3_COLOR, SYSTEM_SVIM_X3_COLOR )
	( SVIM_PRODUCT_D1_HUB, SYSTEM_SVIM_D1_HUB_STRING )
	( SVIM_PRODUCT_D2_HUB, SYSTEM_SVIM_D2_HUB_STRING )
	( SVIM_PRODUCT_D3_HUB, SYSTEM_SVIM_D3_HUB_STRING )
	( SVIM_PRODUCT_X1_HUB, SYSTEM_SVIM_X1_HUB_STRING )
	( SVIM_PRODUCT_X2_GD1A, SYSTEM_SVIM_X2_GD1A )
	( SVIM_PRODUCT_X2_GD1A_COLOR, SYSTEM_SVIM_X2_GD1A_COLOR )
	( SVIM_PRODUCT_X2_GD2A, SYSTEM_SVIM_X2_GD2A )
	( SVIM_PRODUCT_X2_GD2A_COLOR, SYSTEM_SVIM_X2_GD2A_COLOR )
	( SVIM_PRODUCT_X2_GD4A, SYSTEM_SVIM_X2_GD4A )
	( SVIM_PRODUCT_X2_GD4A_COLOR, SYSTEM_SVIM_X2_GD4A_COLOR )
	( SVIM_PRODUCT_X2_GD8A, SYSTEM_SVIM_X2_GD8A )
	( SVIM_PRODUCT_X2_GD8A_COLOR, SYSTEM_SVIM_X2_GD8A_COLOR )
	( SVIM_PRODUCT_X2_GD8A_NONIO, SYSTEM_SVIM_X2_GD8A_NONIO )
	( SVIM_PRODUCT_X2_GD8A_NONIO_COLOR, SYSTEM_SVIM_X2_GD8A_NONIO_COLOR );
	;

/////////////////////////////////////////////////////////////////////////////
// CSVOConfigAssistantDlg dialog

CSVOConfigAssistantDlg::CSVOConfigAssistantDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSVOConfigAssistantDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSVOConfigAssistantDlg)
	m_sAvailableSystem = _T("");
	m_sCurrentSys = _T("");
	m_sConfigurationName = _T("");
	//}}AFX_DATA_INIT

	m_bNewConfiguration = FALSE;
	m_bModified = FALSE;
	m_bInspectionDeleted = FALSE;
	m_iLastInspectionNum = 0;

	SVIMProductEnum eSvimType = TheSVObserverApp.GetSVIMType();

	SetConfigurationSystem( eSvimType );

	const SVIMTypeInfoStruct& l_SVIMInfo = SVHardwareManifest::GetSVIMTypeInfo(m_lConfigurationType);
	MAX_NUM_DIGITIZERS_1394_HUB = l_SVIMInfo.m_MaxDigitizers;
}

CSVOConfigAssistantDlg::~CSVOConfigAssistantDlg()
{
	m_slUsedTriggers.RemoveAll();
	m_slUsedInspect.RemoveAll();
	m_CameraList.ResetContent();
	m_TriggerList.ResetContent();
	m_InspectList.ResetContent();
	m_PPQList.ResetContent();
	m_slInspectionNamesUsed.RemoveAll();
	m_slInspectionLabelsUsed.RemoveAll();
}

void CSVOConfigAssistantDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVOConfigAssistantDlg)
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_LST_ERR_LIST, m_ctlMsgList);
	DDX_Control(pDX, IDC_EDIT_CONFIGURATION_NAME, m_ctlConfigurationName);
	DDX_Control(pDX, IDC_EDIT_CURRENT_SYS, m_ctlCurrentSys);
	DDX_Control(pDX, IDC_COMBO_AVAL_SYS, m_ctlAvailableSys);
	DDX_CBString(pDX, IDC_COMBO_AVAL_SYS, m_sAvailableSystem);
	DDX_Text(pDX, IDC_EDIT_CURRENT_SYS, m_sCurrentSys);
	DDX_Text(pDX, IDC_EDIT_CONFIGURATION_NAME, m_sConfigurationName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSVOConfigAssistantDlg, CDialog)
	//{{AFX_MSG_MAP(CSVOConfigAssistantDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_AVAL_SYS, OnSelchangeComboAvalSys)
	ON_LBN_DBLCLK(IDC_LST_ERR_LIST, OnDblclkLstErrList)
	ON_EN_CHANGE(IDC_EDIT_CONFIGURATION_NAME, OnChangeEditConfigurationName)
	ON_BN_CLICKED(IDHELP, OnHelpButton)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CSVOConfigAssistantDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVOConfigAssistantDlg message handlers

BOOL CSVOConfigAssistantDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_iCameraErrors = 0;
	m_iInspectErrors = 0;
	m_iInsToolsetError = 0;
	m_iInvalidTrigger = 0;
	m_iInvalidCamera = 0;
	m_iPPQTriggerError = 0;
	m_iPPQCameraError = 0;
	m_iPPQMaintainSourceImageError = 0;
	m_i1394ColorCamError = 0;

	m_dlgPropSheet.AddPage(&m_Page1);
	m_dlgPropSheet.AddPage(&m_Page2);
	m_dlgPropSheet.AddPage(&m_Page3);
	m_dlgPropSheet.AddPage(&m_Page4);


	m_dlgPropSheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
	m_dlgPropSheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_dlgPropSheet.ModifyStyle( 0, WS_TABSTOP );

	CRect rcSheet;
	GetDlgItem( IDC_PROPERTY_SHEET )->GetWindowRect( &rcSheet );
	ScreenToClient( &rcSheet );
	m_dlgPropSheet.SetWindowPos( NULL, rcSheet.left-7, rcSheet.top-7, 0, 0, 
			SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );

	
	for (int x = 0; x < m_dlgPropSheet.GetPageCount(); x++)
	{
		m_dlgPropSheet.SetActivePage(x);
	}

	if ( m_bNewConfiguration )
	{
		m_dlgPropSheet.SetActivePage(0);	// page 1; trigger

		SetConfigurationSystem( m_lSystemType );
	}
	else
	{
		m_dlgPropSheet.SetActivePage(1);	// page 2; camera
	}
	
	//Call method to fill Available System...
	if (!m_bNewConfiguration)
	{
		m_bConfigName = TRUE;
		GetConfigurationForExisting();
		m_ctlConfigurationName.SetWindowText( TheSVObserverApp.getConfigFileName() );
		m_ctlConfigurationName.EnableWindow(FALSE);
	}
	SetupSystemComboBox();

	SetCurrentSystemDisplay();

	if (m_bNewConfiguration)
	{
		ReloadForCurrentSystem();
	}
	
	CString l_sProductString;

	//select correct product on right hand side
	l_sProductString = GetNameFromProductID( m_lConfigurationType );
	m_ctlAvailableSys.SelectString(-1,l_sProductString.operator LPCTSTR());

	OnChangeEditConfigurationName();
	SetupMessages();
	IsGood();

	return TRUE;  // return TRUE unless you set the focus to a control
}

// The purpose of this method is to populate the Available Systems combobox.
void CSVOConfigAssistantDlg::SetupSystemComboBox()
{
	CString l_sSystemString;

	bool l_IsColor = ( TheSVObserverApp.IsColorSVIM() == TRUE );

	SVIMProductEnumList list; 
	typedef std::insert_iterator<SVIMProductEnumList> Insertor;
	Insertor insertor(list, list.begin());
	SVHardwareManifest::GetSupportedSVIMList(l_IsColor, insertor);

	for (SVIMProductEnumList::iterator l_Iter = list.begin();l_Iter != list.end(); ++l_Iter)
	{
		const CString& l_Name =  GetNameFromProductID(*l_Iter);
		m_ctlAvailableSys.AddString( l_Name );
	}

	if ( !m_bNewConfiguration )
	{
		const CString& l_Name = GetNameFromProductID( m_lConfigurationType );
		const SVIMTypeInfoStruct& l_SVIMInfo = SVHardwareManifest::GetSVIMTypeInfo( m_lConfigurationType );
		if ( !l_SVIMInfo.m_Supported )
		{
			m_ctlAvailableSys.AddString( l_Name );
		}
	}
}

void CSVOConfigAssistantDlg::SetNewConfiguration(BOOL bNewFlag)
{
	m_bNewConfiguration = bNewFlag;
}

bool CSVOConfigAssistantDlg::IsNewConfiguration()
{
	return m_bNewConfiguration == TRUE;
}

BOOL CSVOConfigAssistantDlg::AddToCameraList(CString sCameraName, int iDig)
{
	return m_CameraList.AddCameraToList(sCameraName,iDig);
}

BOOL CSVOConfigAssistantDlg::AddToCameraList(CString sCameraName)
{
	return m_CameraList.AddCameraToList(sCameraName);
}

BOOL CSVOConfigAssistantDlg::AssignCameraFileToCamera(CString sCameraName, CString sCameraFile)
{
	return m_CameraList.SetCameraFile(sCameraName,sCameraFile);
}

BOOL CSVOConfigAssistantDlg::AssignDigToCamera(CString sCameraName, int iDig)
{
	return m_CameraList.SetCameraDig(sCameraName, iDig);
}

BOOL CSVOConfigAssistantDlg::AddToInspectList(CString sExternal, CString sInternal)
{
	return m_InspectList.AddInspectionToList(sInternal,sExternal);
}

BOOL CSVOConfigAssistantDlg::AddToTriggerList(CString sTriggerName, int iDig)
{
	return m_TriggerList.AddTriggerToList(SVString(sTriggerName), iDig);
}

BOOL CSVOConfigAssistantDlg::AddToPPQList(CString sPPQ, CString sCamera, CString sTrigger, CString sInspection)
{
	BOOL bRet = FALSE;
	bRet = m_PPQList.AddPPQToList(sPPQ);
	bRet = m_PPQList.AttachCameraToPPQ(sPPQ, sCamera) && bRet;
	bRet = m_PPQList.AttachInspectToPPQ(sPPQ, sInspection) && bRet;
	bRet = m_PPQList.AttachTriggerToPPQ(sPPQ, sTrigger) && bRet;
	
	return bRet;
}

BOOL CSVOConfigAssistantDlg::AddToPPQList(CString sPPQ)
{
	return m_PPQList.AddPPQToList(sPPQ);
}

void CSVOConfigAssistantDlg::OnSelchangeComboAvalSys() 
{
	CString sSelectedTxt;
	m_ctlAvailableSys.GetWindowText(sSelectedTxt);
	m_sAvailableSystem = sSelectedTxt;

	SVIMProductEnum CurrentType = m_lConfigurationType; 

	SVIMProductEnum l_ConfigurationType = GetProductIDFromName( sSelectedTxt );

	CString l_PrevName = GetNameFromProductID( CurrentType );
	SVIMProductEnum eType = GetProductIDFromName( l_PrevName );

	if ( ( !m_bNewConfiguration ) || ( m_bModified ) )
	{
		if ( SVHardwareManifest::IsCompatible(CurrentType, l_ConfigurationType) )
		{
			m_bModified = TRUE;

			m_lConfigurationType = l_ConfigurationType;
			UpdateAvailableSystems( CurrentType, m_lConfigurationType );
		}
		else
		{
			// SEJ - check if I/O digital to Non I/O digital or vice versa
			if ((SVHardwareManifest::IsNonIOSVIM(l_ConfigurationType) && !SVHardwareManifest::IsNonIOSVIM(CurrentType)) ||
				(!SVHardwareManifest::IsNonIOSVIM(l_ConfigurationType) && SVHardwareManifest::IsNonIOSVIM(CurrentType)))
			{
				// need to check if
				if ( AfxMessageBox(_T("By switching the system (product) the camera, trigger, inspection and ppq data will be reset, do you wish to continue?"),MB_YESNO) == IDYES)
				{
					m_lConfigurationType = l_ConfigurationType;
					m_bModified = TRUE;
					UpdateAvailableSystems( CurrentType, m_lConfigurationType );
				}
			}
			else
			{
				if ( AfxMessageBox(_T("By switching the system (product) the camera files will become invalid, do you wish to continue?"),MB_YESNO) == IDYES)
				{
					m_lConfigurationType = l_ConfigurationType;

					ClearMessages();
					CheckAgainstCurrentList();
					
					SystemChangeResetCamera( m_lConfigurationType );
					m_bModified = TRUE;
					IsGood();

					if ( SVHardwareManifest::IsDigitalSVIM( m_lConfigurationType ) )
					{
						ConvertToDigital(m_lConfigurationType);
						SetupMessages();
						if( SVHardwareManifest::IsDigitalSVIM( m_lSystemType ))
						{
							m_Page2.ShowCameraDlgBtn( SW_SHOW );
						}
					}
					else
					{
						if ( SVHardwareManifest::IsDigitalSVIM( eType ) )
						{
							ConvertToDigital(m_lConfigurationType); //eType);
							m_Page2.DisableAdvanceBtn();
							SetupMessages();
						}
						m_Page2.ShowCameraDlgBtn( SW_HIDE );
					}
					UpdateAvailableSystems( CurrentType, m_lConfigurationType );
				}
				else
				{
					UINT l_NextIndex = m_ctlAvailableSys.FindStringExact( 0, l_PrevName );

					if ( l_NextIndex != CB_ERR )
					{
						m_ctlAvailableSys.SetCurSel( l_NextIndex );
					}
					else
					{
						// There was a problem.  Set the selection to nothing.
						m_ctlAvailableSys.SetCurSel( -1 );
					}
					IsGood();
				}
				return;
			}
		}
	}

	m_lConfigurationType = l_ConfigurationType;

	if ( (m_bNewConfiguration) && (!m_bModified) )
	{
		ClearMessages();
		ReloadForCurrentSystem();
	}

	if ( m_bModified )
	{
		ClearMessages();
		ReloadForCurrentSystem();
		SetupMessages();
		CheckAgainstCurrentList();
	}
	IsGood();
}

// Certain obsolete SVIM Products should be removed from the Available Systems
// list once another item has been selected.  Unless one of these is the current
// configuration type, this method will do nothing.
void CSVOConfigAssistantDlg::UpdateAvailableSystems( SVIMProductEnum p_CurrentConfigurationType, SVIMProductEnum p_SelectedConfigurationType )
{
	const SVIMTypeInfoStruct& l_SVIMInfo = SVHardwareManifest::GetSVIMTypeInfo(p_CurrentConfigurationType);

	if ( !l_SVIMInfo.m_Supported && p_CurrentConfigurationType != p_SelectedConfigurationType )
	{
		CString l_SelectedSystemName = GetNameFromProductID( p_CurrentConfigurationType );
		UINT l_Index = m_ctlAvailableSys.FindStringExact( 0, l_SelectedSystemName );

		if ( l_Index != CB_ERR )
		{
			m_ctlAvailableSys.DeleteString( l_Index );
		}
	}
}

void CSVOConfigAssistantDlg::ReloadForCurrentSystem()
{
	if (!m_bModified)
	{
		m_CameraList.ResetContent();
		m_TriggerList.ResetContent();
		m_InspectList.ResetContent();
		m_PPQList.ResetContent();
		m_iCameraErrors = 0;

		switch( m_lConfigurationType )
		{
			case SVIM_PRODUCT_FULL:
			{
				CreateDefaultForFullSVIM();
				break;
			}
			case SVIM_PRODUCT_05:
			{
				CreateDefaultFor05SVIM();
				break;
			}
			case SVIM_PRODUCT_RGB_MONO:
			{
				CreateDefaultForRGBSVIM();
				break;
			}
			case SVIM_PRODUCT_RGB_COLOR:
			{
				CreateDefaultForColorSVIM();
				break;
			}
			case SVIM_PRODUCT_D3:
			{
				CreateDefaultForSVIMDigital(3);
				break;
			}
			case SVIM_PRODUCT_D2:
			{
				CreateDefaultForSVIMDigital(2);
				break;
			}
			case SVIM_PRODUCT_D1:
			{
				CreateDefaultForSVIMDigital(1);
				break;
			}
			case SVIM_PRODUCT_X1:
			{
				CreateDefaultForSVIMDigital(1);
				break;
			}
			case SVIM_PRODUCT_X2:
			{
				CreateDefaultForSVIMDigital(2);
				break;
			}
			case SVIM_PRODUCT_X3:
			{
				CreateDefaultForSVIMDigital(3);
				break;
			}
			case SVIM_PRODUCT_D3_COLOR:
			{
				CreateDefaultForSVIMColorDigital(3);
				break;
			}
			case SVIM_PRODUCT_D2_COLOR:
			{
				CreateDefaultForSVIMColorDigital(2);
				break;
			}
			case SVIM_PRODUCT_D1_COLOR:
			{
				CreateDefaultForSVIMColorDigital(1);
				break;
			}
			case SVIM_PRODUCT_X1_COLOR:
			{
				CreateDefaultForSVIMColorDigital(1);
				break;
			}
			case SVIM_PRODUCT_X2_COLOR:
			{
				CreateDefaultForSVIMColorDigital(2);
				break;
			}
			case SVIM_PRODUCT_X3_COLOR:
			{
				CreateDefaultForSVIMColorDigital(3);
				break;
			}
			case SVIM_PRODUCT_D1_HUB:
			{
				CreateDefaultForSVIMDigitalHub(1);
				break;
			}
			case SVIM_PRODUCT_X1_HUB:
			{
				CreateDefaultForSVIMDigitalHub(1);
				break;
			}
			case SVIM_PRODUCT_D2_HUB:
			{
				CreateDefaultForSVIMDigitalHub(2);
				break;
			}
			case SVIM_PRODUCT_D3_HUB:
			{
				CreateDefaultForSVIMDigitalHub(3);
				break;
			}
			case SVIM_PRODUCT_X2_GD1A:
			{
				CreateDefaultForSVIMDigital(1);
				break;
			}
			case SVIM_PRODUCT_X2_GD1A_COLOR:
			{
				CreateDefaultForSVIMColorDigital(1);
				break;
			}
			case SVIM_PRODUCT_X2_GD2A:
			{
				CreateDefaultForSVIMDigital(1);
				break;
			}
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			{
				CreateDefaultForSVIMColorDigital(1);
				break;
			}
			case SVIM_PRODUCT_X2_GD8A:
			{
				CreateDefaultForSVIMDigital(2);
				break;
			}
			case SVIM_PRODUCT_X2_GD8A_COLOR:
			{
				CreateDefaultForSVIMColorDigital(2);
				break;
			}
			case SVIM_PRODUCT_X2_GD8A_NONIO:
			{
				CreateDefaultForSVIMDigitalNonIO(2);
				break;
			}
			case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				CreateDefaultForSVIMDigitalNonIO(2);
				break;
			}
			default:
			{
				m_lConfigurationType = SVIM_PRODUCT_FULL;

				CreateDefaultForFullSVIM();

				CString l_sProductString = GetNameFromProductID( m_lConfigurationType );
				m_ctlAvailableSys.SelectString(-1,l_sProductString.operator LPCTSTR());

				break;
			}
		}
		m_Page1.SetupList();
		m_Page2.SetupList();
		m_Page3.SetupList();
		m_Page4.SetupList();
		m_ctlMsgList.ResetContent();
		SetupMessages();
		OnChangeEditConfigurationName();
		
		if (m_CameraList.GetCameraListCount() > 0)
		{
			m_bCameraGood = TRUE;
		}
		if (m_InspectList.GetInspectionListCount() > 0)
		{
			m_bInspectionGood = TRUE;
		}
		if (m_TriggerList.GetTriggerListCount() > 0)
		{
			m_bTriggerGood = TRUE;
		}
		if (m_PPQList.GetPPQListCount() > 0 )
		{
			m_bPPQGood = TRUE;
		}
	}
	else
	{
		//either a modified config or an existing config
	}
}

void CSVOConfigAssistantDlg::CreateDefaultForFullSVIM()
{
	CString sCameraName;
	CString sTriggerName;
	CString sInspectionName;
	CString sPPQName;
	
	//create 4 of everything
	for (int iCnt = 0; iCnt < 4; iCnt++)
	{
		//add a camera
		sCameraName = GetNextCameraName();
		m_CameraList.AddCameraToList(sCameraName,iCnt);
		
		//add a trigger
		sTriggerName = GetNextTriggerName();
		m_TriggerList.AddTriggerToList(SVString(sTriggerName), iCnt);
		
		//add an inspection...
		sInspectionName = GetNextInspectionName();
		m_InspectList.AddInspectionToList(sInspectionName,sInspectionName);
		m_InspectList.SetToolsetImage(sInspectionName,sCameraName);
		m_InspectList.SetNewDisableMethod(sInspectionName,_T( "Method 1" ));
		m_InspectList.SetEnableAuxiliaryExtent( sInspectionName, 0 );

		//add a ppq, attach trigger, camera and inspection to it.
		sPPQName = GetNextPPQName();
		m_PPQList.AddPPQToList(sPPQName);
		m_PPQList.AttachCameraToPPQ(sPPQName,sCameraName);
		m_PPQList.AttachInspectToPPQ(sPPQName,sInspectionName);
		AddUsedInspect(sInspectionName);
		m_slInspectionNamesUsed.AddTail(sInspectionName);
		m_slInspectionLabelsUsed.AddTail(sInspectionName);
		m_PPQList.AttachTriggerToPPQ(sPPQName,sTriggerName);
		AddUsedTrigger(sTriggerName);
	}
}

void CSVOConfigAssistantDlg::CreateDefaultFor05SVIM()
{
	CString sCameraName;
	CString sTriggerName;
	CString sInspectionName;
	CString sPPQName;
	
	//create 2 of everything
	for (int iCnt = 0; iCnt < 2; iCnt++)
	{
		//add a camera
		sCameraName = GetNextCameraName();
		m_CameraList.AddCameraToList(sCameraName,iCnt);
		
		//add a trigger
		sTriggerName = GetNextTriggerName();
		m_TriggerList.AddTriggerToList(SVString(sTriggerName), iCnt);
		
		//add an inspection...
		sInspectionName = GetNextInspectionName();
		m_InspectList.AddInspectionToList(sInspectionName,sInspectionName);
		m_InspectList.SetToolsetImage(sInspectionName,sCameraName);
		m_InspectList.SetNewDisableMethod( sInspectionName, _T( "Method 1" ) );
		m_InspectList.SetEnableAuxiliaryExtent( sInspectionName, 0 );
		m_slInspectionNamesUsed.AddTail(sInspectionName);
		m_slInspectionLabelsUsed.AddTail(sInspectionName);
		
		//add a ppq, attach trigger, camera and inspection to it.
		sPPQName = GetNextPPQName();
		m_PPQList.AddPPQToList(sPPQName);
		m_PPQList.AttachCameraToPPQ(sPPQName,sCameraName);
		m_PPQList.AttachInspectToPPQ(sPPQName,sInspectionName);
		AddUsedInspect(sInspectionName);
		m_PPQList.AttachTriggerToPPQ(sPPQName,sTriggerName);
		AddUsedTrigger(sTriggerName);
	}
}

void CSVOConfigAssistantDlg::CreateDefaultForRGBSVIM()
{
	//create 1 trigger, 3 camera's and inspections, 1 PPQ
	CString sCameraName;
	CString sTriggerName;
	CString sInspectionName;
	CString sPPQName;
	
	sTriggerName = GetNextTriggerName();
	m_TriggerList.AddTriggerToList(SVString(sTriggerName), 0);
	sPPQName = GetNextPPQName();
	m_PPQList.AddPPQToList(sPPQName);
	m_PPQList.AttachTriggerToPPQ(sPPQName,sTriggerName);
	AddUsedTrigger(sTriggerName);
	
	for (int i = 0; i < 3; i++)
	{
		sCameraName = GetNextCameraName();
		m_CameraList.AddCameraToList(sCameraName,0,i);
		sInspectionName = GetNextInspectionName();
		m_InspectList.AddInspectionToList(sInspectionName,sInspectionName);
		m_InspectList.SetToolsetImage(sInspectionName,sCameraName);
		m_InspectList.SetNewDisableMethod( sInspectionName, _T( "Method 1" ) );
		m_InspectList.SetEnableAuxiliaryExtent( sInspectionName, 0 );
		m_slInspectionNamesUsed.AddTail(sInspectionName);
		m_slInspectionLabelsUsed.AddTail(sInspectionName);
		AddUsedInspect(sInspectionName);
		m_PPQList.AttachCameraToPPQ(sPPQName,sCameraName);
		m_PPQList.AttachInspectToPPQ(sPPQName,sInspectionName);
	}
}

void CSVOConfigAssistantDlg::CreateDefaultForColorSVIM()
{
	CString sCameraName;
	CString sTriggerName;
	CString sInspectionName;
	CString sPPQName;
	
	//create 1 of everything
	//add a camera
	sCameraName = GetNextCameraName();
	m_CameraList.AddCameraToList(sCameraName,0);
	
	//add a trigger
	sTriggerName = GetNextTriggerName();
	m_TriggerList.AddTriggerToList(SVString(sTriggerName), 0);
	
	//add an inspection...
	sInspectionName = GetNextInspectionName();
	m_InspectList.AddInspectionToList(sInspectionName,sInspectionName);
	m_InspectList.SetToolsetImage(sInspectionName,sCameraName);
	m_InspectList.SetNewDisableMethod(sInspectionName,_T( "Method 1" ));
	m_InspectList.SetEnableAuxiliaryExtent( sInspectionName, 0 );
	m_slInspectionNamesUsed.AddTail(sInspectionName);
	m_slInspectionLabelsUsed.AddTail(sInspectionName);
	
	//add a ppq, attach trigger, camera and inspection to it.
	sPPQName = GetNextPPQName();
	m_PPQList.AddPPQToList(sPPQName);
	m_PPQList.AttachCameraToPPQ(sPPQName,sCameraName);
	m_PPQList.AttachInspectToPPQ(sPPQName,sInspectionName);
	AddUsedInspect(sInspectionName);
	m_PPQList.AttachTriggerToPPQ(sPPQName,sTriggerName);
	AddUsedTrigger(sTriggerName);
}

// default setups for the Digital Configs...

void CSVOConfigAssistantDlg::CreateDefaultForSVIMDigitalNonIO(int iNumber)
{
	for ( int iCnt = 0; iCnt < iNumber; iCnt++ )
	{
		//add a camera
		CString sCameraName = GetNextCameraName();
		m_CameraList.AddCameraToList(sCameraName, iCnt);
		
		//add a trigger
		CString sTriggerName = GetNextTriggerName(SV_CAMERA_TRIGGER_NAME);
		m_TriggerList.AddTriggerToList(SVString(sTriggerName), iCnt);
		
		//add an inspection...
		CString sInspectionName = GetNextInspectionName();
		m_InspectList.AddInspectionToList(sInspectionName, sInspectionName);
		m_InspectList.SetToolsetImage(sInspectionName, sCameraName);
		m_slInspectionNamesUsed.AddTail(sInspectionName);
		m_slInspectionLabelsUsed.AddTail(sInspectionName);
		
		//add a ppq, attach trigger, camera and inspection to it.
		CString sPPQName = GetNextPPQName();
		m_PPQList.AddPPQToList(sPPQName);
		m_PPQList.AttachCameraToPPQ(sPPQName, sCameraName);
		m_PPQList.AttachInspectToPPQ(sPPQName, sInspectionName);
		AddUsedInspect(sInspectionName);
		m_PPQList.AttachTriggerToPPQ(sPPQName, sTriggerName);
		AddUsedTrigger(sTriggerName);
	}
}

void CSVOConfigAssistantDlg::CreateDefaultForSVIMDigital(int iNumber)
{
	//digital SVIM previous known as SVIM 1394
	CString sCameraName;
	CString sTriggerName;
	CString sInspectionName;
	CString sPPQName;

	for ( int iCnt = 0; iCnt < iNumber; iCnt++ )
	{
		//add a camera
		sCameraName = GetNextCameraName();
		m_CameraList.AddCameraToList(sCameraName,iCnt);
		
		//add a trigger
		sTriggerName = GetNextTriggerName();
		m_TriggerList.AddTriggerToList(SVString(sTriggerName), iCnt);
		
		//add an inspection...
		sInspectionName = GetNextInspectionName();
		m_InspectList.AddInspectionToList(sInspectionName,sInspectionName);
		m_InspectList.SetToolsetImage(sInspectionName,sCameraName);
		m_slInspectionNamesUsed.AddTail(sInspectionName);
		m_slInspectionLabelsUsed.AddTail(sInspectionName);
		
		//add a ppq, attach trigger, camera and inspection to it.
		sPPQName = GetNextPPQName();
		m_PPQList.AddPPQToList(sPPQName);
		m_PPQList.AttachCameraToPPQ(sPPQName,sCameraName);
		m_PPQList.AttachInspectToPPQ(sPPQName,sInspectionName);
		AddUsedInspect(sInspectionName);
		m_PPQList.AttachTriggerToPPQ(sPPQName,sTriggerName);
		AddUsedTrigger(sTriggerName);
	}
}

void CSVOConfigAssistantDlg::CreateDefaultForSVIMDigitalHub(int iNumber)
{
	CString sCameraName;
	CString sTriggerName;
	CString sInspectionName;
	CString sPPQName;

	ASSERT( GetProductType() == SVIM_PRODUCT_D1_HUB
		|| GetProductType() == SVIM_PRODUCT_D2_HUB
		|| GetProductType() == SVIM_PRODUCT_D3_HUB
		|| GetProductType() == SVIM_PRODUCT_X1_HUB
	);

	const int NUM_CAMERAS_PER_BOARD = 3;

	int iCameraIndex=0;
	for ( int iBoard = 0; iBoard < iNumber; iBoard++ )
	{
		int iCamera( 0 );

		// one trigger, one PPQ per board
		// multiple cameras and inspections
		std::vector<CString> aCameraNames;
		std::vector<CString> aInspectionNames;

		for ( iCamera = 0; iCamera < NUM_CAMERAS_PER_BOARD; iCamera++ )
		{
			//add a camera
			sCameraName = GetNextCameraName();
			aCameraNames.push_back( sCameraName );
			m_CameraList.AddCameraToList(sCameraName, iCameraIndex);
			iCameraIndex++;
			
			//add an inspection...
			sInspectionName = GetNextInspectionName();
			aInspectionNames.push_back( sInspectionName );
			m_InspectList.AddInspectionToList(sInspectionName, sInspectionName);
			m_InspectList.SetToolsetImage(sInspectionName, sCameraName);
			m_slInspectionNamesUsed.AddTail(sInspectionName);
			m_slInspectionLabelsUsed.AddTail(sInspectionName);
		}

		//add a trigger
		sTriggerName = GetNextTriggerName();
		m_TriggerList.AddTriggerToList(SVString(sTriggerName), iBoard);
		
		//add a ppq, attach trigger, cameras and inspections to it.
		sPPQName = GetNextPPQName();
		m_PPQList.AddPPQToList(sPPQName);
		for ( iCamera = 0; iCamera < NUM_CAMERAS_PER_BOARD; iCamera++ )
		{
			m_PPQList.AttachCameraToPPQ(sPPQName, aCameraNames[iCamera]);
			m_PPQList.AttachInspectToPPQ(sPPQName, aInspectionNames[iCamera]);
			AddUsedInspect(aInspectionNames[iCamera]);
		}
		m_PPQList.AttachTriggerToPPQ(sPPQName,sTriggerName);
		AddUsedTrigger(sTriggerName);
	}
}


void CSVOConfigAssistantDlg::CreateDefaultForSVIMColorDigital(int iNumber)
{
	CString sCameraName;
	CString sTriggerName;
	CString sInspectionName;
	CString sPPQName;
	
	for ( int iCnt = 0; iCnt < iNumber; iCnt++ )
	{
		//add a camera
		sCameraName = GetNextCameraName();
		m_CameraList.AddCameraToList(sCameraName,iCnt);
		
		//add a trigger
		sTriggerName = GetNextTriggerName();
		m_TriggerList.AddTriggerToList(SVString(sTriggerName), iCnt);
		
		//add an inspection...
		sInspectionName = GetNextInspectionName();
		m_InspectList.AddInspectionToList(sInspectionName,sInspectionName);
		m_InspectList.SetToolsetImage(sInspectionName,sCameraName);
		m_slInspectionNamesUsed.AddTail(sInspectionName);
		m_slInspectionLabelsUsed.AddTail(sInspectionName);
		
		//add a ppq, attach trigger, camera and inspection to it.
		sPPQName = GetNextPPQName();
		m_PPQList.AddPPQToList(sPPQName);
		m_PPQList.AttachCameraToPPQ(sPPQName,sCameraName);
		m_PPQList.AttachInspectToPPQ(sPPQName,sInspectionName);
		AddUsedInspect(sInspectionName);
		m_PPQList.AttachTriggerToPPQ(sPPQName,sTriggerName);
		AddUsedTrigger(sTriggerName);
	}
}

//end of default setups
CString CSVOConfigAssistantDlg::GetNextCameraName()
{
	int iRet = 0;
	BOOL bFound = TRUE;
	CString sSearchName;
	
	while ( bFound )
	{
		iRet++;
		sSearchName.Format("%s%d",CAMERA_FIXED_NAME,iRet);
		bFound = m_CameraList.IsCameraInList(sSearchName);
	}
	m_iNextCameraNumber = iRet;
	return sSearchName;
}

int CSVOConfigAssistantDlg::GetNextCameraNumber()
{
	return m_iNextCameraNumber;
}

CString CSVOConfigAssistantDlg::GetNextInspectionName()
{
	int iRet = 0;
	BOOL bFound = TRUE;
	CString sSearchName;
	
	if ( m_bInspectionDeleted )
	{
		iRet = m_iLastInspectionNum;
	}
	
	while ( bFound )
	{
		iRet++;
		sSearchName.Format("%s%d", INSPECT_FIXED_NAME, iRet);
		bFound = m_InspectList.IsInspectionInList(sSearchName);
	}
	return sSearchName;
}

CString CSVOConfigAssistantDlg::GetNextTriggerName(const CString& baseName)
{
	int iRet = m_TriggerList.GetNextTriggerID();
	
	CString name;
	name.Format(_T("%s%d"), baseName, iRet + 1);
	return name;
}

int CSVOConfigAssistantDlg::GetNextTriggerID() const
{
	return m_TriggerList.GetNextTriggerID();
}

CString CSVOConfigAssistantDlg::GetNextPPQName()
{
	int iRet = 0;
	BOOL bFound = TRUE;
	CString sSearchName;
	
	while (bFound)
	{
		iRet++;
		sSearchName.Format("%s%d",PPQ_FIXED_NAME,iRet);
		bFound = m_PPQList.IsPPQInList(sSearchName);
	}
	return sSearchName;
}

CString CSVOConfigAssistantDlg::GetInspectionNameFromLabel(CString sInspectLabel)
{
	return m_InspectList.GetInspectionName(sInspectLabel);
}

CString CSVOConfigAssistantDlg::GetInspectionLabelFromName(CString sInspectName)
{
	return m_InspectList.GetInspectionLabel(sInspectName);
}

BOOL CSVOConfigAssistantDlg::RenameInspection(CString sInspectLabel, CString sNewName)
{
	CSVOInspectionObj *pObj = GetInspectionObjectByName(sInspectLabel);
	CString sOldName = pObj->GetInspectionName();
	
	//check to see if the old message is in the list.  if it is replace it with the
	// new inspection name...
	
	CString sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sOldName, INSPECTION_ERROR);    
	
	if (RemoveMessageFromList(sMessage))
	{//add new message
		sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sNewName, INSPECTION_ERROR);
		AddMessageToList(PPQ_DLG,sMessage);
	}
	else
	{
		// inspection is attached. do nothing
	}
	sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sOldName, TOOLSET_IMAGE_ERROR);
	if ( RemoveMessageFromList(sMessage) )
	{
		sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sNewName, TOOLSET_IMAGE_ERROR);
		AddMessageToList(INSPECT_DLG,sMessage);
	}
	
	return m_InspectList.ReNameInspection(sInspectLabel,sNewName);
}

int CSVOConfigAssistantDlg::GetCameraListCount() const
{
	return m_CameraList.GetCameraListCount();
}

int CSVOConfigAssistantDlg::GetInspectionListCount() const
{
	return m_InspectList.GetInspectionListCount();
}

int CSVOConfigAssistantDlg::GetTriggerListCount() const
{
	return m_TriggerList.GetTriggerListCount();
}

int CSVOConfigAssistantDlg::GetPPQListCount() const
{
	return m_PPQList.GetPPQListCount();
}

CSVOInspectionObj *CSVOConfigAssistantDlg::GetInspectionObject(int iPos)
{
	return m_InspectList.GetInspectionByPosition(iPos);
}

CSVOInspectionObj *CSVOConfigAssistantDlg::GetInspectionObjectByName(CString sName)
{
	return m_InspectList.GetInspectionByName(sName);
}

CSVOCameraObj *CSVOConfigAssistantDlg::GetCameraObject(int iPos)
{
	return m_CameraList.GetCameraObjectByPosition(iPos);
}

const CSVOCameraObj *CSVOConfigAssistantDlg::GetCameraObject(int iPos) const
{
	return m_CameraList.GetCameraObjectByPosition(iPos);
}

CSVOCameraObj *CSVOConfigAssistantDlg::GetCameraObjectByName(CString sCameraName)
{
	return m_CameraList.GetCameraObjectByName(sCameraName);
}

CSVOTriggerObj *CSVOConfigAssistantDlg::GetTriggerObject(int iPos)
{
	return m_TriggerList.GetTriggerObjectByPosition(iPos);
}

CSVOTriggerObj *CSVOConfigAssistantDlg::GetTriggerObjectByName(CString sTriggerName)
{
	return m_TriggerList.GetTriggerObjectByName(SVString(sTriggerName));
}

CSVOPPQObj *CSVOConfigAssistantDlg::GetPPQObject(int iPos)
{
	return m_PPQList.GetPPQObjectByPosition(iPos);
}

CSVOPPQObj *CSVOConfigAssistantDlg::GetPPQObjectByName(CString sPPQName)
{
	return m_PPQList.GetPPQObjectByName(sPPQName);
}

BOOL CSVOConfigAssistantDlg::RemoveCameraFromList(CString sCameraName)
{
	return m_CameraList.RemoveCameraFromList(sCameraName);
}

BOOL CSVOConfigAssistantDlg::RemoveInspectionFromList(CString sInspection)
{
	if (IsInspectUsed(sInspection))
	{
		RemoveUsedInspect(sInspection);
	}
	InspectionDeletedCheckAgainstPPQ(sInspection);
	return m_InspectList.RemoveInspectionFromList(sInspection);
}

BOOL CSVOConfigAssistantDlg::RemoveTriggerFromList(CString sTriggerName)
{
	if (IsTriggerUsed(sTriggerName))
	{
		RemoveUsedTrigger(sTriggerName);
	}
	return m_TriggerList.RemoveTriggerFromList(SVString(sTriggerName));
}

BOOL CSVOConfigAssistantDlg::RemovePPQFromList(CString sPPQ)
{
	CSVOPPQObj *pObj = GetPPQObjectByName(sPPQ);
	
	RemoveUsedTrigger(pObj->GetAttachedTriggerName());

	int iInsCnt = pObj->GetAttachedInspectionCount();
	
	for (int i = 0; i< iInsCnt; i++)
	{
		RemoveUsedInspect(pObj->GetAttachedInspection(i));
	}
	
	return m_PPQList.RemovePPQFromList(sPPQ);
}

CString CSVOConfigAssistantDlg::BuildTrgDig(CSVOTriggerObj *pObj)
{
	CString sDigName;
	
	int iDig = pObj->GetTriggerDigNumber();
	
	if (pObj->IsAcquisitionTrigger())
	{
		sDigName = SVHardwareManifest::BuildAcquisitionTriggerDeviceName(iDig).ToString();
	}
	else if (pObj->IsSoftwareTrigger())
	{
		sDigName = SVHardwareManifest::BuildSoftwareTriggerDeviceName(iDig).ToString();
	}
	else
	{
		switch( m_lConfigurationType )
		{
			case SVIM_PRODUCT_FULL:
			case SVIM_PRODUCT_05:
			{
				sDigName.Format("%s%s%d", SVIM_BOARD_QUAD_STRING, SVIM_DIG_NAME_STRING, iDig);
				break;
			}
			case SVIM_PRODUCT_RGB_MONO:
			case SVIM_PRODUCT_RGB_COLOR:
			{
				sDigName.Format("%s%s%d", SVIM_BOARD_RGB_STRING, SVIM_DIG_NAME_STRING, iDig); 
				break;
			}
			case SVIM_PRODUCT_D1:
			case SVIM_PRODUCT_D2:
			case SVIM_PRODUCT_D3:
			case SVIM_PRODUCT_X1:
			case SVIM_PRODUCT_X2:
			case SVIM_PRODUCT_X3:
			case SVIM_PRODUCT_D1_COLOR:
			case SVIM_PRODUCT_D2_COLOR:
			case SVIM_PRODUCT_D3_COLOR:
			case SVIM_PRODUCT_X1_COLOR:
			case SVIM_PRODUCT_X2_COLOR:
			case SVIM_PRODUCT_X3_COLOR:
			case SVIM_PRODUCT_D1_HUB:
			case SVIM_PRODUCT_D2_HUB:
			case SVIM_PRODUCT_D3_HUB:
			case SVIM_PRODUCT_X1_HUB:
			case SVIM_PRODUCT_X2_GD1A:
			case SVIM_PRODUCT_X2_GD1A_COLOR:
			case SVIM_PRODUCT_X2_GD2A:
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			case SVIM_PRODUCT_X2_GD4A:
			case SVIM_PRODUCT_X2_GD4A_COLOR:
			case SVIM_PRODUCT_X2_GD8A:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
			{
				sDigName = SVHardwareManifest::BuildIOBoardTriggerDeviceName(iDig).ToString();
				break;
			}
			case SVIM_PRODUCT_X2_GD8A_NONIO:
			case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				sDigName = SVHardwareManifest::BuildAcquisitionTriggerDeviceName(iDig).ToString();
				break;
			}
		}
	}
	return sDigName;
}

BOOL CSVOConfigAssistantDlg::IsTriggerDigUsed(CString sDigString)
{
	BOOL bRet = FALSE;
	CString sTrigDig;
	
	int iTrgCnt = m_TriggerList.GetTriggerListCount();
	
	for (int i = 0; ( (i < iTrgCnt) && (!bRet) ); i++)
	{
		sTrigDig = BuildTrgDig(m_TriggerList.GetTriggerObjectByPosition(i));
		
		if (sDigString == sTrigDig)
		{
			bRet = TRUE;
		}
	}
	return bRet;
}

CString  CSVOConfigAssistantDlg::BuildDigName(CSVOCameraObj *pObj, bool bIsAcqDev)
{
	CString sDigName;
	int iDigNumber = pObj->GetDigNumber();
	int iBandNum = pObj->GetBandNumber();
	
	if (pObj->IsFileAcquisition())
	{
		switch( m_lConfigurationType )
		{
			case SVIM_PRODUCT_FULL:
			case SVIM_PRODUCT_05:
			{
				sDigName.Format("%s%s%d%s%d", SVIM_BOARD_FILEACQUISITION_STRING, SVIM_DIG_NAME_STRING, iDigNumber, SVIM_CHN_NAME_STRING, iBandNum);
				break;
			}
			case SVIM_PRODUCT_RGB_MONO:
			case SVIM_PRODUCT_RGB_COLOR:
			{
				if (m_lConfigurationType == SVIM_PRODUCT_RGB_COLOR || bIsAcqDev )
				{
					sDigName.Format("%s%s%d%s", SVIM_BOARD_FILEACQUISITION_STRING, SVIM_DIG_NAME_STRING, iDigNumber, SVIM_CHN_ALL_STRING); 
				}
				else
				{
					sDigName.Format("%s%s%d%s%d", SVIM_BOARD_FILEACQUISITION_STRING, SVIM_DIG_NAME_STRING, iDigNumber, SVIM_CHN_NAME_STRING, iBandNum); 
				}
				break;
			}
			case SVIM_PRODUCT_D1:
			case SVIM_PRODUCT_D2:
			case SVIM_PRODUCT_D3:
			case SVIM_PRODUCT_X1:
			case SVIM_PRODUCT_X2:
			case SVIM_PRODUCT_X3:
			case SVIM_PRODUCT_D1_HUB:
			case SVIM_PRODUCT_D2_HUB:
			case SVIM_PRODUCT_D3_HUB:
			case SVIM_PRODUCT_X1_HUB:
			case SVIM_PRODUCT_X2_GD1A:
			case SVIM_PRODUCT_X2_GD2A:
			case SVIM_PRODUCT_X2_GD4A:
			case SVIM_PRODUCT_X2_GD8A:
			case SVIM_PRODUCT_X2_GD8A_NONIO:
			{
				sDigName.Format("%s%s%d%s%d", SVIM_BOARD_FILEACQUISITION_STRING, SVIM_DIG_NAME_STRING, iDigNumber, SVIM_CHN_NAME_STRING, iBandNum);
				break;
			}
			case SVIM_PRODUCT_D1_COLOR:
			case SVIM_PRODUCT_D2_COLOR:
			case SVIM_PRODUCT_D3_COLOR:
			case SVIM_PRODUCT_X1_COLOR:
			case SVIM_PRODUCT_X2_COLOR:
			case SVIM_PRODUCT_X3_COLOR:
			case SVIM_PRODUCT_X2_GD1A_COLOR:
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			case SVIM_PRODUCT_X2_GD4A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				sDigName.Format("%s%s%d%s", SVIM_BOARD_FILEACQUISITION_STRING, SVIM_DIG_NAME_STRING, iDigNumber, SVIM_CHN_ALL_STRING/*, iBandNum*/);
				break;
			}
		} 
	}
	else
	{
		switch( m_lConfigurationType )
		{
			case SVIM_PRODUCT_FULL:
			case SVIM_PRODUCT_05:
			{
				sDigName.Format("%s%s%d%s%d", SVIM_BOARD_QUAD_STRING, SVIM_DIG_NAME_STRING, iDigNumber, SVIM_CHN_NAME_STRING, iBandNum);
				break;
			}
			case SVIM_PRODUCT_RGB_MONO:
			case SVIM_PRODUCT_RGB_COLOR:
			{
				if (m_lConfigurationType == SVIM_PRODUCT_RGB_COLOR || bIsAcqDev )
				{
					sDigName.Format("%s%s%d%s"
						,SVIM_BOARD_RGB_STRING, SVIM_DIG_NAME_STRING, iDigNumber, SVIM_CHN_ALL_STRING); 
				}
				else
				{
					sDigName.Format("%s%s%d%s%d"
						,SVIM_BOARD_RGB_STRING, SVIM_DIG_NAME_STRING, iDigNumber, SVIM_CHN_NAME_STRING, iBandNum); 
				}
				break;
			}
			case SVIM_PRODUCT_D1:
			case SVIM_PRODUCT_D2:
			case SVIM_PRODUCT_D3:
			case SVIM_PRODUCT_X1:
			case SVIM_PRODUCT_X2:
			case SVIM_PRODUCT_X3:
			case SVIM_PRODUCT_D1_HUB:
			case SVIM_PRODUCT_D2_HUB:
			case SVIM_PRODUCT_D3_HUB:
			case SVIM_PRODUCT_X1_HUB:
			{
				sDigName.Format("%s%s%d%s%d", SVIM_BOARD_1394_STRING, SVIM_DIG_NAME_STRING, iDigNumber, SVIM_CHN_NAME_STRING, iBandNum);

				break;
			}
			case SVIM_PRODUCT_D1_COLOR:
			case SVIM_PRODUCT_D2_COLOR:
			case SVIM_PRODUCT_D3_COLOR:
			case SVIM_PRODUCT_X1_COLOR:
			case SVIM_PRODUCT_X2_COLOR:
			case SVIM_PRODUCT_X3_COLOR:
			{
				sDigName.Format("%s%s%d%s", SVIM_BOARD_1394_STRING, SVIM_DIG_NAME_STRING, iDigNumber, SVIM_CHN_ALL_STRING/*, iBandNum*/);

				break;
			}

			case SVIM_PRODUCT_X2_GD1A:
			case SVIM_PRODUCT_X2_GD2A:
			case SVIM_PRODUCT_X2_GD4A:
			case SVIM_PRODUCT_X2_GD8A:
			case SVIM_PRODUCT_X2_GD8A_NONIO:
			{
				sDigName.Format("%s%s%d%s%d", SVIM_BOARD_MATROX_GIGE, SVIM_DIG_NAME_STRING, iDigNumber, SVIM_CHN_NAME_STRING, iBandNum);
				break;
			}

			case SVIM_PRODUCT_X2_GD1A_COLOR:
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			case SVIM_PRODUCT_X2_GD4A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				sDigName.Format("%s%s%d%s", SVIM_BOARD_MATROX_GIGE, SVIM_DIG_NAME_STRING, iDigNumber, SVIM_CHN_ALL_STRING);
				break;
			}
		} 
	}
	return sDigName;
}

BOOL CSVOConfigAssistantDlg::IsDigitizerUsed(CString sDigString)
{
	BOOL bRet = FALSE;
	CString sCameraDig;
	
	int iCamCnt = m_CameraList.GetCameraListCount();
	
	for (int i = 0; ( (i < iCamCnt) && (!bRet) ); i++)
	{
		sCameraDig = BuildDigName(m_CameraList.GetCameraObjectByPosition(i));
		
		if (sDigString == sCameraDig)
		{
			bRet = TRUE;
		}
	}
	return bRet;
}

BOOL CSVOConfigAssistantDlg::IsCameraInList(CString sCameraName)
{
	return m_CameraList.IsCameraInList(sCameraName);
}

BOOL CSVOConfigAssistantDlg::IsInspectionInList(CString sInspectionName)
{
	return m_InspectList.IsInspectionInList(sInspectionName);
}

BOOL CSVOConfigAssistantDlg::IsInspectionNameInList(CString sInspectionName)
{
	return m_InspectList.IsInspectionNameInList(sInspectionName);
}

bool CSVOConfigAssistantDlg::IsTriggerInList(CString sTriggerName) const
{
	return m_TriggerList.IsTriggerInList(SVString(sTriggerName));
}

BOOL CSVOConfigAssistantDlg::IsPPQInList(CString sPPQName)
{
	return m_PPQList.IsPPQInList(sPPQName);
}

void CSVOConfigAssistantDlg::SetCurrentSystem(long lSysValue)
{
	m_lSystemType = (SVIMProductEnum) lSysValue;
}

void CSVOConfigAssistantDlg::SetConfigurationSystem(long lSysValue)
{
	m_lConfigurationType = (SVIMProductEnum) lSysValue;

	switch( m_lConfigurationType )
	{
		case SVIM_PRODUCT_INVALID_TYPE:
		case SVIM_PRODUCT_TYPE_UNKNOWN:
		{
			if( TheSVObserverApp.IsColorSVIM() )
			{
				m_lConfigurationType = SVIM_PRODUCT_D1_COLOR;
			}
			else
			{
				m_lConfigurationType = SVIM_PRODUCT_FULL;
			}
			break;
		}
	}
}

void CSVOConfigAssistantDlg::SetCurrentSystemDisplay()
{
	CString sSystemName;
	
	sSystemName = GetNameFromProductID( m_lSystemType );
	if ( sSystemName == _T("") )
	{
		sSystemName = _T("<UNKNOWN>");
	}
	
	m_ctlCurrentSys.SetWindowText(sSystemName);
}

void CSVOConfigAssistantDlg::VerifyList()
{
	// go thru PPQ list checking to make sure if attached items still exist...
	int iPPQCnt = m_PPQList.GetPPQListCount();
	int iCamCnt;
	int iInsCnt;
	
	CSVOPPQObj *pObj;
	for (int i = 0; i < iPPQCnt; i++)
	{
		pObj = m_PPQList.GetPPQObjectByPosition(i);
		iCamCnt = pObj->GetAttachedCameraCount();
		for (int iC = 0; iC < iCamCnt; iC++)
		{
			if ( !m_CameraList.IsCameraInList(pObj->GetAttachedCamera(iC)) )
			{
				pObj->DetachCameraFromPPQ(pObj->GetAttachedCamera(iC));
			}
		}
		iInsCnt = pObj->GetAttachedInspectionCount();
		for (int iI = 0; iI < iInsCnt; iI++)
		{
			if ( !m_InspectList.IsInspectionInList(pObj->GetAttachedInspection(iI)) )
			{
				pObj->DetachInspectionFromPPQ(pObj->GetAttachedInspection(iI));
			}
		}
		
		if ( !m_TriggerList.IsTriggerInList(SVString(pObj->GetAttachedTriggerName())) )
		{
			pObj->DetachTriggerFromPPQ();
		}
	}
}

BOOL CSVOConfigAssistantDlg::IsTriggerUsed(CString sTriggerName) const
{
	BOOL bRet = FALSE;
	if (m_slUsedTriggers.Find(sTriggerName) != NULL)
	{
		bRet = TRUE;
	}
	return bRet;
}

void CSVOConfigAssistantDlg::AddUsedTrigger(CString sTriggerName)
{
	m_slUsedTriggers.AddTail(sTriggerName);
}

void CSVOConfigAssistantDlg::RemoveUsedTrigger(CString sTriggerName)
{
	POSITION pos = m_slUsedTriggers.Find(sTriggerName);
	
	if (pos)
	{
		m_slUsedTriggers.RemoveAt(pos);
	}
}

int CSVOConfigAssistantDlg::GetUsedTriggerCount() const
{
	return static_cast<int>(m_slUsedTriggers.GetCount());
}

bool CSVOConfigAssistantDlg::IsSoftwareTriggerAllowed(LPCTSTR triggerName) const
{
	bool bRetVal = true;

	// Check for acquistion device and if software triggering is supported
	// For Coreco acquistion device - always return true
	// For 1394/GIGE acquistion device 
	// - check if camera file has been assigned
	// - check if Trigger_Source_Option_N (Software Oneshot) is available

	CSVOConfigAssistantDlg* pNonConstThis = const_cast<CSVOConfigAssistantDlg* >(this);

	// check if analog acquisition system
	if ( SVHardwareManifest::IsDigitalSVIM( pNonConstThis->GetProductType() ) )
	{
		// Get cameras attached to this trigger
		// this is done by getting the ppq for this trigger and iterating the cameras attached to the same ppq
		int iPpq = GetPPQListCount();
		for (int i = 0;i < iPpq;i++)
		{
			const CSVOPPQObj* pPPQ = pNonConstThis->GetPPQObject(i);
			CString attachedTriggerName = pPPQ->GetAttachedTriggerName();
			if (attachedTriggerName.CompareNoCase(triggerName) == 0)
			{
				int camCnt = pPPQ->GetAttachedCameraCount();
				for (int n = 0;n < camCnt;n++)
				{
					CString camName = pPPQ->GetAttachedCamera(n);
					// find camera in camera list
					int iCam = GetCameraListCount();
					for (int c = 0;c < iCam;c++)
					{
						const CSVOCameraObj* pCam = pNonConstThis->GetCameraObject(c);
						CString name = pCam->GetCameraDisplayName();
						if (name == camName)
						{
							bRetVal = pCam->IsSoftwareTriggerSupported();
						}
					}
				}
			}
		}
	}
	return bRetVal;
}

bool CSVOConfigAssistantDlg::IsCameraLineInputAllowed(LPCTSTR triggerName) const
{
	bool bRetVal = false;

	// Check for acquistion device and if Line Input is supported
	// For Coreco acquistion device - always return false
	// For 1394/GIGE acquistion device 
	// - check if camera file has been assigned
	// - check if Line Input Device parameter is available

	CSVOConfigAssistantDlg* pNonConstThis = const_cast<CSVOConfigAssistantDlg* >(this);

	// check if analog acquisition system
	if ( SVHardwareManifest::IsDigitalSVIM( GetProductType() ) )
	{
		// Get cameras attached to this trigger
		// this is done by getting the ppq for this trigger and iterating the cameras attached to the same ppq
		int iPpq = GetPPQListCount();
		for (int i = 0;i < iPpq;i++)
		{
			const CSVOPPQObj* pPPQ = pNonConstThis->GetPPQObject(i);
			const CString& attachedTriggerName = pPPQ->GetAttachedTriggerName();
			if (attachedTriggerName.CompareNoCase(triggerName) == 0)
			{
				int camCnt = pPPQ->GetAttachedCameraCount();
				for (int n = 0;n < camCnt;n++)
				{
					CString camName = pPPQ->GetAttachedCamera(n);
					// find camera in camera list
					int iCam = GetCameraListCount();
					for (int c = 0;c < iCam;c++)
					{
						const CSVOCameraObj* pCam = pNonConstThis->GetCameraObject(c);
						CString name = pCam->GetCameraDisplayName();
						if (name == camName)
						{
							bRetVal = pCam->IsCameraLineInputSupported();
						}
					}
				}
			}
		}
	}
	return bRetVal;
}

SVIMProductEnum CSVOConfigAssistantDlg::GetProductType() const
{
	SVIMProductEnum eProduct;
	//get the current avaliable system first.  if blank use the m_lSystemType
	CString l_AvailableSystem;
	m_ctlAvailableSys.GetWindowText(l_AvailableSystem);
	if ( !l_AvailableSystem.IsEmpty() )
	{
		eProduct = GetProductIDFromName( l_AvailableSystem );
		return eProduct;
	}
	else
	{
		return m_lConfigurationType;
	}
}

BOOL CSVOConfigAssistantDlg::IsInspectUsed(CString sInspect)
{
	BOOL bRet = FALSE;
	
	if ( m_slUsedInspect.Find(sInspect) != NULL )
	{
		bRet = TRUE;
	}
	return bRet;
}

void CSVOConfigAssistantDlg::AddUsedInspect(CString sInspect)
{
	m_slUsedInspect.AddTail(sInspect);
}

void CSVOConfigAssistantDlg::RemoveUsedInspect(CString sInspect)
{
	POSITION pos = m_slUsedInspect.Find(sInspect);
	
	if (pos)
	{
		m_slUsedInspect.RemoveAt(pos);
	}
}

int CSVOConfigAssistantDlg::GetUsedInspectCount()
{
	return static_cast<int>(m_slUsedInspect.GetCount());
}

void CSVOConfigAssistantDlg::SetModified(BOOL bModified)
{
	m_bModified = bModified;
}

void CSVOConfigAssistantDlg::OnOK() 
{
	CString sTmp = m_sConfigurationName.Right(4);
	
	UpdateData(TRUE);

	//check configuration name.  do not allow name to have extension
	if ( ( sTmp.CompareNoCase( ".svx" ) == 0) )
	{
		int iLen = m_sConfigurationName.GetLength();
		sTmp = m_sConfigurationName.Left(iLen-4);
		m_sConfigurationName = sTmp;
	}
	UpdateData(FALSE);
	
	SendDataToConfiguration();
	
	CDialog::OnOK();
}

////////////////////////////////////////////////////////
///               Message area 
///
////////////////////////////////////////////////////////
BOOL CSVOConfigAssistantDlg::AddMessageToList(int iDlg, CString sMessage)
{
	BOOL bRet = FALSE;
	if (m_ctlMsgList.FindStringExact(-1,sMessage) == LB_ERR)
	{
		//message not there add it.
		int iPos = m_ctlMsgList.AddString(sMessage);
		m_ctlMsgList.SetItemData(iPos,iDlg);
		bRet = TRUE;
	}
	return bRet;
}

BOOL CSVOConfigAssistantDlg::RemoveMessageFromList(CString sMessage)
{   
	BOOL bRet = FALSE;
	int iPos = m_ctlMsgList.FindStringExact(-1,sMessage);
	
	if (iPos != LB_ERR)
	{
		m_ctlMsgList.DeleteString(iPos);
		bRet = TRUE;
	}
	return bRet;
}
int CSVOConfigAssistantDlg::RemoveAllObjectMessages(CString sObjectName)
{
	CString sString;
	int iNumberDeleted = 0;
	int iCnt = m_ctlMsgList.GetCount();
	
	for (int i = iCnt-1; -1 < i; i--)
	{
		m_ctlMsgList.GetText(i,sString);
		if (sString.Find(sObjectName,0) != -1)
		{
			if ( sString.Find("ERROR",0) != -1)
			{
				iNumberDeleted++;
			}
			m_ctlMsgList.DeleteString(i);
		}
	}
	
	return iNumberDeleted;
}

void CSVOConfigAssistantDlg::RemoveAllDlgMessages(int iDlg)
{
	int iCnt = m_ctlMsgList.GetCount();
	
	for (int i = iCnt-1; i >= 0; --i)
	{
		int dlgTag = static_cast<int>(m_ctlMsgList.GetItemData(i));
		if (dlgTag == iDlg)
		{
			m_ctlMsgList.DeleteString(i);
		}
	}
}

bool CSVOConfigAssistantDlg::ContainsErrorMessage() const
{
	bool bRetVal = false;
	CString sString;
	int iCnt = m_ctlMsgList.GetCount();

	for (int i = 0; i < iCnt; i++)
	{
		m_ctlMsgList.GetText(i, sString);
		if ( sString.Find(ERR_STR, 0) != -1)
		{
			bRetVal = true;
			break;
		}
	}
	return bRetVal;	
}

void CSVOConfigAssistantDlg::SetupMessages()
{
	//go thru each list and setup messages.
	int iTrg = GetTriggerListCount();
	int iCam = GetCameraListCount();
	int iIns = GetInspectionListCount();
	int iPpq = GetPPQListCount();
	CSVOCameraObj *pCam;
	
	if ( iCam > 0 )
	{
		for (int iC = 0; iC < iCam; iC++)
		{
			pCam = GetCameraObject(iC);
			if (!pCam->IsCameraObjOk())
			{
				if ( AddMessageToList(CAMERA_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCam->GetCameraDisplayName(), CAMERA_INVALID_FILES)) )
				{
					m_iCameraErrors++;
				}
			}
			CheckCamera(pCam);
		}
	}
	else
	{ //configuration had no camera's.  add error message...
		AddMessageToList(CAMERA_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", CAMERA_NONE_DEFINED));
		m_bCameraGood = FALSE;
	}
	
	if ( iTrg < 1 )
	{
		m_bTriggerGood = FALSE;
		AddMessageToList(TRIGGER_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", TRIGGER_NONE_DEFINED));
	}
	else
	{
		CheckTriggers();
	}
	
	SetupTriggerStrobeMessage();

	if ( iIns < 1 )
	{
		m_bInspectionGood = FALSE;
		AddMessageToList(INSPECT_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", INSPECT_NONE_DEFINED));
	}
	
	
	if ( iPpq < 1 )
	{
		m_bPPQGood = FALSE;
		AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", PPQ_NONE_DEFINED));
	}
	else
	{
		//go through each PPQ obj and verify that items are attached...
		for ( int i = 0; i < iPpq; i++ )
		{
			CSVOPPQObj *pPpq = GetPPQObject(i);
			if  ( pPpq != NULL )
			{
				if ( pPpq->GetAttachedCameraCount() < 1 )
				{
					// no camera's attached
					if ( AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPpq->GetPPQName(), PPQ_NO_CAMERA)) )
					{
						m_iPPQCameraError++;
					}
				}
				if ( pPpq->GetAttachedInspectionCount() < 1 )
				{
					//no inspection's attached
					AddMessageToList(PPQ_DLG, BuildDisplayMessage(MESSAGE_TYPE_WARNING, pPpq->GetPPQName(), PPQ_NO_INSPECT));
				}
				if ( pPpq->GetAttachedTriggerName().IsEmpty() )
				{
					//no attached trigger
					if ( AddMessageToList(PPQ_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPpq->GetPPQName(), PPQ_NO_TRIGGER)) )
					{
						m_iPPQTriggerError++;
					}
				}
				if ( pPpq->IsConditionalOutputCameraInput() && !IsCameraLineInputAllowed(pPpq->GetAttachedTriggerName()) )
				{
					// condition for output not valid
					AddMessageToList(PPQ_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPpq->GetPPQName(), PPQ_CONDITIONAL_OUTPUT_INVALID));
				}
			}
		}
	}
}

BOOL CSVOConfigAssistantDlg::SendPPQDataToConfiguration(SVPPQObjectArray& aPPQsToDelete)
{
	BOOL bRet = true;

	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PPQs - pass 1 - detach
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	long lCfgPPQCnt;
	pConfig->GetPPQCount(lCfgPPQCnt); //should be zero on new cfg...
	//loop thru to see if the PPQ's still exist.  if not delete them...
	for (int iPPQ = lCfgPPQCnt -1; -1 < iPPQ; iPPQ--)
	//    for (int iPPQ = 0; iPPQ < lCfgPPQCnt; iPPQ++)
	{
		SVPPQObject* pcPPQ = NULL;
		bRet = pConfig->GetPPQ(iPPQ,&pcPPQ) && bRet;
		if (pcPPQ != NULL)
		{
			if (!IsPPQInList(pcPPQ->GetName()))
			{ //not there.  flag to delete it...
				aPPQsToDelete.Add(pcPPQ);
				
				//PPQ is no longer there.  Detach everything from it.
				pcPPQ->DetachAll();
				
				pConfig->RemovePPQ(pcPPQ);	// remove from config now
				//delete pcPPQ;	// do this AFTER destroying triggers, cameras, and INSPECTIONS
				// destroying the PPQ will also detatch it from triggers, cameras, and inspections
			}
			else
			{
				long lSize = 0;
				long lInsCnt;
				bRet = pcPPQ->GetInspectionCount(lInsCnt);
				
				//loop thru inspections to see if they exist in dlg
				//if not, delete them from ppqobj
				for (long lI = lInsCnt-1; -1 < lI; lI--)
				{
					BOOL bDetachInspect = FALSE;
					CSVOInspectionObj *pTmpIns;
					SVInspectionProcess* pInspect = NULL;
					bRet = pcPPQ->GetInspection(lI, pInspect);
					if (pInspect != NULL)
					{
						pTmpIns = GetInspectionObjectByName(pInspect->GetName());
						if ( pTmpIns != NULL)
						{
							if ( (!IsInspectionOnPPQ(pcPPQ->GetName(), pInspect->GetName())) ||
								(pTmpIns->IsNewInspection()) )
							{
								bDetachInspect = TRUE;
							}
						}
						else
						{
							bDetachInspect = TRUE;
						}
						if ( bDetachInspect )
						{
							bRet = pcPPQ->DetachInspection(pInspect) && bRet;
						}
					}
				}
				
				//do same for camera's
				
				std::deque< SVVirtualCamera* > l_Cameras;

				pcPPQ->GetCameraList( l_Cameras );

				std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

				while( l_Iter != l_Cameras.end() )
				{
					SVVirtualCamera* pVCamera = ( *l_Iter );

					if (pVCamera != NULL)
					{
						if (!IsCameraOnPPQ(pcPPQ->GetName(), pVCamera->GetName()))
						{
							bRet = pcPPQ->DetachCamera(pVCamera, TRUE);
						}
					}

					++l_Iter;
				}

				//check trigger...
				SVTriggerObject* pTrigger = NULL;
				bRet = pcPPQ->GetTrigger(pTrigger) && bRet;
				if (pTrigger != NULL)
				{
					if (!IsTriggerOnPPQ(pcPPQ->GetName(), pTrigger->GetName()))
						//                    if (!IsTriggerInList(pTrigger->GetName()))
					{
						bRet = pcPPQ->DetachTrigger(pTrigger) && bRet;
					}
				}
				pcPPQ->RebuildOutputList();
			}
		}// if ppq != null
	} // for ppq
	return bRet;
}

BOOL CSVOConfigAssistantDlg::SendAcquisitionDataToConfiguration()
{
	BOOL bRet = true;
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	CSVOCameraObj* pCamObj;

	SVLightReference* psvLight = NULL;
	SVFileNameArrayClass* psvFiles = NULL;
	SVLut* pLut = NULL;
	SVDeviceParamCollection* pDeviceParams = NULL;

	CString sAcquName; 
	SVConfigurationObject::SVAcquisitionDeviceMap::iterator aPos = pConfig->GetAcquisitionDeviceStartPosition();
	while (aPos != pConfig->GetAcquisitionDeviceEndPosition())
	{
		//see if things are in the dlg...
		pConfig->GetAcquisitionDeviceNextAssoc( aPos, sAcquName );
		if (!IsDigitizerUsed(sAcquName))
		{
			SVAcquisitionClassPtr psvDevice;
			SVImageProcessingClass::Instance().GetAcquisitionDevice(sAcquName,psvDevice);
			pConfig->RemoveAcquisitionDevice(sAcquName);
			if( !( psvDevice.empty() ) )
			{
				psvDevice->DestroyBuffers();
				psvDevice->UnloadFiles();
				psvDevice->ResetLightReference();
				psvDevice->ResetLut();
			}
		}
	}

	SVFileNameArrayClass svFiles;
	SVLightReference svLight;
	SVLut lut;
	SVDeviceParamCollection svDeviceParams;
	SVFileNameClass svFile;
	CString sCameraFileName;
	CString sDigName;

	int iCamCnt = m_CameraList.GetCameraListCount();

	BOOL bMonoHack = FALSE;
	// if system type is RGB_MONO do this section 1 more time,
	// the extra iteration MUST use dig_Ch_ALL
	if ( m_lConfigurationType == SVIM_PRODUCT_RGB_MONO )
	{
		iCamCnt++;
		bMonoHack = TRUE;
	}

	for (int iAcq = 0; iAcq < iCamCnt; iAcq++)
	{
		svFiles.RemoveAll();
		SVAcquisitionClassPtr psvDevice;
		if ( bMonoHack )
		{
			if ( iAcq == 0 )
			{
				//do ch_all
				pCamObj = GetCameraObject(iAcq);
				sDigName = BuildDigName(pCamObj,TRUE);
			}
			else
			{
				pCamObj = GetCameraObject(iAcq-1);
				sDigName = BuildDigName(pCamObj);
			}
		}
		else
		{
			pCamObj = GetCameraObject(iAcq);
			sDigName = BuildDigName(pCamObj);
		}
		// For File Acquisition
		if (pCamObj->IsFileAcquisition())
		{
			if ( SVImageProcessingClass::Instance().GetAcquisitionDevice( sDigName, psvDevice ) == S_OK )
			{
				SVLightReference lightRef;
				SVLut lut;

				SVDeviceParamCollection deviceParams;

				_variant_t fileNameVar = pCamObj->GetImageFilename();
				_variant_t dirNameVar = pCamObj->GetImageDirectoryName();

				long loadingMode = pCamObj->GetFileLoadingMode();
				_variant_t fileLoadingModeVar = loadingMode;
				
				_variant_t widthVar = pCamObj->GetFileImageWidth();
				_variant_t heightVar = pCamObj->GetFileImageHeight();

				// Only 2 formats supported 32Bit color and 8Bit grayscale
				long format = (IsColorConfig()) ? SVImageFormatRGB8888 : SVImageFormatMono8;
				_variant_t formatVar = format;

				deviceParams.CreateParameter(DeviceParamFileAcqImageFileName, fileNameVar);
				deviceParams.CreateParameter(DeviceParamFileAcqImageDirName, dirNameVar);
				
				if (loadingMode == SingleFileMode)
				{
					// File is supposed to be managed in Single File Mode
					SVFileNameClass svFile;
					svFile.SetFullFileName(pCamObj->GetImageFilename());
					svFiles.Add(svFile);
				}
				deviceParams.CreateParameter(DeviceParamFileAcqLoadingMode, fileLoadingModeVar);
				
				deviceParams.CreateParameter(DeviceParamFileAcqImageWidth, widthVar);
				deviceParams.CreateParameter(DeviceParamFileAcqImageHeight, heightVar);
				deviceParams.CreateParameter(DeviceParamFileAcqImageFormat, formatVar);

				psvDevice->SetDeviceParameters( deviceParams );	// must be done before CreateBuffers

				// save the camera file (for future simulation mode)
				CString sCameraFileName = pCamObj->GetCameraFile();
				svFile.SetFullFileName(sCameraFileName);
			
				if ( CString( svFile.GetExtension() ).CompareNoCase(CorecoCameraFileDefExt) == 0 )
				{
					svFiles.Add(svFile);
					svFile.SetExtension(CorecoVicCameraFileDefExt);
					svFiles.Add(svFile);
				}
				else if ( CString( svFile.GetExtension() ).CompareNoCase(SV1394CameraFileDefExt) == 0 )
				{
					svFiles.Add(svFile);
				}
				else if ( CString( svFile.GetExtension() ).CompareNoCase(SVGigeCameraFileDefExt) == 0 )
				{
					svFiles.Add(svFile);
				}

				// cause the image size to be set
				psvDevice->LoadFiles(svFiles);

				// Get The Image Info
				SVImageInfoClass svImageInfo;
				psvDevice->GetImageInfo(&svImageInfo);

				// Destroy the buffers
				psvDevice->DestroyBuffers();

				// create the buffers
				HRESULT hrCreateBuffers = psvDevice->CreateBuffers( svImageInfo, TheSVObserverApp.GetSourceImageDepth() );

				// Add the acquisition device
				bRet = pConfig->AddAcquisitionDevice( sDigName, svFiles, lightRef, lut, &deviceParams ) && bRet;
			}
		}
		else
		{
			sCameraFileName = pCamObj->GetCameraFile();
			svFile.SetFullFileName(sCameraFileName);
			
			if ( CString( svFile.GetExtension() ).CompareNoCase(CorecoCameraFileDefExt) == 0 )
			{
				svFiles.Add(svFile);
				svFile.SetExtension(CorecoVicCameraFileDefExt);
				svFiles.Add(svFile);
			}
			else if ( CString( svFile.GetExtension() ).CompareNoCase(SV1394CameraFileDefExt) == 0 )
			{
				svFiles.Add(svFile);
			}
			else if ( CString( svFile.GetExtension() ).CompareNoCase(SVGigeCameraFileDefExt) == 0 )
			{
				svFiles.Add(svFile);
			}

			psvLight = NULL;
			
			if ( S_OK == SVImageProcessingClass::Instance().GetAcquisitionDevice( sDigName, psvDevice ) )
			{
				bool bGetLightReference = false;
				if ( psvDevice != NULL )
				{
					SVImageInfoClass svImageInfo;
					
					if ( pConfig->GetAcquisitionDevice( sDigName, psvFiles, psvLight, pLut, pDeviceParams ) )
					{
						psvDevice->DestroyBuffers();
						if( pCamObj->GetCameraFileChanged() )
						{
							psvDevice->UnloadFiles();

							psvDevice->LoadFiles( svFiles );
							// If the camera changed then get the LUT, Light Ref, and device pars
							//  from the camera instead of from the configuration.
							psvDevice->GetLut(*pLut);
							psvDevice->GetLightReference(*psvLight);
							psvDevice->GetDeviceParameters(*pDeviceParams);
						}
					}
					else
					{
						psvDevice->ResetLut();
						psvDevice->GetLut( lut );
						pLut = &lut;
						
						psvDevice->ResetLightReference();
						//psvDevice->GetLightReference( svLight );
						//psvLight = &svLight;
						bGetLightReference = true;
					}
					
					svDeviceParams = pCamObj->GetCameraDeviceParams();
					pDeviceParams = &svDeviceParams;
					
					if( pCamObj->GetCameraFileChanged() )
					{
						psvDevice->LoadFiles(svFiles);

						if ( psvDevice->LoadLightReference( svLight ) == S_OK )
						{
							psvDevice->SetLightReference( svLight );
						}

						if ( svDeviceParams.ParameterExists( DeviceParamCameraFormats ) )
						{
							const SVCameraFormatsDeviceParam* pParam = svDeviceParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
							if ( pParam->SupportsColor() && IsColorConfig() )
							{
								psvDevice->CreateLightReference(3, NULL, NULL);
							}
							else
							{
								psvDevice->CreateLightReference(1, NULL, NULL);
							}
						}
					}
					psvDevice->SetDeviceParameters( svDeviceParams );	// must be done before CreateBuffers for 1394 (in case CameraFormat changes) - EB 20030714
					psvDevice->GetDeviceParameters( svDeviceParams );	// LoadFiles may have created CameraFormat params which were not in the original list; retrieve the new complete list
					psvDevice->GetImageInfo(&svImageInfo);
					HRESULT hrCreateBuffers = psvDevice->CreateBuffers( svImageInfo, TheSVObserverApp.GetSourceImageDepth() );

					// EB 20031218
					// is this going to screw up anything?? we need to do this for 1394... needs to happen AFTER SetDeviceParameters on a New
					if ( bGetLightReference )
					{
						psvDevice->GetLightReference(svLight);
						psvLight = &svLight;
					}
					
					// set the trigger and strobe polarity in the I/O board based on Acq. device params
					// must get from the acq device instead of using svDeviceParams because the
					// device may set defaults in LoadFiles
					SVDeviceParamCollection params;
					psvDevice->GetDeviceParameters(params);

					// trigger
					if ( params.ParameterExists( DeviceParamAcquisitionTriggerEdge ) )
					{
						int iDigNum = psvDevice->DigNumber();
						const SVBoolValueDeviceParam* pParam = params.Parameter(DeviceParamAcquisitionTriggerEdge).DerivedValue(pParam);
						SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(iDigNum, pParam->bValue);
					}

					// strobe
					if ( params.ParameterExists( DeviceParamAcquisitionStrobeEdge ) )
					{
						int iDigNum = psvDevice->DigNumber();
						const SVBoolValueDeviceParam* pParam = params.Parameter(DeviceParamAcquisitionStrobeEdge).DerivedValue(pParam);
						SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(iDigNum, pParam->bValue);
					}
				}// end if ( psvDevice != NULL )
			}// end if ( TheSVObserverApp.mpsvImaging->GetAcquisitionDevice( sDigName, psvDevice ) )
			ASSERT ( psvLight != NULL || pLut != NULL || pDeviceParams != NULL ); // something is wrong if we get here.
				
			if (psvLight == NULL )
			{
				psvLight = &svLight;    // assign dummy var
			}
			if ( pLut == NULL )
			{
				pLut = &lut;    // assign dummy var
			}
			if ( pDeviceParams == NULL )
			{
				pDeviceParams = &svDeviceParams;
			}
			
			bRet = pConfig->AddAcquisitionDevice( sDigName, svFiles, *psvLight, *pLut, pDeviceParams ) && bRet;
		}
	}
	if ( bMonoHack )
	{
		//set camera count back to what it was before...
		iCamCnt--;
	}
	return bRet;
}

BOOL CSVOConfigAssistantDlg::SendCameraDataToConfiguration()
{
	BOOL bRet = true;
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	long lCfgCamCnt;
	SVVirtualCamera* pCamera = NULL;
	
	pConfig->GetCameraCount(lCfgCamCnt);
	
	//check to see if camera still exist...
	for (long l = lCfgCamCnt -1; -1 < l; l--)
	{
		bRet = pConfig->GetCamera(l, &pCamera) && bRet;
		if (pCamera != NULL)
		{
			if (!IsCameraInList(pCamera->GetName())) 
			{
				pConfig->RemoveCamera(pCamera);

				delete pCamera;
			}
			pCamera = NULL;
		} 
	}
	//go thru and add camera's
	int iCamCnt = m_CameraList.GetCameraListCount();
	if (iCamCnt > 0)
	{
		CString sKey;
		CString sName;
		CString sDevice;
		
		for (int i = 0; i < iCamCnt; i++)
		{
			CSVOCameraObj* pCamObj = GetCameraObject(i);
			sKey = pCamObj->GetCameraDisplayName();
			sDevice = BuildDigName(pCamObj);
			pConfig->GetCameraCount(lCfgCamCnt);
			
			for ( long l = lCfgCamCnt - 1; -1 < l; l-- )
			{
				bRet = pConfig->GetCamera(l,&pCamera) && bRet;
				
				if (pCamera != NULL)
				{
					if (sKey.Compare(pCamera->GetName() ) == 0)
					{
						pCamera->Destroy();
						break;
					}
					pCamera = NULL;
				}
			}
			
			BOOL bAddCamera = FALSE;
			
			if ( pCamera == NULL )
			{
				pCamera = new SVVirtualCamera;
				pCamera->SetName( sKey );
				bRet = pCamera != NULL && bRet;
				bAddCamera = TRUE;
			}
			
			if (pCamera != NULL)
			{
				// move from editing camera object to configuration camera object
				pCamera->SetFileAcquisitionMode(pCamObj->IsFileAcquisition());
				pCamera->SetFileLoadingMode(pCamObj->GetFileLoadingMode());
				pCamera->SetImageFilename(pCamObj->GetImageFilename());
				pCamera->SetImageDirectoryName(pCamObj->GetImageDirectoryName());
				pCamera->SetFileImageSizeEditModeFileBased(pCamObj->IsFileImageSizeEditModeFileBased());
				pCamera->SetFileImageSize(pCamObj->GetFileImageSize());

				bRet = pCamera->Create( sDevice ) && bRet;
				
				if ( bAddCamera )
				{
					bRet = pConfig->AddCamera( pCamera ) && bRet;
				}
			}
			pCamera = NULL;
		}
	}

	return bRet;
}

BOOL CSVOConfigAssistantDlg::SendTriggerDataToConfiguration()
{
	BOOL bRet = true;
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	long lCfgTrgCnt;
	pConfig->GetTriggerCount(lCfgTrgCnt);

	// Check for Triggers removed
	SVTriggerObject *pNewTrigger = NULL;
	for ( long lT = lCfgTrgCnt - 1; -1 < lT; lT-- )
	{
		bRet = pConfig->GetTrigger(lT, &pNewTrigger) && bRet;
		if ( !IsTriggerInList(pNewTrigger->GetName()) )
		{
			pConfig->RemoveTrigger(pNewTrigger);
			delete pNewTrigger;
		}
		pNewTrigger = NULL;
	}

	int iTrgCnt = m_TriggerList.GetTriggerListCount();
	if (iTrgCnt > 0)
	{
		CString sKey;
		CString sName;
		CString sDeviceName;
		
		// Check for Triggers changed
		for (int i = 0; i < iTrgCnt; i++)
		{
			CSVOTriggerObj* pTrigObj = GetTriggerObject(i);
			sKey = pTrigObj->GetTriggerDisplayName();
			sDeviceName = BuildTrgDig(pTrigObj);
			
			pNewTrigger = NULL;
			pConfig->GetTriggerCount(lCfgTrgCnt);
			
			// Remove Triggers that have changed
			for ( long l = lCfgTrgCnt - 1; -1 < l; l-- )
			{
				bRet = pConfig->GetTrigger( l,&pNewTrigger ) && bRet;
				if ( pNewTrigger != NULL )
				{
					if ( sKey.Compare(pNewTrigger->GetName() ) == 0 )
					{
						pNewTrigger->Destroy();
						break;
					}
					pNewTrigger = NULL;
				}
			}
			
			BOOL bAddTrigger = FALSE;
			
			if ( pNewTrigger == NULL )
			{
				pNewTrigger = new SVTriggerObject;
				pNewTrigger->SetName( sKey );
				
				bRet = pNewTrigger != NULL && bRet;
				bAddTrigger = TRUE;
			}
			
			if ( pNewTrigger != NULL )
			{
				if (pTrigObj->IsSoftwareTrigger())
				{
					pNewTrigger->SetSoftwareTrigger(true);
					pNewTrigger->SetSoftwareTriggerPeriod(pTrigObj->GetTimerPeriod());
				}
				else
				{
					pNewTrigger->SetSoftwareTrigger(false);
				}

				SVTriggerClass *psvDevice = SVTriggerProcessingClass::Instance().GetTrigger( sDeviceName );
				
				if ( psvDevice != NULL )
				{
					bRet = pNewTrigger->Create(psvDevice) && bRet;
				}
				
				if ( bAddTrigger )
				{
					pNewTrigger->mpsvDevice->miChannelNumber = pTrigObj->GetTriggerDigNumber();
					bRet = pConfig->AddTrigger(pNewTrigger) && bRet;
				}
			}
			pNewTrigger = NULL;
		}
	}
	return bRet;
}

BOOL CSVOConfigAssistantDlg::RenameInspectionObjects(CString sInspectionName, CString sNewInspectionName)
{
	BOOL bRet = TRUE;
	if( sInspectionName != sNewInspectionName )
	{
		SVConfigurationObject* pConfig = NULL;
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );


		// PPQ Object Inspection Name...
		long lCount = 0;
		lCount = m_PPQList.GetPPQListCount();
		for( int i = 0 ; i < lCount ; i++)
		{
			CSVOPPQObj* pPPQObject = GetPPQObject(i);
			if( pPPQObject != NULL )
			{
				int Count = pPPQObject->GetAttachedInspectionCount();
				for( int j = 0 ; j < Count ; j++ )
				{
					if( pPPQObject->GetAttachedInspection(j) == sInspectionName)
					{
						bRet &= pPPQObject->DetachInspectionFromPPQ( sInspectionName);
						bRet &= pPPQObject->AttachInspectionToPPQ( sNewInspectionName);
					}
				}
			}
		}
	}
	return bRet;
}

CSVOPPQObj* CSVOConfigAssistantDlg::GetPPQObjectByInspectionName(const SVString& inspectionName)
{
	bool bFound = false;
	CSVOPPQObj* pPPQObj = NULL;
	long ppqCount = m_PPQList.GetPPQListCount();
	for (int i = 0; !bFound && i < ppqCount; i++)
	{
		pPPQObj = GetPPQObject(i);
		if (pPPQObj != NULL)
		{
			int inspectionCount = pPPQObj->GetAttachedInspectionCount();
			for (int j = 0; !bFound && j < inspectionCount;j++)
			{
				// What is this key (label or name)?
				CString name = pPPQObj->GetAttachedInspection(j);
				if (name == inspectionName.ToString())
				{
					bFound = true;
				}
			}
		}
	}
	return pPPQObj;
}

BOOL CSVOConfigAssistantDlg::SendInspectionDataToConfiguration()
{
	BOOL bRet = true;
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	long lCfgInsCnt = 0;
	SVInspectionProcess *pNewInspect = NULL;
	
	pConfig->GetInspectionCount(lCfgInsCnt);
	
	for (long l1 = lCfgInsCnt -1; -1 < l1; l1--)
	{
		BOOL bDeleteInspect = FALSE;
		bRet = pConfig->GetInspection(l1,&pNewInspect) && bRet;
		if ( pNewInspect != NULL )
		{
			CSVOInspectionObj *pInsTmp = GetInspectionObjectByName(pNewInspect->GetName());
			
			if ( pInsTmp != NULL )
			{
				if ( (!IsInspectionInList(pNewInspect->GetName()))  || (pInsTmp->IsNewInspection()) )
				{
					bDeleteInspect = TRUE;
				}
			}
			else
			{
				bDeleteInspect = TRUE;
			}
			if ( bDeleteInspect )
			{
				pConfig->RemoveInspection(pNewInspect);
				delete pNewInspect;	// also destroys IPDoc
			}
			pNewInspect = NULL;
		}
	}

	int iInsCnt = m_InspectList.GetInspectionListCount();
	if ( iInsCnt > 0 )
	{
		CString sKey; // the label
		CString sName;
		CString sFileName; // what will be seen...
		CString sToolsetImage;
		CString sNewDisableMethod;
		long lEnableAuxiliaryExtent;

		for ( int i = 0; i < iInsCnt; i++ )
		{
			CSVOInspectionObj* pInsObj = GetInspectionObject(i);
			sKey = pInsObj->GetInspectionLabelName();
			sFileName = pInsObj->GetInspectionName();
			sToolsetImage = pInsObj->GetToolsetImage();
			sNewDisableMethod = pInsObj->GetNewDisableMethod();
			lEnableAuxiliaryExtent = pInsObj->GetEnableAuxiliaryExtent();
			
			pConfig->GetInspectionCount(lCfgInsCnt);
			
			for ( long l = lCfgInsCnt -1; -1 < l; l-- )
			{
				bRet = pConfig->GetInspection(l,&pNewInspect) && bRet;
				if (pNewInspect != NULL)
				{
					if ( sKey.Compare(pNewInspect->GetName()) == 0 )
					{
						pNewInspect->SetName(sFileName);
						
						pConfig->RenameOutputListInspectionNames( sKey, sFileName );

						TheSVObserverApp.OnUpdateAllIOViews();
						break;
					}
					pNewInspect = NULL;
				}
			}
			
			BOOL bAddInspection = FALSE;
			
			if ( pNewInspect == NULL )
			{
				const CString& importFilename = pInsObj->GetImportFilename();
				if (!importFilename.IsEmpty())
				{
					unsigned long ulVersion = TheSVObserverApp.getCurrentVersion();

					SVInspectionImportHelper importer(importFilename, sFileName, sToolsetImage, ulVersion);
					SVString title = _T( "Importing Inspection..." );
					SVImportProgress<SVInspectionImportHelper> progress(importer, title.c_str());
					progress.DoModal();

					HRESULT hr = progress.GetStatus();
					if (hr == S_OK)
					{
						if (importer.info.m_inspectionGuid != GUID_NULL)
						{
							// SEJ - add info to list
							m_ImportedInspectionInfoList.push_back(importer.info);
							SVObjectClass* pObject(NULL);
							SVObjectManagerClass::Instance().GetObjectByIdentifier(importer.info.m_inspectionGuid, pObject);
						
							bRet = pObject != NULL;

							if( bRet )
							{
								pNewInspect = reinterpret_cast<SVInspectionProcess *>(pObject);
								RenameInspectionObjects(sKey, sFileName);
								bRet = pConfig->AddInspection( pNewInspect );

								CSVOPPQObj* pPPQObj = GetPPQObjectByInspectionName(sFileName); 
								pPPQObj->SetImportedInputList(importer.info.m_inputList);
			
								BOOL bNewDisableMethod = pNewInspect->GetNewDisableMethod();
								sNewDisableMethod = (bNewDisableMethod) ? _T( "Method 2" ): _T( "Method 1" );
								lEnableAuxiliaryExtent = pNewInspect->GetEnableAuxiliaryExtent();
							}
						}
					}
					else
					{
						CString msg;
						if (hr == -15016)
						{
							msg.Format(_T("Inspection Import failed, version mismatch."));
						}
						else
						{
							msg.Format(_T("Inspection Import failed. Error = %d"), hr);
						}
						AfxMessageBox(msg);
					}
					pInsObj->ClearImportFilename();
				}
				else
				{
					SVObjectManagerClass::Instance().ConstructObject( SVInspectionProcessGuid, pNewInspect );

					bRet = pNewInspect != NULL;

					if( bRet )
					{
						pNewInspect->SetName( sFileName );
						RenameInspectionObjects(sKey, sFileName);

						pNewInspect->SetToolsetImage(sToolsetImage);

						bRet = pNewInspect->CreateInspection(sFileName);

						if( bRet )
						{
							bRet = pConfig->AddInspection( pNewInspect );
						}
					}
				}
			}
			
			if ( pNewInspect != NULL )
			{
				pNewInspect->SetToolsetImage(sToolsetImage);

				pNewInspect->SetNewDisableMethod( sNewDisableMethod == _T( "Method 2" ) );

				long l_lPrevEnable = pNewInspect->GetEnableAuxiliaryExtent();
				pNewInspect->SetEnableAuxiliaryExtent( lEnableAuxiliaryExtent );
				if( l_lPrevEnable && lEnableAuxiliaryExtent == 0 )
				{
					::SVSendMessage( pNewInspect, SVM_RESET_ALL_OBJECTS, NULL, NULL );
				}
			}
			pNewInspect = NULL;
		}
	}
	return bRet;
}

BOOL CSVOConfigAssistantDlg::SendPPQAttachmentsToConfiguration(SVPPQObjectArray& aPPQsToDelete)
{
	BOOL bRet = true;
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	SVInspectionProcess* pInspect = NULL;

	// delete old PPQs
	for (int i = 0; i < aPPQsToDelete.GetSize(); i++)
	{
		SVPPQObject* pcPPQ = aPPQsToDelete.GetAt(i);
		delete pcPPQ;
	}
	aPPQsToDelete.RemoveAll();
	
	long lCPPQCnt = 0;
	pConfig->GetPPQCount(lCPPQCnt);

	int iPPQCnt = m_PPQList.GetPPQListCount();
	if ( iPPQCnt > 0) 
	{
		CString sKey;
		SVPPQObject *pCfgPPQ = NULL;
		
		for (int i = 0; i < iPPQCnt; i++)
		{
			CSVOPPQObj* pPPQObj = GetPPQObject(i);
			sKey = pPPQObj->GetPPQName();
			BOOL bNew = FALSE;
			
			for ( long l = lCPPQCnt -1; -1 < l; l-- )
			{
				bRet = pConfig->GetPPQ(l,&pCfgPPQ) && bRet;
				
				if (pCfgPPQ != NULL)
				{
					if ( sKey.Compare(pCfgPPQ->GetName()) == 0 )
					{
						break;
					}
					
					pCfgPPQ = NULL;
				}
			}
			
			if ( pCfgPPQ == NULL )
			{
				pCfgPPQ = new SVPPQObject;
				pCfgPPQ->SetName(sKey);
				bNew = TRUE;
			}
			
			if ( pCfgPPQ != NULL )
			{
				CString sNewName;
				CString sCfgCameraName;
				CString sPPQCameraName;
				long lSize = 0;
				
				//                bRet = pCfgPPQ->Destroy() && bRet;
				
				bRet = pCfgPPQ->SetPPQOutputMode((SVPPQOutputModeEnum)pPPQObj->GetPPQMode()) && bRet;
				
				// EB 20050225
				long lOldPPQLength=0;
				pCfgPPQ->GetPPQLength( lOldPPQLength );
				bool bPPQLengthChanged = lOldPPQLength != pPPQObj->GetPPQLength();

				bRet = pCfgPPQ->SetPPQLength(pPPQObj->GetPPQLength()) && bRet;
				bRet = pCfgPPQ->SetResetDelay(pPPQObj->GetPPQOutputResetDelay()) && bRet;
				bRet = pCfgPPQ->SetOutputDelay(pPPQObj->GetPPQOutputDelayTime()) && bRet;
				bRet = pCfgPPQ->SetMaintainSourceImages( pPPQObj->GetMaintainSourceImageProperty() ) && bRet;
				bRet = pCfgPPQ->SetInspectionTimeout( pPPQObj->GetInspectionTimeout() ) && bRet;
				pCfgPPQ->SetConditionalOutputName( pPPQObj->GetConditionalOutputName() );

				int iAttachedCamCnt = pPPQObj->GetAttachedCameraCount();
				int iAttachedInsCnt = pPPQObj->GetAttachedInspectionCount();
				
				if ( iAttachedCamCnt > 0 )
				{
					for (int i = 0; i < iAttachedCamCnt; i++)
					{
						BOOL bFound = FALSE;
						long lPosition = 0;
						long l = 0;
						
						sPPQCameraName = pPPQObj->GetAttachedCamera(i);
						
						std::deque< SVVirtualCamera* > l_Cameras;

						pCfgPPQ->GetCameraList( l_Cameras );

						std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

						while( l_Iter != l_Cameras.end() )
						{
							SVVirtualCamera* pCamera = ( *l_Iter );
							
							if (pCamera != NULL)
							{
								if ( sPPQCameraName.Compare(pCamera->GetName()) == 0 )
								{
									pCfgPPQ->GetCameraPPQPosition( lPosition, pCamera );

									pCfgPPQ->DetachCamera( pCamera );

									break;
								}
								
								pCamera = NULL;
							}

							++l_Iter;
						}

						long lCfgAttachedCam = 0;

						pConfig->GetCameraCount(lCfgAttachedCam);
						
						for ( l = lCfgAttachedCam -1; -1 < l; l-- )
						{
							SVVirtualCamera* pCamera;
							bRet = pConfig->GetCamera(l, &pCamera) && bRet;
							if (pCamera != NULL)
							{
								if ( sPPQCameraName.Compare(pCamera->GetName()) == 0 )
								{
									pCfgPPQ->AttachCamera(pCamera,lPosition);
									break;
								}
								pCamera = NULL;
							}
						}
					}
				}
				
				CString sPPQTrigger;
				
				sPPQTrigger = pPPQObj->GetAttachedTriggerName();
				
				if ( !sPPQTrigger.IsEmpty() )
				{
					SVTriggerObject* pTrigger;
					pConfig->GetTriggerCount(lSize);
					for (long l = lSize -1; -1 < l; l--)
					{
						bRet = pConfig->GetTrigger(l, &pTrigger) && bRet;
						if ( pTrigger != NULL )
						{
							if ( sPPQTrigger.Compare(pTrigger->GetName()) == 0 )
							{
								pCfgPPQ->AttachTrigger(pTrigger);
								break;
							}
							
							pTrigger = NULL;
						}
					}
				}
				
				if ( iAttachedInsCnt > 0 )
				{
					CString sPpqInsStr;
					long lInsCnt;
					
					for (int i = 0; i < iAttachedInsCnt; i++)
					{
						BOOL bFound = FALSE;
						CSVOInspectionObj *pTmpIns;
						sPpqInsStr = pPPQObj->GetAttachedInspection(i);
						pTmpIns = GetInspectionObjectByName(sPpqInsStr);
						pCfgPPQ->GetInspectionCount(lInsCnt);
						
						if( pTmpIns != NULL )
						{
							for (long l = lInsCnt -1; -1 < l; l--)
							{
								bRet = pCfgPPQ->GetInspection(l,pInspect) && bRet;
								
								if ( (pInspect != NULL) && (!pTmpIns->IsNewInspection()) )
								{
									if ( sPpqInsStr.Compare(pInspect->GetName()) == 0 )
									{
										bFound = TRUE;
										break;
									}
									pInspect = NULL;
								}
							}
						}
						
						if ( !bFound )
						{
							pConfig->GetInspectionCount(lInsCnt);
							
							for ( long l = lInsCnt -1; -1 < l; l-- )
							{
								bRet = pConfig->GetInspection(l,&pInspect) && bRet;
								if (pInspect != NULL)
								{
									if ( sPpqInsStr.Compare(pInspect->GetName()) == 0 )
									{
										pCfgPPQ->AttachInspection(pInspect);

										break;
									}
									pInspect = NULL;
								}
							}
						}
					}// end for (int i = 0; i < iAttachedInsCnt; i++)
				}// end if ( iAttachedInsCnt > 0 )

				const SVImportedInputList& list = pPPQObj->GetImportedInputList();
				for (SVImportedInputList::const_iterator it = list.begin();it != list.end();++it)
				{
					if (it->type() == typeid(SVImportedRemoteInput))
					{
						const SVImportedRemoteInput& input = boost::any_cast<const SVImportedRemoteInput &>(*it);
						pConfig->AddImportedRemoteInput(pCfgPPQ, input.name, input.ppqPosition, input.index, input.value);
					}
					else
					{
						const SVImportedInput& input = boost::any_cast<const SVImportedInput &>(*it);
						pConfig->AddImportedDigitalInput(pCfgPPQ, input.name, input.ppqPosition);
					}
				}
				// cleanup so we don't do it again
				pPPQObj->ClearImportedInputList();

				if (bNew)
				{
					bRet = pCfgPPQ->Create() && bRet;
					pConfig->GetInputObjectList( &pCfgPPQ->m_pInputList );
					pConfig->GetOutputObjectList( &pCfgPPQ->m_pOutputList );
					bRet = pConfig->AddPPQ(pCfgPPQ) && bRet;
				}
				else
				{
					bRet = pCfgPPQ->Rebuild() && bRet;
				}
				pCfgPPQ = NULL;
			}
		}
	}
	return bRet;
}

//////////////////////////////////////////////////////////////
//   area for writing object list back to config obj

BOOL CSVOConfigAssistantDlg::SendDataToConfiguration()
{
	BOOL bRet = TRUE;

	SVInputObjectList	*pInputObjectList;
	SVOutputObjectList	*pOutputObjectList;
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );


	long l( 0 );
	
	SVPPQObjectArray aPPQsToDelete;

	pConfig->SetProductType( m_lConfigurationType );

/*
	Algorithm Nov 11, 2002

		Remove from Config all PPQs that have been removed, remember those pointers so can delete object later
		Detach remaining PPQ connections to other objects that have been removed

		Remove Acquisition Devices
		Create Acquisition Devices

		Remove Cameras
		Create Cameras

		Remove Triggers
		Create Triggers

		Remove Inspections
		Create Inspections

		Delete old PPQs
		Create PPQs
		Attach PPQs to new objects
*/

	pConfig->GetInputObjectList(&pInputObjectList);
	pConfig->GetOutputObjectList(&pOutputObjectList);

	if( !pInputObjectList )
	{
		pInputObjectList = new SVInputObjectList;
		pInputObjectList->SetName( _T( "Input Object List" ) );
		bRet &= pInputObjectList != NULL && pInputObjectList->Create() && bRet;
	}
	if( !pOutputObjectList )
	{
		pOutputObjectList = new SVOutputObjectList;
		pOutputObjectList->SetName( _T( "Output Object List" ) );
		bRet &= pOutputObjectList != NULL && pOutputObjectList->Create() && bRet;
	}

	pConfig->SetInputObjectList(pInputObjectList);
	pConfig->SetOutputObjectList(pOutputObjectList);

	bRet = SendPPQDataToConfiguration(aPPQsToDelete);

	// EB 2002 02 28
	// Added this to fix memory leak freeing MIL buffers
	// before we close Acq devices, we need to tell all toolsets to Close
	// this closes the connection between toolset images and the MIL acq image.
	TheSVObserverApp.DisconnectToolsetBuffers();

	SendAcquisitionDataToConfiguration();

	bRet = SendCameraDataToConfiguration();

	// EB 2002 02 28
	// Added this to fix memory leak freeing MIL buffers
	// before we close Acq devices, we need to tell all toolsets to Close
	// reopen the connection between toolset images and the MIL acq image here.
	ConnectToolsetBuffers();

	bRet = SendTriggerDataToConfiguration();
	bRet = SendInspectionDataToConfiguration();
		
	bRet = SendPPQAttachmentsToConfiguration(aPPQsToDelete);

	// EB 20031203
	// a temp solution
	// the better solution is to have the acqs subscribe and the triggers provide
	HRESULT hrAttach = pConfig->AttachAcqToTriggers();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	if (m_bNewConfiguration)
//	{
		// go through each inspection, if HasInspectionNameChanged
		// send messages to all objects...
		long lInsCfgCnt;
		SVInspectionProcess *pcfgInsPro;
		CSVOInspectionObj *pTmpObj;
		pConfig->GetInspectionCount(lInsCfgCnt);
		
		for (long lI = lInsCfgCnt -1; -1 < lI; lI--)
		{
			pConfig->GetInspection(lI,&pcfgInsPro);
			pTmpObj = GetInspectionObjectByName(pcfgInsPro->GetName());
			
			if (pTmpObj)
			{
				if (pTmpObj->HasInspectionNameChange())
				{
					::SVSendMessage( pcfgInsPro,SVM_OBJECT_RENAMED,
						reinterpret_cast<LONG_PTR>( static_cast<SVObjectClass*>(pcfgInsPro) ), 
						reinterpret_cast<LONG_PTR>( static_cast<LPCTSTR>(pTmpObj->GetOrginalInspectionName())) );
					SVPPQObject *pPPQ;
					SVOutputObjectList *pOutputObjList;
					long lCount;
					long lPPQcount;
					
					pConfig->GetOutputObjectList(&pOutputObjList);
					
					if (pOutputObjList)
					{
						::SVSendMessage( pOutputObjList, SVM_OBJECT_RENAMED,
							reinterpret_cast<LONG_PTR>(static_cast <SVObjectClass*> (pcfgInsPro)),
							reinterpret_cast<LONG_PTR>(static_cast<LPCTSTR>( pTmpObj->GetOrginalInspectionName())) );
					}
					
					pConfig->GetPPQCount(lCount);
					for (lPPQcount = 0; lPPQcount < lCount; lPPQcount++)
					{
						pConfig->GetPPQ(lPPQcount,&pPPQ);
						if (pPPQ)
						{
							::SVSendMessage( pPPQ, SVM_OBJECT_RENAMED,
								reinterpret_cast<LONG_PTR>(static_cast<SVObjectClass*>(pcfgInsPro)),
								reinterpret_cast<LONG_PTR>(static_cast<LPCTSTR>( pTmpObj->GetOrginalInspectionName())) );
						} //if (pPPQ)
					} //for ppqcount
				} // if name has changed
			}//if pTmpObj
		}//for each inspection
//	}// if it was a new configuration...

	if ( m_bNewConfiguration )
	{
		pConfig->SetConfigurationLoaded();
	}
	return bRet;
}

BOOL CSVOConfigAssistantDlg::GetConfigurationForExisting()
{
	BOOL bRet = TRUE;
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	
	//make sure list are clean...
	m_CameraList.ResetContent();
	m_TriggerList.ResetContent();
	m_InspectList.ResetContent();
	m_PPQList.ResetContent();
	m_slUsedTriggers.RemoveAll();
	m_slUsedInspect.RemoveAll();
	m_slInspectionNamesUsed.RemoveAll();
	m_slInspectionLabelsUsed.RemoveAll();
	
	long lCfgCameraCnt;
	long lCfgTriggerCnt;
	long lCfgInspectCnt;
	long lCfgPPQCnt;
	CString sDigName;
	CString sCameraName;
	CString sTriggerName;
	CString sInspectLabel;
	CString sInspectName;
	CString sPPQName;
	int iDigNumber;
	int iChannel;
	CString sCCAFileName;
	
	SVVirtualCamera *pcfgCamera;
	SVTriggerObject *pcfgTrigger;
	SVInspectionProcess *pcfgInspect;
	SVPPQObject *pcfgPPQ;
	
	CSVOPPQObj *pPpqObj;
	
	m_lConfigurationType = pConfig->GetProductType();
	//load camera information
	pConfig->GetCameraCount(lCfgCameraCnt);
	for (long lCam = lCfgCameraCnt -1; -1 < lCam; lCam--)
	{
		bRet = pConfig->GetCamera(lCam,&pcfgCamera) && bRet;
		if (pcfgCamera != NULL)
		{
			//            pCamObj = new CSVOCameraObj();
			sCameraName = pcfgCamera->GetName();
			if ( pcfgCamera->mpsvDevice != NULL )
			{
				sDigName = pcfgCamera->mpsvDevice->DigName();
				iDigNumber = pcfgCamera->mpsvDevice->DigNumber();
				iChannel = pcfgCamera->mpsvDevice->Channel();
				
				sCCAFileName.Empty();
				//go through mFiles and pull out cca file for now
				long lSize;
				SVFileNameClass oCamFile;
				pcfgCamera->mpsvDevice->GetFileNameArraySize( lSize );
				for ( long lFile = 0; lFile < lSize && sCCAFileName.IsEmpty();lFile++)
				{
					pcfgCamera->mpsvDevice->GetFileName( lFile, oCamFile );
					CString csExt = oCamFile.GetExtension();				
					
					if ( ( csExt.CompareNoCase( CorecoCameraFileDefExt ) == 0 || 
							csExt.CompareNoCase( SV1394CameraFileDefExt ) == 0 || 
							csExt.CompareNoCase( SVGigeCameraFileDefExt ) == 0 )
						&& sCCAFileName.IsEmpty() )
					{
						sCCAFileName = oCamFile.GetFullFileName();
					}
				}

				//Add Camera Name, Camera Files and Camera Device Params to tmp list in case of a cancel
				m_CameraList.AddCameraToList(sCameraName,iDigNumber,iChannel); 
				m_TmpCameraList.AddCameraToList(sCameraName,iDigNumber,iChannel); 

				CSVOCameraObj* pCamObj = m_CameraList.GetCameraObjectByName(sCameraName);

				m_CameraList.SetCameraFile(sCameraName,sCCAFileName);
				m_TmpCameraList.SetCameraFile(sCameraName,sCCAFileName);

				SVDeviceParamCollection l_Params;
				SVDeviceParamCollection l_CameraFileParams;
				pcfgCamera->mpsvDevice->GetDeviceParameters( l_Params );
				pcfgCamera->mpsvDevice->GetCameraFileParameters( l_CameraFileParams );

				m_CameraList.SetCameraDeviceParams( sCameraName, l_Params, l_CameraFileParams );
				m_TmpCameraList.SetCameraDeviceParams( sCameraName, l_Params, l_CameraFileParams );

				// move from configuration (SVVirtualCamera) camera object to editing (CSVOCameraObj) camera object
				pCamObj->SetFileAcquisitionMode(pcfgCamera->IsFileAcquisition());
				pCamObj->SetFileLoadingMode(pcfgCamera->GetFileLoadingMode());
				pCamObj->SetImageFilename(pcfgCamera->GetImageFilename());
				pCamObj->SetImageDirectoryName(pcfgCamera->GetImageDirectoryName());
				pCamObj->SetFileImageSizeEditModeFileBased(pcfgCamera->IsFileImageSizeEditModeFileBased());
				pCamObj->SetFileImageSize(pcfgCamera->GetFileImageSize());
			}
		}
	}
	//end of load camera section
	
	//load trigger section...
	pConfig->GetTriggerCount(lCfgTriggerCnt);
	for (long lTrg = lCfgTriggerCnt -1; -1 < lTrg; lTrg--)
	{
		bRet = pConfig->GetTrigger(lTrg,&pcfgTrigger) && bRet;
		if ( pcfgTrigger != NULL && pcfgTrigger->mpsvDevice != NULL )
		{
			sTriggerName = pcfgTrigger->GetName();
			iDigNumber = pcfgTrigger->mpsvDevice->miChannelNumber;
			if (iDigNumber < 0)
			{
				// use the numeric at the end of the name (it's one based and we want zero based)
				// find the last underscore
				int pos = sTriggerName.ReverseFind(_T('_'));
				if (pos != -1)
				{
					CString name = sTriggerName.Right(sTriggerName.GetLength() - (pos + 1));
					iDigNumber  = _ttoi(name);

					// make it zero based
					if (iDigNumber > 0)
						iDigNumber--;
				}
			}
			m_TriggerList.AddTriggerToList(SVString(sTriggerName), iDigNumber);

			// SEJ - add Software trigger flag and interval here
			CSVOTriggerObj* pTrgObj = m_TriggerList.GetTriggerObjectByName(SVString(sTriggerName));
			pTrgObj->SetSoftwareTrigger(pcfgTrigger->IsSoftwareTrigger());
			pTrgObj->SetTimerPeriod(pcfgTrigger->GetSoftwareTriggerPeriod());
		}
	}
	//end of trigger section
	
	//load inspection section...
	CString sDisable;
	pConfig->GetInspectionCount(lCfgInspectCnt);
	for (long lIns = lCfgInspectCnt -1; -1 < lIns; lIns--)
	{
		bRet = pConfig->GetInspection(lIns,&pcfgInspect) && bRet;
		if (pcfgInspect != NULL)
		{
			sInspectLabel = pcfgInspect->GetName();  // label name
			sInspectName = pcfgInspect->GetName();
			
			m_InspectList.AddInspectionToList(sInspectLabel,sInspectName);
			m_slInspectionNamesUsed.AddTail(sInspectName);
			m_slInspectionLabelsUsed.AddTail(sInspectLabel);
			
			// Determine which image is the main image
			CString sToolsetImage;
			sToolsetImage = pcfgInspect->GetToolsetImage();
			m_InspectList.SetToolsetImage( sInspectLabel, sToolsetImage );

			if( pcfgInspect->GetNewDisableMethod() )
			{
				sDisable = _T( "Method 2" );
			}// end if
			else
			{
				sDisable = _T( "Method 1" );
			}// end else

			m_InspectList.SetNewDisableMethod( sInspectLabel, sDisable );

			m_InspectList.SetShowAuxExtent( sInspectLabel, pcfgInspect->GetGageToolCount() == 0 );

			// Enable Auxiliary Extent
			long l_lEnable = 0;
			if( pcfgInspect->GetEnableAuxiliaryExtent() )
			{
				l_lEnable = 1;
			}
			else
			{
				l_lEnable = 0;
			}
			m_InspectList.SetEnableAuxiliaryExtent( sInspectLabel, l_lEnable );
		}
	}
	//end of inspection section...
	
	//load PPQ section...
	pConfig->GetPPQCount(lCfgPPQCnt);
	
	for (long lPpq = lCfgPPQCnt -1; -1 < lPpq; lPpq--)
	{
		bRet = pConfig->GetPPQ(lPpq,&pcfgPPQ) && bRet;
		if (pcfgPPQ != NULL)
		{
			sPPQName = pcfgPPQ->GetName();
			m_PPQList.AddPPQToList(sPPQName);
			long lppqIns;
			pcfgPPQ->GetInspectionCount(lppqIns);
			
			std::deque< SVVirtualCamera* > l_Cameras;

			pcfgPPQ->GetCameraList( l_Cameras );

			std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

			while( l_Iter != l_Cameras.end() )
			{
				pcfgCamera = ( *l_Iter );

				if ( (pcfgCamera != NULL) && (pcfgCamera->mpsvDevice != NULL) )
				{
					m_PPQList.AttachCameraToPPQ(sPPQName,pcfgCamera->GetName());
				}

				++l_Iter;
			}
			
			for (long lpIns = lppqIns -1; -1 < lpIns; lpIns--)
			{
				pcfgInspect = NULL;
				bRet = pcfgPPQ->GetInspection(lpIns,pcfgInspect) && bRet;
				if (pcfgInspect != NULL)
				{
					m_PPQList.AttachInspectToPPQ(sPPQName,pcfgInspect->GetName());
					AddUsedInspect(pcfgInspect->GetName());
				}
			}
			
			pcfgTrigger = NULL;
			bRet = pcfgPPQ->GetTrigger(pcfgTrigger) && bRet;
			if ( (pcfgTrigger != NULL) && (pcfgTrigger->mpsvDevice != NULL) )
			{
				m_PPQList.AttachTriggerToPPQ(sPPQName,pcfgTrigger->GetName());
				AddUsedTrigger(pcfgTrigger->GetName());
			}
			//set properties for PPQ
			
			pPpqObj = GetPPQObjectByName(sPPQName);
			long lPPQLength;
			long lPPQResetDelay;
			long lPPQDelayTime;
			bool bPPQMaintainSrcImg;
			long lInspectionTimeout;
			SVPPQOutputModeEnum ePPQMode;
			pcfgPPQ->GetPPQLength(lPPQLength);
			pcfgPPQ->GetPPQOutputMode(ePPQMode);
			pcfgPPQ->GetOutputDelay(lPPQDelayTime);
			pcfgPPQ->GetResetDelay(lPPQResetDelay);
			pcfgPPQ->GetMaintainSourceImages(bPPQMaintainSrcImg);
			pcfgPPQ->GetInspectionTimeout(lInspectionTimeout);
			
			pPpqObj->SetPPQLength((int)lPPQLength);
			pPpqObj->SetPPQMode( (int)ePPQMode);
			pPpqObj->SetPPQOutputDelayTime((int)lPPQDelayTime);
			pPpqObj->SetPPQOutputResetDelay((int)lPPQResetDelay);
			pPpqObj->SetMaintainSourceImageProperty(bPPQMaintainSrcImg);
			pPpqObj->SetInspectionTimeout( lInspectionTimeout );
			pPpqObj->SetConditionalOutputName(pcfgPPQ->GetConditionalOutputName());
			
			// Get List Of Inputs
			SVIOEntryHostStructPtrList ppIOEntries; 
			BOOL bRet = pcfgPPQ->GetAvailableInputs( ppIOEntries );
			if (bRet)
			{
				SVNameGuidPairList availableInputs;
				for (SVIOEntryHostStructPtrList::const_iterator it = ppIOEntries.begin();it != ppIOEntries.end();++it)
				{
					SVIOEntryHostStructPtr entry = (*it);
					if (entry->m_ObjectType == IO_DIGITAL_INPUT)
					{
						if (!IsNonIOSVIM(GetProductType()))
						{
							availableInputs.push_back(std::make_pair(entry->m_pValueObject->GetName(), entry->m_IOId));
						}
					}
					else if (entry->m_ObjectType == IO_CAMERA_DATA_INPUT)
					{
						// check for Camera Input Line State...
						if (entry->m_pValueObject)
						{
							if (entry->m_pValueObject->GetEmbeddedID() == SVCameraTriggerLineInStateGuid)
							{
								// SEJ 303 - only if the camera supports it ?
								availableInputs.push_back(std::make_pair(entry->m_pValueObject->GetName(), entry->m_IOId));
							}
						}
					}
				}
				// make list of Name/Guid pairs
				pPpqObj->SetAvailableInputsForConditionalOutput(availableInputs);
			}
		}//end if pcfgPPQ != NULL
	}
	//end of load PPQ section...
	m_Page1.SetupList();
	m_Page2.SetupList();
	m_Page3.SetupList();
	m_Page4.SetupList();
	
	m_bCameraGood = TRUE;
	m_bTriggerGood = TRUE;
	m_bInspectionGood = TRUE;
	m_bPPQGood = TRUE;
	return bRet;
}


BOOL CSVOConfigAssistantDlg::ItemChanged(int iItemDlg, CString sLabelName, int iAction)
{
	BOOL bRet = FALSE;
	CSVOCameraObj *pCam;
	CSVOInspectionObj *pIns;
	CSVOPPQObj *pPpq;
	int iObjCnt = 0;
	
	CString sMessage;
	
	
	switch (iItemDlg)
	{
		case CAMERA_DLG:
		{
			iObjCnt = GetCameraListCount();
			switch (iAction)
			{
				
				case ITEM_ACTION_NEW:
				{
					if (iObjCnt == 1)
					{
						m_bCameraGood = TRUE;
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", CAMERA_NONE_DEFINED);
						RemoveMessageFromList(sMessage);
						//delete message
					}
					pCam = GetCameraObjectByName(sLabelName);
					if (!pCam->IsCameraObjOk())
					{   // check to see if camera files are valid.
						// if not, add message to list.
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCam->GetCameraDisplayName(), CAMERA_INVALID_FILES);
						if ( AddMessageToList(CAMERA_DLG,sMessage) )
						{
							m_iCameraErrors++;
						}
					}
					break;
				}// end case ITEM_ACTION_NEW:

				case ITEM_ACTION_DELETE:
				{
					if (m_iInvalidCamera > 0)
					{
						if ( RemoveMessageFromList(BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, INVALID_CAMERA)) )
						{
							m_iInvalidCamera--;
							//break;
						}
					}
					
					int iNum = RemoveAllObjectMessages(sLabelName);
					if (m_iCameraErrors >0)
					{
						m_iCameraErrors = m_iCameraErrors - iNum;
					}
					if (iObjCnt < 1) 
					{
						m_bCameraGood = FALSE;
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", CAMERA_NONE_DEFINED);
						// they removed the last one.  place message
						AddMessageToList(CAMERA_DLG,sMessage);
					}
					CameraDeletedCheckAgainstPPQ(sLabelName);
					CameraDeletedCheckAgainstInspection( sLabelName );
					break;
				}// end case ITEM_ACTION_DELETE:

				case ITEM_ACTION_PROP:
				{
					pCam = GetCameraObjectByName(sLabelName);
					
					RemoveFileAcquisitionMessages(pCam);

					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCam->GetCameraDisplayName(), CAMERA_INVALID_FILES);

					if (pCam->IsCameraObjOk())
					{
						if ( RemoveMessageFromList(sMessage) )
						{
							m_iCameraErrors--;
						}

						// File Acquisition
						if (pCam->IsFileAcquisition())
						{
							CString sDigName = BuildDigName( pCam, true );
							SVAcquisitionClassPtr pDevice;
							SVImageProcessingClass::Instance().GetAcquisitionDevice( sDigName, pDevice );
							
							CheckCamera(pCam);
							CheckTriggers();
						}
						else
						{
							if ( SVHardwareManifest::IsDigitalSVIM( m_lConfigurationType ) )
							{
								// check for non-supported cam file so we can pop up a message
								CString sDigName = BuildDigName( pCam, true );
								SVAcquisitionClassPtr pDevice;
								SVImageProcessingClass::Instance().GetAcquisitionDevice( sDigName, pDevice );
								if ( !( pDevice.empty() ) )
								{
									SVDeviceParamCollection params;
									HRESULT hr = pDevice->ReadCameraFile( pCam->GetCameraFile(), params );
									if ( hr == SVMSG_INCORRECT_CHECKSUM )
									{
										// just display a warning
										//AfxMessageBox(_T("Warning! The camera file you have selected is not an SVResearch supported file."), MB_OK | MB_ICONEXCLAMATION );
										// add warning to box
									}
									// set in all cases
									pCam->SetCameraFileParams( params );

									CheckCamera(pCam);
									CheckTriggers();
								}// end if ( pDevice )
							}// end if ( m_eBoard == SVIM_BOARD_1394 )
						}
					}
					else
					{
						if ( AddMessageToList(CAMERA_DLG, sMessage) )
						{
							m_iCameraErrors++;
						}
					}
					break;
				}// end case ITEM_ACTION_PROP:

				case ITEM_ACTION_REFRESH:
				{
					int iCam = GetCameraListCount();
					CSVOCameraObj* pCam=NULL;
					
					for (int iC = 0; iC < iCam; iC++)
					{
						pCam = GetCameraObject(iC);
						CheckCamera(pCam);
					}
					break;
				}

				default:
				{
					break;
				}
			}// end switch (iAction)
			
			break;
		}// end case CAMERA_DLG:
		
		case TRIGGER_DLG:
		{
			iObjCnt = GetTriggerListCount();
			switch (iAction)
			{
				case ITEM_ACTION_NEW:
				{
					if (iObjCnt == 1)
					{
						m_bTriggerGood = TRUE;
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", TRIGGER_NONE_DEFINED);
						RemoveMessageFromList(sMessage);
					}
					break;
				}// end case ITEM_ACTION_NEW:

				case ITEM_ACTION_DELETE:
				{
					if ( m_iInvalidTrigger > 0 )
					{
						if ( RemoveMessageFromList(BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, INVALID_TRIGGER)) )
						{
							m_iInvalidTrigger--;
							break;
						}
					}
					if (iObjCnt < 1)
					{
						m_bTriggerGood = FALSE;
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", TRIGGER_NONE_DEFINED);
						AddMessageToList(TRIGGER_DLG,sMessage);
					}
					TriggerDeletedCheckAgainstPPQ(sLabelName);
					break;
				}// end case ITEM_ACTION_DELETE:

				case ITEM_ACTION_PROP:
				{
					CheckTriggers();
					break;
				}

				default:
				{
					break;
				}
			}// end switch (iAction)

			SetupTriggerStrobeMessage();

			break;
		}// end case TRIGGER_DLG:
		
		case INSPECT_DLG:
		{
			iObjCnt = GetInspectionListCount();
			switch (iAction)
			{
				case ITEM_ACTION_NEW:
				{
					pIns = GetInspectionObjectByName(sLabelName);
					pIns->SetNewInspection();
					if (iObjCnt == 1)
					{
						m_bInspectionGood = TRUE;
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", INSPECT_NONE_DEFINED);
						RemoveMessageFromList(sMessage);
					}
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, INSPECTION_ERROR);
					if ( AddMessageToList(PPQ_DLG,sMessage) )
					{
						m_iInspectErrors++;
					}
					break;
				}// end case ITEM_ACTION_NEW:

				case ITEM_ACTION_DELETE:
				{
					m_bInspectionDeleted = TRUE;
					if (iObjCnt < 1)
					{
						m_bInspectionGood = FALSE;
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", INSPECT_NONE_DEFINED);
						AddMessageToList(INSPECT_DLG,sMessage);
					}
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, INSPECTION_ERROR);
					if ( RemoveMessageFromList(sMessage) )
					{
						m_iInspectErrors--;
					}
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, TOOLSET_IMAGE_ERROR);
					if ( RemoveMessageFromList(sMessage) )
					{
						m_iInsToolsetError--;
					}
					InspectionDeletedCheckAgainstPPQ(sLabelName);
					break;
				}// end case ITEM_ACTION_DELETE:

				case ITEM_ACTION_PROP:
				{
					pIns = GetInspectionObjectByName(sLabelName);
					CString sInspect = pIns->GetInspectionName();
					if ( !pIns->GetToolsetImage().IsEmpty() )
					{
						if ( RemoveMessageFromList(BuildDisplayMessage(MESSAGE_TYPE_ERROR, sInspect, TOOLSET_IMAGE_ERROR)) )
						{
							m_iInsToolsetError--;
						}
					}
					break;
				}// end case ITEM_ACTION_PROP:

				default:
				{
					break;
				}
			}// end switch (iAction)
			break;
		}// end case INSPECT_DLG:
		
		case PPQ_DLG:
		{
			iObjCnt = GetPPQListCount();
			switch (iAction)
			{
				case ITEM_ACTION_NEW:
				{
					if (iObjCnt == 1)
					{
						m_bPPQGood = TRUE;
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", PPQ_NONE_DEFINED);
						RemoveMessageFromList(sMessage);
					}
					if ( AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, PPQ_NO_TRIGGER)) )
					{
						m_iPPQTriggerError++;
					}
					if ( AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, PPQ_NO_CAMERA)) )
					{
						m_iPPQCameraError++;
					}
					AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_WARNING, sLabelName, PPQ_NO_INSPECT));
					break;
				}
				case ITEM_ACTION_DELETE:
				{
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, PPQ_NO_TRIGGER);
					if( RemoveMessageFromList(sMessage) )
					{
						m_iPPQTriggerError--;
					}
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, PPQ_NO_CAMERA);
					if ( RemoveMessageFromList(sMessage) )
					{
						m_iPPQCameraError--;
					}
					RemoveAllObjectMessages(sLabelName);
					if (iObjCnt < 1)
					{
						m_bPPQGood = FALSE;
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", PPQ_NONE_DEFINED);
						AddMessageToList(PPQ_DLG,sMessage);
					}
					break;
				}
				case ITEM_PPQ_ADD_TRG:
				{
					pPpq = GetPPQObjectByName(sLabelName);
					if (pPpq)
					{
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPpq->GetPPQName(), PPQ_NO_TRIGGER);
						RemoveMessageFromList(sMessage);
						m_iPPQTriggerError--;
					}
					break;
				}
				case ITEM_PPQ_DEL_TRG:
				{
					pPpq = GetPPQObjectByName(sLabelName);
					if (pPpq)
					{
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPpq->GetPPQName(), PPQ_NO_TRIGGER);
						if ( AddMessageToList(PPQ_DLG,sMessage) )
						{
							m_iPPQTriggerError++;
						}
					}
					break;
				}
				case ITEM_PPQ_ADD_CAM:
				{
					pPpq = GetPPQObjectByName(sLabelName);
					if (pPpq)
					{
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPpq->GetPPQName(), PPQ_NO_CAMERA);
						if ( RemoveMessageFromList(sMessage) )
						{
							m_iPPQCameraError--;
						}
					}
					break;
				}
				case ITEM_PPQ_DEL_CAM:
				{
					pPpq = GetPPQObjectByName(sLabelName);
					if (pPpq)
					{
						if ( pPpq->GetAttachedCameraCount() < 1 )
						{
							sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPpq->GetPPQName(), PPQ_NO_CAMERA);
							if ( AddMessageToList(PPQ_DLG,sMessage) )
							{
								m_iPPQCameraError++;
							}
						}
						CameraDetachedCheckAgainstInspection( sLabelName );
					}
					break;
				}
				case ITEM_PPQ_ADD_INS:
				{
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, INSPECTION_ERROR);
					if ( RemoveMessageFromList(sMessage) )
					{
						m_iInspectErrors--;
					}
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, sLabelName, PPQ_NO_INSPECT);
					RemoveMessageFromList(sMessage);
					
					if ( AddMessageToList(INSPECT_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, TOOLSET_IMAGE_ERROR)) )
					{
						m_iInsToolsetError++;
					}
					
					
					break;
				}
				case ITEM_PPQ_DEL_INS:
				{
					CString sInsLabelName = GetInspectionLabelFromName(sLabelName);
					pIns = GetInspectionObjectByName(sInsLabelName);
					pIns->SetToolsetImage(_T(""));
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, INSPECTION_ERROR);
					if ( AddMessageToList(PPQ_DLG,sMessage) )
					{
						m_iInspectErrors++;
					}
					RemoveUsedInspect(sLabelName);
					break;
				}
				case ITEM_PPQ_DETACH_INS:
				{
					AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_WARNING, sLabelName, PPQ_NO_INSPECT));
					break;
				}
				case ITEM_PPQ_ATTACH_INS:
				{
					RemoveMessageFromList(BuildDisplayMessage(MESSAGE_TYPE_WARNING, sLabelName, PPQ_NO_INSPECT) );
					break;
				}
				case ITEM_ACTION_PROP:
				{
					// check to see if property is valid...
					//if good remove message from list (if it is there)
					//if bad add message to list, if not already there
					//set errors
					pPpq = GetPPQObjectByName(sLabelName);
					if ( pPpq )
					{
						long l_lPpqLength = pPpq->GetPPQLength();
						long l_lImageDepth = TheSVObserverApp.GetMaxPPQLength(); // GetSourceImageDepth() - 2;
						if ( pPpq->GetMaintainSourceImageProperty() )
						{
							if ( l_lPpqLength <= l_lImageDepth )
							{
								// remove error message if exist
								CString l_strError;
								l_strError.Format("%s%d",PPQ_PROP_SRC_IMG_ERROR,l_lImageDepth);
								//create error display message
								if ( RemoveMessageFromList(BuildDisplayMessage( MESSAGE_TYPE_ERROR, sLabelName, l_strError ) ) )
								{
									m_iPPQMaintainSourceImageError--;
								}
							}
							else
							{
								// add error message if it does not exist
								CString l_strError;
								l_strError.Format("%s%d",PPQ_PROP_SRC_IMG_ERROR,l_lImageDepth);
								//create error display message
								if ( AddMessageToList( PPQ_DLG, BuildDisplayMessage( MESSAGE_TYPE_ERROR, sLabelName, l_strError ) ) )
								{
									m_iPPQMaintainSourceImageError++;
								}
								
							}
						}
					}
					break;
				}  
				case ITEM_PPQ_PROP_SRC_IMG:
				{
					//special property case for the PPQ's
					pPpq = GetPPQObjectByName(sLabelName);
					if ( pPpq )
					{
						if ( pPpq->GetMaintainSourceImageProperty() )
						{
							long l_lPpqLength = pPpq->GetPPQLength();
							long l_lImageDepth = TheSVObserverApp.GetMaxPPQLength(); //GetSourceImageDepth() - 2;
							if ( l_lPpqLength > l_lImageDepth )
							{
								CString l_strError;
								l_strError.Format("%s%d",PPQ_PROP_SRC_IMG_ERROR,l_lImageDepth);
								//create error display message
								if ( AddMessageToList( PPQ_DLG, BuildDisplayMessage( MESSAGE_TYPE_ERROR, sLabelName, l_strError ) ) )
								{
									m_iPPQMaintainSourceImageError++;
								}
							}
						}
						else
						{
							// see if error conditions exist, if so remove it
							// maintain source image = false
							CString l_strError;
							long l_lImageDepth = TheSVObserverApp.GetMaxPPQLength(); //GetSourceImageDepth() - 2;
							l_strError.Format("%s%d",PPQ_PROP_SRC_IMG_ERROR,l_lImageDepth);
							//create error display message
							if ( RemoveMessageFromList(BuildDisplayMessage( MESSAGE_TYPE_ERROR, sLabelName, l_strError ) ) )
							{
								m_iPPQMaintainSourceImageError--;
							}
						}
					}// end if ( pPpq )
					break;
				}// end case ITEM_PPQ_PROP_SRC_IMG:
				
				default:
				{
					break;
				}
			}// end switch (iAction)
			break;
		}// end case PPQ_DLG:
		default:
		{
			break;
		}
	}// end switch (iItemDlg)
	IsGood();
	return bRet;
}

CString CSVOConfigAssistantDlg::BuildDisplayMessage(ConfigMsgTypeEnum iErrorFlag, CString sObjectName, CString sMessage)
{
	CString sRetMessage;
	switch (iErrorFlag)
	{
		case MESSAGE_TYPE_ERROR:
		{
			sRetMessage = ERR_STR + sObjectName + ' ' + sMessage;
			break;
		}
		case MESSAGE_TYPE_WARNING:
		{
			sRetMessage = WARNING_STR + sObjectName + ' ' + sMessage;
			break;
		}
		default:
		{
			break;
		}
	}
	
	return sRetMessage;
}

void CSVOConfigAssistantDlg::TriggerDeletedCheckAgainstPPQ(CString sTriggerName)
{
	//see if Trigger is connected to any PPQ.  if so remove it.  and add message to list
	int iPPQCnt = GetPPQListCount();
	CSVOPPQObj *pObj;
	
	for (int i = 0; i < iPPQCnt; i++)
	{
		pObj = GetPPQObject(i);
		
		if (pObj->GetAttachedTriggerName().Compare(sTriggerName) == 0)
		{
			pObj->DetachTriggerFromPPQ();
			pObj->RemoveCameraInputConditionalOutput();
			CString sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pObj->GetPPQName(), PPQ_NO_TRIGGER);
			if ( AddMessageToList(PPQ_DLG,sMessage) )
			{
				m_iPPQTriggerError++;
			}
		}
	}

}

void CSVOConfigAssistantDlg::CameraDeletedCheckAgainstPPQ(CString sCameraName)
{
	int iPPQCnt = GetPPQListCount();
	CSVOPPQObj *pObj;
	int iCamCnt;
	CString sCam;
	
	for (int i = 0; i < iPPQCnt; i++)
	{
		pObj = GetPPQObject(i);
		iCamCnt = pObj->GetAttachedCameraCount();
		
		for (int iC = 0; iC < iCamCnt; iC++)
		{
			if (pObj->GetAttachedCamera(iC).Compare(sCameraName) == 0)
			{
				pObj->DetachCameraFromPPQ(sCameraName);
				pObj->RemoveCameraInputConditionalOutput();
				iCamCnt--;
			}
		}
		if (iCamCnt == 0)
		{
			if ( AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, pObj->GetPPQName(), PPQ_NO_CAMERA)) )
			{
				m_iPPQCameraError++;
			}
		}
	}
}

void CSVOConfigAssistantDlg::CameraDeletedCheckAgainstInspection(CString sCameraName)
{
	CSVOInspectionObj *pInspect;
	long lSize;
	long l;
	
	lSize = GetInspectionListCount();
	for( l = 0; l < lSize; l++ )
	{
		pInspect = GetInspectionObject( l );
		
		if( pInspect->GetToolsetImage().IsEmpty() )
		{
			if ( AddMessageToList( INSPECT_DLG, BuildDisplayMessage( MESSAGE_TYPE_ERROR, pInspect->GetInspectionName(), TOOLSET_IMAGE_ERROR ) ) )
			{
				m_iInsToolsetError++;
			}
		}// end if
		
	}// end for
	
}// end CameraDeletedCheckAgainstInspection

void CSVOConfigAssistantDlg::CameraDetachedCheckAgainstInspection(CString sCameraName)
{
	CSVOInspectionObj *pInspect;
	long lSize;
	long l;
	
	lSize = GetInspectionListCount();
	for( l = 0; l < lSize; l++ )
	{
		pInspect = GetInspectionObject( l );
		
		if( pInspect->GetToolsetImage().IsEmpty() )
		{
			if ( AddMessageToList( INSPECT_DLG, BuildDisplayMessage( MESSAGE_TYPE_ERROR, pInspect->GetInspectionName(), TOOLSET_IMAGE_ERROR ) ) )
			{
				m_iInsToolsetError++;
			}
		}// end if
		
	}// end for
	
}// end CameraDetachedCheckAgainstInspection

void CSVOConfigAssistantDlg::InspectionDeletedCheckAgainstPPQ(CString sInspectName)
{
	int iPPQCnt = GetPPQListCount();
	CSVOPPQObj *pObj;
	int iInsCnt;
	CString sCam;
	
	for (int i = 0; i < iPPQCnt; i++)
	{
		pObj = GetPPQObject(i);
		iInsCnt = pObj->GetAttachedInspectionCount();
		
		for (int iC = 0; iC < iInsCnt; iC++)
		{
			if (pObj->GetAttachedInspection(iC).Compare(sInspectName) == 0)
			{
				pObj->DetachInspectionFromPPQ(sInspectName);
				iInsCnt--;
			}
		}
		if (iInsCnt == 0)
		{
			AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_WARNING, pObj->GetPPQName(), PPQ_NO_INSPECT));
		}
	}
}

void CSVOConfigAssistantDlg::IsGood()
{
	if (!ContainsErrorMessage())
	{
		m_btnOk.EnableWindow(TRUE);
	}
	else
	{
		m_btnOk.EnableWindow(FALSE);
	}
}

void CSVOConfigAssistantDlg::OnDblclkLstErrList() 
{
	int iPage;
	int iCurSel = m_ctlMsgList.GetCurSel();
	if (iCurSel != LB_ERR)
	{
		iPage = (int)m_ctlMsgList.GetItemData(iCurSel);
		if (iPage != 999)
		{
			m_dlgPropSheet.SetActivePage(iPage);
		}
		else
		{
			m_ctlConfigurationName.SetFocus();
		}
	}
}

CString CSVOConfigAssistantDlg::GetConfigurationName()
{
	return m_sConfigurationName;
}

void CSVOConfigAssistantDlg::OnChangeEditConfigurationName() 
{
	UpdateData();
	//	m_ctlConfigurationName.GetWindowText(m_sConfigurationName);
	m_sConfigurationName.TrimLeft();
	m_sConfigurationName.TrimRight();
	
	if (m_sConfigurationName.IsEmpty())
	{
		AddMessageToList(999,BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", CONFIG_NAME_ERR));
		m_bConfigName = FALSE;
	}
	else
	{
		RemoveMessageFromList(BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", CONFIG_NAME_ERR));
		m_bConfigName = TRUE;
	}
	IsGood();
}

BOOL CSVOConfigAssistantDlg::GetNewConfigFlag()
{
	return m_bNewConfiguration;
}

BOOL CSVOConfigAssistantDlg::Modified()
{
	return m_bModified;
}

//
// If bTrigger is true then this function returns the max number of triggers allowed.
//
int CSVOConfigAssistantDlg::GetAllowedNumberOfDigs(BOOL bTrigger/* = FALSE*/)
{
	int iNumberAllowed = 0;

	const SVIMTypeInfoStruct& info = SVHardwareManifest::GetSVIMTypeInfo(m_lConfigurationType);
	
	if (bTrigger)
	{
		iNumberAllowed = info.m_MaxTriggers;
	}
	else
	{
		iNumberAllowed = info.m_MaxDigitizers;
	}
	return iNumberAllowed;
}

void CSVOConfigAssistantDlg::CheckAgainstCurrentList()
{
	const SVIMTypeInfoStruct& info = SVHardwareManifest::GetSVIMTypeInfo(m_lConfigurationType);
	
	int iNumberDigitizersAllowed = info.m_MaxDigitizers;
	int iNumberTriggersAllowed = info.m_MaxTriggers;

	m_iInvalidCamera = 0;
	m_iInvalidTrigger = 0;

	// Clear Messages
	RemoveAllDlgMessages(CAMERA_DLG);
	RemoveAllDlgMessages(TRIGGER_DLG);

	if (iNumberDigitizersAllowed)
	{
		for (int i = 0; i < m_CameraList.GetCameraListCount(); i++)
		{
			CSVOCameraObj *pObj = m_CameraList.GetCameraObjectByPosition(i);
			if (pObj->GetDigNumber() > iNumberDigitizersAllowed-1)
			{
				if ( AddMessageToList(CAMERA_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pObj->GetCameraDisplayName(), INVALID_CAMERA) ) )
				{
					m_iInvalidCamera++;
				}
			}
		}
	}
	if (iNumberTriggersAllowed)
	{
		for (int i = 0; i <  m_TriggerList.GetTriggerListCount(); i++)
		{
			CSVOTriggerObj *pObj = m_TriggerList.GetTriggerObjectByPosition(i);
			if ( pObj->GetTriggerDigNumber() > iNumberTriggersAllowed-1 )
			{
				AddMessageToList(TRIGGER_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, CString(pObj->GetTriggerDisplayName()), INVALID_TRIGGER) );
				m_iInvalidTrigger++;
			}
		}
	}
	m_Page1.SetupList();
	m_Page2.SetupList();
}

BOOL CSVOConfigAssistantDlg::IsInspectionOnPPQ(CString sPPQName, CString sInspectionLabel)
{
	BOOL bFound = FALSE;
	CSVOPPQObj *pPpq = GetPPQObjectByName(sPPQName);
	
	if (pPpq)
	{
		int iInsCnt = pPpq->GetAttachedInspectionCount();
		
		for (int i = 0; ((i < iInsCnt) && (!bFound)); i++)
		{
			if (sInspectionLabel.CompareNoCase(pPpq->GetAttachedInspection(i)) == 0)
			{
				bFound = TRUE;
			}
		}
	}
	return bFound;
}

BOOL CSVOConfigAssistantDlg::IsCameraOnPPQ(CString sPPQName, CString sCameraName)
{
	BOOL bFound = FALSE;
	CSVOPPQObj *pPpq = GetPPQObjectByName(sPPQName);
	
	if (pPpq)
	{
		int iCamCnt = pPpq->GetAttachedCameraCount();
		
		for (int i = 0; ( (i < iCamCnt) && (!bFound) ); i++)
		{
			if (sCameraName.CompareNoCase(pPpq->GetAttachedCamera(i)) == 0)
			{
				bFound = TRUE;
			}
		}
	}
	return bFound;
}

BOOL CSVOConfigAssistantDlg::IsTriggerOnPPQ(CString sPPQName, CString sTriggerName)
{
	BOOL bFound = FALSE;
	CSVOPPQObj *pPpq = GetPPQObjectByName(sPPQName);
	
	if (pPpq)
	{
		if ( sTriggerName.CompareNoCase(pPpq->GetAttachedTriggerName()) == 0 )
		{
			bFound = TRUE;
		}
	}
	return bFound;
}

void CSVOConfigAssistantDlg::LastInspectionLabelDeleted(CString sInspectionLabel)
{
	CString sTmp = sInspectionLabel.Mid(11);
	int iNum = atoi(sTmp);
	if ( iNum > m_iLastInspectionNum )
	{
		m_iLastInspectionNum = iNum;
	}
}

BOOL CSVOConfigAssistantDlg::CanInspectionLableBeUsed(CString sLabelName)
{
	BOOL bRet = FALSE;
	POSITION pos = m_slInspectionLabelsUsed.Find(sLabelName);
	
	if ( pos == NULL )
	{
		bRet = TRUE;
	}   
	return bRet;
}

BOOL CSVOConfigAssistantDlg::CanInspectionNameBeUsed(CString sName)
{
	BOOL bRet = FALSE;
	POSITION pos = m_slInspectionNamesUsed.Find(sName);
	
	if ( pos == NULL )
	{
		bRet = TRUE;
	} 

	return bRet;
}

void CSVOConfigAssistantDlg::AddInspectionLableUsed(CString sLabelName)
{
	m_slInspectionLabelsUsed.AddTail(sLabelName);
}

void CSVOConfigAssistantDlg::AddInspectionNameUsed(CString sName)
{
	m_slInspectionNamesUsed.AddTail(sName);
}

BOOL CSVOConfigAssistantDlg::SystemChangeResetCamera( SVIMProductEnum p_lNewSystemType )
{
	BOOL l_bRet = FALSE;

	switch( p_lNewSystemType )
	{
		case SVIM_PRODUCT_FULL:
		case SVIM_PRODUCT_05:
		{
			//go thru each camera and remove Ch#'s and assign dig #'s
			int iNum = GetCameraListCount();
			int iCh = 0;
			int iDig = 0;
			CSVOCameraObj *pObj = NULL;
			for ( int i = 0; i < iNum; i ++ )
			{
				pObj = GetCameraObject(i);
				if ( pObj )
				{
					iCh = pObj->GetBandNumber();
					pObj->SetBandNumber(0);
					//iDig = pObj->GetDigNumber(); // Leave the dig number alone.
					//pObj->SetDigNumber(iDig);
					pObj->SetCameraFile("");
					if (!pObj->IsFileAcquisition())
					{
						if ( AddMessageToList(CAMERA_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pObj->GetCameraDisplayName(), CAMERA_INVALID_FILES)) )
						{
							m_iCameraErrors++;
						}
					}
				}
			}
			l_bRet = TRUE;
			break;
		}
		default:
		{
			//go thru each camera and remove dig #'s and assign Ch#'s
			int iNum = GetCameraListCount();
			int iCh = 0;
			int iDig = 0;
			CSVOCameraObj *pObj = NULL;
			for ( int i = 0; i < iNum; i++ )
			{
				pObj = GetCameraObject(i);
				if ( pObj )
				{
					iDig = pObj->GetDigNumber();
					pObj->SetDigNumber(0);
					pObj->SetBandNumber(iDig);
					pObj->SetCameraFile("");
					if (!pObj->IsFileAcquisition())
					{
						if ( AddMessageToList(CAMERA_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pObj->GetCameraDisplayName(), CAMERA_INVALID_FILES)) )
						{
							m_iCameraErrors++;
						}
					}
				}
			}
			l_bRet = TRUE;
			break;
		}
	}
	return l_bRet;
}

void CSVOConfigAssistantDlg::ClearMessages()
{
	m_ctlMsgList.ResetContent();

	m_iInspectErrors = 0;
	m_iCameraErrors = 0;
	m_iInsToolsetError = 0;
	m_iInvalidCamera = 0;
	m_iPPQTriggerError = 0;
	m_iInvalidTrigger = 0;
	m_iPPQCameraError = 0;
	m_iPPQMaintainSourceImageError = 0;
	m_i1394ColorCamError = 0;

	OnChangeEditConfigurationName();
}

void CSVOConfigAssistantDlg::ConvertToDigital(SVIMProductEnum eType)
{
	typedef std::map<CString, int> StringIntMap;
	CString baseTriggerName = (!IsNonIOSVIM(eType) ? TRIGGER_FIXED_NAME : SV_CAMERA_TRIGGER_NAME);
	//get trigger count...
	int iTriggerCount = m_TriggerList.GetTriggerListCount();
	int iCameraCount = m_CameraList.GetCameraListCount();

	CString sCameraName;
	CString sTriggerName;

	//delete all triggers and re add them.
	StringIntMap l_Triggers;
	for( int i = 0 ; i < iTriggerCount ; i++ )
	{
		CSVOTriggerObj* pTrig = m_TriggerList.GetTriggerObjectByPosition(i);
		l_Triggers[pTrig->GetTriggerDisplayName()] = pTrig->GetTriggerDigNumber();
	}

	m_TriggerList.ResetContent();

	for ( StringIntMap::iterator it = l_Triggers.begin(); it != l_Triggers.end(); ++it )
	{
		m_TriggerList.AddTriggerToList(SVString(it->first), it->second);
	}

	//delete all cameras and re add them
	StringIntMap l_Cameras;
	for( int i = 0 ; i < iCameraCount ; i++ )
	{
		CSVOCameraObj* pCam = m_CameraList.GetCameraObjectByPosition(i);
		l_Cameras[pCam->GetCameraDisplayName()] = pCam->GetDigNumber();
	}

	m_CameraList.ResetContent();

	for ( StringIntMap::iterator it = l_Cameras.begin(); it != l_Cameras.end() ; ++it )
	{
		m_CameraList.AddCameraToList(it->first, it->second);
	}
}

void CSVOConfigAssistantDlg::RemoveFileAcquisitionMessages(CSVOCameraObj* pCam)
{
	CString sFileAcquisitionNotAllowedMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, pCam->GetCameraDisplayName(), MESSAGE_FILE_ACQUISITION_NOT_ALLOWED);
	CString sEmptyFileNameMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCam->GetCameraDisplayName(), MESSAGE_FILE_ACQUISITION_MISSING_FILENAME);
	CString sEmptyDirNameMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCam->GetCameraDisplayName(), MESSAGE_FILE_ACQUISITION_MISSING_DIRECTORY);
	CString sInvalidFileMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, pCam->GetCameraDisplayName(), MESSAGE_FILE_ACQUISITION_INVALID_FILE);
	CString sInvalidDirMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, pCam->GetCameraDisplayName(), MESSAGE_FILE_ACQUISITION_INVALID_DIRECTORY);
	CString sInvalidImageSizeMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCam->GetCameraDisplayName(), MESSAGE_FILE_ACQUISITION_INVALID_IMAGE_SIZE);

	RemoveMessageFromList(sFileAcquisitionNotAllowedMessage);
	RemoveMessageFromList(sEmptyFileNameMessage);
	RemoveMessageFromList(sEmptyDirNameMessage);
	RemoveMessageFromList(sInvalidFileMessage);
	RemoveMessageFromList(sInvalidDirMessage);
	RemoveMessageFromList(sInvalidImageSizeMessage);
}

HRESULT CSVOConfigAssistantDlg::CheckCamera(CSVOCameraObj* pCam)
{
	RemoveFileAcquisitionMessages(pCam);

	CString sDigName = BuildDigName( pCam, true );
	if (pCam->IsFileAcquisition())
	{
		CString sEmptyFileNameMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCam->GetCameraDisplayName(), MESSAGE_FILE_ACQUISITION_MISSING_FILENAME);
		CString sEmptyDirNameMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCam->GetCameraDisplayName(), MESSAGE_FILE_ACQUISITION_MISSING_DIRECTORY);
		CString sInvalidFileMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, pCam->GetCameraDisplayName(), MESSAGE_FILE_ACQUISITION_INVALID_FILE);
		CString sInvalidDirMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, pCam->GetCameraDisplayName(), MESSAGE_FILE_ACQUISITION_INVALID_DIRECTORY);
		CString sInvalidImageSizeMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCam->GetCameraDisplayName(), MESSAGE_FILE_ACQUISITION_INVALID_IMAGE_SIZE);

		// check that image size is not zero
		const SIZE& size = pCam->GetFileImageSize();
		if (size.cx == 0 || size.cy == 0)
		{
			AddMessageToList( CAMERA_DLG, sInvalidImageSizeMessage );
		}
		
		// check that file or directory is valid
		long loadingMode = pCam->GetFileLoadingMode();
		if (loadingMode == ContinuousMode || loadingMode == SingleIterationMode)
		{
			// check that directory exists
			CString directoryName = pCam->GetImageDirectoryName();
			if (!directoryName.IsEmpty())
			{
				// strip off trailing slash or backslash
				CString lastChar = directoryName.Right(1);
				if (lastChar == _T("\\") || lastChar == _T("/"))
					directoryName.Delete(directoryName.GetLength() - 1);

				struct _stat buf;
				int result = _stat( directoryName, &buf );
				
				if (result != 0)
				{
					AddMessageToList( CAMERA_DLG, sInvalidDirMessage );
				}
			}
			else
			{
				AddMessageToList( CAMERA_DLG, sEmptyDirNameMessage );
			}
		}
		else 
		{
			// check that File Exists
			CString filename = pCam->GetImageFilename();
			if ( filename.IsEmpty() )
			{
				AddMessageToList( CAMERA_DLG, sEmptyFileNameMessage);
			}
			else
			{
				// check that File Exists
				if ( _access( filename, 0 ) != 0 )
				{
					AddMessageToList( CAMERA_DLG, sInvalidFileMessage );
				}
			}
		}
	}
	else
	{
		SVAcquisitionClassPtr pDevice;

		SVImageProcessingClass::Instance().GetAcquisitionDevice( sDigName, pDevice );
		if( !( pDevice.empty() ) )
		{
			SVDeviceParamCollection params;
			HRESULT hr = pDevice->ReadCameraFile( pCam->GetCameraFile(), params );
			CString sMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, pCam->GetCameraDisplayName(), MESSAGE_UNSUPPORTED_CAM_FILE);
			if ( hr == SVMSG_INCORRECT_CHECKSUM )
			{
				//AddMessageToList(CAMERA_DLG,sMessage);
			}
			else
			{
				//RemoveMessageFromList(sMessage);
			}

			bool bDisplayWarnings = TheSVObserverApp.GetSVIMType() == GetProductType();
			CString sMessageIncorrectCamFile = BuildDisplayMessage(MESSAGE_TYPE_WARNING, pCam->GetCameraDisplayName(), MESSAGE_INCORRECT_CAM_FILE);
			RemoveMessageFromList(sMessageIncorrectCamFile);
			if( params.ParameterExists( DeviceParamVendorId ) || params.ParameterExists( DeviceParamModelName ) )
			{
				// do this only if True SVIM type matches selected product type - do warning
				if( bDisplayWarnings )
				{
					if( pDevice->IsValidCameraFileParameters( params ) != S_OK )
					{
						AddMessageToList( CAMERA_DLG, sMessageIncorrectCamFile );
					}
				}// end if ( bDisplayWarnings )
			}

			CString sMessageNotColorCamFile = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCam->GetCameraDisplayName(), MESSAGE_NOT_COLOR_CAM_FILE);
			CString sMessageNoColorCam = BuildDisplayMessage(MESSAGE_TYPE_WARNING, pCam->GetCameraDisplayName(), MESSAGE_NOT_COLOR_CAM);
			
			if ( RemoveMessageFromList(sMessageNotColorCamFile) )
			{
				m_i1394ColorCamError--;
			}
			RemoveMessageFromList(sMessageNoColorCam);
			// if camera file color mismatch selected product color - error
			//      only color product and b/w cam file
			//      the other way is OK, because color cams usu. support b/w images
			if ( GetProductType() == SVIM_PRODUCT_D1_COLOR
				|| GetProductType() == SVIM_PRODUCT_D2_COLOR
				|| GetProductType() == SVIM_PRODUCT_D3_COLOR
				|| GetProductType() == SVIM_PRODUCT_X1_COLOR
				|| GetProductType() == SVIM_PRODUCT_X2_COLOR 
				|| GetProductType() == SVIM_PRODUCT_X3_COLOR
				|| GetProductType() == SVIM_PRODUCT_X2_GD1A_COLOR
				|| GetProductType() == SVIM_PRODUCT_X2_GD2A_COLOR
				|| GetProductType() == SVIM_PRODUCT_X2_GD8A_COLOR )
			{
				if ( params.ParameterExists( DeviceParamCameraFormats ) )
				{
					const SVCameraFormatsDeviceParam* pParam = params.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
					if ( !pParam->SupportsColor() )
					{
						if ( AddMessageToList(CAMERA_DLG,sMessageNotColorCamFile) )
						{
							m_i1394ColorCamError++;
						}
					}

					SVDeviceParamCollection l_HardwareParams;
					pDevice->GetHardwareDeviceCapabilities(l_HardwareParams);

					// if camera file mismatch camera in color options - warning 
					// do this only if True SVIM type matches selected product type - warning
					if ( pDevice->IsValid() && l_HardwareParams.ParameterExists( DeviceParamCameraFormats ) )
					{
						if ( bDisplayWarnings )
						{
							const SVCameraFormatsDeviceParam* pHardwareParam = l_HardwareParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pHardwareParam );
							if ( pParam->SupportsColor() && !pHardwareParam->SupportsColor() )
							{
								AddMessageToList(CAMERA_DLG,sMessageNoColorCam);
							}
						}
					}// end if ( l_HardwareCapabilities.ParameterExists( DeviceParamCameraFormats ) )
				}// end if ( params.ParameterExists( DeviceParamCameraFormats ) )
			}// end if 1394 color product type
		}// end if ( pDevice )
	}
	return S_OK;
}

void CSVOConfigAssistantDlg::CheckTriggers()
{
	int iTrg = GetTriggerListCount();
	for (int i = 0;i < iTrg;i++)
	{
		CSVOTriggerObj* pTrig = GetTriggerObject(i);
		CheckTrigger(pTrig);
	}
	IsGood();
}

BOOL CSVOConfigAssistantDlg::CheckTrigger(CSVOTriggerObj* pTrig)
{
	BOOL bRet = true;
	CString sMessageNoSoftwareTriggerAllowed = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CString(pTrig->GetTriggerDisplayName()), MESSAGE_SOFTWARE_TRIGGER_NOT_ALLOWED);

	if (pTrig->IsSoftwareTrigger())
	{
		// check if Software Trigger is allowed
		bRet = IsSoftwareTriggerAllowed(pTrig->GetTriggerDisplayName());
		if (!bRet)
		{
			// add message
			AddMessageToList(TRIGGER_DLG, sMessageNoSoftwareTriggerAllowed );
		}
		else
		{
			RemoveMessageFromList( sMessageNoSoftwareTriggerAllowed );
		}
	}
	else
	{
		RemoveMessageFromList( sMessageNoSoftwareTriggerAllowed );
	}
	return  bRet;
}

bool CSVOConfigAssistantDlg::IsColorConfig() const
{
	return SVHardwareManifest::IsColorSystem(m_lConfigurationType);
}
		
HRESULT CSVOConfigAssistantDlg::ConnectToolsetBuffers()
{
	// EB 2002 02 28
	// Added this to fix memory leak freeing MIL buffers
	// before we close Acq devices, we need to tell all toolsets to Close
	// reopen the connection between toolset images and the MIL acq image here.

	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	long lInspectionCount = 0;
	SVInspectionProcess* pInspection = NULL;
	pConfig->GetInspectionCount( lInspectionCount );

	for ( long lInspection = 0; lInspection < lInspectionCount; lInspection++ )
	{
		pConfig->GetInspection( lInspection, &pInspection );
		if ( pInspection != NULL )
		{
			if ( pInspection->GetToolSet() != NULL )
			{
				pInspection->ConnectToolSetMainImage();
			}
		}
	}

	return S_OK;
}

void CSVOConfigAssistantDlg::SetupTriggerStrobeMessage()
{
	// Check the ioboard capabilities.  If it has less inverters than triggers 
	// then display a warning message.
	if ( SVHardwareManifest::IsDigitalSVIM( m_lConfigurationType ) )
	{
		if( m_svCapabilities.GetStrobeInverterCount() < m_TriggerList.GetTriggerListCount() )
		{
			if( m_TriggerList.GetTriggerListCount() > 1 )
			{
				AddMessageToList(TRIGGER_DLG,
					BuildDisplayMessage( MESSAGE_TYPE_WARNING, "", MESSAGE_ONE_INVERT_CONTROL ));
			}
			else
			{
				RemoveMessageFromList(BuildDisplayMessage( MESSAGE_TYPE_WARNING, "", MESSAGE_ONE_INVERT_CONTROL ));
			}
		}
	}
}

void CSVOConfigAssistantDlg::SetIOBoardCapabilities( SVIOBoardCapabilities& p_svCapable )
{
	m_svCapabilities = p_svCapable;
}

void CSVOConfigAssistantDlg::OnHelpButton()
{
	long lContextID = m_dlgPropSheet.GetActivePage()->GetWindowContextHelpId();
	OnCommandHelp(0,lContextID);
}

SVImportedInspectionInfoList& CSVOConfigAssistantDlg::GetImportedInspectionInfoList()
{
	return m_ImportedInspectionInfoList;
}

void CSVOConfigAssistantDlg::ClearImportedInspectionInfoList()
{
	m_ImportedInspectionInfoList.clear();
}

CString CSVOConfigAssistantDlg::GetNameFromProductID( SVIMProductEnum p_ID )
{
	CString l_Retval = "";

	SVProductStringMap::index_iterator< from >::type l_Iter = m_ProductStringMap.get< from >().find( p_ID );

	if( l_Iter != m_ProductStringMap.get< from >().end() )
	{
		l_Retval = l_Iter->second;
	}
	return l_Retval;
}

SVIMProductEnum CSVOConfigAssistantDlg::GetProductIDFromName( const CString& p_Name )
{
	SVIMProductEnum l_Retval = SVIM_PRODUCT_INVALID_TYPE;

	SVProductStringMap::index_iterator< to >::type l_Iter = m_ProductStringMap.get< to >().find( p_Name );

	if( l_Iter != m_ProductStringMap.get< to >().end() )
	{
		l_Retval = l_Iter->first;
	}
	return l_Retval;
}

bool CSVOConfigAssistantDlg::IsNonIOSVIM(SVIMProductEnum productType) const
{
	return SVHardwareManifest::IsNonIOSVIM(productType);
}

bool CSVOConfigAssistantDlg::IsGigeSystem() const
{
	return SVHardwareManifest::IsMatroxGige(GetProductType());
}

bool CSVOConfigAssistantDlg::IsDigitalSystem() const
{
	return SVHardwareManifest::IsDigitalSVIM(GetProductType());
}

bool CSVOConfigAssistantDlg::IsValidCamera(int iDig) const
{
	bool bRetVal = false;
	int iNum = GetCameraListCount();
	if (iDig < iNum)
	{
		for (int i = 0;i < iNum && !bRetVal; i++)
		{
			const CSVOCameraObj* pObj = GetCameraObject(i);
			if (pObj)
			{
				if (pObj->GetDigNumber() == iDig)
				{
					bRetVal = true;
				}
			}
		}
	}
	return bRetVal;
}

bool CSVOConfigAssistantDlg::IsFileAcquisition(int iDig) const
{
	bool bRetVal = false;
	int iNum = GetCameraListCount();
	if (iDig < iNum)
	{
		for (int i = 0;i < iNum && !bRetVal; i++)
		{
			const CSVOCameraObj* pObj = GetCameraObject(i);
			if (pObj)
			{
				if (pObj->GetDigNumber() == iDig)
				{
					bRetVal = pObj->IsFileAcquisition();
				}
			}
		}
	}
	return bRetVal;
}

 CSVOPPQObj* CSVOConfigAssistantDlg::GetPPQForAttachedTrigger(const CString& sTriggerName)
{
	//see if Trigger is connected to any PPQ.
	int iPPQCnt = GetPPQListCount();
	
	for (int i = 0; i < iPPQCnt; i++)
	{
		CSVOPPQObj* pObj = GetPPQObject(i);
		
		if (pObj->GetAttachedTriggerName().Compare(sTriggerName) == 0)
		{
			return pObj;
		}
	}
	return NULL;
}

 void CSVOConfigAssistantDlg::OnBnClickedCancel()
{
	// use m_TmpCameraList to get back the orginial camera information
	if ( SVHardwareManifest::IsDigitalSVIM( GetProductType() ) )
	{
		int iSize = m_TmpCameraList.GetCameraListCount();

		for ( int i = 0; i < iSize; i++ )
		{
			CSVOCameraObj *pObj = m_TmpCameraList.GetCameraObjectByPosition(i);
			SVAcquisitionClassPtr psvDevice;
			SVFileNameArrayClass svFiles;
			SVFileNameClass svFile;

			if ( pObj )
			{
				if ( S_OK == SVImageProcessingClass::Instance().GetAcquisitionDevice( BuildDigName(pObj), psvDevice ) )
				{
					CString szFileName = pObj->GetCameraFile();
					svFile.SetFullFileName(szFileName);
					svFiles.Add(svFile);
					psvDevice->LoadFiles(svFiles);
					
					psvDevice->SetDeviceParameters(pObj->GetCameraDeviceParams());
				}
			}
		}
	}//if digital SVIM

	CDialog::OnCancel();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOConfigAssistantDlg.cpp_v  $
 * 
 *    Rev 1.13   07 Mar 2014 18:22:14   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed THIS_FILE.
 *   Removed call to DetachAcqFromTriggers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   04 Feb 2014 15:42:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed OnInitDialog method to use getConfigFileName instead of GetSECFileName
 * Changed OnOK method to remove .SEC file support.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   01 Feb 2014 11:39:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   31 Jan 2014 17:16:36   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   02 Dec 2013 11:03:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  870
 * SCR Title:  Fix Various Issues in the Edit Configuration Dialog
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic to show or hide the camera manager button based on system type and configuration type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Oct 2013 14:51:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  870
 * SCR Title:  Fix Various Issues in the Edit Configuration Dialog
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SystemChangeResetCamera so the camera object is copied correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   02 Oct 2013 06:48:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Aug 2013 13:54:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  851
 * SCR Title:  Fix bugs in the Configuration Assistant dialog
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified ConvertToDigital to use previous information about triggers and cameras when changing to the new system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   07 Aug 2013 13:24:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   26 Jul 2013 10:17:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  848
 * SCR Title:  Fix issue with Configuration Assistant not canceling correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to put back camera files and camera parameters when a cancel was pressed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Jul 2013 09:11:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.148   03 Jun 2013 09:41:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  840
 * SCR Title:  Fix the GigE camera to camera file validation
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added check to validate camera to camera file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 May 2013 16:30:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.147   26 Apr 2013 13:31:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Fixed Bug in Convert to Digital checking for IsNonIOSVIM.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:11:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.146   16 Apr 2013 15:35:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:49:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.145   19 Mar 2013 14:45:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Renamed AddOrUpdateDigitalInput to AddImportedDigitalInput.
 * Renamed AddOrUpdateDDEInput to AddImportedDDEInput.
 * Renamed AddOrUpdateRemoteInput to AddImportedRemoteInput.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.144   15 Mar 2013 13:05:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetConfigurationForExisting to prevent a crash when editing a configration with both a camera_trigger and a regular trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.143   05 Mar 2013 16:50:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added  GetImportedInspectionInfoList method
 * Added ClearImportedInspectionInfoList method
 * Added logic to support the importing of inspections
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.142   21 Jan 2013 11:37:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsCameraLineInoutAllowed method
 * Added IsNOnIOSVIM method
 * Added CreateDefaultForNonIOSVIM method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.141   16 Jan 2013 16:17:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Migrated all hardware tablized data to the hardware manifest object and coverted lokk-up to use the hardware manifest object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.140   10 Jan 2013 15:10:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged 6.01 branch code into main development code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.139   17 Dec 2012 14:02:10   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (InitializeConfigSystemConversion) Changed GD2A to unsupported.
 *   (IsCompatible) Changed to handle GD2A and GD4A correctly. Cleaned up.  Added comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.138   13 Dec 2012 10:46:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (BuildTrgDig) Added SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (BuildDigName) Added cases for SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (GetAllowedNumberOfDigs) Added case for SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (CheckAgainstCurrentList) Added cases for SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (CheckCamera) Added check for SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (IsColorConfig) Added SVIM_PRODUCT_X2_GD1A_COLOR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.137   11 Dec 2012 19:03:44   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (constructor) Added mapping of SVIM_PRODUCT_X2_GD1A->SYSTEM_SVIM_X2_GD1A.
 *     Added mapping of SVIM_PRODUCT_X2_GD1A_COLOR->SYSTEM_SVIM_X2_GD1A_COLOR.
 *   (InitializeConfigSystemConversion) Added SYSTEM_SVIM_X2_GD1A.
 *     Added SYSTEM_SVIM_X2_GD1A_COLOR.
 *   (ReloadForCurrentSystem) Rearranged to reduce total number of cases.
 *     Added case for SVIM_PRODUCT_X2_GD1A_COLOR.
 *     Added case for SVIM_PRODUCT_X2_GD1A.
 *   (GetProductType) Corrected spelling of "available" in comment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.136   04 Oct 2012 10:49:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.135   28 Sep 2012 14:42:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed problem with inspection name look-up.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.134   04 Sep 2012 15:53:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new utility method to handle the inspection name lookup.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.133.1.3   08 Jan 2013 12:23:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.133.1.2   17 Dec 2012 13:20:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (InitializeConfigSystemConversion) Changed GD2A to unsupported.
 *   (IsCompatible) Changed to handle GD2A and GD4A correctly. Cleaned up.  Added comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.133.1.1   12 Dec 2012 19:33:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (BuildTrgDig) Added SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (BuildDigName) Added cases for SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (GetAllowedNumberOfDigs) Added case for SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (CheckAgainstCurrentList) Added cases for SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (CheckCamera) Added check for SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (IsColorConfig) Added SVIM_PRODUCT_X2_GD1A_COLOR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.133.1.0   11 Dec 2012 17:00:56   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (constructor) Added mapping of SVIM_PRODUCT_X2_GD1A->SYSTEM_SVIM_X2_GD1A.
 *     Added mapping of SVIM_PRODUCT_X2_GD1A_COLOR->SYSTEM_SVIM_X2_GD1A_COLOR.
 *   (InitializeConfigSystemConversion) Added SYSTEM_SVIM_X2_GD1A.
 *     Added SYSTEM_SVIM_X2_GD1A_COLOR.
 *   (ReloadForCurrentSystem) Rearranged to reduce total number of cases.
 *     Added case for SVIM_PRODUCT_X2_GD1A_COLOR.
 *     Added case for SVIM_PRODUCT_X2_GD1A.
 *   (GetProductType) Corrected spelling of "available" in comment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.133   20 Aug 2012 08:54:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   ClearMessages:  Added call to OnChangeEditConfigurationName to maintain correct message for configuration name.
 *   SetupMessagesForNewConfig:  Deleted method.  This method did nothing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.132   15 Aug 2012 17:21:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with messages not being updated correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.131   15 Aug 2012 14:41:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.130   14 Aug 2012 15:40:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated product table to remove SVIM Color from the available product configuration type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.129   13 Aug 2012 16:51:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   InitializeConfigSystemConversion:  Changed SYSTEM_SVIM_X2_GD2A and SYSTEM_SVIM_X2_GD2A_COLOR to supported.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.128   13 Aug 2012 11:39:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed X2B-GD8A model/configuration information from application.  Converted source code to use X2-GD8A instead of X2B-GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.127   07 Aug 2012 08:58:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Constructor:  Moved call to InitializeConfigSystemConversion to here from OnInitDialog.
 *     Refactored String/Product Maps into a single map.
 *   InitializeConfigSystemConversion:  Refactored to use m_SVIMTypeMap instead of 2D vector.
 *   OnInitDialog:  Removed call to InitializeConfigSystemConversion.
 *     Replaced use of m_mapProductToString with call to GetNameFromProductID.
 *   GetNameFromProductID:  Added new method.
 *   GetProductIDFromName:  Added new method.
 *   SetupSystemComboBox:  Refactored to use m_SVIMTypeMap.
 *   OnSelchangeComboAvalSys:  Changed to use new methods.
 *   UpdateAvailableSystems:  Added new method.
 *   ReloadForCurrentSystem:  Replaced use of m_mapProductToString with call to GetNameFromProductID.
 *   BuildTrgDig:  Added cases for SVIM_PRODUCT_X2_GD4A and SVIM_PRODUCT_X2_GD4A_COLOR.
 *   BuildDigName:  Added cases for SVIM_PRODUCT_X2_GD4A and SVIM_PRODUCT_X2_GD4A_COLOR.
 *   SetCurrentSystemDisplay:  Replaced use of m_mapProductToString with call to GetNameFromProductID.
 *   GetProductType:  Corrected spelling of "Product".
 *     Replaced use of m_mapProductToString with call to GetNameFromProductID.
 *   GetAllowedNumberOfDigs:  Added cases for SVIM_PRODUCT_X2_GD4A and SVIM_PRODUCT_X2_GD4A_COLOR.
 *   ISCompatible:  Changed parameter name to p_CurrentConfigType.
 *     Restructured to avoid unnecessary ifs.
 *     Removed handling for unsupported SVIMs.
 *   IsColorConfig:  Added case for SVIM_PRODUCT_X2_GD4A_COLOR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.126   09 Jul 2012 13:45:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added X2_GD4A option for compatibility with this legacy product type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.125   02 Jul 2012 17:23:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.124   02 Jul 2012 14:33:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new configuration type SVIM X2B-GD8A.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.123   02 Jul 2012 13:51:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed configuration type SVIM X2-GD4A.
 * 
 * Added new configuration type SVIM X2-GD8A.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.122   19 Jun 2012 13:12:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.121   20 Dec 2011 14:55:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  741
 * SCR Title:  Fix IO List when changing Inspection Name
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added function calls to rename the output list when inspection name has been changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.120   28 Sep 2011 13:11:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.119   19 Sep 2011 13:14:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVInspectionProcess - SetToolSetImage to SendInspectionDataToInspection function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.118   16 Sep 2011 16:14:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.117   12 Aug 2011 14:57:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  736
 * SCR Title:  Add a 4 Camera GigE System to the list of systems
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Type-o....for the 4 Camera GigE system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.116   12 Aug 2011 14:16:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  736
 * SCR Title:  Add a 4 Camera GigE System to the list of systems
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added 4 Camera GigE System.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.115   11 Aug 2011 07:28:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  735
 * SCR Title:  Add Gige Color System
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added system type and product type to support GigE color cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.114   29 Apr 2011 08:15:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added Null Pointer check in SendPPQAttachmentsToConfiguration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.113   28 Apr 2011 13:18:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed Bug in RenameInspectionObjects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.112   19 Apr 2011 14:03:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added code to rename PPQ Inspection name to new inspection name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.111   30 Mar 2011 16:11:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.110   22 Mar 2011 08:01:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.109   21 Mar 2011 07:54:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  719
 * SCR Title:  Fix converting issues with Configuration Assistant Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issues so that when converting from Digital to Analog the invalid camera messages do not disappear
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.108   26 Jan 2011 09:08:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  713
 * SCR Title:  Fix Help button on Configuration Assistant Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added handler for the Help Button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.107   08 Dec 2010 13:15:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.106   09 Nov 2010 16:25:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.105   05 Nov 2010 11:08:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.104   04 Nov 2010 14:08:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.103   01 Jun 2010 10:59:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.102   24 Feb 2010 16:12:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated dialog to fix laptop settings for color systems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.101   19 Feb 2010 13:13:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated is compatible method to remove the Gige compatibility from the 1394 products.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.100   18 Feb 2010 14:22:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to fix the list of products based on color flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.99   15 Feb 2010 12:37:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.98   16 Dec 2009 12:15:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.97   10 Nov 2009 12:47:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Gige Color system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.96   05 Nov 2009 15:08:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated configuration type to fix the slescted system problem.  And fixed the offset problem in the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.95   29 Oct 2009 14:45:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Fixed issues with missing image processing class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.94   29 Oct 2009 11:27:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issue with model number tracking.  Updated version to 5.30 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.93   27 Oct 2009 18:53:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with disconnecting cameras and invalid image behavior.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.92   17 Jun 2009 08:59:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to remove unused code
 * Added InitializeConfigSystemConversion method
 * Revised OnInitDialog to move the initialization of the configuration conversion table to the InitializeConfigSystemConversion method.
 * Added support for Gige system
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.91   03 Jun 2009 09:58:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.90   28 Apr 2009 09:20:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  658
 * SCR Title:  Update SVObserver to handle the new model number info
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed code to correctly handle model numbers with no hardware (Laptop version)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.89   28 Apr 2009 09:09:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  660
 * SCR Title:  Fix bug with Configuration Assistant when changing to another Available System
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed bug with the warning and error message going away when changing the available system
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.88   20 Jan 2009 10:40:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  645
 * SCR Title:  Add support for X2 and X2 Color products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for the X2
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.87   05 Dec 2008 15:26:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Loading modes for File Acquisiiton
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.86   23 Oct 2008 17:52:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CheckCamera to strip off trailing slash in Directory name for File Acquisition before verifying that directory exists
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.85   21 Oct 2008 14:35:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CheckCamera to check for Invalid Filename for FileAcquisition.
 * Revised CheckCamera to check for Invalid Directory name for FileAcquisition.
 * Revised CheckCamera to check for Empty Filename for FileAcquisition.
 * Revised CheckCamera to check for Empty Directory name for FileAcquisition.
 * Revised CheckCamera to check for Invalid Image Size for FileAcquisition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.84   17 Oct 2008 10:37:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the IsSoftwareTriggerAllowed method to use name instead of ordinal.
 * Revised GetConfigurationForExisting to ensure the ordinal for a Trigger is valid.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.83   08 Oct 2008 09:36:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Hardware Trigger Not Allowed message.
 * Removed IsHardwareTriggerAllowed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.82   03 Oct 2008 17:34:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed Frequency to Period for the Software Timer Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.81   16 Sep 2008 14:53:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsFileAcquisitionAllowed method.
 * Added IsHardwareTriggerAllowed method.
 * Revised BuildDigName, IsSoftwareTriggerAllowed, SendAcquisitionDataToConfiguration, SendCameraDataToConfiguration, GetConfigurationForExisting, ItemChanged, SystemChangeResetCamera, CheckCamera and CheckTrigger in support of FileAcquisition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.80   10 Sep 2008 17:11:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Converted monolithic method (SendDataToConfiguration) into sub-methods.
 * Revised to support SoftwareTrigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.79   21 Jun 2007 14:33:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.78   08 May 2007 15:51:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  596
 * SCR Title:  Fix Bug where LUT parameters are not set after changing the camera.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed camera files scope issue in SendDataToConfig.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.77   29 Mar 2007 15:09:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  596
 * SCR Title:  Fix Bug where LUT parameters are not set after changing the camera.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SendDataToConfiguration to reload the camera if the file had changed then sent to config.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.76   23 Mar 2007 11:02:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  593
 * SCR Title:  Remove HUB product types from SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Conversion Arrays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.75   20 Mar 2007 15:16:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  593
 * SCR Title:  Remove HUB product types from SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Completed Color Conversion Tables in OnInitDialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.74   15 Mar 2007 11:03:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug where the trigger - strobe inverter message was displayed on an analog system when it was meant for digital systems only.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.73   14 Feb 2007 08:08:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified the max number of cameras allowed for a X3.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.72   13 Feb 2007 08:12:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add X3 type to Configuration Assistant Dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.71   13 Feb 2007 07:29:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  593
 * SCR Title:  Remove HUB product types from SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed HUB Functionality from Configuration Assistant dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.70   22 Jan 2007 13:21:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  587
 * SCR Title:  Add Timeout Threshold to Extended Time Delay Mode
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added the get and set of the Timeout Threshold for the PPQ
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.69   29 Nov 2006 14:28:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised due to correction of typo in method name IsVaildCameraFileParameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.68   04 Aug 2006 08:06:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  575
 * SCR Title:  Fix Bug dealing with Image locking that causes run once to fail
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Replaced function GetSourceImageDepth to GetMaxPPQLength where needed to allow source image depth to ge greater than the ppq.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.67   19 Jan 2006 11:17:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to hide Aux Extent property when a gage tool is in the toolset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.66   08 Nov 2005 16:06:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to handle new SVIM X1 product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.65   02 Nov 2005 08:35:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  520
 * SCR Title:  Fix select camera ROI dialog crash
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed Format 7 ROI crash (load camera files on selection in new configuration)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.64   24 Oct 2005 09:40:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support adding a flag to the Inspection that will enable Auxiliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.63   20 Oct 2005 10:15:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed potential crash caused by C-style casts
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.62   22 Aug 2005 10:22:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.61   01 Aug 2005 14:20:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   replaced SV2DArrayClass with vector2d
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.60   19 Jul 2005 14:35:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented support for > 4 camera hubs (1394)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.59   29 Jun 2005 07:53:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  493
 * SCR Title:  Fix Bug in SVOutputObjectList
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   static_cast< instead of C-style cast at SendMessage in SendDataToConfig.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.58   28 Feb 2005 08:45:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   if PPQ length changes, call SetObjectDepth for the attached IPDoc's (which changes their children)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.57   18 Feb 2005 12:17:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed bug which caused a crash if deleting a camera from a configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.56   17 Feb 2005 14:59:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   26 Oct 2004 09:12:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added D1_Hub support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   21 Jul 2004 09:13:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  439
 * SCR Title:  Change default tab for Configuration Settings dialog
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made camera tab the default for non-new configurations
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   25 May 2004 08:32:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  421
 * SCR Title:  Allow SVObserver to get the Trigger Edge and Strobe Edge from the Camera File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SendDataToConfiguration method to update the trigger edge and strobe edge information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   09 Feb 2004 09:55:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   RemoveAllObjectMessages would return the number of messages that were removed for a label.  The number of messages should not have included the warning messages.  Will now return only the number of Error messages deleted.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   03 Feb 2004 17:26:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed support for MODEL check
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   23 Jan 2004 11:08:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added check for model name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   21 Jan 2004 15:04:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problem where if a digital camera supports color it would always write out the light reference as 3 bands even if the configuration is mono.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   16 Jan 2004 07:52:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added temp calls to attach/detach acq devices and trigger/startframe signals
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   13 Jan 2004 10:13:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed bug where OK was greyed out incorrectly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   19 Dec 2003 09:44:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed color camera warning if no camera attached
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   18 Dec 2003 14:47:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed 1394 Light Reference problems
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   18 Dec 2003 14:45:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   made color cam detection a warning not an error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   17 Dec 2003 09:37:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the conversion from a 1394.  It will now delete the camera's and trigger's.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   10 Dec 2003 13:56:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed RemoveAllObjectMessages to return the number of deleted messages.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   10 Dec 2003 10:39:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed compatibility issues with the 1394 products. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   21 Nov 2003 09:56:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   set IO board strobe polarity based on Acq Device param
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   19 Nov 2003 11:30:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed bug with PRODUCT_Dx 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   14 Nov 2003 09:40:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added product types for 1394 D1-D3 and the Color version
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   12 Nov 2003 13:55:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed to handle new product types
 * added camera hardware checking
 * added permanent warning message for camera file checksum error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   04 Nov 2003 09:27:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved Connect / Disconnect camera / toolset buffer functions to SVObserverApp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   03 Nov 2003 16:27:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added code to allow disconnecting and reconnecting buffers in preparation for unplugging cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   22 Oct 2003 09:36:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made UnloadFiles/LoadFiles dependent on the camera file changing
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   16 Oct 2003 09:12:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed LoadLightReference to work correctly with 1394
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   15 Oct 2003 13:17:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes to edit configuration to track camera file changes so that the default light reference will be read from the camera file only initially or when changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   02 Oct 2003 10:48:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Add new code to handle the defaulting of unset light reference values to the values in the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   30 Sep 2003 07:31:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   RPY - added support for conversion to 1394
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   16 Sep 2003 09:23:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated SendDataToConfiguration method to use new member varaible.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   02 Sep 2003 14:00:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated Trigger Name for the 1394 SVIM.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   07 Aug 2003 13:27:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed merge error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   07 Aug 2003 12:47:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified CreateBuffers call to use GetSourceImageDepth as per request by Emhart
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   05 Aug 2003 13:50:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the saving/loading/editing of configurations to add a new property for the inspection's disable method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   01 Aug 2003 12:47:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   display message if camera file checksum is bad
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   30 Jul 2003 15:29:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleaned up file tabs / spaces
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   28 Jul 2003 10:52:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved SetDeviceParameters before GetImageInfo
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   15 Jul 2003 08:57:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   set device params before creating buffers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   08 Jul 2003 10:47:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for 1394
 * changed BOOL to HRESULT
 * changed SVLightReferenceArray to SVLightReference
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   23 May 2003 13:23:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  355
 * SCR Title:  Save tool freeform LUT with the configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated SendDataToConfiguration method to set LUT and Light Reference values back into the board.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   22 May 2003 11:57:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  365
 * SCR Title:  Edit configuration crashes when deleting items after going online
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with deleting a PPQ and its attached items.  It will detach all from the PPQ before the PPQ is deleted.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   30 Apr 2003 17:40:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   30 Apr 2003 10:29:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  334, 358
 * SCR Title:  Deleting a camera and PPQ causes SVObserver to crash
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problem with removing a PPQ and a camera.  When a PPQ is deleted it will also detach the camera from the PPQ before deleting the PPQ.  
 * 
 * Changed the rules for converting configurations.  Will only show in the available system what the configuration can actually be converted to.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   22 Apr 2003 12:29:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   10 Apr 2003 13:27:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  339
 * SCR Title:  Deleting a camera used by an Acquisition Tool causes problems
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the call to DetachCamera from the PPQ. Added the second parameter which will remove the dependcies of the camera
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   28 Mar 2003 12:32:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for telling the configuration it is valid on a new configuration - SetConfigurationLoaded
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   19 Mar 2003 15:20:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed some problems with input/outputs and the Go online/Go offline process. Also, made changes to force the PPQ to always have valid inputs and outputs even without an inspection attached.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   14 Mar 2003 07:18:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  336
 * SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed to use maps instead of if statements
 * changed SVIM_RGB_BOARD and SVIM_COLOR_BOARD to use new enums
 * changed m_lSystemType and m_eBoard to enumerations
 * added SetProductType call to SendDataToConfiguration
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   13 Mar 2003 14:33:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  335
 * SCR Title:  Fix Light Reference and LUT problems with RGB Mono systems
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   when sending the data back to the configuration object, if the system type is RGB_MONO it will have an additional pass to send the device Viper_RGB_1.Dig_0.Ch_All, which is were LUT is stored
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Mar 2003 16:19:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  337
 * SCR Title:  Memory leak when editing a configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   in SendDataToConfiguration, before doing the Acquisition section it will make a call to DisconnectToolSetMainImage and after doing the Acquisition it will ConnectToolSetMainImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   05 Mar 2003 12:09:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed old code used to stub for the code to save source images in the PPQ from the Config dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   28 Feb 2003 14:56:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added functionality to detatch child objects from acq buffers before destroying acq buffers - then reattach afterwards.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   11 Feb 2003 17:24:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SendDataToConfiguration method to remove acquisition flag and handle issues in SVCorecoAcquisitionClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Feb 2003 13:21:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316, 321
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the place where it rebuilds the input list for the PPQ.
 * 
 * Added code for the new PPQ property Maintain Source Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jan 2003 11:55:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed available systems to include hardware that is not present.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Jan 2003 09:52:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added Lut functionality to SendDataToConfiguration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Dec 2002 22:31:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  311
 * SCR Title:  Fix Light Reference save for RGB Mono
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SendDataToConfiguration method of the SVOConfigAssistantDlgClass to reflect changes in references of SVAcquisitionClass member variables to accessor functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Dec 2002 13:19:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  304
 * SCR Title:  Fix crash after deleting a PPQ and its attached inspection(s)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVOConfigAssistantDlg::SendDataToConfiguration method to delete PPQ objects after other objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:35:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
