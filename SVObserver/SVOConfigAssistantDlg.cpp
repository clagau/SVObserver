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
//Moved to precompiled header: #include <sys/stat.h>
//Moved to precompiled header: #include <io.h>

#include "SVOConfigAssistantDlg.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFile.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFileLoader.h"
#include "CameraLibrary/SVboolValueDeviceParam.h"
#include "SVFileAcquisitionDevice/SVFileAcquisitionLoadingModeEnum.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVObserver.h"
#include "SVOPPQObj.h"
#include "SVOInspectionObj.h"
#include "TriggerInformation/SVTriggerConstants.h"
#include "TriggerInformation/SVOTriggerObj.h"
#include "SVOCameraObj.h"
#include "InspectionEngine/SVVirtualCamera.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "InspectionEngine/SVDigitizerProcessingClass.h"
#include "SVIPDoc.h"
#include "TriggerHandling/SVTriggerClass.h"
#include "SVMessage/SVMessage.h"
#include "TriggerInformation/SVIMTypeInfoStruct.h"
#include "TriggerInformation/SVTriggerProcessingClass.h"
#include "SVInspectionImporter.h"
#include "SVImportProgress.h"
#include "TriggerInformation/SVHardwareManifest.h"
#include "TextDefinesSvO.h"
#include "SVOGui\GlobalConstantConflictDlg.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/TextDefineSvDef.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

constexpr int GrayScaleBitDepth = 8;

constexpr char*  c_Color = _T(" Color");

// Defines for Camera File extensions
constexpr char*  cGigeCameraFileDefExt = _T(".ogc");

constexpr char* SVIM_BOARD_FILEACQUISITION_STRING     ( _T("File") );
constexpr char* SVIM_BOARD_MATROX_GIGE	( _T("Matrox_GIGE") );

constexpr char* cSvimDigName    ( _T(".Dig_") );

constexpr char* ERR_STR                 ( _T( "**ERROR**   " ) );
constexpr char* WARNING_STR             ( _T( "**WARNING** " ) );
constexpr char* CAMERA_INVALID_FILES    ( _T( "-Camera files are invalid" ) );
constexpr char* CAMERA_NONE_DEFINED     ( _T( "-No cameras defined" ) );
constexpr char* TRIGGER_NONE_DEFINED    ( _T( "-No trigger defined" ) );
constexpr char* INSPECT_NONE_DEFINED    ( _T( "-No inspection defined" ) );
constexpr char* PPQ_NONE_DEFINED        ( _T( "-No PPQ defined" ) );
constexpr char* PPQ_NO_TRIGGER          ( _T( "-No Trigger attached to PPQ" ) );
constexpr char* PPQ_NO_CAMERA           ( _T( "-No Camera attached to PPQ" ) );
constexpr char* PPQ_NO_INSPECT          ( _T( "-No Inspection attached to PPQ" ) );
constexpr char* CONFIG_NAME_ERR         ( _T( " No Configuration name" ) );
constexpr char* INVALID_CAMERA          ( _T( "-Camera is invalid for current product" ) );
constexpr char* INVALID_TRIGGER         ( _T( "-Trigger is invalid for current product" ) );
constexpr char* INSPECTION_ERROR        ( _T( "-Inspection is not attached to a PPQ" ) );
constexpr char* TOOLSET_IMAGE_ERROR     ( _T( "-Inspection has no Toolset Image" ) );
constexpr char* PPQ_PROP_SRC_IMG_ERROR  ( _T( "- Property -'Maintain souce image' set to TRUE, PPQ length > ") );
constexpr char* PPQ_CONDITIONAL_OUTPUT_INVALID ( _T( "-Conditional Output for PPQ is not valid" ) );
constexpr char* MESSAGE_UNSUPPORTED_CAM_FILE ( _T("The camera file you have selected is not an SVResearch supported file.") );
constexpr char* MESSAGE_INCORRECT_CAM_FILE   ( _T("The selected camera file does not match the physical camera.") );
constexpr char* MESSAGE_NOT_COLOR_CAM_FILE   ( _T("The selected camera file is not a color camera file.") );
constexpr char* MESSAGE_NOT_COLOR_CAM        ( _T("The physical camera is not a color camera.") );
constexpr char* MESSAGE_NOT_MONO_CAM_FILE    (_T("The selected camera file is not a mono camera file."));
constexpr char* MESSAGE_ONE_INVERT_CONTROL   ( _T("There is only one Invert control on a SVIM X-Series for all triggers and strobes.") );
constexpr char* MESSAGE_SOFTWARE_TRIGGER_NOT_ALLOWED ( _T("The camera does not support Software Triggering.") );
constexpr char* MESSAGE_FILE_ACQUISITION_NOT_ALLOWED ( _T("File Acquisition is not allowed.") );
constexpr char* MESSAGE_FILE_ACQUISITION_INVALID_FILE ( _T("The Image Filename specified is Invalid.") );
constexpr char* MESSAGE_FILE_ACQUISITION_INVALID_DIRECTORY ( _T("The Image Directory specified is Invalid.") );
constexpr char* MESSAGE_FILE_ACQUISITION_MISSING_FILENAME ( _T("The Image Filename must be specified.") );
constexpr char* MESSAGE_FILE_ACQUISITION_MISSING_DIRECTORY ( _T("The Image Directory must be specified.") );
constexpr char* MESSAGE_FILE_ACQUISITION_INVALID_IMAGE_SIZE ( _T("The Image Size is Invalid.") );
constexpr char* MESSAGE_FILE_ACQUISITION_COLOR_MISMATCH ( _T("The bitmap file acquisition color does not match the camera color setting") );
constexpr char* MESSAGE_INSPECTION_CAMERA_COLOR  ( _T("-The toolset camera image color does not match the inspection color") );

const CSVOConfigAssistantDlg::SVProductStringVector CSVOConfigAssistantDlg::m_ProductStringVector
{
	// SYSTEM ID, DISPLAY NAME
	{SVIM_PRODUCT_X2_GD1A, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD1A)},
	{SVIM_PRODUCT_X2_GD1A_COLOR, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD1A) + std::string(c_Color)},
	{SVIM_PRODUCT_X2_GD2A, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD2A)},
	{SVIM_PRODUCT_X2_GD2A_COLOR, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD2A) + std::string(c_Color)},
	{SVIM_PRODUCT_X2_GD4A, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD4A)},
	{SVIM_PRODUCT_X2_GD4A_COLOR, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD4A) + std::string(c_Color)},
	{SVIM_PRODUCT_X2_GD8A, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD8A)},
	{SVIM_PRODUCT_X2_GD8A_COLOR, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD8A) + std::string(c_Color)},
	//{SVIM_PRODUCT_X2_GD8A_NONIO, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD8A_NONIO)},
	//{SVIM_PRODUCT_X2_GD8A_NONIO_COLOR, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD8A_NONIO) + std::string(c_Color)};
	{SVIM_PRODUCT_NEO1, std::string(SvDef::SVO_PRODUCT_SVIM_NEO1)}
};

/////////////////////////////////////////////////////////////////////////////
// CSVOConfigAssistantDlg dialog

CSVOConfigAssistantDlg::CSVOConfigAssistantDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CSVOConfigAssistantDlg::IDD, pParent)
	, m_ctlConfigurationName(SvDef::cExcludeCharsConfigName)
{
	SVIMProductEnum eSvimType = TheSVObserverApp.GetSVIMType();
	SetConfigurationSystem( eSvimType );
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
	DDX_CBString(pDX, IDC_COMBO_AVAL_SYS, m_AvailableSystem);
	DDX_Text(pDX, IDC_EDIT_CURRENT_SYS, m_CurrentSys);
	DDX_Text(pDX, IDC_EDIT_CONFIGURATION_NAME, m_ConfigurationName);
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
		m_ctlConfigurationName.SetWindowText( TheSVObserverApp.getConfigFileName().c_str() );
		m_ctlConfigurationName.EnableWindow(FALSE);
	}
	SetupSystemComboBox();

	SetCurrentSystemDisplay();

	if (m_bNewConfiguration)
	{
		ReloadForCurrentSystem();
	}

	//select correct product on right hand side
	std::string ProductString = GetNameFromProductID( m_lConfigurationType );
	int iExactStringPosition = m_ctlAvailableSys.FindStringExact(-1, ProductString.c_str() );
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
	SVIMProductEnumList list; 
	typedef std::insert_iterator<SVIMProductEnumList> Insertor;
	Insertor insertor(list, list.begin());
	SvTi::SVHardwareManifest::GetSupportedSVIMList( insertor );

	for (SVIMProductEnumList::iterator l_Iter = list.begin();l_Iter != list.end(); ++l_Iter)
	{
		m_ctlAvailableSys.AddString( GetNameFromProductID(*l_Iter).c_str() );
	}

	if ( !m_bNewConfiguration )
	{
		const SvTi::SVIMTypeInfoStruct& l_SVIMInfo = SvTi::SVHardwareManifest::GetSVIMTypeInfo( m_lConfigurationType );
		if ( !l_SVIMInfo.m_Supported )
		{
			m_ctlAvailableSys.AddString( GetNameFromProductID( m_lConfigurationType ).c_str() );
		}
	}
}

void CSVOConfigAssistantDlg::SetNewConfiguration(bool bNewFlag)
{
	m_bNewConfiguration = bNewFlag;
}

bool CSVOConfigAssistantDlg::IsNewConfiguration()
{
	return m_bNewConfiguration;
}

bool CSVOConfigAssistantDlg::AddToCameraList(LPCTSTR CameraName, int Dig, int CameraID)
{
	return m_CameraList.AddCameraToList(CameraName, Dig, CameraID);
}

bool CSVOConfigAssistantDlg::AddToInspectList(LPCTSTR External, LPCTSTR Internal, bool NewInspection)
{
	return m_InspectList.AddInspectionToList( External, Internal, NewInspection );
}

bool CSVOConfigAssistantDlg::AddToTriggerList(LPCTSTR TriggerName, int iDig)
{
	return m_TriggerList.AddTriggerToList( std::string(TriggerName), iDig );
}

bool CSVOConfigAssistantDlg::AddToPPQList(LPCTSTR PPQ, LPCTSTR Camera, LPCTSTR Trigger, LPCTSTR Inspection)
{
	bool bRet = m_PPQList.AddPPQToList( PPQ );
	bRet = m_PPQList.AttachCameraToPPQ( PPQ, Camera ) && bRet;
	bRet = m_PPQList.AttachInspectToPPQ( PPQ, Inspection ) && bRet;
	m_PPQList.AttachTriggerToPPQ( PPQ, Trigger );

	return bRet;
}

bool CSVOConfigAssistantDlg::AddToPPQList(LPCTSTR PPQ)
{
	return m_PPQList.AddPPQToList( PPQ );
}

void CSVOConfigAssistantDlg::OnSelchangeComboAvalSys() 
{
	m_ctlAvailableSys.GetWindowText( m_AvailableSystem );

	SVIMProductEnum currentConfigType = m_lConfigurationType; 

	SVIMProductEnum newConfigType = GetProductIDFromName( std::string(m_AvailableSystem) );

	if ( false == m_bNewConfiguration || m_bModified )
	{
		if ( SvTi::SVHardwareManifest::IsCompatible(newConfigType, currentConfigType) )
		{
			m_bModified = true;

			m_lConfigurationType = newConfigType;
			UpdateAvailableSystems( currentConfigType, m_lConfigurationType );
		}
		else
		{
			bool configTypeConvereted {false};
			// Check if I/O digital to Non I/O digital or vice versa
			if (SvTi::SVHardwareManifest::IsNonIOSVIM(newConfigType) != SvTi::SVHardwareManifest::IsNonIOSVIM(currentConfigType))
			{
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				INT_PTR result = Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_Config_SwitchResetQuestion, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10138, SvDef::InvalidObjectId, MB_YESNO);
				if ( IDYES == result )
				{
					m_lConfigurationType = newConfigType;
					m_bModified = true;
					UpdateAvailableSystems( currentConfigType, m_lConfigurationType );
					configTypeConvereted = true;
				}
			}
			///When changing from discrete IO to PLC or vice versa then we need to reset the inputs and outputs
			if ((SvTi::SVHardwareManifest::isPlcSystem(newConfigType) != SvTi::SVHardwareManifest::isPlcSystem(currentConfigType)) ||
				(SvTi::SVHardwareManifest::isDiscreteIOSystem(newConfigType) != SvTi::SVHardwareManifest::isDiscreteIOSystem(currentConfigType)))
			{
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_Config_SwitchIOQuestion, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10139, SvDef::InvalidObjectId, MB_YESNO);
				if ( IDYES == result )
				{
					m_lConfigurationType = newConfigType;

					ClearMessages();
					CheckAgainstCurrentList();
					
					m_bModified = true;
					IsGood();

					UpdateAvailableSystems( currentConfigType, m_lConfigurationType );
					configTypeConvereted = true;
				}
			}
			if(false == configTypeConvereted)
			{
				UINT l_NextIndex = m_ctlAvailableSys.FindStringExact(0, GetNameFromProductID(currentConfigType).c_str());

				m_ctlAvailableSys.SetCurSel(l_NextIndex);
				IsGood();
				return;
			}
		}
	}

	m_lConfigurationType = newConfigType;

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
		std::string SelectedSystemName = GetNameFromProductID( p_CurrentConfigurationType );
		UINT l_Index = m_ctlAvailableSys.FindStringExact( 0, SelectedSystemName.c_str() );

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
				CreateDefaultForSVIMDigital(1, SvDef::cTriggerFixedName);
				break;
			}
			case SVIM_PRODUCT_X2_GD8A:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
			case SVIM_PRODUCT_NEO1:
			{
				CreateDefaultForSVIMDigital(2, SvDef::cTriggerFixedName);
				break;
			}
			case SVIM_PRODUCT_X2_GD8A_NONIO:
			case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				CreateDefaultForSVIMDigital(2, SvTi::CameraTriggerName);
				break;
			}
			default:
			{
				m_lConfigurationType = SVIM_PRODUCT_X2_GD4A;

				CreateDefaultForSVIMDigital(4, SvDef::cTriggerFixedName);

				std::string ProductName = GetNameFromProductID( m_lConfigurationType );
				m_ctlAvailableSys.SelectString( -1, ProductName.c_str() );
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
		std::string CameraName = GetNextCameraName();
		m_CameraList.AddCameraToList( CameraName.c_str(), i, i );

		//add a trigger
		std::string TriggerName = GetNextTriggerName(TriggerBaseName);
		m_TriggerList.AddTriggerToList(TriggerName, i);

		//add an inspection...
		std::string InspectionName = GetNextInspectionName();
		m_InspectList.AddInspectionToList( InspectionName.c_str(), InspectionName.c_str(), true );
		m_InspectList.SetToolsetImage( InspectionName.c_str(), CameraName.c_str() );
		m_InspectionNamesUsed.push_back(InspectionName);
		m_InspectionLabelsUsed.push_back(InspectionName);

		//add a ppq, attach trigger, camera and inspection to it.
		std::string PPQName = GetNextPPQName();
		m_PPQList.AddPPQToList( PPQName.c_str() );
		m_PPQList.AttachCameraToPPQ( PPQName.c_str(), CameraName.c_str() );
		m_PPQList.AttachInspectToPPQ( PPQName.c_str(), InspectionName.c_str() );
		AddUsedInspect( InspectionName.c_str() );
		m_PPQList.AttachTriggerToPPQ( PPQName.c_str(), TriggerName.c_str() );
		AddUsedTrigger( TriggerName.c_str() );
	}
}

std::string CSVOConfigAssistantDlg::GetNextCameraName()
{
	int iRet = 0;
	bool bFound = true;
	std::string Result;

	while ( bFound )
	{
		iRet++;
		Result = SvUl::Format( _T("%s%d"), SvDef::cCameraFixedName, iRet );
		bFound = m_CameraList.IsCameraInList( Result.c_str() );
	}
	m_iNextCameraNumber = iRet;
	return Result;
}

int CSVOConfigAssistantDlg::GetNextCameraNumber() const
{
	return m_iNextCameraNumber;
}

std::string CSVOConfigAssistantDlg::GetNextInspectionName() const
{
	int iRet = 0;
	bool bFound = TRUE;
	std::string Result;

	if ( m_bInspectionDeleted )
	{
		iRet = m_iLastInspectionNum;
	}

	while ( bFound )
	{
		iRet++;
		Result = SvUl::Format( _T("%s%d"), SvDef::cInspectionFixedName, iRet );
		bFound = m_InspectList.IsInspectionInList(Result.c_str());
	}
	return Result;
}

std::string CSVOConfigAssistantDlg::GetNextTriggerName(LPCTSTR BaseName) const
{
	int iRet = m_TriggerList.GetNextTriggerID();
	
	std::string Result = SvUl::Format(_T("%s%d"), BaseName, iRet + 1 );

	return Result;
}

int CSVOConfigAssistantDlg::GetNextTriggerID() const
{
	return m_TriggerList.GetNextTriggerID();
}

std::string CSVOConfigAssistantDlg::GetNextPPQName() const
{
	int iRet = 0;
	bool bFound = true;
	std::string Result;

	while( bFound )
	{
		iRet++;
		Result = SvUl::Format( _T("%s%d"), SvDef::cPpqFixedName, iRet );
		bFound = m_PPQList.IsPPQInList( Result.c_str() );
	}
	return Result;
}

std::string CSVOConfigAssistantDlg::GetInspectionNameFromLabel(LPCTSTR InspectLabel)
{
	return m_InspectList.GetInspectionName( InspectLabel );
}

std::string CSVOConfigAssistantDlg::GetInspectionLabelFromName(LPCTSTR InspectionName)
{
	return m_InspectList.GetInspectionLabel( InspectionName );
}

bool CSVOConfigAssistantDlg::RenameInspection(LPCTSTR InspectLabel, LPCTSTR NewName)
{
	const SVOInspectionObjPtr pInspectionObj( GetInspectionObjectByLabel(InspectLabel) );
	std::string OldName;

	if( nullptr != pInspectionObj )
	{
		OldName = pInspectionObj->GetInspectionName();
	}

	//check to see if the old message is in the list.  if it is replace it with the
	// new inspection name...

	std::string Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, OldName.c_str(), INSPECTION_ERROR);    

	if (RemoveMessageFromList(Message.c_str()))
	{//add new message
		Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, NewName, INSPECTION_ERROR);
		AddMessageToList(PPQ_DLG, Message.c_str());
	}
	else
	{
		// inspection is attached. do nothing
	}
	Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, OldName.c_str(), TOOLSET_IMAGE_ERROR);
	if ( RemoveMessageFromList(Message.c_str()) )
	{
		Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, NewName, TOOLSET_IMAGE_ERROR);
		AddMessageToList(INSPECT_DLG, Message.c_str());
	}
	
	return m_InspectList.ReNameInspection(InspectLabel, NewName);
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

SVOInspectionObjPtr CSVOConfigAssistantDlg::GetInspectionObjectByLabel(LPCTSTR Name)
{
	return m_InspectList.GetInspectionByLabel(Name);
}

SVOCameraObjPtr CSVOConfigAssistantDlg::GetCameraObject(int iPos)
{
	return m_CameraList.GetCameraObjectByPosition(iPos);
}

SVOCameraObjPtr CSVOConfigAssistantDlg::GetCameraObjectByName(LPCTSTR CameraName)
{
	return m_CameraList.GetCameraObjectByName(CameraName);
}

SvTi::SVOTriggerObjPtr CSVOConfigAssistantDlg::GetTriggerObject(int iPos)
{
	return m_TriggerList.GetTriggerObjectByPosition(iPos);
}

SvTi::SVOTriggerObjPtr CSVOConfigAssistantDlg::GetTriggerObjectByName(LPCTSTR TriggerName)
{
	return m_TriggerList.GetTriggerObjectByName(std::string(TriggerName));
}

SVOPPQObjPtr CSVOConfigAssistantDlg::GetPPQObject(int iPos)
{
	return m_PPQList.GetPPQObjectByPosition(iPos);
}

SVOPPQObjPtr CSVOConfigAssistantDlg::GetPPQObjectByName(LPCTSTR PPQName)
{
	return m_PPQList.GetPPQObjectByName( PPQName );
}

bool CSVOConfigAssistantDlg::RemoveCameraFromList(LPCTSTR CameraName)
{
	return m_CameraList.RemoveCameraFromList( CameraName );
}

bool CSVOConfigAssistantDlg::RemoveInspectionFromList(LPCTSTR Inspection)
{
	if (IsInspectUsed(Inspection))
	{
		RemoveUsedInspect(Inspection);
	}
	InspectionDeletedCheckAgainstPPQ(Inspection);
	return m_InspectList.RemoveInspectionFromList(Inspection);
}

bool CSVOConfigAssistantDlg::RemoveTriggerFromList(LPCTSTR TriggerName)
{
	if (IsTriggerUsed( TriggerName ) )
	{
		RemoveUsedTrigger( TriggerName );
	}
	return m_TriggerList.RemoveTriggerFromList(std::string(TriggerName));
}

bool CSVOConfigAssistantDlg::RemovePPQFromList(LPCTSTR PPQ)
{
	const SVOPPQObjPtr pPPQObj = GetPPQObjectByName( PPQ );
	
	if( nullptr != pPPQObj )
	{
		RemoveUsedTrigger( pPPQObj->GetAttachedTriggerName().c_str() );

		int iInsCnt = pPPQObj->GetAttachedInspectionCount();

		for (int i = 0; i< iInsCnt; i++)
		{
			RemoveUsedInspect( pPPQObj->GetAttachedInspection(i).c_str() );
		}
	}

	return m_PPQList.RemovePPQFromList(PPQ);
}

std::string CSVOConfigAssistantDlg::BuildTrgDig( const SvTi::SVOTriggerObj& rTriggerObj) const
{
	std::string Result;

	int iDig = rTriggerObj.GetTriggerDigNumber();

	if (rTriggerObj.IsAcquisitionTrigger())
	{
		Result = SvTi::SVHardwareManifest::BuildAcquisitionTriggerDeviceName(iDig);
	}
	else if (rTriggerObj.IsSoftwareTrigger())
	{
		Result = SvTi::SVHardwareManifest::BuildSoftwareTriggerDeviceName(iDig);
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
				Result = SvTi::SVHardwareManifest::BuildIOBoardTriggerDeviceName(iDig);
				break;
			}
			case SVIM_PRODUCT_NEO1:
			{
				Result = SvTi::SVHardwareManifest::BuildHardwareTriggerDeviceName(iDig);
				break;
			}
			case SVIM_PRODUCT_X2_GD8A_NONIO:
			case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				Result = SvTi::SVHardwareManifest::BuildAcquisitionTriggerDeviceName(iDig);
				break;
			}
		}
	}
	return Result;
}

std::string CSVOConfigAssistantDlg::BuildDigName(const SVOCameraObj& rCameraObj) const
{
	std::string Result;
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
			case SVIM_PRODUCT_NEO1:
			case SVIM_PRODUCT_X2_GD1A_COLOR:
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			case SVIM_PRODUCT_X2_GD4A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				Result = SvUl::Format(_T("%s%s%d"), SVIM_BOARD_FILEACQUISITION_STRING, cSvimDigName, iDigNumber);
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
			case SVIM_PRODUCT_NEO1:
			case SVIM_PRODUCT_X2_GD1A_COLOR:
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			case SVIM_PRODUCT_X2_GD4A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				Result = SvUl::Format(_T("%s%s%d"), SVIM_BOARD_MATROX_GIGE, cSvimDigName, iDigNumber);
				break;
			}

			default:
				break;
		} 
	}
	return Result;
}

bool CSVOConfigAssistantDlg::IsDigitizerUsed(LPCTSTR DigString)
{
	bool Result( false );
	std::string CameraDig;

	int iCamCnt = m_CameraList.GetCameraListCount();

	for (int i = 0; i < iCamCnt && !Result; i++)
	{
		if( nullptr != m_CameraList.GetCameraObjectByPosition(i) )
		{
			CameraDig = BuildDigName( *m_CameraList.GetCameraObjectByPosition(i) );

			if( DigString == CameraDig )
			{
				Result = true;
			}
		}
	}
	return Result;
}

bool CSVOConfigAssistantDlg::IsCameraInList(LPCTSTR CameraName) const
{
	return m_CameraList.IsCameraInList(CameraName);
}

bool CSVOConfigAssistantDlg::IsInspectionInList(LPCTSTR InspectionName) const
{
	return m_InspectList.IsInspectionInList(InspectionName);
}

bool CSVOConfigAssistantDlg::IsInspectionNameInList(LPCTSTR InspectionName) const
{
	return m_InspectList.IsInspectionNameInList(InspectionName);
}

bool CSVOConfigAssistantDlg::IsTriggerInList(LPCTSTR TriggerName) const
{
	return m_TriggerList.IsTriggerInList(std::string(TriggerName));
}

bool CSVOConfigAssistantDlg::IsPPQInList(LPCTSTR PPQName) const
{
	return m_PPQList.IsPPQInList(PPQName);
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
	std::string SystemName;

	SystemName = GetNameFromProductID( m_lSystemType );
	if ( SystemName.empty() )
	{
		SystemName = _T("<UNKNOWN>");
	}

	m_ctlCurrentSys.SetWindowText( SystemName.c_str() );
}

bool CSVOConfigAssistantDlg::IsTriggerUsed(LPCTSTR TriggerName) const
{
	bool Result = false;

	SvDef::StringVector::const_iterator Iter = std::find( m_UsedTriggers.begin(), m_UsedTriggers.end(), std::string(TriggerName) );
	if( m_UsedTriggers.end() != Iter )
	{
		Result = true;
	}
	return Result;
}

void CSVOConfigAssistantDlg::AddUsedTrigger(LPCTSTR TriggerName)
{
	m_UsedTriggers.push_back( std::string(TriggerName) );
}

void CSVOConfigAssistantDlg::RemoveUsedTrigger(LPCTSTR TriggerName)
{
	SvDef::StringVector::const_iterator Iter = std::find( m_UsedTriggers.begin(), m_UsedTriggers.end(), std::string(TriggerName) );
	if( m_UsedTriggers.end() != Iter)
	{
		m_UsedTriggers.erase( Iter );
	}
}

bool CSVOConfigAssistantDlg::IsSoftwareTriggerAllowed(LPCTSTR TriggerName) const
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
				std::string attachedTriggerName = pPPQObj->GetAttachedTriggerName();
				if( 0 == SvUl::CompareNoCase( attachedTriggerName, std::string(TriggerName) ) )
				{
					int camCnt = pPPQObj->GetAttachedCameraCount();
					for (int n = 0;n < camCnt;n++)
					{
						std::string CameraName = pPPQObj->GetAttachedCamera(n);
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

bool CSVOConfigAssistantDlg::IsCameraLineInputAllowed(LPCTSTR TriggerName) const
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
				std::string attachedTriggerName = pPPQObj->GetAttachedTriggerName();
				if( 0 == SvUl::CompareNoCase( attachedTriggerName, std::string(TriggerName) ) )
				{
					int camCnt = pPPQObj->GetAttachedCameraCount();
					for (int n = 0;n < camCnt;n++)
					{
						std::string CameraName = pPPQObj->GetAttachedCamera(n);
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
	CString AvailableSystem;
	m_ctlAvailableSys.GetWindowText(AvailableSystem);
	if ( !AvailableSystem.IsEmpty() )
	{
		eProduct = GetProductIDFromName( std::string(AvailableSystem) );
		return eProduct;
	}
	else
	{
		return m_lConfigurationType;
	}
}

bool CSVOConfigAssistantDlg::IsInspectUsed(LPCTSTR Inspection)
{
	bool Result = false;

	SvDef::StringVector::const_iterator Iter = std::find( m_UsedInspections.begin(), m_UsedInspections.end(), std::string(Inspection) );
	if( m_UsedInspections.end() != Iter )
	{
		Result = true;
	}
	return Result;
}

void CSVOConfigAssistantDlg::AddUsedInspect(LPCTSTR Inspection)
{
	m_UsedInspections.push_back( std::string(Inspection) );
}

void CSVOConfigAssistantDlg::RemoveUsedInspect(LPCTSTR Inspection)
{
	SvDef::StringVector::const_iterator Iter = std::find( m_UsedInspections.begin(), m_UsedInspections.end(), std::string(Inspection) );
	if( m_UsedInspections.end() != Iter)
	{
		m_UsedInspections.erase( Iter );
	}
}

void CSVOConfigAssistantDlg::SetModified(bool bModified)
{
	m_bModified = bModified;
}

void CSVOConfigAssistantDlg::OnOK()
{
	UpdateData(true);

	CString sTmp = m_ConfigurationName.Right(4);
	//check configuration name.  do not allow name to have extension
	if ( ( sTmp.CompareNoCase(SvDef::cConfigExtension) == 0) )
	{
		int iLen = m_ConfigurationName.GetLength();
		sTmp = m_ConfigurationName.Left(iLen-4);
		m_ConfigurationName = sTmp;
	}
	UpdateData(false);

	SendDataToConfiguration();

	CDialog::OnOK();
}

////////////////////////////////////////////////////////
///               Message area 
///
////////////////////////////////////////////////////////
bool CSVOConfigAssistantDlg::AddMessageToList(int iDlg, LPCTSTR Message)
{
	bool bRet = FALSE;
	if (m_ctlMsgList.FindStringExact(-1, Message) == LB_ERR)
	{
		//message not there add it.
		int iPos = m_ctlMsgList.AddString( Message );
		m_ctlMsgList.SetItemData(iPos, iDlg);
		bRet = TRUE;
	}
	return bRet;
}

bool CSVOConfigAssistantDlg::RemoveMessageFromList(LPCTSTR Message)
{
	bool bRet = FALSE;
	int iPos = m_ctlMsgList.FindStringExact(-1, Message);
	
	if (iPos != LB_ERR)
	{
		m_ctlMsgList.DeleteString(iPos);
		bRet = TRUE;
	}
	return bRet;
}

int CSVOConfigAssistantDlg::RemoveAllObjectMessages(LPCTSTR ObjectName)
{
	CString sString;
	int iNumberDeleted = 0;
	int iCnt = m_ctlMsgList.GetCount();

	for (int i = iCnt-1; -1 < i; i--)
	{
		m_ctlMsgList.GetText(i,sString);
		if (sString.Find(ObjectName, 0) != -1)
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
					AddMessageToList(CAMERA_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCameraObj->GetCameraDisplayName().c_str(), CAMERA_INVALID_FILES).c_str());
				}
				CheckCamera( *pCameraObj );
			}
		}
	}
	else
	{ //configuration had no camera's.  add error message...
		AddMessageToList(CAMERA_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", CAMERA_NONE_DEFINED).c_str());
		m_bCameraGood = FALSE;
	}

	if ( iTrg < 1 )
	{
		m_bTriggerGood = FALSE;
		AddMessageToList(TRIGGER_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", TRIGGER_NONE_DEFINED).c_str());
	}
	else
	{
		CheckTriggers();
	}
	
	SetupTriggerStrobeMessage();

	if ( iIns < 1 )
	{
		m_bInspectionGood = FALSE;
		AddMessageToList(INSPECT_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", INSPECT_NONE_DEFINED).c_str());
	}

	if ( iPpq < 1 )
	{
		m_bPPQGood = FALSE;
		AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", PPQ_NONE_DEFINED).c_str());
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
					AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName().c_str(), PPQ_NO_CAMERA).c_str());
				}
				if ( pPPQObj->GetAttachedInspectionCount() < 1 )
				{
					//no inspection's attached
					AddMessageToList(PPQ_DLG, BuildDisplayMessage(MESSAGE_TYPE_WARNING, pPPQObj->GetPPQName().c_str(), PPQ_NO_INSPECT).c_str());
				}
				if ( pPPQObj->GetAttachedTriggerName().empty() )
				{
					//no attached trigger
					AddMessageToList(PPQ_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName().c_str(), PPQ_NO_TRIGGER).c_str());
				}
				if ( pPPQObj->IsConditionalOutputCameraInput() && !IsCameraLineInputAllowed(pPPQObj->GetAttachedTriggerName().c_str()) )
				{
					// condition for output not valid
					AddMessageToList(PPQ_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName().c_str(), PPQ_CONDITIONAL_OUTPUT_INVALID).c_str());
				}
			}
		}
	}
}

bool CSVOConfigAssistantDlg::SendPPQDataToConfiguration(SVPPQObjectPtrVector& rPPQsToDelete)
{
	bool bRet = true;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PPQs - pass 1 - detach
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	long lCfgPPQCnt = 0;
	//Count will be 0 if pointer nullptr
	if( nullptr != pConfig){ lCfgPPQCnt = pConfig->GetPPQCount(); } //should be zero on new cfg...
	//loop through to see if the PPQ's still exist in reverse order if not delete them
	for (int iPPQ = lCfgPPQCnt - 1; -1 < iPPQ; --iPPQ)
	{
		SVPPQObject* pPPQ = pConfig->GetPPQ(iPPQ);
		if ( nullptr != pPPQ )
		{
			if (!IsPPQInList(pPPQ->GetName()))
			{ //not there.  flag to delete it...
				rPPQsToDelete.push_back(pPPQ);
				
				//PPQ is no longer there.  Detach everything from it.
				pPPQ->DetachAll();
				
				pConfig->RemovePPQ(pPPQ);	// remove from config now
				//delete pPPQ;	// do this AFTER destroying triggers, cameras, and INSPECTIONS
				// destroying the PPQ will also detatch it from triggers, cameras, and inspections
			}
			else
			{
				long lInsCnt;
				pPPQ->GetInspectionCount(lInsCnt);
				
				//loop thru inspections to see if they exist in dlg
				//if not, delete them from ppqobj
				for (long lI=0; lI < lInsCnt; ++lI)
				{
					bool bDetachInspect = FALSE;
					SVInspectionProcess* pInspection( nullptr );
					bRet = pPPQ->GetInspection(lI, pInspection);
					if ( nullptr != pInspection)
					{
						//Inspection has the old name by now
						const SVOInspectionObjPtr pTmpInspection( GetInspectionObjectByLabel(pInspection->GetName()) );
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

				SvIe::SVVirtualCameraPtrVector cameraVector = pPPQ->GetVirtualCameras();

				for(auto* pCamera : cameraVector)
				{
					if ( nullptr != pCamera )
					{
						if (!IsCameraOnPPQ(pPPQ->GetName(), pCamera->GetName()))
						{
							bRet = pPPQ->DetachCamera(pCamera, true);
						}
					}
				}

				//check trigger...
				SvTi::SVTriggerObject* pTrigger{pPPQ->GetTrigger()};
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

bool CSVOConfigAssistantDlg::SendAcquisitionDataToConfiguration()
{
	bool bRet = true;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	assert( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		return false;
	}

	SVLightReference* psvLight = nullptr;
	SVFileNameArrayClass* psvFiles = nullptr;
	SVLut* pLut = nullptr;
	SVDeviceParamCollection* pDeviceParams = nullptr;

	std::string AcquisitionName;
	SVConfigurationObject::SVAcquisitionDeviceMap::iterator aPos = pConfig->GetAcquisitionDeviceStartPosition();
	while (aPos != pConfig->GetAcquisitionDeviceEndPosition())
	{
		//see if things are in the dlg...
		pConfig->GetAcquisitionDeviceNextAssoc( aPos, AcquisitionName );
		if (!IsDigitizerUsed(AcquisitionName.c_str()))
		{
			SvIe::SVAcquisitionClassPtr psvDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( AcquisitionName.c_str() );
			pConfig->RemoveAcquisitionDevice(AcquisitionName.c_str());
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
	SVDeviceParamCollection svDeviceParams;
	std::string DigName;

	int iCamCnt = m_CameraList.GetCameraListCount();

	for (int iAcq = 0; iAcq < iCamCnt; iAcq++)
	{
		svFiles.clear();
		int CameraIndex(iAcq);
		SvIe::SVAcquisitionClassPtr psvDevice;

		const SVOCameraObjPtr pCameraObj( GetCameraObject( CameraIndex ) );
		if( nullptr != pCameraObj )
		{
			SVFileNameClass svFile;
			SVLut lut;
			int Digitizer = SvIe::SVDigitizerProcessingClass::Instance().getDigitizerID( pCameraObj->GetCameraID() );
			if( -1 != Digitizer )
			{
				pCameraObj->SetDigNumber( Digitizer );
			}
			DigName = BuildDigName( *pCameraObj );
			// For File Acquisition
			if ( pCameraObj->IsFileAcquisition())
			{
				psvDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DigName.c_str() );
				if ( nullptr != psvDevice )
				{
					SVLightReference lightRef;

					SVDeviceParamCollection deviceParams;

					_variant_t fileNameVar = pCameraObj->GetImageFilename().c_str();
					_variant_t dirNameVar = pCameraObj->GetImageDirectoryName().c_str();

					long loadingMode = pCameraObj->GetFileLoadingMode();
					_variant_t fileLoadingModeVar = loadingMode;
				
					_variant_t widthVar = pCameraObj->GetFileImageWidth();
					_variant_t heightVar = pCameraObj->GetFileImageHeight();

					// Only 2 formats supported 32Bit color and 8Bit grayscale
					long format = (pCameraObj->IsColor()) ? SvDef::SVImageFormatBGR888X : SvDef::SVImageFormatMono8;
					_variant_t formatVar = format;

					deviceParams.CreateParameter(DeviceParamFileAcqImageFileName, fileNameVar);
					deviceParams.CreateParameter(DeviceParamFileAcqImageDirName, dirNameVar);
				
					if (loadingMode == SingleFileMode)
					{
						// File is supposed to be managed in Single File Mode
						svFile.SetFullFileName( pCameraObj->GetImageFilename().c_str() );
						svFiles.push_back(svFile);
					}
					deviceParams.CreateParameter(DeviceParamFileAcqLoadingMode, fileLoadingModeVar);
				
					deviceParams.CreateParameter(DeviceParamFileAcqImageWidth, widthVar);
					deviceParams.CreateParameter(DeviceParamFileAcqImageHeight, heightVar);
					deviceParams.CreateParameter(DeviceParamFileAcqImageFormat, formatVar);

					psvDevice->SetDeviceParameters( deviceParams );	// must be done before CreateBuffers

					// save the camera file (for future simulation mode)
					svFile.SetFullFileName( pCameraObj->GetCameraFile().c_str() );
			

					if ( 0 == SvUl::CompareNoCase(std::string(svFile.GetExtension()), std::string(cGigeCameraFileDefExt)) )
					{
						svFiles.push_back(svFile);
					}

					// cause the image size to be set
					psvDevice->LoadFiles(svFiles);

					// Get The Image Info
					SVImageInfoClass svImageInfo;
					psvDevice->GetImageInfo(&svImageInfo);

					// Destroy the buffers
					psvDevice->DestroyBuffers();

					// create the buffers
					psvDevice->CreateBuffers( svImageInfo );

					// Add the acquisition device
					bRet = pConfig->AddAcquisitionDevice( DigName.c_str(), svFiles, lightRef, lut, &deviceParams ) && bRet;
				}
			}
			else
			{
				svFile.SetFullFileName( pCameraObj->GetCameraFile().c_str() );
			
				if ( 0 == SvUl::CompareNoCase( svFile.GetExtension(), std::string(cGigeCameraFileDefExt) ) )
				{
					svFiles.push_back(svFile);
				}

				psvLight = nullptr;
				psvDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DigName.c_str() );
				if ( nullptr != psvDevice )
				{
					bool bGetLightReference = false;
					SVImageInfoClass svImageInfo;

					if ( pConfig->GetAcquisitionDevice( DigName.c_str(), psvFiles, psvLight, pLut, pDeviceParams ) )
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
					psvDevice->CreateBuffers( svImageInfo );

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
				}
				assert ( nullptr != psvLight || nullptr != pLut || nullptr != pDeviceParams ); // something is wrong if we get here.

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

				bRet = pConfig->AddAcquisitionDevice( DigName.c_str(), svFiles, *psvLight, *pLut, pDeviceParams ) && bRet;
			}
		}
	}
	return bRet;
}

bool CSVOConfigAssistantDlg::SendCameraDataToConfiguration()
{
	bool bRet = true;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	assert( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		return false;
	}

	SvIe::SVVirtualCamera* pCamera( nullptr );
	long lCfgCamCnt = pConfig->GetCameraCount();

	//check to see if camera still exist loop through in reverse order for deleting
	for (long l = lCfgCamCnt - 1; -1 < l; --l)
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
		std::string CameraDisplayName;

		for (int i = 0; i < iCamCnt; i++)
		{
			SVOCameraObjPtr pCameraObj = GetCameraObject(i);
			if( nullptr != pCameraObj )
			{
				CameraDisplayName = pCameraObj->GetCameraDisplayName();
				lCfgCamCnt = pConfig->GetCameraCount();

				for (long l = 0; l < lCfgCamCnt; ++l)
				{
					pCamera = pConfig->GetCamera(l);
					if ( nullptr != pCamera )
					{
						if( CameraDisplayName == pCamera->GetName() )
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

				bool bAddCamera = FALSE;

				if ( nullptr == pCamera )
				{
					pCamera = new SvIe::SVVirtualCamera;
					pCamera->SetName( CameraDisplayName.c_str() );
					bRet = nullptr != pCamera && bRet;
					bAddCamera = TRUE;
				}

				if ( nullptr != pCamera )
				{
					// move from editing camera object to configuration camera object
					int Digitizer = SvIe::SVDigitizerProcessingClass::Instance().getDigitizerID( pCameraObj->GetCameraID() );
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

					std::string DeviceName = BuildDigName( *pCameraObj );

					bRet = pCamera->Create( DeviceName.c_str() ) && bRet;

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

bool CSVOConfigAssistantDlg::SendTriggerDataToConfiguration()
{
	bool bRet = true;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	assert( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		return false;
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
		std::string TriggerDisplayName;
		std::string DeviceName;

		// Check for Triggers changed
		for (int i = 0; i < iTrgCnt; i++)
		{
			const SvTi::SVOTriggerObjPtr pTriggerObj( GetTriggerObject(i) );
			if( nullptr != pTriggerObj )
			{
				TriggerDisplayName = pTriggerObj->GetTriggerDisplayName();
				DeviceName = BuildTrgDig( *pTriggerObj );

				pTrigger = nullptr;
				lCfgTrgCnt = pConfig->GetTriggerCount();

				// Remove Triggers that have changed
				for ( long l = lCfgTrgCnt - 1; -1 < l; l-- )
				{
					pTrigger = pConfig->GetTrigger(l);
					if ( nullptr != pTrigger )
					{
						if ( TriggerDisplayName == pTrigger->GetName() )
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

				bool bAddTrigger = FALSE;

				if ( nullptr == pTrigger )
				{
					pTrigger = new SvTi::SVTriggerObject;
					pTrigger->SetName( TriggerDisplayName.c_str() );
				
					bRet = nullptr != pTrigger && bRet;
					bAddTrigger = TRUE;
				}

				if ( nullptr != pTrigger )
				{
					if (pTriggerObj->IsSoftwareTrigger())
					{
						pTrigger->SetSoftwareTriggerPeriod(pTriggerObj->GetTimerPeriod());
					}

					SvTh::SVTriggerClass* psvDevice = SvTi::SVTriggerProcessingClass::Instance().GetTrigger( DeviceName.c_str() );

					if ( nullptr != psvDevice )
					{
						bRet = pTrigger->Create(psvDevice) && bRet;
					}

					if ( bAddTrigger )
					{
						pTrigger->getDevice()->setDigitizerNumber(pTriggerObj->GetTriggerDigNumber());
						bRet = pConfig->AddTrigger(pTrigger) && bRet;
					}
				}
				pTrigger = nullptr;
			}
		}
	}
	return bRet;
}

bool CSVOConfigAssistantDlg::RenameInspectionObjects(LPCTSTR InspectionName, LPCTSTR NewInspectionName)
{
	bool bRet = true;
	if( InspectionName != NewInspectionName )
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
					if( pPPQObject->GetAttachedInspection(j) == InspectionName)
					{
						bRet &= pPPQObject->DetachInspectionFromPPQ( InspectionName );
						bRet &= pPPQObject->AttachInspectionToPPQ( NewInspectionName );
					}
				}
			}
		}
	}
	return bRet;
}

SVOPPQObjPtr CSVOConfigAssistantDlg::GetPPQObjectByInspectionName(const std::string& rInspectionName)
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
				std::string Name = pPPQObj->GetAttachedInspection(j);
				if( Name == rInspectionName.c_str() )
				{
					bFound = true;
				}
			}
		}
	}
	return pPPQObj;
}

bool CSVOConfigAssistantDlg::SendInspectionDataToConfiguration()
{
	bool bRet = true;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	assert( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		return false;
	}

	long lCfgInsCnt = pConfig->GetInspectionCount();
	///Loop through in reverse order for deleting
	for (long l = lCfgInsCnt -1; -1 < l; --l)
	{
		SVInspectionProcess* pInspection = pConfig->GetInspection(l);
		if ( nullptr != pInspection )
		{
			//Inspection has the old name by now
			const SVOInspectionObjPtr pInspectionObj( GetInspectionObjectByLabel( pInspection->GetName()) );
			bool bDeleteInspect = FALSE;

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
			bRet = false;
		}
	}

	int iInsCnt = m_InspectList.GetInspectionListCount();
	if ( iInsCnt > 0 )
	{
		std::string Key;
		std::string inspectionName;
		std::string ToolsetImage;
		std::string NewDisableMethod;
		bool EnableAuxiliaryExtent;

		for ( int i = 0; i < iInsCnt; i++ )
		{
			const SVOInspectionObjPtr pInspectionObj( GetInspectionObject(i) );
			if( nullptr != pInspectionObj )
			{
				Key = pInspectionObj->GetInspectionLabelName();
				inspectionName = pInspectionObj->GetInspectionName();
				ToolsetImage = pInspectionObj->GetToolsetImage();
				NewDisableMethod = pInspectionObj->GetNewDisableMethodString();
				EnableAuxiliaryExtent = (1 == pInspectionObj->GetEnableAuxiliaryExtent());

				lCfgInsCnt = pConfig->GetInspectionCount();

				SVInspectionProcess* pInspection{nullptr};
				for (long l = 0; l < lCfgInsCnt; ++l)
				{
					pInspection = pConfig->GetInspection(l);
					if (nullptr != pInspection)
					{
						if ( Key == pInspection->GetName() )
						{
							pInspection->SetName( inspectionName.c_str() );

							pConfig->RenameOutputListInspectionNames( Key.c_str(), inspectionName.c_str() );

							//rename in configuration
							pInspection->OnObjectRenamed(*pInspection, Key);

							TheSVObserverApp.OnUpdateAllIOViews();
							break;
						}
						pInspection = nullptr;
					}
					else
					{
						bRet = false;
					}
				}

				if ( nullptr == pInspection )
				{
					const std::string& importFilename = pInspectionObj->GetImportFilename();
					if (!importFilename.empty())
					{
						SVInspectionImportHelper importer  = SVInspectionImportHelper(std::string(importFilename), std::string(inspectionName), std::string(ToolsetImage));
						std::string title = _T( "Importing Inspection..." );
						SVImportProgress<SVInspectionImportHelper> progress(importer, title.c_str());
						progress.DoModal();

						HRESULT hr = progress.GetStatus();
						if (S_OK == hr)
						{
							if (SvDef::InvalidObjectId != importer.info.m_inspectionId)
							{
								// Add info to list
								m_ImportedInspectionInfoList.push_back(importer.info);
								SVObjectClass* pObject(nullptr);
								SVObjectManagerClass::Instance().GetObjectByIdentifier(importer.info.m_inspectionId, pObject);

								bRet = (nullptr != pObject);

								if( bRet )
								{
									pInspection = dynamic_cast<SVInspectionProcess*> (pObject);
									RenameInspectionObjects(Key.c_str(), inspectionName.c_str());
									bRet = pConfig->AddInspection( pInspection );

									SVOPPQObjPtr pPPQObj = GetPPQObjectByInspectionName(inspectionName);
									if( nullptr != pPPQObj )
									{
										pPPQObj->SetImportedInputList(importer.info.m_inputList);
									}

									bool bNewDisableMethod = pInspection->IsNewDisableMethodSet();
									NewDisableMethod = (bNewDisableMethod) ? _T( "Method 2" ): _T( "Method 1" );
									EnableAuxiliaryExtent = pInspection->getEnableAuxiliaryExtent();
								}
							}
							resolveGlobalConflicts( importer.GlobalConflicts );
						}
						else
						{
							SvDef::StringVector msgList;
							msgList.push_back(SvUl::Format(_T("%d"), hr));
							SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
							Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Config_InspectionImportFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10140);
						}
						pInspectionObj->ClearImportFilename();
					}
					else
					{
						SVObjectManagerClass::Instance().ConstructObject(SvPb::InspectionProcessClassId, pInspection );

						bRet = (nullptr != pInspection);

						if( bRet )
						{
							pInspection->SetName( inspectionName.c_str() );
							RenameInspectionObjects(Key.c_str(), inspectionName.c_str());

							pInspection->SetToolsetImage(ToolsetImage.c_str());

							bRet = pInspection->CreateInspection(inspectionName.c_str());

							if( bRet )
							{
								bRet = pConfig->AddInspection( pInspection );
							}
						}
					}
				}

				if ( nullptr != pInspection )
				{
					pInspection->SetToolsetImage(ToolsetImage.c_str());

					pInspection->SetNewDisableMethod( NewDisableMethod == _T( "Method 2" ) );

					bool PrevEnable = pInspection->getEnableAuxiliaryExtent();
					pInspection->setEnableAuxiliaryExtent( EnableAuxiliaryExtent );
					if (PrevEnable && !EnableAuxiliaryExtent)
					{
						pInspection->resetAllObjects();
					}
				}
				pInspection = nullptr;
			}
		}
	}
	return bRet;
}

bool CSVOConfigAssistantDlg::SendPPQAttachmentsToConfiguration(SVPPQObjectPtrVector& rPPQsToDelete)
{
	bool bRet = true;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	assert( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		return false;
	}

	// delete old PPQs
	for ( auto pPPQ : rPPQsToDelete)
	{
		delete pPPQ;
	}
	rPPQsToDelete.clear();

	long lCPPQCnt = pConfig->GetPPQCount();

	int iPPQCnt = m_PPQList.GetPPQListCount();
	if ( iPPQCnt > 0) 
	{
		std::string Key;
		SVPPQObject* pPPQ( nullptr );

		for (int i = 0; i < iPPQCnt; i++)
		{
			SVOPPQObjPtr pPPQObj = GetPPQObject(i);
			if( nullptr == pPPQObj)
			{
				continue;
			}

			Key = pPPQObj->GetPPQName();
			bool bNew = FALSE;

			for ( long j = lCPPQCnt -1; -1 < j; j-- )
			{
				pPPQ = pConfig->GetPPQ(j);
				
				if ( nullptr != pPPQ )
				{
					if ( Key == pPPQ->GetName() )
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
				pPPQ->SetName(Key.c_str());
				bNew = TRUE;
			}

			if ( nullptr != pPPQ )
			{
				std::string PPQCameraName;

				pPPQ->SetPPQOutputMode((SvDef::SVPPQOutputModeEnum)pPPQObj->GetPPQMode());

				pPPQ->SetPPQLength(pPPQObj->GetPPQLength());
				pPPQ->SetResetDelay(pPPQObj->GetPPQOutputResetDelay());
				pPPQ->SetOutputDelay(pPPQObj->GetPPQOutputDelayTime());
				pPPQ->SetMaintainSourceImages( pPPQObj->GetMaintainSourceImageProperty() );
				pPPQ->setMaxProcessingOffset4Interest(pPPQObj->GetMaxProcessingOffsetProperty());
				pPPQ->SetInspectionTimeout( pPPQObj->GetInspectionTimeout() );
				pPPQ->SetConditionalOutputName( pPPQObj->GetConditionalOutputName() );

				int iAttachedCamCnt = pPPQObj->GetAttachedCameraCount();
				int iAttachedInsCnt = pPPQObj->GetAttachedInspectionCount();

				for (int j = 0; j < iAttachedCamCnt; ++j)
				{
					long lPosition = 0;

					PPQCameraName = pPPQObj->GetAttachedCamera(j);

					SvIe::SVVirtualCameraPtrVector cameraVector = pPPQ->GetVirtualCameras();

					for(auto* pCamera : cameraVector)
					{
						if ( nullptr != pCamera )
						{
							if ( PPQCameraName == pCamera->GetName() )
							{
								pPPQ->GetCameraPPQPosition( lPosition, pCamera );

								pPPQ->DetachCamera( pCamera );

								break;
							}

							pCamera = nullptr;
						}
					}

					long lCfgAttachedCam = pConfig->GetCameraCount();

					for (long k = 0; k < lCfgAttachedCam; ++k)
					{
						SvIe::SVVirtualCamera* pCamera = pConfig->GetCamera(k);
						if ( nullptr != pCamera )
						{
							if ( PPQCameraName == pCamera->GetName() )
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

				std::string PPQTrigger = pPPQObj->GetAttachedTriggerName();

				if ( !PPQTrigger.empty() )
				{
					long lSize = pConfig->GetTriggerCount();
					for (long l = 0; l < lSize; ++l)
					{
						SvTi::SVTriggerObject* pTrigger = pConfig->GetTrigger(l);
						if ( nullptr != pTrigger )
						{
							if ( PPQTrigger ==pTrigger->GetName() )
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

				for (int j = 0; j < iAttachedInsCnt; j++)
				{
					bool bFound = false;

					std::string PpqInspectionName = pPPQObj->GetAttachedInspection(j);
					const SVOInspectionObjPtr pInspectionObj = GetInspectionObjectByLabel(PpqInspectionName.c_str());
					if( nullptr != pInspectionObj )
					{
						long lInsCnt{0L};
						pPPQ->GetInspectionCount(lInsCnt);
						for (long l = 0; l < lInsCnt; ++l)
						{
							SVInspectionProcess* pInspection(nullptr);
							bRet = pPPQ->GetInspection(l,pInspection) && bRet;
								
							if ( ( nullptr != pInspection ) && (!pInspectionObj->IsNewInspection()) )
							{
								if (pInspectionObj->GetInspectionName() == pInspection->GetName() )
								{
									bFound = true;
									break;
								}
								pInspection = nullptr;
							}
						}
					}

					if ( !bFound )
					{
						long lInsCnt = pConfig->GetInspectionCount();
						for ( long l = 0; l < lInsCnt; ++l)
						{
							SVInspectionProcess* pInspection = pConfig->GetInspection(l);
							if ( nullptr != pInspection )
							{
								if ( PpqInspectionName ==pInspection->GetName() )
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
				}

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

bool CSVOConfigAssistantDlg::SendDataToConfiguration()
{
	bool bRet = TRUE;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	assert( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		return false;
	}

	SVPPQObjectPtrVector aPPQsToDelete;

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
	pConfig->AttachAcqToTriggers();

	if ( m_bNewConfiguration )
	{
		pConfig->SetConfigurationLoaded();
	}
	return bRet;
}

bool CSVOConfigAssistantDlg::GetConfigurationForExisting()
{
	bool bRet = TRUE;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//Without valid config pointer don't need to go further
	assert( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		return false;
	}

	//make sure list are clean...
	m_CameraList.ResetContent();
	m_TriggerList.ResetContent();
	m_InspectList.ResetContent();
	m_PPQList.ResetContent();
	m_UsedTriggers.clear();
	m_UsedInspections.clear();
	m_InspectionNamesUsed.clear();
	m_InspectionLabelsUsed.clear();

	long lCfgCameraCnt;
	long lCfgTriggerCnt;
	long lCfgInspectCnt;
	long lCfgPPQCnt;
	std::string CameraName;
	std::string TriggerName;
	std::string InspectLabel;
	std::string InspectName;
	std::string PPQName;
	int iDigNumber;
	int CameraID;
	int iChannel;
	std::string CameraFileName;

	SvTi::SVTriggerObject* pcfgTrigger( nullptr );
	SVInspectionProcess* pcfgInspection( nullptr );
	SVPPQObject* pcfgPPQ( nullptr );

	m_lConfigurationType = pConfig->GetProductType();
	//load camera information
	lCfgCameraCnt = pConfig->GetCameraCount();
	for (long lCam = 0; lCam < lCfgCameraCnt; ++lCam)
	{
		SvIe::SVVirtualCamera* pcfgCamera = pConfig->GetCamera(lCam);
		if ( nullptr != pcfgCamera )
		{
			CameraName = pcfgCamera->GetName();
			if ( nullptr != pcfgCamera->GetAcquisitionDevice())
			{
				iDigNumber = pcfgCamera->GetAcquisitionDevice()->DigNumber();
				iChannel = pcfgCamera->GetAcquisitionDevice()->Channel();
				CameraID = pcfgCamera->getCameraID();

				CameraFileName.clear();
				
				long lSize;
				SVFileNameClass oCamFile;
				pcfgCamera->GetAcquisitionDevice()->GetFileNameArraySize( lSize );
				for ( long lFile = 0; lFile < lSize && CameraFileName.empty(); lFile++)
				{
					pcfgCamera->GetAcquisitionDevice()->GetFileName( lFile, oCamFile );
					std::string Extension = oCamFile.GetExtension();				

					if ( 0 == SvUl::CompareNoCase( Extension, std::string(cGigeCameraFileDefExt) ) && CameraFileName.empty() )
					{
						CameraFileName = oCamFile.GetFullFileName();
					}
				}

				//Add Camera Name, Camera Files and Camera Device Params to tmp list in case of a cancel
				m_CameraList.AddCameraToList(CameraName.c_str(), iDigNumber, iChannel, CameraID); 
				m_TmpCameraList.AddCameraToList(CameraName.c_str(), iDigNumber, iChannel, CameraID); 
				m_CameraList.SetCameraFile(CameraName.c_str(), CameraFileName.c_str());
				m_TmpCameraList.SetCameraFile(CameraName.c_str(), CameraFileName.c_str());

				SVDeviceParamCollection l_Params;
				SVDeviceParamCollection l_CameraFileParams;
				pcfgCamera->GetAcquisitionDevice()->GetDeviceParameters( l_Params );
				pcfgCamera->GetAcquisitionDevice()->GetCameraFileParameters( l_CameraFileParams );

				m_CameraList.SetCameraDeviceParams( CameraName.c_str(), l_Params, l_CameraFileParams );
				m_TmpCameraList.SetCameraDeviceParams( CameraName.c_str(), l_Params, l_CameraFileParams );

				SVOCameraObjPtr pCameraObj = m_CameraList.GetCameraObjectByName(CameraName.c_str());
				if( nullptr != pCameraObj )
				{
					// move from configuration (SVVirtualCamera) camera object to editing (CSVOCameraObj) camera object
					pCameraObj->SetFileAcquisitionMode(pcfgCamera->IsFileAcquisition());
					pCameraObj->SetFileLoadingMode(pcfgCamera->GetFileLoadingMode());
					pCameraObj->SetImageFilename( std::string(pcfgCamera->GetImageFilename()) );
					pCameraObj->SetImageDirectoryName( std::string(pcfgCamera->GetImageDirectoryName()) );
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
	for (long lTrg = 0; lTrg < lCfgTriggerCnt; ++lTrg)
	{
		pcfgTrigger = pConfig->GetTrigger(lTrg);
		bRet = (nullptr != pcfgTrigger) && bRet;
		if ( nullptr != pcfgTrigger && nullptr != pcfgTrigger->getDevice() )
		{
			TriggerName = pcfgTrigger->GetName();
			iDigNumber = pcfgTrigger->getDevice()->getDigitizerNumber();
			if (iDigNumber < 0)
			{
				// use the numeric at the end of the name (it's one based and we want zero based)
				// find the last underscore
				size_t Pos = TriggerName.rfind(_T('_'));
				if( std::string::npos != Pos )
				{
					std::string TriggerNumber = SvUl::Right( TriggerName, TriggerName.size() - Pos + 1);
					iDigNumber  = _ttoi(TriggerNumber.c_str());

					// make it zero based
					if (iDigNumber > 0)
					{
						iDigNumber--;
					}
				}
			}
			m_TriggerList.AddTriggerToList(std::string(TriggerName), iDigNumber);

			// Add Software trigger flag and interval here
			const SvTi::SVOTriggerObjPtr pTriggerObj( m_TriggerList.GetTriggerObjectByName(std::string(TriggerName)) );
			if( nullptr != pTriggerObj )
			{
				pTriggerObj->SetSoftwareTrigger(pcfgTrigger->getType() == SvDef::TriggerType::SoftwareTrigger);
				pTriggerObj->SetTimerPeriod(pcfgTrigger->GetSoftwareTriggerPeriod());
			}
		}
	}
	//end of trigger section
	
	//load inspection section...
	CString sDisable;
	lCfgInspectCnt = pConfig->GetInspectionCount();
	for (long lIns = 0; lIns < lCfgInspectCnt; ++lIns)
	{
		pcfgInspection = pConfig->GetInspection(lIns);
		if ( nullptr != pcfgInspection )
		{
			InspectLabel = pcfgInspection->GetName();  // label name
			InspectName = pcfgInspection->GetName();

			m_InspectList.AddInspectionToList(InspectLabel.c_str(), InspectName.c_str(), false);
			m_InspectionNamesUsed.push_back( InspectName );
			m_InspectionLabelsUsed.push_back( InspectLabel );

			// Determine which image is the main image
			std::string ToolsetImage;
			ToolsetImage = pcfgInspection->GetToolsetImage();
			m_InspectList.SetToolsetImage( InspectLabel.c_str(), ToolsetImage.c_str() );
			
			m_InspectList.SetColor( InspectLabel.c_str(), pcfgInspection->IsColorCamera() );

			if( pcfgInspection->IsNewDisableMethodSet() )
			{
				sDisable = _T( "Method 2" );
			}// end if
			else
			{
				sDisable = _T( "Method 1" );
			}// end else

			m_InspectList.SetNewDisableMethod( InspectLabel.c_str(), sDisable );

			m_InspectList.SetShowAuxExtent( InspectLabel.c_str() );

			// Enable Auxiliary Extent
			long l_lEnable = 0;
			if( pcfgInspection->getEnableAuxiliaryExtent() )
			{
				l_lEnable = 1;
			}
			else
			{
				l_lEnable = 0;
			}
			m_InspectList.SetEnableAuxiliaryExtent( InspectLabel.c_str(), l_lEnable );
		}
		else
		{
			bRet = FALSE;
		}
	}
	//end of inspection section...
	
	//load PPQ section...
	lCfgPPQCnt = pConfig->GetPPQCount();

	for (long lPpq = 0; lPpq < lCfgPPQCnt; ++lPpq)
	{
		pcfgPPQ = pConfig->GetPPQ(lPpq);
		if ( nullptr != pcfgPPQ )
		{
			PPQName = pcfgPPQ->GetName();
			m_PPQList.AddPPQToList(PPQName.c_str());
			long lppqIns;
			pcfgPPQ->GetInspectionCount(lppqIns);

			SvIe::SVVirtualCameraPtrVector cameraVector = pcfgPPQ->GetVirtualCameras();

			for(auto* pcfgCamera : cameraVector)
			{
				if ( nullptr != pcfgCamera && (nullptr != pcfgCamera->GetAcquisitionDevice()) )
				{
					m_PPQList.AttachCameraToPPQ(PPQName.c_str(), pcfgCamera->GetName());
				}
			}

			for (long lpIns = 0; lpIns <lppqIns; ++lpIns)
			{
				pcfgInspection = nullptr;
				bRet = pcfgPPQ->GetInspection(lpIns,pcfgInspection) && bRet;
				if ( nullptr != pcfgInspection )
				{
					m_PPQList.AttachInspectToPPQ(PPQName.c_str(), pcfgInspection->GetName());
					AddUsedInspect(pcfgInspection->GetName());
				}
			}

			pcfgTrigger = pcfgPPQ->GetTrigger();
			if ( (nullptr != pcfgTrigger ) && (nullptr != pcfgTrigger->getDevice()) )
			{
				m_PPQList.AttachTriggerToPPQ(PPQName.c_str(), pcfgTrigger->GetName());
				AddUsedTrigger(pcfgTrigger->GetName());
			}
			//set properties for PPQ

			SVOPPQObjPtr pPPQObj( GetPPQObjectByName(PPQName.c_str()) );
			if( nullptr != pPPQObj )
			{
				pPPQObj->SetPPQLength(pcfgPPQ->getPPQLength());
				pPPQObj->SetPPQMode( static_cast<int> (pcfgPPQ->getPPQOutputMode()));
				pPPQObj->SetPPQOutputDelayTime(pcfgPPQ->getOutputDelay());
				pPPQObj->SetPPQOutputResetDelay(pcfgPPQ->getResetDelay());
				pPPQObj->SetMaintainSourceImageProperty(pcfgPPQ->getMaintainSourceImages());
				pPPQObj->SetMaxProcessingOffsetProperty(pcfgPPQ->getMaxProcessingOffset4Interest());
				pPPQObj->SetInspectionTimeout(pcfgPPQ->getInspectionTimeout());
				pPPQObj->SetConditionalOutputName(pcfgPPQ->GetConditionalOutputName());

				// Get List Of Inputs
				SVNameObjectIdPairList availableInputs;
				for (const auto rEntry : pcfgPPQ->GetUsedInputs())
				{
					if (IO_DIGITAL_INPUT == rEntry->m_ObjectType)
					{
						if (!IsNonIOSVIM(GetProductType()))
						{
							availableInputs.push_back(std::make_pair(rEntry->getObject()->GetName(), rEntry->m_IOId));
						}
					}
					else if (IO_CAMERA_DATA_INPUT == rEntry->m_ObjectType)
					{
						// check for Camera Input Line State...
						if (nullptr != rEntry->getObject())
						{
							if (rEntry->getObject()->GetEmbeddedID() == SvPb::CameraTriggerLineInStateEId)
							{
								// Only if the camera supports it ?
								availableInputs.push_back(std::make_pair(rEntry->getObject()->GetName(), rEntry->m_IOId));
							}
						}
					}
				}
				// make list of Name/objectId pairs
				pPPQObj->SetAvailableInputsForConditionalOutput(availableInputs);
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

bool CSVOConfigAssistantDlg::ItemChanged(int iItemDlg, LPCTSTR LabelName, int iAction)
{
	bool bRet = FALSE;
	SVOInspectionObjPtr pInspectionObj;
	SVOPPQObjPtr pPPQObj;
	int iObjCnt = 0;

	std::string Message;

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
						Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", CAMERA_NONE_DEFINED);
						RemoveMessageFromList(Message.c_str());
						//delete message
					}
					SVOCameraObjPtr pCameraObj = GetCameraObjectByName(LabelName);
					if( nullptr != pCameraObj )
					{
						if (!pCameraObj->IsCameraObjOk())
						{   // check to see if camera files are valid.
							// if not, add message to list.
							Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCameraObj->GetCameraDisplayName().c_str(), CAMERA_INVALID_FILES);
							AddMessageToList(CAMERA_DLG,Message.c_str());
						}
					}
					break;
				}// end case ITEM_ACTION_NEW:

				case ITEM_ACTION_DELETE:
				{
					RemoveMessageFromList(BuildDisplayMessage(MESSAGE_TYPE_ERROR, LabelName, INVALID_CAMERA).c_str());

					RemoveAllObjectMessages(LabelName);
					if (iObjCnt < 1) 
					{
						m_bCameraGood = FALSE;
						Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, _T(""), CAMERA_NONE_DEFINED);
						// they removed the last one.  place message
						AddMessageToList(CAMERA_DLG,Message.c_str());
					}
					CameraDeletedCheckAgainstPPQ( LabelName );
					CameraDeletedCheckAgainstInspection( LabelName );
					break;
				}// end case ITEM_ACTION_DELETE:

				case ITEM_ACTION_PROP:
				{
					SVOCameraObjPtr pCameraObj = GetCameraObjectByName( LabelName );

					if( nullptr != pCameraObj )
					{
						RemoveFileAcquisitionMessages( pCameraObj->GetCameraDisplayName().c_str() );

						Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCameraObj->GetCameraDisplayName().c_str(), CAMERA_INVALID_FILES);

						if (pCameraObj->IsCameraObjOk())
						{
							RemoveMessageFromList(Message.c_str());
						
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
							AddMessageToList(CAMERA_DLG, Message.c_str());
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
							//The digitizer number can change after calling the camera manager so set the camera file as changed to reload
							int Digitizer = SvIe::SVDigitizerProcessingClass::Instance().getDigitizerID(pCameraObj->GetCameraID());
							pCameraObj->SetDigNumber(Digitizer);
							pCameraObj->SetCameraFileChanged();
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
						Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", TRIGGER_NONE_DEFINED);
						RemoveMessageFromList(Message.c_str());
					}
					break;
				}// end case ITEM_ACTION_NEW:

				case ITEM_ACTION_DELETE:
				{
					RemoveMessageFromList(BuildDisplayMessage(MESSAGE_TYPE_ERROR, LabelName, INVALID_TRIGGER).c_str());
					if (iObjCnt < 1)
					{
						m_bTriggerGood = FALSE;
						Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", TRIGGER_NONE_DEFINED);
						AddMessageToList(TRIGGER_DLG,Message.c_str());
					}
					TriggerDeletedCheckAgainstPPQ(LabelName);
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
					pInspectionObj = GetInspectionObjectByLabel(LabelName);
					if( nullptr != pInspectionObj )
					{
						pInspectionObj->SetNewInspection();
					}
					if (iObjCnt == 1)
					{
						m_bInspectionGood = TRUE;
						Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", INSPECT_NONE_DEFINED);
						RemoveMessageFromList(Message.c_str());
					}
					Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, LabelName, INSPECTION_ERROR);
					AddMessageToList(PPQ_DLG,Message.c_str());
					break;
				}// end case ITEM_ACTION_NEW:

				case ITEM_ACTION_DELETE:
				{
					m_bInspectionDeleted = TRUE;
					if (iObjCnt < 1)
					{
						m_bInspectionGood = FALSE;
						Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", INSPECT_NONE_DEFINED);
						AddMessageToList(INSPECT_DLG,Message.c_str());
					}
					Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, LabelName, INSPECTION_ERROR);
					RemoveMessageFromList(Message.c_str());
					Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, LabelName, TOOLSET_IMAGE_ERROR);
					RemoveMessageFromList(Message.c_str());
					Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR,LabelName, MESSAGE_INSPECTION_CAMERA_COLOR);
					RemoveMessageFromList( Message.c_str() );
					InspectionDeletedCheckAgainstPPQ(LabelName);
					break;
				}// end case ITEM_ACTION_DELETE:

				case ITEM_ACTION_PROP:
				{
					pInspectionObj = GetInspectionObjectByLabel(LabelName);
					if( nullptr != pInspectionObj )
					{
						if( !pInspectionObj->GetToolsetImage().empty() )
						{
							RemoveMessageFromList(BuildDisplayMessage(MESSAGE_TYPE_ERROR, pInspectionObj->GetInspectionName().c_str(), TOOLSET_IMAGE_ERROR).c_str());
						}
						SVOCameraObjPtr pCameraObj = GetCameraObjectByName( pInspectionObj->GetToolsetImage().c_str());
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
						Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", PPQ_NONE_DEFINED);
						RemoveMessageFromList(Message.c_str());
					}
					AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, LabelName, PPQ_NO_TRIGGER).c_str());
					AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, LabelName, PPQ_NO_CAMERA).c_str());
					AddMessageToList(PPQ_DLG,BuildDisplayMessage(MESSAGE_TYPE_WARNING, LabelName, PPQ_NO_INSPECT).c_str());
					break;
				}
				case ITEM_ACTION_DELETE:
				{
					Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, LabelName, PPQ_NO_TRIGGER);
					RemoveMessageFromList(Message.c_str());
					Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, LabelName, PPQ_NO_CAMERA);
					RemoveMessageFromList(Message.c_str());
					RemoveAllObjectMessages(LabelName);
					if (iObjCnt < 1)
					{
						m_bPPQGood = FALSE;
						Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", PPQ_NONE_DEFINED);
						AddMessageToList(PPQ_DLG,Message.c_str());
					}
					break;
				}
				case ITEM_PPQ_ADD_TRG:
				{
					pPPQObj = GetPPQObjectByName(LabelName);
					if (pPPQObj)
					{
						Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName().c_str(), PPQ_NO_TRIGGER);
						RemoveMessageFromList(Message.c_str());
					}
					break;
				}
				case ITEM_PPQ_DEL_TRG:
				{
					pPPQObj = GetPPQObjectByName(LabelName);
					if (pPPQObj)
					{
						Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName().c_str(), PPQ_NO_TRIGGER);
						AddMessageToList(PPQ_DLG,Message.c_str());
					}
					break;
				}
				case ITEM_PPQ_ADD_CAM:
				{
					pPPQObj = GetPPQObjectByName(LabelName);
					if (pPPQObj)
					{
						Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName().c_str(), PPQ_NO_CAMERA);
						RemoveMessageFromList(Message.c_str());
					}
					break;
				}
				case ITEM_PPQ_DEL_CAM:
				{
					pPPQObj = GetPPQObjectByName(LabelName);
					if (pPPQObj)
					{
						if ( pPPQObj->GetAttachedCameraCount() < 1 )
						{
							Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName().c_str(), PPQ_NO_CAMERA);
							AddMessageToList(PPQ_DLG,Message.c_str());
						}
						CameraDetachedCheckAgainstInspection( LabelName );
					}
					break;
				}
				case ITEM_PPQ_ADD_INS:
				{
					Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, LabelName, INSPECTION_ERROR);
					RemoveMessageFromList(Message.c_str());
					Message = BuildDisplayMessage(MESSAGE_TYPE_WARNING, LabelName, PPQ_NO_INSPECT);
					RemoveMessageFromList(Message.c_str());
					
					AddMessageToList(INSPECT_DLG,BuildDisplayMessage(MESSAGE_TYPE_ERROR, LabelName, TOOLSET_IMAGE_ERROR).c_str());

					break;
				}
				case ITEM_PPQ_DEL_INS:
				{
					std::string sInsLabelName = GetInspectionLabelFromName(LabelName);
					pInspectionObj = GetInspectionObjectByLabel(sInsLabelName.c_str());
					if( nullptr != pInspectionObj )
					{
						pInspectionObj->SetToolsetImage(_T(""));
					}
					Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, LabelName, INSPECTION_ERROR);
					AddMessageToList(PPQ_DLG,Message.c_str());
					RemoveUsedInspect(LabelName);
					break;
				}
				case ITEM_PPQ_DETACH_INS:
				{
					AddMessageToList(PPQ_DLG, BuildDisplayMessage(MESSAGE_TYPE_WARNING, LabelName, PPQ_NO_INSPECT).c_str());
					break;
				}
				case ITEM_PPQ_ATTACH_INS:
				{
					RemoveMessageFromList( BuildDisplayMessage(MESSAGE_TYPE_WARNING, LabelName, PPQ_NO_INSPECT).c_str() );
					break;
				}
				case ITEM_ACTION_PROP:
				{
					// check to see if property is valid...
					//if good remove message from list (if it is there)
					//if bad add message to list, if not already there
					//set errors
					pPPQObj = GetPPQObjectByName(LabelName);
					if ( pPPQObj )
					{
						long l_lPpqLength = pPPQObj->GetPPQLength();
						long l_lImageDepth = TheSVObserverApp.GetMaxPPQLength();
						if ( pPPQObj->GetMaintainSourceImageProperty() )
						{
							if ( l_lPpqLength <= l_lImageDepth )
							{
								// remove error message if exist
								std::string Msg = SvUl::Format( _T("%s%d"), PPQ_PROP_SRC_IMG_ERROR, l_lImageDepth);
								//create error display message
								RemoveMessageFromList(BuildDisplayMessage( MESSAGE_TYPE_ERROR, LabelName, Msg.c_str() ).c_str() );
							}
							else
							{
								// add error message if it does not exist
								std::string Msg = SvUl::Format( _T("%s%d"), PPQ_PROP_SRC_IMG_ERROR, l_lImageDepth);
								//create error display message
								AddMessageToList( PPQ_DLG, BuildDisplayMessage( MESSAGE_TYPE_ERROR, LabelName, Msg.c_str() ).c_str() );
							}
						}
					}
					break;
				}
				case ITEM_PPQ_PROP_SRC_IMG:
				{
					//special property case for the PPQ's
					pPPQObj = GetPPQObjectByName(LabelName);
					if ( pPPQObj )
					{
						if ( pPPQObj->GetMaintainSourceImageProperty() )
						{
							long l_lPpqLength = pPPQObj->GetPPQLength();
							long l_lImageDepth = TheSVObserverApp.GetMaxPPQLength();
							if ( l_lPpqLength > l_lImageDepth )
							{
								std::string Msg = SvUl::Format( _T("%s%d"), PPQ_PROP_SRC_IMG_ERROR, l_lImageDepth);
								//create error display message
								AddMessageToList( PPQ_DLG, BuildDisplayMessage( MESSAGE_TYPE_ERROR, LabelName, Msg.c_str() ).c_str() );
							}
						}
						else
						{
							// see if error conditions exist, if so remove it
							// maintain source image = false
							long l_lImageDepth = TheSVObserverApp.GetMaxPPQLength();
							std::string Msg = SvUl::Format( _T("%s%d"),PPQ_PROP_SRC_IMG_ERROR,l_lImageDepth);
							//create error display message
							RemoveMessageFromList(BuildDisplayMessage( MESSAGE_TYPE_ERROR, LabelName, Msg.c_str() ).c_str() );
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

std::string CSVOConfigAssistantDlg::BuildDisplayMessage(ConfigMsgTypeEnum iErrorFlag, LPCTSTR ObjectName, LPCTSTR Message)
{
	std::string Result;

	switch (iErrorFlag)
	{
		case MESSAGE_TYPE_ERROR:
		{
			Result = ERR_STR + std::string(ObjectName) + ' ' + Message;
			break;
		}
		case MESSAGE_TYPE_WARNING:
		{
			Result = WARNING_STR + std::string(ObjectName) + ' ' + Message;
			break;
		}
		default:
		{
			break;
		}
	}

	return Result;
}

void CSVOConfigAssistantDlg::TriggerDeletedCheckAgainstPPQ(LPCTSTR TriggerName)
{
	//see if Trigger is connected to any PPQ.  if so remove it.  and add message to list
	int iPPQCnt = GetPPQListCount();

	for (int i = 0; i < iPPQCnt; i++)
	{
		SVOPPQObjPtr pPPQObj( GetPPQObject(i) );

		if( nullptr != pPPQObj && pPPQObj->GetAttachedTriggerName() == TriggerName )
		{
			pPPQObj->DetachTriggerFromPPQ();
			pPPQObj->RemoveCameraInputConditionalOutput();
			std::string Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName().c_str(), PPQ_NO_TRIGGER);
			AddMessageToList(PPQ_DLG, Message.c_str());
		}
	}
}

void CSVOConfigAssistantDlg::CameraDeletedCheckAgainstPPQ(LPCTSTR CameraName)
{
	int iPPQCnt = GetPPQListCount();
	int iCamCnt;

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
			if( pPPQObj->GetAttachedCamera(iC) == CameraName )
			{
				pPPQObj->DetachCameraFromPPQ( CameraName );
				pPPQObj->RemoveCameraInputConditionalOutput();
				iCamCnt--;
			}
		}
		if (iCamCnt == 0)
		{
			AddMessageToList(PPQ_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pPPQObj->GetPPQName().c_str(), PPQ_NO_CAMERA).c_str());
		}
	}
}

void CSVOConfigAssistantDlg::CameraDeletedCheckAgainstInspection(LPCTSTR )
{
	long lSize;
	long l;

	lSize = GetInspectionListCount();
	for( l = 0; l < lSize; l++ )
	{
		const SVOInspectionObjPtr pInspectionObj( GetInspectionObject( l ) );
		
		if( nullptr != pInspectionObj && pInspectionObj->GetToolsetImage().empty() )
		{
			//Need to remove this message if in the list after Tool Set image has been reseted
			std::string Message( BuildDisplayMessage(MESSAGE_TYPE_ERROR, pInspectionObj->GetInspectionName().c_str(), MESSAGE_INSPECTION_CAMERA_COLOR) );
			RemoveMessageFromList( Message.c_str() );

			AddMessageToList( INSPECT_DLG, BuildDisplayMessage( MESSAGE_TYPE_ERROR, pInspectionObj->GetInspectionName().c_str(), TOOLSET_IMAGE_ERROR ).c_str() );
		}// end if
	}// end for
}// end CameraDeletedCheckAgainstInspection

void CSVOConfigAssistantDlg::CameraDetachedCheckAgainstInspection(LPCTSTR)
{
	long lSize;
	long l;

	lSize = GetInspectionListCount();
	for( l = 0; l < lSize; l++ )
	{
		const SVOInspectionObjPtr pInspectionObj( GetInspectionObject( l ) );

		if( nullptr != pInspectionObj && pInspectionObj->GetToolsetImage().empty() )
		{
			//Need to remove this message if in the list after Tool Set image has been reseted
			std::string Message( BuildDisplayMessage(MESSAGE_TYPE_ERROR, pInspectionObj->GetInspectionName().c_str(), MESSAGE_INSPECTION_CAMERA_COLOR) );
			RemoveMessageFromList( Message.c_str() );

			AddMessageToList( INSPECT_DLG, BuildDisplayMessage( MESSAGE_TYPE_ERROR, pInspectionObj->GetInspectionName().c_str(), TOOLSET_IMAGE_ERROR ).c_str() );
		}// end if
	}// end for
}// end CameraDetachedCheckAgainstInspection

void CSVOConfigAssistantDlg::InspectionDeletedCheckAgainstPPQ(LPCTSTR InspectName)
{
	int iPPQCnt = GetPPQListCount();
	int iInsCnt;

	for (int i = 0; i < iPPQCnt; i++)
	{
		SVOPPQObjPtr pPPQObj( GetPPQObject(i) );
		if( nullptr != pPPQObj )
		{
			iInsCnt = pPPQObj->GetAttachedInspectionCount();

			for (int iC = 0; iC < iInsCnt; iC++)
			{
				if( pPPQObj->GetAttachedInspection(iC) == InspectName )
				{
					pPPQObj->DetachInspectionFromPPQ(InspectName);
					iInsCnt--;
				}
			}
			if (iInsCnt == 0)
			{
				AddMessageToList( PPQ_DLG, BuildDisplayMessage(MESSAGE_TYPE_WARNING, pPPQObj->GetPPQName().c_str(), PPQ_NO_INSPECT).c_str() );
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
	int iCurSel = m_ctlMsgList.GetCurSel();
	if (iCurSel != LB_ERR)
	{
		int iPage = (int)m_ctlMsgList.GetItemData(iCurSel);
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

std::string CSVOConfigAssistantDlg::GetConfigurationName()
{
	return std::string(m_ConfigurationName);
}

void CSVOConfigAssistantDlg::OnChangeEditConfigurationName()
{
	UpdateData();
	m_ConfigurationName.TrimLeft();
	m_ConfigurationName.TrimRight();

	if (m_ConfigurationName.IsEmpty())
	{
		AddMessageToList(999,BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", CONFIG_NAME_ERR).c_str());
		m_bConfigName = FALSE;
	}
	else
	{
		RemoveMessageFromList(BuildDisplayMessage(MESSAGE_TYPE_ERROR, "", CONFIG_NAME_ERR).c_str());
		m_bConfigName = TRUE;
	}
	IsGood();
}

bool CSVOConfigAssistantDlg::GetNewConfigFlag()
{
	return m_bNewConfiguration;
}

bool CSVOConfigAssistantDlg::Modified()
{
	return m_bModified;
}

//
// If bTrigger is true then this function returns the max number of triggers allowed.
//
int CSVOConfigAssistantDlg::GetAllowedNumberOfDigs(bool bTrigger/* = FALSE*/)
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
					AddMessageToList(CAMERA_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pCameraObj->GetCameraDisplayName().c_str(), INVALID_CAMERA).c_str() );
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
				AddMessageToList(TRIGGER_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pTriggerObj->GetTriggerDisplayName(), INVALID_TRIGGER).c_str() );
			}
		}
	}
	m_Page1.SetupList();
	m_Page2.SetupList();
}

bool CSVOConfigAssistantDlg::IsInspectionOnPPQ(LPCTSTR PPQName, LPCTSTR InspectionLabel)
{
	bool bFound = FALSE;
	const SVOPPQObjPtr pPPQObj( GetPPQObjectByName(PPQName) );

	if( nullptr != pPPQObj )
	{
		int iInsCnt = pPPQObj->GetAttachedInspectionCount();

		for (int i = 0; ((i < iInsCnt) && (!bFound)); i++)
		{
			if( InspectionLabel == pPPQObj->GetAttachedInspection(i))
			{
				bFound = TRUE;
			}
		}
	}
	return bFound;
}

bool CSVOConfigAssistantDlg::IsCameraOnPPQ(LPCTSTR PPQName, LPCTSTR CameraName)
{
	bool bFound = FALSE;
	const SVOPPQObjPtr pPPQObj( GetPPQObjectByName(PPQName) );

	if( nullptr != pPPQObj )
	{
		int iCamCnt = pPPQObj->GetAttachedCameraCount();

		for (int i = 0; ( (i < iCamCnt) && (!bFound) ); i++)
		{
			if( CameraName == pPPQObj->GetAttachedCamera(i) )
			{
				bFound = TRUE;
			}
		}
	}
	return bFound;
}

bool CSVOConfigAssistantDlg::IsTriggerOnPPQ(LPCTSTR PPQName, LPCTSTR TriggerName)
{
	bool bFound( false );
	const SVOPPQObjPtr pPPQObj( GetPPQObjectByName(PPQName) );

	if( nullptr != pPPQObj )
	{
		if ( TriggerName == pPPQObj->GetAttachedTriggerName() )
		{
			bFound = true;
		}
	}
	return bFound;
}

void CSVOConfigAssistantDlg::LastInspectionLabelDeleted(LPCTSTR InspectionLabel)
{
	int iNum = atoi( SvUl::Mid( std::string(InspectionLabel), 11 ).c_str() );
	if ( iNum > m_iLastInspectionNum )
	{
		m_iLastInspectionNum = iNum;
	}
}

bool CSVOConfigAssistantDlg::CanInspectionNameBeUsed(LPCTSTR Name)
{
	bool Result( false );

	SvDef::StringVector::const_iterator Iter = std::find( m_InspectionNamesUsed.begin(), m_InspectionNamesUsed.end(), std::string(Name) );
	if( m_InspectionNamesUsed.end() == Iter)
	{
		Result = true;
	}
	return Result;
}

void CSVOConfigAssistantDlg::ClearMessages()
{
	m_ctlMsgList.ResetContent();

	OnChangeEditConfigurationName();
}

void CSVOConfigAssistantDlg::RemoveFileAcquisitionMessages( LPCTSTR CameraName )
{
	std::string FileAcquisitionNotAllowedMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, CameraName, MESSAGE_FILE_ACQUISITION_NOT_ALLOWED);
	std::string EmptyFileNameMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName, MESSAGE_FILE_ACQUISITION_MISSING_FILENAME);
	std::string EmptyDirNameMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName, MESSAGE_FILE_ACQUISITION_MISSING_DIRECTORY);
	std::string InvalidFileMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, CameraName, MESSAGE_FILE_ACQUISITION_INVALID_FILE);
	std::string InvalidDirMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, CameraName, MESSAGE_FILE_ACQUISITION_INVALID_DIRECTORY);
	std::string InvalidImageSizeMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName, MESSAGE_FILE_ACQUISITION_INVALID_IMAGE_SIZE);
	std::string ColorMismatchMessage =  BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName, MESSAGE_FILE_ACQUISITION_COLOR_MISMATCH);

	RemoveMessageFromList(FileAcquisitionNotAllowedMessage.c_str());
	RemoveMessageFromList(EmptyFileNameMessage.c_str());
	RemoveMessageFromList(EmptyDirNameMessage.c_str());
	RemoveMessageFromList(InvalidFileMessage.c_str());
	RemoveMessageFromList(InvalidDirMessage.c_str());
	RemoveMessageFromList(InvalidImageSizeMessage.c_str());
	RemoveMessageFromList(ColorMismatchMessage.c_str());
}

HRESULT CSVOConfigAssistantDlg::CheckCamera( SVOCameraObj& rCameraObj, bool SetFileParameters )
{
	std::string CameraName( rCameraObj.GetCameraDisplayName() );
	RemoveFileAcquisitionMessages( CameraName.c_str() );

	std::string DigName = BuildDigName( rCameraObj );
	SvIe::SVDigitizerProcessingClass::Instance().SetDigitizerColor( DigName.c_str(), rCameraObj.IsColor() );

	if( rCameraObj.IsFileAcquisition())
	{
		std::string EmptyFileNameMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName.c_str(), MESSAGE_FILE_ACQUISITION_MISSING_FILENAME);
		std::string EmptyDirNameMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName.c_str(), MESSAGE_FILE_ACQUISITION_MISSING_DIRECTORY);
		std::string InvalidFileMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, CameraName.c_str(), MESSAGE_FILE_ACQUISITION_INVALID_FILE);
		std::string InvalidDirMessage = BuildDisplayMessage(MESSAGE_TYPE_WARNING, CameraName.c_str(), MESSAGE_FILE_ACQUISITION_INVALID_DIRECTORY);
		std::string InvalidImageSizeMessage = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName.c_str(), MESSAGE_FILE_ACQUISITION_INVALID_IMAGE_SIZE);
		std::string ColorMismatchMessage =  BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName.c_str(), MESSAGE_FILE_ACQUISITION_COLOR_MISMATCH);

		// check that image size is not zero
		const SIZE& size = rCameraObj.GetFileImageSize();
		if (size.cx == 0 || size.cy == 0)
		{
			AddMessageToList( CAMERA_DLG, InvalidImageSizeMessage.c_str() );
		}

		// check that file or directory is valid
		long loadingMode = rCameraObj.GetFileLoadingMode();
		if (loadingMode == ContinuousMode || loadingMode == SingleIterationMode)
		{
			// check that directory exists
			std::string directoryName = rCameraObj.GetImageDirectoryName();
			if( !directoryName.empty() )
			{
				// strip off trailing slash or backslash
				std::string lastChar = SvUl::Right( directoryName, 1 );
				if (lastChar == _T("\\") || lastChar == _T("/"))
				{
					directoryName.erase( directoryName.size() - 1);
				}

				struct _stat buf;
				int result = _stat( directoryName.c_str(), &buf );
				
				if (result != 0)
				{
					AddMessageToList( CAMERA_DLG, InvalidDirMessage.c_str() );
				}
			}
			else
			{
				AddMessageToList( CAMERA_DLG, EmptyDirNameMessage.c_str() );
			}
		}
		else 
		{
			// check that File Exists
			if ( rCameraObj.GetImageFilename().empty() )
			{
				AddMessageToList( CAMERA_DLG, EmptyFileNameMessage.c_str() );
			}
			else
			{
				// check that File Exists
				if ( _access( rCameraObj.GetImageFilename().c_str(), 0 ) != 0 )
				{
					AddMessageToList( CAMERA_DLG, InvalidFileMessage.c_str() );
				}
			}
		}

		SVImageFile FileImage;
		std::string Name;

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
				AddMessageToList( CAMERA_DLG, ColorMismatchMessage.c_str() );
			}
		}
	}
	else
	{
		SvIe::SVAcquisitionClassPtr pDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DigName.c_str() );
		if(nullptr != pDevice)
		{
			SVDeviceParamCollection DeviceParams;

			//First save the current device parameters to the camera object
			pDevice->GetDeviceParameters( DeviceParams );
			rCameraObj.SetCameraDeviceParams( DeviceParams );

			pDevice->ReadCameraFile( rCameraObj.GetCameraFile(), DeviceParams );

			if( SetFileParameters )
			{
				rCameraObj.SetCameraFileParams( DeviceParams );
			}

			bool bDisplayWarnings = TheSVObserverApp.GetSVIMType() == GetProductType();
			std::string MessageIncorrectCamFile = BuildDisplayMessage(MESSAGE_TYPE_WARNING, CameraName.c_str(), MESSAGE_INCORRECT_CAM_FILE);
			RemoveMessageFromList(MessageIncorrectCamFile.c_str());
			if( DeviceParams.ParameterExists( DeviceParamVendorId ) || DeviceParams.ParameterExists( DeviceParamModelName ) )
			{
				// do this only if True SVIM type matches selected product type - do warning
				if( bDisplayWarnings )
				{
					try
					{
						if( S_OK != pDevice->IsValidCameraFileParameters( DeviceParams ) )
						{
							AddMessageToList( CAMERA_DLG, MessageIncorrectCamFile.c_str() );
						}
					}
					catch( const SvStl::MessageContainer& )
					{
						AddMessageToList( CAMERA_DLG, MessageIncorrectCamFile.c_str() );
					}
				}// end if ( bDisplayWarnings )
			}

			std::string MessageNotColorCamFile = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName.c_str(), MESSAGE_NOT_COLOR_CAM_FILE);
			std::string MessageNotMonoCamFile = BuildDisplayMessage(MESSAGE_TYPE_ERROR, CameraName.c_str(), MESSAGE_NOT_MONO_CAM_FILE);
			std::string MessageNoColorCam = BuildDisplayMessage(MESSAGE_TYPE_WARNING, CameraName.c_str(), MESSAGE_NOT_COLOR_CAM);

			RemoveMessageFromList(MessageNotColorCamFile.c_str());
			RemoveMessageFromList(MessageNotMonoCamFile.c_str());
			RemoveMessageFromList(MessageNoColorCam.c_str());
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
						AddMessageToList(CAMERA_DLG, MessageNotColorCamFile.c_str());
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
								AddMessageToList(CAMERA_DLG, MessageNoColorCam.c_str());
							}
						}
					}// end if ( l_HardwareCapabilities.ParameterExists( DeviceParamCameraFormats ) )
				}// end if ( params.ParameterExists( DeviceParamCameraFormats ) )
			}
			else
			{
				if (DeviceParams.ParameterExists(DeviceParamCameraFormats))
				{
					const SVCameraFormatsDeviceParam* pParam = DeviceParams.Parameter(DeviceParamCameraFormats).DerivedValue(pParam);
					if (!pParam->SupportsMono())
					{
						AddMessageToList(CAMERA_DLG, MessageNotMonoCamFile.c_str());
					}
				}
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
			if( pInspectionObj->GetToolsetImage() == rCameraObj.GetCameraDisplayName() )
			{
				std::string Message( BuildDisplayMessage(MESSAGE_TYPE_ERROR, pInspectionObj->GetInspectionLabelName().c_str(), MESSAGE_INSPECTION_CAMERA_COLOR) );
				RemoveMessageFromList( Message.c_str() );

				if ( !pInspectionObj->GetToolsetImage().empty() )
				{
					//If an existing or imported inspection then inspection color is not allowed to be changed
					if( !pInspectionObj->IsNewInspection() || !pInspectionObj->GetImportFilename().empty() )
					{
						if( rCameraObj.IsColor() != pInspectionObj->IsColor() )
						{
							AddMessageToList( INSPECT_DLG, Message.c_str() );
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

bool CSVOConfigAssistantDlg::CheckTrigger( const SvTi::SVOTriggerObj& rTriggerObj)
{
	bool bRet = true;
	std::string TriggerName = rTriggerObj.GetTriggerDisplayName();
	std::string MessageNoSoftwareTriggerAllowed = BuildDisplayMessage(MESSAGE_TYPE_ERROR, TriggerName.c_str(), MESSAGE_SOFTWARE_TRIGGER_NOT_ALLOWED);

	if ( rTriggerObj.IsSoftwareTrigger() )
	{
		// check if Software Trigger is allowed
		bRet = IsSoftwareTriggerAllowed( TriggerName.c_str() );
		if (!bRet)
		{
			// add message
			AddMessageToList(TRIGGER_DLG, MessageNoSoftwareTriggerAllowed.c_str() );
		}
		else
		{
			RemoveMessageFromList( MessageNoSoftwareTriggerAllowed.c_str() );
		}
	}
	else
	{
		RemoveMessageFromList( MessageNoSoftwareTriggerAllowed.c_str() );
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
				AddMessageToList(TRIGGER_DLG, BuildDisplayMessage( MESSAGE_TYPE_WARNING, "", MESSAGE_ONE_INVERT_CONTROL ).c_str() );
			}
			else
			{
				RemoveMessageFromList( BuildDisplayMessage( MESSAGE_TYPE_WARNING, "", MESSAGE_ONE_INVERT_CONTROL ).c_str() );
			}
		}
	}
}

void CSVOConfigAssistantDlg::SetIOBoardCapabilities(const SVIOBoardCapabilities& rCapable)
{
	m_svCapabilities = rCapable;
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

std::string CSVOConfigAssistantDlg::GetNameFromProductID( SVIMProductEnum productID )
{
	std::string result{_T("Unknown")};

	for(const auto& rEntry : m_ProductStringVector)
	{
		if(rEntry.first == productID)
		{
			result = rEntry.second;
			break;
		}
	}
	return result;
}

SVIMProductEnum CSVOConfigAssistantDlg::GetProductIDFromName( const std::string& rName )
{
	SVIMProductEnum result{SVIM_PRODUCT_INVALID_TYPE};

	for (const auto& rEntry : m_ProductStringVector)
	{
		if (rEntry.second == rName)
		{
			result = rEntry.first;
			break;
		}
	}

	return result;
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
			SvIe::SVAcquisitionClassPtr psvDevice;
			SVFileNameArrayClass svFiles;
			SVFileNameClass svFile;

			if( nullptr != pCameraObj )
			{
				psvDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( BuildDigName( *pCameraObj ).c_str() );
				if( nullptr != psvDevice )
				{
					svFile.SetFullFileName( pCameraObj->GetCameraFile().c_str() );
					svFiles.push_back(svFile);
					psvDevice->LoadFiles(svFiles);
					
					psvDevice->SetDeviceParameters(pCameraObj->GetCameraDeviceParams());
				}
			}
		}
	}//if digital SVIM

	CDialog::OnCancel();
}

void CSVOConfigAssistantDlg::resolveGlobalConflicts( SvDef::GlobalConflictPairVector& rGlobalConflicts )
{
	if( 0 < rGlobalConflicts.size() )
	{
		SvOg::GlobalConstantConflictDlg ConflictDlg( rGlobalConflicts, AfxGetMainWnd() );
		if( IDOK == ConflictDlg.DoModal() )
		{
			SvDef::GlobalConflictPairVector::const_iterator Iter( rGlobalConflicts.cbegin() );

			while( rGlobalConflicts.cend() != Iter )
			{
				if( Iter->second.m_Selected )
				{
					SvVol::BasicValueObject* pGlobalObject(nullptr);

					pGlobalObject = dynamic_cast<SvVol::BasicValueObject*> ( SVObjectManagerClass::Instance().GetObject(  Iter->first.m_objectId ) );
					if( nullptr != pGlobalObject )
					{
						pGlobalObject->setValue( Iter->second.m_Value );
						pGlobalObject->setDescription( Iter->second.m_Description.c_str() );
						pGlobalObject->SetObjectAttributesAllowed( SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::OverwriteAttribute );
						if( Iter->second.m_Value.vt == VT_BSTR )
						{
							pGlobalObject->SetObjectAttributesAllowed( SvPb::selectableForEquation, SvOi::SetAttributeType::RemoveAttribute );
						}
					}
				}
				++Iter;
			}
		}
	}
}
