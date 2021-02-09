//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2Range.cpp
/// This is the class for the range tab of Blob Analyzer 2
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BlobAnalyzer2Range.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "GridCtrlLibrary/GridCellCheck.h"
#include "DisplayHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	constexpr int cHeaderSize = 1;
	constexpr int cNameColumnSize = 150;
	constexpr int cRangeColumnSize = 95;
	constexpr int cBoxColumnSize = 25;

	struct ColumnDef
	{
		std::string m_name;
		int m_columnSize;
		int m_protoBufFieldId = -1;

		ColumnDef(const std::string& name, int columnSize, int protoBufFieldId = -1) : m_name(name), m_columnSize(columnSize), m_protoBufFieldId(protoBufFieldId) {};
	};

	enum ColumnPos
	{
		NameColumn = 0,
		RangeEnableColumn,
		FailHighColumn,
		FailHighButtonColumn,
		WarnHighColumn,
		WarnHighButtonColumn,
		WarnLowColumn,
		WarnLowButtonColumn,
		FailLowColumn,
		FailLowButtonColumn,
	};

	std::map<int, ColumnDef> g_columnRangeDefArray = { { NameColumn, ColumnDef{"Name", cNameColumnSize}},
		{RangeEnableColumn, {"", cBoxColumnSize, SvPb::FeatureData::kIsRangeFieldNumber}},
		{FailHighColumn, {"Fail High", cRangeColumnSize, SvPb::FeatureData::kRangeFailHighIndirectFieldNumber }},
		{FailHighButtonColumn, {"", cBoxColumnSize}},
		{WarnHighColumn, {"Warn High", cRangeColumnSize, SvPb::FeatureData::kRangeWarnHighIndirectFieldNumber }},
		{WarnHighButtonColumn, {"", cBoxColumnSize}},
		{WarnLowColumn, {"Warn Low", cRangeColumnSize, SvPb::FeatureData::kRangeWarnLowIndirectFieldNumber }},
		{WarnLowButtonColumn, {"", cBoxColumnSize}},
		{FailLowColumn, {"Fail Low", cRangeColumnSize, SvPb::FeatureData::kRangeFailLowIndirectFieldNumber }},
		{FailLowButtonColumn, {"", cBoxColumnSize}},
	};

	BEGIN_MESSAGE_MAP(BlobAnalyzer2Range, CPropertyPage)
		//{{AFX_MSG_MAP(BlobAnalyzer2Range)
		ON_BN_CLICKED(IDC_BUTTON_FAILHIGH, &OnBnClickedFailHighIndirect)
		ON_BN_CLICKED(IDC_BUTTON_WARNHIGH, &OnBnClickedWarnlHighIndirect)
		ON_BN_CLICKED(IDC_BUTTON_WARNLOW, &OnBnClickedWarnLowIndirect)
		ON_BN_CLICKED(IDC_BUTTON_FAILLOW, &OnBnClickedFailedLowIndirect)
		ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
		ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID, OnGridBeginEdit)
		ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

#pragma region Constructor
	BlobAnalyzer2Range::BlobAnalyzer2Range(uint32_t inspectionId, uint32_t toolId, uint32_t taskObjectId)
		: CPropertyPage(BlobAnalyzer2Range::IDD)
		, m_InspectionID(inspectionId)
		, m_toolID(toolId)
		, m_TaskObjectID(taskObjectId)
		, m_NumberRangeController(inspectionId, m_TaskObjectID)
		, m_objectSelector(inspectionId)
	{
		m_NumberRangeController.Init();
	}

	BlobAnalyzer2Range::~BlobAnalyzer2Range()
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	bool BlobAnalyzer2Range::QueryAllowExit()
	{
		return setInspectionData();
	}
#pragma endregion Public Methods

#pragma region Protected Methods
	void BlobAnalyzer2Range::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(BlobAnalyzer2Range)
		DDX_Control(pDX, IDC_GRID, m_Grid);
		DDX_Text(pDX, IDC_EDIT_FAILHIGH, m_FailHigh);
		DDX_Text(pDX, IDC_EDIT_WARNHIGH, m_WarnHigh);
		DDX_Text(pDX, IDC_EDIT_WARNLOW, m_WarnLow);
		DDX_Text(pDX, IDC_EDIT_FAILLOW, m_FailLow);
		DDX_Control(pDX, IDC_BUTTON_FAILHIGH, m_ButtonFailHigh);
		DDX_Control(pDX, IDC_BUTTON_WARNHIGH, m_ButtonWarnHigh);
		DDX_Control(pDX, IDC_BUTTON_WARNLOW, m_ButtonWarnLow);
		DDX_Control(pDX, IDC_BUTTON_FAILLOW, m_ButtonFailLow);
		//}}AFX_DATA_MAP
	}

	BOOL BlobAnalyzer2Range::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		//(HBITMAP) is a call to the overloaded function operator HBITMAP and no c style cast
		DisplayHelper::setIconListToGrid(m_ImageList, m_downArrowBitmap, m_Grid);
		m_ButtonFailHigh.SetBitmap((HBITMAP)m_downArrowBitmap);
		m_ButtonWarnHigh.SetBitmap((HBITMAP)m_downArrowBitmap);
		m_ButtonWarnLow.SetBitmap((HBITMAP)m_downArrowBitmap);
		m_ButtonFailLow.SetBitmap((HBITMAP)m_downArrowBitmap);

		setDlgData();

		HRESULT result = loadFeatureData();
		if (S_OK != result)
		{
			return false;
		}

		initGridControl();
		FillGridControl();

		UpdateData(FALSE);

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	BOOL BlobAnalyzer2Range::OnSetActive()
	{
		BOOL bOk = CPropertyPage::OnSetActive();
		if (bOk)
		{
			bOk = (S_OK == loadFeatureData());
			FillGridControl();
		}
		return bOk;
	}

	BOOL BlobAnalyzer2Range::OnKillActive()
	{
		SvGcl::CCellID focusId = m_Grid.GetFocusCell();
		if (focusId.IsValid())
		{
			SvGcl::GridCellBase* pCell = m_Grid.GetCell(focusId.row, focusId.col);
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

	void BlobAnalyzer2Range::OnGridClick(NMHDR* pNotifyStruct, LRESULT*)
	{
		auto* pItem = reinterpret_cast<SvGcl::NM_GRIDVIEW*>(pNotifyStruct);
		if (1 > pItem->iRow || m_featureData.size() < pItem->iRow)
		{
			return;
		}

		switch (pItem->iColumn)
		{
		case RangeEnableColumn:
		{
			auto* pCell = dynamic_cast<SvGcl::GridCellCheck*>(m_Grid.GetCell(pItem->iRow, RangeEnableColumn));
			if (nullptr != pCell)
			{
				bool isRange = (TRUE == pCell->GetCheck());
				if (m_featureData[pItem->iRow - 1].is_range() != isRange)
				{
					m_featureData[pItem->iRow - 1].set_is_range(isRange);
					FillGridControl();
				}
			}
			break;
		}
		case FailHighButtonColumn:
			if (m_featureData[pItem->iRow - 1].is_range())
			{
				std::string cellText = m_Grid.GetCell(pItem->iRow, FailHighColumn)->GetText();
				if (ShowObjectSelector(cellText, RangeEnum::ER_FailHigh))
				{
					m_Grid.GetCell(pItem->iRow, FailHighColumn)->SetText(cellText.c_str());
					m_featureData[pItem->iRow - 1].set_range_fail_high_indirect(cellText);
					m_Grid.Refresh();
				}
			}
			break;
		case WarnHighButtonColumn:
			if (m_featureData[pItem->iRow - 1].is_range())
			{
				std::string cellText = m_Grid.GetCell(pItem->iRow, WarnHighColumn)->GetText();
				if (ShowObjectSelector(cellText, RangeEnum::ER_WarnHigh))
				{
					m_Grid.GetCell(pItem->iRow, WarnHighColumn)->SetText(cellText.c_str());
					m_featureData[pItem->iRow - 1].set_range_warn_high_indirect(cellText);
					m_Grid.Refresh();
				}
			}
			break;
		case WarnLowButtonColumn:
			if (m_featureData[pItem->iRow - 1].is_range())
			{
				std::string cellText = m_Grid.GetCell(pItem->iRow, WarnLowColumn)->GetText();
				if (ShowObjectSelector(cellText, RangeEnum::ER_WarnLow))
				{
					m_Grid.GetCell(pItem->iRow, WarnLowColumn)->SetText(cellText.c_str());
					m_featureData[pItem->iRow - 1].set_range_warn_low_indirect(cellText);
					m_Grid.Refresh();
				}
			}
			break;
		case FailLowButtonColumn:
			if (m_featureData[pItem->iRow - 1].is_range())
			{
				std::string cellText = m_Grid.GetCell(pItem->iRow, FailLowColumn)->GetText();
				if (ShowObjectSelector(cellText, RangeEnum::ER_FailLow))
				{
					m_Grid.GetCell(pItem->iRow, FailLowColumn)->SetText(cellText.c_str());
					m_featureData[pItem->iRow - 1].set_range_fail_low_indirect(cellText);
					m_Grid.Refresh();
				}
			}
			break;
		default:
			//nothing to do
			return;
		}
	}

	void BlobAnalyzer2Range::OnGridBeginEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;
		*pResult = (FailHighColumn == pItem->iColumn || WarnHighColumn == pItem->iColumn || WarnLowColumn == pItem->iColumn || FailLowColumn == pItem->iColumn) ? 0 : -1;
	}

	void BlobAnalyzer2Range::OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;

		if (0 < pItem->iRow && m_featureData.size() >= pItem->iRow)
		{
			std::string cellText = m_Grid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
			switch (pItem->iColumn)
			{
			case FailHighColumn:
				m_featureData[pItem->iRow - 1].set_range_fail_high_indirect(cellText);
				break;
			case WarnHighColumn:
				m_featureData[pItem->iRow - 1].set_range_warn_high_indirect(cellText);
				break;
			case WarnLowColumn:
				m_featureData[pItem->iRow - 1].set_range_warn_low_indirect(cellText);
				break;
			case FailLowColumn:
				m_featureData[pItem->iRow - 1].set_range_fail_low_indirect(cellText);
				break;
			}
		}

		*pResult = 0;
	}

	void BlobAnalyzer2Range::OnBnClickedFailHighIndirect()
	{
		std::string Value(m_FailHigh);
		if (ShowObjectSelector(Value, RangeEnum::ER_FailHigh))
		{
			m_FailHigh = Value.c_str();
			UpdateData(false);
		}
	}

	void BlobAnalyzer2Range::OnBnClickedWarnlHighIndirect()
	{
		std::string Value(m_WarnHigh);
		if (ShowObjectSelector(Value, RangeEnum::ER_WarnHigh))
		{
			m_WarnHigh = Value.c_str();
			UpdateData(false);
		}
	}

	void BlobAnalyzer2Range::OnBnClickedWarnLowIndirect()
	{
		std::string Value(m_WarnLow);
		if (ShowObjectSelector(Value, RangeEnum::ER_WarnLow))
		{
			m_WarnLow = Value.c_str();
			UpdateData(false);
		}
	}

	void BlobAnalyzer2Range::OnBnClickedFailedLowIndirect()
	{
		std::string Value(m_FailLow);
		if (ShowObjectSelector(Value, RangeEnum::ER_FailLow))
		{
			m_FailLow = Value.c_str();
			UpdateData(false);
		}
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	bool BlobAnalyzer2Range::setInspectionData()
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
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				SvStl::MessageContainerVector tmpMessages = SvPb::convertProtobufToMessageVector(responseCmd.setfeaturesresponse().messages());
				if (0 < tmpMessages.size())
				{
					Msg.setMessage(tmpMessages[0].getMessage());
				}
				else
				{
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SetFeatureParameterFailed, SvStl::SourceFileParams(StdMessageParams));
				}

				const auto& range = responseCmd.setfeaturesresponse().error_list(0);
				auto iter = std::find_if(g_columnRangeDefArray.begin(), g_columnRangeDefArray.end(), [range](const auto& rEntry) { return rEntry.second.m_protoBufFieldId == range.fieldid(); });
				if (g_columnRangeDefArray.end() != iter)
				{
					m_Grid.SetSelectedRange(range.row(), iter->first, range.row(), iter->first);
					m_Grid.SetFocusCell(range.row(), iter->first);
					CRect rect;
					if (m_Grid.GetCellRect(range.row(), iter->first, rect))
					{
						SvGcl::GridCellBase* pCell = m_Grid.GetCell(range.row(), iter->first);
						if (pCell)
						{
							pCell->Edit(range.row(), iter->first, rect, CPoint(-1, -1), IDC_INPLACE_CONTROL, 0);
						}
					}
				}
				hResult = E_FAIL;
			}
		}

		if (S_OK != hResult)
		{
			return false;
		}

		try
		{
			// Validate Entered data for existance and if within bounds		
			m_NumberRangeController.IsFieldValid(SvStl::Tid_FailHigh, static_cast<LPCSTR>(m_FailHigh));
			m_NumberRangeController.IsFieldValid(SvStl::Tid_FailLow, static_cast<LPCSTR>(m_FailLow));
			m_NumberRangeController.IsFieldValid(SvStl::Tid_WarnHigh, static_cast<LPCSTR>(m_WarnHigh));
			m_NumberRangeController.IsFieldValid(SvStl::Tid_WarnLow, static_cast<LPCSTR>(m_WarnLow));

			m_NumberRangeController.Set(m_NumberRangeController.FailHigh, static_cast<LPCSTR>(m_FailHigh));
			m_NumberRangeController.Set(m_NumberRangeController.FailLow, static_cast<LPCSTR>(m_FailLow));
			m_NumberRangeController.Set(m_NumberRangeController.WarnHigh, static_cast<LPCSTR>(m_WarnHigh));
			m_NumberRangeController.Set(m_NumberRangeController.WarnLow, static_cast<LPCSTR>(m_WarnLow));

			hResult = m_NumberRangeController.Commit();
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(rSvE.getMessage());
			return false;
		}

		return (S_OK == hResult);
	}

	void BlobAnalyzer2Range::initGridControl()
	{
		m_Grid.SetFixedRowCount(cHeaderSize);
		m_Grid.SetFixedColumnCount(0);
		m_Grid.SetRowResize(FALSE);
		m_Grid.SetColumnResize(true);
		m_Grid.AllowReorderColumn(false);
		m_Grid.EnableDragAndDrop(FALSE);
		m_Grid.SetEditable(true);
		m_Grid.SetColumnCount(static_cast<int>(g_columnRangeDefArray.size()));
		SvGcl::GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
		Item.row = 0;
		Item.crBkClr = CLR_DEFAULT;
		Item.nFormat = DT_LEFT | DT_VCENTER | DT_WORDBREAK;
		for (const auto& rData : g_columnRangeDefArray)
		{
			m_Grid.SetColumnWidth(rData.first, rData.second.m_columnSize);
			Item.col = rData.first;
			Item.strText = rData.second.m_name.c_str();
			m_Grid.SetItem(&Item);
		}
	}

	void BlobAnalyzer2Range::FillGridControl()
	{
		m_Grid.SetRowCount(m_featureData.size() + 1);
		for (int i = 0; m_featureData.size() > i; ++i)
		{
			auto row = i + 1;
			m_Grid.SetItemText(row, NameColumn, m_featureData[i].name().c_str());
			m_Grid.SetItemState(row, NameColumn, m_Grid.GetItemState(row, NameColumn) | GVIS_READONLY);
			using namespace SvGcl;
			m_Grid.SetCellType(row, RangeEnableColumn, RUNTIME_CLASS(GridCellCheck));
			auto* pCell = dynamic_cast<SvGcl::GridCellCheck*>(m_Grid.GetCell(row, RangeEnableColumn));
			if (nullptr != pCell)
			{
				pCell->SetCheck(m_featureData[i].is_range());
			}
			if (m_featureData[i].is_range())
			{
				std::string tmp = m_featureData[i].range_fail_high_indirect().empty() ? std::to_string(m_featureData[i].range_fail_high()) : m_featureData[i].range_fail_high_indirect();
				m_Grid.SetItemText(row, FailHighColumn, tmp.c_str());
				m_Grid.SetItemState(row, FailHighColumn, m_Grid.GetItemState(row, FailHighColumn) & (~GVIS_READONLY));
				SvGcl::GV_ITEM buttonItem;
				buttonItem.mask = GVIF_IMAGE;
				buttonItem.iImage = 0;
				buttonItem.row = row;
				buttonItem.col = FailHighButtonColumn;
				m_Grid.SetItem(&buttonItem);
				tmp = m_featureData[i].range_warn_high_indirect().empty() ? std::to_string(m_featureData[i].range_warn_high()) : m_featureData[i].range_warn_high_indirect();
				m_Grid.SetItemText(row, WarnHighColumn, tmp.c_str());
				m_Grid.SetItemState(row, WarnHighColumn, m_Grid.GetItemState(row, WarnHighColumn) & (~GVIS_READONLY));
				buttonItem.col = WarnHighButtonColumn;
				m_Grid.SetItem(&buttonItem);
				tmp = m_featureData[i].range_warn_low_indirect().empty() ? std::to_string(m_featureData[i].range_warn_low()) : m_featureData[i].range_warn_low_indirect();
				m_Grid.SetItemText(row, WarnLowColumn, tmp.c_str());
				m_Grid.SetItemState(row, WarnLowColumn, m_Grid.GetItemState(row, WarnLowColumn) & (~GVIS_READONLY));
				buttonItem.col = WarnLowButtonColumn;
				m_Grid.SetItem(&buttonItem);
				tmp = m_featureData[i].range_fail_low_indirect().empty() ? std::to_string(m_featureData[i].range_fail_low()) : m_featureData[i].range_fail_low_indirect();
				m_Grid.SetItemText(row, FailLowColumn, tmp.c_str());
				m_Grid.SetItemState(row, FailLowColumn, m_Grid.GetItemState(row, FailLowColumn) & (~GVIS_READONLY));
				buttonItem.col = FailLowButtonColumn;
				m_Grid.SetItem(&buttonItem);
			}
			else
			{
				SvGcl::GV_ITEM buttonItem;
				buttonItem.mask = GVIF_IMAGE;
				buttonItem.iImage = -1;
				buttonItem.row = row;

				m_Grid.SetItemText(row, FailHighColumn, "");
				m_Grid.SetItemState(row, FailHighColumn, m_Grid.GetItemState(row, FailHighColumn) | GVIS_READONLY);
				m_Grid.SetItemState(row, FailHighButtonColumn, m_Grid.GetItemState(row, FailHighButtonColumn) | GVIS_READONLY);
				buttonItem.col = FailHighButtonColumn;
				m_Grid.SetItem(&buttonItem);
				m_Grid.SetItemText(row, WarnHighColumn, "");
				m_Grid.SetItemState(row, WarnHighColumn, m_Grid.GetItemState(row, WarnHighColumn) | GVIS_READONLY);
				m_Grid.SetItemState(row, WarnHighButtonColumn, m_Grid.GetItemState(row, WarnHighButtonColumn) | GVIS_READONLY);
				buttonItem.col = WarnHighButtonColumn;
				m_Grid.SetItem(&buttonItem);
				m_Grid.SetItemText(row, WarnLowColumn, "");
				m_Grid.SetItemState(row, WarnLowColumn, m_Grid.GetItemState(row, WarnLowColumn) | GVIS_READONLY);
				m_Grid.SetItemState(row, WarnLowButtonColumn, m_Grid.GetItemState(row, WarnLowButtonColumn) | GVIS_READONLY);
				buttonItem.col = WarnLowButtonColumn;
				m_Grid.SetItem(&buttonItem);
				m_Grid.SetItemText(row, FailLowColumn, "");
				m_Grid.SetItemState(row, FailLowColumn, m_Grid.GetItemState(row, FailLowColumn) | GVIS_READONLY);
				m_Grid.SetItemState(row, FailLowButtonColumn, m_Grid.GetItemState(row, FailLowButtonColumn) | GVIS_READONLY);
				buttonItem.col = FailLowButtonColumn;
				m_Grid.SetItem(&buttonItem);
			}
		}

		m_Grid.Refresh();
	}

	void BlobAnalyzer2Range::showContextMenu(CPoint point)
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

	HRESULT BlobAnalyzer2Range::loadFeatureData()
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

	void BlobAnalyzer2Range::setDlgData()
	{
		try
		{
			m_FailHigh = m_NumberRangeController.Get(m_NumberRangeController.FailHigh).c_str();
			m_FailLow = m_NumberRangeController.Get(m_NumberRangeController.FailLow).c_str();
			m_WarnHigh = m_NumberRangeController.Get(m_NumberRangeController.WarnHigh).c_str();
			m_WarnLow = m_NumberRangeController.Get(m_NumberRangeController.WarnLow).c_str();
		}
		catch (...)
		{
			assert(false);
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Init_RangeControlFailed, SvStl::SourceFileParams(StdMessageParams));
		}
		UpdateData(false);
	}

	bool BlobAnalyzer2Range::ShowObjectSelector(std::string& rName, RangeEnum::ERange fieldEnum)
	{
		std::string Title = m_NumberRangeController.GetOwnerName();
		Title += _T(": ");
		Title += RangeEnum::ERange2String(fieldEnum).c_str();

		return m_objectSelector.Show(rName, Title, this, SvPb::allNumberValueObjects, { m_toolID }, m_toolID);
	}
#pragma endregion Private Mehods
} //namespace SvOg
