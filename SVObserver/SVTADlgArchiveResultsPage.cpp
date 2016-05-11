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
#include <numeric>
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
#include "SVStatusLibrary/MessageManagerResource.h"
#include "SVOResource/ConstGlobalSvOr.h"
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

	// Update the file path to the archive file for associated archive tool.
	CString csArchiveFileName;
	m_ArchiveFileName.GetWindowText( csArchiveFileName );

	//check for valid drive for text archive
	CString szDrive;

	CString sTmpArchiveFileName;
	ArchiveToolHelper athArchivePathAndName;
	athArchivePathAndName.Init(SVString(csArchiveFileName));

	if (athArchivePathAndName.isUsingKeywords())
	{
		if (athArchivePathAndName.isTokensValid())
		{
			sTmpArchiveFileName = athArchivePathAndName.TranslatePath(SVString(csArchiveFileName)).c_str();
			SVCheckPathDir( sTmpArchiveFileName, TRUE );
		}
		else
		{
			//don't allow to exit with invalid path
			SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
			Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_InvalidFileName, StdMessageParams );
			return false;
		}
	}
	else
	{
		//not using Keywords 
		SVCheckPathDir( csArchiveFileName, TRUE );
	}

	if(!ArchiveToolHelper::ValidateDrive(csArchiveFileName,szDrive) || csArchiveFileName.IsEmpty())
	{
		SVStringArray msgList;
		msgList.push_back(SVString(szDrive));
		SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_InvalidDrive, msgList, StdMessageParams );

		return false; 
	}

	//
	// Check for possible duplicate path to results archive in any other
	// archive tool in any other toolset, i.e. it must be unique on the 
	// NT file system.
	//
	BOOL bResult = m_pTool->CheckForUniqueArchiveFilePath(csArchiveFileName);
	if(!bResult)
	{
		SVStringArray msgList;
		msgList.push_back(SVString(csArchiveFileName));
		SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_AP_InvalidFile, msgList, StdMessageParams );
		return false;   // Property is ready to exit.
	}

	m_pTool->SetFileArchive( csArchiveFileName );

	m_pTool->m_dwAppendArchiveFile.SetValue( 1, m_AppendArchive );

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
			ObjectRef.ObjectAttributesSetRef() |= SV_ARCHIVABLE;
		}
	}
	// Now make a list of archivable objects from the SVOutputInfoListClass.
	m_pTool->RebuildResultsArchiveList();

	// Add newly selected values to headers.
	bool bUseHeaders = false;
	HRESULT hr = m_pTool->m_bvoUseHeaders.GetValue( bUseHeaders );
	if( S_OK == hr && bUseHeaders )
	{
		StringPairVect l_HeaderPairs;
		GetSelectedHeaderNamePairs(l_HeaderPairs); // filters by what is selected.
		StoreHeaderValuesToTool( l_HeaderPairs );
	}
	HRESULT hRet = m_pTool->ValidateArchiveTool();

	SVSendMessage( m_pTool, SVM_RESET_ALL_OBJECTS, 0, 0 );

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
	//will need to fix the call to ResetObject at a later date - RPY
	m_pTool->ResetObject();

	m_ItemsSelected.InsertColumn( ItemSelectedCol, _T(""), 0, ItemsSelectedWidth );
	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR24 | ILC_MASK, IconNumber, IconGrowBy );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_CHECKED_ENABLED ) );
	m_ItemsSelected.SetImageList( &m_StateImageList, LVSIL_STATE );

	ReadSelectedObjects();

	m_TreeBitmap.LoadBitmap( IDB_TREE );
	m_Select.SetBitmap( static_cast<HBITMAP> (m_TreeBitmap.GetSafeHandle()) );

	CDWordArray dwaIndex;
	int iIndex=0;
	
	SVToolSetClass* pToolSet = m_pTool->GetInspection()->GetToolSet();

	// Get the list of already existing selected results to archive
	// from the archive tool.
	m_pTool->UpdateTaskObjectOutputList();

	CString		csArchiveFileName; 
	m_pTool->GetFileArchive( csArchiveFileName );
	m_ArchiveFileName.SetWindowText(csArchiveFileName);

	DWORD dwTemp=0;
	m_pTool->m_dwAppendArchiveFile.GetValue( dwTemp );
	m_AppendArchive = (int)dwTemp;

	bool bUseColumnHeaders = false;
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

	SvOsl::SelectorOptions BuildOptions( m_pTool->GetInspection()->GetUniqueObjectID(), SV_ARCHIVABLE );
	SvOg::ToolSetItemSelector<GuiCmd::AttributesSetFilterType> toolsetItemSelector;
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
			ObjectRef.ObjectAttributesSetRef() &= ~SV_ARCHIVABLE;
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
			ObjectRef.ObjectAttributesSetRef() &= ~SV_ARCHIVABLE;
		}

		m_List.erase( SelectedIter );
	}

	ReadSelectedObjects();
}

void SVTADlgArchiveResultsPage::ReadSelectedObjects()
{
	m_ItemsSelected.DeleteAllItems();

	CString strPrefix = m_pTool->GetInspection()->GetName();
	strPrefix += _T(".Tool Set.");

	int Index = 0;
	SvOsl::SelectorItemVector::const_iterator Iter;
	for ( Iter = m_List.begin(); m_List.end() != Iter ; ++Iter )
	{
		SVString Name = Iter->getLocation();
		SvUl_SF::searchAndReplace( Name, strPrefix, _T("") );

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

	SvOsl::SelectorOptions BuildOptions( InspectionGuid, SV_ARCHIVABLE );
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
	CString Filter;
	Filter.LoadString( IDS_FILTER );
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), m_strCaption, Filter, this );

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
					ObjectRef.ObjectAttributesSetRef() &= ~SV_ARCHIVABLE;
				}
			}
		}
	}
}

void SVTADlgArchiveResultsPage::OnBrowse() 
{
	SVFileNameClass	svfncArchiveFileName;

	//get current path
	CString sArchiveFullNameAndPath;
	m_ArchiveFileName.GetWindowText( sArchiveFullNameAndPath );

	ArchiveToolHelper athArchivePathAndName;
	athArchivePathAndName.Init(SVString(sArchiveFullNameAndPath)); 

	bool bUsingKeywords = athArchivePathAndName.isUsingKeywords();
	if (bUsingKeywords)
	{
		if (athArchivePathAndName.isTokensValid())
		{
			sArchiveFullNameAndPath = athArchivePathAndName.TranslatePath(SVString(sArchiveFullNameAndPath)).c_str();
		}
		else
		{
			//don't allow to exit with invalid path
			SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
			Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_InvalidFileName, StdMessageParams );
			return;
		}
	}
	CString sFileName;
	CString sPath;
	int iEndOfPath = sArchiveFullNameAndPath.ReverseFind('\\');
	sFileName = sArchiveFullNameAndPath.Right((sArchiveFullNameAndPath.GetLength() - iEndOfPath));
	sPath = sArchiveFullNameAndPath.Left(iEndOfPath);

	SVCheckPathDir( sArchiveFullNameAndPath, TRUE );

	svfncArchiveFileName.SetFileType(SV_DEFAULT_FILE_TYPE);
	svfncArchiveFileName.SetDefaultFullFileName(sArchiveFullNameAndPath);
	if (svfncArchiveFileName.SelectFile())
	{
		m_ArchiveFileName.SetWindowText(svfncArchiveFileName.GetFullFileName());
	}
}

bool SVTADlgArchiveResultsPage::GetSelectedHeaderNamePairs( StringPairVect& HeaderPairs)
{
	bool bRet = false;
	if( m_pTool )
	{
		// Inputs:
		// Collect and build string pair vector from header Guid and Header Label from the archive tool.
		// but only add if Guids match the selected object Guids
		// output a vector of Object Name / Label pairs filtered by the selected objects..
		std::vector<int> vecInt;

		// Get Lists....
		typedef std::vector<CString> StringVect;
		StringVect l_HeaderLabelNames;
		StringVect l_HeaderObjectGUIDs;
		m_pTool->m_HeaderLabelNames.GetValues( l_HeaderLabelNames );
		m_pTool->m_HeaderObjectGUIDs.GetValues( l_HeaderObjectGUIDs );

		// Collect Object and Label into pairs.
		for( StringVect::const_iterator it = l_HeaderObjectGUIDs.begin(),it1 = l_HeaderLabelNames.begin() ; it != l_HeaderObjectGUIDs.end() ;++it1, ++it)
		{
			StrStrPair l_Pair(*it, *it1 );
			HeaderPairs.push_back(l_Pair);
		}

		// ... Create List from selected...
		StringPairVect SelectedHeaderPairs;
		SvOsl::SelectorItemVector::const_iterator Iter;
		for( Iter = m_List.begin(); m_List.end() != Iter ; ++Iter )
		{
			SVGUID GuidValue( Iter->getItemKey() );

			StrStrPair NewPair( GuidValue.ToString().c_str(), 
			SVObjectManagerClass::Instance().GetObject(GuidValue)->GetCompleteObjectName() );
			SelectedHeaderPairs.push_back(NewPair);
		}

		// copy labels to the selected List...
		for( StringPairVect::iterator it = SelectedHeaderPairs.begin() ; it != SelectedHeaderPairs.end(); ++it)
		{
			bool bFound = false;
			StringPairVect::const_iterator it2 = HeaderPairs.begin();
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

bool SVTADlgArchiveResultsPage::StoreHeaderValuesToTool(StringPairVect& HeaderPairs)
{
	bool bRet = false;
	if( m_pTool )
	{
		std::vector<CString> l_HeaderLabelNames;
		std::vector<CString> l_HeaderObjectGUIDs;
		for( StringPairVect::iterator it = HeaderPairs.begin(); it != HeaderPairs.end() ;++it)
		{
			l_HeaderObjectGUIDs.push_back( it->first);
			l_HeaderLabelNames.push_back( it->second);
		}
		m_pTool->m_HeaderLabelNames.SetArraySize( static_cast<int>(HeaderPairs.size()) );
		m_pTool->m_HeaderObjectGUIDs.SetArraySize( static_cast<int>(HeaderPairs.size()) );
		m_pTool->m_HeaderLabelNames.SetArrayValues(0, l_HeaderLabelNames.begin(), l_HeaderLabelNames.end());
		m_pTool->m_HeaderObjectGUIDs.SetArrayValues(0, l_HeaderObjectGUIDs.begin(), l_HeaderObjectGUIDs.end());
		bRet = true;
	}
	return bRet;
}

void SVTADlgArchiveResultsPage::OnBnClickedHeaderBtn()
{
	// Get a collection of header name/Label pairs from this class and the tool.
	StringPairVect l_HeaderPairs;
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

	m_pTool->m_bvoUseHeaders.SetValue( 1, m_ColumnHeaders );
}
#pragma endregion Private Methods

