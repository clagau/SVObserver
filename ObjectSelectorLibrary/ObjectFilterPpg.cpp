//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
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
//Moved to precompiled header: #include <set>
#include "ObjectFilterPpg.h"

#include "TextDefinesSvOsl.h"
#include "GridCtrlLibrary\GridCellCheck.h"
#include "SVContainerLibrary/ObjectTreeItems.h"
#include "SVOResource\ConstGlobalSvOr.h"
#include <Shlwapi.h>
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr int FilterColumnNumber = 4;
constexpr int IconNumber = 8;
constexpr int IconGrowBy = 1;
constexpr int NameColWidthDefault = 150;
constexpr int LocationColWidthDefault = 290;
constexpr int CheckColWidthDefault = 50;
constexpr int TypeColWidthDefault = 72;
#pragma endregion Declarations

namespace SvOsl
{
	BEGIN_MESSAGE_MAP(ObjectFilterPpg, CPropertyPage)
		ON_WM_SIZE()
		ON_WM_RBUTTONDOWN()
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
	ObjectFilterPpg::ObjectFilterPpg( SvCl::ObjectTreeItems& treeContainer, LPCTSTR title, bool singleSelect /*= true*/ )
		: CPropertyPage( ObjectFilterPpg::IDD )
		, m_rTreeContainer( treeContainer )
		, m_SingleSelect( singleSelect )
		, m_CheckedLocation( _T("") )
		, m_CheckedRow( -1 ) 
	{
		m_psp.pszTitle = title;
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

		m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR24 | ILC_MASK, IconNumber, IconGrowBy );

		for(int i = IDI_EMPTY_ENABLED; i <= IDI_TRI_STATE_DISABLED; i++)
		{
			m_StateImageList.Add( AfxGetApp()->LoadIcon( i ) );
		}

		m_checkedControl.AddString( FilterAll );
		m_checkedControl.AddString( FilterChecked );
		m_checkedControl.AddString( FilterUnchecked );
		m_checkedControl.SetCurSel(0);

		std::set<std::string> typeSet;
		SvCl::ObjectTreeItems::pre_order_iterator Iter = m_rTreeContainer.pre_order_begin();
		
		while( m_rTreeContainer.pre_order_end() != Iter )
		{
			if( Iter->second->isLeaf() )
			{
				typeSet.insert(Iter->second->m_ItemTypeName);
			}
			++Iter;
		}
		
		m_TypeControl.AddString( FilterAll );
		for(std::set<std::string>::const_iterator iter=typeSet.cbegin(); iter != typeSet.cend(); ++iter)
		{
			m_TypeControl.AddString((*iter).c_str());
		}
		m_TypeControl.SetCurSel(0);
		m_FilterNameControl.setHelpText( FilterEnterNameHelp );
		m_FilterLocationControl.setHelpText( FilterEnterLocationHelp );

		m_initFinished = true;

		UpdateData(FALSE);

		return TRUE;
	}

	BOOL ObjectFilterPpg::OnSetActive()
	{
		loadGridCtrl();
		return CPropertyPage::OnSetActive();
	}

	void ObjectFilterPpg::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;

		if (CheckColumn == pItem->iColumn)
		{
			SvGcl::GridCellCheck* cell = dynamic_cast<SvGcl::GridCellCheck*>(m_Grid.GetCell(pItem->iRow, CheckColumn));
			if (nullptr != cell)
			{
				std::string* pLocation = reinterpret_cast<std::string*> ( m_Grid.GetItemData(pItem->iRow, LocationColumn) );
				if( nullptr != pLocation )
				{
					bool isChecked = (TRUE == cell->GetCheck());
					changeCheckState( *pLocation, isChecked, pItem->iRow );
				}
			}
		}
	}

	void ObjectFilterPpg::OnGridRClick(NMHDR *, LRESULT* /*pResult*/)
	{
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
		std::string nameFilterNew {m_FilterNameControl.getEditText()};
		std::string locationFilterNew {m_FilterLocationControl.getEditText()};

		if (m_initFinished && nameFilterNew != m_nameFilterCurrent || locationFilterNew != m_locationFilterCurrent)
		{
			m_nameFilterCurrent = nameFilterNew;
			m_locationFilterCurrent = locationFilterNew;
			loadGridCtrl();
		}
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
			SvGcl::GridCellCheck* cell = dynamic_cast<SvGcl::GridCellCheck*>(m_Grid.GetCell(i, CheckColumn));
			if (nullptr != cell)
			{
				std::string* pLocation = reinterpret_cast<std::string*> ( m_Grid.GetItemData(i, LocationColumn) );
				if( nullptr != pLocation )
				{
					bool isChecked = (TRUE == cell->GetCheck());
					if (isChecked != shouldCheck)
					{
						changeCheckState(*pLocation, shouldCheck, i);
					}
				}
			}
		}
		loadGridCtrl();
	}

	void ObjectFilterPpg::loadGridCtrl()
	{
		int nameColumnWidth = NameColWidthDefault;
		int locationColumnWidth = LocationColWidthDefault;
		int checkColumnWidth = CheckColWidthDefault;
		int typeColumnWidth = TypeColWidthDefault;
		if( FilterColumnNumber == m_Grid.GetColumnCount() )
		{
			nameColumnWidth = m_Grid.GetColumnWidth(NameColumn);
			locationColumnWidth = m_Grid.GetColumnWidth(LocationColumn);
			checkColumnWidth = m_Grid.GetColumnWidth(CheckColumn);
			typeColumnWidth = m_Grid.GetColumnWidth(TypeColumn);
		}

		m_Grid.SetRedraw(FALSE);
		m_Grid.AutoSize(GVS_DATA);
		m_Grid.SetColumnCount( FilterColumnNumber );
		m_Grid.SetFixedRowSelection(TRUE);
		m_Grid.SetFixedRowCount(1);
		m_Grid.SetFixedColumnSelection(FALSE);
		m_Grid.SetColumnWidth(NameColumn, nameColumnWidth);
		m_Grid.SetColumnWidth(LocationColumn, locationColumnWidth);
		m_Grid.SetColumnWidth(CheckColumn, checkColumnWidth);
		m_Grid.SetColumnWidth(TypeColumn, typeColumnWidth);

		//add header
		SvGcl::GV_ITEM Item;
		Item.mask = GVIF_TEXT;
		Item.row = 0;
		Item.col = NameColumn;
		Item.strText = FilterColumnName;
		m_Grid.SetItem(&Item);
		Item.col = LocationColumn;
		Item.strText = FilterColumnLocation;
		m_Grid.SetItem(&Item);
		Item.col = CheckColumn;
		Item.strText = FilterColumnCheck;
		m_Grid.SetItem(&Item);
		Item.col = TypeColumn;
		Item.strText = FilterColumnType;
		m_Grid.SetItem(&Item);

		auto wildcardPatternPlusAsteriskName = m_nameFilterCurrent + _T("*");
		auto wildcardPatternPlusAsteriskLocation = m_locationFilterCurrent + _T("*");

		//add leaves
		int rowCount = 1;
		m_Grid.SetRowCount(rowCount);
		int checkSelection = m_checkedControl.GetCurSel();
		int typeSelection = m_TypeControl.GetCurSel();
		CString typeText = _T("");
		m_TypeControl.GetLBText(typeSelection, typeText);

		SvCl::ObjectTreeItems::pre_order_iterator Iter = m_rTreeContainer.pre_order_begin();
		while( m_rTreeContainer.pre_order_end() != Iter )
		{
			if( Iter->second->isLeaf() )
			{
				if (m_SingleSelect && SvCl::ObjectSelectorItem::CheckedEnabled == Iter->second->m_CheckedState )
				{
					m_CheckedLocation = Iter->first;
				}
				bool isNameValid = PathMatchSpecA(Iter->second->m_Name.c_str(), wildcardPatternPlusAsteriskName.c_str());
				bool isLocationValid = PathMatchSpecA(Iter->second->m_Location.c_str(), wildcardPatternPlusAsteriskLocation.c_str());

				if ( (isNameValid && isLocationValid) &&
					 (0 == checkSelection || 
						( 1 == checkSelection && SvCl::ObjectSelectorItem::CheckedEnabled == Iter->second->m_CheckedState) || 
						( 2 == checkSelection && SvCl::ObjectSelectorItem::CheckedEnabled != Iter->second->m_CheckedState) ) &&
					 (0 == typeSelection || typeText == Iter->second->m_ItemTypeName.c_str() ))
				{
					m_Grid.SetRowCount(rowCount + 1);
					m_Grid.SetItemText(rowCount, NameColumn, Iter->second->m_Name.c_str());
					m_Grid.SetItemState(rowCount, NameColumn, m_Grid.GetItemState(rowCount, 0) | GVIS_READONLY);
					m_Grid.SetItemText(rowCount, LocationColumn, Iter->second->m_Location.c_str());
					m_Grid.SetItemState(rowCount, LocationColumn, m_Grid.GetItemState(rowCount, LocationColumn) | GVIS_READONLY);
					m_Grid.SetItemData(rowCount, LocationColumn, reinterpret_cast<LPARAM> (&Iter->first) );
					//We need to use the using here because the macro RUNTIME_CLASS cannot handle namespaces
					using namespace SvGcl;
					m_Grid.SetCellType(rowCount, CheckColumn, RUNTIME_CLASS( GridCellCheck ));
					SvGcl::GridCellCheck* cell = dynamic_cast<SvGcl::GridCellCheck*>(m_Grid.GetCell(rowCount, CheckColumn));
					if (nullptr != cell)
					{
						if (SvCl::ObjectSelectorItem::CheckedEnabled == Iter->second->m_CheckedState)
						{
							cell->SetCheck( TRUE );
							if (m_SingleSelect)
							{
								m_CheckedRow = rowCount;
							}
						}
						else
						{
							cell->SetCheck( FALSE );
						}
					}

					m_Grid.SetItemText(rowCount, TypeColumn, Iter->second->m_ItemTypeName.c_str());
					m_Grid.SetItemState(rowCount, TypeColumn, m_Grid.GetItemState(rowCount, TypeColumn) | GVIS_READONLY);
					rowCount++;
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
			if( menu.LoadMenu(IDR_TREE_ALL_VALUES_MENU) )
			{
				CMenu* pPopupMenu = menu.GetSubMenu(0);
				if( pPopupMenu )
				{
					ClientToScreen( &point );
					pPopupMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
				}
			}
		}
	}

	void ObjectFilterPpg::changeCheckState( const std::string& rLocation, bool isChecked, int)
	{
		SvCl::ObjectTreeItems::iterator iter = m_rTreeContainer.findItem(rLocation);
		if( m_rTreeContainer.end() != iter )
		{
			if (iter->second->isLeaf() && 
				(iter->second->m_CheckedState == SvCl::ObjectSelectorItem::CheckedEnabled) != isChecked)
			{
				if (m_SingleSelect)
				{
					//remove selection from old leaf
					if ( !m_CheckedLocation.empty() )
					{
						m_rTreeContainer.clearItem( m_CheckedLocation );

						//remove selection in grid
						SvGcl::GridCellCheck* cellOld = dynamic_cast<SvGcl::GridCellCheck*>(m_Grid.GetCell(m_CheckedRow, CheckColumn));
						if (nullptr != cellOld)
						{
							cellOld->SetCheck(FALSE);
						}
						m_CheckedLocation = _T("");
					}

					//set currentSelection to new checked leaf
					if (isChecked)
					{
						m_CheckedLocation = iter->first;
					}
				}

				iter->second->m_CheckedState = isChecked ? SvCl::ObjectSelectorItem::CheckedEnabled : SvCl::ObjectSelectorItem::UncheckedEnabled;
				m_rTreeContainer.setParentState(iter);
			}
		}
	}
	#pragma endregion Protected Methods

} //namespace SvOsl
