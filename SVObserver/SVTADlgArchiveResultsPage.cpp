//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgArchiveResultsPage
//* .File Name       : $Workfile:   SVTADlgArchiveResultsPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.15  $
//* .Check In Date   : $Date:   10 Sep 2014 09:36:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <numeric>
#include "SVTADlgArchiveResultsPage.h"
#include "SVArchiveHeaderEditDlg.h"
#include "SVInspectionProcess.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVFileSystemLibrary/FilepathUtilities.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
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

BEGIN_MESSAGE_MAP(SVTADlgArchiveResultsPage, CPropertyPage)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SELECTED, OnDblClickListSelected)
	ON_BN_CLICKED(IDC_SELECT_BUTTON, OnSelectObjects)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnRemoveItem)
	ON_BN_CLICKED(IDC_BTN_CLEAR_ALL, OnRemoveAllItems)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_FORMAT_RESULTS, OnFormatResults)
	ON_BN_CLICKED(IDC_HEADER_BTN, &SVTADlgArchiveResultsPage::OnBnClickedHeaderBtn)
	ON_BN_CLICKED(IDC_HEADER_CHECK, &SVTADlgArchiveResultsPage::OnBnClickedHeaderCheck)

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
SVTADlgArchiveResultsPage::SVTADlgArchiveResultsPage(uint32_t inspectionId, uint32_t taskObjectId, SVToolAdjustmentDialogSheetClass* Parent)
: CPropertyPage(SVTADlgArchiveResultsPage::IDD)
, m_pParent(Parent)
, m_ColumnHeaders(false)
, m_AppendArchive(false)
, m_FormatResults(false)
, m_inspectionId(inspectionId)
, m_taskId(taskObjectId)
, m_ValueController {SvOg::BoundValues{ inspectionId, taskObjectId }}
, m_TotalWidth(8)
, m_Decimals(2)
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

SVTADlgArchiveResultsPage::~SVTADlgArchiveResultsPage()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool SVTADlgArchiveResultsPage::QueryAllowExit() 
{
	UpdateData(TRUE); 

	m_ValueController.Commit(SvOg::PostAction::doReset);

	std::string ArchiveFilepath = m_pTool->GetUntranslatedFullResultFilepath();

	//check for valid drive for text archive
	SvTo::ArchiveToolHelper athArchivePathAndName;
	athArchivePathAndName.Init( ArchiveFilepath );

	SvStl::MessageTextEnum pathErrorDescriptionId = SvStl::Tid_Empty;

	if (athArchivePathAndName.isUsingKeywords())
	{
		if (athArchivePathAndName.areTokensValid())
		{
			std::string TmpArchiveFileName = athArchivePathAndName.TranslatePath( ArchiveFilepath );
			if (false == pathCanProbablyBeCreatedOrExistsAlready(TmpArchiveFileName.c_str()))
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
		if (false == pathCanProbablyBeCreatedOrExistsAlready(ArchiveFilepath))
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

	m_pTool->m_dwAppendArchiveFile.SetValue(static_cast<DWORD> (m_AppendArchive));
	m_pTool->m_bvoFormatResults.SetValue(m_FormatResults);
	m_pTool->m_dwArchiveResultsMinimumNumberOfCharacters.SetValue(m_TotalWidth);
	m_pTool->m_dwArchiveResultsNumberOfDecimals.SetValue(m_Decimals);

	m_pTool->refreshResultArchiveList(m_ResultsToBeArchived);

	// Add newly selected values to headers.
	BOOL bUseHeaders( false );
	HRESULT hr = m_pTool->m_bvoUseHeaders.GetValue( bUseHeaders );
	if( S_OK == hr && bUseHeaders )
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
void SVTADlgArchiveResultsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SELECTED, m_selectedResultsWidget);
	DDX_Control(pDX, IDC_SELECT_BUTTON, m_Select);
	DDX_Check(pDX, IDC_CHECK_APPEND, m_AppendArchive);
	DDX_Check(pDX, IDC_FORMAT_RESULTS, m_FormatResults);
	DDX_Text(pDX, IDC_TOTAL_WIDTH, m_TotalWidth);
	DDX_Text(pDX, IDC_DECIMAL, m_Decimals);
	DDX_Check(pDX, IDC_HEADER_CHECK, m_ColumnHeaders);                  
	DDX_Control(pDX, IDC_TOTAL_WIDTH, m_TotalWidthEdit);
	DDX_Control(pDX, IDC_DECIMAL, m_DecimalsEdit);
	DDX_Control(pDX, IDC_ARCHIVE_RESULT_FOLDERPATH1, m_resultFolderpathPart1Edit);
	DDX_Control(pDX, IDC_BUTTON_RESULT_FOLDERPATH1, m_resultFolderpathPart1Button);
	DDX_Control(pDX, IDC_ARCHIVE_RESULT_FOLDERPATH2, m_resultFolderpathPart2Edit);
	DDX_Control(pDX, IDC_BUTTON_RESULT_FOLDERPATH2, m_resultFolderpathPart2Button);
	DDX_Control(pDX, IDC_ARCHIVE_RESULT_FILENAME, m_ResultFilenameEdit);
	DDX_Control(pDX, IDC_BUTTON_RESULT_FILENAME, m_ResultFilenameButton);
	m_DecimalsEdit.EnableWindow(m_FormatResults);
	m_TotalWidthEdit.EnableWindow(m_FormatResults);
}


BOOL SVTADlgArchiveResultsPage::OnInitDialog() 
{
	CWaitCursor wait;

	CPropertyPage::OnInitDialog();

	m_ValueController.Init();

	GetWindowText( m_strCaption );

	m_selectedResultsWidget.InsertColumn( ItemSelectedCol, _T(""), 0, ItemsSelectedWidth );
	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR24 | ILC_MASK, IconNumber, IconGrowBy );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_CHECKED_ENABLED ) );
	m_selectedResultsWidget.SetImageList( &m_StateImageList, LVSIL_STATE );

	ReadSelectedObjects();//@TODO [Arvid][10.00][17.6.2020] is this call really necessary? ReadSelectedObjects() will be called again later anyway.

	m_TreeBitmap.LoadBitmap( IDB_TREE );
	m_Select.SetBitmap( static_cast<HBITMAP> (m_TreeBitmap.GetSafeHandle()) );

	CDWordArray dwaIndex;
	
	DWORD dwTemp = 0;
	m_pTool->m_dwAppendArchiveFile.GetValue(dwTemp);
	m_AppendArchive = (int)dwTemp;

	dwTemp = 0;
	m_pTool->m_bvoFormatResults.GetValue(dwTemp);
	m_FormatResults = (BOOL)dwTemp;

	m_ResultFolderpathWidgetHelpers[0] = std::make_unique<SvOg::LinkedValueWidgetHelper>(m_resultFolderpathPart1Edit, m_resultFolderpathPart1Button, m_inspectionId, m_taskId, SvPb::ArchiveResultFolderpathPart1EId, &m_ValueController);
	m_ResultFolderpathWidgetHelpers[1] = std::make_unique<SvOg::LinkedValueWidgetHelper>(m_resultFolderpathPart2Edit, m_resultFolderpathPart2Button, m_inspectionId, m_taskId, SvPb::ArchiveResultFolderpathPart2EId, &m_ValueController);
	m_ResultFolderpathWidgetHelpers[2] = std::make_unique<SvOg::LinkedValueWidgetHelper>(m_ResultFilenameEdit, m_ResultFilenameButton, m_inspectionId, m_taskId, SvPb::ArchiveResultFilenameEId, &m_ValueController);

	m_pTool->m_dwArchiveResultsMinimumNumberOfCharacters.GetValue(m_TotalWidth);

	m_pTool->m_dwArchiveResultsNumberOfDecimals.GetValue(m_Decimals);

	m_pTool->m_bvoUseHeaders.GetValue(m_ColumnHeaders);
	GetDlgItem(IDC_HEADER_BTN)->EnableWindow(m_ColumnHeaders);

	SVObjectReferenceVector resultVector = m_pTool->assembleResultReferenceVector();
	m_ResultsToBeArchived.swap(resultVector);

	ReadSelectedObjects();
	UpdateData(FALSE);
	return TRUE;
}

void SVTADlgArchiveResultsPage::OnDblClickListSelected(NMHDR*, LRESULT*)
{
	ShowObjectSelector();
}

void SVTADlgArchiveResultsPage::OnSelectObjects()
{
	ShowObjectSelector();
}

void SVTADlgArchiveResultsPage::OnRemoveAllItems()
{
	m_ResultsToBeArchived.clear();
	ReadSelectedObjects();
}

void SVTADlgArchiveResultsPage::OnRemoveItem()
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

void SVTADlgArchiveResultsPage::ReadSelectedObjects()
{
	m_selectedResultsWidget.DeleteAllItems();

	std::string toolsetPrefix = m_pTool->GetInspection()->GetName();
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

void SVTADlgArchiveResultsPage::ShowObjectSelector()
{
	uint32_t InspectionId(m_pTool->GetInspection()->getObjectId());

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{ SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::cameraObject, SvPb::SearchArea::toolsetItems }, InspectionId, SvPb::archivable, 
		SvDef::InvalidObjectId, false, SvPb::allValueObjects, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed, m_pTool->getObjectId());

	SvCmd::InspectionCommands(InspectionId, requestCmd, &responseCmd);
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

	std::string Title = SvUl::Format( _T("%s - %s"), m_strCaption.GetString(), m_pTool->GetInspection()->GetName() );
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

void SVTADlgArchiveResultsPage::OnBrowse() 
{
	SVFileNameClass	svfncArchiveFileName;

	//get current path
	CString Text;
	m_resultFolderpathPart1Edit.GetWindowText( Text );
	std::string ArchiveFullName = Text.GetString();

	SVCheckPathDir( ArchiveFullName.c_str(), TRUE );

	svfncArchiveFileName.SetFileType(SV_DEFAULT_FILE_TYPE);
	svfncArchiveFileName.SetDefaultFullFileName( ArchiveFullName.c_str() );
	if (svfncArchiveFileName.SelectFile())
	{
		m_resultFolderpathPart1Edit.SetWindowText(svfncArchiveFileName.GetFullFileName().c_str()); 
	}
}

void SVTADlgArchiveResultsPage::OnFormatResults()
{
	UpdateData(TRUE);
}

SvDef::StringPairVector SVTADlgArchiveResultsPage::GetSelectedHeaderNamePairs()
{
	// Inputs:
	// Collect and build string pair vector from header id and Header Label from the archive tool.
	// but only add if ids match the selected object ids
	// output a vector of Object Name / Label pairs filtered by the selected objects..

	// Get Lists....
	SvDef::StringVector headerLabels;
	SvDef::StringVector objectrefIdentifiers;
	m_pTool->m_HeaderLabelNames.GetArrayValues( headerLabels );
	m_pTool->m_HeaderObjectIDs.GetArrayValues( objectrefIdentifiers );

	std::map<std::string, std::string> headerLabelByObjectRefIdentifier;

	// Create an associative array for finding header labels by objectRefIdentifier
	for( SvDef::StringVector::const_iterator it = objectrefIdentifiers.begin(),it1 = headerLabels.begin() ; it != objectrefIdentifiers.end() ;++it1, ++it)
	{
		headerLabelByObjectRefIdentifier[*it] = *it1;
	}

	std::string toolsetPrefix = m_pTool->GetInspection()->GetName();
	toolsetPrefix += _T(".Tool Set.");

	SvDef::StringPairVector selectedHeaderPairs;
	for(auto const& rEntry : m_ResultsToBeArchived)
	{
		// we want to be able to refer to individual object elements (for arrays)
		// for scalar objects we obtain the same identification string as before
		auto objectRefIdentifier = rEntry.GetObjectIdAndIndexOneBased();
		auto completeName = rEntry.GetCompleteName(true);
		
		if (rEntry.getObject()->GetObjectSubType() == SvPb::SVTimerValueObjectType)
		{
			completeName = completeName + " (µs)";
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

void SVTADlgArchiveResultsPage::StoreHeaderValuesToTool(SvDef::StringPairVector& HeaderPairs)
{
	SvDef::StringVector headerLabels;
	SvDef::StringVector objectrefIdentifiers;
	for(SvDef::StringPairVector::iterator it = HeaderPairs.begin(); it != HeaderPairs.end() ;++it)
	{
		objectrefIdentifiers.push_back( it->first );
		headerLabels.push_back( it->second );
	}
	m_pTool->m_HeaderLabelNames.SetArraySize( static_cast<int>(HeaderPairs.size()) );
	m_pTool->m_HeaderObjectIDs.SetArraySize( static_cast<int>(HeaderPairs.size()) );
	m_pTool->m_HeaderLabelNames.SetArrayValues(headerLabels);
	m_pTool->m_HeaderObjectIDs.SetArrayValues( objectrefIdentifiers);
}

void SVTADlgArchiveResultsPage::OnBnClickedHeaderBtn()
{
	// Get a collection of header name/Label pairs from this class and the tool.
	
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

void SVTADlgArchiveResultsPage::OnBnClickedHeaderCheck()
{
	UpdateData( TRUE );

	BOOL bEnable = 0 != m_ResultsToBeArchived.size() && m_ColumnHeaders;
	GetDlgItem(IDC_HEADER_BTN)->EnableWindow(bEnable);

	m_pTool->m_bvoUseHeaders.SetValue(m_ColumnHeaders);
}


void SVTADlgArchiveResultsPage::OnButtonResultFolderpathPart1()
{
	m_ResultFolderpathWidgetHelpers[0]->OnButton();
}

void SVTADlgArchiveResultsPage::OnKillFocusResultFolderpathPart1()
{
	m_ResultFolderpathWidgetHelpers[0]->EditboxToValue();
}

void SVTADlgArchiveResultsPage::OnButtonResultFolderpathPart2()
{
	m_ResultFolderpathWidgetHelpers[1]->OnButton();
}

void SVTADlgArchiveResultsPage::OnKillFocusResultFolderpathPart2()
{
	m_ResultFolderpathWidgetHelpers[1]->EditboxToValue();
}


void SVTADlgArchiveResultsPage::OnButtonResultFilename()
{
	m_ResultFolderpathWidgetHelpers[2]->OnButton();
}

void SVTADlgArchiveResultsPage::OnKillFocusResultFilename()
{
	m_ResultFolderpathWidgetHelpers[2]->EditboxToValue();
}


#pragma endregion Private Methods


