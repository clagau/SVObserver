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
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "InspectionCommands/BuildSelectableItems.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVArchiveTool.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"
#include "SVArchiveHeaderEditDlg.h"
#include "ArchiveToolHelper.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Definitions/StringTypeDef.h"
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
	ON_BN_CLICKED(IDC_HEADER_BTN, &SVTADlgArchiveResultsPage::OnBnClickedHeaderBtn)
	ON_BN_CLICKED(IDC_HEADER_CHECK, &SVTADlgArchiveResultsPage::OnBnClickedHeaderCheck)
END_MESSAGE_MAP()

static const int ItemSelectedCol = 1;
static const int ItemsSelectedWidth = 500;
static const int IconNumber = 1;
static const int IconGrowBy = 2;
#pragma endregion Declarations

#pragma region Constructor
SVTADlgArchiveResultsPage::SVTADlgArchiveResultsPage(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent) 
: CPropertyPage(SVTADlgArchiveResultsPage::IDD)
, m_pParent(Parent)
, m_pTool(nullptr)
, m_ColumnHeaders(false)
, m_AppendArchive( 0 )
{
	m_strCaption = m_psp.pszTitle;
	if( nullptr != m_pParent )
	{
		m_pTool = dynamic_cast <SVArchiveTool*> (m_pParent->GetTool());
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
	std::string ArchiveFileName = Text;

	//check for valid drive for text archive
	std::string TmpArchiveFileName = ArchiveFileName;
	ArchiveToolHelper athArchivePathAndName;
	athArchivePathAndName.Init( ArchiveFileName );

	if (athArchivePathAndName.isUsingKeywords())
	{
		if (athArchivePathAndName.isTokensValid())
		{
			TmpArchiveFileName = athArchivePathAndName.TranslatePath( ArchiveFileName );
			SVCheckPathDir( TmpArchiveFileName.c_str(), true );
		}
		else
		{
			//don't allow to exit with invalid path
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_InvalidFileName, SvStl::SourceFileParams(StdMessageParams) );
			return false;
		}
	}
	else
	{
		//not using Keywords 
		SVCheckPathDir( ArchiveFileName.c_str(), true );
	}
	std::string Drive;
	if(!ArchiveToolHelper::ValidateDrive(ArchiveFileName.c_str(), Drive) || ArchiveFileName.empty())
	{
		SvDef::StringVector msgList;
		msgList.push_back( Drive );
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_InvalidDrive, msgList, SvStl::SourceFileParams(StdMessageParams) );

		return false; 
	}

	m_pTool->SetFileArchive( ArchiveFileName.c_str() );

	m_pTool->m_dwAppendArchiveFile.SetValue(static_cast<DWORD> (m_AppendArchive));

	m_pTool->setResultArchiveList(m_List);

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
	DDX_Check(pDX, IDC_HEADER_CHECK, m_ColumnHeaders);
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

	ReadSelectedObjects();

	m_TreeBitmap.LoadBitmap( IDB_TREE );
	m_Select.SetBitmap( static_cast<HBITMAP> (m_TreeBitmap.GetSafeHandle()) );

	CDWordArray dwaIndex;
	int iIndex=0;
	
	SVToolSetClass* pToolSet = dynamic_cast<SVInspectionProcess*>(m_pTool->GetInspection())->GetToolSet();

	std::string ArchiveFileName; 
	m_pTool->GetFileArchive( ArchiveFileName );
	m_ArchiveFileName.SetWindowText( ArchiveFileName.c_str() );

	DWORD dwTemp=0;
	m_pTool->m_dwAppendArchiveFile.GetValue( dwTemp );
	m_AppendArchive = (int)dwTemp;

	m_pTool->m_bvoUseHeaders.GetValue(m_ColumnHeaders);
	GetDlgItem(IDC_HEADER_BTN)->EnableWindow(m_ColumnHeaders);

	m_List.swap(m_pTool->getResultArchiveList());

	ReadSelectedObjects();
	UpdateData(FALSE);
	return TRUE;
}

void SVTADlgArchiveResultsPage::OnDblClickListSelected( NMHDR *pNMHDR, LRESULT *pResult )
{
	ShowObjectSelector();
}

void SVTADlgArchiveResultsPage::OnSelectObjects()
{
	ShowObjectSelector();
}

void SVTADlgArchiveResultsPage::OnRemoveAllItems()
{
	m_List.clear();
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
		SVObjectReferenceVector::const_iterator SelectedIter( m_List.begin() + *Iter );
		m_List.erase( SelectedIter );
	}

	ReadSelectedObjects();
}

void SVTADlgArchiveResultsPage::ReadSelectedObjects()
{
	m_ItemsSelected.DeleteAllItems();

	std::string Prefix = m_pTool->GetInspection()->GetName();
	Prefix += _T(".Tool Set.");

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

void SVTADlgArchiveResultsPage::ShowObjectSelector()
{
	std::string InspectionName( m_pTool->GetInspection()->GetName() );
	SVGUID InspectionGuid( m_pTool->GetInspection()->GetUniqueObjectID() );

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, std::string( _T("") ) );

	SvCmd::SelectorOptions BuildOptions {{SvCmd::ObjectSelectorType::toolsetItems}, InspectionGuid, SvDef::SV_ARCHIVABLE};
	SvCl::SelectorItemVector SelectorItems;
	SvCmd::BuildSelectableItems(BuildOptions, std::back_inserter(SelectorItems));
	SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(SelectorItems);

	SvDef::StringSet CheckItems;
	for (auto const& rEntry : m_List)
	{
		CheckItems.insert(rEntry.GetCompleteName(true));
	}
	SvOsl::ObjectTreeGenerator::Instance().setCheckItems( CheckItems );

	std::string Title = SvUl::Format( _T("%s - %s"), m_strCaption, InspectionName.c_str() );
	std::string Filter = SvUl::LoadStdString( IDS_FILTER );
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), m_strCaption, Filter.c_str(), this );

	if( IDOK == Result )
	{
		m_List.clear();
		for (auto const& rEntry : SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects())
		{
			SVGUID ObjectGuid{ rEntry.m_ItemKey };

			SVObjectClass* pObject(nullptr);
			SVObjectManagerClass::Instance().GetObjectByIdentifier(ObjectGuid, pObject);

			if (nullptr != pObject)
			{
				SVObjectReference ObjectRef(pObject);
				if (rEntry.m_Array)
				{
					ObjectRef.SetArrayIndex(rEntry.m_ArrayIndex);
				}
				m_List.push_back(ObjectRef);
			}
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

	ArchiveToolHelper athArchivePathAndName;
	athArchivePathAndName.Init( ArchiveFullName ); 

	bool bUsingKeywords = athArchivePathAndName.isUsingKeywords();
	if (bUsingKeywords)
	{
		if (athArchivePathAndName.isTokensValid())
		{
			ArchiveFullName = athArchivePathAndName.TranslatePath(ArchiveFullName);
		}
		else
		{
			//don't allow to exit with invalid path
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_InvalidFileName, SvStl::SourceFileParams(StdMessageParams) );
			return;
		}
	}

	SVCheckPathDir( ArchiveFullName.c_str(), TRUE );

	svfncArchiveFileName.SetFileType(SV_DEFAULT_FILE_TYPE);
	svfncArchiveFileName.SetDefaultFullFileName( ArchiveFullName.c_str() );
	if (svfncArchiveFileName.SelectFile())
	{
		m_ArchiveFileName.SetWindowText(svfncArchiveFileName.GetFullFileName().c_str());
	}
}

bool SVTADlgArchiveResultsPage::GetSelectedHeaderNamePairs(SvDef::StringPairVector& HeaderPairs)
{
	bool bRet = false;
	if( m_pTool )
	{
		// Inputs:
		// Collect and build string pair vector from header Guid and Header Label from the archive tool.
		// but only add if Guids match the selected object Guids
		// output a vector of Object Name / Label pairs filtered by the selected objects..

		// Get Lists....
		SvDef::StringVector HeaderLabelNames;
		SvDef::StringVector HeaderObjectGUIDs;
		m_pTool->m_HeaderLabelNames.GetArrayValues( HeaderLabelNames );
		m_pTool->m_HeaderObjectGUIDs.GetArrayValues( HeaderObjectGUIDs );

		// Collect Object and Label into pairs.
		for( SvDef::StringVector::const_iterator it = HeaderObjectGUIDs.begin(),it1 = HeaderLabelNames.begin() ; it != HeaderObjectGUIDs.end() ;++it1, ++it)
		{
			SvDef::StringPair l_Pair(*it, *it1 );
			HeaderPairs.push_back(l_Pair);
		}

		// ... Create List from selected...
		SvDef::StringPairVector SelectedHeaderPairs;
		for(auto const& rEntry : m_List)
		{
			SVGUID GuidValue{ rEntry.Guid() };

			SvDef::StringPair NewPair( GuidValue.ToString().c_str(),
			SVObjectManagerClass::Instance().GetObject(GuidValue)->GetCompleteName() );
			SelectedHeaderPairs.push_back(NewPair);
		}

		// copy labels to the selected List...
		for(SvDef::StringPairVector::iterator it = SelectedHeaderPairs.begin() ; it != SelectedHeaderPairs.end(); ++it)
		{
			bool bFound = false;
			SvDef::StringPairVector::const_iterator it2 = HeaderPairs.begin();
			for( ; it2 != HeaderPairs.end() ; ++it2)
			{
				if( it->first == it2->first ) // GUIDs match
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
		SvDef::StringVector HeaderObjectGUIDs;
		for(SvDef::StringPairVector::iterator it = HeaderPairs.begin(); it != HeaderPairs.end() ;++it)
		{
			HeaderObjectGUIDs.push_back( it->first );
			HeaderLabelNames.push_back( it->second );
		}
		m_pTool->m_HeaderLabelNames.SetArraySize( static_cast<int>(HeaderPairs.size()) );
		m_pTool->m_HeaderObjectGUIDs.SetArraySize( static_cast<int>(HeaderPairs.size()) );
		m_pTool->m_HeaderLabelNames.SetArrayValues( HeaderLabelNames.begin(), HeaderLabelNames.end() );
		m_pTool->m_HeaderObjectGUIDs.SetArrayValues( HeaderObjectGUIDs.begin(), HeaderObjectGUIDs.end() );
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

	BOOL bEnable = 0 != m_List.size() && m_ColumnHeaders;
	GetDlgItem(IDC_HEADER_BTN)->EnableWindow(bEnable);

	m_pTool->m_bvoUseHeaders.SetValue(m_ColumnHeaders);
}
#pragma endregion Private Methods

