//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobAnalyzer2FeatureSelector.cpp
/// This is the class for the feature selector of Blob Analyzer 2
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BlobAnalyzer2FeatureSelector.h"
#include "GridCtrlLibrary/GridCellCheck.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	constexpr int cHeaderSize = 1;
	constexpr int cNameColumnSize = 250;
	constexpr int cCheckBoxColumnSize = 50;

	BEGIN_MESSAGE_MAP(BlobAnalyzer2FeatureSelector, CDialog)
		//{{AFX_MSG_MAP(BlobAnalyzer2FeatureSelector)
		ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

#pragma region Constructor
	BlobAnalyzer2FeatureSelector::BlobAnalyzer2FeatureSelector(std::vector<FeatureData4Selector> featureVector, int currentNumberOfActive)
		: CDialog(BlobAnalyzer2FeatureSelector::IDD)
		, m_featureData(featureVector)
		, m_numberOfActive(currentNumberOfActive)
	{
	}

	BlobAnalyzer2FeatureSelector::~BlobAnalyzer2FeatureSelector()
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
#pragma endregion Public Methods

#pragma region Protected Methods
	void BlobAnalyzer2FeatureSelector::DoDataExchange(CDataExchange* pDX)
	{
		__super::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(BlobAnalyzer2FeatureSelector)
		DDX_Control(pDX, IDC_GRID, m_Grid);
		DDX_Control(pDX, IDC_FEATURE_NUMBER, m_numberLabel);
		//}}AFX_DATA_MAP
	}

	BOOL BlobAnalyzer2FeatureSelector::OnInitDialog()
	{
		__super::OnInitDialog();

		initGridControl();
		FillGridControl();

		UpdateData(FALSE);

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	void BlobAnalyzer2FeatureSelector::OnGridClick(NMHDR* pNotifyStruct, LRESULT*)
	{
		auto* pItem = reinterpret_cast<SvGcl::NM_GRIDVIEW*>(pNotifyStruct);
		if (1 > pItem->iRow || m_featureData.size() < pItem->iRow)
		{
			return;
		}

		if (0 == pItem->iColumn)
		{
			auto* pCell = dynamic_cast<SvGcl::GridCellCheck*>(m_Grid.GetCell(pItem->iRow, 0));
			if (nullptr != pCell)
			{
				bool isActive = (TRUE == pCell->GetCheck());
				if (isActive != m_featureData[pItem->iRow - 1].m_isActive)
				{
					if (isActive)
					{
						++m_numberOfActive;
					}
					else
					{
						--m_numberOfActive;
					}
					m_featureData[pItem->iRow - 1].m_isActive = isActive;
				}
			}
		}
		
		FillGridControl();
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	void BlobAnalyzer2FeatureSelector::initGridControl()
	{
		m_Grid.SetFixedRowCount(cHeaderSize);
		m_Grid.SetFixedColumnCount(0);
		m_Grid.SetRowResize(FALSE);
		m_Grid.SetColumnResize(true);
		m_Grid.AllowReorderColumn(false);
		m_Grid.EnableDragAndDrop(FALSE);
		m_Grid.SetEditable(true);
		m_Grid.SetColumnCount(2);
		SvGcl::GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
		Item.row = 0;
		Item.crBkClr = CLR_DEFAULT;
		Item.nFormat = DT_LEFT | DT_VCENTER | DT_WORDBREAK;

		m_Grid.SetColumnWidth(0, cCheckBoxColumnSize);
		Item.col = 0;
		Item.strText = "";
		m_Grid.SetItem(&Item);
		
		m_Grid.SetColumnWidth(1, cNameColumnSize);
		Item.col = 1;
		Item.strText = "Name";
		m_Grid.SetItem(&Item);
	}

	void BlobAnalyzer2FeatureSelector::FillGridControl()
	{
		m_Grid.SetRowCount(static_cast<int>(m_featureData.size() + 1));
		for (int i = 0; m_featureData.size() > i; ++i)
		{
			auto row = i + 1;
			if (!m_featureData[i].m_isNecessary)
			{
				using namespace SvGcl;
				m_Grid.SetCellType(row, 0, RUNTIME_CLASS(GridCellCheck));
				auto* pCell = dynamic_cast<SvGcl::GridCellCheck*>(m_Grid.GetCell(row, 0));
				if (nullptr != pCell)
				{
					pCell->SetCheck(m_featureData[i].m_isActive);
					UINT state = m_Grid.GetItemState(row, 0);
					if (!m_featureData[i].m_isActive && m_numberOfActive >= SvDef::c_maxTableColumn)
					{
						state |= GVIS_READONLY;
					}
					else
					{
						state &= ~GVIS_READONLY;
					}
					m_Grid.SetItemState(row, 0, state);
				}
			}
			m_Grid.SetItemText(row, 1, m_featureData[i].m_name.c_str());
			m_Grid.SetItemState(row, 1, m_Grid.GetItemState(row, 1) | GVIS_READONLY);
		}

		m_Grid.Refresh();
		updateControls();
	}

	void BlobAnalyzer2FeatureSelector::updateControls()
	{
		CString text;
		text.Format("%d from %d", m_numberOfActive, SvDef::c_maxTableColumn);
		m_numberLabel.SetWindowTextA(text);
	}
#pragma endregion Private Mehods
} //namespace SvOg
