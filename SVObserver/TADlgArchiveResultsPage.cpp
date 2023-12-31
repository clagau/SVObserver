//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgArchiveResultsPage
//* .File Name       : $Workfile:   TADlgArchiveResultsPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.15  $
//* .Check In Date   : $Date:   10 Sep 2014 09:36:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <numeric>
#include "TADlgArchiveResultsPage.h"
#include "SVArchiveHeaderEditDlg.h"
#include "SVInspectionProcess.h"
#include "TADlgSheetClass.h"
#include "Definitions/StringTypeDef.h"
#include "SVOGuiUtility/GuiValueHelper.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "FilesystemUtilities/FilepathUtilities.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilitylibrary/Safearrayhelper.h"
#include "Tools/SVArchiveTool.h"
#include "Tools/ArchiveToolHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(TADlgArchiveResultsPage, CPropertyPage)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SELECTED, OnDblClickListSelected)
	ON_BN_CLICKED(IDC_SELECT_BUTTON, OnSelectObjects)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnRemoveItem)
	ON_BN_CLICKED(IDC_BTN_CLEAR_ALL, OnRemoveAllItems)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_FORMAT_RESULTS, OnFormatResults)
	ON_BN_CLICKED(IDC_HEADER_BTN, &TADlgArchiveResultsPage::OnBnClickedHeaderBtn)
	ON_BN_CLICKED(IDC_USE_HEADERS, &TADlgArchiveResultsPage::OnBnClickedHeaderCheck)

	ON_BN_CLICKED(IDC_BUTTON_RESULT_FOLDERPATH1, OnButtonResultFolderpathPart1)
	ON_EN_KILLFOCUS(IDC_ARCHIVE_RESULT_FOLDERPATH1, OnKillFocusResultFolderpathPart1)
	ON_BN_CLICKED(IDC_BUTTON_RESULT_FOLDERPATH2, OnButtonResultFolderpathPart2)
	ON_EN_KILLFOCUS(IDC_ARCHIVE_RESULT_FOLDERPATH2, OnKillFocusResultFolderpathPart2)
	ON_BN_CLICKED(IDC_BUTTON_RESULT_FILENAME, OnButtonResultFilename)
	ON_EN_KILLFOCUS(IDC_ARCHIVE_RESULT_FILENAME, OnKillFocusResultFilename)

END_MESSAGE_MAP()

constexpr int ItemSelectedCol = 1;
constexpr int ItemsSelectedWidth = 500;
constexpr int IconNumber = 1;
constexpr int IconGrowBy = 2;
#pragma endregion Declarations

#pragma region Constructor
TADlgArchiveResultsPage::TADlgArchiveResultsPage(uint32_t inspectionId, uint32_t taskObjectId, TADlgSheetClass* Parent)
: CPropertyPage(TADlgArchiveResultsPage::IDD)
, m_pParent(Parent)
, m_inspectionId(inspectionId)
, m_taskId(taskObjectId)
, m_ValueController {SvOgu::BoundValues{ inspectionId, taskObjectId }}
{
	m_strCaption = m_psp.pszTitle;

	if (nullptr != m_pParent)
	{
		m_pArchiveTool = dynamic_cast <SvTo::SVArchiveTool*> (m_pParent->GetTaskObject());

		if (nullptr != m_pArchiveTool)
		{
			auto [ok, name] = SvCmd::getInspectionName(inspectionId);

			if (ok)
			{
				m_inspectionName = name;
				return;
			}
		}
	}

	SvStl::MessageManager e(SvStl::MsgType::Data);
	e.setMessage(SVMSG_SVO_5008_NULLTOOL, SvStl::Tid_ToolInvalid, SvStl::SourceFileParams(StdMessageParams));
	e.Throw();
}
#pragma endregion Constructor


#pragma region Public Methods
bool TADlgArchiveResultsPage::QueryAllowExit() 
{
	UpdateData(TRUE); 

	m_ValueController.Commit(SvOgu::PostAction::doReset);
	m_ValueController.Init();

	// Check the file path to the archive file for associated archive tool.

	//@TODO [Arvid][10.40][27.6.2023] should be: std::string ArchiveFilepath =m_ValueController.Get<_variant_t>(SvPb::ArchiveResultFilepathEId); (but see below)
	std::string ArchiveFilepath = SvUl::createStdString(m_ValueController.Get<SvOgu::LinkedValueData>(SvPb::ArchiveResultFilepathEId).m_Value); //@TODO [Arvid][10.40][27.6.2023] ArchiveResultFilepathEId should refer to a SVStringValueObjectClass. But if does, Tid_ObjectBuilder_SetObjectValueError is displayed when loading a existing configuration containing an Archive Tool
	

	//check for valid drive for text archive
	SvTo::ArchiveToolHelper athArchivePathAndName;
	athArchivePathAndName.Init( ArchiveFilepath );

	SvStl::MessageTextEnum pathErrorDescriptionId = SvStl::Tid_Empty;

	if (athArchivePathAndName.isUsingKeywords())
	{
		if (athArchivePathAndName.areTokensValid())
		{
			std::string TmpArchiveFileName = athArchivePathAndName.TranslatePath( ArchiveFilepath );
			if (false == SvFs::pathCanProbablyBeCreatedOrExistsAlready(TmpArchiveFileName))
			{
				pathErrorDescriptionId = SvStl::Tid_InvalidKeywordsInPath;
			}
		}
		else
		{
			pathErrorDescriptionId = SvStl::Tid_InvalidPath;
		}
	}
	else
	{	//not using Keywords 
		if (false == SvFs::pathCanProbablyBeCreatedOrExistsAlready(ArchiveFilepath))
		{
			pathErrorDescriptionId = SvStl::Tid_InvalidPath;
		}
	}

	if(SvStl::Tid_Empty != pathErrorDescriptionId)
	{
		// do not allow exiting with invalid path
		SvDef::StringVector msgList;
		msgList.push_back(ArchiveFilepath);
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_73_ARCHIVE_MEMORY, pathErrorDescriptionId, msgList, SvStl::SourceFileParams(StdMessageParams));
		return false;
	}

	m_ValueController.Set<DWORD>(SvPb::ArchiveAppendArchiveFileEId, m_appendArchive);
	m_ValueController.Set<BOOL>(SvPb::FormatArchiveToolResultsEId, m_formatResults);
	m_ValueController.Set<DWORD>(SvPb::ArchiveResultsMinimumNumberOfCharactersEId, m_minimumNumberOfCharacters);
	m_ValueController.Set<DWORD>(SvPb::ArchiveResultsNumberOfDecimalsEId, m_numberOfDecimals);

	m_ValueController.Commit();

	m_pArchiveTool->refreshResultArchiveList(m_ResultsToBeArchived);

	// Add newly selected values to headers.
	if(m_ValueController.Get<DWORD>(SvPb::ArchiveUseColumnHeadersEId))
	{
		auto selectedHeaderNamePairs = GetSelectedHeaderNamePairs();
		StoreHeaderValuesToTool(selectedHeaderNamePairs);
	}

	if (nullptr != m_pParent)
	{
		m_pParent->markDocumentAsDirty();
	}

	return true;   // Everything is OK
}
#pragma endregion Public Methods

#pragma region Private Methods
void TADlgArchiveResultsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SELECTED, m_selectedResultsWidget);
	DDX_Control(pDX, IDC_SELECT_BUTTON, m_Select);
	DDX_Check(pDX, IDC_CHECK_APPEND, m_appendArchive);
	DDX_Check(pDX, IDC_FORMAT_RESULTS, m_formatResults);
	DDX_Text(pDX, IDC_MINIMUM_CHARACTERS, m_minimumNumberOfCharacters);
	DDX_Text(pDX, IDC_DECIMAL, m_numberOfDecimals);
	m_useColumnHeaders = (0 != m_useColumnHeaders) ? 1 : 0; //m_ValueController.Get<DWORD>(SvPb::ArchiveUseColumnHeadersEId) may have returned -1
	DDX_Check(pDX, IDC_USE_HEADERS, m_useColumnHeaders);
	DDX_Control(pDX, IDC_MINIMUM_CHARACTERS, m_minimumNumberOfCharactersEdit);
	DDX_Control(pDX, IDC_DECIMAL, m_numberOfDecimalsEdit);
	DDX_Control(pDX, IDC_ARCHIVE_RESULT_FOLDERPATH1, m_resultFolderpathPart1Edit);
	DDX_Control(pDX, IDC_BUTTON_RESULT_FOLDERPATH1, m_resultFolderpathPart1Button);
	DDX_Control(pDX, IDC_ARCHIVE_RESULT_FOLDERPATH2, m_resultFolderpathPart2Edit);
	DDX_Control(pDX, IDC_BUTTON_RESULT_FOLDERPATH2, m_resultFolderpathPart2Button);
	DDX_Control(pDX, IDC_ARCHIVE_RESULT_FILENAME, m_ResultFilenameEdit);
	DDX_Control(pDX, IDC_BUTTON_RESULT_FILENAME, m_ResultFilenameButton);
	m_numberOfDecimalsEdit.EnableWindow(m_formatResults);
	m_minimumNumberOfCharactersEdit.EnableWindow(m_formatResults);

	CWnd *pHeaderButton = GetDlgItem(IDC_HEADER_BTN);

	if (nullptr != pHeaderButton)
	{
		pHeaderButton->EnableWindow(0 != m_ResultsToBeArchived.size());
	}
}


BOOL TADlgArchiveResultsPage::OnInitDialog() 
{
	CWaitCursor wait;

	m_ValueController.Init();

	//these must be read from m_ValueController before CPropertyPage::OnInitDialog() is called
	m_appendArchive = m_ValueController.Get<BOOL>(SvPb::ArchiveAppendArchiveFileEId);
	m_formatResults = m_ValueController.Get<BOOL>(SvPb::FormatArchiveToolResultsEId);
	m_useColumnHeaders = m_ValueController.Get<DWORD>(SvPb::ArchiveUseColumnHeadersEId);

	CPropertyPage::OnInitDialog();

	GetWindowText( m_strCaption );

	m_selectedResultsWidget.InsertColumn( ItemSelectedCol, _T(""), 0, ItemsSelectedWidth );
	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR24 | ILC_MASK, IconNumber, IconGrowBy );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_CHECKED_ENABLED ) );
	m_selectedResultsWidget.SetImageList( &m_StateImageList, LVSIL_STATE );

	ReadSelectedObjects();//@TODO [Arvid][10.00][17.6.2020] is this call really necessary? ReadSelectedObjects() will be called again later anyway.

	m_TreeBitmap.LoadBitmap( IDB_TREE );
	m_Select.SetBitmap( static_cast<HBITMAP> (m_TreeBitmap.GetSafeHandle()) );

	m_ResultFolderpathWidgetHelpers[0] = std::make_unique<SvOgu::LinkedValueWidgetHelper>(m_resultFolderpathPart1Edit, m_resultFolderpathPart1Button, m_inspectionId, m_taskId, SvPb::ArchiveResultFolderpathPart1EId, &m_ValueController);
	m_ResultFolderpathWidgetHelpers[1] = std::make_unique<SvOgu::LinkedValueWidgetHelper>(m_resultFolderpathPart2Edit, m_resultFolderpathPart2Button, m_inspectionId, m_taskId, SvPb::ArchiveResultFolderpathPart2EId, &m_ValueController);
	m_ResultFolderpathWidgetHelpers[2] = std::make_unique<SvOgu::LinkedValueWidgetHelper>(m_ResultFilenameEdit, m_ResultFilenameButton, m_inspectionId, m_taskId, SvPb::ArchiveResultFilenameEId, &m_ValueController);

	m_minimumNumberOfCharacters = m_ValueController.Get<DWORD>(SvPb::ArchiveResultsMinimumNumberOfCharactersEId);
	m_numberOfDecimals = m_ValueController.Get<DWORD>(SvPb::ArchiveResultsNumberOfDecimalsEId);

	SVObjectReferenceVector resultVector = m_pArchiveTool->assembleResultReferenceVector();
	m_ResultsToBeArchived.swap(resultVector);

	ReadSelectedObjects();
	UpdateData(FALSE);
	return TRUE;
}

void TADlgArchiveResultsPage::OnDblClickListSelected(NMHDR*, LRESULT*)
{
	ShowObjectSelector();
}

void TADlgArchiveResultsPage::OnSelectObjects()
{
	ShowObjectSelector();
}

void TADlgArchiveResultsPage::OnRemoveAllItems()
{
	m_ResultsToBeArchived.clear();
	ReadSelectedObjects();
}

void TADlgArchiveResultsPage::OnRemoveItem()
{
	POSITION Pos = m_selectedResultsWidget.GetFirstSelectedItemPosition();
	std::vector<int> SelectedVector;

	while( nullptr != Pos )
	{
		int ItemIndex = m_selectedResultsWidget.GetNextSelectedItem( Pos );
		SelectedVector.push_back( ItemIndex );
	}

	//Remove in reverse order
	std::vector<int>::const_reverse_iterator Iter;
	for( Iter = SelectedVector.crbegin(); SelectedVector.crend() != Iter; ++Iter )
	{
		SVObjectReferenceVector::const_iterator SelectedIter( m_ResultsToBeArchived.begin() + *Iter );
		m_ResultsToBeArchived.erase( SelectedIter );
	}

	ReadSelectedObjects();
}

void TADlgArchiveResultsPage::ReadSelectedObjects()
{
	m_selectedResultsWidget.DeleteAllItems();

	std::string toolsetPrefix = m_inspectionName;
	toolsetPrefix += _T(".Tool Set.");

	int Index = 0;
	for (auto const& rEntry : m_ResultsToBeArchived)
	{
		std::string Name{ rEntry.GetCompleteName(true) };
		SvUl::searchAndReplace( Name, toolsetPrefix.c_str(), _T("") );

		m_selectedResultsWidget.InsertItem(LVIF_STATE | LVIF_TEXT,
			Index,
			Name.c_str(),
			INDEXTOSTATEIMAGEMASK(1),
			LVIS_STATEIMAGEMASK,
			1,
			0);
		Index++;
	}
}

void TADlgArchiveResultsPage::ShowObjectSelector()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{ SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::cameraObject, SvPb::SearchArea::toolsetItems }, m_inspectionId, SvPb::viewable, 
		SvDef::InvalidObjectId, false, SvPb::allValueObjects, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed, m_taskId);

	SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);
	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject, IDD + SvOr::HELPFILE_DLG_IDD_OFFSET);
	if (responseCmd.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
	}

	SvDef::StringSet CheckItems;
	for (auto const& rEntry : m_ResultsToBeArchived)
	{
		CheckItems.insert(rEntry.GetObjectNameToObjectType(SvPb::SVToolSetObjectType, true));
	}
	SvOsl::ObjectTreeGenerator::Instance().setCheckItems( CheckItems );

	std::string Title = std::format( _T("{} - {}"), m_strCaption.GetString(), m_inspectionName );
	std::string Filter = SvUl::LoadStdString( IDS_FILTER );
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), m_strCaption.GetString(), Filter.c_str(), this );

	if( IDOK == Result )
	{
		m_ResultsToBeArchived.clear();
		for (auto const& rEntry : SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects())
		{
			SVObjectReference ObjectRef(rEntry);
			m_ResultsToBeArchived.push_back(ObjectRef);
		}
		ReadSelectedObjects();
	}
}

void TADlgArchiveResultsPage::OnBrowse() 
{
	SvFs::FileHelper	folderpathPart1;

	//get current path
	CString Text;
	m_resultFolderpathPart1Edit.GetWindowText( Text );
	std::string firstPart = Text.GetString();

	SvFs::ensureDirectoryExists( firstPart.c_str(), TRUE );

	folderpathPart1.SetFileType(SvFs::FileType::defaultType);
	folderpathPart1.SetDefaultPathName(firstPart );
	if (folderpathPart1.SelectPath())
	{
		m_resultFolderpathPart1Edit.SetWindowText(folderpathPart1.GetPathName().c_str());
		m_ResultFolderpathWidgetHelpers[0]->EditboxToValue();
	}
}

void TADlgArchiveResultsPage::OnFormatResults()
{
	UpdateData(TRUE);
}


SvDef::StringPairVector TADlgArchiveResultsPage::GetSelectedHeaderNamePairs()
{
	// Inputs:
	// Collect and build string pair vector from header id and Header Label from the archive tool.
	// but only add if ids match the selected object ids
	// output a vector of Object Name / Label pairs filtered by the selected objects..

	// Get Lists....

	SvDef::StringVector headerLabels = m_ValueController.getStringVector(SvPb::ArchiveHeaderLabelEId);
	SvDef::StringVector objectrefIdentifiers = m_ValueController.getStringVector(SvPb::ArchiveHeaderEId); //@TODO [Arvid][10.40][27.6.2023] this is empty (and the corresponding variant not even of array type the first time (only). Why?)

	std::map<std::string, std::string> headerLabelByObjectRefIdentifier;

	// Create an associative array for finding header labels by objectRefIdentifier
	for( SvDef::StringVector::const_iterator it = objectrefIdentifiers.begin(), it1 = headerLabels.begin() ; it != objectrefIdentifiers.end() ;++it1, ++it)
	{
		headerLabelByObjectRefIdentifier[*it] = *it1;
	}

	std::string toolsetPrefix = m_inspectionName + _T(".Tool Set.");

	SvDef::StringPairVector selectedHeaderPairs;
	for(auto const& rEntry : m_ResultsToBeArchived)
	{
		// we want to be able to refer to individual object elements (for arrays)
		// for scalar objects we obtain the same identification string as before
		auto objectRefIdentifier = rEntry.GetObjectIdAndIndexOneBased();
		auto completeName = rEntry.GetCompleteName(true);
		
		if (rEntry.getObject()->GetObjectSubType() == SvPb::SVTimerValueObjectType)
		{
			completeName = completeName + " (�s)";
		}
		
		// and we want our header labels not too verbose.
		SvUl::searchAndReplace(completeName, toolsetPrefix.c_str(), _T(""));

		if (headerLabelByObjectRefIdentifier.find(objectRefIdentifier) != headerLabelByObjectRefIdentifier.end())
		{
			// i.e., ID and index (if any) match: a header for this object reference has already been defined
			completeName = headerLabelByObjectRefIdentifier[objectRefIdentifier];
		}
		selectedHeaderPairs.push_back(SvDef::StringPair(objectRefIdentifier, completeName));
	}

	return selectedHeaderPairs;
}


void TADlgArchiveResultsPage::StoreHeaderValuesToTool(SvDef::StringPairVector& HeaderPairs)
{
	SvDef::StringVector headerLabels;
	SvDef::StringVector objectrefIdentifiers;
	for(SvDef::StringPairVector::iterator it = HeaderPairs.begin(); it != HeaderPairs.end(); ++it)
	{
		objectrefIdentifiers.push_back( it->first );
		headerLabels.push_back( it->second );
	}

	m_ValueController.writeStringVector(headerLabels, SvPb::ArchiveHeaderLabelEId);
	m_ValueController.writeStringVector(objectrefIdentifiers, SvPb::ArchiveHeaderEId);
	m_ValueController.Commit();
}

void TADlgArchiveResultsPage::OnBnClickedHeaderBtn()
{
	// Get a collection of header name/label pairs from this class and the tool.
	
	SvDef::StringPairVector HeaderPairs = GetSelectedHeaderNamePairs(); // filters by what is selected.

	SVArchiveHeaderEditDlg dlg;
	dlg.SetValues( HeaderPairs );

	INT_PTR iRet = dlg.DoModal();
	if( iRet == IDOK)
	{
		dlg.GetValues( HeaderPairs );
		StoreHeaderValuesToTool( HeaderPairs );
	}
}

void TADlgArchiveResultsPage::OnBnClickedHeaderCheck()
{
	UpdateData( TRUE );

	m_ValueController.Set<DWORD>(SvPb::ArchiveUseColumnHeadersEId, m_useColumnHeaders);
	m_ValueController.Commit();
}


void TADlgArchiveResultsPage::OnButtonResultFolderpathPart1()
{
	m_ResultFolderpathWidgetHelpers[0]->OnButton();
}

void TADlgArchiveResultsPage::OnKillFocusResultFolderpathPart1()
{
	m_ResultFolderpathWidgetHelpers[0]->EditboxToValue();
}

void TADlgArchiveResultsPage::OnButtonResultFolderpathPart2()
{
	m_ResultFolderpathWidgetHelpers[1]->OnButton();
}

void TADlgArchiveResultsPage::OnKillFocusResultFolderpathPart2()
{
	m_ResultFolderpathWidgetHelpers[1]->EditboxToValue();
}


void TADlgArchiveResultsPage::OnButtonResultFilename()
{
	m_ResultFolderpathWidgetHelpers[2]->OnButton();
}

void TADlgArchiveResultsPage::OnKillFocusResultFilename()
{
	m_ResultFolderpathWidgetHelpers[2]->EditboxToValue();
}


#pragma endregion Private Methods


