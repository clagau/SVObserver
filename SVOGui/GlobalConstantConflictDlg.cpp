//*****************************************************************************
/// \copyright (c) 2015,2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to solve Global Constant value conflicts
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "GlobalConstantConflictDlg.h"

#include "TextDefinesSvOg.h"
#include "GridCtrlLibrary\GridCellCheck.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOg
{
#pragma region Declarations
	constexpr int cHeaderSize = 1;
	constexpr int DataSets = 2;
	constexpr int GridColumnNumber = 4;

	BEGIN_MESSAGE_MAP(GlobalConstantConflictDlg, CDialog)
		ON_NOTIFY(NM_CLICK, IDC_GLOBAL_CONFLICT_GRID, &GlobalConstantConflictDlg::OnGridClick)
	END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
	GlobalConstantConflictDlg::GlobalConstantConflictDlg(SvUl::GlobalConflictPairVector& rGlobalPairs, CWnd* pParent /*nullptr*/)
		: CDialog(GlobalConstantConflictDlg::IDD, pParent)
		, m_rGlobalPairs( rGlobalPairs )
	{

	}

	GlobalConstantConflictDlg::~GlobalConstantConflictDlg()
	{
	}
#pragma endregion Constructor

#pragma region Protected Methods
	void GlobalConstantConflictDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_GLOBAL_CONFLICT_GRID, m_Grid);
	}

	BOOL GlobalConstantConflictDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();

		m_Grid.SetRedraw( false );
		m_Grid.SetFixedRowCount( cHeaderSize );
		m_Grid.SetRowResize( false );
		m_Grid.SetColumnResize( true );
		m_Grid.AllowReorderColumn( false );
		m_Grid.EnableDragAndDrop( false );
		m_Grid.SetEditable( false );
		m_Grid.SetColumnCount( GridColumnNumber * DataSets );
		m_Grid.SetRowCount( static_cast<int> (m_rGlobalPairs.size()) + cHeaderSize );
		
		LoadGlobalData();
		UpdateData( FALSE );

		m_Grid.SetRedraw( true );
		m_Grid.AutoSizeColumns();
		return TRUE;
	}

	void GlobalConstantConflictDlg::LoadGlobalData()
	{
		SvUl::GlobalConflictPairVector::const_iterator Iter( m_rGlobalPairs.cbegin() );
		int RowIndex( 0 );
		
		for( int i=0; i < DataSets*GridColumnNumber; i++ )
		{
			m_Grid.SetItemText( RowIndex, i,  GlobalConflictHeader[i] );
		}

		RowIndex++;
		while( m_rGlobalPairs.cend() != Iter )
		{
			const SvUl::GlobalConstantData& rCurrentGlobalData = Iter->first;
			const SvUl::GlobalConstantData& rImportGlobalData = Iter->second;

			int ColumnIndex( 0 );
			setGridGlobalData( rCurrentGlobalData, RowIndex, ColumnIndex );
			setGridGlobalData( rImportGlobalData, RowIndex, ColumnIndex );
			RowIndex++;
			++Iter;
		}
	}

	void GlobalConstantConflictDlg::setGridGlobalData(const SvUl::GlobalConstantData& rGlobalData, int RowIndex, int& rColumnIndex )
	{
		//The using is needed due to the RUNTIME_CLASS which does not accept namespaces
		using namespace SvGcl;
		m_Grid.SetCellType(RowIndex, rColumnIndex, RUNTIME_CLASS( GridCellCheck ));
		SvGcl::GridCellCheck* pCell = dynamic_cast<SvGcl::GridCellCheck*>(m_Grid.GetCell( RowIndex, rColumnIndex ));
		if( nullptr != pCell )
		{
			pCell->SetCheck( rGlobalData.m_Selected );
		}
		rColumnIndex++;
		m_Grid.SetItemText( RowIndex, rColumnIndex, rGlobalData.m_DottedName.c_str() );
		m_Grid.SetItemState( RowIndex, rColumnIndex, m_Grid.GetItemState( RowIndex, rColumnIndex ) | GVIS_READONLY );

		std::string Text;
		std::string Type;
		convertValueToString( rGlobalData.m_Value, Text, Type );
		rColumnIndex++;
		m_Grid.SetItemText( RowIndex, rColumnIndex, Type.c_str() );
		m_Grid.SetItemState( RowIndex, rColumnIndex, m_Grid.GetItemState( RowIndex, rColumnIndex ) | GVIS_READONLY );
		rColumnIndex++;
		m_Grid.SetItemText( RowIndex, rColumnIndex, Text.c_str() );
		m_Grid.SetItemState( RowIndex, rColumnIndex, m_Grid.GetItemState( RowIndex, rColumnIndex ) | GVIS_READONLY );
		rColumnIndex++;
	}

	void GlobalConstantConflictDlg::convertValueToString( const _variant_t& rValue, std::string& rText,  std::string& rType )
	{
		switch( rValue.vt )
		{
		case VT_R8:
			{
				rText = std::format( _T("{:.06f}"), rValue.dblVal );
				rType = GlobalConstantTypes[ SvUl::GlobalConstantData::DecimalType ];
			}
			break;
		case VT_BSTR:
			{
				rText = SvUl::createStdString(rValue);
				rType = GlobalConstantTypes[ SvUl::GlobalConstantData::TextType ];
			}
			break;
		default:
			break;
		}
	}

	void GlobalConstantConflictDlg::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
	{
		SvGcl::NM_GRIDVIEW* pItem = reinterpret_cast<SvGcl::NM_GRIDVIEW*> (pNotifyStruct);

		SvGcl::GridCellCheck* pCell = dynamic_cast<SvGcl::GridCellCheck*>( m_Grid.GetCell(pItem->iRow,  pItem->iColumn ));
		if( nullptr != pCell )
		{
			//Check if Current Import checkbox clicked
			int OtherCheckCol = 0 == pItem->iColumn ? GridColumnNumber : 0;
			BOOL State( pCell->GetCheck() );
			pCell->SetCheck( !State );
			//Set the other checkbox to the opposite state
			pCell = dynamic_cast<SvGcl::GridCellCheck*>( m_Grid.GetCell(pItem->iRow,  OtherCheckCol ));
			if( nullptr != pCell )
			{
				pCell->SetCheck( State );
			}
			//Swap the selection in the list
			SvUl::GlobalConflictPairVector::iterator Iter( m_rGlobalPairs.begin() );
			std::advance( Iter, pItem->iRow - 1);
			if( m_rGlobalPairs.end() != Iter )
			{
				Iter->first.m_Selected = !Iter->first.m_Selected;
				Iter->second.m_Selected = !Iter->second.m_Selected;
			}
		}
	}

#pragma endregion Protected Methods
} //namespace SvOg
