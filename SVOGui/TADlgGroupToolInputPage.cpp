//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file TADlgGroupToolInputPage.cpp
/// This is the class for the input tab of Group Tool
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgGroupToolInputPage.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "GridCtrlLibrary/GridCellCombo.h"
#include "SVOGuiUtility/DisplayHelper.h"
#include "SVOGuiUtility/LinkedValueSelectorDialog.h"
#include "Definitions/Color.h"
#include "SVShowDependentsDialog.h"
#include "SVOGuiUtility/GroupToolHelper.h"
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
constexpr int cTypeColumnSize = 100;
constexpr int cResultColumnSize = 80;
constexpr int cNameColumnSize = 150;
constexpr int cIndirectNameColumnSize = 285;
constexpr int cBoxColumnSize = 25;

struct ColumnDef
{
	std::string m_name;
	int m_columnSize;

	ColumnDef(const std::string& name, int columnSize) : m_name(name), m_columnSize(columnSize) {};
};

enum ColumnPos
{
	DependencyColumn = 0,
	NameColumn,
	TypeColumn,
	ResultColumn,
	ValueColumn,
	ValueButtonColumn,
	DefaultColumn,
};

std::map<int, ColumnDef> g_columnInputObjectsDefArray = {
	{DependencyColumn, {"", 15}},
	{NameColumn, ColumnDef{"Name", cNameColumnSize}},
	{TypeColumn, {"Type", cTypeColumnSize }},
	{ResultColumn, {"Result", cResultColumnSize}},
	{ValueColumn, {"Value", cIndirectNameColumnSize }},
	{ValueButtonColumn, {"", cBoxColumnSize}},
	{DefaultColumn, {"Default", cNameColumnSize}},
};








}

SvStl::MessageContainerVector getErrorMessage(uint32_t inspectionId, uint32_t toolId)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pGetMessageListRequest = requestCmd.mutable_getmessagelistrequest();
	pGetMessageListRequest->set_objectid(toolId);

	HRESULT hr = SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);
	if (hr == S_OK)
	{
		return SvPb::convertProtobufToMessageVector(responseCmd.errormessage());
	}
	Log_Assert(false);
	return {};
}

BEGIN_MESSAGE_MAP(TADlgGroupToolInputPage, CPropertyPage)
	//{{AFX_MSG_MAP(TaDlgGroupToolInput)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_MOVEUP, OnBnClickedMoveUp)
	ON_BN_CLICKED(IDC_BUTTON_MOVEDOWN, OnBnClickedMoveDown)
	ON_EN_KILLFOCUS(IDC_EDIT_COMMENT, OnKillFocusComment)
	ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
	ON_NOTIFY(GVN_VALUE_SELCHANGED, IDC_GRID, OnGridValueSelectionChanged)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnSelectionChanged)
	ON_COMMAND(ID_ADD_COLUMN, OnBnClickedButtonAdd)
	ON_COMMAND(ID_REMOVE_COLUMNS, OnBnClickedButtonRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
TADlgGroupToolInputPage::TADlgGroupToolInputPage(uint32_t inspectionId, uint32_t toolId, uint32_t taskId, bool isModuleTool)
	: CPropertyPage(TADlgGroupToolInputPage::IDD)
	, m_InspectionID(inspectionId)
	, m_toolId(toolId)
	, m_TaskObjectID(taskId)
	, m_isModuleTool(isModuleTool)
	, m_Values {SvOgu::BoundValues{ inspectionId, taskId }}
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool TADlgGroupToolInputPage::QueryAllowExit()
{
	auto* pSheet = dynamic_cast<CPropertySheet*>(GetParent());
	if (nullptr != pSheet && pSheet->GetActivePage() == this)
	{
		setInspectionData();
	}
	return true;
}
#pragma endregion Public Methods

#pragma region Protected Methods
void TADlgGroupToolInputPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADlgGroupToolInputPage)
	DDX_Control(pDX, IDC_GRID, m_Grid);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_strComment);
	//}}AFX_DATA_MAP
}

BOOL TADlgGroupToolInputPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SvOgu::DisplayHelper::setIconListToGrid(m_ImageList, m_downArrowBitmap, m_Grid);

	m_errorMessages = getErrorMessage(m_InspectionID, m_toolId);

	if (m_isModuleTool)
	{
		SetHelpID(IDD_MODULE_INPUT);
		GetDlgItem(IDC_BUTTON_REMOVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_ADD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_MOVEUP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_MOVEDOWN)->ShowWindow(SW_HIDE);
	}

	loadDataList();
	initGridControl();
	FillGridControl();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
}

BOOL TADlgGroupToolInputPage::OnSetActive()
{
	BOOL bOk = CPropertyPage::OnSetActive();
	if (bOk)
	{
		loadDataList();
		FillGridControl();
	}
	return bOk;
}

BOOL TADlgGroupToolInputPage::OnKillActive()
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

void TADlgGroupToolInputPage::OnGridClick(NMHDR* pNotifyStruct, LRESULT*)
{
	auto* pItem = reinterpret_cast<SvGcl::NM_GRIDVIEW*>(pNotifyStruct);
	if (1 > pItem->iRow || m_inputData.size() < pItem->iRow)
	{
		return;
	}

	switch (pItem->iColumn)
	{
		case DependencyColumn:
		{
			if (false == m_inputData[pItem->iRow - 1].isOk())
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Display);
				Msg.setMessage(m_inputData[pItem->iRow - 1].m_errorData);
			}
			if (0 < m_inputData[pItem->iRow - 1].m_dependencies.size())
			{
				SVShowDependentsDialog Dlg(m_inputData[pItem->iRow - 1].m_dependencies);
				Dlg.DoModal();
			}
			break;
		}
		case ValueButtonColumn:
		{
			SvOgu::LinkedValueSelectorDialog dlg(m_InspectionID, m_Values.GetObjectID(SvPb::ExternalInputEId + (pItem->iRow - 1)), m_inputData[pItem->iRow - 1]);
			if (IDOK == dlg.DoModal())
			{
				m_inputData[pItem->iRow - 1].m_data = dlg.getData();
				setValueColumn(pItem->iRow - 1);
				setInspectionData(false);
				m_Grid.Refresh();
			}
		}
		break;
		default:
			//nothing to do
			return;
	}
}

void TADlgGroupToolInputPage::OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*)pNotifyStruct;
	bool bAcceptChange = OnValueChanged(pItem->iRow, pItem->iColumn);
	*pResult = (bAcceptChange) ? 0 : -1;
}

void TADlgGroupToolInputPage::OnGridValueSelectionChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*)pNotifyStruct;
	bool bAcceptChange = OnValueChanged(pItem->iRow, pItem->iColumn);
	*pResult = (bAcceptChange) ? 0 : -1;
}

void TADlgGroupToolInputPage::OnSelectionChanged(NMHDR*, LRESULT*)
{
	setInspectionData(false);
	UpdateEnableButtons();
}

void TADlgGroupToolInputPage::OnBnClickedButtonAdd()
{
	addEntry(m_InspectionID, m_TaskObjectID, SvOgu::c_inputName, m_inputData);
	setInspectionData(false);
	FillGridControl();
}

void TADlgGroupToolInputPage::OnBnClickedButtonRemove()
{
	SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
	for (int i = Selection.GetMaxRow(); i >= Selection.GetMinRow() && i <= m_inputData.size(); --i)
	{
		for (int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++)
		{
			if (m_Grid.IsCellSelected(i, j) && 0 < i && m_inputData.size() > i - 1)
			{
				m_inputData.erase(m_inputData.begin() + (i - 1));
				break;
			}
		}
	}
	FillGridControl();
}

void TADlgGroupToolInputPage::OnBnClickedMoveUp()
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

void TADlgGroupToolInputPage::OnBnClickedMoveDown()
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

void TADlgGroupToolInputPage::OnKillFocusComment()
{
	SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
	if (Selection.GetMinRow() == Selection.GetMaxRow() && 0 < Selection.GetMinRow() && m_inputData.size() >= Selection.GetMaxRow())
	{
		UpdateData();
		m_inputData[Selection.GetMinRow() - 1].m_data.m_comment = m_strComment;
	}
}
#pragma endregion Protected Methods

#pragma region Private Methods
bool TADlgGroupToolInputPage::setInspectionData(bool displayMessageBox)
{
	UpdateData(TRUE);
	try
	{
		m_errorMessages = sendValuesToInspection(m_InspectionID, m_TaskObjectID, m_inputData, displayMessageBox);
		loadDataList();
		FillGridControl();
		return m_errorMessages.empty();
	}
	catch (const SvStl::MessageContainer& rSvE)
	{
		if (displayMessageBox)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(rSvE.getMessage());
		}
		return false;
	}
}

void TADlgGroupToolInputPage::initGridControl()
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

void TADlgGroupToolInputPage::setGridControlReadOnlyFlag(int row, int column, bool isChangeable)
{
	if (isChangeable)
	{
		m_Grid.SetItemState(row, column, m_Grid.GetItemState(row, column) & (~GVIS_READONLY));
	}
	else
	{
		m_Grid.SetItemState(row, column, m_Grid.GetItemState(row, column) | GVIS_READONLY);
	}
}

void TADlgGroupToolInputPage::FillGridControl()
{
	CStringArray typeOptions;
	SvDef::NameValueVector typePairs;
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

		bool hasDependencies = (0 < m_inputData[i].m_dependencies.size());
		m_Grid.SetItemBkColour(row, DependencyColumn, m_inputData[i].isOk() ? SvDef::White : SvDef::Black);
		m_Grid.SetItemFgColour(row, DependencyColumn, m_inputData[i].isOk() ? SvDef::Black : SvDef::White);
		m_Grid.SetItemText(row, DependencyColumn, hasDependencies ? "D" : "");
		setGridControlReadOnlyFlag(row, DependencyColumn, false);

		m_Grid.SetItemText(row, NameColumn, m_inputData[i].m_name.c_str());
		setGridControlReadOnlyFlag(row, NameColumn, false == m_isModuleTool);

		using namespace SvGcl;
		m_Grid.SetCellType(row, TypeColumn, RUNTIME_CLASS(GridCellCombo));
		setGridControlReadOnlyFlag(row, TypeColumn, false == m_isModuleTool);
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
		try
		{
			m_inputData[i].m_data.m_defaultValue = SvOgu::fitVariantToType(m_inputData[i].m_type, m_inputData[i].m_data.m_defaultValue);
		}
		catch (...)
		{
			m_inputData[i].m_data.m_defaultValue = SvPb::getDefaultString(type);
		}

		std::string res = "";
		if (m_inputData[i].m_type == SvPb::LinkedValueTypeEnum::TypeStates)
		{
			if (m_inputData[i].m_data.m_selectedOption == SvPb::LinkedSelectedOption::DirectValue)
			{
				res = SvOgu::StateHelper::toString( m_inputData[i].m_data.m_Value);
			}
		}
		else if (SvPb::isValueType(m_inputData[i].m_type))
		{
			res = static_cast<CString>(m_inputData[i].m_data.m_Value).GetString();
		}

		m_Grid.SetItemText(row, ResultColumn, res.c_str());

		setGridControlReadOnlyFlag(row, ResultColumn, false);

		setValueColumn(i);

		SvGcl::GV_ITEM buttonItem;
		buttonItem.mask = GVIF_IMAGE;
		buttonItem.iImage = 0;
		buttonItem.row = row;
		buttonItem.col = ValueButtonColumn;
		m_Grid.SetItem(&buttonItem);

		if (SvPb::isValueType(type))
		{
			if (type == SvPb::LinkedValueTypeEnum::TypeStates)
			{
				m_Grid.SetCellType(row, DefaultColumn, RUNTIME_CLASS(GridCellCombo));
				setGridControlReadOnlyFlag(row, DefaultColumn, false == m_isModuleTool);
				auto* pCell_Def = dynamic_cast<SvGcl::GridCellCombo*>(m_Grid.GetCell(row, DefaultColumn));
				CStringArray typePassed;
				auto states = SvOgu::StateHelper::getStates();
				for (auto el : states)
				{
					typePassed.Add(el.first.c_str());
				}
				pCell_Def->SetOptions(typePassed);
				pCell_Def->SetStyle(CBS_DROPDOWNLIST);
				auto text = SvOgu::StateHelper::toString(m_inputData[i].m_data.m_defaultValue);
				pCell_Def->SetText(text.c_str());
				
			}
			else
			{
				m_Grid.SetItemText(row, DefaultColumn, static_cast<CString>(m_inputData[i].m_data.m_defaultValue));
				setGridControlReadOnlyFlag(row, DefaultColumn, false == m_isModuleTool);
			}
		}
		else
		{
			m_Grid.SetItemText(row, DefaultColumn, "");
			setGridControlReadOnlyFlag(row, DefaultColumn, false);
		}
	}

	m_Grid.Refresh();
	UpdateEnableButtons();
}

void TADlgGroupToolInputPage::setValueColumn(int pos)
{
	bool isChangeable {false};
	CString valueString;
	switch (m_inputData[pos].m_data.m_selectedOption)
	{
		case SvPb::DirectValue:
			if (SvPb::LinkedValueTypeEnum::TypeStates == m_inputData[pos].m_type)
			{
				isChangeable = false;
				valueString = SvOgu::StateHelper::toString(m_inputData[pos].m_data.m_directValue).c_str();
				 
			}
			else if (SvPb::isValueType(m_inputData[pos].m_type))
			{
				isChangeable = true;
				valueString = static_cast<CString>(m_inputData[pos].m_data.m_directValue);
			}
			break;
		case SvPb::IndirectValue:
			valueString = SvCmd::getDottedName(m_InspectionID, m_inputData[pos].m_data.m_indirectIdName).c_str();
			break;
		case SvPb::Formula:
			valueString = m_inputData[pos].m_data.m_formula.c_str();
			break;
		default:
			break;
	}
	m_Grid.SetItemText(pos + 1, ValueColumn, valueString);
	if (isChangeable)
	{
		m_Grid.SetItemBkColour(pos + 1, ValueColumn, SvDef::White);
		m_Grid.SetItemState(pos + 1, ValueColumn, m_Grid.GetItemState(pos + 1, ValueColumn) & (~GVIS_READONLY));
	}
	else
	{
		m_Grid.SetItemBkColour(pos + 1, ValueColumn, SvDef::WhiteSmoke);
		m_Grid.SetItemState(pos + 1, ValueColumn, m_Grid.GetItemState(pos + 1, ValueColumn) | GVIS_READONLY);
	}
}

void TADlgGroupToolInputPage::showContextMenu(CPoint point)
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

SvDef::NameValuePair TADlgGroupToolInputPage::getType(const std::string& rTypeName)
{
	const auto& rTypePairs = m_Values.GetEnumTypes(SvPb::InputObjectTypeEId);
	auto iter = std::find_if(rTypePairs.begin(), rTypePairs.end(), [rTypeName](const auto& rEntry) { return rTypeName == rEntry.first; });
	if (rTypePairs.end() != iter)
	{
		return *iter;
	}
	else
	{
		Log_Assert(false);
		return {};
	}
}

void TADlgGroupToolInputPage::loadDataList()
{
	SvOgu::loadDataList(m_inputData, m_Values, SvPb::ExternalInputEId, SvPb::InputObjectTypeEId, m_errorMessages);
}

void TADlgGroupToolInputPage::UpdateEnableButtons()
{
	SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
	bool bMoveUpEnable = false;
	bool bMoveDownEnable = false;
	if (Selection.GetMinRow() == Selection.GetMaxRow())
	{
		int pos = Selection.GetMinRow(); //GetMaxRow identically
		bMoveUpEnable = (1 < pos && pos <= m_inputData.size());
		bMoveDownEnable = (0 < pos && pos + 1 < m_Grid.GetRowCount());
		if (0 < Selection.GetMinRow() && Selection.GetMinRow() <= m_inputData.size())
		{
			GetDlgItem(IDC_EDIT_COMMENT)->ShowWindow(SW_SHOW);
			static_cast<CEdit*>(GetDlgItem(IDC_EDIT_COMMENT))->SetReadOnly(m_isModuleTool);
			m_strComment = m_inputData[Selection.GetMinRow() - 1].m_data.m_comment.c_str();
			UpdateData(false);
		}
		else
		{
			GetDlgItem(IDC_EDIT_COMMENT)->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		GetDlgItem(IDC_EDIT_COMMENT)->ShowWindow(SW_HIDE);
	}

	GetDlgItem(IDC_BUTTON_MOVEUP)->EnableWindow(bMoveUpEnable);
	GetDlgItem(IDC_BUTTON_MOVEDOWN)->EnableWindow(bMoveDownEnable);

	bool bRemoveEnable = -1 != Selection.GetMaxRow() && Selection.GetMinRow() <= m_inputData.size();
	if (bRemoveEnable)
	{
		for (int i = Selection.GetMaxRow(); i >= Selection.GetMinRow() && i <= m_inputData.size(); --i)
		{
			for (int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++)
			{
				if (m_Grid.IsCellSelected(i, j) && 0 < i && m_inputData.size() > i - 1)
				{
					bRemoveEnable = bRemoveEnable && 0 == m_inputData[i - 1].m_dependencies.size();
				}
			}
		}
	}
	GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(bRemoveEnable);

	if (SvDef::c_maxNumberOfParameter > m_inputData.size())
	{
		GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(true);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(false);
	}
}

bool TADlgGroupToolInputPage::OnValueChanged(int row, int column)
{
	bool bAcceptChange {true};
	if (0 < row && m_inputData.size() >= row)
	{
		std::string cellText = m_Grid.GetCell(row, column)->GetText();
		switch (column)
		{
			case NameColumn:
				if (m_inputData[row - 1].m_name != cellText)
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
						auto oldName = m_inputData[row - 1].m_name;
						m_inputData[row - 1].m_name = cellText;

						auto errorMessage = checkParameterNames(m_InspectionID, m_TaskObjectID, m_inputData);
						if (0 < errorMessage.size())
						{
							SvStl::MessageManager Msg(SvStl::MsgType::Display);
							Msg.setMessage(errorMessage[0].getMessage());
							m_inputData[row - 1].m_name = oldName;
							return false;
						}
					}
				}
				break;
			case TypeColumn:
			{
				const auto& type = getType(cellText);
				m_inputData[row - 1].m_type = static_cast<SvPb::LinkedValueTypeEnum>(type.second);
				checkAndCorrectTypes(m_inputData[row - 1]);
				FillGridControl();
				break;
			}
			case DefaultColumn:
			{
				if (SvPb::isValueType(m_inputData[row - 1].m_type))
				{
					try
					{
						//check if text a correct variant	
						m_inputData[row - 1].m_data.m_defaultValue = SvOgu::convertTextToVariant(m_inputData[row - 1].m_type, cellText);
					}
					catch (const SvStl::MessageContainer& rSvE)
					{
						SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
						Msg.setMessage(rSvE.getMessage());
						bAcceptChange = false;
						m_inputData[row - 1].m_data.m_defaultValue = SvPb::getDefaultString(m_inputData[row - 1].m_type);
						m_Grid.SetItemText(row, DefaultColumn, static_cast<CString>(m_inputData[row - 1].m_data.m_defaultValue));
					}
				}
				else
				{
					m_inputData[row - 1].m_data.m_defaultValue = _variant_t {};
				}
				break;
			}
			case ValueColumn:
			{
				bAcceptChange = m_inputData[row - 1].setValue(cellText);
				break;
			}
		}
	}
	return bAcceptChange;
}
#pragma endregion Private Mehods
} //namespace SvOg
