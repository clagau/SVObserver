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
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVObjectLibrary/SVGetObjectDequeByTypeVisitor.h"
#include "SVArchiveTool.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
#include "SVIPDoc.h"
#include "SVOCore/SVImageClass.h"
#include "SVInspectionProcess.h"
#include "SVMemoryManager.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"
#include "SVArchiveHeaderEditDlg.h"
#include "ArchiveToolHelper.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVOResource\ConstGlobalSvOr.h"
#include "SVStatusLibrary\GlobalPath.h"
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
END_MESSAGE_MAP()

static const int UpperLimitImageNumbers = 10000000; ///Upper Limit for Input 
static const int ItemSelectedCol = 1;
static const int ItemsSelectedWidth = 500;
static const int IconNumber = 1;
static const int IconGrowBy = 1;
static const long MaxImagesWarningLimit = 100L;
#pragma endregion Declarations

#pragma region Constructor
SVTADlgArchiveImagePage::SVTADlgArchiveImagePage(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent) 
: CPropertyPage(SVTADlgArchiveImagePage::IDD)
, m_pParent(Parent)
, m_pTool(nullptr)
, m_StopAtMaxImages( false )
, m_iModeIndex( -1 )
, m_eSelectedArchiveMethod( SVArchiveInvalidMethod )
, m_ImagesToArchive( 0 )
, m_TotalArchiveImageMemoryAvailable( 0 )
, m_InitialArchiveImageMemoryUsageExcludingThisTool( 0 )
, m_InitialToolImageMemoryUsage( 0 )
, m_ToolImageMemoryUsage( 0 )
, m_Init( false )
{
	if( m_pParent )
	{
		m_pTool = dynamic_cast <SVArchiveTool*> (m_pParent->GetTool());
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
	if (SVArchiveGoOffline == m_eSelectedArchiveMethod)
	{
		//check to see if any items are selected in the image tree
		
		int iSize = m_List.GetSize();
		if( 0 < iSize )
		{
			//if memory usage < 0 do not all them to exit
			__int64 FreeMemory = CalculateFreeMem();
			if (0 > FreeMemory)
			{
				SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_AP_NotEnoughMemoryPleaseDeselect, SvStl::SourceFileParams(StdMessageParams) );
				return false;
			}
		}
	}

	//update the image path
	CString csImageFolder;
	m_ImageFilesRoot.GetWindowText( csImageFolder );
	SVString svImageFolder = SVString(csImageFolder);

	CString sTmpArchiveImagePath = csImageFolder;
	ArchiveToolHelper athImagePath;
	athImagePath.Init(svImageFolder);

	if (athImagePath.isUsingKeywords())
	{
		if (athImagePath.isTokensValid())
		{
			sTmpArchiveImagePath = athImagePath.TranslatePath(svImageFolder).c_str();
			SVCheckPathDir( sTmpArchiveImagePath, TRUE );
		}
		else
		{
			//don't allow to exit with invalid path
			SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
			Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_InvalidImagePath, SvStl::SourceFileParams(StdMessageParams) );
			return false;
		}
	}
	else
	{
		//not using Keywords 
		SVCheckPathDir( csImageFolder, TRUE );
	}

	CString szDrive;
	//check for valid drive for image archive
	if(!ArchiveToolHelper::ValidateDrive(csImageFolder,szDrive ) || csImageFolder.IsEmpty())
	{
		SVStringArray msgList;
		msgList.push_back(SVString(szDrive));
		SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_InvalidDrive, msgList, SvStl::SourceFileParams(StdMessageParams) );
		return false; 
	}

	m_pTool->SetImageArchivePath( csImageFolder );

	m_pTool->m_dwArchiveStopAtMaxImages.SetValue( 1, m_StopAtMaxImages );

	int iCurSel = m_Mode.GetCurSel();
	m_eSelectedArchiveMethod = static_cast <SVArchiveMethodEnum> (m_Mode.GetItemData(iCurSel));
	m_pTool->m_evoArchiveMethod.SetValue( 1, static_cast <DWORD> (m_eSelectedArchiveMethod) );

	CString s;
	m_MaxImages.GetWindowText(s);
	DWORD dwTemp = atol((LPCTSTR)s);
	m_ImagesToArchive = dwTemp;
	if( UpperLimitImageNumbers < dwTemp )
	{
		SVStringArray msgList;
		msgList.push_back(SvUl_SF::Format(_T("%ld"), dwTemp));
		msgList.push_back(SvUl_SF::Format(_T("%ld"), UpperLimitImageNumbers));
		SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_Error_you_have_Selected_X_Must_less_then_Y, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16075_ImageNrToBig  );
		return false;

	}

	if( MaxImagesWarningLimit < dwTemp )
	{
		SVStringArray msgList;
		msgList.push_back(SvUl_SF::Format(_T("%ld"),  dwTemp));
		SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_ArchiveTool_WarningMaxImages, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
	//Max images must be at least 1
	if( 1L > dwTemp )
	{
		dwTemp = 1L;
	}

	m_pTool->m_dwArchiveMaxImagesCount.SetValue( 1, dwTemp );

	SvOsl::SelectorItemVector::const_iterator Iter;
	for ( Iter = m_List.begin(); m_List.end() != Iter ; ++Iter )
	{
		//The tree item key is the object GUID
		SVGUID ObjectGuid( Iter->getItemKey() );

		SVObjectClass* pObject( nullptr );
		SVObjectManagerClass::Instance().GetObjectByIdentifier( ObjectGuid, pObject );

		SVObjectReference ObjectRef( pObject );
		if( Iter->isArray() )
		{
			ObjectRef.SetArrayIndex( Iter->getArrayIndex() );
		}
		ObjectRef.ObjectAttributesSetRef() |= SV_ARCHIVABLE_IMAGE;
	}

	m_pTool->RebuildImageArchiveList();

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
	HRESULT hRet = m_pTool->ValidateArchiveTool();

	m_pTool->resetAllObjects(true, false);

	return true;
}
#pragma endregion Public Methods

#pragma region Private Methods
void SVTADlgArchiveImagePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SELECTED, m_ItemsSelected);
	DDX_Control(pDX, IDC_SELECT_BUTTON, m_Select);
	DDX_Control(pDX, IDC_AVAILABLE_ARCHIVE_IMAGE_MEMORY, m_wndAvailableArchiveImageMemory);
	DDX_Control(pDX, IDC_MODE_COMBO, m_Mode);
	DDX_Control(pDX, IDC_EDIT_MAX_IMAGES, m_MaxImages);
	DDX_Control(pDX, IDC_ARCHIVE_IMAGE_FILEROOT, m_ImageFilesRoot);
	DDX_Check(pDX, IDC_CHECK_STOP_AT_MAX, m_StopAtMaxImages);
	DDX_CBIndex(pDX, IDC_MODE_COMBO, m_iModeIndex);
}

BOOL SVTADlgArchiveImagePage::OnInitDialog() 
{
	CWaitCursor Wait;
	m_Init = true;

	CPropertyPage::OnInitDialog();

	GetWindowText( m_strCaption );
	//will need to fix the call to ResetObject at a later date - RPY
	m_pTool->ResetObject();

	m_ItemsSelected.InsertColumn( ItemSelectedCol, _T(""), 0, ItemsSelectedWidth );
	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR24 | ILC_MASK, IconNumber, IconGrowBy );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_CHECKED_ENABLED ) );
	m_ItemsSelected.SetImageList( &m_StateImageList, LVSIL_STATE );

	m_TreeBitmap.LoadBitmap( IDB_TREE );
	m_Select.SetBitmap( static_cast<HBITMAP> (m_TreeBitmap.GetSafeHandle()) );

	CDWordArray dwaIndex;
	int iIndex=0;
	
	SVEnumerateVector vec;
	m_pTool->m_evoArchiveMethod.GetEnumTypes(vec);
	for ( size_t i=0; i < vec.size(); i++ )
	{
		m_Mode.SetItemData( iIndex = m_Mode.AddString(vec[i].first), vec[i].second );
		dwaIndex.SetAtGrow( vec[i].second, iIndex );
	}

	unsigned long lMode;
	m_pTool->m_evoArchiveMethod.GetValue( lMode );
	m_eSelectedArchiveMethod = static_cast<SVArchiveMethodEnum>( lMode );
	m_iModeIndex = dwaIndex.GetAt( lMode );

	//
	// Get a pointer to the toolset
	//
	SVToolSetClass* pToolSet = dynamic_cast<SVInspectionProcess*>(m_pTool->GetInspection())->GetToolSet();

	DWORD dwTemp=0;
    m_pTool->m_dwArchiveMaxImagesCount.GetValue( dwTemp );
	m_ImagesToArchive = dwTemp;
	CString s;
	s.Format(_T("%ld"),dwTemp);
	m_MaxImages.SetWindowText((LPCTSTR)s);

	//store the MaxImageNumber
	m_sMaxImageNumber = s;

	__int64 MemUsed = TheSVMemoryManager().ReservedBytes( SvO::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME );
 	m_ToolImageMemoryUsage = 0;
	m_TotalArchiveImageMemoryAvailable = TheSVMemoryManager().SizeOfPoolBytes( SvO::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME );

	m_wndAvailableArchiveImageMemory.ShowWindow( lMode == SVArchiveGoOffline );

	CString		csImageFolder; 
	
	m_pTool->GetImageArchivePath( csImageFolder );
	m_ImageFilesRoot.SetWindowText(csImageFolder);

	m_pTool->m_dwArchiveStopAtMaxImages.GetValue( dwTemp );
	m_StopAtMaxImages = (int)dwTemp;

	BuildImageList();

	m_mapInitialSelectedImageMemUsage = m_mapSelectedImageMemUsage;
	m_ToolImageMemoryUsage = CalculateToolMemoryUsage();
	m_InitialToolImageMemoryUsage = m_ToolImageMemoryUsage;

	m_InitialArchiveImageMemoryUsageExcludingThisTool = MemUsed;

	// calculate free mem if in SVArchiveGoOffline mode
	if (SVArchiveGoOffline == m_eSelectedArchiveMethod)
	{
		m_InitialArchiveImageMemoryUsageExcludingThisTool -= m_ToolImageMemoryUsage;
	}

	ReadSelectedObjects();
	m_Init = false;
	UpdateData(FALSE);
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
			ObjectRef.ObjectAttributesSetRef() &= ~SV_ARCHIVABLE_IMAGE;
		}
	}
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
			ObjectRef.ObjectAttributesSetRef() &= ~SV_ARCHIVABLE_IMAGE;
		}

		m_List.erase( SelectedIter );
	}

	ReadSelectedObjects();
}

void SVTADlgArchiveImagePage::BuildImageList()
{
	SVObjectTypeInfoStruct  info;

	SVToolSetClass* pToolSet = dynamic_cast<SVInspectionProcess*>(m_pTool->GetInspection())->GetToolSet();

	BOOL lDone = FALSE;

	info.ObjectType = SVImageObjectType;
	info.SubType = SVNotSetSubObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, pToolSet->GetUniqueObjectID() );

	SVImageListClass ImageList;
	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;
	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		if( nullptr != pImage )
		{
			ImageList.Add(pImage);
		}
	}

	// Set the archivable attributes in images based on.
	m_pTool->SetImageAttributesFromArchiveList(&ImageList);

	SvOsl::SelectorOptions BuildOptions( m_pTool->GetInspection()->GetUniqueObjectID(), SV_ARCHIVABLE_IMAGE );
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
}

void SVTADlgArchiveImagePage::MemoryUsage()
{
	m_mapSelectedImageMemUsage.clear();

	SvOsl::SelectorItemVector::const_iterator Iter;
	for ( Iter = m_List.begin(); m_List.end() != Iter ; ++Iter )
	{
		SVGUID ObjectGuid( Iter->getItemKey() );

		SVObjectClass* pObject( nullptr );
		SVObjectManagerClass::Instance().GetObjectByIdentifier( ObjectGuid, pObject );

		if( nullptr != pObject )
		{
			SVImageClass* pImage = dynamic_cast <SVImageClass*> ( pObject );
			if ( nullptr != pImage )
			{
				m_mapSelectedImageMemUsage[ pImage ] = SVArchiveTool::CalculateImageMemory( pImage );
			}
		}
	}
	}


void SVTADlgArchiveImagePage::ReadSelectedObjects()
{
	m_ItemsSelected.DeleteAllItems();

	MemoryUsage();
	if( SVArchiveGoOffline == m_eSelectedArchiveMethod )
	{
		CalculateFreeMem();
	}

	CString Prefix;
	CString ToolName;
	ToolName.LoadString( IDS_CLASSNAME_SVTOOLSET );
	Prefix.Format( _T("%s.%s."), m_pTool->GetInspection()->GetName(), ToolName );

	int Index = 0;
	SvOsl::SelectorItemVector::const_iterator Iter;
	for ( Iter = m_List.begin(); m_List.end() != Iter ; ++Iter )
	{
		SVString Name;
		Name = Iter->getLocation();
		SvUl_SF::searchAndReplace( Name, Prefix, _T("") );

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
	SVString InspectionName( m_pTool->GetInspection()->GetName() );
	SVGUID InspectionGuid( m_pTool->GetInspection()->GetUniqueObjectID() );

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );

	SvOsl::SelectorOptions BuildOptions( InspectionGuid, SV_ARCHIVABLE_IMAGE );
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
		//We need to check the memory requirements for the images
		const SvOsl::SelectorItemVector& rModifiedObjects = SvOsl::ObjectTreeGenerator::Instance().getModifiedObjects();
		SvOsl::SelectorItemVector::const_iterator Iter;
		for(Iter = rModifiedObjects.begin(); rModifiedObjects.end() != Iter; ++Iter )
		{
			SVGUID ImageGuid( Iter->getItemKey() );
			if( !checkImageMemory( ImageGuid, Iter->isSelected() ) )
			{
				//If memory check failed then the original image selection remains
				return;
			}
		}

		m_List = SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects();

		ReadSelectedObjects();

		//We need to remove unselected objects attributes
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
					ObjectRef.ObjectAttributesSetRef() &= ~SV_ARCHIVABLE_IMAGE;
				}
			}
		}
	}
}

void SVTADlgArchiveImagePage::OnBrowse() 
{
	SVFileNameClass	svfncImageFolder;

	// Set the default folder to start with.
	CString csInitialPath(SvStl::GlobalPath::Inst().GetTempPath(_T("\\")).c_str());

	m_ImageFilesRoot.GetWindowText(csInitialPath);
	if (!csInitialPath.IsEmpty())
		svfncImageFolder.SetPathName(csInitialPath);

	ArchiveToolHelper athImagePath;
	athImagePath.Init(SVString(csInitialPath)); 

	bool bUsingKeywords = athImagePath.isUsingKeywords();
	if (bUsingKeywords)
	{
		if (athImagePath.isTokensValid())
		{
			csInitialPath = athImagePath.TranslatePath(SVString(csInitialPath)).c_str();
		}
		else
		{
			//don't allow to exit with invalid path
			SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
			Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_InvalidImagePath, SvStl::SourceFileParams(StdMessageParams) );
			return;
		}
	}
    //
    // Select the folder to copy to..
    //
	svfncImageFolder.SetFileType(SV_DEFAULT_FILE_TYPE);
	if (svfncImageFolder.SelectPath())
    {
		csInitialPath = svfncImageFolder.GetPathName();
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
	if (iSel != CB_ERR)
	{
		m_eSelectedArchiveMethod = static_cast <SVArchiveMethodEnum> (m_Mode.GetItemData( iSel ));
		m_wndAvailableArchiveImageMemory.ShowWindow( SVArchiveGoOffline == m_eSelectedArchiveMethod );

		//if changing to SVArchiveGoOffline mode - build m_mapSelectedImageUsage with selected items in the tree
		if (SVArchiveGoOffline == m_eSelectedArchiveMethod)
		{
			MemoryUsage();
			//check to make sure they did not go over the available memory
			__int64 FreeMem = CalculateFreeMem();
			if ( 0 > FreeMem )
			{
				SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_AP_NotEnoughMemoryInChangeMode, SvStl::SourceFileParams(StdMessageParams) );
			}
		}
		else if(SVArchiveSynchronous == m_eSelectedArchiveMethod )
		{
			SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
			Exception.setMessage( SVMSG_SVO_89_ARCHIVE_TOOL_SYNCHRONOUS_MODE, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		}
	}
}

void SVTADlgArchiveImagePage::OnChangeEditMaxImages() 
{
	CString strNumImages;
	m_MaxImages.GetWindowText(strNumImages);
	m_ImagesToArchive = atol(strNumImages);

	if (!m_Init)
	{
		//check to make sure they don't type in a value below 1
		if ( 0 < m_ImagesToArchive )
		{  
			//check to make sure we don't go over the amount of free memory
			if (SVArchiveGoOffline == m_eSelectedArchiveMethod)
			{
				__int64 llFreeMem = CalculateFreeMem();
				if( 0 <= llFreeMem )
				{
					m_sMaxImageNumber = strNumImages;
					m_ImagesToArchive = atol(strNumImages);
				}
				else
				{
					SVStringArray msgList;
					msgList.push_back(SVString(strNumImages));
					SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
					Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_AP_NotEnoughImageMemoryInChangeMode, msgList, SvStl::SourceFileParams(StdMessageParams) );
					m_ImagesToArchive = atol(m_sMaxImageNumber);
					if(m_sMaxImageNumber != strNumImages)
					{
						m_MaxImages.SetWindowText((LPCSTR)m_sMaxImageNumber);
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
			m_MaxImages.SetWindowText(m_sMaxImageNumber);
		}
	}
}

bool SVTADlgArchiveImagePage::checkImageMemory( SVGUID ImageGuid , bool bNewState )
{
	UpdateData();
	bool bOk = true;

	SVObjectClass* pObject( nullptr );
	SVObjectManagerClass::Instance().GetObjectByIdentifier( ImageGuid, pObject );
	SVImageClass* pImage = dynamic_cast <SVImageClass*> ( pObject );
	ASSERT(pImage);

	//Get amount of memory needed for the selected image.
	long MemoryForSelectedImage = SVArchiveTool::CalculateImageMemory( pImage );
	MemoryForSelectedImage *= m_ImagesToArchive;

	if( bNewState )// want to select
	{	
		bool bAddItem = true;

		//only check for memory if in mode SVArchiveGoOffline
		if (SVArchiveGoOffline == m_eSelectedArchiveMethod)
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
				bCanReserve = TheSVMemoryManager().CanReservePoolMemory( SvO::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, lDelta );
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
				SVStringArray msgList;
				msgList.push_back(SVString(pImage->GetCompleteObjectName()));
				SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_AP_NotEnoughImageMemoryToSelect, msgList, SvStl::SourceFileParams(StdMessageParams) );
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
		if (SVArchiveGoOffline == m_eSelectedArchiveMethod)
		{
			__int64 FreeMem = CalculateFreeMem();

			if( 0 > FreeMem )
			{
				SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvOi::Tid_AP_NotEnoughMemoryPleaseDeselectImage, SvStl::SourceFileParams(StdMessageParams) );

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
		CString AvailableMemory;
		AvailableMemory.Format( SvO::AvailableArchiveImageMemory, (double) FreeMem / (double) (1024 * 1024) );
		m_wndAvailableArchiveImageMemory.SetWindowText( AvailableMemory );
	}

	return FreeMem;
}

