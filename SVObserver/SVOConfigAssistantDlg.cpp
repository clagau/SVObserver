//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOConfigAssistantDlg
//* .File Name       : $Workfile:   SVOConfigAssistantDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.19  $
//* .Check In Date   : $Date:   23 Oct 2014 17:37:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <iterator>
//Moved to precompiled header: #include <boost/assign.hpp>
//Moved to precompiled header: #include <sys/stat.h>
//Moved to precompiled header: #include <io.h>


#include "SVOConfigAssistantDlg.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFile.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFileLoader.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "CameraLibrary/SVBoolValueDeviceParam.h"
#include "SVFileAcquisitionDevice/SVFileAcquisitionLoadingModeEnum.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObserver.h"
#include "SVOPPQObj.h"
#include "SVOInspectionObj.h"
#include "TriggerInformation/SVOTriggerObj.h"
#include "SVOCameraObj.h"
#include "SVVirtualCamera.h"
#include "SVInputObjectList.h"
#include "SVOutputObjectList.h"
#include "SVDigitizerProcessingClass.h"
#include "SVAcquisitionClass.h"
#include "SVIPDoc.h"
#include "TriggerHandling/SVTriggerClass.h"
#include "SVMessage/SVMessage.h"
#include "TriggerInformation/SVTriggerProcessingClass.h"
#include "SVInspectionImporter.h"
#include "SVImportProgress.h"
#include "TriggerInformation/SVHardwareManifest.h"
#include "TextDefinesSvO.h"
#include "SVOGui\GlobalConstantConflictDlg.h"
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int GrayScaleBitDepth = 8;

// Defines for Camera File extensions
static const CString SVGigeCameraFileDefExt = _T(".ogc");

//const strings for Gig-E product types
static const CString SYSTEM_SVIM_X2_GD1A		( _T("SVIM X2-GD1A") );
static const CString SYSTEM_SVIM_X2_GD1A_COLOR	( _T("SVIM X2-GD1A Color") );
static const CString SYSTEM_SVIM_X2_GD1A_MIXED	( _T("SVIM X2-GD1A Mixed camera") );
static const CString SYSTEM_SVIM_X2_GD2A		( _T("SVIM X2-GD2A") );
static const CString SYSTEM_SVIM_X2_GD2A_COLOR	( _T("SVIM X2-GD2A Color") );
static const CString SYSTEM_SVIM_X2_GD4A		( _T("SVIM X2-GD4A") );
static const CString SYSTEM_SVIM_X2_GD4A_COLOR	( _T("SVIM X2-GD4A Color") );
static const CString SYSTEM_SVIM_X2_GD8A		( _T("SVIM X2-GD8A") );
static const CString SYSTEM_SVIM_X2_GD8A_COLOR	( _T("SVIM X2-GD8A Color") );
static const CString SYSTEM_SVIM_X2_GD8A_MIXED	( _T("SVIM X2-GD8A Mixed camera") );

static const CString SYSTEM_SVIM_X2_GD8A_NONIO		( _T("SVIM Non I/O") );
static const CString SYSTEM_SVIM_X2_GD8A_NONIO_COLOR	( _T("SVIM Non I/O Color") );

static const CString SVIM_BOARD_FILEACQUISITION_STRING     ( _T("File") );
static const CString SVIM_BOARD_MATROX_GIGE	( _T("Matrox_GIGE") );

static const CString SVIM_DIG_NAME_STRING       ( _T(".Dig_") );

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
static const CString MESSAGE_FILE_ACQUISITION_COLOR_MISMATCH ( _T("The bitmap file acquisition color does not match the camera color setting") );
static const CString MESSAGE_INSPECTION_CAMERA_COLOR  ( _T("-The toolset camera image color does not match the inspection color") );

const CSVOConfigAssistantDlg::SVProductStringMap CSVOConfigAssistantDlg::m_ProductStringMap = boost::assign::map_list_of< SVIMProductEnum, CString >
	// SYSTEM ID, DISPLAY NAME
	( SVIM_PRODUCT_X2_GD1A, SYSTEM_SVIM_X2_GD1A )
	( SVIM_PRODUCT_X2_GD1A_COLOR, SYSTEM_SVIM_X2_GD1A_COLOR )
	( SVIM_PRODUCT_X2_GD2A, SYSTEM_SVIM_X2_GD2A )
	( SVIM_PRODUCT_X2_GD2A_COLOR, SYSTEM_SVIM_X2_GD2A_COLOR )
	( SVIM_PRODUCT_X2_GD4A, SYSTEM_SVIM_X2_GD4A )
	( SVIM_PRODUCT_X2_GD4A_COLOR, SYSTEM_SVIM_X2_GD4A_COLOR )
	( SVIM_PRODUCT_X2_GD8A, SYSTEM_SVIM_X2_GD8A )
	( SVIM_PRODUCT_X2_GD8A_COLOR, SYSTEM_SVIM_X2_GD8A_COLOR )
	//( SVIM_PRODUCT_X2_GD8A_NONIO, SYSTEM_SVIM_X2_GD8A_NONIO )
	//( SVIM_PRODUCT_X2_GD8A_NONIO_COLOR, SYSTEM_SVIM_X2_GD8A_NONIO_COLOR );
	;

/////////////////////////////////////////////////////////////////////////////
// CSVOConfigAssistantDlg dialog

CSVOConfigAssistantDlg::CSVOConfigAssistantDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CSVOConfigAssistantDlg::IDD, pParent)
	, m_ctlConfigurationName(SvO::SVEXCLUDECHARS_CONFIG_NAME)
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
	m_dlgPropSheet.SetWindowPos( nullptr, rcSheet.left-7, rcSheet.top-7, 0, 0, 
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
	int iExactStringPosition = m_ctlAvailableSys.FindStringExact(-1,l_sProductString.operator LPCSTR());
	if (CB_ERR != iExactStringPosition)
	{
		m_ctlAvailableSys.SetCurSel(iExactStringPosition);
	}
	else
	{
		m_ctlAvailableSys.SetCurSel(0);
	}

	OnChangeEditConfigurationName();
	SetupMessages();
	IsGood();

	return TRUE;  // return TRUE unless you set the focus to a control
}

// The purpose of this method is to populate the Available Systems combobox.
void CSVOConfigAssistantDlg::SetupSystemComboBox()
{
	CString l_sSystemString;

	SVIMProductEnumList list; 
	typedef std::insert_iterator<SVIMProductEnumList> Insertor;
	Insertor insertor(list, list.begin());
	SvTi::SVHardwareManifest::GetSupportedSVIMList( insertor );

	for (SVIMProductEnumList::iterator l_Iter = list.begin();l_Iter != list.end(); ++l_Iter)
	{
		const CString& l_Name =  GetNameFromProductID(*l_Iter);
		m_ctlAvailableSys.AddString( l_Name );
	}

	if ( !m_bNewConfiguration )
	{
		const CString& l_Name = GetNameFromProductID( m_lConfigurationType );
		const SvTi::SVIMTypeInfoStruct& l_SVIMInfo = SvTi::SVHardwareManifest::GetSVIMTypeInfo( m_lConfigurationType );
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

BOOL CSVOConfigAssistantDlg::AddToCameraList(CString CameraName, int Dig, int CameraID)
{
	return m_CameraList.AddCameraToList(CameraName, Dig, CameraID);
}

BOOL CSVOConfigAssistantDlg::AddToInspectList(CString sExternal, CString sInternal, bool NewInspection)
{
	return m_InspectList.AddInspectionToList(sExternal, sInternal, NewInspection);
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

	SVIMProductEnum CurrentSvimType = m_lConfigurationType; 

	SVIMProductEnum l_ConfigurationType = GetProductIDFromName( sSelectedTxt );

	CString l_PrevName = GetNameFromProductID( CurrentSvimType );
	SVIMProductEnum eType = GetProductIDFromName( l_PrevName );

	if ( ( !m_bNewConfiguration ) || ( m_bModified ) )
	{
		if ( SvTi::SVHardwareManifest::IsCompatible(l_ConfigurationType, CurrentSvimType) )
		{
			m_bModified = TRUE;

			m_lConfigurationType = l_ConfigurationType;
			UpdateAvailableSystems( CurrentSvimType, m_lConfigurationType );
		}
		else
		{
			// Check if I/O digital to Non I/O digital or vice versa
			if ((SvTi::SVHardwareManifest::IsNonIOSVIM(l_ConfigurationType) && !SvTi::SVHardwareManifest::IsNonIOSVIM(CurrentSvimType)) ||
				(!SvTi::SVHardwareManifest::IsNonIOSVIM(l_ConfigurationType) && SvTi::SVHardwareManifest::IsNonIOSVIM(CurrentSvimType)))
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvOi::Tid_Config_SwitchResetQuestion, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10138, SV_GUID_NULL, MB_YESNO);
				if ( IDYES == result )
				{
					m_lConfigurationType = l_ConfigurationType;
					m_bModified = TRUE;
					UpdateAvailableSystems( CurrentSvimType, m_lConfigurationType );
				}
			}
			else
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvOi::Tid_Config_SwitchInvalidQuestion, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10139, SV_GUID_NULL, MB_YESNO);
				if ( IDYES == result )
				{
					m_lConfigurationType = l_ConfigurationType;

					ClearMessages();
					CheckAgainstCurrentList();
					
					SystemChangeResetCamera( m_lConfigurationType );
					m_bModified = TRUE;
					IsGood();

					if ( SvTi::SVHardwareManifest::IsDigitalSVIM( m_lConfigurationType ) )
					{
						ConvertToDigital(m_lConfigurationType);
						SetupMessages();
						if( SvTi::SVHardwareManifest::IsDigitalSVIM( m_lSystemType ))
						{
							m_Page2.ShowCameraDlgBtn( SW_SHOW );
						}
					}
					else
					{
						if ( SvTi::SVHardwareManifest::IsDigitalSVIM( eType ) )
						{
							ConvertToDigital(m_lConfigurationType); //eType);
							m_Page2.DisableAdvanceBtn();
							SetupMessages();
						}
						m_Page2.ShowCameraDlgBtn( SW_HIDE );
					}
					UpdateAvailableSystems( CurrentSvimType, m_lConfigurationType );
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
	const SvTi::SVIMTypeInfoStruct& l_SVIMInfo = SvTi::SVHardwareManifest::GetSVIMTypeInfo(p_CurrentConfigurationType);

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

		switch( m_lConfigurationType )
		{
			case SVIM_PRODUCT_X2_GD1A:
			case SVIM_PRODUCT_X2_GD1A_COLOR:
			case SVIM_PRODUCT_X2_GD2A:
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			{
				CreateDefaultForSVIMDigital(1, SvO::cTriggerFixedName);
				break;
			}
			case SVIM_PRODUCT_X2_GD8A:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
			{
				CreateDefaultForSVIMDigital(2, SvO::cTriggerFixedName);
				break;
			}
			case SVIM_PRODUCT_X2_GD8A_NONIO:
			case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				CreateDefaultForSVIMDigital(2, SvTh::CameraTriggerName);
				break;
			}
			default:
			{
				m_lConfigurationType = SVIM_PRODUCT_X2_GD4A;

				CreateDefaultForSVIMDigital(4, SvO::cTriggerFixedName);

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

void CSVOConfigAssistantDlg::CreateDefaultForSVIMDigital(int Number, LPCTSTR TriggerBaseName )
{
	for ( int i = 0; i < Number; i++ )
	{
		//add a camera
		CString sCameraName = GetNextCameraName();
		m_CameraList.AddCameraToList(sCameraName, i, i);

		//add a trigger
		CString sTriggerName = GetNextTriggerName(TriggerBaseName);
		m_TriggerList.AddTriggerToList(SVString(sTriggerName), i);

		//add an inspection...
		CString sInspectionName = GetNextInspectionName();
		m_InspectList.AddInspectionToList(sInspectionName, sInspectionName, true);
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

CString CSVOConfigAssistantDlg::GetNextCameraName()
{
	int iRet = 0;
	BOOL bFound = TRUE;
	CString sSearchName;

	while ( bFound )
	{
		iRet++;
		sSearchName.Format("%s%d", SvO::cCameraFixedName, iRet);
		bFound = m_CameraList.IsCameraInList(sSearchName);
	}
	m_iNextCameraNumber = iRet;
	return sSearchName;
}

int CSVOConfigAssistantDlg::GetNextCameraNumber() const
{
	return m_iNextCameraNumber;
}

CString CSVOConfigAssistantDlg::GetNextInspectionName() const
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
		sSearchName.Format("%s%d", SvO::cInspectionFixedName, iRet);
		bFound = m_InspectList.IsInspectionInList(sSearchName);
	}
	return sSearchName;
}

CString CSVOConfigAssistantDlg::GetNextTriggerName(const CString& baseName) const
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

CString CSVOConfigAssistantDlg::GetNextPPQName() const
{
	int iRet = 0;
	BOOL bFound = TRUE;
	CString sSearchName;

	while (bFound)
	{
		iRet++;
		sSearchName.Format("%s%d", SvO::cPpqFixedName, iRet);
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
	const SVOInspectionObjPtr pInspectionObj( GetInspectionObjectByName(sInspectLabel) );
	CString sOldName;

	if( nullptr != pInspectionObj )
	{
		sOldName = pInspectionObj->GetInspectionName();
	}

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

SVOInspectionObjPtr CSVOConfigAssistantDlg::GetInspectionObject(int iPos)
{
	return m_InspectList.GetInspectionByPosition(iPos);
}

SVOInspectionObjPtr CSVOConfigAssistantDlg::GetInspectionObjectByName(CString sName)
{
	return m_InspectList.GetInspectionByName(sName);
}

SVOCameraObjPtr CSVOConfigAssistantDlg::GetCameraObject(int iPos)
{
	return m_CameraList.GetCameraObjectByPosition(iPos);
}

SVOCameraObjPtr CSVOConfigAssistantDlg::GetCameraObjectByName(CString sCameraName)
{
	return m_CameraList.GetCameraObjectByName(sCameraName);
}

SvTi::SVOTriggerObjPtr CSVOConfigAssistantDlg::GetTriggerObject(int iPos)
{
	return m_TriggerList.GetTriggerObjectByPosition(iPos);
}

SvTi::SVOTriggerObjPtr CSVOConfigAssistantDlg::GetTriggerObjectByName(CString sTriggerName)
{
	return m_TriggerList.GetTriggerObjectByName(SVString(sTriggerName));
}

SVOPPQObjPtr CSVOConfigAssistantDlg::GetPPQObject(int iPos)
{
	return m_PPQList.GetPPQObjectByPosition(iPos);
}

SVOPPQObjPtr CSVOConfigAssistantDlg::GetPPQObjectByName(CString sPPQName)
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
	const SVOPPQObjPtr pPPQObj = GetPPQObjectByName(sPPQ);
	
	if( nullptr != pPPQObj )
	{
		RemoveUsedTrigger(pPPQObj->GetAttachedTriggerName());

		int iInsCnt = pPPQObj->GetAttachedInspectionCount();

		for (int i = 0; i< iInsCnt; i++)
		{
			RemoveUsedInspect(pPPQObj->GetAttachedInspection(i));
		}
	}

	return m_PPQList.RemovePPQFromList(sPPQ);
}

CString CSVOConfigAssistantDlg::BuildTrgDig( const SvTi::SVOTriggerObj& rTriggerObj) const
{
	CString sDigName;

	int iDig = rTriggerObj.GetTriggerDigNumber();

	if (rTriggerObj.IsAcquisitionTrigger())
	{
		sDigName = SvTi::SVHardwareManifest::BuildAcquisitionTriggerDeviceName(iDig).c_str();
	}
	else if (rTriggerObj.IsSoftwareTrigger())
	{
		sDigName = SvTi::SVHardwareManifest::BuildSoftwareTriggerDeviceName(iDig).c_str();
	}
	else
	{
		switch( m_lConfigurationType )
		{
			case SVIM_PRODUCT_X2_GD1A:
			case SVIM_PRODUCT_X2_GD1A_COLOR:
			case SVIM_PRODUCT_X2_GD2A:
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			case SVIM_PRODUCT_X2_GD4A:
			case SVIM_PRODUCT_X2_GD4A_COLOR:
			case SVIM_PRODUCT_X2_GD8A:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
			{
				sDigName = SvTi::SVHardwareManifest::BuildIOBoardTriggerDeviceName(iDig).c_str();
				break;
			}
			case SVIM_PRODUCT_X2_GD8A_NONIO:
			case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				sDigName = SvTi::SVHardwareManifest::BuildAcquisitionTriggerDeviceName(iDig).c_str();
				break;
			}
		}
	}
	return sDigName;
}

CString  CSVOConfigAssistantDlg::BuildDigName(const SVOCameraObj& rCameraObj) const
{
	CString sDigName;
	int iDigNumber = rCameraObj.GetDigNumber();

	if (rCameraObj.IsFileAcquisition())
	{
		switch( m_lConfigurationType )
		{
			case SVIM_PRODUCT_X2_GD1A:
			case SVIM_PRODUCT_X2_GD2A:
			case SVIM_PRODUCT_X2_GD4A:
			case SVIM_PRODUCT_X2_GD8A:
			case SVIM_PRODUCT_X2_GD8A_NONIO:
			case SVIM_PRODUCT_X2_GD1A_COLOR:
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			case SVIM_PRODUCT_X2_GD4A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				sDigName.Format(_T("%s%s%d"), SVIM_BOARD_FILEACQUISITION_STRING, SVIM_DIG_NAME_STRING, iDigNumber);
				break;
			}

			default:
				break;
		} 
	}
	else
	{
		switch( m_lConfigurationType )
		{
			case SVIM_PRODUCT_X2_GD1A:
			case SVIM_PRODUCT_X2_GD2A:
			case SVIM_PRODUCT_X2_GD4A:
			case SVIM_PRODUCT_X2_GD8A:
			case SVIM_PRODUCT_X2_GD8A_NONIO:
			case SVIM_PRODUCT_X2_GD1A_COLOR:
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			case SVIM_PRODUCT_X2_GD4A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				sDigName.Format(_T("%s%s%d"), SVIM_BOARD_MATROX_GIGE, SVIM_DIG_NAME_STRING, iDigNumber);
				break;
			}

			default:
				break;
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
		if( nullptr != m_CameraList.GetCameraObjectByPosition(i) )
		{
			sCameraDig = BuildDigName( *m_CameraList.GetCameraObjectByPosition(i) );

			if (sDigString == sCameraDig)
			{
				bRet = TRUE;
			}
		}
	}
	return bRet;
}

BOOL CSVOConfigAssistantDlg::IsCameraInList(CString sCameraName) const
{
	return m_CameraList.IsCameraInList(sCameraName);
}

BOOL CSVOConfigAssistantDlg::IsInspectionInList(CString sInspectionName) const
{
	return m_InspectList.IsInspectionInList(sInspectionName);
}

BOOL CSVOConfigAssistantDlg::IsInspectionNameInList(CString sInspectionName) const
{
	return m_InspectList.IsInspectionNameInList(sInspectionName);
}

bool CSVOConfigAssistantDlg::IsTriggerInList(CString sTriggerName) const
{
	return m_TriggerList.IsTriggerInList(SVString(sTriggerName));
}

BOOL CSVOConfigAssistantDlg::IsPPQInList(CString sPPQName) const
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
			m_lConfigurationType = SVIM_PRODUCT_X2_GD8A;
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

BOOL CSVOConfigAssistantDlg::IsTriggerUsed(CString sTriggerName) const
{
	BOOL bRet = false;
	if (nullptr != m_slUsedTriggers.Find(sTriggerName))
	{
		bRet = true;
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

bool CSVOConfigAssistantDlg::IsSoftwareTriggerAllowed(LPCTSTR triggerName) const
{
	bool bRetVal = true;

	// Check for acquistion device and if software triggering is supported
	// For GIGE acquistion device 
	// - check if camera file has been assigned
	// - check if Trigger_Source_Option_N (Software Oneshot) is available

	// check if Digital acquisition system
	if ( SvTi::SVHardwareManifest::IsDigitalSVIM( GetProductType() ) )
	{
		// Get cameras attached to this trigger
		// this is done by getting the ppq for this trigger and iterating the cameras attached to the same ppq
		int iPpq = GetPPQListCount();
		for (int i = 0;i < iPpq;i++)
		{
			const SVOPPQObjPtr pPPQObj = m_PPQList.GetPPQObjectByPosition(i);
			if( nullptr != pPPQObj )
			{
				CString attachedTriggerName = pPPQObj->GetAttachedTriggerName();
				if (attachedTriggerName.CompareNoCase(triggerName) == 0)
				{
					int camCnt = pPPQObj->GetAttachedCameraCount();
					for (int n = 0;n < camCnt;n++)
					{
						CString CameraName = pPPQObj->GetAttachedCamera(n);
						// find camera in camera list
						int iCam = GetCameraListCount();
						for (int c = 0;c < iCam;c++)
						{
							const SVOCameraObjPtr pCameraObj = m_CameraList.GetCameraObjectByPosition(c);
							if( nullptr != pCameraObj )
							{
								if (pCameraObj->GetCameraDisplayName() == CameraName)
								{
									bRetVal = pCameraObj->IsSoftwareTriggerSupported();
								}
							}
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
	// For GIGE acquistion device 
	// - check if camera file has been assigned
	// - check if Line Input Device parameter is available

	// check if Digital acquisition system
	if ( SvTi::SVHardwareManifest::IsDigitalSVIM( GetProductType() ) )
	{
		// Get cameras attached to this trigger
		// this is done by getting the ppq for this trigger and iterating the cameras attached to the same ppq
		int iPpq = GetPPQListCount();
		for (int i = 0;i < iPpq;i++)
		{
			const SVOPPQObjPtr pPPQObj = m_PPQList.GetPPQObjectByPosition(i);
			if( nullptr != pPPQObj )
			{
				const CString& attachedTriggerName = pPPQObj->GetAttachedTriggerName();
				if (attachedTriggerName.CompareNoCase(triggerName) == 0)
				{
					int camCnt = pPPQObj->GetAttachedCameraCount();
					for (int n = 0;n < camCnt;n++)
					{
						CString CameraName = pPPQObj->GetAttachedCamera(n);
						// find camera in camera list
						int iCam = GetCameraListCount();
						for (int c = 0;c < iCam;c++)
						{
							const SVOCameraObjPtr pCameraObj = m_CameraList.GetCameraObjectByPosition(c);
							if( nullptr != pCameraObj )
							{
								if (pCameraObj->GetCameraDisplayName() == CameraName)
								{
									bRetVal = pCameraObj->IsCameraLineInputSupported();
								}
							}
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
	BOOL bRet = false;

	if ( nullptr != m_slUsedInspect.Find(sInspect))
	{
		bRet = true;
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

	if ( iCam > 0 )
	{
		for (int iC = 0; iC < iCam; iC++)
		{
			const SVOCameraObjPtr pCameraObj( GetCameraObject(iC) );
			if( nullptr != pCameraObj )
			{
				if (!pCameraObj->IsCameraObjOk())
				{
					AddMessageToList(CAMERA_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCameraObj->GetCameraDisplayName(), CAMERA_INVALID_FILES));
				}
				CheckCamera( *pCameraObj );
			}
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
			const SVOPPQObjPtr pPPQObj = GetPPQObject(i);
			if ( nullptr != pPPQObj )
			{
				if ( pPPQObj->GetAttachedCameraCount() < 1 )
				{
					// no camera's attached
					AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName(), PPQ_NO_CAMERA));
				}
				if ( pPPQObj->GetAttachedInspectionCount() < 1 )
				{
					//no inspection's attached
					AddMessageToList(PPQ_DLG, BuildDisplayMessage(MESSAGE_TYPE_WARNING, pPPQObj->GetPPQName(), PPQ_NO_INSPECT));
				}
				if ( pPPQObj->GetAttachedTriggerName().IsEmpty() )
				{
					//no attached trigger
					AddMessageToList(PPQ_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName(), PPQ_NO_TRIGGER));
				}
				if ( pPPQObj->IsConditionalOutputCameraInput() && !IsCameraLineInputAllowed(pPPQObj->GetAttachedTriggerName()) )
				{
					// condition for output not valid
					AddMessageToList(PPQ_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName(), PPQ_CONDITIONAL_OUTPUT_INVALID));
				}
			}
		}
	}
}

BOOL CSVOConfigAssistantDlg::SendPPQDataToConfiguration(SVPPQObjectArray& aPPQsToDelete)
{
	BOOL bRet = true;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PPQs - pass 1 - detach
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	long lCfgPPQCnt = 0;
	//Count will be 0 if pointer nullptr
	if( nullptr != pConfig){ lCfgPPQCnt = pConfig->GetPPQCount(); } //should be zero on new cfg...
	//loop thru to see if the PPQ's still exist.  if not delete them...
	for (int iPPQ = lCfgPPQCnt -1; -1 < iPPQ; iPPQ--)
	{
		SVPPQObject* pPPQ = pConfig->GetPPQ(iPPQ);
		if ( nullptr != pPPQ )
		{
			if (!IsPPQInList(pPPQ->GetName()))
			{ //not there.  flag to delete it...
				aPPQsToDelete.Add(pPPQ);
				
				//PPQ is no longer there.  Detach everything from it.
				pPPQ->DetachAll();
				
				pConfig->RemovePPQ(pPPQ);	// remove from config now
				//delete pPPQ;	// do this AFTER destroying triggers, cameras, and INSPECTIONS
				// destroying the PPQ will also detatch it from triggers, cameras, and inspections
			}
			else
			{
				long lSize = 0;
				long lInsCnt;
				bRet = pPPQ->GetInspectionCount(lInsCnt);
				
				//loop thru inspections to see if they exist in dlg
				//if not, delete them from ppqobj
				for (long lI = lInsCnt-1; -1 < lI; lI--)
				{
					BOOL bDetachInspect = FALSE;
					SVInspectionProcess* pInspection( nullptr );
					bRet = pPPQ->GetInspection(lI, pInspection);
					if ( nullptr != pInspection)
					{
						const SVOInspectionObjPtr pTmpInspection( GetInspectionObjectByName(pInspection->GetName()) );
						if ( nullptr != pTmpInspection )
						{
							if ( (!IsInspectionOnPPQ(pPPQ->GetName(), pInspection->GetName())) ||
								(pTmpInspection->IsNewInspection()) )
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
							bRet = pPPQ->DetachInspection(pInspection) && bRet;
						}
					}
				}

				//do same for camera's

				std::deque< SVVirtualCamera* > l_Cameras;

				pPPQ->GetCameraList( l_Cameras );

				std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

				while( l_Iter != l_Cameras.end() )
				{
					SVVirtualCamera* pCamera = ( *l_Iter );

					if ( nullptr != pCamera )
					{
						if (!IsCameraOnPPQ(pPPQ->GetName(), pCamera->GetName()))
						{
							bRet = pPPQ->DetachCamera(pCamera, TRUE);
						}
					}

					++l_Iter;
				}

				//check trigger...
				SvTi::SVTriggerObject* pTrigger( nullptr );
				bRet = pPPQ->GetTrigger(pTrigger) && bRet;
				if ( nullptr != pTrigger )
				{
					if (!IsTriggerOnPPQ(pPPQ->GetName(), pTrigger->GetName()))
					{
						bRet = pPPQ->DetachTrigger(pTrigger) && bRet;
					}
				}
				pPPQ->RebuildOutputList();
			}
		}// if ppq != null
		else
		{
			bRet = FALSE;
		}
	} // for ppq
	return bRet;
}

BOOL CSVOConfigAssistantDlg::SendAcquisitionDataToConfiguration()
{
	BOOL bRet = true;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	ASSERT( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		bRet = false;
		return bRet;
	}

	SVLightReference* psvLight = nullptr;
	SVFileNameArrayClass* psvFiles = nullptr;
	SVLut* pLut = nullptr;
	SVDeviceParamCollection* pDeviceParams = nullptr;

	CString sAcquName;
	SVConfigurationObject::SVAcquisitionDeviceMap::iterator aPos = pConfig->GetAcquisitionDeviceStartPosition();
	while (aPos != pConfig->GetAcquisitionDeviceEndPosition())
	{
		//see if things are in the dlg...
		pConfig->GetAcquisitionDeviceNextAssoc( aPos, sAcquName );
		if (!IsDigitizerUsed(sAcquName))
		{
			SVAcquisitionClassPtr psvDevice;
			psvDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( sAcquName );
			pConfig->RemoveAcquisitionDevice(sAcquName);
			if( nullptr != psvDevice )
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

	for (int iAcq = 0; iAcq < iCamCnt; iAcq++)
	{
		svFiles.RemoveAll();
		int CameraIndex(iAcq);
		bool IsAcqDevice( false );
		SVAcquisitionClassPtr psvDevice;

		const SVOCameraObjPtr pCameraObj( GetCameraObject( CameraIndex ) );
		if( nullptr != pCameraObj )
		{
			int Digitizer = SVDigitizerProcessingClass::Instance().getDigitizerID( pCameraObj->GetCameraID() );
			if( -1 != Digitizer )
			{
				pCameraObj->SetDigNumber( Digitizer );
			}
			sDigName = BuildDigName( *pCameraObj );
			// For File Acquisition
			if ( pCameraObj->IsFileAcquisition())
			{
				psvDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( sDigName );
				if ( nullptr != psvDevice )
				{
					SVLightReference lightRef;
					SVLut lut;

					SVDeviceParamCollection deviceParams;

					_variant_t fileNameVar = pCameraObj->GetImageFilename();
					_variant_t dirNameVar = pCameraObj->GetImageDirectoryName();

					long loadingMode = pCameraObj->GetFileLoadingMode();
					_variant_t fileLoadingModeVar = loadingMode;
				
					_variant_t widthVar = pCameraObj->GetFileImageWidth();
					_variant_t heightVar = pCameraObj->GetFileImageHeight();

					// Only 2 formats supported 32Bit color and 8Bit grayscale
					long format = (pCameraObj->IsColor()) ? SVImageFormatRGB8888 : SVImageFormatMono8;
					_variant_t formatVar = format;

					deviceParams.CreateParameter(DeviceParamFileAcqImageFileName, fileNameVar);
					deviceParams.CreateParameter(DeviceParamFileAcqImageDirName, dirNameVar);
				
					if (loadingMode == SingleFileMode)
					{
						// File is supposed to be managed in Single File Mode
						SVFileNameClass svFile;
						svFile.SetFullFileName(pCameraObj->GetImageFilename());
						svFiles.Add(svFile);
					}
					deviceParams.CreateParameter(DeviceParamFileAcqLoadingMode, fileLoadingModeVar);
				
					deviceParams.CreateParameter(DeviceParamFileAcqImageWidth, widthVar);
					deviceParams.CreateParameter(DeviceParamFileAcqImageHeight, heightVar);
					deviceParams.CreateParameter(DeviceParamFileAcqImageFormat, formatVar);

					psvDevice->SetDeviceParameters( deviceParams );	// must be done before CreateBuffers

					// save the camera file (for future simulation mode)
					CString sCameraFileName = pCameraObj->GetCameraFile();
					svFile.SetFullFileName(sCameraFileName);
			
					if ( 0 == CString( svFile.GetExtension() ).CompareNoCase(SVGigeCameraFileDefExt) )
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
				sCameraFileName = pCameraObj->GetCameraFile();
				svFile.SetFullFileName(sCameraFileName);
			
				if ( 0 == CString( svFile.GetExtension() ).CompareNoCase(SVGigeCameraFileDefExt)  )
				{
					svFiles.Add(svFile);
				}

				psvLight = nullptr;
				psvDevice =  SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( sDigName );
				if ( nullptr != psvDevice )
				{
					bool bGetLightReference = false;
					if ( nullptr != psvDevice )
					{
						SVImageInfoClass svImageInfo;

						if ( pConfig->GetAcquisitionDevice( sDigName, psvFiles, psvLight, pLut, pDeviceParams ) )
						{
							psvDevice->DestroyBuffers();
							if( pCameraObj->GetCameraFileChanged() )
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

						svDeviceParams = pCameraObj->GetCameraDeviceParams();
						pDeviceParams = &svDeviceParams;

						if( pCameraObj->GetCameraFileChanged() )
						{
							psvDevice->LoadFiles(svFiles);

							if ( S_OK == psvDevice->LoadLightReference( svLight ) )
							{
								psvDevice->SetLightReference( svLight );
							}

							if ( svDeviceParams.ParameterExists( DeviceParamCameraFormats ) )
							{
								const SVCameraFormatsDeviceParam* pParam = svDeviceParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
								if ( pParam->SupportsColor() )
								{
									psvDevice->CreateLightReference(3);
								}
								else
								{
									psvDevice->CreateLightReference(1);
								}
							}
						}
						psvDevice->SetDeviceParameters( svDeviceParams );	// must be done before CreateBuffers (in case CameraFormat changes)
						psvDevice->GetDeviceParameters( svDeviceParams );	// LoadFiles may have created CameraFormat params which were not in the original list; retrieve the new complete list
						psvDevice->GetImageInfo(&svImageInfo);
						HRESULT hrCreateBuffers = psvDevice->CreateBuffers( svImageInfo, TheSVObserverApp.GetSourceImageDepth() );

						// needs to happen AFTER SetDeviceParameters on a New
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
					}// end if ( nullptr != psvDevice )
				}// end if ( TheSVObserverApp.mpsvImaging->GetAcquisitionDevice( sDigName, psvDevice ) )
				ASSERT ( nullptr != psvLight || nullptr != pLut || nullptr != pDeviceParams ); // something is wrong if we get here.

				if ( nullptr == psvLight )
				{
					psvLight = &svLight;    // assign dummy var
				}
				if ( nullptr == pLut )
				{
					pLut = &lut;    // assign dummy var
				}
				if ( nullptr == pDeviceParams )
				{
					pDeviceParams = &svDeviceParams;
				}

				bRet = pConfig->AddAcquisitionDevice( sDigName, svFiles, *psvLight, *pLut, pDeviceParams ) && bRet;
			}
		}
	}
	return bRet;
}

BOOL CSVOConfigAssistantDlg::SendCameraDataToConfiguration()
{
	BOOL bRet = true;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	ASSERT( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		bRet = false;
		return bRet;
	}

	SVVirtualCamera* pCamera( nullptr );
	long lCfgCamCnt = pConfig->GetCameraCount();

	//check to see if camera still exist...
	for (long l = lCfgCamCnt -1; -1 < l; l--)
	{
		pCamera = pConfig->GetCamera(l);
		if ( nullptr != pCamera )
		{
			if (!IsCameraInList(pCamera->GetName())) 
			{
				pConfig->RemoveCamera(pCamera);

				delete pCamera;
			}
			pCamera = nullptr;
		}
		else
		{
			bRet = FALSE;
		}
	}
	//go thru and add camera's
	int iCamCnt = m_CameraList.GetCameraListCount();
	if (iCamCnt > 0)
	{
		CString sKey;
		CString sName;

		for (int i = 0; i < iCamCnt; i++)
		{
			SVOCameraObjPtr pCameraObj = GetCameraObject(i);
			if( nullptr != pCameraObj )
			{
				sKey = pCameraObj->GetCameraDisplayName();
				lCfgCamCnt = pConfig->GetCameraCount();

				for ( long l = lCfgCamCnt - 1; -1 < l; l-- )
				{
					pCamera = pConfig->GetCamera(l);
					if ( nullptr != pCamera )
					{
						if (sKey.Compare(pCamera->GetName() ) == 0)
						{
							pCamera->Destroy();
							break;
						}
						pCamera = nullptr;
					}
					else
					{
						bRet = FALSE;
					}
				}

				BOOL bAddCamera = FALSE;

				if ( nullptr == pCamera )
				{
					pCamera = new SVVirtualCamera;
					pCamera->SetName( sKey );
					bRet = nullptr != pCamera && bRet;
					bAddCamera = TRUE;
				}

				if ( nullptr != pCamera )
				{
					// move from editing camera object to configuration camera object
					int Digitizer = SVDigitizerProcessingClass::Instance().getDigitizerID( pCameraObj->GetCameraID() );
					if( -1 != Digitizer )
					{
						pCameraObj->SetDigNumber( Digitizer );
					}
					pCamera->setCameraID( pCameraObj->GetCameraID() );
					pCamera->SetFileAcquisitionMode(pCameraObj->IsFileAcquisition());
					pCamera->SetFileLoadingMode(pCameraObj->GetFileLoadingMode());
					pCamera->SetImageFilename(pCameraObj->GetImageFilename());
					pCamera->SetImageDirectoryName(pCameraObj->GetImageDirectoryName());
					pCamera->SetFileImageSizeEditModeFileBased(pCameraObj->IsFileImageSizeEditModeFileBased());
					pCamera->SetFileImageSize(pCameraObj->GetFileImageSize());
					pCamera->SetIsColor( pCameraObj->IsColor() );

					CString sDevice;
					sDevice = BuildDigName( *pCameraObj );

					bRet = pCamera->Create( sDevice ) && bRet;

					if ( bAddCamera )
					{
						bRet = pConfig->AddCamera( pCamera ) && bRet;
					}
				}
				pCamera = nullptr;
			}
		}
	}

	return bRet;
}

BOOL CSVOConfigAssistantDlg::SendTriggerDataToConfiguration()
{
	BOOL bRet = true;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	ASSERT( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		bRet = false;
		return bRet;
	}

	long lCfgTrgCnt = pConfig->GetTriggerCount();

	// Check for Triggers removed
	SvTi::SVTriggerObject* pTrigger( nullptr );
	for ( long lT = lCfgTrgCnt - 1; -1 < lT; lT-- )
	{
		pTrigger = pConfig->GetTrigger(lT);
		bRet = (nullptr != pTrigger) && bRet;
		if ( nullptr != pTrigger && !IsTriggerInList(pTrigger->GetName()) )
		{
			pConfig->RemoveTrigger(pTrigger);
			delete pTrigger;
		}
		pTrigger = nullptr;
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
			const SvTi::SVOTriggerObjPtr pTriggerObj( GetTriggerObject(i) );
			if( nullptr != pTriggerObj )
			{
				sKey = pTriggerObj->GetTriggerDisplayName();
				sDeviceName = BuildTrgDig( *pTriggerObj );

				pTrigger = nullptr;
				lCfgTrgCnt = pConfig->GetTriggerCount();

				// Remove Triggers that have changed
				for ( long l = lCfgTrgCnt - 1; -1 < l; l-- )
				{
					pTrigger = pConfig->GetTrigger(l);
					if ( nullptr != pTrigger )
					{
						if ( sKey.Compare(pTrigger->GetName() ) == 0 )
						{
							pTrigger->Destroy();
							break;
						}
						pTrigger = nullptr;
					}
					else
					{
						bRet = FALSE;
					}
				}

				BOOL bAddTrigger = FALSE;

				if ( nullptr == pTrigger )
				{
					pTrigger = new SvTi::SVTriggerObject;
					pTrigger->SetName( sKey );
				
					bRet = nullptr != pTrigger && bRet;
					bAddTrigger = TRUE;
				}

				if ( nullptr != pTrigger )
				{
					if (pTriggerObj->IsSoftwareTrigger())
					{
						pTrigger->SetSoftwareTrigger(true);
						pTrigger->SetSoftwareTriggerPeriod(pTriggerObj->GetTimerPeriod());
					}
					else
					{
						pTrigger->SetSoftwareTrigger(false);
					}

					SvTh::SVTriggerClass* psvDevice = SvTi::SVTriggerProcessingClass::Instance().GetTrigger( sDeviceName );

					if ( nullptr != psvDevice )
					{
						bRet = pTrigger->Create(psvDevice) && bRet;
					}

					if ( bAddTrigger )
					{
						pTrigger->mpsvDevice->miChannelNumber = pTriggerObj->GetTriggerDigNumber();
						bRet = pConfig->AddTrigger(pTrigger) && bRet;
					}
				}
				pTrigger = nullptr;
			}
		}
	}
	return bRet;
}

BOOL CSVOConfigAssistantDlg::RenameInspectionObjects(CString sInspectionName, CString sNewInspectionName)
{
	BOOL bRet = TRUE;
	if( sInspectionName != sNewInspectionName )
	{
		// PPQ Object Inspection Name...
		long lCount = 0;
		lCount = m_PPQList.GetPPQListCount();
		for( int i = 0 ; i < lCount ; i++)
		{
			SVOPPQObjPtr pPPQObject = GetPPQObject(i);
			if( nullptr != pPPQObject )
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

SVOPPQObjPtr CSVOConfigAssistantDlg::GetPPQObjectByInspectionName(const SVString& inspectionName)
{
	bool bFound = false;
	SVOPPQObjPtr pPPQObj;

	long ppqCount = m_PPQList.GetPPQListCount();
	for (int i = 0; !bFound && i < ppqCount; i++)
	{
		pPPQObj =  GetPPQObject(i);
		if ( nullptr != pPPQObj )
		{
			int inspectionCount = pPPQObj->GetAttachedInspectionCount();
			for (int j = 0; !bFound && j < inspectionCount;j++)
			{
				// What is this key (label or name)?
				CString name = pPPQObj->GetAttachedInspection(j);
				if (name == inspectionName.c_str())
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
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	ASSERT( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		bRet = false;
		return bRet;
	}

	SVInspectionProcess* pInspection( nullptr );
	long lCfgInsCnt = pConfig->GetInspectionCount();

	for (long l1 = lCfgInsCnt -1; -1 < l1; l1--)
	{
		SVInspectionProcess* pInspection = pConfig->GetInspection(l1);
		if ( nullptr != pInspection )
		{
			const SVOInspectionObjPtr pInspectionObj( GetInspectionObjectByName( pInspection->GetName()) );
			BOOL bDeleteInspect = FALSE;

			if ( nullptr != pInspectionObj )
			{
				if ( (!IsInspectionInList(pInspection->GetName()))  || (pInspectionObj->IsNewInspection()) )
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
				pConfig->RemoveInspection(pInspection);
				delete pInspection;	// also destroys IPDoc
			}
			pInspection = nullptr;
		}
		else
		{
			bRet = FALSE;
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
			const SVOInspectionObjPtr pInspectionObj( GetInspectionObject(i) );
			if( nullptr != pInspectionObj )
			{
				sKey = pInspectionObj->GetInspectionLabelName();
				sFileName = pInspectionObj->GetInspectionName();
				sToolsetImage = pInspectionObj->GetToolsetImage();
				sNewDisableMethod = pInspectionObj->GetNewDisableMethod();
				lEnableAuxiliaryExtent = pInspectionObj->GetEnableAuxiliaryExtent();

				lCfgInsCnt = pConfig->GetInspectionCount();

				for ( long l = lCfgInsCnt -1; -1 < l; l-- )
				{
					pInspection = pConfig->GetInspection(l);
					if (nullptr != pInspection)
					{
						if ( sKey.Compare(pInspection->GetName()) == 0 )
						{
							pInspection->SetName(sFileName);

							pConfig->RenameOutputListInspectionNames( sKey, sFileName );

							TheSVObserverApp.OnUpdateAllIOViews();
							break;
						}
						pInspection = nullptr;
					}
					else
					{
						bRet = FALSE;
					}
				}

				BOOL bAddInspection = FALSE;

				if ( nullptr == pInspection )
				{
					const CString& importFilename = pInspectionObj->GetImportFilename();
					if (!importFilename.IsEmpty())
					{
						SVInspectionImportHelper importer  = SVInspectionImportHelper(SVString(importFilename), SVString(sFileName), SVString(sToolsetImage));
						SVString title = _T( "Importing Inspection..." );
						SVImportProgress<SVInspectionImportHelper> progress(importer, title.c_str());
						progress.DoModal();

						HRESULT hr = progress.GetStatus();
						if (S_OK == hr)
						{
							if (GUID_NULL != importer.info.m_inspectionGuid)
							{
								// Add info to list
								m_ImportedInspectionInfoList.push_back(importer.info);
								SVObjectClass* pObject(nullptr);
								SVObjectManagerClass::Instance().GetObjectByIdentifier(importer.info.m_inspectionGuid, pObject);

								bRet = (nullptr != pObject);

								if( bRet )
								{
									pInspection = dynamic_cast<SVInspectionProcess*> (pObject);
									RenameInspectionObjects(sKey, sFileName);
									bRet = pConfig->AddInspection( pInspection );

									SVOPPQObjPtr pPPQObj = GetPPQObjectByInspectionName(SVString(sFileName));
									if( nullptr != pPPQObj )
									{
										pPPQObj->SetImportedInputList(importer.info.m_inputList);
									}

									BOOL bNewDisableMethod = pInspection->GetNewDisableMethod();
									sNewDisableMethod = (bNewDisableMethod) ? _T( "Method 2" ): _T( "Method 1" );
									lEnableAuxiliaryExtent = pInspection->GetEnableAuxiliaryExtent();
								}
							}
							resolveGlobalConflicts( importer.GlobalConflicts );
						}
						else
						{
							SVStringArray msgList;
							msgList.push_back(SvUl_SF::Format(_T("%d"), hr));
							SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
							INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Config_InspectionImportFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10140);
						}
						pInspectionObj->ClearImportFilename();
					}
					else
					{
						SVObjectManagerClass::Instance().ConstructObject( SVInspectionProcessGuid, pInspection );

						bRet = (nullptr != pInspection);

						if( bRet )
						{
							pInspection->SetName( sFileName );
							RenameInspectionObjects(sKey, sFileName);

							pInspection->SetToolsetImage(sToolsetImage);

							bRet = pInspection->CreateInspection(sFileName);

							if( bRet )
							{
								bRet = pConfig->AddInspection( pInspection );
							}
						}
					}
				}

				if ( nullptr != pInspection )
				{
					pInspection->SetToolsetImage(sToolsetImage);

					pInspection->SetNewDisableMethod( sNewDisableMethod == _T( "Method 2" ) );

					long l_lPrevEnable = pInspection->GetEnableAuxiliaryExtent();
					pInspection->SetEnableAuxiliaryExtent( lEnableAuxiliaryExtent );
					if( l_lPrevEnable && lEnableAuxiliaryExtent == 0 )
					{
						pInspection->resetAllObjects(true, false);
					}
				}
				pInspection = nullptr;
			}
		}
	}
	return bRet;
}

BOOL CSVOConfigAssistantDlg::SendPPQAttachmentsToConfiguration(SVPPQObjectArray& aPPQsToDelete)
{
	BOOL bRet = true;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	ASSERT( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		bRet = false;
		return bRet;
	}

	SVInspectionProcess* pInspection( nullptr );

	// delete old PPQs
	for (int i = 0; i < aPPQsToDelete.GetSize(); i++)
	{
		SVPPQObject* pPPQ = aPPQsToDelete.GetAt(i);
		delete pPPQ;
	}
	aPPQsToDelete.RemoveAll();

	long lCPPQCnt = pConfig->GetPPQCount();

	int iPPQCnt = m_PPQList.GetPPQListCount();
	if ( iPPQCnt > 0) 
	{
		CString sKey;
		SVPPQObject* pPPQ( nullptr );

		for (int i = 0; i < iPPQCnt; i++)
		{
			SVOPPQObjPtr pPPQObj = GetPPQObject(i);
			if( nullptr == pPPQObj)
			{
				continue;
			}

			sKey = pPPQObj->GetPPQName();
			BOOL bNew = FALSE;

			for ( long l = lCPPQCnt -1; -1 < l; l-- )
			{
				pPPQ = pConfig->GetPPQ(l);
				
				if ( nullptr != pPPQ )
				{
					if ( sKey.Compare(pPPQ->GetName()) == 0 )
					{
						break;
					}
					
					pPPQ = nullptr;
				}
				else
				{
					bRet = FALSE;
				}
			}

			if ( nullptr == pPPQ )
			{
				pPPQ = new SVPPQObject;
				pPPQ->SetName(sKey);
				bNew = TRUE;
			}

			if ( nullptr != pPPQ )
			{
				CString sNewName;
				CString sCfgCameraName;
				CString sPPQCameraName;
				long lSize = 0;

				bRet = pPPQ->SetPPQOutputMode((SVPPQOutputModeEnum)pPPQObj->GetPPQMode()) && bRet;

				// EB 20050225
				long lOldPPQLength=0;
				pPPQ->GetPPQLength( lOldPPQLength );
				bool bPPQLengthChanged = lOldPPQLength != pPPQObj->GetPPQLength();

				bRet = pPPQ->SetPPQLength(pPPQObj->GetPPQLength()) && bRet;
				bRet = pPPQ->SetResetDelay(pPPQObj->GetPPQOutputResetDelay()) && bRet;
				bRet = pPPQ->SetOutputDelay(pPPQObj->GetPPQOutputDelayTime()) && bRet;
				bRet = pPPQ->SetMaintainSourceImages( pPPQObj->GetMaintainSourceImageProperty() ) && bRet;
				bRet = pPPQ->SetInspectionTimeout( pPPQObj->GetInspectionTimeout() ) && bRet;
				pPPQ->SetConditionalOutputName( pPPQObj->GetConditionalOutputName() );

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

						pPPQ->GetCameraList( l_Cameras );

						std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

						while( l_Iter != l_Cameras.end() )
						{
							SVVirtualCamera* pCamera = ( *l_Iter );

							if ( nullptr != pCamera )
							{
								if ( sPPQCameraName.Compare(pCamera->GetName()) == 0 )
								{
									pPPQ->GetCameraPPQPosition( lPosition, pCamera );

									pPPQ->DetachCamera( pCamera );

									break;
								}

								pCamera = nullptr;
							}

							++l_Iter;
						}

						long lCfgAttachedCam = pConfig->GetCameraCount();

						for ( l = lCfgAttachedCam -1; -1 < l; l-- )
						{
							SVVirtualCamera* pCamera = pConfig->GetCamera(l);
							if ( nullptr != pCamera )
							{
								if ( sPPQCameraName.Compare(pCamera->GetName()) == 0 )
								{
									pPPQ->AttachCamera(pCamera,lPosition);
									break;
								}
								pCamera = nullptr;
							}
							else
							{
								bRet = FALSE;
							}
						}
					}
				}

				CString sPPQTrigger;

				sPPQTrigger = pPPQObj->GetAttachedTriggerName();

				if ( !sPPQTrigger.IsEmpty() )
				{
					SvTi::SVTriggerObject* pTrigger( nullptr );
					lSize = pConfig->GetTriggerCount();
					for (long l = lSize -1; -1 < l; l--)
					{
						pTrigger = pConfig->GetTrigger(l);
						if ( nullptr != pTrigger )
						{
							if ( sPPQTrigger.Compare(pTrigger->GetName()) == 0 )
							{
								pPPQ->AttachTrigger(pTrigger);
								break;
							}

							pTrigger = nullptr;
						}
						else
						{
							bRet = FALSE;
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

						sPpqInsStr = pPPQObj->GetAttachedInspection(i);
						const SVOInspectionObjPtr pInspectionObj = GetInspectionObjectByName(sPpqInsStr);
						pPPQ->GetInspectionCount(lInsCnt);

						if( nullptr != pInspectionObj )
						{
							for (long l = lInsCnt -1; -1 < l; l--)
							{
								bRet = pPPQ->GetInspection(l,pInspection) && bRet;
								
								if ( ( nullptr != pInspection ) && (!pInspectionObj->IsNewInspection()) )
								{
									if ( sPpqInsStr.Compare(pInspection->GetName()) == 0 )
									{
										bFound = TRUE;
										break;
									}
									pInspection = nullptr;
								}
							}
						}

						if ( !bFound )
						{
							lInsCnt = pConfig->GetInspectionCount();

							for ( long l = lInsCnt -1; -1 < l; l-- )
							{
								pInspection = pConfig->GetInspection(l);
								if ( nullptr != pInspection )
								{
									if ( sPpqInsStr.Compare(pInspection->GetName()) == 0 )
									{
										pPPQ->AttachInspection(pInspection);

										break;
									}
									pInspection = nullptr;
								}
								else
								{
									bRet = FALSE;
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
						pConfig->AddImportedRemoteInput(pPPQ, input.name, input.ppqPosition, input.index, input.value);
					}
					else
					{
						const SVImportedInput& input = boost::any_cast<const SVImportedInput &>(*it);
						pConfig->AddImportedDigitalInput(pPPQ, input.name, input.ppqPosition);
					}
				}
				// cleanup so we don't do it again
				pPPQObj->ClearImportedInputList();

				if (bNew)
				{
					bRet = pPPQ->Create() && bRet;
					pPPQ->m_pInputList = pConfig->GetInputObjectList( );
					pPPQ->m_pOutputList = pConfig->GetOutputObjectList( );
					bRet = pConfig->AddPPQ(pPPQ) && bRet;
				}
				else
				{
					bRet = pPPQ->Rebuild() && bRet;
				}
				pPPQ = nullptr;
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

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	ASSERT( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		bRet = false;
		return bRet;
	}

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

	SVInputObjectList* pInputObjectList = pConfig->GetInputObjectList();
	if( nullptr == pInputObjectList )
	{
		pInputObjectList = new SVInputObjectList;
		pInputObjectList->SetName( _T( "Input Object List" ) );
		bRet &= nullptr != pInputObjectList && pInputObjectList->Create() && bRet;
	}

	SVOutputObjectList* pOutputObjectList = pConfig->GetOutputObjectList();
	if( nullptr == pOutputObjectList )
	{
		pOutputObjectList = new SVOutputObjectList;
		pOutputObjectList->SetName( _T( "Output Object List" ) );
		bRet &= nullptr != pOutputObjectList && pOutputObjectList->Create() && bRet;
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

	// go through each inspection, if HasInspectionNameChanged
	// send messages to all objects...
	long lInsCfgCnt = pConfig->GetInspectionCount();

	for (long lI = lInsCfgCnt -1; -1 < lI; lI--)
	{
		SVInspectionProcess* pInspection = pConfig->GetInspection(lI);
		SVOInspectionObjPtr pTmpObj;
		//If pInspection is nullptr then pTmpObj will also be nullptr
		if( nullptr != pInspection ){ pTmpObj = GetInspectionObjectByName(pInspection->GetName()); }

		if( nullptr != pTmpObj )
		{
			if (pTmpObj->HasInspectionNameChange())
			{
				pInspection->OnObjectRenamed(*pInspection, SVString(pTmpObj->GetOrginalInspectionName()));
				SVPPQObject* pPPQ( nullptr );
				SVOutputObjectList* pOutputObjList = pConfig->GetOutputObjectList();

				if (nullptr != pOutputObjList)
				{
					pOutputObjList->OnObjectRenamed(*pInspection, SVString(pTmpObj->GetOrginalInspectionName()));
				}

				long lCount = pConfig->GetPPQCount();
				for (long lPPQcount = 0; lPPQcount < lCount; lPPQcount++)
				{
					pPPQ = pConfig->GetPPQ(lPPQcount);
					if (nullptr != pPPQ)
					{
						pPPQ->OnObjectRenamed(*pInspection, SVString(pTmpObj->GetOrginalInspectionName()));
					} //if (pPPQ)
				} //for ppqcount
			} // if name has changed
		}//if pTmpObj
	}//for each inspection

	if ( m_bNewConfiguration )
	{
		pConfig->SetConfigurationLoaded();
	}
	return bRet;
}

BOOL CSVOConfigAssistantDlg::GetConfigurationForExisting()
{
	BOOL bRet = TRUE;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	ASSERT( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		bRet = false;
		return bRet;
	}

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
	int CameraID;
	int iChannel;
	CString sCameraFileName;

	SVVirtualCamera* pcfgCamera( nullptr );
	SvTi::SVTriggerObject* pcfgTrigger( nullptr );
	SVInspectionProcess* pcfgInspection( nullptr );
	SVPPQObject* pcfgPPQ( nullptr );

	m_lConfigurationType = pConfig->GetProductType();
	//load camera information
	lCfgCameraCnt = pConfig->GetCameraCount();
	for (long lCam = lCfgCameraCnt -1; -1 < lCam; lCam--)
	{
		pcfgCamera = pConfig->GetCamera(lCam);
		if ( nullptr != pcfgCamera )
		{
			sCameraName = pcfgCamera->GetName();
			if ( nullptr != pcfgCamera->mpsvDevice )
			{
				sDigName = pcfgCamera->mpsvDevice->DigName();
				iDigNumber = pcfgCamera->mpsvDevice->DigNumber();
				iChannel = pcfgCamera->mpsvDevice->Channel();
				CameraID = pcfgCamera->getCameraID();

				sCameraFileName.Empty();
				
				long lSize;
				SVFileNameClass oCamFile;
				pcfgCamera->mpsvDevice->GetFileNameArraySize( lSize );
				for ( long lFile = 0; lFile < lSize && sCameraFileName.IsEmpty();lFile++)
				{
					pcfgCamera->mpsvDevice->GetFileName( lFile, oCamFile );
					CString csExt = oCamFile.GetExtension();				

					if ( 0 == csExt.CompareNoCase( SVGigeCameraFileDefExt ) && sCameraFileName.IsEmpty() )
					{
						sCameraFileName = oCamFile.GetFullFileName();
					}
				}

				//Add Camera Name, Camera Files and Camera Device Params to tmp list in case of a cancel
				m_CameraList.AddCameraToList(sCameraName,iDigNumber,iChannel, CameraID); 
				m_TmpCameraList.AddCameraToList(sCameraName,iDigNumber,iChannel, CameraID); 
				m_CameraList.SetCameraFile(sCameraName, sCameraFileName);
				m_TmpCameraList.SetCameraFile(sCameraName, sCameraFileName);

				SVDeviceParamCollection l_Params;
				SVDeviceParamCollection l_CameraFileParams;
				pcfgCamera->mpsvDevice->GetDeviceParameters( l_Params );
				pcfgCamera->mpsvDevice->GetCameraFileParameters( l_CameraFileParams );

				m_CameraList.SetCameraDeviceParams( sCameraName, l_Params, l_CameraFileParams );
				m_TmpCameraList.SetCameraDeviceParams( sCameraName, l_Params, l_CameraFileParams );

				SVOCameraObjPtr pCameraObj = m_CameraList.GetCameraObjectByName(sCameraName);
				if( nullptr != pCameraObj )
				{
					// move from configuration (SVVirtualCamera) camera object to editing (CSVOCameraObj) camera object
					pCameraObj->SetFileAcquisitionMode(pcfgCamera->IsFileAcquisition());
					pCameraObj->SetFileLoadingMode(pcfgCamera->GetFileLoadingMode());
					pCameraObj->SetImageFilename(pcfgCamera->GetImageFilename());
					pCameraObj->SetImageDirectoryName(pcfgCamera->GetImageDirectoryName());
					pCameraObj->SetFileImageSizeEditModeFileBased(pcfgCamera->IsFileImageSizeEditModeFileBased());
					pCameraObj->SetFileImageSize(pcfgCamera->GetFileImageSize());
					pCameraObj->SetIsColor( pcfgCamera->IsColor() );
				}
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	//end of load camera section

	//load trigger section...
	lCfgTriggerCnt = pConfig->GetTriggerCount();
	for (long lTrg = lCfgTriggerCnt -1; -1 < lTrg; lTrg--)
	{
		pcfgTrigger = pConfig->GetTrigger(lTrg);
		bRet = (nullptr != pcfgTrigger) && bRet;
		if ( nullptr != pcfgTrigger && nullptr != pcfgTrigger->mpsvDevice )
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

			// Add Software trigger flag and interval here
			const SvTi::SVOTriggerObjPtr pTriggerObj( m_TriggerList.GetTriggerObjectByName(SVString(sTriggerName)) );
			if( nullptr != pTriggerObj )
			{
				pTriggerObj->SetSoftwareTrigger(pcfgTrigger->IsSoftwareTrigger());
				pTriggerObj->SetTimerPeriod(pcfgTrigger->GetSoftwareTriggerPeriod());
			}
		}
	}
	//end of trigger section
	
	//load inspection section...
	CString sDisable;
	lCfgInspectCnt = pConfig->GetInspectionCount();
	for (long lIns = lCfgInspectCnt -1; -1 < lIns; lIns--)
	{
		pcfgInspection = pConfig->GetInspection(lIns);
		if ( nullptr != pcfgInspection )
		{
			sInspectLabel = pcfgInspection->GetName();  // label name
			sInspectName = pcfgInspection->GetName();

			m_InspectList.AddInspectionToList(sInspectLabel,sInspectName, false);
			m_slInspectionNamesUsed.AddTail(sInspectName);
			m_slInspectionLabelsUsed.AddTail(sInspectLabel);

			// Determine which image is the main image
			CString sToolsetImage;
			sToolsetImage = pcfgInspection->GetToolsetImage();
			m_InspectList.SetToolsetImage( sInspectLabel, sToolsetImage );
			
			m_InspectList.SetColor( sInspectLabel, pcfgInspection->IsColorCamera() );

			if( pcfgInspection->GetNewDisableMethod() )
			{
				sDisable = _T( "Method 2" );
			}// end if
			else
			{
				sDisable = _T( "Method 1" );
			}// end else

			m_InspectList.SetNewDisableMethod( sInspectLabel, sDisable );

			m_InspectList.SetShowAuxExtent( sInspectLabel );

			// Enable Auxiliary Extent
			long l_lEnable = 0;
			if( pcfgInspection->GetEnableAuxiliaryExtent() )
			{
				l_lEnable = 1;
			}
			else
			{
				l_lEnable = 0;
			}
			m_InspectList.SetEnableAuxiliaryExtent( sInspectLabel, l_lEnable );
		}
		else
		{
			bRet = FALSE;
		}
	}
	//end of inspection section...
	
	//load PPQ section...
	lCfgPPQCnt = pConfig->GetPPQCount();

	for (long lPpq = lCfgPPQCnt -1; -1 < lPpq; lPpq--)
	{
		pcfgPPQ = pConfig->GetPPQ(lPpq);
		if ( nullptr != pcfgPPQ )
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

				if ( nullptr != pcfgCamera && (nullptr != pcfgCamera->mpsvDevice) )
				{
					m_PPQList.AttachCameraToPPQ(sPPQName,pcfgCamera->GetName());
				}

				++l_Iter;
			}

			for (long lpIns = lppqIns -1; -1 < lpIns; lpIns--)
			{
				pcfgInspection = nullptr;
				bRet = pcfgPPQ->GetInspection(lpIns,pcfgInspection) && bRet;
				if ( nullptr != pcfgInspection )
				{
					m_PPQList.AttachInspectToPPQ(sPPQName,pcfgInspection->GetName());
					AddUsedInspect(pcfgInspection->GetName());
				}
			}

			pcfgTrigger = nullptr;
			bRet = pcfgPPQ->GetTrigger(pcfgTrigger) && bRet;
			if ( (nullptr != pcfgTrigger ) && (nullptr != pcfgTrigger->mpsvDevice) )
			{
				m_PPQList.AttachTriggerToPPQ(sPPQName,pcfgTrigger->GetName());
				AddUsedTrigger(pcfgTrigger->GetName());
			}
			//set properties for PPQ

			SVOPPQObjPtr pPPQObj( GetPPQObjectByName(sPPQName) );
			if( nullptr != pPPQObj )
			{
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

				pPPQObj->SetPPQLength((int)lPPQLength);
				pPPQObj->SetPPQMode( (int)ePPQMode);
				pPPQObj->SetPPQOutputDelayTime((int)lPPQDelayTime);
				pPPQObj->SetPPQOutputResetDelay((int)lPPQResetDelay);
				pPPQObj->SetMaintainSourceImageProperty(bPPQMaintainSrcImg);
				pPPQObj->SetInspectionTimeout( lInspectionTimeout );
				pPPQObj->SetConditionalOutputName(pcfgPPQ->GetConditionalOutputName());

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
									// Only if the camera supports it ?
									availableInputs.push_back(std::make_pair(entry->m_pValueObject->GetName(), entry->m_IOId));
								}
							}
						}
					}
					// make list of Name/Guid pairs
					pPPQObj->SetAvailableInputsForConditionalOutput(availableInputs);
				}
			}
		}//end if nullptr != pcfgPPQ
		else
		{
			bRet = FALSE;
		}
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
	SVOCameraObjPtr pCameraObj;
	SVOInspectionObjPtr pInspectionObj;
	SVOPPQObjPtr pPPQObj;
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
					pCameraObj = GetCameraObjectByName(sLabelName);
					if( nullptr != pCameraObj )
					{
						if (!pCameraObj->IsCameraObjOk())
						{   // check to see if camera files are valid.
							// if not, add message to list.
							sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCameraObj->GetCameraDisplayName(), CAMERA_INVALID_FILES);
							AddMessageToList(CAMERA_DLG,sMessage);
						}
					}
					break;
				}// end case ITEM_ACTION_NEW:

				case ITEM_ACTION_DELETE:
				{
					RemoveMessageFromList(BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, INVALID_CAMERA));

					RemoveAllObjectMessages(sLabelName);
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
					pCameraObj = GetCameraObjectByName(sLabelName);

					if( nullptr != pCameraObj )
					{
						RemoveFileAcquisitionMessages( pCameraObj->GetCameraDisplayName() );

						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCameraObj->GetCameraDisplayName(), CAMERA_INVALID_FILES);

						if (pCameraObj->IsCameraObjOk())
						{
							RemoveMessageFromList(sMessage);
						
							// File Acquisition
							if (pCameraObj->IsFileAcquisition())
							{
								CheckCamera( *pCameraObj );
								CheckTriggers();
								CheckColor( *pCameraObj );
							}
							else
							{
								if ( SvTi::SVHardwareManifest::IsDigitalSVIM( m_lConfigurationType ) )
								{
									CheckCamera( *pCameraObj, true );
									CheckTriggers();
									CheckColor( *pCameraObj );
								}
							}
						}
						else
						{
							AddMessageToList(CAMERA_DLG, sMessage);
						}
					}
					break;
				}// end case ITEM_ACTION_PROP:

				case ITEM_ACTION_REFRESH:
				{
					int iCam = GetCameraListCount();

					for (int iC = 0; iC < iCam; iC++)
					{
						const SVOCameraObjPtr pCameraObj( m_CameraList.GetCameraObjectByPosition( iC ) );
						if( nullptr != pCameraObj )
						{
							CheckCamera( *pCameraObj );
							CheckColor( *pCameraObj );
						}
					}
					break;
				}

				case ITEM_ACTION_SAVE:
				{
					SendAcquisitionDataToConfiguration();
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
					RemoveMessageFromList(BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, INVALID_TRIGGER));
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
					pInspectionObj = GetInspectionObjectByName(sLabelName);
					if( nullptr != pInspectionObj )
					{
						pInspectionObj->SetNewInspection();
					}
					if (iObjCnt == 1)
					{
						m_bInspectionGood = TRUE;
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", INSPECT_NONE_DEFINED);
						RemoveMessageFromList(sMessage);
					}
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, INSPECTION_ERROR);
					AddMessageToList(PPQ_DLG,sMessage);
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
					RemoveMessageFromList(sMessage);
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, TOOLSET_IMAGE_ERROR);
					RemoveMessageFromList(sMessage);
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, MESSAGE_INSPECTION_CAMERA_COLOR);
					RemoveMessageFromList( sMessage );
					InspectionDeletedCheckAgainstPPQ(sLabelName);
					break;
				}// end case ITEM_ACTION_DELETE:

				case ITEM_ACTION_PROP:
				{
					pInspectionObj = GetInspectionObjectByName(sLabelName);
					if( nullptr != pInspectionObj )
					{
						CString sInspect( pInspectionObj->GetInspectionName() );
						CString sToolsetImage( pInspectionObj->GetToolsetImage() );
						if( !sToolsetImage.IsEmpty() )
						{
							RemoveMessageFromList(BuildDisplayMessage(MESSAGE_TYPE_ERROR, sInspect, TOOLSET_IMAGE_ERROR));
						}
						pCameraObj = GetCameraObjectByName(sToolsetImage);
						if( nullptr != pCameraObj )
						{
							CheckColor( *pCameraObj );
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
					AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, PPQ_NO_TRIGGER));
					AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, PPQ_NO_CAMERA));
					AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_WARNING, sLabelName, PPQ_NO_INSPECT));
					break;
				}
				case ITEM_ACTION_DELETE:
				{
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, PPQ_NO_TRIGGER);
					RemoveMessageFromList(sMessage);
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, PPQ_NO_CAMERA);
					RemoveMessageFromList(sMessage);
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
					pPPQObj = GetPPQObjectByName(sLabelName);
					if (pPPQObj)
					{
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName(), PPQ_NO_TRIGGER);
						RemoveMessageFromList(sMessage);
					}
					break;
				}
				case ITEM_PPQ_DEL_TRG:
				{
					pPPQObj = GetPPQObjectByName(sLabelName);
					if (pPPQObj)
					{
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName(), PPQ_NO_TRIGGER);
						AddMessageToList(PPQ_DLG,sMessage);
					}
					break;
				}
				case ITEM_PPQ_ADD_CAM:
				{
					pPPQObj = GetPPQObjectByName(sLabelName);
					if (pPPQObj)
					{
						sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName(), PPQ_NO_CAMERA);
						RemoveMessageFromList(sMessage);
					}
					break;
				}
				case ITEM_PPQ_DEL_CAM:
				{
					pPPQObj = GetPPQObjectByName(sLabelName);
					if (pPPQObj)
					{
						if ( pPPQObj->GetAttachedCameraCount() < 1 )
						{
							sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName(), PPQ_NO_CAMERA);
							AddMessageToList(PPQ_DLG,sMessage);
						}
						CameraDetachedCheckAgainstInspection( sLabelName );
					}
					break;
				}
				case ITEM_PPQ_ADD_INS:
				{
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, INSPECTION_ERROR);
					RemoveMessageFromList(sMessage);
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, sLabelName, PPQ_NO_INSPECT);
					RemoveMessageFromList(sMessage);
					
					AddMessageToList(INSPECT_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, TOOLSET_IMAGE_ERROR));

					break;
				}
				case ITEM_PPQ_DEL_INS:
				{
					CString sInsLabelName = GetInspectionLabelFromName(sLabelName);
					pInspectionObj = GetInspectionObjectByName(sInsLabelName);
					if( nullptr != pInspectionObj )
					{
						pInspectionObj->SetToolsetImage(_T(""));
					}
					sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, sLabelName, INSPECTION_ERROR);
					AddMessageToList(PPQ_DLG,sMessage);
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
					pPPQObj = GetPPQObjectByName(sLabelName);
					if ( pPPQObj )
					{
						long l_lPpqLength = pPPQObj->GetPPQLength();
						long l_lImageDepth = TheSVObserverApp.GetMaxPPQLength(); // GetSourceImageDepth() - 2;
						if ( pPPQObj->GetMaintainSourceImageProperty() )
						{
							if ( l_lPpqLength <= l_lImageDepth )
							{
								// remove error message if exist
								CString l_strError;
								l_strError.Format("%s%d",PPQ_PROP_SRC_IMG_ERROR,l_lImageDepth);
								//create error display message
								RemoveMessageFromList(BuildDisplayMessage( MESSAGE_TYPE_ERROR, sLabelName, l_strError ) );
							}
							else
							{
								// add error message if it does not exist
								CString l_strError;
								l_strError.Format("%s%d",PPQ_PROP_SRC_IMG_ERROR,l_lImageDepth);
								//create error display message
								AddMessageToList( PPQ_DLG, BuildDisplayMessage( MESSAGE_TYPE_ERROR, sLabelName, l_strError ) );
							}
						}
					}
					break;
				}
				case ITEM_PPQ_PROP_SRC_IMG:
				{
					//special property case for the PPQ's
					pPPQObj = GetPPQObjectByName(sLabelName);
					if ( pPPQObj )
					{
						if ( pPPQObj->GetMaintainSourceImageProperty() )
						{
							long l_lPpqLength = pPPQObj->GetPPQLength();
							long l_lImageDepth = TheSVObserverApp.GetMaxPPQLength(); //GetSourceImageDepth() - 2;
							if ( l_lPpqLength > l_lImageDepth )
							{
								CString l_strError;
								l_strError.Format("%s%d",PPQ_PROP_SRC_IMG_ERROR,l_lImageDepth);
								//create error display message
								AddMessageToList( PPQ_DLG, BuildDisplayMessage( MESSAGE_TYPE_ERROR, sLabelName, l_strError ) );
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
							RemoveMessageFromList(BuildDisplayMessage( MESSAGE_TYPE_ERROR, sLabelName, l_strError ) );
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

	for (int i = 0; i < iPPQCnt; i++)
	{
		SVOPPQObjPtr pPPQObj( GetPPQObject(i) );

		if( nullptr != pPPQObj && pPPQObj->GetAttachedTriggerName().Compare(sTriggerName) == 0)
		{
			pPPQObj->DetachTriggerFromPPQ();
			pPPQObj->RemoveCameraInputConditionalOutput();
			CString sMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName(), PPQ_NO_TRIGGER);
			AddMessageToList(PPQ_DLG,sMessage);
		}
	}
}

void CSVOConfigAssistantDlg::CameraDeletedCheckAgainstPPQ(CString sCameraName)
{
	int iPPQCnt = GetPPQListCount();
	int iCamCnt;
	CString sCam;

	for (int i = 0; i < iPPQCnt; i++)
	{
		SVOPPQObjPtr pPPQObj( GetPPQObject(i) );
		if( nullptr == pPPQObj )
		{
			continue;
		}

		iCamCnt = pPPQObj->GetAttachedCameraCount();
		for (int iC = 0; iC < iCamCnt; iC++)
		{
			if (pPPQObj->GetAttachedCamera(iC).Compare(sCameraName) == 0)
			{
				pPPQObj->DetachCameraFromPPQ(sCameraName);
				pPPQObj->RemoveCameraInputConditionalOutput();
				iCamCnt--;
			}
		}
		if (iCamCnt == 0)
		{
			AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName(), PPQ_NO_CAMERA));
		}
	}
}

void CSVOConfigAssistantDlg::CameraDeletedCheckAgainstInspection(CString sCameraName)
{
	long lSize;
	long l;

	lSize = GetInspectionListCount();
	for( l = 0; l < lSize; l++ )
	{
		const SVOInspectionObjPtr pInspectionObj( GetInspectionObject( l ) );
		
		if( nullptr != pInspectionObj && pInspectionObj->GetToolsetImage().IsEmpty() )
		{
			//Need to remove this message if in the list after Tool Set image has been reseted
			CString MessageColor( BuildDisplayMessage(MESSAGE_TYPE_ERROR, pInspectionObj->GetInspectionName(), MESSAGE_INSPECTION_CAMERA_COLOR) );
			RemoveMessageFromList( MessageColor );

			AddMessageToList( INSPECT_DLG, BuildDisplayMessage( MESSAGE_TYPE_ERROR, pInspectionObj->GetInspectionName(), TOOLSET_IMAGE_ERROR ) );
		}// end if
	}// end for
}// end CameraDeletedCheckAgainstInspection

void CSVOConfigAssistantDlg::CameraDetachedCheckAgainstInspection(CString sCameraName)
{
	long lSize;
	long l;

	lSize = GetInspectionListCount();
	for( l = 0; l < lSize; l++ )
	{
		const SVOInspectionObjPtr pInspectionObj( GetInspectionObject( l ) );

		if( nullptr != pInspectionObj && pInspectionObj->GetToolsetImage().IsEmpty() )
		{
			//Need to remove this message if in the list after Tool Set image has been reseted
			CString MessageColor( BuildDisplayMessage(MESSAGE_TYPE_ERROR, pInspectionObj->GetInspectionName(), MESSAGE_INSPECTION_CAMERA_COLOR) );
			RemoveMessageFromList( MessageColor );

			AddMessageToList( INSPECT_DLG, BuildDisplayMessage( MESSAGE_TYPE_ERROR, pInspectionObj->GetInspectionName(), TOOLSET_IMAGE_ERROR ) );
		}// end if
	}// end for
}// end CameraDetachedCheckAgainstInspection

void CSVOConfigAssistantDlg::InspectionDeletedCheckAgainstPPQ(CString sInspectName)
{
	int iPPQCnt = GetPPQListCount();
	int iInsCnt;
	CString sCam;

	for (int i = 0; i < iPPQCnt; i++)
	{
		SVOPPQObjPtr pPPQObj( GetPPQObject(i) );
		if( nullptr != pPPQObj )
		{
			iInsCnt = pPPQObj->GetAttachedInspectionCount();

			for (int iC = 0; iC < iInsCnt; iC++)
			{
				if (pPPQObj->GetAttachedInspection(iC).Compare(sInspectName) == 0)
				{
					pPPQObj->DetachInspectionFromPPQ(sInspectName);
					iInsCnt--;
				}
			}
			if (iInsCnt == 0)
			{
				AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_WARNING, pPPQObj->GetPPQName(), PPQ_NO_INSPECT));
			}
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

	const SvTi::SVIMTypeInfoStruct& info = SvTi::SVHardwareManifest::GetSVIMTypeInfo(m_lConfigurationType);

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
	const SvTi::SVIMTypeInfoStruct& info = SvTi::SVHardwareManifest::GetSVIMTypeInfo(m_lConfigurationType);

	int iNumberDigitizersAllowed = info.m_MaxDigitizers;
	int iNumberTriggersAllowed = info.m_MaxTriggers;

	// Clear Messages
	RemoveAllDlgMessages(CAMERA_DLG);
	RemoveAllDlgMessages(TRIGGER_DLG);

	if (iNumberDigitizersAllowed)
	{
		for (int i = 0; i < m_CameraList.GetCameraListCount(); i++)
		{
			const SVOCameraObjPtr pCameraObj( m_CameraList.GetCameraObjectByPosition(i) );
			if( nullptr != pCameraObj )
			{
				if (pCameraObj->GetDigNumber() > iNumberDigitizersAllowed-1)
				{
					AddMessageToList(CAMERA_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCameraObj->GetCameraDisplayName(), INVALID_CAMERA) );
				}
			}
		}
	}
	if (iNumberTriggersAllowed)
	{
		for (int i = 0; i <  m_TriggerList.GetTriggerListCount(); i++)
		{
			const SvTi::SVOTriggerObjPtr pTriggerObj( m_TriggerList.GetTriggerObjectByPosition(i) );
			if ( nullptr != pTriggerObj && pTriggerObj->GetTriggerDigNumber() > iNumberTriggersAllowed-1 )
			{
				AddMessageToList(TRIGGER_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, CString(pTriggerObj->GetTriggerDisplayName()), INVALID_TRIGGER) );
			}
		}
	}
	m_Page1.SetupList();
	m_Page2.SetupList();
}

BOOL CSVOConfigAssistantDlg::IsInspectionOnPPQ(CString sPPQName, CString sInspectionLabel)
{
	BOOL bFound = FALSE;
	const SVOPPQObjPtr pPPQObj( GetPPQObjectByName(sPPQName) );

	if( nullptr != pPPQObj )
	{
		int iInsCnt = pPPQObj->GetAttachedInspectionCount();

		for (int i = 0; ((i < iInsCnt) && (!bFound)); i++)
		{
			if (sInspectionLabel.CompareNoCase(pPPQObj->GetAttachedInspection(i)) == 0)
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
	const SVOPPQObjPtr pPPQObj( GetPPQObjectByName(sPPQName) );

	if( nullptr != pPPQObj )
	{
		int iCamCnt = pPPQObj->GetAttachedCameraCount();

		for (int i = 0; ( (i < iCamCnt) && (!bFound) ); i++)
		{
			if (sCameraName.CompareNoCase(pPPQObj->GetAttachedCamera(i)) == 0)
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
	const SVOPPQObjPtr pPPQObj( GetPPQObjectByName(sPPQName) );

	if( nullptr != pPPQObj )
	{
		if ( sTriggerName.CompareNoCase(pPPQObj->GetAttachedTriggerName()) == 0 )
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

BOOL CSVOConfigAssistantDlg::CanInspectionNameBeUsed(CString sName)
{
	BOOL bRet = false;
	POSITION pos = m_slInspectionNamesUsed.Find(sName);

	if ( nullptr == pos )
	{
		bRet = true;
	}
	return bRet;
}

BOOL CSVOConfigAssistantDlg::SystemChangeResetCamera( SVIMProductEnum p_lNewSystemType )
{
	BOOL l_bRet = false;

	switch( p_lNewSystemType )
	{
		case SVIM_PRODUCT_X2_GD1A:
		case SVIM_PRODUCT_X2_GD2A:
		case SVIM_PRODUCT_X2_GD4A:
		case SVIM_PRODUCT_X2_GD8A:
		case SVIM_PRODUCT_X2_GD1A_COLOR:
		case SVIM_PRODUCT_X2_GD2A_COLOR:
		case SVIM_PRODUCT_X2_GD4A_COLOR:
		case SVIM_PRODUCT_X2_GD8A_COLOR:
		{
			int iNum = GetCameraListCount();
			int iCh = 0;
			int iDig = 0;
			for ( int i = 0; i < iNum; i++ )
			{
				SVOCameraObjPtr pCameraObj( GetCameraObject(i) );
				if ( nullptr != pCameraObj )
				{
					iDig = pCameraObj->GetDigNumber();
					pCameraObj->SetDigNumber(iDig);
					pCameraObj->SetBandNumber(0);
					pCameraObj->SetCameraFile("");
					if (!pCameraObj->IsFileAcquisition())
					{
						AddMessageToList(CAMERA_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCameraObj->GetCameraDisplayName(), CAMERA_INVALID_FILES));
					}
				}
			}
			l_bRet = true;
			break;
		}
		default:
		{
			//go thru each camera and remove dig #'s and assign Ch#'s
			int iNum = GetCameraListCount();
			int iCh = 0;
			int iDig = 0;
			for ( int i = 0; i < iNum; i++ )
			{
				SVOCameraObjPtr pCameraObj( GetCameraObject(i) );
				if ( nullptr != pCameraObj )
				{
					iDig = pCameraObj->GetDigNumber();
					pCameraObj->SetDigNumber(0);
					pCameraObj->SetBandNumber(iDig);
					pCameraObj->SetCameraFile("");
					if (!pCameraObj->IsFileAcquisition())
					{
						AddMessageToList(CAMERA_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCameraObj->GetCameraDisplayName(), CAMERA_INVALID_FILES));
					}
				}
			}
			l_bRet = true;
			break;
		}
	}
	return l_bRet;
}

void CSVOConfigAssistantDlg::ClearMessages()
{
	m_ctlMsgList.ResetContent();

	OnChangeEditConfigurationName();
}

void CSVOConfigAssistantDlg::ConvertToDigital(SVIMProductEnum eType)
{
	typedef std::map<CString, int> StringIntMap;
	CString baseTriggerName = (!IsNonIOSVIM(eType) ? SvO::cTriggerFixedName : SvTh::CameraTriggerName);
	//get trigger count...
	int iTriggerCount = m_TriggerList.GetTriggerListCount();
	int iCameraCount = m_CameraList.GetCameraListCount();

	CString sCameraName;
	CString sTriggerName;

	//delete all triggers and re add them.
	StringIntMap l_Triggers;
	for( int i = 0 ; i < iTriggerCount ; i++ )
	{
		const SvTi::SVOTriggerObjPtr pTriggerObj( m_TriggerList.GetTriggerObjectByPosition(i) );
		if( nullptr != pTriggerObj )
		{
			l_Triggers[pTriggerObj->GetTriggerDisplayName()] = pTriggerObj->GetTriggerDigNumber();
		}
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
		const SVOCameraObjPtr pCameraObj( m_CameraList.GetCameraObjectByPosition(i) );
		if( nullptr != pCameraObj )
		{
			l_Cameras[pCameraObj->GetCameraDisplayName()] = pCameraObj->GetCameraID();
		}
	}

	m_CameraList.ResetContent();

	for ( StringIntMap::iterator it = l_Cameras.begin(); it != l_Cameras.end() ; ++it )
	{
		m_CameraList.AddCameraToList(it->first, it->second, it->second);
	}
}

void CSVOConfigAssistantDlg::RemoveFileAcquisitionMessages( const CString& rCameraName )
{
	CString sFileAcquisitionNotAllowedMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, rCameraName, MESSAGE_FILE_ACQUISITION_NOT_ALLOWED);
	CString sEmptyFileNameMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, rCameraName, MESSAGE_FILE_ACQUISITION_MISSING_FILENAME);
	CString sEmptyDirNameMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, rCameraName, MESSAGE_FILE_ACQUISITION_MISSING_DIRECTORY);
	CString sInvalidFileMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, rCameraName, MESSAGE_FILE_ACQUISITION_INVALID_FILE);
	CString sInvalidDirMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, rCameraName, MESSAGE_FILE_ACQUISITION_INVALID_DIRECTORY);
	CString sInvalidImageSizeMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, rCameraName, MESSAGE_FILE_ACQUISITION_INVALID_IMAGE_SIZE);
	CString sColorMismatchMessage =  BuildDisplayMessage(MESSAGE_TYPE_ERROR, rCameraName, MESSAGE_FILE_ACQUISITION_COLOR_MISMATCH);

	RemoveMessageFromList(sFileAcquisitionNotAllowedMessage);
	RemoveMessageFromList(sEmptyFileNameMessage);
	RemoveMessageFromList(sEmptyDirNameMessage);
	RemoveMessageFromList(sInvalidFileMessage);
	RemoveMessageFromList(sInvalidDirMessage);
	RemoveMessageFromList(sInvalidImageSizeMessage);
	RemoveMessageFromList(sColorMismatchMessage);
}

HRESULT CSVOConfigAssistantDlg::CheckCamera( SVOCameraObj& rCameraObj, bool SetFileParameters )
{
	CString CameraName( rCameraObj.GetCameraDisplayName() );
	RemoveFileAcquisitionMessages( CameraName );

	CString sDigName = BuildDigName( rCameraObj );
	SVDigitizerProcessingClass::Instance().SetDigitizerColor( sDigName, rCameraObj.IsColor() );

	if( rCameraObj.IsFileAcquisition())
	{
		CString sEmptyFileNameMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName, MESSAGE_FILE_ACQUISITION_MISSING_FILENAME);
		CString sEmptyDirNameMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName, MESSAGE_FILE_ACQUISITION_MISSING_DIRECTORY);
		CString sInvalidFileMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, CameraName, MESSAGE_FILE_ACQUISITION_INVALID_FILE);
		CString sInvalidDirMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, CameraName, MESSAGE_FILE_ACQUISITION_INVALID_DIRECTORY);
		CString sInvalidImageSizeMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName, MESSAGE_FILE_ACQUISITION_INVALID_IMAGE_SIZE);
		CString sColorMismatchMessage =  BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName, MESSAGE_FILE_ACQUISITION_COLOR_MISMATCH);

		// check that image size is not zero
		const SIZE& size = rCameraObj.GetFileImageSize();
		if (size.cx == 0 || size.cy == 0)
		{
			AddMessageToList( CAMERA_DLG, sInvalidImageSizeMessage );
		}

		// check that file or directory is valid
		long loadingMode = rCameraObj.GetFileLoadingMode();
		if (loadingMode == ContinuousMode || loadingMode == SingleIterationMode)
		{
			// check that directory exists
			CString directoryName = rCameraObj.GetImageDirectoryName();
			if (!directoryName.IsEmpty())
			{
				// strip off trailing slash or backslash
				CString lastChar = directoryName.Right(1);
				if (lastChar == _T("\\") || lastChar == _T("/"))
				{
					directoryName.Delete(directoryName.GetLength() - 1);
				}

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
			CString filename = rCameraObj.GetImageFilename();
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

		SVImageFile FileImage;
		SVString Name;

		if ( 0 == rCameraObj.GetFileLoadingMode() )
		{
			Name = rCameraObj.GetImageFilename();
		}
		else
		{
			Name = rCameraObj.GetImageDirectoryName();
		}

		if( S_OK == SVImageFileLoader::LoadFirstFile( Name.c_str(), _T("*.bmp"), FileImage ) )
		{
			bool isColorImage = FileImage.GetBitDepth() > GrayScaleBitDepth;

			if ( rCameraObj.IsColor() != isColorImage )
			{
				AddMessageToList( CAMERA_DLG,sColorMismatchMessage );
			}
		}
	}
	else
	{
		SVAcquisitionClassPtr pDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( sDigName );
		if( !( pDevice.empty() ) )
		{
			SVDeviceParamCollection DeviceParams;

			//First save the current device parameters to the camera object
			pDevice->GetDeviceParameters( DeviceParams );
			rCameraObj.SetCameraDeviceParams( DeviceParams );

			HRESULT hr = pDevice->ReadCameraFile( SVString(rCameraObj.GetCameraFile()), DeviceParams );

			if( SetFileParameters )
			{
				rCameraObj.SetCameraFileParams( DeviceParams );
			}

			bool bDisplayWarnings = TheSVObserverApp.GetSVIMType() == GetProductType();
			CString sMessageIncorrectCamFile = BuildDisplayMessage(MESSAGE_TYPE_WARNING, CameraName, MESSAGE_INCORRECT_CAM_FILE);
			RemoveMessageFromList(sMessageIncorrectCamFile);
			if( DeviceParams.ParameterExists( DeviceParamVendorId ) || DeviceParams.ParameterExists( DeviceParamModelName ) )
			{
				// do this only if True SVIM type matches selected product type - do warning
				if( bDisplayWarnings )
				{
					try
					{
						if( S_OK != pDevice->IsValidCameraFileParameters( DeviceParams ) )
						{
							AddMessageToList( CAMERA_DLG, sMessageIncorrectCamFile );
						}
					}
					catch( const SvStl::MessageContainer& )
					{
						AddMessageToList( CAMERA_DLG, sMessageIncorrectCamFile );
					}
				}// end if ( bDisplayWarnings )
			}

			CString sMessageNotColorCamFile = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName, MESSAGE_NOT_COLOR_CAM_FILE);
			CString sMessageNoColorCam = BuildDisplayMessage(MESSAGE_TYPE_WARNING, CameraName, MESSAGE_NOT_COLOR_CAM);

			RemoveMessageFromList(sMessageNotColorCamFile);
			RemoveMessageFromList(sMessageNoColorCam);
			// if camera file color mismatch selected product color - error
			//      only color product and b/w cam file
			//      the other way is OK, because color cams usu. support b/w images
			if ( rCameraObj.IsColor() )
			{
				if ( DeviceParams.ParameterExists( DeviceParamCameraFormats ) )
				{
					const SVCameraFormatsDeviceParam* pParam = DeviceParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
					if ( !pParam->SupportsColor() )
					{
						AddMessageToList(CAMERA_DLG,sMessageNotColorCamFile);
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
			}
		}// end if ( pDevice )
	}
	return S_OK;
}

void CSVOConfigAssistantDlg::CheckColor( const SVOCameraObj& rCameraObj )
{

	int Count = m_InspectList.GetInspectionListCount();

	for ( int i = 0; i < Count; i++ )
	{
		const SVOInspectionObjPtr pInspectionObj( GetInspectionObject(i) );

		if( nullptr != pInspectionObj )
		{
			CString sToolsetImage( pInspectionObj->GetToolsetImage() );

			if( sToolsetImage == rCameraObj.GetCameraDisplayName() )
			{
				CString MessageColor( BuildDisplayMessage(MESSAGE_TYPE_ERROR, pInspectionObj->GetInspectionLabelName(), MESSAGE_INSPECTION_CAMERA_COLOR) );
				RemoveMessageFromList( MessageColor );

				if ( !sToolsetImage.IsEmpty() )
				{
					//If an existing or imported inspection then inspection color is not allowed to be changed
					if( !pInspectionObj->IsNewInspection() || !pInspectionObj->GetImportFilename().IsEmpty() )
					{
						if( rCameraObj.IsColor() != pInspectionObj->IsColor() )
						{
							AddMessageToList( INSPECT_DLG, MessageColor );
						}
					}
				}
			}
		}
	}
}

void CSVOConfigAssistantDlg::CheckTriggers()
{
	int iTrg = GetTriggerListCount();
	for (int i = 0;i < iTrg;i++)
	{
		const SvTi::SVOTriggerObjPtr pTriggerObj( GetTriggerObject(i) );
		if( nullptr != pTriggerObj )
		{
			CheckTrigger( *pTriggerObj );
		}
	}
	IsGood();
}

BOOL CSVOConfigAssistantDlg::CheckTrigger( const SvTi::SVOTriggerObj& rTriggerObj)
{
	BOOL bRet = true;
	CString TriggerName( rTriggerObj.GetTriggerDisplayName() );
	CString sMessageNoSoftwareTriggerAllowed = BuildDisplayMessage(MESSAGE_TYPE_ERROR, TriggerName, MESSAGE_SOFTWARE_TRIGGER_NOT_ALLOWED);

	if ( rTriggerObj.IsSoftwareTrigger() )
	{
		// check if Software Trigger is allowed
		bRet = IsSoftwareTriggerAllowed( TriggerName );
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

HRESULT CSVOConfigAssistantDlg::ConnectToolsetBuffers()
{
	// EB 2002 02 28
	// Added this to fix memory leak freeing MIL buffers
	// before we close Acq devices, we need to tell all toolsets to Close
	// reopen the connection between toolset images and the MIL acq image here.

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	long lInspectionCount = 0;

	//If pointer nullptr then count is 0
	if( nullptr != pConfig ) { lInspectionCount = pConfig->GetInspectionCount( ); }

	for ( long lInspection = 0; lInspection < lInspectionCount; lInspection++ )
	{
		SVInspectionProcess* pInspection = pConfig->GetInspection( lInspection );
		if ( nullptr != pInspection )
		{
			if ( nullptr != pInspection->GetToolSet() )
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
	if ( SvTi::SVHardwareManifest::IsDigitalSVIM( m_lConfigurationType ) )
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
	CString l_Retval = "Unknown";

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
	return SvTi::SVHardwareManifest::IsNonIOSVIM(productType);
}

bool CSVOConfigAssistantDlg::IsGigeSystem() const
{
	return SvTi::SVHardwareManifest::IsMatroxGige(GetProductType());
}

bool CSVOConfigAssistantDlg::IsDigitalSystem() const
{
	return SvTi::SVHardwareManifest::IsDigitalSVIM(GetProductType());
}

bool CSVOConfigAssistantDlg::IsValidCamera(int iDig) const
{
	bool bRetVal = false;
	int iNum = GetCameraListCount();
	if (iDig < iNum)
	{
		for (int i = 0;i < iNum && !bRetVal; i++)
		{
			const SVOCameraObjPtr pCameraObj = m_CameraList.GetCameraObjectByPosition(i);
			if( nullptr != pCameraObj )
			{
				if( pCameraObj->GetDigNumber() == iDig )
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
			const SVOCameraObjPtr pCameraObj = m_CameraList.GetCameraObjectByPosition(i);
			if( nullptr != pCameraObj )
			{
				if (pCameraObj->GetDigNumber() == iDig)
				{
					bRetVal = pCameraObj->IsFileAcquisition();
				}
			}
		}
	}
	return bRetVal;
}

void CSVOConfigAssistantDlg::OnBnClickedCancel()
{
	// use m_TmpCameraList to get back the orginial camera information
	if ( SvTi::SVHardwareManifest::IsDigitalSVIM( GetProductType() ) )
	{
		int iSize = m_TmpCameraList.GetCameraListCount();

		for ( int i = 0; i < iSize; i++ )
		{
			const SVOCameraObjPtr pCameraObj = m_TmpCameraList.GetCameraObjectByPosition(i);
			SVAcquisitionClassPtr psvDevice;
			SVFileNameArrayClass svFiles;
			SVFileNameClass svFile;

			if( nullptr != pCameraObj )
			{
				psvDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( BuildDigName( *pCameraObj ) );
				if( nullptr != psvDevice )
				{
					CString szFileName = pCameraObj->GetCameraFile();
					svFile.SetFullFileName(szFileName);
					svFiles.Add(svFile);
					psvDevice->LoadFiles(svFiles);
					
					psvDevice->SetDeviceParameters(pCameraObj->GetCameraDeviceParams());
				}
			}
		}
	}//if digital SVIM

	CDialog::OnCancel();
}

void CSVOConfigAssistantDlg::resolveGlobalConflicts( SvOi::GlobalConflictPairVector& rGlobalConflicts )
{
	if( 0 < rGlobalConflicts.size() )
	{
		SvOg::GlobalConstantConflictDlg ConflictDlg( rGlobalConflicts, AfxGetMainWnd() );
		if( IDOK == ConflictDlg.DoModal() )
		{
			SvOi::GlobalConflictPairVector::const_iterator Iter( rGlobalConflicts.cbegin() );

			while( rGlobalConflicts.cend() != Iter )
			{
				if( Iter->second.m_Selected )
				{
					BasicValueObject* pGlobalObject(nullptr);

					pGlobalObject = dynamic_cast<BasicValueObject*> ( SVObjectManagerClass::Instance().GetObject(  Iter->first.m_Guid ) );
					if( nullptr != pGlobalObject )
					{
						pGlobalObject->setValue( Iter->second.m_Value );
						pGlobalObject->setDescription( Iter->second.m_Description.c_str() );
						pGlobalObject->ObjectAttributesAllowedRef() = SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;
						if( Iter->second.m_Value.vt == VT_BSTR )
						{
							pGlobalObject->ObjectAttributesAllowedRef() &= ~SV_SELECTABLE_FOR_EQUATION;
						}
					}
				}
				++Iter;
			}
		}
	}
}
