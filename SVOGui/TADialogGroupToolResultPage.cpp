//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file TADialogGroupToolResultPage.cpp
/// This is the class for the input tab of Group Tool
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADialogGroupToolResultPage.h"
#include "ObjectSelectorController.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "GridCtrlLibrary/GridCellCombo.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
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
	constexpr int cTypeColumnSize = 100;
	constexpr int cNameColumnSize = 150;
	constexpr int cIndirectNameColumnSize = 320;
	constexpr int cBoxColumnSize = 25;

	struct ColumnDef
	{
		std::string m_name;
		int m_columnSize;

		ColumnDef(const std::string& name, int columnSize) : m_name(name), m_columnSize(columnSize) {};
	};

	enum ColumnPos
	{
		NameColumn = 0,
		TypeColumn,
		ValueColumn,
		ValueButtonColumn,
	};

	std::map<int, ColumnDef> g_columnResultObjectsDefArray = { { NameColumn, ColumnDef{"Name", cNameColumnSize}},
		{TypeColumn, {"Type", cTypeColumnSize }},
		{ValueColumn, {"Value", cIndirectNameColumnSize }},
		{ValueButtonColumn, {"", cBoxColumnSize}},
	};

	//define in TADialogGroupToolInputPage.cpp
	std::string getDefaultString(SvPb::InputTypeEnum type);


	BEGIN_MESSAGE_MAP(TADialogGroupToolResultPage, CPropertyPage)
		//{{AFX_MSG_MAP(TaDlgGroupToolInput)
		ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
		ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
		ON_BN_CLICKED(IDC_BUTTON_MOVEUP, OnBnClickedMoveUp)
		ON_BN_CLICKED(IDC_BUTTON_MOVEDOWN, OnBnClickedMoveDown)
		ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
		ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
		ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnSelectionChanged)
		ON_COMMAND(ID_ADD_COLUMN, OnBnClickedButtonAdd)
		ON_COMMAND(ID_REMOVE_COLUMNS, OnBnClickedButtonRemove)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

#pragma region Constructor
	TADialogGroupToolResultPage::TADialogGroupToolResultPage(uint32_t inspectionId, uint32_t toolId, uint32_t taskId)
		: CPropertyPage(TADialogGroupToolResultPage::IDD, IDS_CLASSNAME_RESULT_PARAMETER_TASK)
		, m_InspectionID(inspectionId)
		, m_toolId(toolId)
		, m_TaskObjectID(taskId)
		, m_Values{ SvOg::BoundValues{ inspectionId, taskId } }
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	bool TADialogGroupToolResultPage::QueryAllowExit()
	{
		return setInspectionData();
	}
#pragma endregion Public Methods

#pragma region Protected Methods
	void TADialogGroupToolResultPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TADialogGroupToolResultPage)
		DDX_Control(pDX, IDC_GRID, m_Grid);
		//}}AFX_DATA_MAP
	}

	constexpr int c_maxValues = 50;

	BOOL TADialogGroupToolResultPage::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		DisplayHelper::setIconListToGrid(m_ImageList, m_downArrowBitmap, m_Grid);

		loadDataList();
		initGridControl();
		FillGridControl();

		UpdateData(FALSE);

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	BOOL TADialogGroupToolResultPage::OnSetActive()
	{
		BOOL bOk = CPropertyPage::OnSetActive();
		if (bOk)
		{
			loadDataList();
			FillGridControl();
		}
		return bOk;
	}

	BOOL TADialogGroupToolResultPage::OnKillActive()
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

	void TADialogGroupToolResultPage::OnGridClick(NMHDR* pNotifyStruct, LRESULT*)
	{
		auto* pItem = reinterpret_cast<SvGcl::NM_GRIDVIEW*>(pNotifyStruct);
		if (1 > pItem->iRow || m_resultData.size() < pItem->iRow)
		{
			return;
		}

		switch (pItem->iColumn)
		{
		case ValueButtonColumn:
		{
			const auto& type = getType(m_Grid.GetCell(pItem->iRow, TypeColumn)->GetText());
			std::string cellText = m_Grid.GetCell(pItem->iRow, ValueColumn)->GetText();
			bool isChanged = false;
			switch (type.second)
			{
			case 0: //decimal
			{
				ObjectSelectorController objectSelector(m_InspectionID);
				isChanged = objectSelector.Show(cellText, m_Grid.GetCell(pItem->iRow, NameColumn)->GetText(), this, SvPb::allNumberValueObjects, { m_TaskObjectID });
				break;
			}
			case 1: //text
			{
				ObjectSelectorController objectSelector(m_InspectionID, SvDef::InvalidObjectId, SvPb::ObjectAttributes::viewable);
				isChanged = objectSelector.Show(cellText, m_Grid.GetCell(pItem->iRow, NameColumn)->GetText(), this, SvPb::allValueObjects, { m_TaskObjectID });
				break;
			}
			case 2: //table
			{
				ObjectSelectorController objectSelector(m_InspectionID, SvDef::InvalidObjectId, SvPb::ObjectAttributes::taskObject);
				isChanged = objectSelector.Show(cellText, m_Grid.GetCell(pItem->iRow, NameColumn)->GetText(), this, SvPb::tableObjects, { m_TaskObjectID });
				break;
			}
			case 3: //gray image
			{
				ObjectSelectorController objectSelector(m_InspectionID, SvDef::InvalidObjectId, SvPb::ObjectAttributes::archivableImage);
				isChanged = objectSelector.Show(cellText, m_Grid.GetCell(pItem->iRow, NameColumn)->GetText(), this, SvPb::grayImageObjects, { m_TaskObjectID });
				break;
			}
			case 4: //color image
			{
				ObjectSelectorController objectSelector(m_InspectionID, SvDef::InvalidObjectId, SvPb::ObjectAttributes::archivableImage);
				isChanged = objectSelector.Show(cellText, m_Grid.GetCell(pItem->iRow, NameColumn)->GetText(), this, SvPb::colorImageObjects, { m_TaskObjectID });
				break;
			}
			case 5: //image
			{
				ObjectSelectorController objectSelector(m_InspectionID, SvDef::InvalidObjectId, SvPb::ObjectAttributes::archivableImage);
				isChanged = objectSelector.Show(cellText, m_Grid.GetCell(pItem->iRow, NameColumn)->GetText(), this, SvPb::allImageObjects, { m_TaskObjectID });
				break;
			}
			default:
				break;
			}
			if (isChanged)
			{
				m_resultData[pItem->iRow - 1].m_ValueStr = cellText;
				m_Grid.GetCell(pItem->iRow, ValueColumn)->SetText(cellText.c_str());
				m_Grid.Refresh();
			}
		}
		break;
		default:
			//nothing to do
			return;
		}
	}

	void TADialogGroupToolResultPage::OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;
		bool bAcceptChange = true;

		if (0 < pItem->iRow && m_resultData.size() >= pItem->iRow)
		{
			std::string cellText = m_Grid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
			switch (pItem->iColumn)
			{
			case NameColumn:
				if (m_resultData[pItem->iRow - 1].m_name != cellText)
				{
					if (m_resultData.end() != std::find_if(m_resultData.begin(), m_resultData.end(), [cellText](const auto& rEntry) { return rEntry.m_name == cellText; }))
					{
						SvDef::StringVector msgList;
						msgList.emplace_back(cellText);
						SvStl::MessageManager Msg(SvStl::MsgType::Display);
						Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RenameError_DuplicateName, msgList, SvStl::SourceFileParams(StdMessageParams));
						bAcceptChange = false;
					}
					else
					{
						m_resultData[pItem->iRow - 1].m_name = cellText;
					}
				}
				break;
			case TypeColumn:
			{
				const auto& type = getType(cellText);
				m_resultData[pItem->iRow - 1].m_type = static_cast<SvPb::InputTypeEnum>(type.second);
				FillGridControl();
				break;
			}
			case ValueColumn:
			{
				m_resultData[pItem->iRow - 1].m_ValueStr = cellText;
			}
			}
		}

		*pResult = (bAcceptChange) ? 0 : -1;
	}

	void TADialogGroupToolResultPage::OnSelectionChanged(NMHDR*, LRESULT*)
	{
		UpdateEnableButtons();
	}

	void TADialogGroupToolResultPage::OnBnClickedButtonAdd()
	{
		if (m_resultData.size() < c_maxValues)
		{
			GroupResultData data;
			if (0 < std::count_if(m_resultData.begin(), m_resultData.end(), [data](const auto& rEntry) { return rEntry.m_name == data.m_name; }))
			{ //duplicate
				std::string newName;
				int additionalValue = 1;
				do
				{
					newName = c_resultName + std::to_string(additionalValue++);
				} while (0 < std::count_if(m_resultData.begin(), m_resultData.end(), [newName](const auto& rEntry) { return rEntry.m_name == newName; }));
				data.m_name = newName;
			}
			m_resultData.emplace_back(std::move(data));
		}

		FillGridControl();
	}

	void TADialogGroupToolResultPage::OnBnClickedButtonRemove()
	{
		SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
		for (int i = Selection.GetMaxRow(); i >= Selection.GetMinRow() && i <= m_resultData.size(); --i)
		{
			for (int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++)
			{
				if (m_Grid.IsCellSelected(i, j) && 0 < i && m_resultData.size() > i - 1)
				{
					m_resultData.erase(m_resultData.begin() + (i - 1));
					break;
				}
			}
		}
		FillGridControl();
	}

	void TADialogGroupToolResultPage::OnBnClickedMoveUp()
	{
		SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
		if (Selection.GetMinRow() == Selection.GetMaxRow() && 1 < Selection.GetMinRow())
		{
			std::swap(m_resultData[Selection.GetMinRow() - 1], m_resultData[Selection.GetMinRow() - 2]);
			Selection.SetMinRow(Selection.GetMinRow() - 1);
			Selection.SetMaxRow(Selection.GetMaxRow() - 1);
			m_Grid.SetSelectedRange(Selection, true);
			m_Grid.SetFocusCell(Selection.GetMaxRow(), Selection.GetMaxCol());
			FillGridControl();
		}
	}

	void TADialogGroupToolResultPage::OnBnClickedMoveDown()
	{
		SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
		if (Selection.GetMinRow() == Selection.GetMaxRow() && 0 < Selection.GetMinRow() && Selection.GetMinRow() + 1 < m_Grid.GetRowCount())
		{
			std::swap(m_resultData[Selection.GetMinRow() - 1], m_resultData[Selection.GetMinRow()]);
			Selection.SetMinRow(Selection.GetMinRow() + 1);
			Selection.SetMaxRow(Selection.GetMaxRow() + 1);
			m_Grid.SetSelectedRange(Selection, true);
			m_Grid.SetFocusCell(Selection.GetMaxRow(), Selection.GetMaxCol());
			FillGridControl();
		}
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	bool TADialogGroupToolResultPage::setInspectionData()
	{
		UpdateData(TRUE);
		try
		{
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest = requestCmd.mutable_setandsortembeddedvaluerequest();
			pRequest->set_ownerid(m_TaskObjectID);
			for (const auto& data : m_resultData)
			{
				auto* pTmp = pRequest->add_embeddedlist();
				pTmp->set_name(data.m_name);
				pTmp->set_oldembeddedid(data.m_oldEmbeddedId);
				pTmp->set_type(data.m_type);
				pTmp->set_defaultvalue(getDefaultString(data.m_type));
				pTmp->set_value(data.m_ValueStr);
			}

			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK != hr)
			{
				SvStl::MessageContainerVector messages;
				if (responseCmd.has_setandsortembeddedvalueresponse())
				{
					auto& rMsg = responseCmd.setandsortembeddedvalueresponse();
					if (rMsg.has_errormessages() && 0 < rMsg.errormessages().messages_size())
					{
						messages = convertProtobufToMessageVector(rMsg.errormessages());
					}
				}

				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				if (0 < messages.size())
				{
					Msg.setMessage(messages[0].getMessage());
				}
				else
				{
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams));
				}
			}
			return (S_OK == hr);
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(rSvE.getMessage());
			return false;
		}
	}

	void TADialogGroupToolResultPage::initGridControl()
	{
		m_Grid.SetFixedRowCount(cHeaderSize);
		m_Grid.SetFixedColumnCount(0);
		m_Grid.SetRowResize(FALSE);
		m_Grid.SetColumnResize(true);
		m_Grid.AllowReorderColumn(false);
		m_Grid.EnableDragAndDrop(FALSE);
		m_Grid.SetEditable(true);
		m_Grid.SetColumnCount(static_cast<int>(g_columnResultObjectsDefArray.size()));
		SvGcl::GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
		Item.row = 0;
		Item.crBkClr = CLR_DEFAULT;
		Item.nFormat = DT_LEFT | DT_VCENTER | DT_WORDBREAK;
		for (const auto& rData : g_columnResultObjectsDefArray)
		{
			m_Grid.SetColumnWidth(rData.first, rData.second.m_columnSize);
			Item.col = rData.first;
			Item.strText = rData.second.m_name.c_str();
			m_Grid.SetItem(&Item);
		}
	}

	void TADialogGroupToolResultPage::FillGridControl()
	{
		CStringArray typeOptions;
		SvOi::NameValueVector typePairs;
		int numberOfObjects = static_cast<int>(m_resultData.size());
		if (0 < numberOfObjects)
		{
			typePairs = m_Values.GetEnumTypes(SvPb::ResultObjectTypeEId);
			for (const auto& entry : typePairs)
			{
				typeOptions.Add(entry.first.c_str());
			}
		}
		m_Grid.SetRowCount(numberOfObjects + 1);
		for (int i = 0; numberOfObjects > i; ++i)
		{
			auto row = i + 1;
			m_Grid.SetItemText(row, NameColumn, m_resultData[i].m_name.c_str());
			m_Grid.SetItemState(row, NameColumn, m_Grid.GetItemState(row, NameColumn) & (~GVIS_READONLY));
			using namespace SvGcl;
			m_Grid.SetCellType(row, TypeColumn, RUNTIME_CLASS(GridCellCombo));
			auto* pCell = dynamic_cast<SvGcl::GridCellCombo*>(m_Grid.GetCell(row, TypeColumn));

			pCell->SetOptions(typeOptions);
			pCell->SetStyle(CBS_DROPDOWNLIST);
			auto type = m_resultData[i].m_type;
			auto iter = std::find_if(typePairs.begin(), typePairs.end(), [type](const auto& rEntry) { return rEntry.second == type; });
			if (typePairs.end() != iter)
			{
				pCell->SetText(iter->first.c_str());
			}
			else
			{
				pCell->SetText(typePairs[0].first.c_str());
			}

			auto value = std::string(static_cast<LPCSTR>(m_Values.Get<_bstr_t>(SvPb::ResultObjectLinkedEId + i)));
			if (m_resultData[i].m_ValueStr.empty())
			{
				m_resultData[i].m_ValueStr = getDefaultString(type);
			}
			m_Grid.SetItemText(row, ValueColumn, m_resultData[i].m_ValueStr.c_str());
			m_Grid.SetItemState(row, ValueColumn, m_Grid.GetItemState(row, ValueColumn) & (~GVIS_READONLY));
			SvGcl::GV_ITEM buttonItem;
			buttonItem.mask = GVIF_IMAGE;
			buttonItem.iImage = 0;
			buttonItem.row = row;
			buttonItem.col = ValueButtonColumn;
			m_Grid.SetItem(&buttonItem);
		}

		m_Grid.Refresh();
		UpdateEnableButtons();
	}

	void TADialogGroupToolResultPage::showContextMenu(CPoint point)
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

	SvOi::NameValuePair TADialogGroupToolResultPage::getType(const std::string& rTypeName)
	{
		const auto& rTypePairs = m_Values.GetEnumTypes(SvPb::ResultObjectTypeEId);
		auto iter = std::find_if(rTypePairs.begin(), rTypePairs.end(), [rTypeName](const auto& rEntry) { return rTypeName == rEntry.first; });
		if (rTypePairs.end() != iter)
		{
			return *iter;
		}
		else
		{
			assert(false);
			return {};
		}
	}

	void TADialogGroupToolResultPage::loadDataList()
	{
		m_resultData.clear();
		m_Values.Init();
		for (int i = 0; c_maxValues > i; ++i)
		{
			if (0 == m_Values.GetAllowedAttributeFlags(SvPb::ResultObjectValueEId + i))
			{	//stop loading values, because it is hidden.
				break;
			}

			GroupResultData data;
			data.m_name = m_Values.GetName(SvPb::ResultObjectValueEId + i);
			data.m_oldEmbeddedId = SvPb::ResultObjectValueEId + i;
			data.m_type = static_cast<SvPb::InputTypeEnum>(m_Values.Get<int>(SvPb::ResultObjectTypeEId + i));
			data.m_ValueStr = std::string(static_cast<LPCSTR>(m_Values.Get<_bstr_t>(SvPb::ResultObjectLinkedEId + i)));
			if (data.m_ValueStr.empty())
			{
				auto valueVar = m_Values.Get<variant_t>(SvPb::ResultObjectValueEId + i);
				if (VT_EMPTY != valueVar.vt)
				{
					data.m_ValueStr = SvUl::VariantToString(valueVar);
				}
			}
			m_resultData.emplace_back(std::move(data));
		}
	}

	void TADialogGroupToolResultPage::UpdateEnableButtons()
	{
		SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
		bool bMoveUpEnable = false;
		bool bMoveDownEnable = false;
		if (Selection.GetMinRow() == Selection.GetMaxRow())
		{
			bMoveUpEnable = (1 < Selection.GetMinRow() && Selection.GetMinRow() <= m_resultData.size());
			bMoveDownEnable = (0 < Selection.GetMinRow() && Selection.GetMinRow() + 2 < m_Grid.GetRowCount());
		}

		GetDlgItem(IDC_BUTTON_MOVEUP)->EnableWindow(bMoveUpEnable);
		GetDlgItem(IDC_BUTTON_MOVEDOWN)->EnableWindow(bMoveDownEnable);
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(-1 != Selection.GetMaxRow() && Selection.GetMinRow() <= m_resultData.size());

		if (SvDef::c_maxTableColumn > m_resultData.size())
		{
			GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(true);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(false);
		}
	}
#pragma endregion Private Mehods
} //namespace SvOg
