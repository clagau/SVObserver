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
#include <numeric>
#include "SVTADlgArchiveImagePage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVObjectLibrary/SVGetObjectDequeByTypeVisitor.h"
#include "SVArchiveTool.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
#include "SVIPDoc.h"
#include "SVImageClass.h"
#include "SVInspectionProcess.h"
#include "SVMemoryManager.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"
#include "SVArchiveHeaderEditDlg.h"
#include "ArchiveToolHelper.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/MessageManagerResource.h"
#include "SVOResource\ConstGlobalSvOr.h"
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
static const int IconGrowBy = 2;
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
		SVObjectListClass ObjectList;
		int iSize = ObjectList.GetSize();
		if( 0 < iSize )
		{
			//if memory usage < 0 do not all them to exit
			__int64 FreeMemory = CalculateFreeMem();
			if (0 > FreeMemory)
			{
				SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvO::AP_NotEnoughMemoryPleaseDeselect, StdMessageParams );
				return false;
			}
		}
	}

	//update the image path
	CString csImageFolder;
	m_ImageFilesRoot.GetWindowText( csImageFolder );

	CString sTmpArchiveImagePath = csImageFolder;
	ArchiveToolHelper athImagePath;
	athImagePath.Init(csImageFolder);

	if (athImagePath.isUsingKeywords())
	{
		if (athImagePath.isTokensValid())
		{
			sTmpArchiveImagePath = athImagePath.TranslatePath(csImageFolder).c_str();
			SVCheckPathDir( sTmpArchiveImagePath, TRUE );
		}
		else
		{
			//don't allow to exit with invalid path
			SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
			Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvO::InvalidImagePath, StdMessageParams );
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
		CString temp;
		temp.Format ("Invalid drive:  %s", szDrive);
		SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, temp.GetString(), StdMessageParams );
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
		SVString svTemp;
		svTemp.Format(	SvO::Error_you_have_Selected_X_Must_less_then_Y, dwTemp, UpperLimitImageNumbers);
		SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, svTemp.c_str(), StdMessageParams, SvOi::Err_16075_ImageNrToBig  );
		return FALSE;

	}

	if( MaxImagesWarningLimit < dwTemp )
	{
		CString csTemp;
		csTemp.Format(	_T("WARNING: You have selected %ld for the Max Images count"), dwTemp);
		SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, csTemp.GetString(), StdMessageParams );
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
	SVIPDoc* pIPDoc = NULL;
	pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( m_pTool->GetInspection()->GetUniqueObjectID() );

	if( nullptr != pIPDoc )
	{
		pIPDoc->SetModifiedFlag();
	}

	HRESULT hRet = m_pTool->ValidateArchiveTool();

	SVSendMessage( m_pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL );

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
	SVToolSetClass* pToolSet = m_pTool->GetInspection()->GetToolSet();

	DWORD dwTemp=0;
    m_pTool->m_dwArchiveMaxImagesCount.GetValue( dwTemp );
	m_ImagesToArchive = dwTemp;
	CString s;
	s.Format(_T("%ld"),dwTemp);
	m_MaxImages.SetWindowText((LPCTSTR)s);

	//store the MaxImageNumber
	m_sMaxImageNumber = s;

	__int64 MemUsed = TheSVMemoryManager().ReservedBytes( ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME );
 	m_ToolImageMemoryUsage = 0;
	m_TotalArchiveImageMemoryAvailable = TheSVMemoryManager().SizeOfPoolBytes( ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME );

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
	if( SVArchiveGoOffline == m_eSelectedArchiveMethod )
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

	SVToolSetClass* pToolSet = m_pTool->GetInspection()->GetToolSet();

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
	SvOg::ToolSetItemSelector<SvOg::AttributeSetFilter> toolsetItemSelector;
	SvOi::ISelectorItemVectorPtr pToolsetList =  toolsetItemSelector( BuildOptions );
	//Copy list to member variable for easier use
	if( !pToolsetList.empty() )
	{
		SvOsl::SelectorItemVector* pSelectorList = dynamic_cast<SvOsl::SelectorItemVector*> (pToolsetList.get());
		if( nullptr != pSelectorList )
		{
			m_List = *pSelectorList;
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
		Name.replace( Prefix, _T("") );

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

	SVString Title;
	CString Filter;
	Title.Format( _T("%s - %s"), m_strCaption, InspectionName.c_str() );
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
	CString csInitialPath(_T("C:\\Temp\\"));

	m_ImageFilesRoot.GetWindowText(csInitialPath);
	if (!csInitialPath.IsEmpty())
		svfncImageFolder.SetPathName(csInitialPath);

	ArchiveToolHelper athImagePath;
	athImagePath.Init(csInitialPath); 

	bool bUsingKeywords = athImagePath.isUsingKeywords();
	if (bUsingKeywords)
	{
		if (athImagePath.isTokensValid())
		{
			csInitialPath = athImagePath.TranslatePath(csInitialPath).c_str();
		}
		else
		{
			//don't allow to exit with invalid path
			SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
			Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvO::InvalidImagePath, StdMessageParams );
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
		if( SVArchiveGoOffline == m_eSelectedArchiveMethod)
		{
			MemoryUsage();
			//check to make sure they did not go over the available memory
			__int64 FreeMem = CalculateFreeMem();
			if ( 0 > FreeMem )
			{
				SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvO::AP_NotEnoughMemoryInChangeMode, StdMessageParams );
			}
		}
		else if(SVArchiveSynchronous == m_eSelectedArchiveMethod )
		{
			SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
			Exception.setMessage( SVMSG_SVO_89_ARCHIVE_TOOL_SYNCHRONOUS_MODE, nullptr, StdMessageParams );
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
			if( SVArchiveGoOffline == m_eSelectedArchiveMethod )
			{
				__int64 llFreeMem = CalculateFreeMem();
				if( 0 <= llFreeMem )
				{
					m_sMaxImageNumber = strNumImages;
					m_ImagesToArchive = atol(strNumImages);
				}
				else
				{
					CString sMsg;
					sMsg.Format("There is not enough Available Archive Image Memory for %s images in Change Mode. Available\nArchive Image Memory is the result of the selected images and the Max Images number.\nThe selection will be reset.",strNumImages);
					SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
					Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, sMsg.GetString(), StdMessageParams );
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
		if( SVArchiveGoOffline == m_eSelectedArchiveMethod )
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
				bCanReserve = TheSVMemoryManager().CanReservePoolMemory( ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, lDelta );
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
				CString strMessage;
				strMessage.Format(_T("Not enough Archive Image Memory to select %s"), pImage->GetCompleteObjectName());
				SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, strMessage.GetString(), StdMessageParams );
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
		if( SVArchiveGoOffline == m_eSelectedArchiveMethod )
		{
			__int64 FreeMem = CalculateFreeMem();

			if( 0 > FreeMem )
			{
				SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
				Exception.setMessage( SVMSG_SVO_73_ARCHIVE_MEMORY, SvO::AP_NotEnoughMemoryPleaseDeselectImage, StdMessageParams );

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolArchivePage.cpp_v  $
 * 
 *    Rev 1.15   10 Sep 2014 09:36:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SetValue to OnBnClickedHeaderCheck to store value in the tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   10 Sep 2014 06:34:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed archive tool header check box check in OnInitDialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   02 Sep 2014 11:23:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  914
 * SCR Title:  Fix negative memory issues with the Archive Tool when in change mode
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Revised Browse2 method to check the string length before indexing
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   28 Aug 2014 07:25:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  916
 * SCR Title:  Fix issue with available memory calculation with Archive Tool (SV0-350)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added member variable m_bInit.  When the dialog is being initialized do not display error messages in method  OnChangeEditMaxImages
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   21 Aug 2014 11:08:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  916
 * SCR Title:  Fix issue with available memory calculation with Archive Tool (SV0-350)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issue with one an error message and also with being able to put in 0 for max number of images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   12 Aug 2014 14:28:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  916
 * SCR Title:  Fix issue with available memory calculation with Archive Tool (SV0-350)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update text for error messages
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   12 Aug 2014 06:47:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added click callback to update header button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Aug 2014 09:27:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  916
 * SCR Title:  Fix issue with available memory calculation with Archive Tool (SV0-350)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed OnChangeEditMaxImages so it would not write back the number if it was the same.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   25 Jul 2014 15:14:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  923
 * SCR Title:  Fix the default file selection for the Archive Tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   when browsing for the archive file, start with the path that is in the edit field.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   23 Jul 2014 11:42:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  916
 * SCR Title:  Fix issue with available memory calculation with Archive Tool (SV0-350)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   OnInitDialog to call the Archive Tool Reset Object.
 * BuildImageList - removed old code.
 * QueryAllowExit - show message and don't allow to exit if the memory is negative.
 * CanSelectObjectCallback - changed logic to calculate memory correctly.
 * OnSelchangeCombo - When switching to Change Mode, calculate memory and show message if memory goes negative based on selections.
 * OnChangeEditMaxImages - show message if memory goes negative.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   26 Jun 2014 07:07:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified GetSelectedHeaderNamePairs to use the Tree Order function from the object selector.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   25 Jun 2014 13:01:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug in GetSelectedHeaderNamePairs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2014 14:43:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Nov 2013 12:44:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added static_casts to build without warnings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Nov 2013 07:38:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added header support functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:41:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   04 Oct 2012 11:00:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   30 Jul 2012 13:08:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   27 Jul 2012 09:05:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   02 Jul 2012 17:50:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   27 Jan 2011 12:03:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   08 Dec 2010 13:45:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   21 Jun 2007 15:03:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   11 Apr 2006 15:14:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  559
 * SCR Title:  Fix Archive Tool dialog bug
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed image selection bug - memory calculation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   25 Jan 2006 12:40:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified to match change in SVOutputInfoListTreeCtrl
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   13 Oct 2005 14:04:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed index 1 from GetValue calls.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   06 Oct 2005 07:47:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem of incorrect Change Mode memory usage reported in Archive page
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   08 Sep 2005 13:09:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed ArchiveMethod from a DWord value object to an Enumerate value object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   06 Sep 2005 08:42:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed initialization bug
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   31 Aug 2005 10:14:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Modified text descriptions on dialog and in "when to archive" drop list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   25 Aug 2005 15:00:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed uninitialized variable problem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   18 Aug 2005 08:33:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   change "Going offline" to "Leave Run Mode"
 * change "Online asynchronous" to "Run Mode Asynchronous"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   09 Aug 2005 08:49:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   08 Aug 2005 15:20:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed display of free mem on change max images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   08 Aug 2005 14:00:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renamed SVToolArchive to SVArchiveTool
 * implemented usage tracking
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   03 Aug 2005 14:56:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   change ArchivePageOK to QueryAllowExit
 * create paths on exit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   20 Jun 2005 10:11:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated how the archive enums are referenced
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   20 May 2005 12:29:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added archive mode combo
 * archive member variable name changed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   18 Feb 2005 07:26:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes to support new Reset methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   22 Apr 2003 16:46:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   20 Nov 2002 14:00:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   theSVObjectManager
 * ValueObject Data Indexes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   14 Aug 2001 10:43:14   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  220
 * SCR Title:  Fix invalid path message when adding a Load Image tool
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added code in the SVToolAdjustmentArchivePage::ArchivePageOK() function to
 * validate the drive of the archive text file as well as the archive
 * image file. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   13 Jun 2001 10:26:30   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: OnInitDialog, ArchivePageOK.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   19 Apr 2001 20:00:36   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   11 Apr 2001 16:36:38   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Changes were made in the way stringFileArchivePath and stringImageFileRootPath variables are handled
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   06 Feb 2001 13:47:48   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2.2.0   16 May 2001 14:12:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  202
 * SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Jan 19 2000 15:12:08   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  87
 * SCR Title:  Archive Images - All Max Count greater than 100
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Allow more than 100 images to be archived.  If user selects number of images greater than 100, a dialog is displayed with a warning.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Nov 30 1999 16:33:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  70
 * SCR Title:  Archive Tool fails to save selected toolset image in configuration.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed software to ignore 'attributes allowed' as saved in configuration files for image objects.  This repairs the problem noted by this SCR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:51:46   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   Nov 17 1999 07:25:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added check for unique results archive file paths.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   Nov 10 1999 12:38:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  57
 * SCR Title:  Version 3.00 Beta 18 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Documents marked as changed when Dialog OK buttons pressed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   Oct 26 1999 15:04:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Limit Archived images to a user selectable number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Oct 22 1999 14:24:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Implement the capability to append to results archive, and include a time stamp for first record archived on each 'run'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   Oct 21 1999 17:11:20   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Repair object connections so objects deleted will report their dependencies if selected to be archived.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Oct 20 1999 12:26:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fix bug that causes crash if object with image to be archive  is removed from toolset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Oct 18 1999 12:55:26   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Image file arhive folder selection dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Oct 14 1999 11:29:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Image archive changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Oct 13 1999 14:37:48   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to archive images into .bmp files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Oct 06 1999 15:22:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   More archive work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Oct 05 1999 16:46:52   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Scripting work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Sep 16 1999 13:40:48   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Class name changed from SVArchiveToolClass to SVToolArchive.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 15 1999 15:58:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added a Archive configuration 'page' to the tool adjust property 'sheet'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
