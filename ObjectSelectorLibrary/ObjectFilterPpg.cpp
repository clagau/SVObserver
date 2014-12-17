//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectFilterPpg
//* .File Name       : $Workfile:   ObjectFilterPpg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   16 Dec 2014 17:34:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "GridCtrlLibrary\GridCellCheck.h"
#include "ObjectFilterPpg.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::ObjectSelectorLibrary;
using namespace Seidenader::SVTreeLibrary;
using namespace Seidenader::GridCtrlLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//IMPLEMENT_DYNCREATE(ObjectFilterPpg, CPropertyPage)
#pragma endregion Declarations

BEGIN_MESSAGE_MAP(ObjectFilterPpg, CPropertyPage)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_HELPINFO()
	ON_NOTIFY(NM_CLICK, IDC_GRID_CONTROL, OnGridClick)
	ON_NOTIFY(NM_RCLICK, IDC_GRID_CONTROL, OnGridRClick)
	ON_EN_CHANGE(IDC_EDIT_FILTER_NAME, OnChangeFilter)
	ON_EN_CHANGE(IDC_EDIT_FILTER_LOCATION, OnChangeFilter)
	ON_CBN_SELCHANGE(IDC_COMBO_FILTER_CHECK, OnChangeFilter)
	ON_CBN_SELCHANGE(IDC_COMBO_FILTER_TYPE, OnChangeFilter)
	ON_BN_CLICKED(IDC_RESET_BUTTON, OnResetFilter)
	ON_COMMAND( ID_TREE_CHECKALL, OnCheckAll )
	ON_COMMAND( ID_TREE_UNCHECKALL, OnUncheckAll )
END_MESSAGE_MAP()

#pragma region Constructor
ObjectFilterPpg::ObjectFilterPpg( ObjectTreeItems& treeContainer, const SVString& title, bool singleSelect /*= true*/ )
	: CPropertyPage( ObjectFilterPpg::IDD )
	, m_rTreeContainer( treeContainer )
	, m_SingleSelect( singleSelect )
	, m_RowOfCurrentSelection( -1 )
{
	m_psp.pszTitle = title.c_str();
	m_psp.dwFlags |= PSP_USETITLE;
}

ObjectFilterPpg::~ObjectFilterPpg()
{
}
#pragma endregion Constructor

#pragma region Protected Methods
void ObjectFilterPpg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID_CONTROL, m_Grid);
	DDX_Control(pDX, IDC_EDIT_FILTER_NAME, m_FilterNameControl);
	DDX_Control(pDX, IDC_EDIT_FILTER_LOCATION, m_FilterLocationControl);
	DDX_Control(pDX, IDC_COMBO_FILTER_CHECK, m_checkedControl);
	DDX_Control(pDX, IDC_COMBO_FILTER_TYPE, m_TypeControl);
}

BOOL ObjectFilterPpg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	setResizeControls();

	m_StateImageList.Create( 16, 16, TRUE, 16, 4 ); // BRW - Explain the reasons for these numbers.

	for(int i = IDI_EMPTY_ENABLED; i <= IDI_TRI_STATE_DISABLED; i++)
	{
		m_StateImageList.Add( AfxGetApp()->LoadIcon( i ) );
	}

	m_checkedControl.AddString( _T( "All" ) ); // BRW - Move these to a string table in SVOResource.
	m_checkedControl.AddString( _T( "Checked" ) ); // BRW - Move these to a string table in SVOResource.
	m_checkedControl.AddString( _T( "Unchecked" ) ); // BRW - Move these to a string table in SVOResource.
	m_checkedControl.SetCurSel(0);

	std::set<SVString> typeSet;
	ObjectTreeItems::SVTree_pre_order_iterator Iter = m_rTreeContainer.pre_order_begin();
	while( m_rTreeContainer.pre_order_end() != Iter )
	{
		if( Iter->second.isLeaf() )
		{
			typeSet.insert(Iter->second.getItemTypeName());
		}
		++Iter;
	}
	m_TypeControl.AddString( _T( "All" ) ); // BRW - Move these to a string table in SVOResource.
	for(std::set<SVString>::const_iterator iter=typeSet.cbegin(); iter != typeSet.cend(); ++iter)
	{
		m_TypeControl.AddString((*iter).c_str());
	}
	m_TypeControl.SetCurSel(0);
	m_FilterNameControl.setHelpText( _T("Enter Name") ); // BRW - Move these to a string table in SVOResource.
	m_FilterLocationControl.setHelpText( _T("Enter Location") ); // BRW - Move these to a string table in SVOResource.

	UpdateData(FALSE);

	return TRUE;
}

BOOL ObjectFilterPpg::OnSetActive()
{
	loadGridCtrl();

	return CPropertyPage::OnSetActive();
}

void ObjectFilterPpg::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	//Resize all controls
	m_Resizer.Resize(this);
}

void ObjectFilterPpg::OnHelp()
{
	AfxGetApp()->HtmlHelp( IDD + 0x80000 );
}

BOOL ObjectFilterPpg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	pHelpInfo->iCtrlId = IDD + 0x80000;
	AfxGetApp()->HtmlHelp( pHelpInfo->iCtrlId,HH_HELP_CONTEXT );
	return TRUE ; 
}

void ObjectFilterPpg::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	/*SvGCL::*/NM_GRIDVIEW* pItem = (/*SvGCL::*/NM_GRIDVIEW*) pNotifyStruct;

	if (CheckedColumn == pItem->iColumn)
	{
		/*SvGCL::*/CGridCellCheck* cell = dynamic_cast</*SvGCL::*/CGridCellCheck*>(m_Grid.GetCell(pItem->iRow, CheckedColumn));
		if (nullptr != cell)
		{
			CString location = m_Grid.GetItemText(pItem->iRow, LocationColumn);
			bool isChecked = (TRUE == cell->GetCheck());
			changeCheckState(location, isChecked, pItem->iRow);
		}
	}
}

void ObjectFilterPpg::OnGridRClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	/*SvGCL::*/NM_GRIDVIEW* pItem = (/*SvGCL::*/NM_GRIDVIEW*) pNotifyStruct;

	CPoint p;
	if (GetCursorPos(&p))
	{
		ScreenToClient(&p);
		showContextMenu( p );
	}
}

void ObjectFilterPpg::OnChangeFilter()
{
	UpdateData(TRUE);
	loadGridCtrl();
}

void ObjectFilterPpg::OnResetFilter()
{
	m_FilterNameControl.setEditText(_T(""));
	m_FilterLocationControl.setEditText(_T(""));
	m_checkedControl.SetCurSel(0);
	m_TypeControl.SetCurSel(0);
	UpdateData(FALSE);
	OnChangeFilter();
}

void ObjectFilterPpg::OnRButtonDown( UINT Flags, CPoint Point ) 
{
	showContextMenu( Point );
	CPropertyPage::OnRButtonDown( Flags, Point );
}

void ObjectFilterPpg::OnCheckAll()
{
	checkAllItem(true);
}

void ObjectFilterPpg::OnUncheckAll()
{
	checkAllItem(false);
}

void ObjectFilterPpg::checkAllItem(bool shouldCheck)
{
	int count = m_Grid.GetRowCount();

	//First line is header, start with second line
	for (int i=1; i < count; i++)
	{
		/*SvGCL::*/CGridCellCheck* cell = dynamic_cast</*SvGCL::*/CGridCellCheck*>(m_Grid.GetCell(i, CheckedColumn));
		if (nullptr != cell)
		{
			CString location = m_Grid.GetItemText(i, LocationColumn);
			bool isChecked = (TRUE == cell->GetCheck());
			if (isChecked != shouldCheck)
			{
				changeCheckState(location, shouldCheck, i);
			}
		}
	}
	loadGridCtrl();
}

void ObjectFilterPpg::setResizeControls()
{
	m_Resizer.Add(this, IDC_GRID_CONTROL, RESIZE_LOCKLEFT | RESIZE_LOCKRIGHT | RESIZE_LOCKBOTTOM | RESIZE_LOCKTOP );
	m_Resizer.Add(this, IDC_RESET_BUTTON, RESIZE_LOCKLEFT | RESIZE_LOCKBOTTOM );
}

void ObjectFilterPpg::loadGridCtrl()
{
	int nameColumnWidth = 150;
	int locationColumnWidth = 290;
	int checkedColumnWidth = 50;
	int typeColumnWidth = 72;
	if (4 == m_Grid.GetColumnCount())
	{
		nameColumnWidth = m_Grid.GetColumnWidth(NameColumn);
		locationColumnWidth = m_Grid.GetColumnWidth(LocationColumn);
		checkedColumnWidth = m_Grid.GetColumnWidth(CheckedColumn);
		typeColumnWidth = m_Grid.GetColumnWidth(TypeColumn);
	}

	m_Grid.SetRedraw(FALSE);
	m_Grid.AutoSize(GVS_DATA);
	m_Grid.SetColumnCount(4);
	m_Grid.SetFixedRowSelection(TRUE);
	m_Grid.SetFixedRowCount(1);
	m_Grid.SetFixedColumnSelection(FALSE);
	m_Grid.SetColumnWidth(NameColumn, nameColumnWidth);
	m_Grid.SetColumnWidth(LocationColumn, locationColumnWidth);
	m_Grid.SetColumnWidth(CheckedColumn, checkedColumnWidth);
	m_Grid.SetColumnWidth(TypeColumn, typeColumnWidth);

	//add header
	GV_ITEM Item;
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = NameColumn;
	Item.strText = _T("Name"); // BRW - Move these to a string table in SVOResource.
	m_Grid.SetItem(&Item);
	Item.col = LocationColumn;
	Item.strText = _T("Location"); // BRW - Move these to a string table in SVOResource.
	m_Grid.SetItem(&Item);
	Item.col = CheckedColumn;
	Item.strText = _T("Check"); // BRW - Move these to a string table in SVOResource.
	m_Grid.SetItem(&Item);
	Item.col = TypeColumn;
	Item.strText = _T("Type"); // BRW - Move these to a string table in SVOResource.
	m_Grid.SetItem(&Item);

	//add leaves
	int i = 1;
	m_Grid.SetRowCount(i);
	SVString filterNameUpper(m_FilterNameControl.getEditText());
	SVString filterLocationUpper(m_FilterLocationControl.getEditText());
	filterNameUpper.MakeUpper();
	filterLocationUpper.MakeUpper();
	ObjectTreeItems::SVTree_pre_order_iterator Iter = m_rTreeContainer.pre_order_begin();
	while( m_rTreeContainer.pre_order_end() != Iter )
	{
		if( Iter->second.isLeaf() )
		{
			int checkSelection = m_checkedControl.GetCurSel();
			int typeSelection = m_TypeControl.GetCurSel();
			SVString nameUpper(Iter->second.getName());
			SVString locationUpper(Iter->second.getDisplayLocation());
			nameUpper.MakeUpper();
			locationUpper.MakeUpper();
			bool isNameValid = nameUpper.isSubmatch(filterNameUpper);
			bool isLocationValid = locationUpper.isSubmatch(filterLocationUpper);
			CString typeText = _T("");
			m_TypeControl.GetLBText(typeSelection, typeText);
			if ( (isNameValid && isLocationValid) &&
				 (0 == checkSelection || 
					( 1 == checkSelection && IObjectSelectorItem::CheckedEnabled == Iter->second.getCheckedState()) || 
					( 2 == checkSelection && IObjectSelectorItem::CheckedEnabled != Iter->second.getCheckedState()) ) &&
				 (0 == typeSelection || Iter->second.getItemTypeName().c_str() == typeText ))
			{
				m_Grid.SetRowCount(i + 1);
				m_Grid.SetItemText(i, NameColumn, Iter->second.getName().c_str());
				m_Grid.SetItemState(i, NameColumn, m_Grid.GetItemState(i, 0) | GVIS_READONLY);
				m_Grid.SetItemText(i, LocationColumn, Iter->second.getDisplayLocation().c_str());
				m_Grid.SetItemState(i, LocationColumn, m_Grid.GetItemState(i, LocationColumn) | GVIS_READONLY);
				m_Grid.SetCellType(i, CheckedColumn, /*RUNTIME_CLASS_N(SvGCL, CGridCellCheck)*/RUNTIME_CLASS(CGridCellCheck));
				/*SvGCL::*/CGridCellCheck* cell = dynamic_cast</*SvGCL::*/CGridCellCheck*>(m_Grid.GetCell(i, CheckedColumn));
				if (nullptr != cell)
				{
					if (IObjectSelectorItem::CheckedEnabled == Iter->second.getCheckedState())
					{
						cell->SetCheck( TRUE );
						if (m_SingleSelect)
						{
							m_RowOfCurrentSelection = i;
						}
					}
					else
					{
						cell->SetCheck( FALSE );
					}
				}

				m_Grid.SetItemText(i, TypeColumn, Iter->second.getItemTypeName().c_str());
				m_Grid.SetItemState(i, TypeColumn, m_Grid.GetItemState(i, TypeColumn) | GVIS_READONLY);
				i++;
			}
		}
		++Iter;
	}

	m_Grid.SetRedraw(TRUE, TRUE);
}

void ObjectFilterPpg::showContextMenu( CPoint point )
{
	if ( !m_SingleSelect )
	{
		CMenu menu;
		CMenu* pPopupMenu;

		UINT menuResource = 0;

		menuResource = IDR_TREE_ALL_VALUES_MENU;

		if( 0 != menuResource && menu.LoadMenu( menuResource ) )
		{
			if( pPopupMenu = menu.GetSubMenu( 0 ) )
			{
				ClientToScreen( &point );
				pPopupMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
			}
		}
	}
}

void Seidenader::ObjectSelectorLibrary::ObjectFilterPpg::changeCheckState( const CString &location, bool isChecked, int rowNumber )
{
	ObjectTreeItems::iterator iter = m_rTreeContainer.findItem(location);
	if( m_rTreeContainer.end() != iter )
	{
		if (iter->second.isLeaf() && 
			(iter->second.getCheckedState() == IObjectSelectorItem::CheckedEnabled) != isChecked)
		{
			if (m_SingleSelect)
			{
				//remove selection from old leaf
				if (m_RowOfCurrentSelection >= 0)
				{
					//remove selection in tree
					CString location = m_Grid.GetItemText(m_RowOfCurrentSelection, LocationColumn);
					m_rTreeContainer.clearItem(location);
					//remove selection in grid
					/*SvGCL::*/CGridCellCheck* cellOld = dynamic_cast</*SvGCL::*/CGridCellCheck*>(m_Grid.GetCell(m_RowOfCurrentSelection, CheckedColumn));
					if (nullptr != cellOld)
					{
						cellOld->SetCheck(FALSE);
					}
				}

				//set currentSelection to new checked leaf
				if (isChecked)
				{
					m_RowOfCurrentSelection = rowNumber;
				}
				else
				{
					m_RowOfCurrentSelection = -1;
				}
			}

			iter->second.setCheckedState(isChecked?IObjectSelectorItem::CheckedEnabled:IObjectSelectorItem::UncheckedEnabled);
			m_rTreeContainer.setParentState(iter);
		}
	}
}
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectFilterPpg.cpp_v  $
 * 
 *    Rev 1.0   16 Dec 2014 17:34:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */