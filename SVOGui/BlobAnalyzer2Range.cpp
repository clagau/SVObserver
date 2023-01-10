//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobAnalyzer2Range.cpp
/// This is the class for the range tab of Blob Analyzer 2
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BlobAnalyzer2Range.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "GridCtrlLibrary/GridCellCheck.h"
#include "SVOGuiUtility/DisplayHelper.h"
#include "SVOGuiUtility/BlobAnalyzer2Helper.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	namespace
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
			{FailHighColumn, {"Fail High", cRangeColumnSize, SvPb::FeatureData::kRangeFailHighFieldNumber }},
			{FailHighButtonColumn, {"", cBoxColumnSize}},
			{WarnHighColumn, {"Warn High", cRangeColumnSize, SvPb::FeatureData::kRangeWarnHighFieldNumber }},
			{WarnHighButtonColumn, {"", cBoxColumnSize}},
			{WarnLowColumn, {"Warn Low", cRangeColumnSize, SvPb::FeatureData::kRangeWarnLowFieldNumber }},
			{WarnLowButtonColumn, {"", cBoxColumnSize}},
			{FailLowColumn, {"Fail Low", cRangeColumnSize, SvPb::FeatureData::kRangeFailLowFieldNumber }},
			{FailLowButtonColumn, {"", cBoxColumnSize}},
		};
	}

	BEGIN_MESSAGE_MAP(BlobAnalyzer2Range, CPropertyPage)
		//{{AFX_MSG_MAP(BlobAnalyzer2Range)
		ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_FAILHIGH, IDC_BUTTON_FAILLOW, OnButtonRange)
		ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_FAILHIGH, IDC_EDIT_FAILLOW, OnKillFocusRange)
		ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
		ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID, OnGridBeginEdit)
		ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

#pragma region Constructor
	BlobAnalyzer2Range::BlobAnalyzer2Range(uint32_t inspectionId, uint32_t taskObjectId)
		: CPropertyPage(BlobAnalyzer2Range::IDD)
		, m_InspectionID(inspectionId)
		, m_TaskObjectID(taskObjectId)
		, m_NumberRangeController(inspectionId, m_TaskObjectID)
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
		DDX_Control(pDX, IDC_BUTTON_FAILHIGH, m_RangeButtons[RangeEnum::ER_FailHigh]);
		DDX_Control(pDX, IDC_BUTTON_WARNHIGH, m_RangeButtons[RangeEnum::ER_WarnHigh]);
		DDX_Control(pDX, IDC_BUTTON_WARNLOW, m_RangeButtons[RangeEnum::ER_WarnLow]);
		DDX_Control(pDX, IDC_BUTTON_FAILLOW, m_RangeButtons[RangeEnum::ER_FailLow]);
		DDX_Control(pDX, IDC_EDIT_FAILHIGH, m_RangeEdits[RangeEnum::ER_FailHigh]);
		DDX_Control(pDX, IDC_EDIT_WARNHIGH, m_RangeEdits[RangeEnum::ER_WarnHigh]);
		DDX_Control(pDX, IDC_EDIT_WARNLOW, m_RangeEdits[RangeEnum::ER_WarnLow]);
		DDX_Control(pDX, IDC_EDIT_FAILLOW, m_RangeEdits[RangeEnum::ER_FailLow]);
		//}}AFX_DATA_MAP
	}

	BOOL BlobAnalyzer2Range::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		//(HBITMAP) is a call to the overloaded function operator HBITMAP and no c style cast
		SvOgu::DisplayHelper::setIconListToGrid(m_ImageList, m_downArrowBitmap, m_Grid);
		for (int i = 0; i < m_RangeWidgets.size(); ++i)
		{
			m_RangeWidgets[i] = m_NumberRangeController.createWidget(static_cast<RangeEnum::ERange>(i), m_RangeEdits[i], m_RangeButtons[i]);
		}

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

		auto* pWnd = GetFocus();
		if (nullptr != pWnd)
		{
			auto nId = pWnd->GetDlgCtrlID();
			if (std::ranges::any_of(m_RangeEdits, [nId](const auto& rEntry) { return nId == rEntry.GetDlgCtrlID(); }))
			{
				OnKillFocusRange(nId);
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

		std::string Title = m_NumberRangeController.GetOwnerName();
		Title += _T(": ");

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
				Title += RangeEnum::ERange2String(RangeEnum::ER_FailHigh);
				SvOgu::startObjectSelector(m_Grid, Title, pItem->iRow, FailHighColumn, m_InspectionID, m_featureData[pItem->iRow - 1].range_fail_high_id(), *m_featureData[pItem->iRow - 1].mutable_range_fail_high());
			}
			break;
		case WarnHighButtonColumn:
			if (m_featureData[pItem->iRow - 1].is_range())
			{
				Title += RangeEnum::ERange2String(RangeEnum::ER_WarnHigh);
				SvOgu::startObjectSelector(m_Grid, Title, pItem->iRow, WarnHighColumn, m_InspectionID, m_featureData[pItem->iRow - 1].range_warn_high_id(), *m_featureData[pItem->iRow - 1].mutable_range_warn_high());
			}
			break;
		case WarnLowButtonColumn:
			if (m_featureData[pItem->iRow - 1].is_range())
			{
				Title += RangeEnum::ERange2String(RangeEnum::ER_WarnLow);
				SvOgu::startObjectSelector(m_Grid, Title, pItem->iRow, WarnLowColumn, m_InspectionID, m_featureData[pItem->iRow - 1].range_warn_low_id(), *m_featureData[pItem->iRow - 1].mutable_range_warn_low());
			}
			break;
		case FailLowButtonColumn:
			if (m_featureData[pItem->iRow - 1].is_range())
			{
				Title += RangeEnum::ERange2String(RangeEnum::ER_FailLow);
				SvOgu::startObjectSelector(m_Grid, Title, pItem->iRow, FailLowButtonColumn, m_InspectionID, m_featureData[pItem->iRow - 1].range_fail_low_id(), *m_featureData[pItem->iRow - 1].mutable_range_fail_low());
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
			SvPb::LinkedValue* pLinkedValue = nullptr;
			std::string cellText = m_Grid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
			switch (pItem->iColumn)
			{
				case FailHighColumn:
					pLinkedValue = m_featureData[pItem->iRow - 1].mutable_range_fail_high();
					break;
				case WarnHighColumn:
					pLinkedValue = m_featureData[pItem->iRow - 1].mutable_range_warn_high();
					break;
				case WarnLowColumn:
					pLinkedValue = m_featureData[pItem->iRow - 1].mutable_range_warn_low();
					break;
				case FailLowColumn:
					pLinkedValue = m_featureData[pItem->iRow - 1].mutable_range_fail_low();
					break;
			}

			Log_Assert(nullptr != pLinkedValue);
			if (nullptr != pLinkedValue && SvPb::LinkedSelectedOption::DirectValue == pLinkedValue->option())
			{
				variant_t tmp {cellText.c_str()};
				SvStl::MessageContainer msgContainer;
				bool isValid = (S_OK == ::VariantChangeTypeEx(&tmp, &tmp, SvDef::LCID_USA, VARIANT_ALPHABOOL, VT_R8));
				if (false == isValid)
				{
					SvDef::StringVector msgList;
					auto iter = g_columnRangeDefArray.find(pItem->iColumn);
					msgList.push_back(g_columnRangeDefArray.end() != iter ? iter->second.m_name : "");
					msgContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
					SvStl::MessageManager Msg(SvStl::MsgType::Display);
					Msg.setMessage(msgContainer.getMessage());
					*pResult = -1;
					return;
				}
				else
				{
					SvPb::ConvertVariantToProtobuf(tmp, pLinkedValue->mutable_directvalue());
					SvPb::ConvertVariantToProtobuf(tmp, pLinkedValue->mutable_value());
				}
			}
		}

		*pResult = 0;
	}

	void BlobAnalyzer2Range::OnButtonRange(UINT nID)
	{
		int index(nID - IDC_BUTTON_FAILHIGH);

		Log_Assert(m_RangeWidgets.size() > index && 0 <= index && m_RangeWidgets[index]);
		if (m_RangeWidgets.size() > index && 0 <= index && m_RangeWidgets[index])
		{
			m_RangeWidgets[index]->OnButton();
		}
	}

	void BlobAnalyzer2Range::OnKillFocusRange(UINT nID)
	{
		int index(nID - IDC_EDIT_FAILHIGH);

		Log_Assert(m_RangeWidgets.size() > index && 0 <= index && m_RangeWidgets[index]);
		if (m_RangeWidgets.size() > index && 0 <= index && m_RangeWidgets[index])
		{
			m_RangeWidgets[index]->EditboxToValue();
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
				for (const auto& error : responseCmd.setfeaturesresponse().error_list())
				{
					auto iter = std::find_if(g_columnRangeDefArray.begin(), g_columnRangeDefArray.end(), [error](const auto& rEntry) { return rEntry.second.m_protoBufFieldId == error.fieldid(); });
					if (g_columnRangeDefArray.end() != iter)
					{
						m_Grid.SetSelectedRange(error.row(), iter->first, error.row(), iter->first);
						m_Grid.SetFocusCell(error.row(), iter->first);
						CRect rect;
						if (m_Grid.GetCellRect(error.row(), iter->first, rect))
						{
							SvGcl::GridCellBase* pCell = m_Grid.GetCell(error.row(), iter->first);
							if (pCell)
							{
								pCell->Edit(error.row(), iter->first, rect, CPoint(-1, -1), IDC_INPLACE_CONTROL, 0);
							}
						}
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
						hResult = E_FAIL;
						break;
					}
				}				
			}
		}

		if (S_OK != hResult)
		{
			return false;
		}

		try
		{
			// Validate Entered data for existence and if within bounds		
			m_NumberRangeController.IsFieldValid(RangeEnum::ERange::ER_FailHigh);
			m_NumberRangeController.IsFieldValid(RangeEnum::ERange::ER_FailLow);
			m_NumberRangeController.IsFieldValid(RangeEnum::ERange::ER_WarnHigh);
			m_NumberRangeController.IsFieldValid(RangeEnum::ERange::ER_WarnLow);

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
				setValueColumn(m_Grid, row, FailHighColumn, SvOgu::convertLinkedValue(m_featureData[i].range_fail_high()));
				SvGcl::GV_ITEM buttonItem;
				buttonItem.mask = GVIF_IMAGE;
				buttonItem.iImage = 0;
				buttonItem.row = row;
				buttonItem.col = FailHighButtonColumn;
				m_Grid.SetItem(&buttonItem);
				setValueColumn(m_Grid, row, WarnHighColumn, SvOgu::convertLinkedValue(m_featureData[i].range_warn_high()));
				buttonItem.col = WarnHighButtonColumn;
				m_Grid.SetItem(&buttonItem);
				setValueColumn(m_Grid, row, WarnLowColumn, SvOgu::convertLinkedValue(m_featureData[i].range_warn_low()));
				buttonItem.col = WarnLowButtonColumn;
				m_Grid.SetItem(&buttonItem);
				setValueColumn(m_Grid, row, FailLowColumn, SvOgu::convertLinkedValue(m_featureData[i].range_fail_low()));
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
#pragma endregion Private Mehods
} //namespace SvOg
