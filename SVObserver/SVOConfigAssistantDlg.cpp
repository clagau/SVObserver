//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVIOLibrary/PlcOutputObject.h"
#include "SVIOLibrary/SVOutputObjectList.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFile.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFileLoader.h"
#include "CameraLibrary/SVboolValueDeviceParam.h"
#include "SVFileAcquisitionDevice/SVFileAcquisitionLoadingModeEnum.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "ConfigurationOuttakes.h"
#include "SVObserverOuttakes.h"
#include "SVObserver.h"
#include "SVOPPQObj.h"
#include "SVOInspectionObj.h"
#include "Triggering/SVOTriggerObj.h"
#include "Triggering/SVTriggerObject.h"
#include "SVOCameraObj.h"
#include "InspectionEngine/SVVirtualCamera.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "InspectionEngine/SVDigitizerProcessingClass.h"
#include "SVIPDoc.h"
#include "Triggering/SVTriggerClass.h"
#include "SVMessage/SVMessage.h"
#include "Triggering/SVTriggerProcessingClass.h"
#include "SVInspectionImporter.h"
#include "SVImportProgress.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "SVOLibrary/SVIMTypeInfoStruct.h"
#include "TextDefinesSvO.h"
#include "SVOGui\GlobalConstantConflictDlg.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/TextDefineSvDef.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef std::vector<SVIMProductEnum> SVIMProductEnumList;

constexpr int GrayScaleBitDepth = 8;

constexpr const char*  c_Color = _T(" Color");

// Defines for Camera File extensions
constexpr const char*  cGigeCameraFileDefExt = _T(".ogc");

constexpr const char* SVIM_BOARD_FILEACQUISITION_STRING     ( _T("File") );
constexpr const char* SVIM_BOARD_MATROX_GIGE	( _T("Matrox_GIGE") );

constexpr const char* cSvimDigName    ( _T(".Dig_") );

constexpr const char* ERR_STR                 ( _T( "**ERROR**   " ) );
constexpr const char* WARNING_STR             ( _T( "**WARNING** " ) );
constexpr const char* CAMERA_INVALID_FILES    ( _T( "-Camera files are invalid" ) );
constexpr const char* CAMERA_NONE_DEFINED     ( _T( "-No cameras defined" ) );
constexpr const char* TRIGGER_NONE_DEFINED    ( _T( "-No trigger defined" ) );
constexpr const char* INSPECT_NONE_DEFINED    ( _T( "-No inspection defined" ) );
constexpr const char* PPQ_NONE_DEFINED        ( _T( "-No PPQ defined" ) );
constexpr const char* PPQ_NO_TRIGGER          ( _T( "-No Trigger attached to PPQ" ) );
constexpr const char* PPQ_NO_CAMERA           ( _T( "-No Camera attached to PPQ" ) );
constexpr const char* PPQ_NO_INSPECT          ( _T( "-No Inspection attached to PPQ" ) );
constexpr const char* CONFIG_NAME_ERR         ( _T( " No Configuration name" ) );
constexpr const char* INVALID_CAMERA          ( _T( "-Camera is invalid for current product" ) );
constexpr const char* INVALID_TRIGGER         ( _T( "-Trigger is invalid for current product" ) );
constexpr const char* INSPECTION_ERROR        ( _T( "-Inspection is not attached to a PPQ" ) );
constexpr const char* TOOLSET_IMAGE_ERROR     ( _T( "-Inspection has no Toolset Image" ) );
constexpr const char* PPQ_PROP_SRC_IMG_ERROR  ( _T( "- Property -'Maintain souce image' set to TRUE, PPQ length > ") );
constexpr const char* PPQ_CONDITIONAL_OUTPUT_INVALID ( _T( "-Conditional Output for PPQ is not valid" ) );
constexpr const char* MESSAGE_UNSUPPORTED_CAM_FILE ( _T("The camera file you have selected is not an SVResearch supported file.") );
constexpr const char* MESSAGE_INCORRECT_CAM_FILE   ( _T("The selected camera file does not match the physical camera.") );
constexpr const char* MESSAGE_NOT_COLOR_CAM_FILE   ( _T("The selected camera file is not a color camera file.") );
constexpr const char* MESSAGE_NOT_COLOR_CAM        ( _T("The physical camera is not a color camera.") );
constexpr const char* MESSAGE_NOT_MONO_CAM_FILE    (_T("The selected camera file is not a mono camera file."));
constexpr const char* MESSAGE_ONE_INVERT_CONTROL   ( _T("There is only one Invert control on a SVIM X-Series for all triggers and strobes.") );
constexpr const char* MESSAGE_SOFTWARE_TRIGGER_NOT_ALLOWED ( _T("The camera does not support Software Triggering.") );
constexpr const char* MESSAGE_CAMERA_TRIGGER_NOT_ALLOWED(_T("Camera trigger is not allowed PPQ Length <= 2."));
constexpr const char* MESSAGE_CAMERA_TRIGGER_TO_MANY_CAMERAS(_T("Camera trigger only works with 1 hardware camera attached to the PPQ"));
constexpr const char* MESSAGE_FILE_ACQUISITION_NOT_ALLOWED ( _T("File Acquisition is not allowed.") );
constexpr const char* MESSAGE_FILE_ACQUISITION_INVALID_FILE ( _T("The Image Filename specified is Invalid.") );
constexpr const char* MESSAGE_FILE_ACQUISITION_INVALID_DIRECTORY ( _T("The Image Directory specified is Invalid.") );
constexpr const char* MESSAGE_FILE_ACQUISITION_MISSING_FILENAME ( _T("The Image Filename must be specified.") );
constexpr const char* MESSAGE_FILE_ACQUISITION_MISSING_DIRECTORY ( _T("The Image Directory must be specified.") );
constexpr const char* MESSAGE_FILE_ACQUISITION_INVALID_IMAGE_SIZE ( _T("The Image Size is Invalid.") );
constexpr const char* MESSAGE_FILE_ACQUISITION_COLOR_MISMATCH ( _T("The bitmap file acquisition color does not match the camera color setting") );
constexpr const char* MESSAGE_INSPECTION_CAMERA_COLOR  ( _T("-The toolset camera image color does not match the inspection color") );
constexpr const char* EXECUTION_FILES_MISSING(_T("- Execution file missing"));

const SVOConfigAssistantDlg::SVProductStringVector SVOConfigAssistantDlg::m_ProductStringVector
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
	{SVIM_PRODUCT_NEO, std::string(SvDef::SVO_PRODUCT_SVIM_NEO)}
};

/////////////////////////////////////////////////////////////////////////////
// SVOConfigAssistantDlg dialog

SVOConfigAssistantDlg::SVOConfigAssistantDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SVOConfigAssistantDlg::IDD, pParent)
	, m_ctlConfigurationName(SvDef::cExcludeCharsConfigName)
{
	SVIMProductEnum eSvimType = TheSVObserverApp().GetSVIMType();
	SetConfigurationSystem( eSvimType );
}

void SVOConfigAssistantDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOConfigAssistantDlg)
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

BEGIN_MESSAGE_MAP(SVOConfigAssistantDlg, CDialog)
	//{{AFX_MSG_MAP(SVOConfigAssistantDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_AVAL_SYS, OnSelchangeComboAvalSys)
	ON_LBN_DBLCLK(IDC_LST_ERR_LIST, OnDblclkLstErrList)
	ON_EN_CHANGE(IDC_EDIT_CONFIGURATION_NAME, OnChangeEditConfigurationName)
	ON_BN_CLICKED(IDHELP, OnHelpButton)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &SVOConfigAssistantDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOConfigAssistantDlg message handlers

BOOL SVOConfigAssistantDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_dlgPropSheet.AddPage(&m_TriggerPage);
	m_dlgPropSheet.AddPage(&m_CameraPage);
	m_dlgPropSheet.AddPage(&m_InspectionPage);
	m_dlgPropSheet.AddPage(&m_PPQPage);
	m_dlgPropSheet.AddPage(&m_FileExecutionPage);

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
		m_ctlConfigurationName.SetWindowText( getConfigFileName().c_str() );
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
void SVOConfigAssistantDlg::SetupSystemComboBox()
{
	SVIMProductEnumList list; 
	typedef std::insert_iterator<SVIMProductEnumList> Inserter;
	Inserter inserter(list, list.begin());
	SVHardwareManifest::GetSupportedSVIMList( inserter );

	for (SVIMProductEnumList::iterator l_Iter = list.begin();l_Iter != list.end(); ++l_Iter)
	{
		m_ctlAvailableSys.AddString( GetNameFromProductID(*l_Iter).c_str() );
	}

	if ( !m_bNewConfiguration )
	{
		const SVIMTypeInfoStruct& l_SVIMInfo = SVHardwareManifest::GetSVIMTypeInfo( m_lConfigurationType );
		if ( !l_SVIMInfo.m_Supported )
		{
			m_ctlAvailableSys.AddString( GetNameFromProductID( m_lConfigurationType ).c_str() );
		}
	}
}

void SVOConfigAssistantDlg::SetNewConfiguration(bool bNewFlag)
{
	m_bNewConfiguration = bNewFlag;
}

bool SVOConfigAssistantDlg::IsNewConfiguration()
{
	return m_bNewConfiguration;
}

bool SVOConfigAssistantDlg::AddToCameraList(LPCTSTR CameraName, int Dig, int CameraID)
{
	return m_CameraList.AddCameraToList(CameraName, Dig, CameraID);
}

bool SVOConfigAssistantDlg::AddToInspectionList(LPCTSTR InspectionName, bool NewInspection)
{
	return m_InspectList.AddInspectionToList(InspectionName,  NewInspection);
}

bool SVOConfigAssistantDlg::AddToTriggerList(LPCTSTR TriggerName, int iDig)
{
	return m_TriggerList.AddTriggerToList( std::string(TriggerName), iDig );
}

bool SVOConfigAssistantDlg::AddToPPQList(LPCTSTR PPQ, LPCTSTR Camera, LPCTSTR Trigger, LPCTSTR Inspection)
{
	bool bRet = m_PPQList.AddPPQToList( PPQ );
	bRet = m_PPQList.AttachCameraToPPQ( PPQ, Camera ) && bRet;
	bRet = m_PPQList.AttachInspectToPPQ( PPQ, Inspection ) && bRet;
	m_PPQList.AttachTriggerToPPQ( PPQ, Trigger );

	return bRet;
}

bool SVOConfigAssistantDlg::AddToPPQList(LPCTSTR PPQ)
{
	return m_PPQList.AddPPQToList( PPQ );
}

void SVOConfigAssistantDlg::OnSelchangeComboAvalSys() 
{
	m_ctlAvailableSys.GetWindowText( m_AvailableSystem );

	SVIMProductEnum currentConfigType = m_lConfigurationType; 

	SVIMProductEnum newConfigType = GetProductIDFromName( std::string(m_AvailableSystem) );

	if ( false == m_bNewConfiguration || m_bModified )
	{
		if ( SVHardwareManifest::IsCompatible(newConfigType, currentConfigType) )
		{
			m_bModified = true;

			m_lConfigurationType = newConfigType;
			UpdateAvailableSystems( currentConfigType, m_lConfigurationType );
		}
		else
		{
			bool configTypeConvereted {false};
			///When changing from discrete IO to PLC or vice versa then we need to reset the inputs and outputs
			if ((SVHardwareManifest::isPlcSystem(newConfigType) != SVHardwareManifest::isPlcSystem(currentConfigType)) ||
				(SVHardwareManifest::isDiscreteIOSystem(newConfigType) != SVHardwareManifest::isDiscreteIOSystem(currentConfigType)))
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
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
void SVOConfigAssistantDlg::UpdateAvailableSystems( SVIMProductEnum p_CurrentConfigurationType, SVIMProductEnum p_SelectedConfigurationType )
{
	const SVIMTypeInfoStruct& l_SVIMInfo = SVHardwareManifest::GetSVIMTypeInfo(p_CurrentConfigurationType);

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

void SVOConfigAssistantDlg::ReloadForCurrentSystem()
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
			case SVIM_PRODUCT_NEO:
			{
				CreateDefaultForSVIMDigital(2, SvDef::cTriggerFixedName);
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
		m_TriggerPage.SetupList();
		m_CameraPage.SetupList();
		m_InspectionPage.SetupList();
		m_PPQPage.SetupList();
		m_FileExecutionPage.SetupList();
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

void SVOConfigAssistantDlg::CreateDefaultForSVIMDigital(int Number, LPCTSTR TriggerBaseName )
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
		m_InspectList.AddInspectionToList( InspectionName.c_str(), true );
		m_InspectList.SetToolsetImage( InspectionName.c_str(), CameraName.c_str() );

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

std::string SVOConfigAssistantDlg::GetNextCameraName()
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

int SVOConfigAssistantDlg::GetNextCameraNumber() const
{
	return m_iNextCameraNumber;
}

std::string SVOConfigAssistantDlg::GetNextInspectionName() const
{
	int iRet = 0;
	bool bFound = true;
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
		bFound |= m_InspectList.IsOriginalInspectionInList(Result.c_str());
	}
	return Result;
}

std::string SVOConfigAssistantDlg::GetNextTriggerName(LPCTSTR BaseName) const
{
	int iRet = m_TriggerList.GetNextTriggerID();
	
	std::string Result = SvUl::Format(_T("%s%d"), BaseName, iRet + 1 );

	return Result;
}

int SVOConfigAssistantDlg::GetNextTriggerID() const
{
	return m_TriggerList.GetNextTriggerID();
}

std::string SVOConfigAssistantDlg::GetNextPPQName() const
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

bool SVOConfigAssistantDlg::RenameInspection(LPCTSTR InspectionName, LPCTSTR NewName)
{
	const SVOInspectionObjPtr pInspectionObj( GetInspectionObjectByName(InspectionName) );
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
	bool result = m_InspectList.RenameInspection(InspectionName, NewName);
	RenameInspectionObjects(InspectionName, NewName);

	return result;
}

int SVOConfigAssistantDlg::GetCameraListCount() const
{
	return m_CameraList.GetCameraListCount();
}

int SVOConfigAssistantDlg::GetInspectionListCount() const
{
	return m_InspectList.GetInspectionListCount();
}

int SVOConfigAssistantDlg::GetTriggerListCount() const
{
	return m_TriggerList.GetTriggerListCount();
}

int SVOConfigAssistantDlg::GetPPQListCount() const
{
	return m_PPQList.GetPPQListCount();
}

SVOInspectionObjPtr SVOConfigAssistantDlg::GetInspectionObject(int iPos)
{
	return m_InspectList.GetInspectionByPosition(iPos);
}

SVOInspectionObjPtr SVOConfigAssistantDlg::GetInspectionObjectByName(LPCTSTR Name)
{
	return m_InspectList.GetInspectionByName(Name);
}

SVOCameraObjPtr SVOConfigAssistantDlg::GetCameraObject(int iPos)
{
	return m_CameraList.GetCameraObjectByPosition(iPos);
}

SVOCameraObjPtr SVOConfigAssistantDlg::GetCameraObjectByName(LPCTSTR CameraName)
{
	return m_CameraList.GetCameraObjectByName(CameraName);
}

SvTrig::SVOTriggerObjPtr SVOConfigAssistantDlg::GetTriggerObject(int iPos)
{
	return m_TriggerList.GetTriggerObjectByPosition(iPos);
}

SvTrig::SVOTriggerObjPtr SVOConfigAssistantDlg::GetTriggerObjectByName(LPCTSTR TriggerName)
{
	return m_TriggerList.GetTriggerObjectByName(std::string(TriggerName));
}

SVOPPQObjPtr SVOConfigAssistantDlg::GetPPQObject(int iPos)
{
	return m_PPQList.GetPPQObjectByPosition(iPos);
}

SVOPPQObjPtr SVOConfigAssistantDlg::GetPPQObjectByName(LPCTSTR PPQName)
{
	return m_PPQList.GetPPQObjectByName( PPQName );
}

bool SVOConfigAssistantDlg::RemoveCameraFromList(LPCTSTR CameraName)
{
	return m_CameraList.RemoveCameraFromList( CameraName );
}

bool SVOConfigAssistantDlg::RemoveInspectionFromList(LPCTSTR Inspection)
{
	if (IsInspectUsed(Inspection))
	{
		RemoveUsedInspect(Inspection);
	}
	InspectionDeletedCheckAgainstPPQ(Inspection);
	return m_InspectList.RemoveInspectionFromList(Inspection);
}

bool SVOConfigAssistantDlg::RemoveTriggerFromList(LPCTSTR TriggerName)
{
	if (IsTriggerUsed( TriggerName ) )
	{
		RemoveUsedTrigger( TriggerName );
	}
	return m_TriggerList.RemoveTriggerFromList(std::string(TriggerName));
}

bool SVOConfigAssistantDlg::RemovePPQFromList(LPCTSTR PPQ)
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

std::string SVOConfigAssistantDlg::BuildDigName(const SVOCameraObj& rCameraObj) const
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
			case SVIM_PRODUCT_NEO:
			case SVIM_PRODUCT_X2_GD1A_COLOR:
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			case SVIM_PRODUCT_X2_GD4A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
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
			case SVIM_PRODUCT_NEO:
			case SVIM_PRODUCT_X2_GD1A_COLOR:
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			case SVIM_PRODUCT_X2_GD4A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
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

bool SVOConfigAssistantDlg::IsDigitizerUsed(LPCTSTR DigString)
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

bool SVOConfigAssistantDlg::IsCameraInList(LPCTSTR CameraName) const
{
	return m_CameraList.IsCameraInList(CameraName);
}

bool SVOConfigAssistantDlg::IsInspectionInList(LPCTSTR InspectionName) const
{
	return m_InspectList.IsInspectionInList(InspectionName);
}

bool SVOConfigAssistantDlg::IsOriginalInspectionInList(LPCTSTR InspectionName) const
{
	return m_InspectList.IsOriginalInspectionInList(InspectionName);
}

bool SVOConfigAssistantDlg::IsTriggerInList(LPCTSTR TriggerName) const
{
	return m_TriggerList.IsTriggerInList(std::string(TriggerName));
}

bool SVOConfigAssistantDlg::IsPPQInList(LPCTSTR PPQName) const
{
	return m_PPQList.IsPPQInList(PPQName);
}

void SVOConfigAssistantDlg::SetCurrentSystem(long lSysValue)
{
	m_lSystemType = (SVIMProductEnum) lSysValue;
}

void SVOConfigAssistantDlg::SetConfigurationSystem(long lSysValue)
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

void SVOConfigAssistantDlg::SetCurrentSystemDisplay()
{
	std::string SystemName;

	SystemName = GetNameFromProductID( m_lSystemType );
	if ( SystemName.empty() )
	{
		SystemName = _T("<UNKNOWN>");
	}

	m_ctlCurrentSys.SetWindowText( SystemName.c_str() );
}

bool SVOConfigAssistantDlg::IsTriggerUsed(LPCTSTR TriggerName) const
{
	bool Result = false;

	SvDef::StringVector::const_iterator Iter = std::find( m_UsedTriggers.begin(), m_UsedTriggers.end(), std::string(TriggerName) );
	if( m_UsedTriggers.end() != Iter )
	{
		Result = true;
	}
	return Result;
}

void SVOConfigAssistantDlg::AddUsedTrigger(LPCTSTR TriggerName)
{
	m_UsedTriggers.push_back( std::string(TriggerName) );
}

void SVOConfigAssistantDlg::RemoveUsedTrigger(LPCTSTR TriggerName)
{
	SvDef::StringVector::const_iterator Iter = std::find( m_UsedTriggers.begin(), m_UsedTriggers.end(), std::string(TriggerName) );
	if( m_UsedTriggers.end() != Iter)
	{
		m_UsedTriggers.erase( Iter );
	}
}

bool SVOConfigAssistantDlg::IsSoftwareTriggerAllowed(LPCTSTR TriggerName) const
{
	bool bRetVal = true;

	// Check for acquistion device and if software triggering is supported
	// For GIGE acquistion device 
	// - check if camera file has been assigned
	// - check if Trigger_Source_Option_N (Software Oneshot) is available

	// check if Digital acquisition system
	if ( SVHardwareManifest::IsDigitalSVIM( GetProductType() ) )
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

bool SVOConfigAssistantDlg::IsCameraTriggerAllowed(LPCTSTR TriggerName) const
{
	bool result{ true };

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
				///Camera Trigger needs PPQ > 2
				if (2 >= pPPQObj->GetPPQLength())
				{
					result = false;
				}
			}
		}
	}
	return result;
}

bool SVOConfigAssistantDlg::HasExactlyOneHWCameraForTrigger(LPCTSTR TriggerName) const
{
	bool result{ false };

	int ppqCount = GetPPQListCount();
	for (int i = 0; i < ppqCount; i++)
	{
		const SVOPPQObjPtr pPPQObj = m_PPQList.GetPPQObjectByPosition(i);
		if (nullptr != pPPQObj)
		{
			std::string attachedTriggerName = pPPQObj->GetAttachedTriggerName();
			if (0 == SvUl::CompareNoCase(attachedTriggerName, std::string(TriggerName)))
			{
				int hardwareCameraCount{ 0 };
				int attachedCameraCount = pPPQObj->GetAttachedCameraCount();
				for (int j = 0; j < attachedCameraCount; ++j)
				{
					std::string CameraName = pPPQObj->GetAttachedCamera(j);
					// find camera in camera list
					int cameraCount = GetCameraListCount();
					for (int c = 0; c < cameraCount; c++)
					{
						const SVOCameraObjPtr pCameraObj = m_CameraList.GetCameraObjectByPosition(c);
						if (nullptr != pCameraObj)
						{
							if (pCameraObj->GetCameraDisplayName() == CameraName)
							{
								hardwareCameraCount += pCameraObj->IsFileAcquisition() ? 0 : 1;
							}
						}
					}
				}
				result = (hardwareCameraCount == 1) ? true : false;
			}
		}
	}
	return result;
}

bool SVOConfigAssistantDlg::IsCameraLineInputAllowed(LPCTSTR TriggerName) const
{
	bool bRetVal = false;

	// Check for acquistion device and if Line Input is supported
	// For GIGE acquistion device 
	// - check if camera file has been assigned
	// - check if Line Input Device parameter is available

	// check if Digital acquisition system
	if ( SVHardwareManifest::IsDigitalSVIM( GetProductType() ) )
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

SVIMProductEnum SVOConfigAssistantDlg::GetProductType() const
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

bool SVOConfigAssistantDlg::IsInspectUsed(LPCTSTR Inspection)
{
	bool Result = false;

	SvDef::StringVector::const_iterator Iter = std::find( m_UsedInspections.begin(), m_UsedInspections.end(), std::string(Inspection) );
	if( m_UsedInspections.end() != Iter )
	{
		Result = true;
	}
	return Result;
}

void SVOConfigAssistantDlg::AddUsedInspect(LPCTSTR Inspection)
{
	m_UsedInspections.push_back( std::string(Inspection) );
}

void SVOConfigAssistantDlg::RemoveUsedInspect(LPCTSTR Inspection)
{
	SvDef::StringVector::const_iterator Iter = std::find( m_UsedInspections.begin(), m_UsedInspections.end(), std::string(Inspection) );
	if( m_UsedInspections.end() != Iter)
	{
		m_UsedInspections.erase( Iter );
	}
}

void SVOConfigAssistantDlg::SetModified(bool bModified)
{
	m_bModified = bModified;
}

void SVOConfigAssistantDlg::OnOK()
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
bool SVOConfigAssistantDlg::AddMessageToList(int iDlg, LPCTSTR Message)
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

bool SVOConfigAssistantDlg::RemoveMessageFromList(LPCTSTR Message)
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

int SVOConfigAssistantDlg::RemoveAllObjectMessages(LPCTSTR ObjectName)
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

void SVOConfigAssistantDlg::RemoveAllDlgMessages(int iDlg)
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

bool SVOConfigAssistantDlg::ContainsErrorMessage() const
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

void SVOConfigAssistantDlg::SetupMessages()
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

bool SVOConfigAssistantDlg::SendPPQDataToConfiguration(SVPPQObjectPtrVector& rPPQsToDelete)
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
				for (long lI= lInsCnt-1; -1 < lI; --lI)
				{
					SVInspectionProcess* pInspection( nullptr );
					bRet = pPPQ->GetInspection(lI, pInspection);
					if ( nullptr != pInspection)
					{
						bool bDetachInspect{ false };
						const SVOInspectionObjPtr pTmpInspection(m_InspectList.GetInspectionByOriginalName(pInspection->GetName()));
						if ( nullptr != pTmpInspection )
						{
							if ( (!IsInspectionOnPPQ(pPPQ->GetName(), pInspection->GetName())) ||
								(pTmpInspection->IsNewInspection()) )
							{
								bDetachInspect = true;
							}
						}
						else
						{
							bDetachInspect = true;
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
				SvTrig::SVTriggerObject* pTrigger{pPPQ->GetTrigger()};
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
			bRet = false;
		}
	} // for ppq
	return bRet;
}

bool SVOConfigAssistantDlg::SendAcquisitionDataToConfiguration()
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

	std::string DigName;

	int iCamCnt = m_CameraList.GetCameraListCount();
	for (int iAcq = 0; iAcq < iCamCnt; iAcq++)
	{
		int CameraIndex(iAcq);
		SvIe::SVAcquisitionClassPtr psvDevice;

		const SVOCameraObjPtr pCameraObj( GetCameraObject( CameraIndex ) );
		if( nullptr != pCameraObj )
		{
			SVFileNameArrayClass svFiles;
			SVFileNameClass svFile;
			SVLut lut;
			SVLightReference lightRef;
			// For File Acquisition
			if ( pCameraObj->IsFileAcquisition())
			{
				//File acquisition should set the digitizer number always to the camera ID
				pCameraObj->SetDigNumber(pCameraObj->GetCameraID());
				DigName = BuildDigName(*pCameraObj);
				psvDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DigName.c_str() );
				if ( nullptr != psvDevice )
				{
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
				int Digitizer = SvIe::SVDigitizerProcessingClass::Instance().getDigitizerID(pCameraObj->GetCameraID());
				if (-1 != Digitizer)
				{
					pCameraObj->SetDigNumber(Digitizer);
				}
				DigName = BuildDigName(*pCameraObj);
				svFile.SetFullFileName( pCameraObj->GetCameraFile().c_str() );
			
				if ( 0 == SvUl::CompareNoCase( svFile.GetExtension(), std::string(cGigeCameraFileDefExt) ) )
				{
					svFiles.push_back(svFile);
				}

				SVLightReference* psvLight {nullptr};
				SVLut* pLut {nullptr};
				SVDeviceParamCollection* pDeviceParams {nullptr};
				SVDeviceParamCollection svDeviceParams;

				psvDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DigName.c_str() );
				if ( nullptr != psvDevice )
				{
					bool bGetLightReference = false;
					SVFileNameArrayClass* psvFiles {nullptr};
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
						bGetLightReference = true;
					}

					svDeviceParams = pCameraObj->GetCameraDeviceParams();
					pDeviceParams = &svDeviceParams;

					if( pCameraObj->GetCameraFileChanged() )
					{
						psvDevice->LoadFiles(svFiles);

						if ( S_OK == psvDevice->LoadLightReference(lightRef) )
						{
							psvDevice->SetLightReference(lightRef);
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
						psvDevice->GetLightReference(lightRef);
						psvLight = &lightRef;
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
					psvLight = &lightRef;    // assign dummy var
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

	//Remove any unused acquisition devices
	std::string AcquisitionName;
	SVConfigurationObject::SVAcquisitionDeviceMap::iterator aPos = pConfig->GetAcquisitionDeviceStartPosition();
	while (aPos != pConfig->GetAcquisitionDeviceEndPosition())
	{
		//see if things are in the dlg...
		pConfig->GetAcquisitionDeviceNextAssoc(aPos, AcquisitionName);
		if (!IsDigitizerUsed(AcquisitionName.c_str()))
		{
			SvIe::SVAcquisitionClassPtr psvDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice(AcquisitionName.c_str());
			pConfig->RemoveAcquisitionDevice(AcquisitionName.c_str());
			if (nullptr != psvDevice)
			{
				psvDevice->DestroyBuffers();
				psvDevice->UnloadFiles();
				psvDevice->ResetLightReference();
				psvDevice->ResetLut();
			}
		}
	}

	return bRet;
}

bool SVOConfigAssistantDlg::SendCameraDataToConfiguration()
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
			bRet = false;
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
						bRet = false;
					}
				}

				bool bAddCamera = false;

				if ( nullptr == pCamera )
				{
					pCamera = new SvIe::SVVirtualCamera;
					pCamera->SetName( CameraDisplayName.c_str() );
					bAddCamera = true;
				}

				if ( nullptr != pCamera )
				{
					if (false == pCameraObj->IsFileAcquisition())
					{
						// move from editing camera object to configuration camera object
						int Digitizer = SvIe::SVDigitizerProcessingClass::Instance().getDigitizerID(pCameraObj->GetCameraID());
						if (-1 != Digitizer)
						{
							pCameraObj->SetDigNumber(Digitizer);
						}
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

bool SVOConfigAssistantDlg::SendTriggerDataToConfiguration()
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
	SvTrig::SVTriggerObject* pTrigger( nullptr );
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
		// Check for Triggers changed
		for (int i = 0; i < iTrgCnt; i++)
		{
			const SvTrig::SVOTriggerObjPtr pTriggerObj( GetTriggerObject(i) );
			if( nullptr != pTriggerObj )
			{
				std::string TriggerDisplayName{ pTriggerObj->GetTriggerDisplayName() };
				std::string DeviceName{ SVHardwareManifest::BuildTriggerDeviceName(m_lConfigurationType, pTriggerObj->GetTriggerDigNumber(), pTriggerObj->getTriggerType()) };

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
						bRet = false;
					}
				}

				bool bAddTrigger{ false };

				if ( nullptr == pTrigger )
				{
					pTrigger = new SvTrig::SVTriggerObject;
					pTrigger->SetName( TriggerDisplayName.c_str() );
					bAddTrigger = true;
				}

				if ( nullptr != pTrigger )
				{
					SvTrig::SVTriggerClass* pMainTriggerDevice{ SvTrig::SVTriggerProcessingClass::Instance().GetTrigger(DeviceName.c_str()) };
					if (nullptr != pMainTriggerDevice)
					{
						SvTrig::SVTriggerClass* pSoftwareTrigger{ nullptr };
						if (SvDef::TriggerType::SoftwareTrigger != pMainTriggerDevice->getType())
						{
							std::string softwareTriggerName{ SVHardwareManifest::BuildTriggerDeviceName(m_lConfigurationType, pMainTriggerDevice->getDigitizerNumber(), SvDef::TriggerType::SoftwareTrigger) };
							pSoftwareTrigger = SvTrig::SVTriggerProcessingClass::Instance().GetTrigger(softwareTriggerName.c_str());
						}
						bRet = pTrigger->Create(pMainTriggerDevice, pSoftwareTrigger) && bRet;
					}
					pTrigger->SetSoftwareTriggerPeriod(pTriggerObj->GetTimerPeriod());
					pTrigger->setObjectIDParameters(pTriggerObj->getObjectIDParameters());

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

bool SVOConfigAssistantDlg::RenameInspectionObjects(LPCTSTR InspectionName, LPCTSTR NewInspectionName)
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
						RemoveUsedInspect(InspectionName);
						AddUsedInspect(NewInspectionName);
					}
				}
			}
		}
	}
	return bRet;
}

SVOPPQObjPtr SVOConfigAssistantDlg::GetPPQObjectByInspectionName(const std::string& rInspectionName)
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

bool SVOConfigAssistantDlg::SendInspectionDataToConfiguration()
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
			const SVOInspectionObjPtr pInspectionObj(m_InspectList.GetInspectionByOriginalName(pInspection->GetName()));
			bool bDeleteInspect = false;

			if ( nullptr != pInspectionObj )
			{
				if ( (false == m_InspectList.IsOriginalInspectionInList(pInspection->GetName()))  || (pInspectionObj->IsNewInspection()) )
				{
					bDeleteInspect = true;
				}
			}
			else
			{
				bDeleteInspect = true;
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
				Key = pInspectionObj->GetOriginalName();
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

							TheSVObserverApp().UpdateAllIOViews();
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
						SVInspectionImportHelper importer  = SVInspectionImportHelper(importFilename, inspectionName, ToolsetImage);
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
										pPPQObj->SetImportedInspectionInfo(std::move(importer.info));
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
							SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
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

							pInspection->SetToolsetImage(ToolsetImage);

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
					pInspection->SetToolsetImage(ToolsetImage);

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

bool SVOConfigAssistantDlg::SendPPQAttachmentsToConfiguration(SVPPQObjectPtrVector& rPPQsToDelete)
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
						SvTrig::SVTriggerObject* pTrigger = pConfig->GetTrigger(l);
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
					const SVOInspectionObjPtr pInspectionObj = m_InspectList.GetInspectionByOriginalName(PpqInspectionName.c_str());
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
								if (pInspectionObj->GetOriginalName() == pInspection->GetName())
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

				const SVImportedInspectionInfo& rImportedInspectionInfo = pPPQObj->GetImportedInspectionInfo();
				pConfig->SetObjectAttributes(rImportedInspectionInfo.m_objectAttributeList);
				for (const auto& rImportInput : rImportedInspectionInfo.m_inputList)
				{
					if(SvXml::cRemoteType == rImportInput.m_type)
					{
						pConfig->AddImportedRemoteInput(pPPQ, rImportInput.m_name, rImportInput.m_ppqPosition, rImportInput.m_index, rImportInput.m_value);
					}
					else if(SVHardwareManifest::isDiscreteIOSystem(m_lConfigurationType) && SvXml::cDigitalType == rImportInput.m_type)
					{
						pConfig->AddImportedDigitalInput(pPPQ, rImportInput.m_name, rImportInput.m_ppqPosition);
					}
				}
				for (const auto& rImportOutput : rImportedInspectionInfo.m_outputList)
				{
					if (SvPb::PlcOutputObjectType == static_cast<SvPb::SVObjectSubTypeEnum> (rImportOutput.m_subType))
					{
						unsigned long outputNr{ 0UL };
						SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(outputNr);
						SVOutputObjectList* pOutputList{ pConfig->GetOutputObjectList() };
						if (nullptr != pOutputList)
						{
							std::string ppqName = pPPQObj->GetPPQName();
							int ppqIndex = atoi(SvUl::Mid(ppqName, std::string(SvDef::cPpqFixedName).length()).c_str()) - 1;
							if (ppqIndex >= 0)
							{
								uint32_t outputIndex = rImportOutput.m_channel % outputNr;
								long channelNr = ppqIndex * outputNr + outputIndex;
								uint32_t ioID = ObjectIdEnum::PlcOutputId + channelNr;
								SVOutputObjectPtr pOutput = pOutputList->GetOutput(ioID);
								bool isPlcOutputFree = nullptr == pOutput;
								if (isPlcOutputFree)
								{
									pOutput = std::make_shared<PlcOutputObject>();
									PlcOutputObject* pPlcOutput = dynamic_cast<PlcOutputObject*> (pOutput.get());
									pOutput->updateObjectId(ioID);
									pOutput->SetName(rImportOutput.m_name.c_str());
									if (nullptr != pPlcOutput)
									{
										pPlcOutput->SetChannel(channelNr);
										pPlcOutput->Combine(rImportOutput.m_isCombined, rImportOutput.m_isAndAck);
									}
									pOutputList->AttachOutput(pOutput);
								}
								else
								{
									SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
									SvDef::StringVector msgList;
									msgList.push_back(std::to_string(channelNr + 1));
									msgList.push_back(pPPQ->GetName());
									msgList.push_back(rImportOutput.m_name);
									Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_PlcOutputAlreadyUsed, msgList, SvStl::SourceFileParams(StdMessageParams));
								}
							}
						}
					}
				}
				// cleanup so we don't do it again
				pPPQObj->ClearImportedInspectionInfo();

				if (bNew)
				{
					bRet = pPPQ->Create() && bRet;
					pPPQ->m_pInputList = pConfig->GetInputObjectList();
					pPPQ->m_pOutputList = pConfig->GetOutputObjectList();
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

bool SVOConfigAssistantDlg::SendDataToConfiguration()
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

	pConfig->SetProductType( m_lConfigurationType, m_bNewConfiguration);

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
	DisconnectToolsetBuffers();

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

	//POST/PRE Execution
	//Save & copy filepath
	CString runDirectory = SvStl::GlobalPath::Inst().GetRunPath().c_str();

	if (m_preExecutionFilePath.empty() == false && m_preExecutionFilePath != pConfig->getPreRunExecutionFilePath())
	{
		if (pConfig->getPreRunExecutionFilePath() != pConfig->getPostRunExecutionFilePath())
		{
			std::remove(pConfig->getPreRunExecutionFilePath().c_str());
			RemoveFileFromConfig(pConfig->getPreRunExecutionFilePath().c_str());
		}

		CString destinationPath{ runDirectory };
		destinationPath += '\\';
		destinationPath += std::filesystem::path(m_preExecutionFilePath).filename().c_str();
		::CopyFile(m_preExecutionFilePath.c_str(), destinationPath, false);
		AddFileToConfig(destinationPath);
		m_preExecutionFilePath = destinationPath;
		pConfig->setPreRunExecutionFilePath(destinationPath);
	}
	else if (m_preExecutionFilePath.empty())
	{
		if (pConfig->getPreRunExecutionFilePath() != pConfig->getPostRunExecutionFilePath())
		{
			std::remove(pConfig->getPreRunExecutionFilePath().c_str());
			RemoveFileFromConfig(pConfig->getPreRunExecutionFilePath().c_str());
		}
		pConfig->setPreRunExecutionFilePath(m_preExecutionFilePath.c_str());
	}

	if (m_postExecutionFilePath.empty() == false && m_postExecutionFilePath != pConfig->getPostRunExecutionFilePath())
	{
		if (pConfig->getPreRunExecutionFilePath() != pConfig->getPostRunExecutionFilePath())
		{
			std::remove(pConfig->getPostRunExecutionFilePath().c_str());
			RemoveFileFromConfig(pConfig->getPostRunExecutionFilePath().c_str());
		}

		CString destinationPath{ runDirectory };
		destinationPath += '\\';
		destinationPath += std::filesystem::path(m_postExecutionFilePath).filename().c_str();
		::CopyFile(m_postExecutionFilePath.c_str(), destinationPath, false);
		AddFileToConfig(destinationPath);
		m_postExecutionFilePath = destinationPath;
		pConfig->setPostRunExecutionFilePath(destinationPath);
	}
	else if (m_postExecutionFilePath.empty())
	{
		if (pConfig->getPreRunExecutionFilePath() != pConfig->getPostRunExecutionFilePath())
		{
			std::remove(pConfig->getPostRunExecutionFilePath().c_str());
			RemoveFileFromConfig(pConfig->getPostRunExecutionFilePath().c_str());
		}
		pConfig->setPostRunExecutionFilePath(m_postExecutionFilePath.c_str());
	}

	if ( m_bNewConfiguration )
	{
		pConfig->SetConfigurationLoaded();
	}
	return bRet;
}

bool SVOConfigAssistantDlg::GetConfigurationForExisting()
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

	long lCfgCameraCnt;
	long lCfgTriggerCnt;
	long lCfgInspectCnt;
	long lCfgPPQCnt;
	std::string CameraName;
	std::string TriggerName;
	std::string PPQName;
	int iDigNumber;
	int CameraID;
	int iChannel;
	std::string CameraFileName;

	SvTrig::SVTriggerObject* pcfgTrigger( nullptr );
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

			const SvTrig::SVOTriggerObjPtr pTriggerObj( m_TriggerList.GetTriggerObjectByName(std::string(TriggerName)) );
			if( nullptr != pTriggerObj )
			{
				pTriggerObj->setTriggerType(pcfgTrigger->getType());
				pTriggerObj->SetTimerPeriod(pcfgTrigger->GetSoftwareTriggerPeriod());
				pTriggerObj->setObjectIDParameters(pcfgTrigger->getObjectIDParameters());
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
			std::string InspectionName = pcfgInspection->GetName();

			m_InspectList.AddInspectionToList(InspectionName.c_str(), false);

			// Determine which image is the main image
			std::string ToolsetImage;
			ToolsetImage = pcfgInspection->GetToolsetImage();
			m_InspectList.SetToolsetImage(InspectionName.c_str(), ToolsetImage.c_str() );
			
			m_InspectList.SetColor(InspectionName.c_str(), pcfgInspection->IsColorCamera() );

			if( pcfgInspection->IsNewDisableMethodSet() )
			{
				sDisable = _T( "Method 2" );
			}// end if
			else
			{
				sDisable = _T( "Method 1" );
			}// end else

			m_InspectList.SetNewDisableMethod(InspectionName.c_str(), sDisable );

			m_InspectList.SetShowAuxExtent(InspectionName.c_str() );

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
			m_InspectList.SetEnableAuxiliaryExtent(InspectionName.c_str(), l_lEnable );
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
						availableInputs.push_back(std::make_pair(rEntry->getObject()->GetName(), rEntry->m_IOId));
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

	//load Execution section...
	m_preExecutionFilePath = pConfig->getPreRunExecutionFilePath();
	m_postExecutionFilePath = pConfig->getPostRunExecutionFilePath();
	//end of load Execution section..

	m_TriggerPage.SetupList();
	m_CameraPage.SetupList();
	m_InspectionPage.SetupList();
	m_PPQPage.SetupList();
	m_FileExecutionPage.SetupList();

	m_bCameraGood = TRUE;
	m_bTriggerGood = TRUE;
	m_bInspectionGood = TRUE;
	m_bPPQGood = TRUE;
	return bRet;
}

bool SVOConfigAssistantDlg::ItemChanged(int iItemDlg, LPCTSTR LabelName, int iAction)
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
								if ( SVHardwareManifest::IsDigitalSVIM( m_lConfigurationType ) )
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
							if (false == pCameraObj->IsFileAcquisition())
							{
								//The digitizer number can change after calling the camera manager so set the camera file as changed to reload
								int Digitizer = SvIe::SVDigitizerProcessingClass::Instance().getDigitizerID(pCameraObj->GetCameraID());
								pCameraObj->SetDigNumber(Digitizer);
							}
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
					pInspectionObj = GetInspectionObjectByName(LabelName);
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
					pInspectionObj = GetInspectionObjectByName(LabelName);
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
						CheckTriggers();
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
						CheckTriggers();
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
					pInspectionObj = GetInspectionObjectByName(LabelName);
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
						CheckTriggers();
						long l_lPpqLength = pPPQObj->GetPPQLength();
						long l_lImageDepth = TheSVObserverApp().GetMaxPPQLength();
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
							long l_lImageDepth = TheSVObserverApp().GetMaxPPQLength();
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
							long l_lImageDepth = TheSVObserverApp().GetMaxPPQLength();
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

		case EXECUTION_DLG:
		{
			switch (iAction)
			{
				case ITEM_ACTION_REFRESH:
				{
					constexpr const char* PRE_EXECUTION_FILE(_T("PreExecutionFile"));
					constexpr const char* POST_EXECUTION_FILE(_T("PostExecutionFile"));

					Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, PRE_EXECUTION_FILE, EXECUTION_FILES_MISSING);
					if (!std::filesystem::exists(m_preExecutionFilePath) && m_preExecutionFilePath.length() != 0)
					{
						AddMessageToList(EXECUTION_DLG, Message.c_str());
					}
					else
					{
						RemoveMessageFromList(Message.c_str());
					}

					Message = BuildDisplayMessage(MESSAGE_TYPE_ERROR, POST_EXECUTION_FILE, EXECUTION_FILES_MISSING);
					if (!std::filesystem::exists(m_postExecutionFilePath) && m_postExecutionFilePath.length() != 0)
					{
						AddMessageToList(EXECUTION_DLG, Message.c_str());
					}
					else
					{
						RemoveMessageFromList(Message.c_str());
					}

					break;
				}

				default:
				{
					break;
				}
			}
		}

		default:
		{
			break;
		}
	}// end switch (iItemDlg)
	IsGood();
	return bRet;
}

std::string SVOConfigAssistantDlg::BuildDisplayMessage(ConfigMsgTypeEnum iErrorFlag, LPCTSTR ObjectName, LPCTSTR Message)
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

void SVOConfigAssistantDlg::TriggerDeletedCheckAgainstPPQ(LPCTSTR TriggerName)
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

void SVOConfigAssistantDlg::CameraDeletedCheckAgainstPPQ(LPCTSTR CameraName)
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

void SVOConfigAssistantDlg::CameraDeletedCheckAgainstInspection(LPCTSTR )
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

void SVOConfigAssistantDlg::CameraDetachedCheckAgainstInspection(LPCTSTR)
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

void SVOConfigAssistantDlg::InspectionDeletedCheckAgainstPPQ(LPCTSTR InspectName)
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

void SVOConfigAssistantDlg::IsGood()
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

void SVOConfigAssistantDlg::OnDblclkLstErrList() 
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

std::string SVOConfigAssistantDlg::GetConfigurationName()
{
	return std::string(m_ConfigurationName);
}

void SVOConfigAssistantDlg::OnChangeEditConfigurationName()
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

bool SVOConfigAssistantDlg::GetNewConfigFlag()
{
	return m_bNewConfiguration;
}

bool SVOConfigAssistantDlg::Modified()
{
	return m_bModified;
}

//
// If bTrigger is true then this function returns the max number of triggers allowed.
//
int SVOConfigAssistantDlg::GetAllowedNumberOfDigs(bool bTrigger/* = FALSE*/)
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

void SVOConfigAssistantDlg::CheckAgainstCurrentList()
{
	const SVIMTypeInfoStruct& info = SVHardwareManifest::GetSVIMTypeInfo(m_lConfigurationType);

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
			const SvTrig::SVOTriggerObjPtr pTriggerObj( m_TriggerList.GetTriggerObjectByPosition(i) );
			if ( nullptr != pTriggerObj && pTriggerObj->GetTriggerDigNumber() > iNumberTriggersAllowed-1 )
			{
				AddMessageToList(TRIGGER_DLG, BuildDisplayMessage(MESSAGE_TYPE_ERROR, pTriggerObj->GetTriggerDisplayName(), INVALID_TRIGGER).c_str() );
			}
		}
	}
	m_TriggerPage.SetupList();
	m_CameraPage.SetupList();
}

bool SVOConfigAssistantDlg::IsInspectionOnPPQ(LPCTSTR PPQName, LPCTSTR InspectionLabel)
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

bool SVOConfigAssistantDlg::IsCameraOnPPQ(LPCTSTR PPQName, LPCTSTR CameraName)
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

bool SVOConfigAssistantDlg::IsTriggerOnPPQ(LPCTSTR PPQName, LPCTSTR TriggerName)
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

void SVOConfigAssistantDlg::LastInspectionLabelDeleted(LPCTSTR InspectionLabel)
{
	int iNum = atoi( SvUl::Mid( std::string(InspectionLabel), 11 ).c_str() );
	if ( iNum > m_iLastInspectionNum )
	{
		m_iLastInspectionNum = iNum;
	}
}

void SVOConfigAssistantDlg::ClearMessages()
{
	m_ctlMsgList.ResetContent();

	OnChangeEditConfigurationName();
}

void SVOConfigAssistantDlg::RemoveFileAcquisitionMessages( LPCTSTR CameraName )
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

HRESULT SVOConfigAssistantDlg::CheckCamera( SVOCameraObj& rCameraObj, bool SetFileParameters )
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

			bool bDisplayWarnings = TheSVObserverApp().GetSVIMType() == GetProductType();
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
					if ( l_HardwareParams.ParameterExists( DeviceParamCameraFormats ) )
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

void SVOConfigAssistantDlg::CheckColor( const SVOCameraObj& rCameraObj )
{

	int Count = m_InspectList.GetInspectionListCount();

	for ( int i = 0; i < Count; i++ )
	{
		const SVOInspectionObjPtr pInspectionObj( GetInspectionObject(i) );

		if( nullptr != pInspectionObj )
		{
			if( pInspectionObj->GetToolsetImage() == rCameraObj.GetCameraDisplayName() )
			{
				std::string Message( BuildDisplayMessage(MESSAGE_TYPE_ERROR, pInspectionObj->GetOriginalName().c_str(), MESSAGE_INSPECTION_CAMERA_COLOR) );
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

void SVOConfigAssistantDlg::CheckTriggers()
{
	int iTrg = GetTriggerListCount();
	for (int i = 0;i < iTrg;i++)
	{
		const SvTrig::SVOTriggerObjPtr pTriggerObj( GetTriggerObject(i) );
		if( nullptr != pTriggerObj )
		{
			CheckTrigger( *pTriggerObj );
		}
	}
	IsGood();
}

bool SVOConfigAssistantDlg::CheckTrigger( const SvTrig::SVOTriggerObj& rTriggerObj)
{
	bool bRet = true;
	std::string TriggerName = rTriggerObj.GetTriggerDisplayName();
	std::string MessageNoSoftwareTriggerAllowed = BuildDisplayMessage(MESSAGE_TYPE_ERROR, TriggerName.c_str(), MESSAGE_SOFTWARE_TRIGGER_NOT_ALLOWED);
	std::string MessageNoCameraTriggerAllowed = BuildDisplayMessage(MESSAGE_TYPE_ERROR, TriggerName.c_str(), MESSAGE_CAMERA_TRIGGER_NOT_ALLOWED);
	std::string MessageToManyCamerasWithCameraTrigger = BuildDisplayMessage(MESSAGE_TYPE_WARNING, TriggerName.c_str(), MESSAGE_CAMERA_TRIGGER_TO_MANY_CAMERAS);

	RemoveMessageFromList(MessageNoSoftwareTriggerAllowed.c_str());
	RemoveMessageFromList(MessageNoCameraTriggerAllowed.c_str());
	RemoveMessageFromList(MessageToManyCamerasWithCameraTrigger.c_str());

	if (SvDef::TriggerType::SoftwareTrigger == rTriggerObj.getTriggerType())
	{
		bRet = IsSoftwareTriggerAllowed( TriggerName.c_str() );
		if (false == bRet)
		{
			AddMessageToList(TRIGGER_DLG, MessageNoSoftwareTriggerAllowed.c_str() );
		}
	}
	if (SvDef::TriggerType::CameraTrigger == rTriggerObj.getTriggerType())
	{
		bRet = IsCameraTriggerAllowed(TriggerName.c_str());
		if (false == bRet)
		{
			AddMessageToList(TRIGGER_DLG, MessageNoCameraTriggerAllowed.c_str());
		}
		else
		{
			bRet = HasExactlyOneHWCameraForTrigger(TriggerName.c_str());
			if (false == bRet)
			{
				AddMessageToList(TRIGGER_DLG, MessageToManyCamerasWithCameraTrigger.c_str());
			}
		}
	}
	return  bRet;
}

HRESULT SVOConfigAssistantDlg::ConnectToolsetBuffers()
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

void SVOConfigAssistantDlg::SetupTriggerStrobeMessage()
{
	// Check the ioboard capabilities.  If it has less inverters than triggers 
	// then display a warning message.
	if ( SVHardwareManifest::IsDigitalSVIM( m_lConfigurationType ) )
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

void SVOConfigAssistantDlg::SetIOBoardCapabilities(const std::string& rIOBoard, SVIMProductEnum eSVIMType)
{
	SVIOBoardCapabilities svCapable;
	unsigned long lCount;
	SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount(lCount);
	svCapable.SetInputCount(lCount);
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(lCount);
	svCapable.SetOutputCount(lCount);

	// Special code to determine the inverters 
	// and triggers based on IO board
	if (_T("10") == rIOBoard)
	{
		svCapable.SetStrobeInverters(1);
		svCapable.SetTriggerInverters(1);
		svCapable.SetTriggerCount(1);
	}
	else if (_T("12") == rIOBoard)
	{
		svCapable.SetStrobeInverters(1);
		svCapable.SetTriggerInverters(1);
		svCapable.SetTriggerCount(3);
	}
	else if (_T("00") == rIOBoard)
	{
		// Get Trigger count from the TriggerDLL (in this case the DigitizerDLL)
		const SVIMTypeInfoStruct& info = SVHardwareManifest::GetSVIMTypeInfo(eSVIMType);
		svCapable.SetNonIOSVIM(info.m_MaxTriggers);
	}
	else if (_T("30") == rIOBoard)
	{
		// Get Trigger count from the SVPlcIO.dll
		const SVIMTypeInfoStruct& info = SVHardwareManifest::GetSVIMTypeInfo(eSVIMType);
		svCapable.SetStrobeInverters(info.m_MaxTriggers);
		svCapable.SetTriggerInverters(info.m_MaxTriggers);
		svCapable.SetTriggerCount(info.m_MaxTriggers);
	}
	else
	{
		svCapable.SetStrobeInverters(3);
		svCapable.SetTriggerInverters(3);
		svCapable.SetTriggerCount(3);
	}

	m_svCapabilities = svCapable;
}

void SVOConfigAssistantDlg::OnHelpButton()
{
	long lContextID = m_dlgPropSheet.GetActivePage()->GetWindowContextHelpId();
	OnCommandHelp(0,lContextID);
}

SVImportedInspectionInfoList& SVOConfigAssistantDlg::GetImportedInspectionInfoList()
{
	return m_ImportedInspectionInfoList;
}

void SVOConfigAssistantDlg::ClearImportedInspectionInfoList()
{
	m_ImportedInspectionInfoList.clear();
}

std::string SVOConfigAssistantDlg::GetNameFromProductID( SVIMProductEnum productID )
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

SVIMProductEnum SVOConfigAssistantDlg::GetProductIDFromName( const std::string& rName )
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

bool SVOConfigAssistantDlg::IsGigeSystem() const
{
	return SVHardwareManifest::IsMatroxGige(GetProductType());
}

bool SVOConfigAssistantDlg::IsFileAcquisition(int iDig) const
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

void SVOConfigAssistantDlg::OnBnClickedCancel()
{
	// use m_TmpCameraList to get back the orginial camera information
	if ( SVHardwareManifest::IsDigitalSVIM( GetProductType() ) )
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

void SVOConfigAssistantDlg::resolveGlobalConflicts( SvUl::GlobalConflictPairVector& rGlobalConflicts )
{
	if( 0 < rGlobalConflicts.size() )
	{
		SvOg::GlobalConstantConflictDlg ConflictDlg( rGlobalConflicts, this );
		if( IDOK == ConflictDlg.DoModal() )
		{
			SvUl::GlobalConflictPairVector::const_iterator Iter( rGlobalConflicts.cbegin() );

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
