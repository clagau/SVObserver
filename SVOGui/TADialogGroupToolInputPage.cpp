//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file TADialogGroupToolInputPage.cpp
/// This is the class for the input tab of Group Tool
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADialogGroupToolInputPage.h"
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
	constexpr int cIndirectNameColumnSize = 220;
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
		DefaultColumn,
		ValueColumn,
		ValueButtonColumn,
	};

	std::map<int, ColumnDef> g_columnInputObjectsDefArray = { { NameColumn, ColumnDef{"Name", cNameColumnSize}},
		{TypeColumn, {"Type", cTypeColumnSize }},
		{DefaultColumn, {"Default", cNameColumnSize}},
		{ValueColumn, {"Value", cIndirectNameColumnSize }},
		{ValueButtonColumn, {"", cBoxColumnSize}},
	};


	std::string getDefaultString(SvPb::InputTypeEnum type)
	{
		switch (type)
		{
		case SvPb::InputTypeEnum::TypeDecimal: //decimal
			return "0";
		default: //do nothing
			return {};
		}
	}

	std::string checkText(SvPb::InputTypeEnum type, const std::string& text)
	{
		std::string result{ text };
		try
		{
			switch (type)
			{
			case SvPb::InputTypeEnum::TypeDecimal:
				// cppcheck-suppress ignoredReturnValue ; only for check if it possible to move string in double. In error case function throws exception.
				std::stod(text);
				break;
			case SvPb::InputTypeEnum::TypeText:
				break;
			default: //do nothing
				result = {};
				break;
			}
		}
		catch (...)
		{
			SvDef::StringVector msgList;
			msgList.push_back(text);
			SvStl::MessageManager msg(SvStl::MsgType::Data);
			msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ConvertTextToVariantFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
			msg.Throw();
		}

		return result;
	}


	BEGIN_MESSAGE_MAP(TADialogGroupToolInputPage, CPropertyPage)
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
	TADialogGroupToolInputPage::TADialogGroupToolInputPage(uint32_t inspectionId, uint32_t toolId, uint32_t taskId)
		: CPropertyPage(TADialogGroupToolInputPage::IDD)
		, m_InspectionID(inspectionId)
		, m_toolId(toolId)
		, m_TaskObjectID(taskId)
		, m_Values{ SvOg::BoundValues{ inspectionId, taskId }}
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	bool TADialogGroupToolInputPage::QueryAllowExit()
	{
		return setInspectionData();
	}
#pragma endregion Public Methods

#pragma region Protected Methods
	void TADialogGroupToolInputPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TADialogGroupToolInputPage)
		DDX_Control(pDX, IDC_GRID, m_Grid);
		//}}AFX_DATA_MAP
	}

	constexpr int c_maxValues = 50;

	BOOL TADialogGroupToolInputPage::OnInitDialog()
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

	BOOL TADialogGroupToolInputPage::OnSetActive()
	{
		BOOL bOk = CPropertyPage::OnSetActive();
		if (bOk)
		{
			loadDataList();
			FillGridControl();
		}
		return bOk;
	}

	BOOL TADialogGroupToolInputPage::OnKillActive()
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

	void TADialogGroupToolInputPage::OnGridClick(NMHDR* pNotifyStruct, LRESULT*)
	{
		auto* pItem = reinterpret_cast<SvGcl::NM_GRIDVIEW*>(pNotifyStruct);
		if (1 > pItem->iRow || m_inputData.size() < pItem->iRow)
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
				isChanged = objectSelector.Show(cellText, m_Grid.GetCell(pItem->iRow, NameColumn)->GetText(), this, SvPb::allNumberValueObjects, { m_toolId });
				break;
			}
			case 1: //text
			{
				ObjectSelectorController objectSelector(m_InspectionID, SvDef::InvalidObjectId, SvPb::ObjectAttributes::viewable);
				isChanged = objectSelector.Show(cellText, m_Grid.GetCell(pItem->iRow, NameColumn)->GetText(), this, SvPb::allValueObjects, { m_toolId });
				break;
			}
			case 2: //table
			{
				ObjectSelectorController objectSelector(m_InspectionID, SvDef::InvalidObjectId, SvPb::ObjectAttributes::taskObject);
				isChanged = objectSelector.Show(cellText, m_Grid.GetCell(pItem->iRow, NameColumn)->GetText(), this, SvPb::tableObjects, { m_toolId });
				break;
			}
			case 3: //gray image
			{
				ObjectSelectorController objectSelector(m_InspectionID, SvDef::InvalidObjectId, SvPb::ObjectAttributes::archivableImage);
				isChanged = objectSelector.Show(cellText, m_Grid.GetCell(pItem->iRow, NameColumn)->GetText(), this, SvPb::grayImageObjects, { m_toolId });
				break;
			}
			case 4: //color image
			{
				ObjectSelectorController objectSelector(m_InspectionID, SvDef::InvalidObjectId, SvPb::ObjectAttributes::archivableImage);
				isChanged = objectSelector.Show(cellText, m_Grid.GetCell(pItem->iRow, NameColumn)->GetText(), this, SvPb::colorImageObjects, { m_toolId });
				break;
			}
			case 5: //image
			{
				ObjectSelectorController objectSelector(m_InspectionID, SvDef::InvalidObjectId, SvPb::ObjectAttributes::archivableImage);
				isChanged = objectSelector.Show(cellText, m_Grid.GetCell(pItem->iRow, NameColumn)->GetText(), this, SvPb::allImageObjects, { m_toolId });
				break;
			}
			default:
				break;
			}
			if (isChanged)
			{
				m_inputData[pItem->iRow - 1].m_ValueStr = cellText;
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

	void TADialogGroupToolInputPage::OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;
		bool bAcceptChange = true;

		if (0 < pItem->iRow && m_inputData.size() >= pItem->iRow)
		{
			std::string cellText = m_Grid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
			switch (pItem->iColumn)
			{
			case NameColumn:
				if (m_inputData[pItem->iRow - 1].m_name != cellText)
				{
					if (m_inputData.end() != std::find_if(m_inputData.begin(), m_inputData.end(), [cellText](const auto& rEntry) { return rEntry.m_name == cellText; }))
					{
						SvDef::StringVector msgList;
						msgList.emplace_back(cellText);
						SvStl::MessageManager Msg(SvStl::MsgType::Display);
						Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RenameError_DuplicateName, msgList, SvStl::SourceFileParams(StdMessageParams));
						bAcceptChange = false;
					}
					else
					{
						m_inputData[pItem->iRow - 1].m_name = cellText;
					}
				}
				break;
			case TypeColumn:
			{
				const auto& type = getType(cellText);
				m_inputData[pItem->iRow - 1].m_type = static_cast<SvPb::InputTypeEnum>(type.second);
				FillGridControl();
				break;
			}
			case DefaultColumn:
			{
				if (SvPb::InputTypeEnum::TypeDecimal == m_inputData[pItem->iRow - 1].m_type || SvPb::InputTypeEnum::TypeText == m_inputData[pItem->iRow - 1].m_type)
				{
					try
					{
						//check if text a correct variant	
						m_inputData[pItem->iRow - 1].m_defaultValue = checkText(m_inputData[pItem->iRow - 1].m_type, cellText);
					}
					catch (const SvStl::MessageContainer& rSvE)
					{
						SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
						Msg.setMessage(rSvE.getMessage());
						bAcceptChange = false;
						m_inputData[pItem->iRow - 1].m_defaultValue = getDefaultString(m_inputData[pItem->iRow - 1].m_type);
						m_Grid.SetItemText(pItem->iRow, DefaultColumn, m_inputData[pItem->iRow - 1].m_defaultValue.c_str());
					}
				}
				else
				{
					m_inputData[pItem->iRow - 1].m_defaultValue = _variant_t{};
				}
				break;
			}
			case ValueColumn:
			{
				m_inputData[pItem->iRow - 1].m_ValueStr = cellText;
			}
			}
		}

		*pResult = (bAcceptChange) ? 0 : -1;
	}

	void TADialogGroupToolInputPage::OnSelectionChanged(NMHDR*, LRESULT*)
	{
		UpdateEnableButtons();
	}

	void TADialogGroupToolInputPage::OnBnClickedButtonAdd()
	{
		if (m_inputData.size() < c_maxValues)
		{
			GroupInputData data;
			if (0 < std::count_if(m_inputData.begin(), m_inputData.end(), [data](const auto& rEntry) { return rEntry.m_name == data.m_name; }))
			{ //duplicate
				std::string newName;
				int additionalValue = 1;
				do
				{
					newName = c_inputName + std::to_string(additionalValue++);
				} while (0 < std::count_if(m_inputData.begin(), m_inputData.end(), [newName](const auto& rEntry) { return rEntry.m_name == newName; }));
				data.m_name = newName;
			}
			m_inputData.emplace_back(std::move(data));
		}

		FillGridControl();
	}

	void TADialogGroupToolInputPage::OnBnClickedButtonRemove()
	{
		SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
		for (int i = Selection.GetMaxRow(); i >= Selection.GetMinRow() && i <= m_inputData.size(); --i)
		{
			for (int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++)
			{
				if (m_Grid.IsCellSelected(i, j) && 0 < i && m_inputData.size() > i -1)
				{
					m_inputData.erase(m_inputData.begin() + (i - 1));
					break;
				}
			}
		}
		FillGridControl();
	}

	void TADialogGroupToolInputPage::OnBnClickedMoveUp()
	{
		SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
		if (Selection.GetMinRow() == Selection.GetMaxRow() && 1 < Selection.GetMinRow())
		{
			std::swap(m_inputData[Selection.GetMinRow() - 1], m_inputData[Selection.GetMinRow() - 2]);
			Selection.SetMinRow(Selection.GetMinRow() - 1);
			Selection.SetMaxRow(Selection.GetMaxRow() - 1);
			m_Grid.SetSelectedRange(Selection, true);
			m_Grid.SetFocusCell(Selection.GetMaxRow(), Selection.GetMaxCol());
			FillGridControl();
		}
	}

	void TADialogGroupToolInputPage::OnBnClickedMoveDown()
	{
		SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
		if (Selection.GetMinRow() == Selection.GetMaxRow() && 0 < Selection.GetMinRow() && Selection.GetMinRow() + 1 < m_Grid.GetRowCount())
		{
			std::swap(m_inputData[Selection.GetMinRow() - 1], m_inputData[Selection.GetMinRow()]);
			Selection.SetMinRow(Selection.GetMinRow() + 1);
			Selection.SetMaxRow(Selection.GetMaxRow() + 1);
			m_Grid.SetSelectedRange(Selection, true);
			m_Grid.SetFocusCell(Selection.GetMaxRow(), Selection.GetMaxCol());
			FillGridControl();
		}
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	bool TADialogGroupToolInputPage::setInspectionData()
	{
		UpdateData(TRUE);
		try
		{
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest = requestCmd.mutable_setandsortembeddedvaluerequest();
			pRequest->set_ownerid(m_TaskObjectID);
			for (const auto& data : m_inputData)
			{
				auto* pTmp = pRequest->add_embeddedlist();
				pTmp->set_name(data.m_name);
				pTmp->set_oldembeddedid(data.m_oldEmbeddedId);
				pTmp->set_type(data.m_type);
				pTmp->set_defaultvalue(data.m_defaultValue);
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

	void TADialogGroupToolInputPage::initGridControl()
	{
		m_Grid.SetFixedRowCount(cHeaderSize);
		m_Grid.SetFixedColumnCount(0);
		m_Grid.SetRowResize(FALSE);
		m_Grid.SetColumnResize(true);
		m_Grid.AllowReorderColumn(false);
		m_Grid.EnableDragAndDrop(FALSE);
		m_Grid.SetEditable(true);
		m_Grid.SetColumnCount(static_cast<int>(g_columnInputObjectsDefArray.size()));
		SvGcl::GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
		Item.row = 0;
		Item.crBkClr = CLR_DEFAULT;
		Item.nFormat = DT_LEFT | DT_VCENTER | DT_WORDBREAK;
		for (const auto& rData : g_columnInputObjectsDefArray)
		{
			m_Grid.SetColumnWidth(rData.first, rData.second.m_columnSize);
			Item.col = rData.first;
			Item.strText = rData.second.m_name.c_str();
			m_Grid.SetItem(&Item);
		}
	}

	void TADialogGroupToolInputPage::FillGridControl()
	{
		CStringArray typeOptions;
		SvOi::NameValueVector typePairs;
		int numberOfObjects = static_cast<int>(m_inputData.size());
		if (0 < numberOfObjects)
		{
			typePairs = m_Values.GetEnumTypes(SvPb::InputObjectTypeEId);
			for (const auto& entry : typePairs)
			{
				typeOptions.Add(entry.first.c_str());
			}
		}
		m_Grid.SetRowCount(numberOfObjects + 1);
		for (int i = 0; numberOfObjects > i; ++i)
		{
			auto row = i + 1;
			m_Grid.SetItemText(row, NameColumn, m_inputData[i].m_name.c_str());
			m_Grid.SetItemState(row, NameColumn, m_Grid.GetItemState(row, NameColumn) & (~GVIS_READONLY));
			using namespace SvGcl;
			m_Grid.SetCellType(row, TypeColumn, RUNTIME_CLASS(GridCellCombo));
			auto* pCell = dynamic_cast<SvGcl::GridCellCombo*>(m_Grid.GetCell(row, TypeColumn));
			
			pCell->SetOptions(typeOptions);
			pCell->SetStyle(CBS_DROPDOWNLIST);
			auto type = m_inputData[i].m_type;
			auto iter = std::find_if(typePairs.begin(), typePairs.end(), [type](const auto& rEntry) { return rEntry.second == type; });
			if (typePairs.end() != iter)
			{
				pCell->SetText(iter->first.c_str());
			}
			else
			{
				pCell->SetText(typePairs[0].first.c_str());
			}

			//check if defaultValue correct
			auto defaultValue = getDefaultString(type);
			try
			{
				m_inputData[i].m_defaultValue = checkText(m_inputData[i].m_type, m_inputData[i].m_defaultValue);
			}
			catch(...)
			{
				m_inputData[i].m_defaultValue = defaultValue;
			}
			
			if (2 > type) //table
			{
				m_Grid.SetItemText(row, DefaultColumn, m_inputData[i].m_defaultValue.c_str());
				m_Grid.SetItemState(row, DefaultColumn, m_Grid.GetItemState(row, DefaultColumn) & (~GVIS_READONLY));
			}
			else
			{
				m_Grid.SetItemText(row, DefaultColumn, "");
				m_Grid.SetItemState(row, DefaultColumn, m_Grid.GetItemState(row, DefaultColumn) | GVIS_READONLY);
			}

			auto value = std::string(static_cast<LPCSTR>(m_Values.Get<_bstr_t>(SvPb::ExternalInputLinkedEId + i)));
			if (m_inputData[i].m_ValueStr.empty())
			{
				m_inputData[i].m_ValueStr = defaultValue;
			}
			m_Grid.SetItemText(row, ValueColumn, m_inputData[i].m_ValueStr.c_str());
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

	void TADialogGroupToolInputPage::showContextMenu(CPoint point)
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

	SvOi::NameValuePair TADialogGroupToolInputPage::getType(const std::string& rTypeName)
	{
		const auto& rTypePairs = m_Values.GetEnumTypes(SvPb::InputObjectTypeEId);
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

	void TADialogGroupToolInputPage::loadDataList()
	{
		m_inputData.clear();
		m_Values.Init();
		for (int i = 0; c_maxValues > i; ++i)
		{
			if (0 == m_Values.GetAllowedAttributeFlags(SvPb::ExternalInputEId + i))
			{	//stop loading values, because it is hidden.
				break;
			}

			GroupInputData data;
			data.m_name = m_Values.GetName(SvPb::ExternalInputEId + i);
			data.m_oldEmbeddedId = SvPb::ExternalInputEId + i;
			data.m_type = static_cast<SvPb::InputTypeEnum>(m_Values.Get<int>(SvPb::InputObjectTypeEId + i));
			data.m_defaultValue = m_Values.GetDefault<CString>(SvPb::ExternalInputEId + i);
			data.m_ValueStr = std::string(static_cast<LPCSTR>(m_Values.Get<_bstr_t>(SvPb::ExternalInputLinkedEId + i)));
			if (data.m_ValueStr.empty())
			{
				auto valueVar = m_Values.Get<variant_t>(SvPb::ExternalInputEId + i);
				if (VT_EMPTY != valueVar.vt)
				{
					data.m_ValueStr = SvUl::VariantToString(valueVar);
				}
			}
			m_inputData.emplace_back(std::move(data));
		}
	}

	void TADialogGroupToolInputPage::UpdateEnableButtons()
	{
		SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
		bool bMoveUpEnable = false;
		bool bMoveDownEnable = false;
		if (Selection.GetMinRow() == Selection.GetMaxRow())
		{
			bMoveUpEnable = (1 < Selection.GetMinRow() && Selection.GetMinRow() <= m_inputData.size());
			bMoveDownEnable = (0 < Selection.GetMinRow() && Selection.GetMinRow() + 2 < m_Grid.GetRowCount());
		}

		GetDlgItem(IDC_BUTTON_MOVEUP)->EnableWindow(bMoveUpEnable);
		GetDlgItem(IDC_BUTTON_MOVEDOWN)->EnableWindow(bMoveDownEnable);
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(-1 != Selection.GetMaxRow() && Selection.GetMinRow() <= m_inputData.size());

		if (SvDef::c_maxTableColumn > m_inputData.size())
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
