//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2Feature.cpp
/// This is the class for the feature tab of Blob Analyzer 2
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BlobAnalyzer2Feature.h"
#include "BlobAnalyzer2FeatureSelector.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "GridCtrlLibrary/GridCellCheck.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	constexpr int cHeaderSize = 1;
	constexpr int cTextColumnSize = 150;
	constexpr int cBoxColumnSize = 30;

	struct ColumnDef 
	{
		std::string m_name;
		int m_columnSize;
		int m_protoBufFieldId = -1;

		ColumnDef(const std::string& name, int columnSize, int protoBufFieldId = -1) : m_name(name), m_columnSize(columnSize), m_protoBufFieldId(protoBufFieldId) {};
		//int type;
	};

	enum ColumnPos
	{
		NameColumn = 0,
		SortEnableColumn,
		SortDirectionColumn,
		ExcludeEnabledColumn,
		ExcludeInnerColumn,
		LowerBoundColumn,
		LowerBoundButtonColumn,
		UpperBoundColumn,
		UpperBoundButtonColumn,
	};

	std::map<int, ColumnDef> g_columnFeatureDefArray = { { NameColumn, ColumnDef{"Name", cTextColumnSize}},
		{SortEnableColumn, {"Sort", cBoxColumnSize, SvPb::FeatureData::kIsSortFieldNumber}},
		{SortDirectionColumn, {"Direction", cBoxColumnSize, SvPb::FeatureData::kIsAscentFieldNumber }},
		{ExcludeEnabledColumn, {"Exclude", cBoxColumnSize, SvPb::FeatureData::kIsExcludeFieldNumber}},
		{ExcludeInnerColumn, {"Inner", cBoxColumnSize, SvPb::FeatureData::kIsExcludeInnerFieldNumber }},
		{LowerBoundColumn, {"Lower Bound", cTextColumnSize, SvPb::FeatureData::kLowerBoundIndirectFieldNumber}},
		{LowerBoundButtonColumn, {"", cBoxColumnSize}},
		{UpperBoundColumn, {"Upper Bound", cTextColumnSize, SvPb::FeatureData::kUpperBoundIndirectFieldNumber}},
		{UpperBoundButtonColumn, {"", cBoxColumnSize}}
	};
	
	BEGIN_MESSAGE_MAP(BlobAnalyzer2Feature, CPropertyPage)
		//{{AFX_MSG_MAP(BlobAnalyzer2Feature)
		ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedSelector)
		ON_BN_CLICKED(IDC_BUTTON_MOVEUP, OnBnClickedMoveUp)
		ON_BN_CLICKED(IDC_BUTTON_MOVEDOWN, OnBnClickedMoveDown)
		ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
		ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
		ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnSelectionChanged)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

#pragma region Constructor
	BlobAnalyzer2Feature::BlobAnalyzer2Feature(uint32_t inspectionId, uint32_t taskObjectId)
		: CPropertyPage(BlobAnalyzer2Feature::IDD)
		, m_InspectionID(inspectionId)
		, m_TaskObjectID(taskObjectId)
		, m_objectSelector(inspectionId, taskObjectId)
	{
	}

	BlobAnalyzer2Feature::~BlobAnalyzer2Feature()
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	bool BlobAnalyzer2Feature::QueryAllowExit()
	{
		return setInspectionData();
	}
#pragma endregion Public Methods

#pragma region Protected Methods
	void BlobAnalyzer2Feature::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(BlobAnalyzer2Feature)
		DDX_Control(pDX, IDC_GRID, m_Grid);
		//}}AFX_DATA_MAP
	}

	BOOL BlobAnalyzer2Feature::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		HRESULT result = loadFeatureData();
		if (S_OK != result)
		{
			return false;
		}

		initGridControl();
		FillGridControl();

		m_downArrowBitmap.LoadOEMBitmap(OBM_DNARROW);
		m_ImageList.Create(16, 16, ILC_COLOR8, 0, 1);
		m_ImageList.Add(&m_downArrowBitmap, nullptr);
		m_Grid.SetImageList(&m_ImageList);

		UpdateData(FALSE);

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
	}

	BOOL BlobAnalyzer2Feature::OnSetActive()
	{
		BOOL bOk = CPropertyPage::OnSetActive();
		if (bOk)
		{
			bOk = (S_OK == loadFeatureData());
			FillGridControl();
		}
		return bOk;
	}
		
	BOOL BlobAnalyzer2Feature::OnKillActive()
	{
		SvGcl::CCellID focusId = m_Grid.GetFocusCell();
		if (focusId.IsValid())
		{
			SvGcl::CGridCellBase* pCell = m_Grid.GetCell(focusId.row, focusId.col);
			if (nullptr != pCell)
			{
				pCell->EndEdit();
			}
		}

		bool ret = setInspectionData();
		if (ret)
		{
			return CPropertyPage::OnKillActive();
		}
		return false;
	}

	void BlobAnalyzer2Feature::OnBnClickedMoveUp()
	{
		SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
		if (Selection.GetMinRow() == Selection.GetMaxRow() && 1 < Selection.GetMinRow())
		{
			m_featureData.SwapElements(Selection.GetMinRow() - 1, Selection.GetMinRow() - 2);
			Selection.SetMinRow(Selection.GetMinRow() - 1);
			Selection.SetMaxRow(Selection.GetMaxRow() - 1);
			m_Grid.SetSelectedRange(Selection, true);
			m_Grid.SetFocusCell(Selection.GetMaxRow(), Selection.GetMaxCol());
			FillGridControl();
		}
	}

	void BlobAnalyzer2Feature::OnBnClickedMoveDown()
	{
		SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
		if (Selection.GetMinRow() == Selection.GetMaxRow() && 0 < Selection.GetMinRow() && Selection.GetMinRow() + 1 < m_Grid.GetRowCount())
		{
			m_featureData.SwapElements(Selection.GetMinRow() - 1, Selection.GetMinRow());
			Selection.SetMinRow(Selection.GetMinRow() + 1);
			Selection.SetMaxRow(Selection.GetMaxRow() + 1);
			m_Grid.SetSelectedRange(Selection, true);
			m_Grid.SetFocusCell(Selection.GetMaxRow(), Selection.GetMaxCol());
			FillGridControl();
		}
	}

	void BlobAnalyzer2Feature::OnBnClickedSelector()
	{
		static const SvPb::GetAvailableFeaturesResponse availableFeature = getAvailableFeatures();
		std::vector<FeatureData4Selector> features;
		for (const auto& rEntry : availableFeature.list())
		{
			bool isActive = (m_featureData.end() != std::find_if(m_featureData.begin(), m_featureData.end(), [rEntry](const auto& rValue) { return rEntry.id() == rValue.type(); }));
			features.emplace_back(rEntry.name(), rEntry.id(), isActive, rEntry.isnecessary());
		}

		BlobAnalyzer2FeatureSelector dlg(features);
		if (IDOK == dlg.DoModal())
		{
			features = dlg.getFeatureData();
			for (const auto& rEntry : features)
			{
				if (rEntry.m_isActive)
				{ //add if not set yet
					if (m_featureData.end() == std::find_if(m_featureData.begin(), m_featureData.end(), [rEntry](const auto& rValue) { return rEntry.m_type == rValue.type(); }))
					{
						auto* pNewFeature = m_featureData.Add();
						pNewFeature->set_type(rEntry.m_type);
						pNewFeature->set_name(rEntry.m_name);
					}
				}
				else
				{	//remove if set yet
					auto iter = std::find_if(m_featureData.begin(), m_featureData.end(), [rEntry](const auto& rValue) { return rEntry.m_type == rValue.type(); });
					if (m_featureData.end() != iter)
					{
						m_featureData.erase(iter);
					}
				}
			}
			setInspectionData();
			FillGridControl();
		}
	}

	void BlobAnalyzer2Feature::OnGridClick(NMHDR* pNotifyStruct, LRESULT*)
	{
		auto* pItem = reinterpret_cast<SvGcl::NM_GRIDVIEW*>(pNotifyStruct);
		if (1 > pItem->iRow || m_featureData.size() < pItem->iRow)
		{
			return;
		}

		switch (pItem->iColumn)
		{
		case SortEnableColumn:
		{
			auto* pCell = dynamic_cast<SvGcl::CGridCellCheck*>(m_Grid.GetCell(pItem->iRow, SortEnableColumn));
			if (nullptr != pCell)
			{
				bool isSort = (TRUE == pCell->GetCheck());
				if (m_featureData[pItem->iRow - 1].is_sort() != isSort)
				{
					m_featureData[pItem->iRow - 1].set_is_sort(isSort);
					sortFeatures(pItem->iRow-1);
					FillGridControl();
				}
			}
			break;
		}
		case SortDirectionColumn:
		{
			auto* pCell = dynamic_cast<SvGcl::CGridCellCheck*>(m_Grid.GetCell(pItem->iRow, SortDirectionColumn));
			if (nullptr != pCell)
			{
				m_featureData[pItem->iRow - 1].set_is_ascent(TRUE == pCell->GetCheck());
			}
			break;
		}
		case ExcludeEnabledColumn:
		{
			auto* pCell = dynamic_cast<SvGcl::CGridCellCheck*>(m_Grid.GetCell(pItem->iRow, ExcludeEnabledColumn));
			if (nullptr != pCell)
			{
				bool isExclude = (TRUE == pCell->GetCheck());
				if (m_featureData[pItem->iRow - 1].is_exclude() != isExclude)
				{
					m_featureData[pItem->iRow - 1].set_is_exclude(isExclude);
					FillGridControl();
				}
			}
			break;
		}
		case ExcludeInnerColumn:
		{
			auto* pCell = dynamic_cast<SvGcl::CGridCellCheck*>(m_Grid.GetCell(pItem->iRow, ExcludeInnerColumn));
			if (nullptr != pCell)
			{
				m_featureData[pItem->iRow - 1].set_is_exclude_inner(TRUE == pCell->GetCheck());
			}
			break;
		}
		case UpperBoundButtonColumn:
			if (m_featureData[pItem->iRow - 1].is_exclude())
			{
				std::string cellText = m_Grid.GetCell(pItem->iRow, UpperBoundColumn)->GetText();
				auto iter = g_columnFeatureDefArray.find(UpperBoundColumn);
				if (ShowObjectSelector(cellText, g_columnFeatureDefArray.end() != iter ? iter->second.m_name : "ObjectSelector"))
				{
					m_Grid.GetCell(pItem->iRow, UpperBoundColumn)->SetText(cellText.c_str());
					m_featureData[pItem->iRow - 1].set_upper_bound_indirect(cellText);
					m_Grid.Refresh();
				}
			}
			break;
		case LowerBoundButtonColumn:
			if (m_featureData[pItem->iRow - 1].is_exclude())
			{
				std::string cellText = m_Grid.GetCell(pItem->iRow, LowerBoundColumn)->GetText();
				auto iter = g_columnFeatureDefArray.find(LowerBoundColumn);
				if (ShowObjectSelector(cellText, g_columnFeatureDefArray.end() != iter ? iter->second.m_name : "ObjectSelector"))
				{
					m_Grid.GetCell(pItem->iRow, LowerBoundColumn)->SetText(cellText.c_str());
					m_featureData[pItem->iRow - 1].set_lower_bound_indirect(cellText);
					m_Grid.Refresh();
				}
			}
			break;
		default:
			//nothing to do
			return;
		}

		setInspectionData();
		UpdateEnableButtons();
	}

	void BlobAnalyzer2Feature::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;

		if ((LowerBoundColumn == pItem->iColumn || UpperBoundColumn == pItem->iColumn) && 0 < pItem->iRow && m_featureData.size() >= pItem->iRow)
		{
			std::string cellText = m_Grid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
			if (LowerBoundColumn == pItem->iColumn)
			{
				m_featureData[pItem->iRow - 1].set_lower_bound_indirect(cellText);
			}
			else
			{
				m_featureData[pItem->iRow - 1].set_upper_bound_indirect(cellText);
			}
			setInspectionData();
		}

		*pResult = 0;
	}

	void BlobAnalyzer2Feature::OnSelectionChanged(NMHDR*, LRESULT*)
	{
		UpdateEnableButtons();		
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	bool BlobAnalyzer2Feature::setInspectionData()
	{
		UpdateData(TRUE);

		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_setfeaturesrequest();
		pRequest->set_objectid(m_TaskObjectID);
		*pRequest->mutable_list() = m_featureData;

		HRESULT hResult = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (hResult == S_OK && responseCmd.has_setfeaturesresponse())
		{
			if (0 < responseCmd.setfeaturesresponse().error_list().size())
			{
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				SvStl::MessageContainerVector tmpMessages =  SvPb::setMessageVectorFromMessagePB(responseCmd.setfeaturesresponse().messages());
				if (0 < tmpMessages.size())
				{
					Msg.setMessage(tmpMessages[0].getMessage());
				}
				else
				{
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SetFeatureParameterFailed, SvStl::SourceFileParams(StdMessageParams));
				}
				const auto& range = responseCmd.setfeaturesresponse().error_list(0);
				auto iter = std::find_if(g_columnFeatureDefArray.begin(), g_columnFeatureDefArray.end(), [range](const auto& rEntry) { return rEntry.second.m_protoBufFieldId == range.fieldid(); });
				if (g_columnFeatureDefArray.end() != iter)
				{
					m_Grid.SetSelectedRange(range.row(), iter->first, range.row(), iter->first);
					m_Grid.SetFocusCell(range.row(), iter->first);
					CRect rect;
					if (m_Grid.GetCellRect(range.row(), iter->first, rect))
					{
						SvGcl::CGridCellBase* pCell = m_Grid.GetCell(range.row(), iter->first);
						if (pCell)
						{
							pCell->Edit(range.row(), iter->first, rect, CPoint(-1, -1), IDC_INPLACE_CONTROL, 0);
						}
					}
				}
				hResult = E_FAIL;
			}
		}

		if (S_OK == hResult)
		{
			hResult = SvCmd::RunOnceSynchronous(m_InspectionID);
		}

		return S_OK == hResult;
	}

	void BlobAnalyzer2Feature::initGridControl()
	{
		m_Grid.SetFixedRowCount(cHeaderSize);
		m_Grid.SetFixedColumnCount(0);
		m_Grid.SetRowResize(FALSE);
		m_Grid.SetColumnResize(true);
		m_Grid.AllowReorderColumn(false);
		m_Grid.EnableDragAndDrop(FALSE);
		m_Grid.SetEditable(true);
		m_Grid.SetColumnCount(static_cast<int>(g_columnFeatureDefArray.size()));
		SvGcl::GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
		Item.row = 0;
		Item.crBkClr = CLR_DEFAULT;
		Item.nFormat = DT_LEFT | DT_VCENTER | DT_WORDBREAK;
		for (const auto& rData : g_columnFeatureDefArray)
		{
			m_Grid.SetColumnWidth(rData.first, rData.second.m_columnSize);
			Item.col = rData.first;
			Item.strText = rData.second.m_name.c_str();
			m_Grid.SetItem(&Item);
		}
	}

	void BlobAnalyzer2Feature::FillGridControl()
	{
		int sortNumber = 0;
		m_Grid.SetRowCount(m_featureData.size() + 1);
		for (int i = 0; m_featureData.size() > i; ++i)
		{
			auto row = i + 1;
			m_Grid.SetItemText(row, NameColumn, m_featureData[i].name().c_str());
			m_Grid.SetItemState(row, NameColumn, m_Grid.GetItemState(row, NameColumn) | GVIS_READONLY);
			using namespace SvGcl;
			if (3 > sortNumber)
			{
				m_Grid.SetCellType(row, SortEnableColumn, RUNTIME_CLASS(CGridCellCheck));
				auto* pCell = dynamic_cast<SvGcl::CGridCellCheck*>(m_Grid.GetCell(row, SortEnableColumn));
				if (nullptr != pCell)
				{
					pCell->SetCheck(m_featureData[i].is_sort());
					sortNumber += m_featureData[i].is_sort();
				}
			}
			else
			{
				m_Grid.SetCellType(row, SortEnableColumn, RUNTIME_CLASS(CGridCell));
				m_Grid.SetItemState(row, SortEnableColumn, m_Grid.GetItemState(row, SortEnableColumn) | GVIS_READONLY);
			}
			UINT state = m_Grid.GetItemState(row, SortDirectionColumn);
			if (m_featureData[i].is_sort())
			{
				m_Grid.SetCellType(row, SortDirectionColumn, RUNTIME_CLASS(CGridCellCheck));
				auto* pCell = dynamic_cast<SvGcl::CGridCellCheck*>(m_Grid.GetCell(row, SortDirectionColumn));
				if (nullptr != pCell)
				{
					pCell->SetCheck(m_featureData[i].is_ascent());
				}
				state &= ~GVIS_READONLY;
			}
			else
			{
				m_Grid.SetCellType(row, SortDirectionColumn, RUNTIME_CLASS(CGridCell));
				state |= GVIS_READONLY;
			}
			m_Grid.SetItemState(row, SortDirectionColumn, state);
			m_Grid.SetCellType(row, ExcludeEnabledColumn, RUNTIME_CLASS(CGridCellCheck));
			auto* pCell = dynamic_cast<SvGcl::CGridCellCheck*>(m_Grid.GetCell(row, ExcludeEnabledColumn));
			if (nullptr != pCell)
			{
				pCell->SetCheck(m_featureData[i].is_exclude());
			}
			if (m_featureData[i].is_exclude())
			{
				m_Grid.SetCellType(row, ExcludeInnerColumn, RUNTIME_CLASS(CGridCellCheck));
				pCell = dynamic_cast<SvGcl::CGridCellCheck*>(m_Grid.GetCell(row, ExcludeInnerColumn));
				if (nullptr != pCell)
				{
					pCell->SetCheck(m_featureData[i].is_exclude_inner());
				}
				m_Grid.SetItemState(row, ExcludeInnerColumn, m_Grid.GetItemState(row, ExcludeInnerColumn) & (~GVIS_READONLY));
				std::string tmp = m_featureData[i].lower_bound_indirect().empty() ? std::to_string(m_featureData[i].lower_bound()) : m_featureData[i].lower_bound_indirect();
				m_Grid.SetItemText(row, LowerBoundColumn, tmp.c_str());
				m_Grid.SetItemState(row, LowerBoundColumn, m_Grid.GetItemState(row, LowerBoundColumn) & (~GVIS_READONLY));
				SvGcl::GV_ITEM buttonItem;
				buttonItem.mask = GVIF_IMAGE;
				buttonItem.iImage = 0;
				buttonItem.row = row;
				buttonItem.col = LowerBoundButtonColumn;
				m_Grid.SetItem(&buttonItem);
				tmp = m_featureData[i].upper_bound_indirect().empty() ? std::to_string(m_featureData[i].upper_bound()) : m_featureData[i].upper_bound_indirect();
				m_Grid.SetItemText(row, UpperBoundColumn, tmp.c_str());
				m_Grid.SetItemState(row, UpperBoundColumn, m_Grid.GetItemState(row, UpperBoundColumn) & (~GVIS_READONLY));
				buttonItem.col = UpperBoundButtonColumn;
				m_Grid.SetItem(&buttonItem);
			}
			else
			{
				m_Grid.SetCellType(row, ExcludeInnerColumn, RUNTIME_CLASS(CGridCell));
				m_Grid.SetItemState(row, ExcludeInnerColumn, m_Grid.GetItemState(row, ExcludeInnerColumn) | GVIS_READONLY);
				m_Grid.SetItemText(row, LowerBoundColumn, "");
				m_Grid.SetItemState(row, LowerBoundColumn, m_Grid.GetItemState(row, LowerBoundColumn) | GVIS_READONLY);
				SvGcl::GV_ITEM buttonItem;
				buttonItem.mask = GVIF_IMAGE;
				buttonItem.iImage = -1;
				buttonItem.row = row;
				buttonItem.col = LowerBoundButtonColumn;
				m_Grid.SetItem(&buttonItem);
				m_Grid.SetItemState(row, LowerBoundButtonColumn, m_Grid.GetItemState(row, UpperBoundColumn) | GVIS_READONLY);
				m_Grid.SetItemText(row, UpperBoundColumn, "");
				m_Grid.SetItemState(row, UpperBoundColumn, m_Grid.GetItemState(row, UpperBoundColumn) | GVIS_READONLY);
				buttonItem.col = UpperBoundButtonColumn;
				m_Grid.SetItem(&buttonItem);
				m_Grid.SetItemState(row, UpperBoundButtonColumn, m_Grid.GetItemState(row, UpperBoundColumn) | GVIS_READONLY);
			}
		}
		
		m_Grid.Refresh();
		UpdateEnableButtons();
	}

	void BlobAnalyzer2Feature::showContextMenu(CPoint point)
	{
		CMenu menu;
		if (menu.LoadMenu(IDR_TABLE_DEFINE_MOUSE_MENU))
		{
			if (CMenu* pPopupMenu = menu.GetSubMenu(0))
			{
				ClientToScreen(&point);
				pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
			}
		}
	}

	void BlobAnalyzer2Feature::UpdateEnableButtons()
	{
		SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
		bool bMoveUpEnable = false;
		bool bMoveDownEnable = false;

		if (Selection.GetMinRow() == Selection.GetMaxRow() && m_featureData.size() > Selection.GetMinRow() + 1 && 0 < Selection.GetMinRow())
		{
			bool isSort = m_featureData[Selection.GetMinRow() - 1].is_sort();
			bMoveUpEnable = (isSort && 1 < Selection.GetMinRow());
			bMoveDownEnable = (isSort && m_featureData.size() > Selection.GetMinRow() && m_featureData[Selection.GetMinRow()].is_sort());
		}
		
		GetDlgItem(IDC_BUTTON_MOVEUP)->EnableWindow(bMoveUpEnable);
		GetDlgItem(IDC_BUTTON_MOVEDOWN)->EnableWindow(bMoveDownEnable);
	}

	HRESULT BlobAnalyzer2Feature::loadFeatureData()
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getfeaturesrequest();
		pRequest->set_objectid(m_TaskObjectID);

		HRESULT hResult = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (hResult == S_OK)
		{
			if (responseCmd.has_getfeaturesresponse())
			{
				m_featureData = responseCmd.getfeaturesresponse().list();
			}
			else
			{
				hResult = E_FAIL;
			}
		}

		return hResult;
	}

	SvPb::GetAvailableFeaturesResponse BlobAnalyzer2Feature::getAvailableFeatures() const
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		requestCmd.mutable_getavailablefeaturesrequest();
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getavailablefeaturesresponse())
		{
			return responseCmd.getavailablefeaturesresponse();
		}
		assert(false);
		return {};
	}

	bool BlobAnalyzer2Feature::ShowObjectSelector(std::string& rName, const std::string& title)
	{
		return m_objectSelector.Show(rName, title, this, SvPb::SelectorFilter::rangeValue);
	}

	void BlobAnalyzer2Feature::sortFeatures(int pos)
	{
		if (0 > pos || m_featureData.size() <= pos)
		{	//wrong pos
			return;
		}

		if (m_featureData[pos].is_sort())
		{
			while (0 < pos && !m_featureData[pos - 1].is_sort())
			{	//move to last sort pos
				m_featureData.SwapElements(pos, pos--);
			}
		}
		else
		{
			while (m_featureData.size() > pos - 1 && m_featureData[pos+1].is_sort())
			{	//move to first not sort pos
				m_featureData.SwapElements(pos, pos++);
			}
		}
	}
#pragma endregion Private Mehods
} //namespace SvOg