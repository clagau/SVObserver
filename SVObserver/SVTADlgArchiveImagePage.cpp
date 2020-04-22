	//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgArchiveImagePage
//* .File Name       : $Workfile:   SVTADlgArchiveImagePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.15  $
//* .Check In Date   : $Date:   10 Sep 2014 09:36:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <numeric>
#include "SVTADlgArchiveImagePage.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "TextDefinesSvO.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "InspectionEngine/SVImageClass.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOLibrary/SVMemoryManager.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Tools/SVArchiveTool.h"
#include "Tools/ArchiveToolHelper.h"
#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVTADlgArchiveImagePage, CPropertyPage)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SELECTED, OnDblClickListSelected)
	ON_BN_CLICKED(IDC_SELECT_BUTTON, OnSelectObjects)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnRemoveItem)
	ON_BN_CLICKED(IDC_BTN_CLEAR_ALL, OnRemoveAllItems)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_CBN_SELCHANGE(IDC_MODE_COMBO, OnSelchangeModeCombo)
	ON_EN_CHANGE(IDC_EDIT_MAX_IMAGES, OnChangeEditMaxImages)
	ON_BN_CLICKED(IDC_CHECK_STOP_AT_MAX, UpdateMaxImageWidgetState)
	ON_BN_CLICKED(IDC_USE_TRIGGER_COUNT, UpdateMaxImageWidgetState)
	ON_BN_CLICKED(IDC_BUTTON_FILENAME_INDEX1, OnButtonFilenameIndex1)
	ON_BN_CLICKED(IDC_BUTTON_FILENAME_INDEX2, OnButtonFilenameIndex2)
	ON_BN_CLICKED(IDC_BUTTON_DIRECTORYNAME_INDEX, OnButtonDirectorynameIndex)
	ON_BN_CLICKED(IDC_BUTTON_SUBFOLDER_SELECTION, OnButtonSubfolderSelection)
	ON_BN_CLICKED(IDC_BUTTON_SUBFOLDER_LOCATION, OnButtonSubfolderLocation)
	ON_BN_CLICKED(IDC_USE_ALTERNATIVE_IMAGE_PATHS, OnButtonUseAlternativeImagePaths)


END_MESSAGE_MAP()

constexpr int UpperLimitImageNumbers = 10000000; ///Upper Limit for Input 
constexpr int ItemSelectedCol = 1;
constexpr int ItemsSelectedWidth = 500;
constexpr int IconNumber = 1;
constexpr int IconGrowBy = 1;
constexpr long MaxImagesWarningLimit = 100L;
#pragma endregion Declarations

#pragma region Constructor
SVTADlgArchiveImagePage::SVTADlgArchiveImagePage(uint32_t inspectionId, uint32_t taskObjectId, SVToolAdjustmentDialogSheetClass* Parent) 
: CPropertyPage(SVTADlgArchiveImagePage::IDD)
, m_pParent(Parent)
, m_objectSelector(inspectionId)
, m_Values{ SvOg::BoundValues{ inspectionId, taskObjectId } }
, m_alternativeImagePaths(m_Values)
{
	if( m_pParent )
	{
		m_pTool = dynamic_cast <SvTo::SVArchiveTool*> (m_pParent->GetTaskObject());
	}

}

SVTADlgArchiveImagePage::~SVTADlgArchiveImagePage()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool SVTADlgArchiveImagePage::QueryAllowExit() 
{
	UpdateData(TRUE); 

	//check to see if mode is SVArchiveGoOffline.  
	if (SvTo::SVArchiveGoOffline == m_eSelectedArchiveMethod)
	{
		//check to see if any items are selected in the image tree
		
		int iSize = static_cast<int> (m_List.size());
		if( 0 < iSize )
		{
			//if memory usage < 0 do not all them to exit
			__int64 FreeMemory = CalculateFreeMem();
			if (0 > FreeMemory)
			{
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_AP_NotEnoughMemoryPleaseDeselect, SvStl::SourceFileParams(StdMessageParams) );
				return false;
			}
		}
	}

	m_alternativeImagePaths.EditboxesToTextValues();

	//update the image path
	CString Text;
	m_ImageFilesRoot.GetWindowText( Text );
	std::string ImageFolder = Text;

	SvTo::ArchiveToolHelper athImagePath;
	athImagePath.Init(ImageFolder);

	if (athImagePath.isUsingKeywords())
	{
		if (athImagePath.isTokensValid())
		{
			std::string TmpArchiveImagePath = athImagePath.TranslatePath(ImageFolder);
			SVCheckPathDir( TmpArchiveImagePath.c_str(), TRUE );
		}
		else
		{
			//don't allow to exit with invalid path
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_InvalidImagePath, SvStl::SourceFileParams(StdMessageParams) );
			return false;
		}
	}
	else
	{
		//not using Keywords 
		SVCheckPathDir( ImageFolder.c_str(), TRUE );
	}

	std::string Drive;
	//check for valid drive for image archive
	if(!SvTo::ArchiveToolHelper::ValidateDrive(ImageFolder.c_str(), Drive ) || ImageFolder.empty())
	{
		SvDef::StringVector msgList;
		msgList.push_back(Drive);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_InvalidDrive, msgList, SvStl::SourceFileParams(StdMessageParams) );
		return false; 
	}

	m_pTool->SetImageArchivePath( ImageFolder.c_str() );

	m_pTool->m_dwArchiveStopAtMaxImages.SetValue(static_cast<DWORD> (m_StopAtMaxImagesButton.GetCheck()));
	m_pTool->m_dwUseTriggerCountForImages.SetValue(static_cast<DWORD> (m_UseTriggerCount));
	m_pTool->m_useAlternativeImagePaths.SetValue(m_useAlternativeImagePaths);

	int iCurSel = m_Mode.GetCurSel();
	m_eSelectedArchiveMethod = static_cast<SvTo::SVArchiveMethodEnum> (m_Mode.GetItemData(iCurSel));
	m_pTool->m_evoArchiveMethod.SetValue(static_cast<long> (m_eSelectedArchiveMethod));

	m_EditMaxImages.GetWindowText(Text);
	m_ImagesToArchive = atol(Text);
	if( UpperLimitImageNumbers < m_ImagesToArchive)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%ld"), m_ImagesToArchive));
		msgList.push_back(SvUl::Format(_T("%ld"), UpperLimitImageNumbers));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_Error_you_have_Selected_X_Must_less_then_Y, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16075_ImageNrToBig  );
		return false;

	}

	if( MaxImagesWarningLimit < m_ImagesToArchive)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%ld"),  m_ImagesToArchive));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
		Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_WarningMaxImages, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
	//Max images must be at least 1
	if( 1L > m_ImagesToArchive)
	{
		m_ImagesToArchive= 1L;
	}

	m_pTool->m_dwArchiveMaxImagesCount.SetValue(m_ImagesToArchive);

	m_pTool->setImageArchiveList(m_List);

	// Mark the document as 'dirty' so user will be prompted to save
	// this configuration on program exit.
	if (m_pParent)
	{
		SVIPDoc* pIPDoc = m_pParent->GetIPDoc();

		if( nullptr != pIPDoc )
		{
			pIPDoc->SetModifiedFlag();
		}
	}

	m_Values.Commit();

	m_pTool->resetAllObjects();

	return true;
}


#pragma endregion Public Methods

#pragma region Private Methods
void SVTADlgArchiveImagePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	
	m_alternativeImagePaths.DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SELECTED, m_ItemsSelected);
	DDX_Control(pDX, IDC_SELECT_BUTTON, m_Select);
	DDX_Control(pDX, IDC_AVAILABLE_ARCHIVE_IMAGE_MEMORY, m_wndAvailableArchiveImageMemory);
	DDX_Control(pDX, IDC_MODE_COMBO, m_Mode);
	DDX_Control(pDX, IDC_EDIT_MAX_IMAGES, m_EditMaxImages);
	DDX_Control(pDX, IDC_ARCHIVE_IMAGE_FILEROOT, m_ImageFilesRoot);
	DDX_Control(pDX, IDC_USE_TRIGGER_COUNT, m_UseTriggerCountButton);
	DDX_Control(pDX, IDC_CHECK_STOP_AT_MAX, m_StopAtMaxImagesButton);
	DDX_Check(pDX, IDC_USE_TRIGGER_COUNT, m_UseTriggerCount);
	DDX_Check(pDX, IDC_USE_ALTERNATIVE_IMAGE_PATHS, m_useAlternativeImagePaths);
	DDX_CBIndex(pDX, IDC_MODE_COMBO, m_iModeIndex);

	EnableMaxImagesAccordingToOtherSettings();
}


void SVTADlgArchiveImagePage::EnableMaxImagesAccordingToOtherSettings()
{
	m_EditMaxImages.EnableWindow((!(m_UseTriggerCount|| m_useAlternativeImagePaths) || m_StopAtMaxImagesButton.GetCheck()));
}


BOOL SVTADlgArchiveImagePage::OnInitDialog() 
{
	CWaitCursor Wait;
	m_Init = true;

	CPropertyPage::OnInitDialog();

	m_Values.Init();

	GetWindowText( m_strCaption );

	m_ItemsSelected.InsertColumn( ItemSelectedCol, _T(""), 0, ItemsSelectedWidth );
	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR24 | ILC_MASK, IconNumber, IconGrowBy );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_CHECKED_ENABLED ) );
	m_ItemsSelected.SetImageList( &m_StateImageList, LVSIL_STATE );

	m_TreeBitmap.LoadBitmap( IDB_TREE );
	m_Select.SetBitmap( static_cast<HBITMAP> (m_TreeBitmap.GetSafeHandle()) );

	CDWordArray dwaIndex;
	
	for (auto const& rEntry : m_pTool->m_evoArchiveMethod.GetEnumVector())
	{
		int iIndex = m_Mode.AddString(rEntry.first.c_str());
		m_Mode.SetItemData( iIndex, static_cast<DWORD_PTR> (rEntry.second) );
		dwaIndex.SetAtGrow( rEntry.second, iIndex );
	}

	long lMode;
	m_pTool->m_evoArchiveMethod.GetValue( lMode );
	m_eSelectedArchiveMethod = static_cast<SvTo::SVArchiveMethodEnum>( lMode );
	m_iModeIndex = dwaIndex.GetAt( lMode );

	DWORD dwTemp=0;
    m_pTool->m_dwArchiveMaxImagesCount.GetValue( dwTemp );
	m_ImagesToArchive = dwTemp;
	std::string Text = SvUl::Format(_T("%ld"),dwTemp);
	m_EditMaxImages.SetWindowText( Text.c_str() );

	//store the MaxImageNumber
	// cppcheck-suppress danglingLifetime //the pointer is immediately converted to a CString and does not "dangle"
	m_sMaxImageNumber = Text.c_str(); 

	__int64 MemUsed = TheSVMemoryManager().ReservedBytes( SvDef::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME );
 	m_ToolImageMemoryUsage = 0;
	m_TotalArchiveImageMemoryAvailable = TheSVMemoryManager().SizeOfPoolBytes( SvDef::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME );

	m_wndAvailableArchiveImageMemory.ShowWindow( lMode == SvTo::SVArchiveGoOffline );

	std::string ImageFolder; 
	
	m_pTool->GetImageArchivePath( ImageFolder );
	m_ImageFilesRoot.SetWindowText( ImageFolder.c_str() );

	m_pTool->m_dwArchiveStopAtMaxImages.GetValue( dwTemp );
	m_StopAtMaxImagesButton.SetCheck(static_cast<BOOL>(dwTemp));

	m_pTool->m_dwUseTriggerCountForImages.GetValue(dwTemp);
	m_UseTriggerCount = static_cast<BOOL>(dwTemp);

	m_pTool->m_useAlternativeImagePaths.GetValue(m_useAlternativeImagePaths);

	m_List.swap(m_pTool->getImageArchiveList());

	m_mapInitialSelectedImageMemUsage = m_mapSelectedImageMemUsage;
	m_ToolImageMemoryUsage = CalculateToolMemoryUsage();
	m_InitialToolImageMemoryUsage = m_ToolImageMemoryUsage;

	m_InitialArchiveImageMemoryUsageExcludingThisTool = MemUsed;

	// calculate free mem if in SVArchiveGoOffline mode
	if (SvTo::SVArchiveGoOffline == m_eSelectedArchiveMethod)
	{
		m_InitialArchiveImageMemoryUsageExcludingThisTool -= m_ToolImageMemoryUsage;
	}

	m_alternativeImagePaths.TextValuesToEditboxes();

	ReadSelectedObjects();

	m_Init = false;
	UpdateData(FALSE);

	OnButtonUseAlternativeImagePaths();

	return TRUE;
}

void SVTADlgArchiveImagePage::OnDblClickListSelected( NMHDR *pNMHDR, LRESULT *pResult )
{
	ShowObjectSelector();
}

void SVTADlgArchiveImagePage::OnSelectObjects()
{
	ShowObjectSelector();
}

void SVTADlgArchiveImagePage::OnRemoveAllItems()
{
	m_List.clear();
	ReadSelectedObjects();
}

void SVTADlgArchiveImagePage::OnRemoveItem()
{
	POSITION Pos = m_ItemsSelected.GetFirstSelectedItemPosition();
	std::vector<int> SelectedVector;

	while( nullptr != Pos )
	{
		int ItemIndex = m_ItemsSelected.GetNextSelectedItem( Pos );
		SelectedVector.push_back( ItemIndex );
	}

	//Remove in reverse order
	std::vector<int>::const_reverse_iterator Iter;
	for( Iter = SelectedVector.crbegin(); SelectedVector.crend() != Iter; ++Iter )
	{
		SVObjectReferenceVector::const_iterator SelectedIter( m_List.begin() + *Iter );
		m_List.erase( SelectedIter );
	}

	ReadSelectedObjects();
}

void SVTADlgArchiveImagePage::MemoryUsage()
{
	m_mapSelectedImageMemUsage.clear();

	for (auto const& rEntry : m_List )
	{
		SVObjectClass* pObject( nullptr );
		SVObjectManagerClass::Instance().GetObjectByIdentifier(rEntry.getObjectId(), pObject );

		if( nullptr != pObject )
		{
			SvIe::SVImageClass* pImage = dynamic_cast <SvIe::SVImageClass*> ( pObject );// cppcheck-suppress knownConditionTrueFalse //dynamic cast can in fact return nullptr!
			if ( nullptr != pImage )
			{
				m_mapSelectedImageMemUsage[ pImage ] = SvTo::SVArchiveTool::CalculateImageMemory( pImage );
			}
		}
	}
}


void SVTADlgArchiveImagePage::ReadSelectedObjects()
{
	m_ItemsSelected.DeleteAllItems();

	MemoryUsage();
	if(SvTo::SVArchiveGoOffline == m_eSelectedArchiveMethod)
	{
		CalculateFreeMem();
	}

	std::string ToolName = SvUl::LoadStdString( IDS_CLASSNAME_SVTOOLSET );
	std::string Prefix = SvUl::Format( _T("%s.%s."), m_pTool->GetInspection()->GetName(), ToolName.c_str() );

	int Index = 0;
	for (auto const& rEntry : m_List)
	{
		std::string Name{ rEntry.GetCompleteName(true) };
		SvUl::searchAndReplace( Name, Prefix.c_str(), _T("") );

		m_ItemsSelected.InsertItem(LVIF_STATE | LVIF_TEXT,
			Index,
			Name.c_str(),
			INDEXTOSTATEIMAGEMASK(1),
			LVIS_STATEIMAGEMASK,
			1,
			0);
		Index++;
	}
}

void SVTADlgArchiveImagePage::ShowObjectSelector()
{
	uint32_t inspectionId(m_pTool->GetInspection()->getObjectId());

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::ObjectSelectorType::toolsetItems}, inspectionId, SvPb::archivableImage);
	SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject, IDD + SvOr::HELPFILE_DLG_IDD_OFFSET);
	if (responseCmd.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
	}

	SvDef::StringSet CheckItems;
	for (auto const& rEntry : m_List)
	{
		CheckItems.insert(rEntry.GetObjectNameToObjectType(SvPb::SVToolSetObjectType, true));
	}
	SvOsl::ObjectTreeGenerator::Instance().setCheckItems( CheckItems );

	std::string Title = SvUl::Format( _T("%s - %s"), m_strCaption, m_pTool->GetInspection()->GetName());
	std::string Filter = SvUl::LoadStdString( IDS_FILTER );
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), m_strCaption, Filter.c_str(), this );

	if( IDOK == Result )
	{
		const SvDef::StringVector& rSelectedList = SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects();
		//We need to check the memory requirements for the images
		for(auto const& rEntry : SvOsl::ObjectTreeGenerator::Instance().getModifiedObjects())
		{
			try
			{
				uint32_t ImageId = calcObjectId(rEntry);
				bool isSelected = rSelectedList.end() != std::find(rSelectedList.begin(), rSelectedList.end(), rEntry);
				if (!checkImageMemory(ImageId, isSelected))
				{
					//If memory check failed then the original image selection remains
					return;
				}
			}
			catch (...)
			{
				//If memory check failed then the original image selection remains
				return;
			}
		}

		m_List.clear();
		for (auto const& rEntry : rSelectedList)
		{
			SVObjectReference ObjectRef(rEntry);
			m_List.push_back(ObjectRef);
		}

		ReadSelectedObjects();
	}
}


void SVTADlgArchiveImagePage::UpdateMaxImageWidgetState()
{
	UpdateData(TRUE);
	EnableMaxImagesAccordingToOtherSettings();
}


void SVTADlgArchiveImagePage::OnBrowse() 
{
	SVFileNameClass	svfncImageFolder;

	// Set the default folder to start with.
	CString csInitialPath(SvStl::GlobalPath::Inst().GetTempPath(_T("\\")).c_str());

	m_ImageFilesRoot.GetWindowText(csInitialPath);
	if (!csInitialPath.IsEmpty())
	{
		svfncImageFolder.SetPathName(csInitialPath);
	}

	SvTo::ArchiveToolHelper athImagePath;
	athImagePath.Init(std::string(csInitialPath)); 

	bool bUsingKeywords = athImagePath.isUsingKeywords();
	if (bUsingKeywords)
	{
		if (athImagePath.isTokensValid())
		{
			csInitialPath = athImagePath.TranslatePath(std::string(csInitialPath)).c_str();
		}
		else
		{
			//don't allow to exit with invalid path
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_InvalidImagePath, SvStl::SourceFileParams(StdMessageParams) );
			return;
		}
	}
    //
    // Select the folder to copy to..
    //
	svfncImageFolder.SetFileType(SV_DEFAULT_FILE_TYPE);
	if (svfncImageFolder.SelectPath())
    {
		csInitialPath = svfncImageFolder.GetPathName().c_str();
		int len = csInitialPath.GetLength();
		if (len)
		{
			if (csInitialPath.GetAt(len - 1) != _T('\\'))
			{
				csInitialPath += _T('\\');
			}
		}
    }
	m_ImageFilesRoot.SetWindowText (csInitialPath);
}

void SVTADlgArchiveImagePage::OnSelchangeModeCombo() 
{
	UpdateData();
	int iSel = m_Mode.GetCurSel();

	forceStopAtMaxIfRequired();

	if (iSel != CB_ERR)
	{
		m_eSelectedArchiveMethod = static_cast <SvTo::SVArchiveMethodEnum> (m_Mode.GetItemData( iSel ));
		m_wndAvailableArchiveImageMemory.ShowWindow(SvTo::SVArchiveGoOffline == m_eSelectedArchiveMethod );

		//if changing to SVArchiveGoOffline mode - build m_mapSelectedImageUsage with selected items in the tree
		if (SvTo::SVArchiveGoOffline == m_eSelectedArchiveMethod)
		{
			MemoryUsage();
			//check to make sure they did not go over the available memory
			__int64 FreeMem = CalculateFreeMem();
			if ( 0 > FreeMem )
			{
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_AP_NotEnoughMemoryInChangeMode, SvStl::SourceFileParams(StdMessageParams) );
			}
		}
		else if(SvTo::SVArchiveSynchronous == m_eSelectedArchiveMethod )
		{
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Exception.setMessage( SVMSG_SVO_89_ARCHIVE_TOOL_SYNCHRONOUS_MODE, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		}
		forceStopAtMaxIfRequired();
	}
}


void SVTADlgArchiveImagePage::forceStopAtMaxIfRequired()
{
	BOOL stopAtMaxRequired = m_useAlternativeImagePaths && (SvTo::SVArchiveGoOffline == m_eSelectedArchiveMethod);
	m_StopAtMaxImagesButton.EnableWindow(!stopAtMaxRequired);
	if (stopAtMaxRequired)
	{
		m_StopAtMaxImagesButton.SetCheck(TRUE);
	}
}


void SVTADlgArchiveImagePage::OnChangeEditMaxImages() 
{
	CString strNumImages;
	m_EditMaxImages.GetWindowText(strNumImages);
	m_ImagesToArchive = atol(strNumImages);

	if (!m_Init)
	{
		//check to make sure they don't type in a value below 1
		if ( 0 < m_ImagesToArchive )
		{  
			//check to make sure we don't go over the amount of free memory
			if (SvTo::SVArchiveGoOffline == m_eSelectedArchiveMethod)
			{
				__int64 llFreeMem = CalculateFreeMem();
				if( 0 <= llFreeMem )
				{
					m_sMaxImageNumber = strNumImages;
					m_ImagesToArchive = atol(strNumImages);
				}
				else
				{
					SvDef::StringVector msgList;
					msgList.push_back(std::string(strNumImages));
					SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
					Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_AP_NotEnoughImageMemoryInChangeMode, msgList, SvStl::SourceFileParams(StdMessageParams) );
					m_ImagesToArchive = atol(m_sMaxImageNumber);
					if(m_sMaxImageNumber != strNumImages)
					{
						m_EditMaxImages.SetWindowText((LPCSTR)m_sMaxImageNumber);
					}
				}
			}
			else
			{
				m_sMaxImageNumber = strNumImages;
			}
		}
		else
		{
			m_EditMaxImages.SetWindowText(m_sMaxImageNumber);
		}
	}
}

bool SVTADlgArchiveImagePage::checkImageMemory(uint32_t imageId, bool bNewState)
{
	UpdateData();
	bool bOk = true;

	SVObjectClass* pObject( nullptr );
	SVObjectManagerClass::Instance().GetObjectByIdentifier(imageId, pObject );
	SvIe::SVImageClass* pImage = dynamic_cast <SvIe::SVImageClass*> ( pObject );
	ASSERT(pImage);

	//Get amount of memory needed for the selected image.
	long MemoryForSelectedImage = SvTo::SVArchiveTool::CalculateImageMemory( pImage );
	MemoryForSelectedImage *= m_ImagesToArchive;

	if( bNewState )// want to select
	{	
		bool bAddItem = true;

		//only check for memory if in mode SVArchiveGoOffline
		if (SvTo::SVArchiveGoOffline == m_eSelectedArchiveMethod)
		{
			__int64 CurrentToolFreeMem = CalculateFreeMem();
			//lDelta is the total amount of memory that will need to be allocated.  Only gets commited once the tool's reset object gets called.
			__int64 lDelta = MemoryForSelectedImage - m_InitialToolImageMemoryUsage + m_ToolImageMemoryUsage;			
			__int64 Difference = CurrentToolFreeMem - MemoryForSelectedImage;

			bool bCanReserve = false;
			if( 0 <= Difference )
			{
				bCanReserve = true;
			}

			if (bCanReserve && (0 < lDelta))
			{
				bCanReserve = TheSVMemoryManager().CanReservePoolMemory( SvDef::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, lDelta );
			}

			if (bCanReserve)
			{
				bAddItem = true;
				m_mapSelectedImageMemUsage[ pImage ] = MemoryForSelectedImage / m_ImagesToArchive;
				CalculateFreeMem();
			}
			else
			{
				bAddItem = false;
				bOk = false;
				SvDef::StringVector msgList;
				msgList.push_back(std::string(pImage->GetCompleteName()));
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_AP_NotEnoughImageMemoryToSelect, msgList, SvStl::SourceFileParams(StdMessageParams) );
			}
		}
		if (bAddItem)
		{
			m_mapSelectedImageMemUsage[ pImage ] = MemoryForSelectedImage / m_ImagesToArchive;
		}
	}
	else	// want to deselect
	{
		m_mapSelectedImageMemUsage.erase( pImage );

		//Calculate Free Mem if in SVArchiveGoOffline mode
		if (SvTo::SVArchiveGoOffline == m_eSelectedArchiveMethod)
		{
			__int64 FreeMem = CalculateFreeMem();

			if( 0 > FreeMem )
			{
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_AP_NotEnoughMemoryPleaseDeselectImage, SvStl::SourceFileParams(StdMessageParams) );

			}
		}
	}
	return bOk;
}

__int64 SVTADlgArchiveImagePage::CalculateToolMemoryUsage()
{
	__int64 ToolImageMemoryUsage( 0 );

	MapSelectedImageType::const_iterator iter;
	for (iter = m_mapSelectedImageMemUsage.begin(); iter != m_mapSelectedImageMemUsage.end(); ++iter)
	{
		ToolImageMemoryUsage += iter->second;
	}
	ToolImageMemoryUsage *= m_ImagesToArchive;

	return ToolImageMemoryUsage;
}

__int64 SVTADlgArchiveImagePage::CalculateFreeMem()
{
	m_ToolImageMemoryUsage = CalculateToolMemoryUsage();
	__int64 FreeMem( -1 );

	if( 0 <= m_ToolImageMemoryUsage )
	{
		FreeMem = m_TotalArchiveImageMemoryAvailable - (m_ToolImageMemoryUsage + m_InitialArchiveImageMemoryUsageExcludingThisTool);
		std::string AvailableMemory = SvUl::Format( SvO::AvailableArchiveImageMemory, (double) FreeMem / (double) (1024 * 1024) );
		m_wndAvailableArchiveImageMemory.SetWindowText( AvailableMemory.c_str() );
	}

	return FreeMem;
}

afx_msg void SVTADlgArchiveImagePage::OnButtonUseAlternativeImagePaths()
{
	UpdateData(TRUE);
	
	m_alternativeImagePaths.OnButtonUseAlternativeImagePaths(m_useAlternativeImagePaths);
	m_UseTriggerCountButton.EnableWindow(!m_useAlternativeImagePaths);

	forceStopAtMaxIfRequired();
}


void SVTADlgArchiveImagePage::AlternativeImagePaths::EditboxToTextValue(ValueAndGuiInformation& uiInfo)
{
	CString Text;
	uiInfo.m_valueEdit.GetWindowText(Text);
	m_rValues.Set<CString>(uiInfo.m_embeddedId, Text);
}

void SVTADlgArchiveImagePage::AlternativeImagePaths::TextValueToEditbox(ValueAndGuiInformation& uiInfo)
{
	std::string temp;
	
	if (SvPb::NoEmbeddedId != uiInfo.m_embeddedLinkId) //i.e. this is a linked value
	{
		temp = m_rValues.Get<CString>(uiInfo.m_embeddedLinkId); //See whether we can get a dotted name
	}

	if (temp.empty()) // because this is not a linked value (or it is, but there is no associated dotted name)
	{
		temp = m_rValues.Get<CString>(uiInfo.m_embeddedId);
	}

	uiInfo.m_valueEdit.SetWindowText(temp.c_str());
	
	if (uiInfo.m_pDottedNameSelectButton) //We have an associated button that should contain a down arrow
	{
		uiInfo.m_pDottedNameSelectButton->SetBitmap(static_cast<HBITMAP> (m_downArrowBitmap)); // Put the Down Arrow on the Button
	}
}


void SVTADlgArchiveImagePage::AlternativeImagePaths::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_EDIT_FILENAME_INDEX1, m_EditFilenameIndex1);
	DDX_Control(pDX, IDC_BUTTON_FILENAME_INDEX1, m_ButtonFilenameIndex1);
	DDX_Control(pDX, IDC_EDIT_FILENAME_INDEX2, m_EditFilenameIndex2);
	DDX_Control(pDX, IDC_BUTTON_FILENAME_INDEX2, m_ButtonFilenameIndex2);
	DDX_Control(pDX, IDC_EDIT_DIRECTORYNAME_BASE, m_EditBaseDirectoryname);
	DDX_Control(pDX, IDC_EDIT_SUBFOLDER_SELECTION, m_EditSubfolderSelection);
	DDX_Control(pDX, IDC_BUTTON_SUBFOLDER_SELECTION, m_ButtonSubfolderSelection);
	DDX_Control(pDX, IDC_EDIT_SUBFOLDER_LOCATION, m_EditSubfolderLocation);
	DDX_Control(pDX, IDC_BUTTON_SUBFOLDER_LOCATION, m_ButtonSubfolderLocation);
	DDX_Control(pDX, IDC_EDIT_FILENAME_BASE, m_EditBaseFilename);
	DDX_Control(pDX, IDC_EDIT_FILENAME_CENTER, m_EditCenterFilename);
	DDX_Control(pDX, IDC_EDIT_DIRECTORYNAME_INDEX, m_EditDirectorynameIndex);
	DDX_Control(pDX, IDC_BUTTON_DIRECTORYNAME_INDEX, m_ButtonDirectorynameIndex);

}

void SVTADlgArchiveImagePage::AlternativeImagePaths::SelectFilenameIndex1(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent)
{
	CString Temp;
	m_EditFilenameIndex1.GetWindowText(Temp);
	std::string Value = Temp;
	std::string Title = SvUl::LoadStdString(IDS_OBJECTNAME_FILENAME_INDEX1);
	if (rObjectSelector.Show(Value, Title, pParent))
	{
		m_EditFilenameIndex1.SetWindowText(Value.c_str());
	}
}


void SVTADlgArchiveImagePage::AlternativeImagePaths::SelectFilenameIndex2(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent)
{
	CString Temp;
	m_EditFilenameIndex2.GetWindowText(Temp);
	std::string Value = Temp;
	std::string Title = SvUl::LoadStdString(IDS_OBJECTNAME_FILENAME_INDEX2);
	if (rObjectSelector.Show(Value, Title, pParent))
	{
		m_EditFilenameIndex2.SetWindowText(Value.c_str());
	}
}


void SVTADlgArchiveImagePage::AlternativeImagePaths::SelectDirectorynameIndex(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent)
{
	CString Temp;
	m_EditDirectorynameIndex.GetWindowText(Temp);
	std::string Value = Temp;
	std::string Title = SvUl::LoadStdString(IDS_OBJECTNAME_DIRECTORYNAME_INDEX);
	if (rObjectSelector.Show(Value, Title, pParent))
	{
		m_EditDirectorynameIndex.SetWindowText(Value.c_str());
	}
}


void SVTADlgArchiveImagePage::AlternativeImagePaths::SelectSubfolderSelection(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent)
{
	CString Temp;
	m_EditSubfolderSelection.GetWindowText(Temp);
	std::string Value = Temp;
	std::string Title = SvUl::LoadStdString(IDS_OBJECTNAME_SUBFOLDER_SELECTION);
	if (rObjectSelector.Show(Value, Title, pParent))
	{
		m_EditSubfolderSelection.SetWindowText(Value.c_str());
	}
}


void SVTADlgArchiveImagePage::AlternativeImagePaths::SelectSubfolderLocation(SvOg::ObjectSelectorController& rObjectSelector, CWnd* pParent)
{
	CString Temp;
	m_EditSubfolderLocation.GetWindowText(Temp);
	std::string Value = Temp;
	std::string Title = SvUl::LoadStdString(IDS_OBJECTNAME_SUBFOLDER_LOCATION);
	if (rObjectSelector.Show(Value, Title, pParent))
	{
		m_EditSubfolderLocation.SetWindowText(Value.c_str());
	}
}


afx_msg void SVTADlgArchiveImagePage::AlternativeImagePaths::OnButtonUseAlternativeImagePaths(BOOL enable)
{
	for (auto & uiInfo : m_vecValueAndGuiInfo)
	{
		uiInfo.m_valueEdit.EnableWindow(enable);
		if (uiInfo.m_pDottedNameSelectButton) //i.e. this is a linked value
		{
			uiInfo.m_pDottedNameSelectButton->EnableWindow(enable);
		}
	}
}

void SVTADlgArchiveImagePage::AlternativeImagePaths::EditboxesToTextValues()
{
	for (auto & uiInfo : m_vecValueAndGuiInfo)
	{
		EditboxToTextValue(uiInfo);
	}

}


void SVTADlgArchiveImagePage::AlternativeImagePaths::TextValuesToEditboxes()
{
	for (auto & uiInfo : m_vecValueAndGuiInfo)
	{
		TextValueToEditbox(uiInfo);
	}
}



