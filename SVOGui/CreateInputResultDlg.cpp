//*****************************************************************************
/// \copyright (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to create Inputs and Results for groupTool to remove dependencies inside-out.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "CreateInputResultDlg.h"
#include "SVOGuiUtility/GroupToolHelper.h"
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
	NameColumn,
	ValueColumn,
	NumberOfColumn //Do not use, is only to get number of columns
};

std::map<int, ColumnDef> g_columnInputObjectsDefMap = {
	{NameColumn, ColumnDef{"Name", cValueColumnSize}},
	{ValueColumn, {"Linked Object", cValueColumnSize }},
};

void initGrid(SvGcl::GridCtrl& rGrid)
{
	rGrid.SetRedraw(false);
	rGrid.SetFixedRowCount(cHeaderSize);
	rGrid.SetRowResize(false);
	rGrid.SetColumnResize(true);
	rGrid.AllowReorderColumn(false);
	rGrid.EnableDragAndDrop(false);
	rGrid.SetEditable(true);
	rGrid.SetColumnCount(NumberOfColumn);
	SvGcl::GV_ITEM Item;
	Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
	Item.row = 0;
	Item.crBkClr = CLR_DEFAULT;
	Item.nFormat = DT_LEFT | DT_VCENTER | DT_WORDBREAK;
	for (const auto& rData : g_columnInputObjectsDefMap)
	{
		rGrid.SetColumnWidth(rData.first, rData.second.m_columnSize);
		Item.col = rData.first;
		Item.strText = rData.second.m_name.c_str();
		rGrid.SetItem(&Item);
	}
}

void setGrid(SvGcl::GridCtrl& rGrid, const SvOg::CreateInputResultDlg::DataMapType& map, std::vector<std::string>& nameVec)
{
	assert(map.size() <= nameVec.size());
	size_t nameOffset = nameVec.size() - map.size();
	int number = static_cast<int>(map.size());
	rGrid.SetRowCount(number + 1);
	auto mapIter = map.begin();
	for (int i = 0; number > i && map.end() != mapIter && nameVec.size() > i + nameOffset; ++i, ++mapIter)
	{
		auto row = i + 1;

		rGrid.SetItemText(row, NameColumn, nameVec[i+ nameOffset].c_str());
		rGrid.SetItemText(row, ValueColumn, mapIter->first.c_str());
		rGrid.SetItemState(row, ValueColumn, rGrid.GetItemState(row, ValueColumn) | GVIS_READONLY);
	}
	rGrid.Refresh();
}

uint32_t getObjectId(uint32_t inspectionId, uint32_t ownerId, SvPb::SVObjectSubTypeEnum subType)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectidrequest()->mutable_info();
	pRequest->set_ownerid(ownerId);
	pRequest->mutable_infostruct()->set_objecttype(SvPb::ParameterTaskObjectType);
	pRequest->mutable_infostruct()->set_subtype(subType);

	HRESULT hr = SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectidresponse())
	{
		return responseCmd.getobjectidresponse().objectid();
	}
	else
	{
		return SvDef::InvalidObjectId;
	}
}

uint32_t getObjectId(uint32_t inspectionId, const std::string& objectName)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectidrequest();
	pRequest->set_name(objectName);

	HRESULT hr = SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectidresponse())
	{
		return responseCmd.getobjectidresponse().objectid();
	}
	else
	{
		return SvDef::InvalidObjectId;
	}
}

void addParameterEntry(uint32_t inspectionId, uint32_t taskId, const SvOg::CreateInputResultDlg::DataMapType& rDataMap, const std::vector<std::string>& rParaNames)
{
	assert(rDataMap.size() <= rParaNames.size());
	size_t nameOffset = rParaNames.size() - rDataMap.size();
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_addparameteranduseitrequest();
	pRequest->set_parametertaskid(taskId);
	for (auto j = nameOffset; const auto& rEntry : rDataMap)
	{
		pRequest->set_parametername(rParaNames[j++]);
		pRequest->set_linkedname(rEntry.first);
		pRequest->clear_usenamelist();
		for (const auto& rName : rEntry.second)
		{
			pRequest->add_usenamelist(rName);
		}
		HRESULT hr = SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);
		assert(S_OK == hr);
	}
}
}

namespace SvOg
{
BEGIN_MESSAGE_MAP(CreateInputResultDlg, CDialog)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnInputGridEndEdit)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_RESULT_GRID, OnResultGridEndEdit)
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
CreateInputResultDlg::CreateInputResultDlg(uint32_t inspectionId, uint32_t toolId, const ::google::protobuf::RepeatedPtrField< SvPb::NameNamePair >& rDependencyPairs, CWnd* pParent /*nullptr*/)
	: CDialog(CreateInputResultDlg::IDD, pParent)
	, m_inspectionId {inspectionId}
	, m_toolId {toolId}
	, m_inputId { getObjectId(inspectionId, toolId, SvPb::ParameterInputObjectType)}
	, m_inputValueCtrl{SvOgu::BoundValues{ inspectionId, m_inputId }}
	, m_resultId {getObjectId(inspectionId, toolId, SvPb::ParameterResultObjectType)}
	, m_resultValueCtrl {SvOgu::BoundValues{ inspectionId, m_resultId }}
{
	auto groupToolName {SvCmd::getDottedName(m_inspectionId, m_toolId, true)+"."};
	for (const auto& rDepPair : rDependencyPairs)
	{
		if (rDepPair.nameto()._Starts_with(groupToolName))
		{
			m_inputMap[rDepPair.namefrom()].push_back(rDepPair.nameto());
		}
		else if (rDepPair.namefrom()._Starts_with(groupToolName))
		{
			m_resultMap[rDepPair.namefrom()].push_back(rDepPair.nameto());
		}
		else
		{
			assert(false);
		}
	}
	
}

CreateInputResultDlg::~CreateInputResultDlg()
{}
#pragma endregion Constructor

#pragma region Protected Methods
void CreateInputResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_InputGrid);
	DDX_Control(pDX, IDC_RESULT_GRID, m_ResultGrid);
}

void fillParameterNameList(const std::vector<SvOgu::GroupInputResultData>& oldDataList, const std::string& baseName, size_t number, std::vector<std::string>& list)
{
	list.clear();
	std::ranges::transform(oldDataList, back_inserter(list), [](const auto& rEntry) { return rEntry.m_name; });
	list.resize(number+oldDataList.size());
	
	int lastIndex = 0;
	for (auto i = oldDataList.size(); i < number + oldDataList.size(); ++i)
	{
		auto newName = baseName + std::to_string(lastIndex++);
		while (0 < std::ranges::count_if(list, [newName](const auto& rEntry) { return rEntry == newName; }))
		{
			newName = baseName + std::to_string(lastIndex++);
		}
		list[i] = newName;
	}
}

BOOL CreateInputResultDlg::OnInitDialog()
{
	if (0 < m_inputMap.size())
	{
		std::vector<SvOgu::GroupInputResultData> inputOldDataList;
		loadDataList(inputOldDataList, m_inputValueCtrl, SvPb::ExternalInputEId, SvPb::InputObjectTypeEId, {});
		if (SvDef::c_maxTableColumn < inputOldDataList.size() + m_inputMap.size())
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TooManyVariables, {SvOgu::c_inputName}, SvStl::SourceFileParams(StdMessageParams));
			CDialog::OnCancel();
			return false;
		}

		fillParameterNameList(inputOldDataList, SvOgu::c_inputName, m_inputMap.size(), m_inputParameterNames);
		assert(0 == SvOgu::checkParameterNames(m_inspectionId, m_inputId, m_inputParameterNames).size());
	}
	if (0 < m_resultMap.size())
	{
		std::vector<SvOgu::GroupInputResultData> resultOldDataList;
		loadDataList(resultOldDataList, m_resultValueCtrl, SvPb::ResultObjectValueEId, SvPb::ResultObjectTypeEId, {});
		if (SvDef::c_maxTableColumn < resultOldDataList.size() + m_resultMap.size())
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TooManyVariables, {SvOgu::c_resultName}, SvStl::SourceFileParams(StdMessageParams));
			CDialog::OnCancel();
			return false;
		}
		fillParameterNameList(resultOldDataList, SvOgu::c_resultName, m_resultMap.size(), m_resultParameterNames);
		assert(0 == SvOgu::checkParameterNames(m_inspectionId, m_resultId, m_resultParameterNames).size());
	}

	CDialog::OnInitDialog();

	initGrid(m_InputGrid);
	initGrid(m_ResultGrid);

	SetGridData();
	UpdateData(FALSE);

	m_InputGrid.SetRedraw(true);
	m_ResultGrid.SetRedraw(true);
	return TRUE;
}

void CreateInputResultDlg::OnOK()
{
	if (0 < m_inputMap.size())
	{
		addParameterEntry(m_inspectionId, m_inputId, m_inputMap, m_inputParameterNames);
	}
	if (0 < m_resultMap.size())
	{
		addParameterEntry(m_inspectionId, m_resultId, m_resultMap, m_resultParameterNames);
	}
	CDialog::OnOK();
}

void CreateInputResultDlg::SetGridData()
{
	setGrid(m_InputGrid, m_inputMap, m_inputParameterNames);
	setGrid(m_ResultGrid, m_resultMap, m_resultParameterNames);
}

bool checkAndSetName(uint32_t ipId, uint32_t taskId, const std::string& rNewName, std::vector<std::string>& parameterNames, size_t namePos)
{
	if (parameterNames[namePos] != rNewName)
	{
		if (parameterNames.end() != std::ranges::find(parameterNames, rNewName))
		{
			SvDef::StringVector msgList;
			msgList.emplace_back(rNewName);
			SvStl::MessageManager Msg(SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RenameError_DuplicateName, msgList, SvStl::SourceFileParams(StdMessageParams));
			return false;
		}
		else
		{
			auto oldName = parameterNames[namePos];
			parameterNames[namePos] = rNewName;

			auto errorMessage = SvOgu::checkParameterNames(ipId, taskId, parameterNames);
			if (0 < errorMessage.size())
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Display);
				Msg.setMessage(errorMessage[0].getMessage());
				parameterNames[namePos] = oldName;
				return false;
			}
		}
	}
	return true;
}

void CreateInputResultDlg::OnInputGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*)pNotifyStruct;
	bool bAcceptChange = true;
	size_t namePos = pItem->iRow - 1 + m_inputParameterNames.size() - m_inputMap.size();

	if (0 < pItem->iRow && m_inputParameterNames.size() > namePos)
	{
		std::string cellText = m_InputGrid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
		switch (pItem->iColumn)
		{
			case NameColumn:
			{
				bAcceptChange = checkAndSetName(m_inspectionId, m_inputId, cellText, m_inputParameterNames, namePos);
				break;
			}
		}
	}

	*pResult = (bAcceptChange) ? 0 : -1;
}

void CreateInputResultDlg::OnResultGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*)pNotifyStruct;
	bool bAcceptChange = true;
	size_t namePos = pItem->iRow - 1 + m_resultParameterNames.size() - m_resultMap.size();

	if (0 < pItem->iRow && m_resultParameterNames.size() > namePos)
	{
		std::string cellText = m_ResultGrid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
		switch (pItem->iColumn)
		{
			case NameColumn:
			{
				bAcceptChange = checkAndSetName(m_inspectionId, m_resultId, cellText, m_resultParameterNames, namePos);
				break;
			}
		}
	}

	*pResult = (bAcceptChange) ? 0 : -1;
}
#pragma endregion Protected Methods
} //namespace SvOg