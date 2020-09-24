//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVTADlgArchiveImagePage.h" //@TODO [Arvid][10.00][7.8.2020] should be removed
#include "SVArchiveHeaderEditDlg.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
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
END_MESSAGE_MAP()

constexpr int ItemSelectedCol = 1;
constexpr int ItemsSelectedWidth = 500;
constexpr int IconNumber = 1;
constexpr int IconGrowBy = 2;
#pragma endregion Declarations

#pragma region Constructor
SVTADlgArchiveResultsPage::SVTADlgArchiveResultsPage(uint32_t, uint32_t, SVToolAdjustmentDialogSheetClass* Parent, SVTADlgArchiveImagePage *pArchiveImagePage) //@TODO [Arvid][10.00][7.8.2020] Remove unnamed parameters
: CPropertyPage(SVTADlgArchiveResultsPage::IDD)
, m_pParent(Parent)
, m_pArchiveImagePage(pArchiveImagePage)
, m_pTool(nullptr)
, m_ColumnHeaders(false)
, m_AppendArchive(false)
, m_FormatResults(false)
, m_TotalWidth(8)
, m_Decimals(2)
{
	m_strCaption = m_psp.pszTitle;
	if( nullptr != m_pParent )
	{
		m_pTool = dynamic_cast <SvTo::SVArchiveTool*> (m_pParent->GetTaskObject());
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

	CString Text;
	// Update the file path to the archive file for associated archive tool.
	m_ArchiveFileName.GetWindowText( Text );
	std::string ArchiveFilepath = Text;

	//check for valid drive for text archive
	SvTo::ArchiveToolHelper athArchivePathAndName;
	athArchivePathAndName.Init( ArchiveFilepath );

	m_pArchiveImagePage->updateImageFilePathRootElements();

	SvStl::MessageTextEnum pathErrorDescriptionId = SvStl::Tid_Empty;

	if (athArchivePathAndName.isUsingKeywords())
	{
		if (athArchivePathAndName.areTokensValid())
		{
			std::string TmpArchiveFileName = athArchivePathAndName.TranslatePath( ArchiveFilepath );
			if (false == SVCheckPathDir(TmpArchiveFileName.c_str(), true))
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
		if (false == SVCheckPathDir(ArchiveFilepath.c_str(), true))
		{
			pathErrorDescriptionId = SvStl::Tid_InvalidPath;
		}
	}

	if(SvStl::Tid_Empty != pathErrorDescriptionId)
	{   //do not allow exiting with invalid path
		SvDef::StringVector msgList;
		msgList.push_back(ArchiveFilepath);
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_73_ARCHIVE_MEMORY, pathErrorDescriptionId, SvStl::SourceFileParams(StdMessageParams));
		return false;
	}

	if(!ValidateDrive(ArchiveFilepath) || ArchiveFilepath.empty())
	{
		return false; 
	}

	m_pTool->SetFileArchive( ArchiveFilepath.c_str() );

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
		SvDef::StringPairVector l_HeaderPairs;
		GetSelectedHeaderNamePairs(l_HeaderPairs); // filters by what is selected.
		StoreHeaderValuesToTool( l_HeaderPairs );
	}

	m_pTool->resetAllObjects();

	// Mark the document as 'dirty' so user will be prompted to save
	// this configuration on program exit.
	SVIPDoc* l_pIPDoc = m_pParent->GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		l_pIPDoc->SetModifiedFlag();
	}

	return true;   // Everything is OK
}
#pragma endregion Public Methods

#pragma region Private Methods
void SVTADlgArchiveResultsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SELECTED, m_ItemsSelected);
	DDX_Control(pDX, IDC_SELECT_BUTTON, m_Select);
	DDX_Control(pDX, IDC_ARCHIVE_FILENAME, m_ArchiveFileName);
	DDX_Check(pDX, IDC_CHECK_APPEND, m_AppendArchive);
	DDX_Check(pDX, IDC_FORMAT_RESULTS, m_FormatResults);
	DDX_Text(pDX, IDC_TOTAL_WIDTH, m_TotalWidth);
	DDX_Text(pDX, IDC_DECIMAL, m_Decimals);
	DDX_Check(pDX, IDC_HEADER_CHECK, m_ColumnHeaders);                  
	DDX_Control(pDX, IDC_TOTAL_WIDTH, m_TotalWidthEdit);
	DDX_Control(pDX, IDC_DECIMAL, m_DecimalsEdit);

	m_DecimalsEdit.EnableWindow(m_FormatResults);
	m_TotalWidthEdit.EnableWindow(m_FormatResults);
}


BOOL SVTADlgArchiveResultsPage::OnInitDialog() 
{
	CWaitCursor wait;

	CPropertyPage::OnInitDialog();

	GetWindowText( m_strCaption );

	m_ItemsSelected.InsertColumn( ItemSelectedCol, _T(""), 0, ItemsSelectedWidth );
	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR24 | ILC_MASK, IconNumber, IconGrowBy );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_CHECKED_ENABLED ) );
	m_ItemsSelected.SetImageList( &m_StateImageList, LVSIL_STATE );

	ReadSelectedObjects();//@TODO [Arvid][10.00][17.6.2020] is this call really necessary? ReadSelectedObjects() will be called again later anyway.

	m_TreeBitmap.LoadBitmap( IDB_TREE );
	m_Select.SetBitmap( static_cast<HBITMAP> (m_TreeBitmap.GetSafeHandle()) );

	CDWordArray dwaIndex;
	
	std::string ArchiveFilepath; 
	m_pTool->GetArchiveResultFilepath( ArchiveFilepath );
	m_ArchiveFileName.SetWindowText( ArchiveFilepath.c_str() );

	DWORD dwTemp = 0;
	m_pTool->m_dwAppendArchiveFile.GetValue(dwTemp);
	m_AppendArchive = (int)dwTemp;

	dwTemp = 0;
	m_pTool->m_bvoFormatResults.GetValue(dwTemp);
	m_FormatResults = (BOOL)dwTemp;

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

void SVTADlgArchiveResultsPage::OnDblClickListSelected(NMHDR*, LRESULT*) //@TODO [Arvid][10.00][7.8.2020] Remove unnamed parameter
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
		SVObjectReferenceVector::const_iterator SelectedIter( m_ResultsToBeArchived.begin() + *Iter );
		m_ResultsToBeArchived.erase( SelectedIter );
	}

	ReadSelectedObjects();
}

void SVTADlgArchiveResultsPage::ReadSelectedObjects()
{
	m_ItemsSelected.DeleteAllItems();

	std::string Prefix = m_pTool->GetInspection()->GetName();
	Prefix += _T(".Tool Set.");

	int Index = 0;
	for (auto const& rEntry : m_ResultsToBeArchived)
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

void SVTADlgArchiveResultsPage::ShowObjectSelector()
{
	uint32_t InspectionId(m_pTool->GetInspection()->getObjectId());

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{ SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::cameraObject, SvPb::SearchArea::toolsetItems }, InspectionId, SvPb::archivable);

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
	m_ArchiveFileName.GetWindowText( Text );
	std::string ArchiveFullName = Text;

	SVCheckPathDir( ArchiveFullName.c_str(), TRUE );

	svfncArchiveFileName.SetFileType(SV_DEFAULT_FILE_TYPE);
	svfncArchiveFileName.SetDefaultFullFileName( ArchiveFullName.c_str() );
	if (svfncArchiveFileName.SelectFile())
	{
		m_ArchiveFileName.SetWindowText(svfncArchiveFileName.GetFullFileName().c_str());
	}
}

void SVTADlgArchiveResultsPage::OnFormatResults()
{
	UpdateData(TRUE);
}

bool SVTADlgArchiveResultsPage::GetSelectedHeaderNamePairs(SvDef::StringPairVector& HeaderPairs)
{
	bool bRet = false;
	if( m_pTool )
	{
		// Inputs:
		// Collect and build string pair vector from header id and Header Label from the archive tool.
		// but only add if ids match the selected object ids
		// output a vector of Object Name / Label pairs filtered by the selected objects..

		// Get Lists....
		SvDef::StringVector HeaderLabelNames;
		SvDef::StringVector HeaderObjectIDs;
		m_pTool->m_HeaderLabelNames.GetArrayValues( HeaderLabelNames );
		m_pTool->m_HeaderObjectIDs.GetArrayValues( HeaderObjectIDs );

		// Collect Object and Label into pairs.
		for( SvDef::StringVector::const_iterator it = HeaderObjectIDs.begin(),it1 = HeaderLabelNames.begin() ; it != HeaderObjectIDs.end() ;++it1, ++it)
		{
			SvDef::StringPair l_Pair(*it, *it1 );
			HeaderPairs.push_back(l_Pair);
		}

		// ... Create List from selected...
		SvDef::StringPairVector SelectedHeaderPairs;
		for(auto const& rEntry : m_ResultsToBeArchived)
		{
			uint32_t objectId{ rEntry.getObjectId() };

			SvDef::StringPair NewPair(convertObjectIdToString(objectId),
			SVObjectManagerClass::Instance().GetObject(objectId)->GetCompleteName() );
			SelectedHeaderPairs.push_back(NewPair);
		}

		// copy labels to the selected List...
		for(SvDef::StringPairVector::iterator it = SelectedHeaderPairs.begin() ; it != SelectedHeaderPairs.end(); ++it)
		{
			bool bFound = false;
			SvDef::StringPairVector::const_iterator it2 = HeaderPairs.begin();
			for( ; it2 != HeaderPairs.end() ; ++it2)
			{
				if( it->first == it2->first ) // IDs match
				{
					// Found Stop looking...
					bFound = true;
					break;
				}
			}
			if( bFound )
			{
				// copy label to new list.
				it->second = it2->second;
			}
		}

		HeaderPairs = SelectedHeaderPairs;
		bRet = true;
	}
	return bRet;
}

bool SVTADlgArchiveResultsPage::StoreHeaderValuesToTool(SvDef::StringPairVector& HeaderPairs)
{
	bool bRet = false;
	if( m_pTool )
	{
		SvDef::StringVector HeaderLabelNames;
		SvDef::StringVector HeaderObjectIDs;
		for(SvDef::StringPairVector::iterator it = HeaderPairs.begin(); it != HeaderPairs.end() ;++it)
		{
			HeaderObjectIDs.push_back( it->first );
			HeaderLabelNames.push_back( it->second );
		}
		m_pTool->m_HeaderLabelNames.SetArraySize( static_cast<int>(HeaderPairs.size()) );
		m_pTool->m_HeaderObjectIDs.SetArraySize( static_cast<int>(HeaderPairs.size()) );
		m_pTool->m_HeaderLabelNames.SetArrayValues(HeaderLabelNames);
		m_pTool->m_HeaderObjectIDs.SetArrayValues( HeaderObjectIDs);
		bRet = true;
	}
	return bRet;
}

void SVTADlgArchiveResultsPage::OnBnClickedHeaderBtn()
{
	// Get a collection of header name/Label pairs from this class and the tool.
	SvDef::StringPairVector l_HeaderPairs;
	GetSelectedHeaderNamePairs(l_HeaderPairs); // filters by what is selected.

	SVArchiveHeaderEditDlg l_dlg;
	l_dlg.SetValues( l_HeaderPairs );

	INT_PTR iRet = l_dlg.DoModal();
	if( iRet == IDOK)
	{
		l_dlg.GetValues( l_HeaderPairs );
		StoreHeaderValuesToTool( l_HeaderPairs );
	}
}

void SVTADlgArchiveResultsPage::OnBnClickedHeaderCheck()
{
	UpdateData( TRUE );

	BOOL bEnable = 0 != m_ResultsToBeArchived.size() && m_ColumnHeaders;
	GetDlgItem(IDC_HEADER_BTN)->EnableWindow(bEnable);

	m_pTool->m_bvoUseHeaders.SetValue(m_ColumnHeaders);
}
#pragma endregion Private Methods

