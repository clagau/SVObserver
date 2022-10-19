//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVToolAdjustmentDialogSheetClass.h"
#include "TextDefinesSvO.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "InspectionEngine/SVImageClass.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVFileSystemLibrary/FilepathUtilities.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Tools/SVArchiveTool.h"
#include "Tools/ArchiveToolHelper.h"
#include "SVOLibrary/SVMemoryManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(SVTADlgArchiveImagePage, CPropertyPage)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SELECTED,			OnDblClickListSelected)
	ON_BN_CLICKED(IDC_SELECT_BUTTON,				OnSelectObjects)
	ON_BN_CLICKED(IDC_BTN_CLEAR,					OnRemoveItem)
	ON_BN_CLICKED(IDC_BTN_CLEAR_ALL,				OnRemoveAllItems)
	ON_BN_CLICKED(IDC_BROWSE_IMAGE_FILEPATHROOT1,	OnBrowseImageFilepathroot1)
	ON_CBN_SELCHANGE(IDC_WHEN_TO_ARCHIVE,			OnSelchangeWhenToArchive)
	ON_EN_CHANGE(IDC_EDIT_MAX_IMAGES,				OnChangeEditMaxImages)
	ON_EN_CHANGE(IDC_EDIT_MAX_QUEUE,				OnChangeMaxImageQueueLength)
	ON_BN_CLICKED(IDC_CHECK_STOP_AT_MAX,			OnStopAtMaxClicked)
	ON_BN_CLICKED(IDC_BTN_DETAILS,					OnDetails)
	ON_BN_CLICKED(IDC_BUTTON_DIRECTORYNAME_BASE,	OnButton<	AipCtr::Lve::BaseDirectoryname>)
	ON_EN_KILLFOCUS(IDC_EDIT_DIRECTORYNAME_BASE,	OnKillFocus<AipCtr::Lve::BaseDirectoryname>)
	ON_BN_CLICKED(IDC_BUTTON_DIRECTORYNAME_INDEX,	OnButton<	AipCtr::Lve::DirectorynameIndex>)
	ON_EN_KILLFOCUS(IDC_EDIT_DIRECTORYNAME_INDEX,	OnKillFocus<AipCtr::Lve::DirectorynameIndex>)
	ON_BN_CLICKED(IDC_BUTTON_SUBFOLDER_SELECTION,	OnButton<	AipCtr::Lve::SubfolderSelection>)
	ON_EN_KILLFOCUS(IDC_EDIT_SUBFOLDER_SELECTION,	OnKillFocus<AipCtr::Lve::SubfolderSelection>)
	ON_BN_CLICKED(IDC_BUTTON_SUBFOLDER_LOCATION,	OnButton<	AipCtr::Lve::SubfolderLocation>)
	ON_EN_KILLFOCUS(IDC_EDIT_SUBFOLDER_LOCATION,	OnKillFocus<AipCtr::Lve::SubfolderLocation>)
	ON_BN_CLICKED(IDC_BUTTON_FILENAME_BASE,			OnButton<	AipCtr::Lve::BaseFilename>)
	ON_EN_KILLFOCUS(IDC_EDIT_FILENAME_BASE,			OnKillFocus<AipCtr::Lve::BaseFilename>)
	ON_BN_CLICKED(	IDC_BUTTON_FILENAME_INDEX1,		OnButton<	AipCtr::Lve::FilenameIndex1>)
	ON_EN_KILLFOCUS(IDC_EDIT_FILENAME_INDEX1,		OnKillFocus<AipCtr::Lve::FilenameIndex1>)
	ON_BN_CLICKED(IDC_BUTTON_FILENAME_CENTER,		OnButton<	AipCtr::Lve::CenterFilename>)
	ON_EN_KILLFOCUS(IDC_EDIT_FILENAME_CENTER,		OnKillFocus<AipCtr::Lve::CenterFilename>)
	ON_BN_CLICKED(	IDC_BUTTON_FILENAME_INDEX2,		OnButton<	AipCtr::Lve::FilenameIndex2>)
	ON_EN_KILLFOCUS(IDC_EDIT_FILENAME_INDEX2,		OnKillFocus<AipCtr::Lve::FilenameIndex2>)

	ON_BN_CLICKED(IDC_USE_ALTERNATIVE_IMAGE_PATH,	OnButtonUseAlternativeImagePath)
	ON_BN_CLICKED(IDC_BUTTON_MAX_QUEUE,				OnButtonMaxImageQueueLength)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_FILEPATHROOT1,	OnButtonImageFilepathroot1)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_FILEPATHROOT2,	OnButtonImageFilepathroot2)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_FILEPATHROOT3,	OnButtonImageFilepathroot3)
	ON_EN_KILLFOCUS(IDC_ARCHIVE_IMAGE_FILEPATHROOT1, OnKillFocusImageFilepathroot1)
	ON_EN_KILLFOCUS(IDC_ARCHIVE_IMAGE_FILEPATHROOT2, OnKillFocusImageFilepathroot2)
	ON_EN_KILLFOCUS(IDC_ARCHIVE_IMAGE_FILEPATHROOT3, OnKillFocusImageFilepathroot3)
	
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
, m_inspectionId(inspectionId)
, m_taskId(taskObjectId)
, m_ValueController{ SvOgu::BoundValues{ inspectionId, taskObjectId } }
, m_alternativeImagePaths(m_ValueController, inspectionId, taskObjectId)
{
	m_strCaption = m_psp.pszTitle;
	if( nullptr != m_pParent )
	{
		m_pTool = dynamic_cast <SvTo::SVArchiveTool*> (m_pParent->GetTaskObject());
		if (nullptr == m_pTool)
		{
			SvStl::MessageManager e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_5008_NULLTOOL, SvStl::Tid_ToolInvalid, SvStl::SourceFileParams(StdMessageParams));
			e.Throw();
		}
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
		int iSize = static_cast<int> (m_ImagesToBeArchived.size());
		if( 0 < iSize )
		{
			//if memory usage < 0 do not all them to exit
			__int64 FreeMemory = recalculateRemainingImageMemory();
			if (0 > FreeMemory)
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_AP_NotEnoughMemoryPleaseDeselect, SvStl::SourceFileParams(StdMessageParams) );
				return false;
			}
		}
	}

	if (!validateArchiveImageFilepath())
	{
		return false; //do not exit with an invalid path
	}

	int iCurSel = m_WhenToArchive.GetCurSel();
	m_eSelectedArchiveMethod = static_cast<SvTo::SVArchiveMethodEnum> (m_WhenToArchive.GetItemData(iCurSel));
	m_ValueController.Set<long>(SvPb::ArchiveMethodEId, static_cast<long> (m_eSelectedArchiveMethod));
	
	CString Temp;
	m_EditMaxImages.GetWindowText(Temp);
	m_ImagesToArchive = atol(Temp);
	if( UpperLimitImageNumbers < m_ImagesToArchive)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%ld"), m_ImagesToArchive));
		msgList.push_back(SvUl::Format(_T("%ld"), UpperLimitImageNumbers));
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_Error_you_have_Selected_X_Must_less_then_Y, msgList, SvStl::SourceFileParams(StdMessageParams));
		return true;
	}

	if( MaxImagesWarningLimit < m_ImagesToArchive)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%ld"),  m_ImagesToArchive));
		SvStl::MessageManager Exception(SvStl::MsgType::Log );
		Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_WarningMaxImages, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}

	//max images must be at least 1
	if( 1L > m_ImagesToArchive)
	{
		m_ImagesToArchive= 1L;
	}

	updateAndCommitImageValues();

	if (nullptr != m_pParent)
	{
		m_pParent->markDocumentAsDirty();
	}

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
	DDX_Control(pDX, IDC_STATIC_MAX_QUEUE, m_maximumImageQueueLengthStaticText);
	DDX_Control(pDX, IDC_EDIT_MAX_QUEUE, m_maximumImageQueueLengthEditBox);
	DDX_Control(pDX, IDC_BUTTON_MAX_QUEUE, m_maximumImageQueueLengthButton);
	DDX_Control(pDX, IDC_WHEN_TO_ARCHIVE, m_WhenToArchive);
	DDX_Control(pDX, IDC_EDIT_MAX_IMAGES, m_EditMaxImages);
	DDX_Control(pDX, IDC_ARCHIVE_IMAGE_FILEPATHROOT1, m_ImageFilepathroot1);
	DDX_Control(pDX, IDC_BUTTON_IMAGE_FILEPATHROOT1, m_ImageFilepathroot1Button);
	DDX_Control(pDX, IDC_ARCHIVE_IMAGE_FILEPATHROOT2, m_ImageFilepathroot2);
	DDX_Control(pDX, IDC_BUTTON_IMAGE_FILEPATHROOT2, m_ImageFilepathroot2Button);
	DDX_Control(pDX, IDC_ARCHIVE_IMAGE_FILEPATHROOT3, m_ImageFilepathroot3);
	DDX_Control(pDX, IDC_BUTTON_IMAGE_FILEPATHROOT3, m_ImageFilepathroot3Button);
	DDX_Control(pDX, IDC_USE_ALTERNATIVE_IMAGE_PATH, m_useAlternativeImagePathButton);
	DDX_Control(pDX, IDC_CHECK_STOP_AT_MAX, m_StopAtMaxImagesButton);

	DDX_CBIndex(pDX, IDC_WHEN_TO_ARCHIVE, m_WhenToArchiveIndex);

	BOOL stopAtMaxIsMandatory = m_useAlternativeImagePathButton.GetCheck() && (SvTo::SVArchiveGoOffline == m_eSelectedArchiveMethod);

	m_StopAtMaxImagesButton.EnableWindow(!stopAtMaxIsMandatory);

	if (stopAtMaxIsMandatory)
	{
		m_StopAtMaxImagesButton.SetCheck(TRUE);
	}

	auto numberOfImages = m_ItemsSelected.GetItemCount();
	m_useAlternativeImagePathButton.EnableWindow(numberOfImages < 2);
	auto groupBoxText = (numberOfImages < 2 ? "Alternative Image Path" : "Alternative Image Path (only for single image)");
	GetDlgItem(IDC_ALTERNATIVE_IMAGE_PATH)->SetWindowText(groupBoxText);
	if (numberOfImages > 1)
	{
		m_useAlternativeImagePathButton.SetCheck(false);
	}

	m_EditMaxImages.EnableWindow((!m_useAlternativeImagePathButton.GetCheck() || m_StopAtMaxImagesButton.GetCheck()));
}


bool SVTADlgArchiveImagePage::validateArchiveImageFilepath()
{
	m_ValueController.Set<bool>(SvPb::UseAlternativeImagePathsEId, m_useAlternativeImagePathButton.GetCheck() ? true : false);
	m_ValueController.Commit(SvOgu::PostAction::doNothing); //changes need to be committed before paths can be built

	if (!m_pTool->updateCurrentImagePathRoot(nullptr, true))
	{
		return false; // the current image path root cannot even be determined
	}

	std::string imageDirectoryPath = m_pTool->archiveImageDirectory();

	try
	{
		if (pathCanProbablyBeCreatedOrExistsAlready(imageDirectoryPath))
		{
			return true;
		}
	}
	catch (...)
	{
	}

	SvDef::StringVector msgList;
	msgList.push_back(imageDirectoryPath);
	SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
	Exception.setMessage(SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_InvalidPath, msgList, SvStl::SourceFileParams(StdMessageParams));

	return false;
}

void SVTADlgArchiveImagePage::updateAndCommitImageValues()
{
	int iCurSel = m_WhenToArchive.GetCurSel();
	m_eSelectedArchiveMethod = static_cast<SvTo::SVArchiveMethodEnum> (m_WhenToArchive.GetItemData(iCurSel));
	m_ValueController.Set<long>(SvPb::ArchiveMethodEId, static_cast<long> (m_eSelectedArchiveMethod));
	m_ValueController.Set<DWORD>(SvPb::ArchiveMaxImagesCountEId, static_cast<DWORD> (m_ImagesToArchive));
	m_ValueController.Set<DWORD>(SvPb::ArchiveStopAtMaxImagesEId, m_StopAtMaxImagesButton.GetCheck() ? 1 : 0);

	m_pTool->setImageArchiveList(m_ImagesToBeArchived);

	m_ValueController.Commit(SvOgu::PostAction::doReset);
}

BOOL SVTADlgArchiveImagePage::OnInitDialog() 
{
	CWaitCursor Wait;
	m_Init = true;

	CPropertyPage::OnInitDialog();

	m_ValueController.Init();

	GetWindowText( m_strCaption );

	m_ItemsSelected.InsertColumn( ItemSelectedCol, _T(""), 0, ItemsSelectedWidth );
	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR24 | ILC_MASK, IconNumber, IconGrowBy );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_CHECKED_ENABLED ) );
	m_ItemsSelected.SetImageList( &m_StateImageList, LVSIL_STATE );

	m_TreeBitmap.LoadBitmap( IDB_TREE );
	m_Select.SetBitmap( static_cast<HBITMAP> (m_TreeBitmap.GetSafeHandle()) );
	
	m_maximumImageQueueLengthWidgetHelper = std::make_unique<SvOgu::LinkedValueWidgetHelper>(m_maximumImageQueueLengthEditBox, m_maximumImageQueueLengthButton, m_inspectionId, m_taskId, SvPb::MaximumImageQueueLengthEId, &m_ValueController);

	CDWordArray dwaIndex;
	
	for (auto const& rEntry : m_ValueController.GetEnumTypes(SvPb::ArchiveMethodEId))
	{
		int iIndex = m_WhenToArchive.AddString(rEntry.first.c_str());
		m_WhenToArchive.SetItemData( iIndex, static_cast<DWORD_PTR> (rEntry.second) );
		dwaIndex.SetAtGrow( rEntry.second, iIndex );
	}

	long lMode = m_ValueController.Get<long>(SvPb::ArchiveMethodEId);
	m_eSelectedArchiveMethod = static_cast<SvTo::SVArchiveMethodEnum>( lMode );
	m_WhenToArchiveIndex = dwaIndex.GetAt( lMode );

	m_ImagesToArchive = m_ValueController.Get<DWORD>(SvPb::ArchiveMaxImagesCountEId);
	std::string Temp = SvUl::Format(_T("%ld"), m_ImagesToArchive);
	m_EditMaxImages.SetWindowText( Temp.c_str() );

	// store the MaxImageNumber
	m_sMaxImageNumber = Temp.c_str(); 

	m_wndAvailableArchiveImageMemory.ShowWindow(SvTo::memoryNeedsToBeConsidered(m_eSelectedArchiveMethod));
	m_maximumImageQueueLengthStaticText.ShowWindow(SvTo::memoryNeedsToBeConsidered(m_eSelectedArchiveMethod));
	m_maximumImageQueueLengthEditBox.ShowWindow(SvTo::memoryNeedsToBeConsidered(m_eSelectedArchiveMethod));
	m_maximumImageQueueLengthButton.ShowWindow(SvTo::memoryNeedsToBeConsidered(m_eSelectedArchiveMethod));

	m_ImageFilepathrootWidgetHelpers[0] = std::make_unique<SvOgu::LinkedValueWidgetHelper>(m_ImageFilepathroot1, m_ImageFilepathroot1Button, m_inspectionId, m_taskId, SvPb::ArchiveImageFileRootPart1EId, &m_ValueController);
	m_ImageFilepathrootWidgetHelpers[1] = std::make_unique<SvOgu::LinkedValueWidgetHelper>(m_ImageFilepathroot2, m_ImageFilepathroot2Button, m_inspectionId, m_taskId, SvPb::ArchiveImageFileRootPart2EId, &m_ValueController);
	m_ImageFilepathrootWidgetHelpers[2] = std::make_unique<SvOgu::LinkedValueWidgetHelper>(m_ImageFilepathroot3, m_ImageFilepathroot3Button, m_inspectionId, m_taskId, SvPb::ArchiveImageFileRootPart3EId, &m_ValueController);
	m_alternativeImagePaths.init();

	m_StopAtMaxImagesButton.SetCheck(m_ValueController.Get<bool>(SvPb::ArchiveStopAtMaxImagesEId));
	m_useAlternativeImagePathButton.SetCheck(m_ValueController.Get<bool>(SvPb::UseAlternativeImagePathsEId));
	
	SVObjectReferenceVector imageVector{m_pTool->getImageArchiveList()};
	m_ImagesToBeArchived.swap(imageVector);

	m_Init = false;
	UpdateData(FALSE);

	ReadSelectedObjects();

	OnButtonUseAlternativeImagePath();

	return TRUE;
}

void SVTADlgArchiveImagePage::OnDblClickListSelected(NMHDR*, LRESULT*)
{
	ShowObjectSelector();
}

void SVTADlgArchiveImagePage::OnSelectObjects()
{
	ShowObjectSelector();
}

void SVTADlgArchiveImagePage::OnRemoveAllItems()
{
	m_ImagesToBeArchived.clear();
	ReadSelectedObjects();
	UpdateData(FALSE);

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
		SVObjectReferenceVector::const_iterator SelectedIter( m_ImagesToBeArchived.begin() + *Iter );
		m_ImagesToBeArchived.erase( SelectedIter );
	}

	ReadSelectedObjects();

	UpdateData(FALSE);
}

void SVTADlgArchiveImagePage::ReadSelectedObjects()
{
	m_ItemsSelected.DeleteAllItems();

	std::string Prefix = SvUl::Format( _T("%s.%s."), m_pTool->GetInspection()->GetName(), SvUl::LoadedStrings::g_ToolSetName.c_str() );

	int Index = 0;

	if (m_useAlternativeImagePathButton.GetCheck() && m_ImagesToBeArchived.size() > 1)
	{
		// keep only the last element of m_ImagesToBeArchived
		m_ImagesToBeArchived.erase(m_ImagesToBeArchived.begin(), --m_ImagesToBeArchived.end());
		// the others would not be archived anyway since an alternative image path has been configured
	}


	for (auto const& rEntry : m_ImagesToBeArchived)
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

	recalculateRemainingImageMemory();
}

void SVTADlgArchiveImagePage::ShowObjectSelector()
{
	uint32_t inspectionId(m_pTool->GetInspection()->getObjectId());

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::SearchArea::toolsetItems}, inspectionId, SvPb::archivableImage, SvDef::InvalidObjectId, false, SvPb::allImageObjects, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed, m_pTool->getObjectId());
	SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject, IDD + SvOr::HELPFILE_DLG_IDD_OFFSET);
	if (responseCmd.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
	}

	SvDef::StringSet CheckItems;
	for (auto const& rEntry : m_ImagesToBeArchived)
	{
		CheckItems.insert(rEntry.GetObjectNameToObjectType(SvPb::SVToolSetObjectType, true));
	}
	SvOsl::ObjectTreeGenerator::Instance().setCheckItems( CheckItems );

	std::string Title = SvUl::Format( _T("%s - %s"), m_strCaption.GetString(), m_pTool->GetInspection()->GetName());
	std::string Filter = SvUl::LoadStdString( IDS_FILTER );
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), m_strCaption.GetString(), Filter.c_str(), this );

	if( IDOK == Result )
	{
		const SvDef::StringVector& rSelectedList = SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects();

		m_ImagesToBeArchived.clear();
		for (auto const& rEntry : rSelectedList)
		{
			if (m_useAlternativeImagePathButton.GetCheck() && m_ImagesToBeArchived.size() > 0)
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Display); 
				Exception.setMessage(SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_OnlyOneArchiveImageWhenAlternativeImagePath, SvStl::SourceFileParams(StdMessageParams));
				break;
			}
			SVObjectReference ObjectRef(rEntry);
			m_ImagesToBeArchived.push_back(ObjectRef);
		}

		ReadSelectedObjects();
		UpdateData(FALSE);
	}

	recalculateRemainingImageMemory();
}


void SVTADlgArchiveImagePage::OnStopAtMaxClicked()
{
	UpdateData(TRUE);
}


void SVTADlgArchiveImagePage::OnBrowseImageFilepathroot1()
{
	SVFileNameClass	svfncImageFolder;

	CString firstPartOfImageArchivePathRoot;
	m_ImageFilepathroot1.GetWindowText(firstPartOfImageArchivePathRoot);

	if (!firstPartOfImageArchivePathRoot.IsEmpty())
	{
		svfncImageFolder.SetPathName(firstPartOfImageArchivePathRoot);
	}
	else
	{
		// Set the default folder to start with.
		CString csDefaultInitialPath(SvStl::GlobalPath::Inst().GetTempPath(_T("\\")).c_str());
		svfncImageFolder.SetPathName(csDefaultInitialPath);
	}

	//
	// Select the folder to copy to..
	//
	svfncImageFolder.SetFileType(SV_DEFAULT_FILE_TYPE);
	if (svfncImageFolder.SelectPath())
	{
		firstPartOfImageArchivePathRoot = svfncImageFolder.GetPathName().c_str();
		int len = firstPartOfImageArchivePathRoot.GetLength();
		if (len)
		{
			if (firstPartOfImageArchivePathRoot.GetAt(len - 1) != _T('\\'))
			{
				firstPartOfImageArchivePathRoot += _T('\\');
			}
		}
	}

	auto data = m_ValueController.Get<SvOgu::LinkedValueData>(SvPb::ArchiveImageFileRootPart1EId);
	data.m_selectedOption = SvPb::DirectValue;
	data.m_directValue = firstPartOfImageArchivePathRoot;
	data.m_Value = data.m_directValue;
	m_ValueController.Set<SvOgu::LinkedValueData>(SvPb::ArchiveImageFileRootPart1EId, data);
	m_ImageFilepathrootWidgetHelpers[0]->update();
}

void SVTADlgArchiveImagePage::OnSelchangeWhenToArchive() 
{
	UpdateData();

	int iSel = m_WhenToArchive.GetCurSel();

	if (iSel != CB_ERR)
	{
		m_eSelectedArchiveMethod = static_cast <SvTo::SVArchiveMethodEnum> (m_WhenToArchive.GetItemData( iSel ));
		m_wndAvailableArchiveImageMemory.ShowWindow(SvTo::memoryNeedsToBeConsidered(m_eSelectedArchiveMethod));
		m_maximumImageQueueLengthStaticText.ShowWindow(SvTo::SVArchiveAsynchronous == m_eSelectedArchiveMethod);
		m_maximumImageQueueLengthEditBox.ShowWindow(SvTo::SVArchiveAsynchronous == m_eSelectedArchiveMethod);
		m_maximumImageQueueLengthButton.ShowWindow(SvTo::SVArchiveAsynchronous == m_eSelectedArchiveMethod);

		if(SvTo::SVArchiveSynchronous == m_eSelectedArchiveMethod )
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Exception.setMessage( SVMSG_SVO_89_ARCHIVE_TOOL_SYNCHRONOUS_MODE, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		}
	}

	recalculateRemainingImageMemory();

	UpdateData();
}


void SVTADlgArchiveImagePage::OnDetails()
{
	auto poolInfo = SVMemoryManager::Instance().poolInfo();
	std::string memoryInformation = std::accumulate(poolInfo.begin(), poolInfo.end(), std::string(), [](const auto& a, const auto& b){return a + b; });
	AfxMessageBox(memoryInformation.c_str());
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
			if (SvTo::memoryNeedsToBeConsidered(m_eSelectedArchiveMethod))
			{
				__int64 llFreeMem = recalculateRemainingImageMemory();

				if( 0 <= llFreeMem )
				{
					m_sMaxImageNumber = strNumImages;
					m_ImagesToArchive = atol(strNumImages);
				}
				else
				{
					SvDef::StringVector msgList;
					msgList.push_back(std::string(strNumImages));
					SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
					Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_AP_NotEnoughImageMemoryWhenChangingMode, msgList, SvStl::SourceFileParams(StdMessageParams) );

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


__int64 SVTADlgArchiveImagePage::recalculateRemainingImageMemory()
{
	UpdateData(true);
	updateAndCommitImageValues(); 

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_resetobjectrequest();
	pRequest->set_objectid(m_taskId);
	SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	__int64 FreeMem = SVMemoryManager::Instance().remainingMemoryInBytes();

	LPCSTR formatText = (FreeMem < 0 ? SvO::AvailableArchiveImageMemoryExceeded : SvO::AvailableArchiveImageMemory);

	double megabytes = (double)FreeMem / (double)(SvDef::cBytesPerMegabyte);

	std::string availableMemory = SvUl::Format(formatText, fabs(megabytes));
	m_wndAvailableArchiveImageMemory.SetWindowText(availableMemory.c_str());

	return FreeMem;
}

afx_msg void SVTADlgArchiveImagePage::OnButtonUseAlternativeImagePath()
{
	UpdateData(TRUE);
	
	m_alternativeImagePaths.OnButtonUseAlternativeImagePath(m_useAlternativeImagePathButton.GetCheck());
}

afx_msg void SVTADlgArchiveImagePage::OnButtonMaxImageQueueLength()
{
	m_maximumImageQueueLengthWidgetHelper->OnButton();
}

void SVTADlgArchiveImagePage::OnChangeMaxImageQueueLength()
{
	if (nullptr != m_maximumImageQueueLengthWidgetHelper)
	{
		m_maximumImageQueueLengthWidgetHelper->EditboxToValue();
		recalculateRemainingImageMemory();
	}
}

void SVTADlgArchiveImagePage::OnButtonImageFilepathroot1()
{
	m_ImageFilepathrootWidgetHelpers[0]->OnButton();
}

void SVTADlgArchiveImagePage::OnButtonImageFilepathroot2()
{
	m_ImageFilepathrootWidgetHelpers[1]->OnButton();
}

void SVTADlgArchiveImagePage::OnButtonImageFilepathroot3()
{
	m_ImageFilepathrootWidgetHelpers[2]->OnButton(); 
}

void SVTADlgArchiveImagePage::OnKillFocusImageFilepathroot1()
{
	m_ImageFilepathrootWidgetHelpers[0]->EditboxToValue();
}

void SVTADlgArchiveImagePage::OnKillFocusImageFilepathroot2()
{
	m_ImageFilepathrootWidgetHelpers[1]->EditboxToValue();
}

void SVTADlgArchiveImagePage::OnKillFocusImageFilepathroot3()
{
	m_ImageFilepathrootWidgetHelpers[2]->EditboxToValue();
}

void SVTADlgArchiveImagePage::AipCtr::init()
{
	m_WidgetHelpers[Lve::BaseDirectoryname] = m_source.produce(m_EditBaseDirectoryname, m_ButtonBaseDirectoryname, SvPb::BaseDirectorynameEId);
	m_WidgetHelpers[Lve::DirectorynameIndex] = m_source.produce(m_EditDirectorynameIndex, m_ButtonDirectorynameIndex, SvPb::DirectorynameIndexEId);
	m_WidgetHelpers[Lve::SubfolderSelection] = m_source.produce(m_EditSubfolderSelection, m_ButtonSubfolderSelection, SvPb::SubfolderSelectionEId);
	m_WidgetHelpers[Lve::SubfolderLocation] = m_source.produce(m_EditSubfolderLocation, m_ButtonSubfolderLocation, SvPb::SubfolderLocationEId);
	m_WidgetHelpers[Lve::BaseFilename] = m_source.produce(m_EditBaseFilename, m_ButtonBaseFilename, SvPb::BaseFilenameEId);
	m_WidgetHelpers[Lve::FilenameIndex1] = m_source.produce(m_EditFilenameIndex1, m_ButtonFilenameIndex1, SvPb::FilenameIndex1EId);
	m_WidgetHelpers[Lve::CenterFilename] = m_source.produce(m_EditCenterFilename, m_ButtonCenterFilename, SvPb::CenterFilenameEId);
	m_WidgetHelpers[Lve::FilenameIndex2] = m_source.produce(m_EditFilenameIndex2, m_ButtonFilenameIndex2, SvPb::FilenameIndex2EId);
}

void SVTADlgArchiveImagePage::AipCtr::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_EDIT_DIRECTORYNAME_BASE, m_EditBaseDirectoryname);
	DDX_Control(pDX, IDC_BUTTON_DIRECTORYNAME_BASE, m_ButtonBaseDirectoryname);
	DDX_Control(pDX, IDC_EDIT_DIRECTORYNAME_INDEX, m_EditDirectorynameIndex);
	DDX_Control(pDX, IDC_BUTTON_DIRECTORYNAME_INDEX, m_ButtonDirectorynameIndex);
	DDX_Control(pDX, IDC_EDIT_SUBFOLDER_SELECTION, m_EditSubfolderSelection);
	DDX_Control(pDX, IDC_BUTTON_SUBFOLDER_SELECTION, m_ButtonSubfolderSelection);
	DDX_Control(pDX, IDC_EDIT_SUBFOLDER_LOCATION, m_EditSubfolderLocation);
	DDX_Control(pDX, IDC_BUTTON_SUBFOLDER_LOCATION, m_ButtonSubfolderLocation);
	DDX_Control(pDX, IDC_EDIT_FILENAME_BASE, m_EditBaseFilename);
	DDX_Control(pDX, IDC_BUTTON_FILENAME_BASE, m_ButtonBaseFilename);
	DDX_Control(pDX, IDC_EDIT_FILENAME_INDEX1, m_EditFilenameIndex1);
	DDX_Control(pDX, IDC_BUTTON_FILENAME_INDEX1, m_ButtonFilenameIndex1);
	DDX_Control(pDX, IDC_EDIT_FILENAME_CENTER, m_EditCenterFilename);
	DDX_Control(pDX, IDC_BUTTON_FILENAME_CENTER, m_ButtonCenterFilename);
	DDX_Control(pDX, IDC_EDIT_FILENAME_INDEX2, m_EditFilenameIndex2);
	DDX_Control(pDX, IDC_BUTTON_FILENAME_INDEX2, m_ButtonFilenameIndex2);
}


void SVTADlgArchiveImagePage::AipCtr::OnButton(Lve widgetEnum)
{
	if (widgetEnum < m_WidgetHelpers.size() && m_WidgetHelpers[widgetEnum])
	{
		m_WidgetHelpers[widgetEnum]->OnButton();
	}
}

void SVTADlgArchiveImagePage::AipCtr::OnKillFocus(Lve widgetEnum)
{
	if (widgetEnum < m_WidgetHelpers.size() && m_WidgetHelpers[widgetEnum])
	{
		m_WidgetHelpers[widgetEnum]->EditboxToValue();
	}
}

afx_msg void SVTADlgArchiveImagePage::AipCtr::OnButtonUseAlternativeImagePath(BOOL enable) 
{
	for (auto& pWidget : m_WidgetHelpers)
	{
		if (pWidget)
		{
			pWidget->EnableGuiElements(enable);
		}
	}
}
