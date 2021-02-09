//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TaTableAnalyzerPage.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Tool adjustment page to control the analyzer for the table.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TaTableAnalyzerPage.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "TextDefinesSvOg.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "FormulaController.h"
#include "SVFormulaEditorSheet.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{

constexpr char* NoColumnTag = _T("(No Column Available)");
constexpr char* IsAscTag = _T("IsAsc");
constexpr char* ExcludeHighTag = _T("ExcludeHigh");
constexpr char* ExcludeHighLinkTag = _T("ExcludeHigh_Link");
constexpr char* ExcludeLowTag = _T("ExcludeLow");
constexpr char* ExcludeLowLinkTag = _T("ExcludeLow_Link");
constexpr char* LimitValueTag = _T("LimitValue");
constexpr char* LimitValueLinkTag = _T("LimitValue_Link");

BEGIN_MESSAGE_MAP(TaTableAnalyzerPage, CPropertyPage)
	//{{AFX_MSG_MAP(TaTableAnalyzerPage)
	ON_BN_CLICKED(IDC_BUTTON3, OnButtonClearAll)
	ON_BN_CLICKED(IDC_BUTTON2, OnButtonDeleteCurrentAnalyzer)
	ON_BN_CLICKED(IDC_BUTTON1, OnButtonInsertNewAnalyzer)
	ON_LBN_SELCHANGE(IDC_ANALYZER_LIST, OnSelchangeAnalyzerList)
	ON_CBN_SELCHANGE(IDC_COLUMN_SELECT_CBOX, OnChangeColumnSelection)
	ON_BN_CLICKED(IDC_BUTTON_EXCLUDE_HIGH, OnButtonClickExcludeHigh)
	ON_BN_CLICKED(IDC_BUTTON_EXCLUDE_LOW, OnButtonClickExcludeLow)
	ON_BN_CLICKED(IDC_BUTTON_LIMIT_VALUE, OnButtonClickLimitValue)
	ON_BN_CLICKED(IDC_BUTTON_ADDCOLUMN_FORMULA, OnAddColumnFormula)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
TaTableAnalyzerPage::TaTableAnalyzerPage(uint32_t inspectionId, uint32_t taskObjectId)
	: CPropertyPage(TaTableAnalyzerPage::IDD)
	, m_InspectionID(inspectionId)
	, m_TaskObjectID(taskObjectId)
	, m_selectedAnalyzerID(SvDef::InvalidObjectId)
	, m_SortDirection(-1)
	, m_selectedSubType(SvPb::SVNotSetSubObjectType)
	, m_inputName(_T(""))
	, m_objectSelector(inspectionId)
{
}

TaTableAnalyzerPage::~TaTableAnalyzerPage()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool TaTableAnalyzerPage::QueryAllowExit()
{
	HRESULT hResult = SetInspectionData();
	if (S_OK == hResult)
	{
		hResult = checkAllAnaylzer();
	}
	return (S_OK == hResult);
}
#pragma endregion Public Methods

#pragma region Protected Methods
void TaTableAnalyzerPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TaTableAnalyzerPage)
	DDX_Control(pDX, IDC_ANALYZER_LIST, m_analyzerListBox);
	DDX_Control(pDX, IDC_AVAILABLE_ANALYZER_COMBO, m_availableAnaylzerCB);
	DDX_Control(pDX, IDC_COLUMN_SELECT_CBOX, m_columnSelectionCB);
	DDX_Control(pDX, IDC_EDIT_EXCLUDE_HIGH, m_EditExcludeHigh);
	DDX_Control(pDX, IDC_EDIT_EXCLUDE_LOW, m_EditExcludeLow);
	DDX_Control(pDX, IDC_EDIT_LIMIT_VALUE, m_EditLimitValue);
	DDX_Control(pDX, IDC_EDIT_ADDCOLUMN_NAME, m_EditAddColumnName);
	DDX_Control(pDX, IDC_BUTTON_EXCLUDE_HIGH, m_ButtonExcludeHigh);
	DDX_Control(pDX, IDC_BUTTON_EXCLUDE_LOW, m_ButtonExcludeLow);
	DDX_Control(pDX, IDC_BUTTON_LIMIT_VALUE, m_ButtonLimitValue);
	DDX_Radio(pDX, IDC_ASC_RADIO, m_SortDirection);
	DDX_Text(pDX, IDC_EDIT_ADDCOLUMN_FORMULA, m_AddFormulaString);
	//}}AFX_DATA_MAP
}

BOOL TaTableAnalyzerPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SvUl::NameClassIdList availableList;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getcreatableobjectsrequest();
	pRequest->set_objectid(m_TaskObjectID);
	pRequest->mutable_typeinfo()->set_objecttype(SvPb::TableAnalyzerType);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getcreatableobjectsresponse())
	{
		const auto& rList = responseCmd.getcreatableobjectsresponse().list();
		std::transform(rList.begin(), rList.end(), std::back_inserter(availableList),
			[](const auto& rEntry) -> SvUl::NameClassIdPair { return{ rEntry.objectname(), rEntry.classid() }; });
	}

	m_availableAnaylzerCB.Init(availableList, _T(""), Analyzer_NoAnalyzerAvailable);

	// Put the Down Arrow on the Button
	m_downArrowBitmap.LoadOEMBitmap(OBM_DNARROW);

	m_ButtonExcludeHigh.SetBitmap(static_cast<HBITMAP> (m_downArrowBitmap));
	m_ButtonExcludeLow.SetBitmap(static_cast<HBITMAP> (m_downArrowBitmap));
	m_ButtonLimitValue.SetBitmap(static_cast<HBITMAP> (m_downArrowBitmap));

	RetrieveAvailableColumnList();
	refresh();
	SetPropertyControls();

	UpdateData(FALSE);

	return TRUE;
}

void TaTableAnalyzerPage::OnButtonClearAll()
{
	// For all Items in the Selected (Instantiated) Analyzer list
	SvUl::NameObjectIdList availableList = getTableAnalyzer();
	
	// remove all Analyzer items (instantiated)
	size_t listSize = availableList.size();
	for (int i = 0; i < listSize; ++i)
	{
		uint32_t analyzerID = availableList[i].second;
		if (SvDef::InvalidObjectId != analyzerID)
		{
			// Close, Disconnect and Delete it
			SvPb::InspectionCmdRequest requestCmd;
			auto* pRequest = requestCmd.mutable_deleteobjectrequest(); ;
			pRequest->set_flag(SvPb::DeleteObjectRequest::Flag_None);
			pRequest->set_objectid(analyzerID);

			SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
		}
	}

	m_analyzerListBox.SetCurSel(-1);

	// Refresh Dialog...
	refresh();
}

void TaTableAnalyzerPage::OnButtonDeleteCurrentAnalyzer()
{
	if (SvDef::InvalidObjectId != m_selectedAnalyzerID)
	{
		// Close, Disconnect and Delete it
		SvPb::InspectionCmdRequest requestCmd;
		auto* pRequest = requestCmd.mutable_deleteobjectrequest(); ;
		pRequest->set_objectid(m_selectedAnalyzerID);

		SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
		m_selectedAnalyzerID = SvDef::InvalidObjectId;
		m_selectedSubType = SvPb::SVNotSetSubObjectType;
		// Refresh Dialog...
		refresh();
	}
}

void TaTableAnalyzerPage::OnButtonInsertNewAnalyzer()
{
	if (S_OK != prepareSwitchOfAnalyzerSelection())
	{
		return;
	}

	auto classID = m_availableAnaylzerCB.getSelectedValue();

	if (SvPb::NoObjectClassId != classID)
	{
		uint32_t AnalyzerInsertBeforeID = m_analyzerListBox.getObjectId(m_analyzerListBox.GetCurSel());

		// Construct and Create the Analyzer Class Object
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_createobjectrequest();
		pRequest->set_ownerid(m_TaskObjectID);
		pRequest->set_classid(classID);
		pRequest->set_taskobjectinsertbeforeid(AnalyzerInsertBeforeID);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK != hr)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_CreationAnalyzerFailed, SvStl::SourceFileParams(StdMessageParams));
		}

		// Refresh Dialog...
		refresh();
	}
}

void TaTableAnalyzerPage::OnSelchangeAnalyzerList()
{
	if (S_OK != prepareSwitchOfAnalyzerSelection())
	{
		return;
	}

	int index = m_analyzerListBox.GetCurSel();
	m_selectedAnalyzerID = m_analyzerListBox.getObjectId(index);

	SetPropertyControls();
}// end OnSelchangeAnalyzerList


void TaTableAnalyzerPage::OnChangeColumnSelection()
{
	UpdateData(TRUE); // get data from dialog

	uint32_t columnId = m_columnSelectionCB.getSelectedValue();
	if (SvDef::InvalidObjectId != columnId && SvDef::InvalidObjectId != m_selectedAnalyzerID)
	{
		SvPb::InspectionCmdRequest requestCmd;
		auto* pRequest = requestCmd.mutable_connecttoobjectrequest();
		pRequest->set_objectid(m_selectedAnalyzerID);
		pRequest->set_inputname(m_inputName);
		pRequest->set_newconnectedid(columnId);
		pRequest->set_objecttype(SvPb::SVValueObjectType);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
		if (S_OK != hr)
		{
			SvDef::StringVector msgList;
			msgList.push_back(m_inputName);
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ConnectFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}
}

void TaTableAnalyzerPage::OnButtonClickExcludeHigh()
{
	CString Temp;
	m_EditExcludeHigh.GetWindowText(Temp);
	std::string Value = Temp;
	std::string Title = SvUl::LoadStdString(IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_HIGHVALUE);
	if (m_objectSelector.Show(Value, Title, this, SvPb::allNumberValueObjects, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed, m_TaskObjectID))
	{
		m_EditExcludeHigh.SetWindowText(Value.c_str());
	}
	UpdateData(false);
}

void TaTableAnalyzerPage::OnButtonClickExcludeLow()
{
	CString Temp;
	m_EditExcludeLow.GetWindowText(Temp);
	std::string Value = Temp;
	std::string Title = SvUl::LoadStdString(IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_LOWVALUE);
	if (m_objectSelector.Show(Value, Title, this, SvPb::allNumberValueObjects, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed, m_TaskObjectID))
	{
		m_EditExcludeLow.SetWindowText(Value.c_str());
	}
	UpdateData(false);
}

void TaTableAnalyzerPage::OnButtonClickLimitValue()
{
	CString Temp;
	m_EditLimitValue.GetWindowText(Temp);
	std::string Value = Temp;
	std::string Title = SvUl::LoadStdString(IDS_OBJECTNAME_TABLEANALYZERLIMIT_VALUE);
	if (m_objectSelector.Show(Value, Title, this, SvPb::allNumberValueObjects, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed, m_TaskObjectID))
	{
		m_EditLimitValue.SetWindowText(Value.c_str());
	}
	UpdateData(false);
}

void TaTableAnalyzerPage::OnAddColumnFormula()
{
	if (nullptr != m_pSelectedAddEquationFormula)
	{
		SvOg::SVFormulaEditorSheetClass dlg(m_pSelectedAddEquationFormula, "Formula");
		dlg.DoModal();
		m_AddFormulaString = m_pSelectedAddEquationFormula->GetEquationText().c_str();
	}
	UpdateData(false);
}

BOOL TaTableAnalyzerPage::OnSetActive()
{
	refresh();
	return CPropertyPage::OnSetActive();
}

BOOL TaTableAnalyzerPage::OnKillActive()
{
	HRESULT hResult = SetInspectionData();
	if (S_OK == hResult)
	{
		hResult = checkAllAnaylzer();
	}

	if (S_OK == hResult)
	{
		return CPropertyPage::OnKillActive();
	}
	else
	{
		return FALSE;
	}
}

HRESULT TaTableAnalyzerPage::SetInspectionData()
{
	UpdateData(TRUE); // get data from dialog
	SvStl::MessageContainerVector errorMessageList;
	HRESULT hrOk = S_OK;

	if (nullptr != m_pValues)
	{
		switch (m_selectedSubType)
		{
			case SvPb::TableAnalyzerSortType:
				m_pValues->Set<bool>(SvPb::TableAnaylzerSortIsASCEId, 0 == m_SortDirection);
				hrOk = m_pValues->Commit();
				errorMessageList = m_pValues->getFailedMessageList();
				break;
			case SvPb::TableAnalyzerExcludeType:
			{
				CString Value;
				m_EditExcludeHigh.GetWindowText(Value);
				m_pValues->Set<CString>(SvPb::TableAnaylzerExcludeHighEId, Value);
				m_EditExcludeLow.GetWindowText(Value);
				m_pValues->Set<CString>(SvPb::TableAnaylzerExcludeLowEId, Value);
				hrOk = m_pValues->Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);
				errorMessageList = m_pValues->getFailedMessageList();
			}
			break;
			case SvPb::TableAnalyzerLimitType:
			{
				CString Value;
				m_EditLimitValue.GetWindowText(Value);
				m_pValues->Set<CString>(SvPb::TableAnaylzerLimitValueEId, Value);
				hrOk = m_pValues->Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);
				errorMessageList = m_pValues->getFailedMessageList();
			}
			break;
			default:
				break;
		}
	}

	if (m_selectedSubType == SvPb::TableAnalyzerAddColumnType)
	{
		hrOk = SetAddAnalyzerData(errorMessageList);
	}

	UpdateData(FALSE);

	if (S_OK != hrOk)
	{
		if (0 < errorMessageList.size())
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(errorMessageList[0].getMessage());
		}
		else
		{
			//display an error if set failed.
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), hrOk));
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_SetTableAnalyzerData, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}

	return hrOk;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TaTableAnalyzerPage::refresh()
{
	SvUl::NameObjectIdList availableList = getTableAnalyzer();
	// Populate Analyzer list box and run Analyzer...
	m_analyzerListBox.init(availableList, Analyzer_NoAnalyzer);
	int index = m_analyzerListBox.GetCurSel();
	m_selectedAnalyzerID = m_analyzerListBox.getObjectId(index);

	SetPropertyControls();
}

void TaTableAnalyzerPage::SetPropertyControls()
{
	m_selectedSubType = SvPb::SVNotSetSubObjectType;
	if (SvDef::InvalidObjectId != m_selectedAnalyzerID)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
		pRequest->set_objectid(m_selectedAnalyzerID);
		
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
		{
			m_selectedSubType = responseCmd.getobjectparametersresponse().typeinfo().subtype();
		}
	}// end if

	ShowControls(m_selectedSubType);
	switch (m_selectedSubType)
	{
		case SvPb::TableAnalyzerSortType:
			setSortProperties();
			break;
		case SvPb::TableAnalyzerExcludeType:
			setExcludeProperties();
			break;
		case SvPb::TableAnalyzerLimitType:
			setLimitProperties();
			break;
		case SvPb::TableAnalyzerAddColumnType:
			setAddColumnProperties();
			break;
		default:
			break;
	}
	UpdateData(false);
}

HRESULT TaTableAnalyzerPage::RetrieveAvailableColumnList()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
	pRequest->set_objectid(m_TaskObjectID);
	pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVValueObjectType);
	pRequest->mutable_typeinfo()->set_subtype(SvPb::DoubleSortValueObjectType);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	SvUl::NameObjectIdList availableList;
	if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
	{
		m_availableColumn = SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());
	}
	return hr;
}

void TaTableAnalyzerPage::ShowControls(long SubType)
{
	GetDlgItem(IDC_COLUMN_SELECT_CBOX)->ShowWindow((SvPb::TableAnalyzerSortType == SubType || SvPb::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);

	GetDlgItem(IDC_SORT_LABEL)->ShowWindow((SvPb::TableAnalyzerSortType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_ASC_RADIO)->ShowWindow((SvPb::TableAnalyzerSortType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_DESC_RADIO)->ShowWindow((SvPb::TableAnalyzerSortType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_STATIC_ASC)->ShowWindow((SvPb::TableAnalyzerSortType == SubType) ? SW_SHOW : SW_HIDE);

	GetDlgItem(IDC_EXCLUDE_LABEL)->ShowWindow((SvPb::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_EXCLUDE_LABEL2)->ShowWindow((SvPb::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_EXCLUDE_LABEL3)->ShowWindow((SvPb::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);
	m_EditExcludeHigh.ShowWindow((SvPb::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);
	m_EditExcludeLow.ShowWindow((SvPb::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BUTTON_EXCLUDE_HIGH)->ShowWindow((SvPb::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BUTTON_EXCLUDE_LOW)->ShowWindow((SvPb::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);

	GetDlgItem(IDC_LIMIT_LABEL)->ShowWindow((SvPb::TableAnalyzerLimitType == SubType) ? SW_SHOW : SW_HIDE);
	m_EditLimitValue.ShowWindow((SvPb::TableAnalyzerLimitType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BUTTON_LIMIT_VALUE)->ShowWindow((SvPb::TableAnalyzerLimitType == SubType) ? SW_SHOW : SW_HIDE);

	GetDlgItem(IDC_ADDCOLUMN_LABEL)->ShowWindow((SvPb::TableAnalyzerAddColumnType == SubType) ? SW_SHOW : SW_HIDE);
	m_EditAddColumnName.ShowWindow((SvPb::TableAnalyzerAddColumnType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BUTTON_ADDCOLUMN_FORMULA)->ShowWindow((SvPb::TableAnalyzerAddColumnType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_EDIT_ADDCOLUMN_FORMULA)->ShowWindow((SvPb::TableAnalyzerAddColumnType == SubType) ? SW_SHOW : SW_HIDE);

	RedrawWindow();
}

void TaTableAnalyzerPage::setSortProperties()
{
	setColumnSelectionCB();

	if (nullptr == m_pValues || m_selectedAnalyzerID != m_pValues->GetTaskID())
	{
		m_pValues.reset();
		m_pValues = std::shared_ptr<ValueController> {new ValueController{ SvOg::BoundValues{ m_InspectionID, m_selectedAnalyzerID } }};
		m_pValues->Init();
	}

	m_SortDirection = m_pValues->Get<bool>(SvPb::TableAnaylzerSortIsASCEId) ? 0 : 1;
	UpdateData(FALSE);
}

void TaTableAnalyzerPage::setExcludeProperties()
{
	setColumnSelectionCB();

	if (nullptr == m_pValues || m_selectedAnalyzerID != m_pValues->GetTaskID())
	{
		m_pValues.reset();
		m_pValues = std::shared_ptr<ValueController> {new ValueController{ SvOg::BoundValues{ m_InspectionID, m_selectedAnalyzerID } }};
		m_pValues->Init();
	}

	std::string highString = m_pValues->Get<CString>(SvPb::TableAnaylzerExcludeHigh_LinkEId);
	if (highString.empty())
	{
		highString = m_pValues->Get<CString>(SvPb::TableAnaylzerExcludeHighEId);
	}
	m_EditExcludeHigh.SetWindowText(highString.c_str());
	std::string lowString = m_pValues->Get<CString>(SvPb::TableAnaylzerExcludeLow_LinkEId);
	if (lowString.empty())
	{
		lowString = m_pValues->Get<CString>(SvPb::TableAnaylzerExcludeLowEId);
	}
	m_EditExcludeLow.SetWindowText(lowString.c_str());
}

void TaTableAnalyzerPage::setLimitProperties()
{
	if (nullptr == m_pValues || m_selectedAnalyzerID != m_pValues->GetTaskID())
	{
		m_pValues.reset();
		m_pValues = std::shared_ptr<ValueController> {new ValueController{ SvOg::BoundValues{ m_InspectionID, m_selectedAnalyzerID } }};
		m_pValues->Init();
	}

	std::string valueString = m_pValues->Get<CString>(SvPb::TableAnaylzerLimitValue_LinkEId);
	if (valueString.empty())
	{
		valueString = m_pValues->Get<CString>(SvPb::TableAnaylzerLimitValueEId);
	}
	m_EditLimitValue.SetWindowText(valueString.c_str());
}

void TaTableAnalyzerPage::setAddColumnProperties()
{
	if (nullptr == m_pSelectedAddEquationFormula || m_selectedAnalyzerID != m_pSelectedAddEquationFormula->GetTaskId())
	{
		m_pSelectedAddEquationFormula.reset();
		SvDef::SVObjectTypeInfoStruct Info {SvPb::SVEquationObjectType, SvPb::IndexEquationObjectType };
		m_pSelectedAddEquationFormula = SvOi::IFormulaControllerPtr {new FormulaController {m_InspectionID, m_selectedAnalyzerID, Info}};
	}

	if (nullptr != m_pSelectedAddEquationFormula)
	{
		m_EditAddColumnName.SetWindowText(m_pSelectedAddEquationFormula->GetEquationName().c_str());
		m_AddFormulaString = m_pSelectedAddEquationFormula->GetEquationText().c_str();
	}
}

void TaTableAnalyzerPage::setColumnSelectionCB()
{
	RetrieveAvailableColumnList();
	std::string selectedTableName;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getinputsrequest();
	pRequest->set_objectid(m_selectedAnalyzerID);
	pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVValueObjectType);
	pRequest->mutable_typeinfo()->set_subtype(SvPb::DoubleSortValueObjectType);
	
	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	SvUl::InputNameObjectIdPairList connectedList;
	if (S_OK == hr && responseCmd.has_getinputsresponse() && 0 < responseCmd.getinputsresponse().list_size())
	{
		m_inputName = responseCmd.getinputsresponse().list(0).inputname();
		selectedTableName = responseCmd.getinputsresponse().list(0).objectname();
	}

	m_columnSelectionCB.Init(m_availableColumn, selectedTableName, NoColumnTag);
}

HRESULT TaTableAnalyzerPage::prepareSwitchOfAnalyzerSelection()
{
	//Set Values before change selection
	HRESULT Result = SetInspectionData();
	if (S_OK != Result)
	{
		int index = LB_ERR;
		for (int i = 0; i < m_analyzerListBox.GetCount(); i++)
		{
			if (m_analyzerListBox.getObjectId(i) == m_selectedAnalyzerID)
			{
				index = i;
				break;
			}
		}
		m_analyzerListBox.SetCurSel(index);
	}
	return Result;
}

HRESULT TaTableAnalyzerPage::checkAllAnaylzer()
{
	HRESULT hrOk = S_OK;
	//reset all analyzer to check if they correct
	for (int i = 0; S_OK == hrOk && i < m_analyzerListBox.GetCount(); i++)
	{
		uint32_t analyzerID = m_analyzerListBox.getObjectId(i);
		if (SvDef::InvalidObjectId != analyzerID && analyzerID != m_selectedAnalyzerID)
		{
			// Do a reset of the analyzer
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest = requestCmd.mutable_resetobjectrequest();
			pRequest->set_objectid(analyzerID);

			hrOk = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK != hrOk && responseCmd.has_standardresponse())
			{
				SvStl::MessageContainerVector errorMessageList = SvPb::convertProtobufToMessageVector(responseCmd.standardresponse().errormessages());
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				if (0 < errorMessageList.size())
				{
					Msg.setMessage(errorMessageList[0].getMessage());
				}
				else
				{
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
				}

				m_analyzerListBox.SetCurSel(i);
				m_selectedAnalyzerID = analyzerID;
				SetPropertyControls();
			}
		}
	}
	return hrOk;
}

HRESULT TaTableAnalyzerPage::SetAddAnalyzerData(SvStl::MessageContainerVector &rErrorMessageList)
{
	HRESULT hrOk = S_OK;
	if (nullptr != m_pSelectedAddEquationFormula)
	{
		CString columnName;
		m_EditAddColumnName.GetWindowText(columnName);

		std::string oldName = columnName;
		std::string newName = columnName;
		SvUl::RemoveCharacters(newName, SvDef::cGeneralExcludeChars);
		if (newName != oldName)
		{
			m_EditAddColumnName.SetWindowText(newName.c_str());
		}

		//Has the name changed
		if (newName != m_pSelectedAddEquationFormula->GetEquationName())
		{
			std::string CheckOnlySpaces {newName};
			SvUl::RemoveCharacters(CheckOnlySpaces, _T(" "));

			if (!newName.empty() && !CheckOnlySpaces.empty())
			{
				hrOk = m_pSelectedAddEquationFormula->SetEquationName(newName);
				if (S_OK != hrOk)
				{
					SvDef::StringVector msgList;
					msgList.push_back(SvUl::Format(_T("%d"), hrOk));
					SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_SetTableAnalyzerData, msgList, SvStl::SourceFileParams(StdMessageParams));
					rErrorMessageList.push_back(Msg);
					return E_FAIL;
				}


				SvPb::InspectionCmdRequest requestCmd;
				SvPb::InspectionCmdResponse responseCmd;
				auto* pRequest = requestCmd.mutable_resetobjectrequest();
				pRequest->set_objectid(m_TaskObjectID);

				hrOk = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
				if (S_OK != hrOk && responseCmd.has_standardresponse())
				{
					SvStl::MessageContainerVector taskResetMessageList = SvPb::convertProtobufToMessageVector(responseCmd.standardresponse().errormessages());
					for (const auto& rMessage : taskResetMessageList)
					{
						const auto& rMessageData = rMessage.getMessage();
						if (SvStl::Tid_TableColumnName_NotUnique == rMessageData.m_AdditionalTextId &&
							0 < rMessageData.m_AdditionalTextList.size() && 0 == columnName.Compare(rMessageData.m_AdditionalTextList[0].c_str()))
						{
							rErrorMessageList.push_back(rMessage);
						}
					}

					if (0 == rErrorMessageList.size())
					{
						requestCmd.Clear();
						responseCmd.Clear();
						//if error found, but no name error with this analyzer, check if this analyzer have an error
						auto* pGetMessageListRequest = requestCmd.mutable_getmessagelistrequest();
						pGetMessageListRequest->set_objectid(m_selectedAnalyzerID);

						HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
						if (hr == S_OK && responseCmd.has_standardresponse())
						{
							rErrorMessageList = SvPb::convertProtobufToMessageVector(responseCmd.standardresponse().errormessages());
						}
						if (0 < rErrorMessageList.size())
						{
							SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
							Msg.setMessage(rErrorMessageList[0].getMessage());
						}
						else
						{
							hrOk = S_OK;
						}
					}
				}
			}
			else
			{
				hrOk = E_FAIL;
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumnName_Empty, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
	return hrOk;
}

SvUl::NameObjectIdList TaTableAnalyzerPage::getTableAnalyzer()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
	pRequest->set_objectid(m_TaskObjectID);
	pRequest->mutable_typeinfo()->set_objecttype(SvPb::TableAnalyzerType);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
	{
		return SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());
	}	
	return SvUl::NameObjectIdList();
}

#pragma endregion Private Methods
} //namespace SvOg

