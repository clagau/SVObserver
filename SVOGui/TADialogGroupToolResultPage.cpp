//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TADialogGroupToolResultPage.cpp
/// This is the class for the input tab of Group Tool
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADialogGroupToolResultPage.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "GridCtrlLibrary/GridCellCombo.h"
#include "SVOGuiUtility/DisplayHelper.h"
#include "SVOGuiUtility/LinkedValueSelectorDialog.h"
#include "SVShowDependentsDialog.h"
#include "Definitions/Color.h"
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
constexpr int cNameColumnSize = 150;
constexpr int cResultColumnSize = 80;
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
};

std::map<int, ColumnDef> g_columnResultObjectsDefArray = {
	{DependencyColumn, ColumnDef{"", 15}},
	{NameColumn, ColumnDef{"Name", cNameColumnSize}},
	{TypeColumn, {"Type", cTypeColumnSize }},
	{ResultColumn, {"Result", cResultColumnSize }},
	{ValueColumn, {"Value", cIndirectNameColumnSize }},
	{ValueButtonColumn, {"", cBoxColumnSize}},
};
}

SvStl::MessageContainerVector getErrorMessage(uint32_t inspectionId, uint32_t toolId);

	BEGIN_MESSAGE_MAP(TADialogGroupToolResultPage, CPropertyPage)
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
TADialogGroupToolResultPage::TADialogGroupToolResultPage(uint32_t inspectionId, uint32_t toolId, uint32_t taskId, bool isInputsChangeAble)
	: CPropertyPage(TADialogGroupToolResultPage::IDD, IDS_CLASSNAME_RESULT_PARAMETER_TASK)
	, m_InspectionID(inspectionId)
	, m_toolId(toolId)
	, m_TaskObjectID(taskId)
	, m_isChangeAble(isInputsChangeAble)
	, m_Values {SvOgu::BoundValues{ inspectionId, taskId }}
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool TADialogGroupToolResultPage::QueryAllowExit()
{
	auto* pSheet = dynamic_cast<CPropertySheet*>(GetParent());
	if (nullptr != pSheet && pSheet->GetActivePage() == this)
	{
		return setInspectionData();
	}
	return true;
}
#pragma endregion Public Methods

#pragma region Protected Methods
void TADialogGroupToolResultPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADialogGroupToolResultPage)
	DDX_Control(pDX, IDC_GRID, m_Grid);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_strComment);
	//}}AFX_DATA_MAP
}

BOOL TADialogGroupToolResultPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SvOgu::DisplayHelper::setIconListToGrid(m_ImageList, m_downArrowBitmap, m_Grid);

	m_errorMessages = getErrorMessage(m_InspectionID, m_toolId);

	if (false == m_isChangeAble)
	{
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
		case DependencyColumn:
		{
			if (false == (m_resultData[pItem->iRow - 1]).isOk())
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Display);
				Msg.setMessage(m_resultData[pItem->iRow - 1].m_errorData);
			}
			if (0 < m_resultData[pItem->iRow - 1].m_dependencies.size())
			{
				SVShowDependentsDialog Dlg(m_resultData[pItem->iRow - 1].m_dependencies);
				Dlg.DoModal();
			}
			break;
		}
		case ValueButtonColumn:
		{
			if (m_isChangeAble)
			{
				SvOgu::LinkedValueSelectorDialog dlg(m_InspectionID, m_Values.GetObjectID(SvPb::ResultObjectValueEId + (pItem->iRow - 1)), m_resultData[pItem->iRow - 1]);
				if (IDOK == dlg.DoModal())
				{
					m_resultData[pItem->iRow - 1].m_data = dlg.getData();
					setValueColumn(pItem->iRow - 1);
					setInspectionData(false);
					m_Grid.Refresh();
				}
			}
		}
		break;
		default:
			//nothing to do
			return;
	}
}

bool TADialogGroupToolResultPage::OnValueChanged(int row, int column)
{
	bool bAcceptChange {true};
	if (0 < row && m_resultData.size() >= row)
	{
		std::string cellText = m_Grid.GetCell(row, column)->GetText();
		switch (column)
		{
			case NameColumn:
				if (m_resultData[row - 1].m_name != cellText)
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
						auto oldName = m_resultData[row - 1].m_name;
						m_resultData[row - 1].m_name = cellText;

						auto errorMessage = checkParameterNames(m_InspectionID, m_TaskObjectID, m_resultData);
						if (0 < errorMessage.size())
						{
							SvStl::MessageManager Msg(SvStl::MsgType::Display);
							Msg.setMessage(errorMessage[0].getMessage());
							m_resultData[row - 1].m_name = oldName;
							return false;
						}
					}
				}
				break;
			case TypeColumn:
			{
				const auto& type = getType(cellText);
				m_resultData[row - 1].m_type = static_cast<SvPb::LinkedValueTypeEnum>(type.second);
				SvOgu::checkAndCorrectTypes(m_resultData[row - 1]);
				FillGridControl();
				break;
			}
			case ValueColumn:
			{
				bAcceptChange = m_resultData[row - 1].setValue(cellText);
			}
		}
	}
	return bAcceptChange;
}

void TADialogGroupToolResultPage::OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*)pNotifyStruct;
	bool bAcceptChange = OnValueChanged(pItem->iRow, pItem->iColumn);
	*pResult = (bAcceptChange) ? 0 : -1;
}

void TADialogGroupToolResultPage::OnGridValueSelectionChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*)pNotifyStruct;
	bool bAcceptChange = OnValueChanged(pItem->iRow, pItem->iColumn);
	*pResult = (bAcceptChange) ? 0 : -1;
}

void TADialogGroupToolResultPage::OnSelectionChanged(NMHDR*, LRESULT*)
{
	setInspectionData(false);
	UpdateEnableButtons();
}

void TADialogGroupToolResultPage::OnBnClickedButtonAdd()
{
	addEntry(m_InspectionID, m_TaskObjectID, SvOgu::c_resultName, m_resultData);
	setInspectionData(false);
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

void TADialogGroupToolResultPage::OnKillFocusComment()
{
	SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
	if (Selection.GetMinRow() == Selection.GetMaxRow() && 0 < Selection.GetMinRow() && m_resultData.size() >= Selection.GetMaxRow())
	{
		UpdateData();
		m_resultData[Selection.GetMinRow() - 1].m_data.m_comment = m_strComment;
	}
}
#pragma endregion Protected Methods

#pragma region Private Methods
bool TADialogGroupToolResultPage::setInspectionData(bool displayMessageBox)
{
	UpdateData(TRUE);
	try
	{
		m_errorMessages = sendValuesToInspection(m_InspectionID, m_TaskObjectID, m_resultData, displayMessageBox);
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

void TADialogGroupToolResultPage::setGridControlReadOnlyFlag(int row, int column, bool isChangeable)
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

void TADialogGroupToolResultPage::FillGridControl()
{
	CStringArray typeOptions;
	SvDef::NameValueVector typePairs;
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

		bool hasDependencies = (0 < m_resultData[i].m_dependencies.size());
		m_Grid.SetItemBkColour(row, DependencyColumn, m_resultData[i].isOk() ? SvDef::White : SvDef::Black);
		m_Grid.SetItemFgColour(row, DependencyColumn, m_resultData[i].isOk() ? SvDef::Black : SvDef::White);
		m_Grid.SetItemText(row, DependencyColumn, hasDependencies ? "D" : "");
		setGridControlReadOnlyFlag(row, DependencyColumn, false);

		m_Grid.SetItemText(row, NameColumn, m_resultData[i].m_name.c_str());
		setGridControlReadOnlyFlag(row, NameColumn, m_isChangeAble);
		using namespace SvGcl;
		m_Grid.SetCellType(row, TypeColumn, RUNTIME_CLASS(GridCellCombo));
		setGridControlReadOnlyFlag(row, TypeColumn, m_isChangeAble);
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

		std::string res = "";
		if (m_resultData[i].m_type == SvPb::LinkedValueTypeEnum::TypeStates)
		{
			if (m_resultData[i].m_data.m_selectedOption == SvPb::LinkedSelectedOption::DirectValue)
			{
				res = SvOgu::StateHelper::toString(m_resultData[i].m_data.m_Value);
			}
		}
		else if (SvPb::isValueType(m_resultData[i].m_type))
		{
			res = static_cast<CString>(m_resultData[i].m_data.m_Value).GetString();
		}
		m_Grid.SetItemText(row, ResultColumn, res.c_str());
		setGridControlReadOnlyFlag(row, ResultColumn, false);

		setValueColumn(i);

		if (m_isChangeAble)
		{
			SvGcl::GV_ITEM buttonItem;
			buttonItem.mask = GVIF_IMAGE;
			buttonItem.iImage = 0;
			buttonItem.row = row;
			buttonItem.col = ValueButtonColumn;
			m_Grid.SetItem(&buttonItem);
		}
	}
	
	m_Grid.Refresh();
	UpdateEnableButtons();
}

void TADialogGroupToolResultPage::setValueColumn(int pos)
{
	bool isChangeable {false};
	CString valueString;

	switch (m_resultData[pos].m_data.m_selectedOption)
	{
		case SvPb::DirectValue:
			if (SvPb::LinkedValueTypeEnum::TypeStates == m_resultData[pos].m_type)
			{
				isChangeable = false;
				valueString = SvOgu::StateHelper::toString(m_resultData[pos].m_data.m_directValue).c_str();

			}
			else if (SvPb::isValueType(m_resultData[pos].m_type))
			{
				isChangeable = true;
				valueString = static_cast<CString>(m_resultData[pos].m_data.m_directValue);
			}

			break;
		case SvPb::IndirectValue:
			valueString = SvCmd::getDottedName(m_InspectionID, m_resultData[pos].m_data.m_indirectIdName).c_str();
			break;
		case SvPb::Formula:
			valueString = m_resultData[pos].m_data.m_formula.c_str();
			break;
		default:
			break;
	}
	m_resultData[pos].m_data.m_defaultValue = SvPb::getDefaultString(m_resultData[pos].m_type);
	m_Grid.SetItemText(pos + 1, ValueColumn, valueString);
	if (isChangeable)
	{
		m_Grid.SetItemBkColour(pos + 1, ValueColumn, SvDef::White);
		setGridControlReadOnlyFlag(pos + 1, ValueColumn, m_isChangeAble);
	}
	else
	{
		m_Grid.SetItemBkColour(pos + 1, ValueColumn, SvDef::WhiteSmoke);
		setGridControlReadOnlyFlag(pos + 1, ValueColumn, false);
	}
	setGridControlReadOnlyFlag(pos + 1, ValueButtonColumn, m_isChangeAble);
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

SvDef::NameValuePair TADialogGroupToolResultPage::getType(const std::string& rTypeName)
{
	const auto& rTypePairs = m_Values.GetEnumTypes(SvPb::ResultObjectTypeEId);
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

void TADialogGroupToolResultPage::loadDataList()
{
	SvOgu::loadDataList(m_resultData, m_Values, SvPb::ResultObjectValueEId, SvPb::ResultObjectTypeEId, m_errorMessages);
}

void TADialogGroupToolResultPage::UpdateEnableButtons()
{
	SvGcl::CellRange Selection = m_Grid.GetSelectedCellRange();
	bool bMoveUpEnable = false;
	bool bMoveDownEnable = false;
	if (Selection.GetMinRow() == Selection.GetMaxRow())
	{
		int pos = Selection.GetMinRow(); //GetMaxRow identically
		bMoveUpEnable = (1 < pos && pos <= m_resultData.size());
		bMoveDownEnable = (0 < pos && pos + 1 < m_Grid.GetRowCount());

		if (0 < Selection.GetMinRow() && Selection.GetMinRow() <= m_resultData.size())
		{
			GetDlgItem(IDC_EDIT_COMMENT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_EDIT_COMMENT)->EnableWindow(m_isChangeAble);
			m_strComment = m_resultData[Selection.GetMinRow() - 1].m_data.m_comment.c_str();
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

	bool bRemoveEnable = -1 != Selection.GetMaxRow() && Selection.GetMinRow() <= m_resultData.size();
	if (bRemoveEnable)
	{
		for (int i = Selection.GetMaxRow(); i >= Selection.GetMinRow() && i <= m_resultData.size(); --i)
		{
			for (int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++)
			{
				if (m_Grid.IsCellSelected(i, j) && 0 < i && m_resultData.size() > i - 1)
				{
					bRemoveEnable = bRemoveEnable && 0 == m_resultData[i - 1].m_dependencies.size();
				}
			}
		}
	}
	GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(bRemoveEnable);

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
