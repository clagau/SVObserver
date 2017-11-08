/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This class is used as a property page for different sheets 
/// It lists selected objects and uses the object selector to change the selection
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SelectedObjectsPage.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVOResource\ConstGlobalSvOr.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SelectedObjectsPage, CPropertyPage)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_SELECTED, OnDblClickListSelected)
	ON_BN_CLICKED(IDC_SELECT_BUTTON, OnSelectObjects)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnRemoveItem)
	ON_BN_CLICKED(IDC_BTN_CLEAR_ALL, OnRemoveAllItems)
END_MESSAGE_MAP()

static const int ItemSelectedCol = 1;
static const int ItemsSelectedWidth = 500;
static const int IconNumber = 1;
static const int IconGrowBy = 1;
#pragma endregion Declarations

#pragma region Constructor
SelectedObjectsPage::SelectedObjectsPage( const std::string& rInspectionName, const SVGUID& rInspectionID, LPCTSTR Caption, const SvOsl::SelectorItemVector& rList, UINT AttributeFilters, int id )
: CPropertyPage(id)
, m_InspectionName( rInspectionName )
, m_InspectionID ( rInspectionID )
, m_List( rList )
, m_AttributeFilter( AttributeFilters )
{
	m_strCaption = Caption;
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;
}

SelectedObjectsPage::~SelectedObjectsPage()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool SelectedObjectsPage::QueryAllowExit()
{
	return true;
}

BOOL SelectedObjectsPage::OnSetActive()
{
	UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

BOOL SelectedObjectsPage::OnKillActive()
{
	UpdateData(TRUE);

	return CPropertyPage::OnKillActive();
}
#pragma endregion Public Methods

#pragma region Private Methods
void SelectedObjectsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SELECTED, m_ItemsSelected);
	DDX_Control(pDX, IDC_SELECT_BUTTON, m_Select);
}

BOOL SelectedObjectsPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_ItemsSelected.InsertColumn( ItemSelectedCol, _T(""), 0, ItemsSelectedWidth );
	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR24 | ILC_MASK, IconNumber, IconGrowBy );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_CHECKED_ENABLED ) );
	m_ItemsSelected.SetImageList( &m_StateImageList, LVSIL_STATE );

	ReadSelectedObjects();

	m_TreeBitmap.LoadBitmap( IDB_TREE );
	m_Select.SetBitmap( static_cast<HBITMAP> (m_TreeBitmap.GetSafeHandle()) );
	
	return TRUE;
}

void SelectedObjectsPage::OnDblClickListSelected( NMHDR *pNMHDR, LRESULT *pResult )
{
	ShowObjectSelector();
}

void SelectedObjectsPage::OnSelectObjects()
{
	ShowObjectSelector();
}

void SelectedObjectsPage::OnRemoveAllItems()
{
	m_List.clear();
	ReadSelectedObjects();
}

void SelectedObjectsPage::OnRemoveItem()
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
		m_List.erase( m_List.begin() + *Iter );
	}

	ReadSelectedObjects();
}

void SelectedObjectsPage::ReadSelectedObjects()
{
	m_ItemsSelected.DeleteAllItems();

	std::string Prefix = m_InspectionName;
	Prefix += _T(".Tool Set.");

	int Index = 0;
	SvOsl::SelectorItemVector::const_iterator Iter;
	for ( Iter = m_List.begin(); m_List.end() != Iter ; ++Iter )
	{
		std::string Name;
		Name = Iter->getLocation();
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

void SelectedObjectsPage::ShowObjectSelector()
{
	UINT AttributeFilters( m_AttributeFilter );

	//For values and conditions only use viewable objects
	switch( AttributeFilters )
	{
	case SvDef::SV_DD_VALUE:
		{
			AttributeFilters |= SvDef::SV_VIEWABLE;
		}
		break;
	default:
		break;
	}

	std::string InspectionName( m_InspectionName );
	SVGUID InspectionGuid( m_InspectionID );

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, std::string( _T("") ) );

	SvOsl::SelectorOptions BuildOptions( InspectionGuid, AttributeFilters );
	SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );

	SvOsl::SelectorItemVector::const_iterator Iter;
	SvDef::StringSet CheckItems;
	for ( Iter = m_List.begin(); m_List.end() != Iter ; ++Iter )
	{
		std::string ObjectName;
		ObjectName = Iter->getLocation();
		CheckItems.insert( ObjectName );
	}
	SvOsl::ObjectTreeGenerator::Instance().setCheckItems( CheckItems );

	std::string Title = SvUl::Format( _T("%s - %s"), m_strCaption, InspectionName.c_str() );
	std::string Filter = SvUl::LoadStdString( IDS_FILTER );
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), m_strCaption, Filter.c_str(), this );

	if( IDOK == Result )
	{
		m_List = SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects();

		ReadSelectedObjects();
	}
}
#pragma endregion Private Methods
