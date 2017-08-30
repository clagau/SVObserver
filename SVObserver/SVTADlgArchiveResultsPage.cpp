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
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVArchiveTool.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"
#include "SVArchiveHeaderEditDlg.h"
#include "ArchiveToolHelper.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVUtilityLibrary/SVString.h"
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
	SVString ArchiveFileName = Text;

	//check for valid drive for text archive
	SVString TmpArchiveFileName = ArchiveFileName;
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
			SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
			Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_InvalidFileName, SvStl::SourceFileParams(StdMessageParams) );
			return false;
		}
	}
	else
	{
		//not using Keywords 
		SVCheckPathDir( ArchiveFileName.c_str(), true );
	}
	SVString Drive;
	if(!ArchiveToolHelper::ValidateDrive(ArchiveFileName.c_str(), Drive) || ArchiveFileName.empty())
	{
		SVStringVector msgList;
		msgList.push_back( Drive );
		SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_InvalidDrive, msgList, SvStl::SourceFileParams(StdMessageParams) );

		return false; 
	}

	m_pTool->SetFileArchive( ArchiveFileName.c_str() );

	m_pTool->m_dwAppendArchiveFile.SetValue(static_cast<DWORD> (m_AppendArchive));

	SvOsl::SelectorItemVector::const_iterator Iter;
	for ( Iter = m_List.begin(); m_List.end() != Iter ; ++Iter )
	{
		//The tree item key is the object GUID
		SVGUID ObjectGuid( Iter->getItemKey() );

		SVObjectClass* pObject( nullptr );
		SVObjectManagerClass::Instance().GetObjectByIdentifier( ObjectGuid, pObject );

		if( nullptr != pObject )
		{
			SVObjectReference ObjectRef( pObject );
			if( Iter->isArray() )
			{
				ObjectRef.SetArrayIndex( Iter->getArrayIndex() );
			}
			ObjectRef.SetObjectAttributesSet( SvOi::SV_ARCHIVABLE, SvOi::SetAttributeType::AddAttribute );
		}
	}
	// Now make a list of archivable objects from the SVOutputInfoListClass.
	m_pTool->RebuildResultsArchiveList();

	// Add newly selected values to headers.
	BOOL bUseHeaders( false );
	HRESULT hr = m_pTool->m_bvoUseHeaders.GetValue( bUseHeaders );
	if( S_OK == hr && bUseHeaders )
	{
		StringPairVector l_HeaderPairs;
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

	// Get the list of already existing selected results to archive
	// from the archive tool.
	m_pTool->UpdateTaskObjectOutputList();

	SVString ArchiveFileName; 
	m_pTool->GetFileArchive( ArchiveFileName );
	m_ArchiveFileName.SetWindowText( ArchiveFileName.c_str() );

	DWORD dwTemp=0;
	m_pTool->m_dwAppendArchiveFile.GetValue( dwTemp );
	m_AppendArchive = (int)dwTemp;

	BOOL bUseColumnHeaders( false );
	m_pTool->m_bvoUseHeaders.GetValue( bUseColumnHeaders );
	if(bUseColumnHeaders && m_pTool->m_arrayResultsInfoObjectsToArchive.GetSize() > 0)
	{
		m_ColumnHeaders = 1;
		GetDlgItem(IDC_HEADER_BTN)->EnableWindow();
	}
	else
	{
		m_ColumnHeaders = 0;
		GetDlgItem(IDC_HEADER_BTN)->EnableWindow(FALSE);
	}

	SvOsl::SelectorOptions BuildOptions( m_pTool->GetInspection()->GetUniqueObjectID(), SvOi::SV_ARCHIVABLE );
	SvOg::ToolSetItemSelector<SvCmd::AttributesSetFilterType> toolsetItemSelector;
	SvOi::ISelectorItemVectorPtr pToolsetList = toolsetItemSelector( BuildOptions );
	//Copy list to member variable for easier use
	if( !pToolsetList.empty() )
	{
		SvOsl::SelectorItemVector* pSelectorList = dynamic_cast<SvOsl::SelectorItemVector*> (pToolsetList.get());
		if( nullptr != pSelectorList )
		{
			m_List.swap(*pSelectorList);
		}
	}

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
	SvOsl::SelectorItemVector::const_iterator Iter;
	for ( Iter = m_List.begin(); m_List.end() != Iter ; ++Iter )
	{
		SVGUID ObjectGuid( Iter->getItemKey() );

		SVObjectClass* pObject( nullptr );
		SVObjectManagerClass::Instance().GetObjectByIdentifier( ObjectGuid, pObject );

		if( nullptr != pObject )
		{
			SVObjectReference ObjectRef( pObject );
			if( Iter->isArray() )
			{
				ObjectRef.SetArrayIndex( Iter->getArrayIndex() );
			}
			ObjectRef.SetObjectAttributesSet( SvOi::SV_ARCHIVABLE, SvOi::SetAttributeType::RemoveAttribute );
		}
	}
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
		SvOsl::SelectorItemVector::const_iterator SelectedIter( m_List.begin() + *Iter );
	
		SVGUID ObjectGuid( SelectedIter->getItemKey() );
		SVObjectClass* pObject( nullptr );
		SVObjectManagerClass::Instance().GetObjectByIdentifier( ObjectGuid, pObject );

		if( nullptr != pObject )
		{
			SVObjectReference ObjectRef( pObject );
			if( SelectedIter->isArray() )
			{
				ObjectRef.SetArrayIndex( SelectedIter->getArrayIndex() );
			}
			ObjectRef.SetObjectAttributesSet( SvOi::SV_ARCHIVABLE, SvOi::SetAttributeType::RemoveAttribute );
		}

		m_List.erase( SelectedIter );
	}

	ReadSelectedObjects();
}

void SVTADlgArchiveResultsPage::ReadSelectedObjects()
{
	m_ItemsSelected.DeleteAllItems();

	SVString Prefix = m_pTool->GetInspection()->GetName();
	Prefix += _T(".Tool Set.");

	int Index = 0;
	SvOsl::SelectorItemVector::const_iterator Iter;
	for ( Iter = m_List.begin(); m_List.end() != Iter ; ++Iter )
	{
		SVString Name = Iter->getLocation();
		SvUl_SF::searchAndReplace( Name, Prefix.c_str(), _T("") );

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
	SVString InspectionName( m_pTool->GetInspection()->GetName() );
	SVGUID InspectionGuid( m_pTool->GetInspection()->GetUniqueObjectID() );

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );

	SvOsl::SelectorOptions BuildOptions( InspectionGuid, SvOi::SV_ARCHIVABLE );
	SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );

	SvOsl::SelectorItemVector::const_iterator Iter;
	SVStringSet CheckItems;
	for ( Iter = m_List.begin(); m_List.end() != Iter ; ++Iter )
	{
		SVString ObjectName;
		ObjectName = Iter->getLocation();
		CheckItems.insert( ObjectName );
	}
	SvOsl::ObjectTreeGenerator::Instance().setCheckItems( CheckItems );

	SVString Title = SvUl_SF::Format( _T("%s - %s"), m_strCaption, InspectionName.c_str() );
	SVString Filter = SvUl_SF::LoadSVString( IDS_FILTER );
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), m_strCaption, Filter.c_str(), this );

	if( IDOK == Result )
	{
		m_List = SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects();

		ReadSelectedObjects();

		//We need to remove unselected objects attributes
		const SvOsl::SelectorItemVector& rModifiedObjects = SvOsl::ObjectTreeGenerator::Instance().getModifiedObjects();
		SvOsl::SelectorItemVector::const_iterator Iter;
		for(Iter = rModifiedObjects.begin(); rModifiedObjects.end() != Iter; ++Iter )
		{
			if( !Iter->isSelected() )
			{
				SVGUID ObjectGuid( Iter->getItemKey() );

				SVObjectClass* pObject( nullptr );
				SVObjectManagerClass::Instance().GetObjectByIdentifier( ObjectGuid, pObject );

				if( nullptr != pObject )
				{
					SVObjectReference ObjectRef( pObject );
					if( Iter->isArray() )
					{
						ObjectRef.SetArrayIndex( Iter->getArrayIndex() );
					}
					ObjectRef.SetObjectAttributesSet( SvOi::SV_ARCHIVABLE, SvOi::SetAttributeType::RemoveAttribute );
				}
			}
		}
	}
}

void SVTADlgArchiveResultsPage::OnBrowse() 
{
	SVFileNameClass	svfncArchiveFileName;

	//get current path
	CString Text;
	m_ArchiveFileName.GetWindowText( Text );
	SVString ArchiveFullName = Text;

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
			SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
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

bool SVTADlgArchiveResultsPage::GetSelectedHeaderNamePairs( StringPairVector& HeaderPairs)
{
	bool bRet = false;
	if( m_pTool )
	{
		// Inputs:
		// Collect and build string pair vector from header Guid and Header Label from the archive tool.
		// but only add if Guids match the selected object Guids
		// output a vector of Object Name / Label pairs filtered by the selected objects..

		// Get Lists....
		SVStringVector HeaderLabelNames;
		SVStringVector HeaderObjectGUIDs;
		m_pTool->m_HeaderLabelNames.GetArrayValues( HeaderLabelNames );
		m_pTool->m_HeaderObjectGUIDs.GetArrayValues( HeaderObjectGUIDs );

		// Collect Object and Label into pairs.
		for( SVStringVector::const_iterator it = HeaderObjectGUIDs.begin(),it1 = HeaderLabelNames.begin() ; it != HeaderObjectGUIDs.end() ;++it1, ++it)
		{
			SVStringPair l_Pair(*it, *it1 );
			HeaderPairs.push_back(l_Pair);
		}

		// ... Create List from selected...
		StringPairVector SelectedHeaderPairs;
		SvOsl::SelectorItemVector::const_iterator Iter;
		for( Iter = m_List.begin(); m_List.end() != Iter ; ++Iter )
		{
			SVGUID GuidValue( Iter->getItemKey() );

			SVStringPair NewPair( GuidValue.ToString().c_str(), 
			SVObjectManagerClass::Instance().GetObject(GuidValue)->GetCompleteName() );
			SelectedHeaderPairs.push_back(NewPair);
		}

		// copy labels to the selected List...
		for( StringPairVector::iterator it = SelectedHeaderPairs.begin() ; it != SelectedHeaderPairs.end(); ++it)
		{
			bool bFound = false;
			StringPairVector::const_iterator it2 = HeaderPairs.begin();
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

bool SVTADlgArchiveResultsPage::StoreHeaderValuesToTool(StringPairVector& HeaderPairs)
{
	bool bRet = false;
	if( m_pTool )
	{
		SVStringVector HeaderLabelNames;
		SVStringVector HeaderObjectGUIDs;
		for( StringPairVector::iterator it = HeaderPairs.begin(); it != HeaderPairs.end() ;++it)
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
	StringPairVector l_HeaderPairs;
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

