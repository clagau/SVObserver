//*****************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// \file TADialogTableDefinesPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Property page for the tool adjustment sheet to defines the table in the Table-tool.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADialogTableDefinesPage.h"
#include "FormulaController.h"
#include "SVFormulaEditorSheet.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVClassRegisterListClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
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
constexpr int cFormulaColumnSize = 500;
constexpr int cNameColumn = 0;
constexpr int cFormulaColumn = 1;
constexpr char* cEquationName = _T("Table Column");
constexpr char* cHeaderName_NameColumn = _T("Column Name");
constexpr char* cHeaderName_FormulaColumn = _T("Formula");

BEGIN_MESSAGE_MAP(TADialogTableDefinesPage, CPropertyPage)
	//{{AFX_MSG_MAP(TADialogTableDefinesPage)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_MOVEUP, OnBnClickedMoveUp)
	ON_BN_CLICKED(IDC_BUTTON_MOVEDOWN, OnBnClickedMoveDown)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
	ON_NOTIFY(NM_RCLICK, IDC_GRID, OnGridRClick)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnSelectionChanged)
	ON_COMMAND(ID_ADD_COLUMN, OnBnClickedButtonAdd)
	ON_COMMAND(ID_REMOVE_COLUMNS, OnBnClickedButtonRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
TADialogTableDefinesPage::TADialogTableDefinesPage(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID)
	: CPropertyPage(TADialogTableDefinesPage::IDD)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
{
}

TADialogTableDefinesPage::~TADialogTableDefinesPage()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool TADialogTableDefinesPage::QueryAllowExit()
{
	ValidateData();
	return true;
}
#pragma endregion Public Methods

#pragma region Protected Methods
void TADialogTableDefinesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADialogTableDefinesPage)
	DDX_Control(pDX, IDC_GRID, m_Grid);
	//}}AFX_DATA_MAP
}

BOOL TADialogTableDefinesPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	initGridControl();
	FillGridControl();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL TADialogTableDefinesPage::OnKillActive()
{
	ValidateData();
	return CPropertyPage::OnKillActive();
}

void TADialogTableDefinesPage::OnBnClickedButtonRemove()
{
	SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
	for (int i = Selection.GetMinRow(); i <= Selection.GetMaxRow() && i <= m_gridList.size(); i++)
	{
		for (int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++)
		{
			if (m_Grid.IsCellSelected(i, j))
			{
				SvPb::InspectionCmdRequest requestCmd;
				auto* pRequest= requestCmd.mutable_deleteobjectrequest();
				pRequest->set_flag(SvPb::DeleteObjectRequest::Flag_ResetInspection);
				SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_gridList[i - 1].second);

				HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
				if (S_OK != hr)
				{
					SvDef::StringVector msgList;
					msgList.push_back(SvUl::Format(_T("%d"), hr));
					SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumn_RemovingFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				}
				break;
			}
		}
	}
	FillGridControl();
}

void TADialogTableDefinesPage::OnBnClickedMoveUp()
{
	SVGUID moveGuid = GUID_NULL;
	SVGUID preGuid = GUID_NULL;
	SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
	if (Selection.GetMinRow() == Selection.GetMaxRow() && 1 < Selection.GetMinRow())
	{
		moveGuid = m_gridList[Selection.GetMinRow() - 1].second;
		preGuid = m_gridList[Selection.GetMinRow() - 2].second;
		MoveColumn(moveGuid, preGuid);
		Selection.SetMinRow(Selection.GetMinRow() - 1);
		Selection.SetMaxRow(Selection.GetMaxRow() - 1);
		m_Grid.SetSelectedRange(Selection, true);
		m_Grid.SetFocusCell(Selection.GetMaxRow(), Selection.GetMaxCol());
		FillGridControl();
	}
}

void TADialogTableDefinesPage::OnBnClickedMoveDown()
{
	SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
	if (Selection.GetMinRow() == Selection.GetMaxRow() && 0 < Selection.GetMinRow() && Selection.GetMinRow() + 1 < m_Grid.GetRowCount())
	{
		SVGUID moveGuid = m_gridList[Selection.GetMinRow() - 1].second;
		SVGUID preGuid{GUID_NULL};
		if (Selection.GetMinRow() + 3 < m_Grid.GetRowCount()) //3 because minRow is zero-based and last line in this grid is a empty line and should not be chosen
		{
			preGuid = m_gridList[Selection.GetMinRow() + 1].second;
		}

		MoveColumn(moveGuid, preGuid);
		Selection.SetMinRow(Selection.GetMinRow() + 1);
		Selection.SetMaxRow(Selection.GetMaxRow() + 1);
		m_Grid.SetSelectedRange(Selection, true);
		m_Grid.SetFocusCell(Selection.GetMaxRow(), Selection.GetMaxCol());
		FillGridControl();
	}
}

void TADialogTableDefinesPage::OnBnClickedButtonAdd()
{
	SVGUID addPreGuid = GUID_NULL;
	SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
	if (Selection.GetMinRow() == Selection.GetMaxRow() && 0 < Selection.GetMinRow() && m_gridList.size() >= Selection.GetMaxRow())
	{
		addPreGuid = m_gridList[Selection.GetMinRow() - 1].second;
	}
	int number = static_cast<int>(m_gridList.size()) + 1;
	std::string name = SvUl::Format(_T("%s %d"), cEquationName, number);

	//search for unique name until one is found
	while (!isTableNameUnique(name))
	{
		number++;
		name = SvUl::Format(_T("%s %d"), cEquationName, number);
	}
	AddColumn(name, addPreGuid);

	ValidateData(); //validate the new line (this does a reset and add the column to the tableObject)
	FillGridControl();
}

void TADialogTableDefinesPage::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;

	if (cFormulaColumn == pItem->iColumn && 0 < pItem->iRow && m_gridList.size() >= pItem->iRow)
	{
		ValidateData(); //validate the new line (this does a reset and add the column to the tableObject)
		std::string strCaption = SvUl::Format(_T("%s %s"), m_gridList[pItem->iRow - 1].first.c_str(), _T("Formula"));

		SvOg::SVFormulaEditorSheetClass dlg(m_InspectionID, m_TaskObjectID, m_gridList[pItem->iRow - 1].second, strCaption.c_str());
		dlg.DoModal();
		FillGridControl();
	}
}

void TADialogTableDefinesPage::OnGridRClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	CPoint p;
	if (GetCursorPos(&p))
	{
		ScreenToClient(&p);
		showContextMenu(p);
	}
}

void TADialogTableDefinesPage::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;
	bool bAcceptChange = true;

	if (cNameColumn == pItem->iColumn && 0 < pItem->iRow)
	{
		std::string CellText = m_Grid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
		std::string newName = CellText;
		SvUl::RemoveCharacters(newName, SvDef::cGeneralExcludeChars);
		if (newName != CellText)
		{
			m_Grid.SetItemText(pItem->iRow, pItem->iColumn, newName.c_str());
		}
		//Has the name changed
		if (m_gridList.size() >= pItem->iRow && newName == m_gridList[pItem->iRow - 1].first)
		{
			bAcceptChange = false;
		}
		else
		{
			std::string CheckOnlySpaces {newName};
			SvUl::RemoveCharacters(CheckOnlySpaces, _T(" "));

			if (!newName.empty() && !CheckOnlySpaces.empty())
			{
				if (isTableNameUnique(newName))
				{
					if (m_gridList.size() >= pItem->iRow)
					{
						SvPb::InspectionCmdRequest requestCmd;
						auto* pRequest = requestCmd.mutable_setobjectnamerequest();
						SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_gridList[pItem->iRow - 1].second.ToGUID());
						pRequest->set_objectname(newName.c_str());

						HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
						if (S_OK != hr)
						{
							bAcceptChange = false;
							SvDef::StringVector msgList;
							msgList.push_back(SvUl::Format(_T("%d"), hr));
							SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
							Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumn_RenamingFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
						}
						else
						{
							m_gridList[pItem->iRow - 1].first = newName;
						}
					}
					else if (m_gridList.size() + 1 == pItem->iRow)
					{
						HRESULT hr = AddColumn(newName);
						if (S_OK != hr)
						{
							bAcceptChange = false;
						}
						else
						{
							FillGridControl();
						}
					}
					else
					{
						bAcceptChange = false;
					}
				}
				else
				{
					bAcceptChange = false;
					SvDef::StringVector msgList;
					msgList.push_back(newName);
					SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumnName_NotUnique, msgList, SvStl::SourceFileParams(StdMessageParams));
				}
			}
			else
			{
				bAcceptChange = false;
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumnName_Empty, SvStl::SourceFileParams(StdMessageParams));
			}
		}

	}

	*pResult = (bAcceptChange) ? 0 : -1;
}

void TADialogTableDefinesPage::OnSelectionChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	UpdateEnableButtons();
}
#pragma endregion Protected Methods

#pragma region Private Methods
HRESULT TADialogTableDefinesPage::ValidateData()
{
	UpdateData(TRUE);

	// Do a reset of the Tool
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_resetobjectrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_TaskObjectID);

	HRESULT hResult = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (hResult == S_OK && responseCmd.has_standardresponse())
	{
		SvStl::MessageContainerVector errorMessageList = SvPb::setMessageVectorFromMessagePB(responseCmd.standardresponse().errormessages());
		if (0 < errorMessageList.size())
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(errorMessageList[0].getMessage());
		}
	}

	if (hResult == S_OK)
	{
		requestCmd.Clear();
		auto* pSetDefaultRequest = requestCmd.mutable_setdefaultinputsrequest();
		SvPb::SetGuidInProtoBytes(pSetDefaultRequest->mutable_objectid(), m_InspectionID);

		hResult = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
	}
	
	return hResult;
}

void TADialogTableDefinesPage::initGridControl()
{
	m_Grid.SetFixedRowCount(cHeaderSize);
	m_Grid.SetFixedColumnCount(0);
	m_Grid.SetRowResize(FALSE);
	m_Grid.SetColumnResize(true);
	m_Grid.AllowReorderColumn(false);
	m_Grid.EnableDragAndDrop(FALSE);
	m_Grid.SetEditable(true);
	m_Grid.SetColumnCount(2);
	m_Grid.SetColumnWidth(cNameColumn, cNameColumnSize);
	m_Grid.SetColumnWidth(cFormulaColumn, cFormulaColumnSize);
	SvGcl::GV_ITEM Item;
	Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
	Item.row = 0;
	Item.col = cNameColumn;
	Item.crBkClr = CLR_DEFAULT;
	Item.nFormat = DT_LEFT | DT_VCENTER | DT_WORDBREAK;
	Item.strText = cHeaderName_NameColumn;
	m_Grid.SetItem(&Item);

	Item.col = cFormulaColumn;
	Item.strText = cHeaderName_FormulaColumn;
	m_Grid.SetItem(&Item);
}

void TADialogTableDefinesPage::FillGridControl()
{
	m_gridList.clear();
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_TaskObjectID);
	pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVEquationObjectType);
	pRequest->mutable_typeinfo()->set_subtype(SvPb::TableColumnEquationObjectType);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
	{
		m_gridList = SvCmd::convertNameGuidList(responseCmd.getavailableobjectsresponse().list());
		m_Grid.SetRowCount(cHeaderSize + static_cast<int>(m_gridList.size()) + 1); //the last one is the empty line "----"
		SvGcl::GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
		Item.crBkClr = CLR_DEFAULT;
		Item.nFormat = DT_LEFT | DT_VCENTER;
		for (int i = 0; i < m_gridList.size(); i++)
		{
			Item.row = i + cHeaderSize;
			Item.col = cNameColumn;
			Item.strText = m_gridList[i].first.c_str();
			m_Grid.SetItem(&Item);

			Item.col = cFormulaColumn;
			FormulaController formulaController(m_InspectionID, m_TaskObjectID, m_gridList[i].second.ToGUID());
			Item.strText = formulaController.GetEquationText().c_str();
			m_Grid.SetItem(&Item);
			m_Grid.SetItemState(Item.row, cFormulaColumn, m_Grid.GetItemState(Item.row, cFormulaColumn) | GVIS_READONLY);
		}

		Item.row = static_cast<int>(m_gridList.size() + cHeaderSize);
		Item.col = cNameColumn;
		Item.strText = "";
		m_Grid.SetItem(&Item);

		Item.col = cFormulaColumn;
		Item.strText = "";
		m_Grid.SetItem(&Item);
		m_Grid.SetItemState(Item.row, cFormulaColumn, m_Grid.GetItemState(Item.row, cFormulaColumn) | GVIS_READONLY);
	}
	m_Grid.Refresh();
	UpdateEnableButtons();
}

bool TADialogTableDefinesPage::isTableNameUnique(const std::string& name)
{
	bool bResult = true;
	for (int i = 0; i < m_gridList.size(); i++)
	{
		if (m_gridList[i].first == name)
		{
			bResult = false;
		}
	}
	return bResult;
}

void TADialogTableDefinesPage::showContextMenu(CPoint point)
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

void TADialogTableDefinesPage::MoveColumn(SVGUID moveGuid, SVGUID preGuid)
{
	SvPb::InspectionCmdRequest requestCmd;
	auto* pRequest = requestCmd.mutable_moveobjectrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_parentid(), m_TaskObjectID);
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), moveGuid);
	SvPb::SetGuidInProtoBytes(pRequest->mutable_movepreid(), preGuid);
	pRequest->set_listmode(SvPb::MoveObjectRequest_ListEnum_FriendList);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
	if (S_OK != hr)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), hr));
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumn_MovingFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
}

void TADialogTableDefinesPage::UpdateEnableButtons()
{
	SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
	bool bMoveUpEnable = false;
	bool bMoveDownEnable = false;
	if (Selection.GetMinRow() == Selection.GetMaxRow())
	{
		bMoveUpEnable = (1 < Selection.GetMinRow() && Selection.GetMinRow() <= m_gridList.size());
		bMoveDownEnable = (0 < Selection.GetMinRow() && Selection.GetMinRow() + 2 < m_Grid.GetRowCount());
	}

	GetDlgItem(IDC_BUTTON_MOVEUP)->EnableWindow(bMoveUpEnable);
	GetDlgItem(IDC_BUTTON_MOVEDOWN)->EnableWindow(bMoveDownEnable);
	GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(-1 != Selection.GetMaxRow() && Selection.GetMinRow() <= m_gridList.size());

	if (c_maxTableColumn > m_gridList.size())
	{
		GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(true);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(false);
	}
}

HRESULT TADialogTableDefinesPage::AddColumn(const std::string& rName, SVGUID addPreGuid)
{
	SvPb::InspectionCmdRequest requestCmd;
	auto* pRequest = requestCmd.mutable_createobjectrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), m_TaskObjectID);
	pRequest->set_classid(SvPb::TableColumnEquationId);
	pRequest->mutable_friend_()->set_name(rName);
	SvPb::SetGuidInProtoBytes(pRequest->mutable_friend_()->mutable_preguid(), addPreGuid);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
	if (S_OK != hr)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), hr));
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumn_AddingFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	return hr;
}

#pragma endregion Private Mehods
} //namespace SvOg
