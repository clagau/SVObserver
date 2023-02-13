//*****************************************************************************
/// \copyright (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to solve Global Constant value conflicts
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "InputConflictDlg.h"
#include "GridCtrlLibrary\GridCellCombo.h"
#include "Definitions\Color.h"
#include "Definitions\GlobalConst.h"
#include "SVOGuiUtility/DisplayHelper.h"
#include "SVOGuiUtility/LinkedValueSelectorDialog.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
constexpr int cHeaderSize = 1;
constexpr int cValueColumnSize = 350;
constexpr int cBoxColumnSize = 25;

struct ColumnDef
{
	std::string m_name;
	int m_columnSize;

	ColumnDef(const std::string& name, int columnSize) : m_name(name), m_columnSize(columnSize) {};
};

enum ColumnPos
{
	ErrorColumn = 0,
	NameColumn,
	ValueColumn,
	ValueButtonColumn,
	OldValueColumn,
	NumberOfColumn //Do not use, is only to get number of columns
};

std::map<int, ColumnDef> g_columnInputObjectsDefMap = {
	{ErrorColumn, {"", 15}},
	{NameColumn, ColumnDef{"Name", cValueColumnSize}},
	{ValueColumn, {"Value", cValueColumnSize }},
	{ValueButtonColumn, {"", cBoxColumnSize}},
	{OldValueColumn, {"Old Value", cValueColumnSize }},
};
}

namespace SvOg
{
BEGIN_MESSAGE_MAP(InputConflictDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_GLOBAL_CONFLICT_GRID, OnGridClick)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GLOBAL_CONFLICT_GRID, OnGridEndEdit)
	ON_NOTIFY(GVN_VALUE_SELCHANGED, IDC_GLOBAL_CONFLICT_GRID, OnGridValueSelectionChanged)
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
InputConflictDlg::InputConflictDlg(uint32_t inspectionId, const ::google::protobuf::RepeatedPtrField<SvPb::FixedInputData>& rInputData, const std::vector<uint32_t>& rToolIds, LPCTSTR title /*= "Input Conflict"*/, CWnd* pParent /*nullptr*/)
	: CDialog(InputConflictDlg::IDD, pParent)
	, m_inspectionId {inspectionId}
	, m_rInputDataVector {rInputData}
	, m_rToolIds {rToolIds}
	,m_title {title}
{

}

InputConflictDlg::~InputConflictDlg()
{}
#pragma endregion Constructor

#pragma region Protected Methods
void InputConflictDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GLOBAL_CONFLICT_GRID, m_Grid);
}

BOOL InputConflictDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_title);

	SvOgu::DisplayHelper::setIconListToGrid(m_ImageList, m_downArrowBitmap, m_Grid);

	m_Grid.SetRedraw(false);
	m_Grid.SetFixedRowCount(cHeaderSize);
	m_Grid.SetRowResize(false);
	m_Grid.SetColumnResize(true);
	m_Grid.AllowReorderColumn(false);
	m_Grid.EnableDragAndDrop(false);
	m_Grid.SetEditable(true);
	m_Grid.SetColumnCount(NumberOfColumn);
	SvGcl::GV_ITEM Item;
	Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
	Item.row = 0;
	Item.crBkClr = CLR_DEFAULT;
	Item.nFormat = DT_LEFT | DT_VCENTER | DT_WORDBREAK;
	for (const auto& rData : g_columnInputObjectsDefMap)
	{
		m_Grid.SetColumnWidth(rData.first, rData.second.m_columnSize);
		Item.col = rData.first;
		Item.strText = rData.second.m_name.c_str();
		m_Grid.SetItem(&Item);
	}

	LoadGlobalData();
	UpdateData(FALSE);

	m_Grid.SetRedraw(true, true);
	return TRUE;
}

void InputConflictDlg::setLinkedValueColumns(int pos)
{
	if (0 <= pos && m_rInputDataVector.size() > pos)
	{
		if (nullptr == m_valuesControllerMap[m_rInputDataVector[pos].parentid()])
		{
			std::unique_ptr<SvOgu::ValueController> valueCtrl = std::make_unique<SvOgu::ValueController>(SvOgu::BoundValues{m_inspectionId, m_rInputDataVector[pos].parentid() });
			valueCtrl->Init();
			m_valuesControllerMap[m_rInputDataVector[pos].parentid()] = std::move(valueCtrl);
		}

		auto valueData = m_valuesControllerMap[m_rInputDataVector[pos].parentid()]->Get<SvOgu::LinkedValueData>(m_rInputDataVector[pos].embeddedid());
		bool isChangeable {false};
		CString valueString;
		switch (valueData.m_selectedOption)
		{
			case SvPb::DirectValue:
			case SvPb::Formula:
				valueString = static_cast<CString>(valueData.m_Value);
				isChangeable = SvPb::LinkedSelectedOption::DirectValue == valueData.m_selectedOption;
				break;
			case SvPb::IndirectValue:
				valueString = SvCmd::getDottedName(m_inspectionId, valueData.m_indirectIdName).c_str();
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
}

void InputConflictDlg::setInputObjectColumn(int pos)
{
	if (0 <= pos && m_rInputDataVector.size() > pos)
	{
		CStringArray stringList;
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto iter = m_inputObjectValueMap.find(m_rInputDataVector[pos].objectid());
		if (m_inputObjectValueMap.end() == iter)
		{
			auto* pAvailableRequest = requestCmd.mutable_getavailableobjectsrequest();
			auto* pInputSearchParameter = pAvailableRequest->mutable_input_search();
			pInputSearchParameter->set_input_connected_objectid(m_rInputDataVector[pos].objectid());
			pAvailableRequest->set_desired_first_object_type_for_name(SvPb::SVToolSetObjectType);
			pAvailableRequest->set_exclude_first_object_name_in_name(true);

			HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);
			if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
			{
				auto& list = m_inputObjectValueMap[m_rInputDataVector[pos].objectid()];
				for (const auto& entry : responseCmd.getavailableobjectsresponse().list())
				{
					list.push_back(entry);
					stringList.Add(entry.objectname().c_str());
				}
			}
		}	
		else
		{
			for (const auto& entry : iter->second)
			{
				stringList.Add(entry.objectname().c_str());
			}
		}
		using namespace SvGcl;
		m_Grid.SetCellType(pos + 1, ValueColumn, RUNTIME_CLASS(GridCellCombo));
		auto* pCell = dynamic_cast<SvGcl::GridCellCombo*>(m_Grid.GetCell(pos + 1, ValueColumn));

		pCell->SetOptions(stringList);
		pCell->SetStyle(CBS_DROPDOWNLIST);
		auto* pDataRequest = requestCmd.mutable_getinputdatarequest();
		pDataRequest->set_objectid(m_rInputDataVector[pos].objectid());
		pDataRequest->set_desired_first_object_type_for_connected_name(SvPb::SVToolSetObjectType);
		pDataRequest->set_exclude_first_object_name_in_conntected_name(true);
		responseCmd.Clear();
		HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getinputdataresponse())
		{
			if (false == pCell->setTextIfInList(responseCmd.getinputdataresponse().data().connected_objectdottedname().c_str()))
			{
				pCell->SetText(nullptr);
			}
		}
	}
}

void InputConflictDlg::LoadGlobalData()
{
	int numberOfObjects = static_cast<int>(m_rInputDataVector.size());
	m_Grid.SetRowCount(numberOfObjects + 1);
	for (int i = 0; numberOfObjects > i; ++i)
	{
		auto row = i + 1;

		bool hasError = false; // (0 < m_inputData[i].m_dependencies.size());
		//m_Grid.SetItemBkColour(row, ErrorColumn, isOk(m_inputData[i]) ? SvDef::White : SvDef::Black);
		m_Grid.SetItemFgColour(row, ErrorColumn, hasError ? SvDef::Black : SvDef::White);
		//m_Grid.SetItemText(row, ErrorColumn, hasDependencies ? "D" : "");
		m_Grid.SetItemState(row, ErrorColumn, m_Grid.GetItemState(row, ErrorColumn) | GVIS_READONLY);

		m_Grid.SetItemText(row, NameColumn, m_rInputDataVector[i].name().c_str());
		m_Grid.SetItemState(row, NameColumn, m_Grid.GetItemState(row, NameColumn) | GVIS_READONLY);
		if (m_rInputDataVector[i].islinkedvalue())
		{
			setLinkedValueColumns(i);

			SvGcl::GV_ITEM buttonItem;
			buttonItem.mask = GVIF_IMAGE;
			buttonItem.iImage = 0;
			buttonItem.row = row;
			buttonItem.col = ValueButtonColumn;
			m_Grid.SetItem(&buttonItem);
		}
		else
		{
			setInputObjectColumn(i);
		}

		m_Grid.SetItemText(row, OldValueColumn, m_rInputDataVector[i].oldinputvalue().c_str());
		m_Grid.SetItemState(row, OldValueColumn, m_Grid.GetItemState(row, NameColumn) | GVIS_READONLY);
	}
	m_Grid.Refresh();
}

void InputConflictDlg::OnGridClick(NMHDR* pNotifyStruct, LRESULT* /*pResult*/)
{
	SvGcl::NM_GRIDVIEW* pItem = reinterpret_cast<SvGcl::NM_GRIDVIEW*> (pNotifyStruct);
	int pos = pItem->iRow - 1;
	if (0 < pItem->iRow && m_rInputDataVector.size() >= pItem->iRow)
	{
		std::string cellText = m_Grid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
		switch (pItem->iColumn)
		{
			case ValueButtonColumn:
			{
				if (m_rInputDataVector[pos].islinkedvalue())
				{
					if (nullptr != m_valuesControllerMap[m_rInputDataVector[pos].parentid()])
					{
						auto valueData = m_valuesControllerMap[m_rInputDataVector[pos].parentid()]->Get<SvOgu::LinkedValueData>(m_rInputDataVector[pos].embeddedid());
						SvOgu::LinkedValueSelectorDialog dlg(m_inspectionId, m_rInputDataVector[pos].objectid(), m_rInputDataVector[pos].name(), valueData, valueData.m_defaultValue.vt);
						if (IDOK == dlg.DoModal())
						{
							commitLinkedData(pos, dlg.getData());
						}
					}
				}
			}
			break;
			default:
				//nothing to do
				return;
		}
	}
}

void InputConflictDlg::OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*)pNotifyStruct;
	bool bAcceptChange = true;

	if (0 < pItem->iRow && m_rInputDataVector.size() >= pItem->iRow)
	{
		std::string cellText = m_Grid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
		switch (pItem->iColumn)
		{
			case ValueColumn:
			{
				if (m_rInputDataVector[pItem->iRow - 1].islinkedvalue())
				{
					if (nullptr != m_valuesControllerMap[m_rInputDataVector[pItem->iRow - 1].parentid()])
					{
						auto valueData = m_valuesControllerMap[m_rInputDataVector[pItem->iRow - 1].parentid()]->Get<SvOgu::LinkedValueData>(m_rInputDataVector[pItem->iRow - 1].embeddedid());
						bAcceptChange = setLinkedValue(cellText, "m_valuesControllerMap[m_rInputDataVector[pItem->iRow - 1].parentid()].name()", valueData);
						if (bAcceptChange)
						{
							commitLinkedData(pItem->iRow - 1, valueData);
						}
					}
				}
				else
				{
					commitInputObject(pItem->iRow - 1, cellText);
				}
				break;
			}
		}
	}

	*pResult = (bAcceptChange) ? 0 : -1;
}

void InputConflictDlg::OnGridValueSelectionChanged(NMHDR* pNotifyStruct, LRESULT*)
{
	SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*)pNotifyStruct;
	if (0 < pItem->iRow && m_rInputDataVector.size() >= pItem->iRow)
	{
		std::string cellText = m_Grid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
		commitInputObject(pItem->iRow - 1, cellText);
		updateSelections();
	}
}

bool InputConflictDlg::setLinkedValue(const std::string& rNewStr, const std::string& rName, SvOgu::LinkedValueData& rData)
{
	if (SvPb::LinkedSelectedOption::DirectValue == rData.m_selectedOption /*&& SvPb::isValueType(data.m_type)*/)
	{
		variant_t tmp {rNewStr.c_str()};
		SvStl::MessageContainer msgContainer;
		bool isValid = (S_OK == ::VariantChangeTypeEx(&rData.m_directValue, &tmp, SvDef::LCID_USA, VARIANT_ALPHABOOL, rData.m_defaultValue.vt));
		if (false == isValid)
		{
			SvDef::StringVector msgList;
			msgList.push_back(rName);
			msgContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
			SvStl::MessageManager Msg(SvStl::MsgType::Display);
			Msg.setMessage(msgContainer.getMessage());
		}
		else
		{
			rData.m_Value = rData.m_directValue;
		}
		return isValid;
	}
	return false;
}

void InputConflictDlg::commitLinkedData(int pos, const SvOgu::LinkedValueData& rData)
{
	m_valuesControllerMap[m_rInputDataVector[pos].parentid()]->Set<SvOgu::LinkedValueData>(m_rInputDataVector[pos].embeddedid(), rData);
	m_valuesControllerMap[m_rInputDataVector[pos].parentid()]->Commit();
	setLinkedValueColumns(pos);
	m_Grid.Refresh();
}

void InputConflictDlg::commitInputObject(int pos, const std::string& rText)
{
	auto iter = m_inputObjectValueMap.find(m_rInputDataVector[pos].objectid());
	if (m_inputObjectValueMap.end() != iter)
	{
		auto valueIter = std::ranges::find_if(iter->second, [rText](const auto& rEntry) { return rText == rEntry.objectname(); });
		if (iter->second.end() != valueIter)
		{
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest = requestCmd.mutable_connecttoobjectrequest();
			pRequest->set_objectid(m_rInputDataVector[pos].objectid());
			pRequest->set_newconnectedid(valueIter->objectid());
			HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);
			Log_Assert(S_OK == hr);
		}
		else
		{
			Log_Assert(false);
		}
	}
	else
	{
		Log_Assert(false);
	}
}

void InputConflictDlg::updateSelections()
{
	SvPb::InspectionCmdRequest requestCmd;
	auto* pRequest = requestCmd.mutable_resetobjectrequest();
	for (auto id : m_rToolIds)
	{
		pRequest->set_objectid(id);
		SvCmd::InspectionCommands(m_inspectionId, requestCmd, nullptr);
	}
	m_inputObjectValueMap.clear();
	LoadGlobalData();
}
#pragma endregion Protected Methods
} //namespace SvOg