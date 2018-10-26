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
#include "Definitions/ObjectDefines.h"
#include "Definitions/StringTypeDef.h"
#include "SVCommandLibrary/SVObjectSynchronousCommandTemplate.h"
#include "TextDefinesSvOg.h"
#include "InspectionCommands/GetCreatableObjects.h"
#include "InspectionCommands/GetAvailableObjects.h"
#include "InspectionCommands/GetObjectTypeInfo.h"
#include "InspectionCommands/GetInstanceIDByTypeInfo.h"
#include "InspectionCommands/ConstructAndInsertTaskObject.h"
#include "InspectionCommands/ConnectToObject.h"
#include "InspectionCommands/GetInputs.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "InspectionCommands/CommandFunctionHelper.h"
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

static LPCTSTR NoColumnTag = _T("(No Column Available)");
static const std::string IsAscTag(_T("IsAsc"));
static const std::string ExcludeHighTag(_T("ExcludeHigh"));
static const std::string ExcludeHighLinkTag(_T("ExcludeHigh_Link"));
static const std::string ExcludeLowTag(_T("ExcludeLow"));
static const std::string ExcludeLowLinkTag(_T("ExcludeLow_Link"));
static const std::string LimitValueTag(_T("LimitValue"));
static const std::string LimitValueLinkTag(_T("LimitValue_Link"));

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
TaTableAnalyzerPage::TaTableAnalyzerPage(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID)
	: CPropertyPage(TaTableAnalyzerPage::IDD)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	, m_selectedAnalyzerID(GUID_NULL)
	, m_SortDirection(-1)
	, m_selectedSubType(SvDef::SVNotSetSubObjectType)
	, m_inputName(_T(""))
	, m_objectSelector(rInspectionID)
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

	typedef SvCmd::GetCreatableObjects Command;
	typedef std::shared_ptr<Command> CommandPtr;

	SvUl::NameGuidList availableList;
	CommandPtr commandPtr {new Command(m_TaskObjectID, SvDef::SVObjectTypeInfoStruct(SvDef::TableAnalyzerType, SvDef::SVNotSetSubObjectType))};
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		availableList = commandPtr->AvailableObjects();
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
	typedef SvCmd::GetAvailableObjects Command;
	typedef std::shared_ptr<Command> CommandPtr;
	SvUl::NameGuidList availableList;
	CommandPtr commandPtr {new Command(m_TaskObjectID, SvDef::SVObjectTypeInfoStruct(SvDef::TableAnalyzerType, SvDef::SVNotSetSubObjectType))};
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		availableList = commandPtr->AvailableObjects();
	}

	// remove all Analyzer items (instantiated)
	size_t listSize = availableList.size();
	for (int i = 0; i < listSize; ++i)
	{
		SVGUID analyzerGUID = availableList[i].second;
		if (GUID_NULL != analyzerGUID)
		{
			// Close, Disconnect and Delete it
			SvPb::InspectionCmdMsgs Request,Response;
			SvPb::DestroyChildRequest* pDestroyChildRequest = Request.mutable_destroychildrequest(); ;
			pDestroyChildRequest->set_flag(SvPb::DestroyChildRequest::Flag_None);
			SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_taskobjectlistid(), m_TaskObjectID);
			SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_objectid(), analyzerGUID);
			SvCmd::InspectionCommandsSynchronous(m_InspectionID, &Request, &Response);
		}
	}

	m_analyzerListBox.SetCurSel(-1);

	// Refresh Dialog...
	refresh();
}

void TaTableAnalyzerPage::OnButtonDeleteCurrentAnalyzer()
{
	if (GUID_NULL != m_selectedAnalyzerID)
	{
		// Close, Disconnect and Delete it
		SvPb::InspectionCmdMsgs Request,Response;
		SvPb::DestroyChildRequest* pDestroyChildRequest = Request.mutable_destroychildrequest(); ;
		SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_taskobjectlistid(), m_TaskObjectID);
		SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_objectid(), m_selectedAnalyzerID);
		SvCmd::InspectionCommandsSynchronous(m_InspectionID, &Request, &Response);
		m_selectedAnalyzerID = GUID_NULL;
		m_selectedSubType = SvDef::SVNotSetSubObjectType;

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

	const SVGUID classID = m_availableAnaylzerCB.getSelectedGUID();

	if (GUID_NULL != classID)
	{
		int destinyIndex = m_analyzerListBox.GetCurSel();

		if (LB_ERR == destinyIndex) // First Entryitem
		{
			destinyIndex = 0;
		}

		// Construct and Create the Analyzer Class Object
		typedef SvCmd::ConstructAndInsertTaskObject Command;
		typedef std::shared_ptr<Command> CommandPtr;
		CommandPtr commandPtr {new Command(m_TaskObjectID, classID, destinyIndex)};
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK != hr)
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
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
	m_selectedAnalyzerID = m_analyzerListBox.getGUID(index);

	SetPropertyControls();
}// end OnSelchangeAnalyzerList


void TaTableAnalyzerPage::OnChangeColumnSelection()
{
	UpdateData(TRUE); // get data from dialog

	SVGUID columnGuid = m_columnSelectionCB.getSelectedGUID();
	if (GUID_NULL != columnGuid && GUID_NULL != m_selectedAnalyzerID)
	{
		HRESULT hr = E_INVALIDARG;
		typedef SvCmd::ConnectToObject Command;
		typedef std::shared_ptr<Command> CommandPtr;
		CommandPtr commandPtr {new Command(m_selectedAnalyzerID, m_inputName, columnGuid, SvDef::SVValueObjectType)};
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK != hr)
		{
			SvDef::StringVector msgList;
			msgList.push_back(m_inputName);
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
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
	if (m_objectSelector.Show(Value, Title, this))
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
	if (m_objectSelector.Show(Value, Title, this))
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
	if (m_objectSelector.Show(Value, Title, this))
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
			case SvDef::TableAnalyzerSortType:
				m_pValues->Set<bool>(TableAnaylzerSortIsASCGuid, 0 == m_SortDirection);
				hrOk = m_pValues->Commit();
				errorMessageList = m_pValues->getFailedMessageList();
				break;
			case SvDef::TableAnalyzerExcludeType:
			{
				CString Value;
				m_EditExcludeHigh.GetWindowText(Value);
				m_pValues->Set<CString>(TableAnaylzerExcludeHighGuid, Value);
				m_EditExcludeLow.GetWindowText(Value);
				m_pValues->Set<CString>(TableAnaylzerExcludeLowGuid, Value);
				hrOk = m_pValues->Commit(SvOg::doResetRunOnce);
				errorMessageList = m_pValues->getFailedMessageList();
			}
			break;
			case SvDef::TableAnalyzerLimitType:
			{
				CString Value;
				m_EditLimitValue.GetWindowText(Value);
				m_pValues->Set<CString>(TableAnaylzerLimitValueGuid, Value);
				hrOk = m_pValues->Commit(SvOg::doResetRunOnce);
				errorMessageList = m_pValues->getFailedMessageList();
			}
			break;
			default:
				break;
		}
	}

	if (m_selectedSubType == SvDef::TableAnalyzerAddColumnType)
	{
		hrOk = SetAddAnalyzerData(errorMessageList);
	}

	UpdateData(FALSE);

	if (S_OK != hrOk)
	{
		if (0 < errorMessageList.size())
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(errorMessageList[0].getMessage());
		}
		else
		{
			//display an error if set failed.
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), hrOk));
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_SetTableAnalyzerData, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}

	return hrOk;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TaTableAnalyzerPage::refresh()
{
	typedef SvCmd::GetAvailableObjects Command;
	typedef std::shared_ptr<Command> CommandPtr;

	SvUl::NameGuidList availableList;
	CommandPtr commandPtr {new Command(m_TaskObjectID, SvDef::SVObjectTypeInfoStruct(SvDef::TableAnalyzerType, SvDef::SVNotSetSubObjectType))};
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		availableList = commandPtr->AvailableObjects();
	}

	// Populate Analyzer list box and run Analyzer...
	m_analyzerListBox.init(availableList, Analyzer_NoAnalyzer);
	int index = m_analyzerListBox.GetCurSel();
	m_selectedAnalyzerID = m_analyzerListBox.getGUID(index);

	SetPropertyControls();
}

void TaTableAnalyzerPage::SetPropertyControls()
{
	m_selectedSubType = SvDef::SVNotSetSubObjectType;
	if (GUID_NULL != m_selectedAnalyzerID)
	{
		typedef SvCmd::GetObjectTypeInfo Command;
		typedef std::shared_ptr<Command> CommandPtr;
		CommandPtr commandPtr {new Command(m_selectedAnalyzerID)};
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			SvDef::SVObjectTypeInfoStruct info = commandPtr->GetTypeInfo();
			m_selectedSubType = info.SubType;
		}
	}// end if

	ShowControls(m_selectedSubType);
	switch (m_selectedSubType)
	{
		case SvDef::TableAnalyzerSortType:
			setSortProperties();
			break;
		case SvDef::TableAnalyzerExcludeType:
			setExcludeProperties();
			break;
		case SvDef::TableAnalyzerLimitType:
			setLimitProperties();
			break;
		case SvDef::TableAnalyzerAddColumnType:
			setAddColumnProperties();
			break;
		default:
			break;
	}
	UpdateData(false);
}

HRESULT TaTableAnalyzerPage::RetrieveAvailableColumnList()
{
	typedef SvCmd::GetAvailableObjects Command;
	typedef std::shared_ptr<Command> CommandPtr;

	CommandPtr commandPtr {new Command(m_TaskObjectID, SvDef::SVObjectTypeInfoStruct(SvDef::SVValueObjectType, SvDef::DoubleSortValueObjectType))};
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		m_availableColumn = commandPtr->AvailableObjects();
	}
	return hr;
}

void TaTableAnalyzerPage::ShowControls(long SubType)
{
	GetDlgItem(IDC_COLUMN_SELECT_CBOX)->ShowWindow((SvDef::TableAnalyzerSortType == SubType || SvDef::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);

	GetDlgItem(IDC_SORT_LABEL)->ShowWindow((SvDef::TableAnalyzerSortType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_ASC_RADIO)->ShowWindow((SvDef::TableAnalyzerSortType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_DESC_RADIO)->ShowWindow((SvDef::TableAnalyzerSortType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_STATIC_ASC)->ShowWindow((SvDef::TableAnalyzerSortType == SubType) ? SW_SHOW : SW_HIDE);

	GetDlgItem(IDC_EXCLUDE_LABEL)->ShowWindow((SvDef::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_EXCLUDE_LABEL2)->ShowWindow((SvDef::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_EXCLUDE_LABEL3)->ShowWindow((SvDef::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);
	m_EditExcludeHigh.ShowWindow((SvDef::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);
	m_EditExcludeLow.ShowWindow((SvDef::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BUTTON_EXCLUDE_HIGH)->ShowWindow((SvDef::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BUTTON_EXCLUDE_LOW)->ShowWindow((SvDef::TableAnalyzerExcludeType == SubType) ? SW_SHOW : SW_HIDE);

	GetDlgItem(IDC_LIMIT_LABEL)->ShowWindow((SvDef::TableAnalyzerLimitType == SubType) ? SW_SHOW : SW_HIDE);
	m_EditLimitValue.ShowWindow((SvDef::TableAnalyzerLimitType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BUTTON_LIMIT_VALUE)->ShowWindow((SvDef::TableAnalyzerLimitType == SubType) ? SW_SHOW : SW_HIDE);

	GetDlgItem(IDC_ADDCOLUMN_LABEL)->ShowWindow((SvDef::TableAnalyzerAddColumnType == SubType) ? SW_SHOW : SW_HIDE);
	m_EditAddColumnName.ShowWindow((SvDef::TableAnalyzerAddColumnType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BUTTON_ADDCOLUMN_FORMULA)->ShowWindow((SvDef::TableAnalyzerAddColumnType == SubType) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_EDIT_ADDCOLUMN_FORMULA)->ShowWindow((SvDef::TableAnalyzerAddColumnType == SubType) ? SW_SHOW : SW_HIDE);

	RedrawWindow();
}

void TaTableAnalyzerPage::setSortProperties()
{
	setColumnSelectionCB();

	if (nullptr == m_pValues || m_selectedAnalyzerID != m_pValues->GetTaskID())
	{
		m_pValues.reset();
		m_pValues = std::shared_ptr<Controller> {new Controller{ SvOg::BoundValues{ m_InspectionID, m_selectedAnalyzerID } }};
		m_pValues->Init();
	}

	m_SortDirection = m_pValues->Get<bool>(TableAnaylzerSortIsASCGuid) ? 0 : 1;
	UpdateData(FALSE);
}

void TaTableAnalyzerPage::setExcludeProperties()
{
	setColumnSelectionCB();

	if (nullptr == m_pValues || m_selectedAnalyzerID != m_pValues->GetTaskID())
	{
		m_pValues.reset();
		m_pValues = std::shared_ptr<Controller> {new Controller{ SvOg::BoundValues{ m_InspectionID, m_selectedAnalyzerID } }};
		m_pValues->Init();
	}

	std::string highString = m_pValues->Get<CString>(TableAnaylzerExcludeHigh_LinkGuid);
	if (highString.empty())
	{
		highString = m_pValues->Get<CString>(TableAnaylzerExcludeHighGuid);
	}
	m_EditExcludeHigh.SetWindowText(highString.c_str());
	std::string lowString = m_pValues->Get<CString>(TableAnaylzerExcludeLow_LinkGuid);
	if (lowString.empty())
	{
		lowString = m_pValues->Get<CString>(TableAnaylzerExcludeLowGuid);
	}
	m_EditExcludeLow.SetWindowText(lowString.c_str());
}

void TaTableAnalyzerPage::setLimitProperties()
{
	if (nullptr == m_pValues || m_selectedAnalyzerID != m_pValues->GetTaskID())
	{
		m_pValues.reset();
		m_pValues = std::shared_ptr<Controller> {new Controller{ SvOg::BoundValues{ m_InspectionID, m_selectedAnalyzerID } }};
		m_pValues->Init();
	}

	std::string valueString = m_pValues->Get<CString>(TableAnaylzerLimitValue_LinkGuid);
	if (valueString.empty())
	{
		valueString = m_pValues->Get<CString>(TableAnaylzerLimitValueGuid);
	}
	m_EditLimitValue.SetWindowText(valueString.c_str());
}

void TaTableAnalyzerPage::setAddColumnProperties()
{
	if (nullptr == m_pSelectedAddEquationFormula || m_selectedAnalyzerID != m_pSelectedAddEquationFormula->GetTaskId())
	{
		m_pSelectedAddEquationFormula.reset();
		SvDef::SVObjectTypeInfoStruct Info {SvDef::SVEquationObjectType, SvDef::TableAddColumnEquationObjectType};
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
	typedef SvCmd::GetInputs Command;
	typedef std::shared_ptr<Command> CommandPtr;
	CommandPtr commandPtr {new Command(m_selectedAnalyzerID, SvDef::SVObjectTypeInfoStruct(SvDef::SVValueObjectType, SvDef::DoubleSortValueObjectType))};
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		const SvUl::InputNameGuidPairList& rConnectedList = commandPtr->ConnectedObjects();
		if (rConnectedList.size())
		{
			m_inputName = rConnectedList.begin()->first;
			selectedTableName = rConnectedList.begin()->second.first;
		}
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
			if (m_analyzerListBox.getGUID(i) == m_selectedAnalyzerID)
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
		SVGUID analyzerGUID = m_analyzerListBox.getGUID(i);
		if (GUID_NULL != analyzerGUID && analyzerGUID != m_selectedAnalyzerID)
		{
			// Do a reset of the analyzer
			SvPb::InspectionCmdMsgs Request, Response;
			SvPb::ResetObjectRequest* pResetObjectRequest = Request.mutable_resetobjectrequest();
			SvPb::SetGuidInProtoBytes(pResetObjectRequest->mutable_objectid(), analyzerGUID);
			hrOk = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &Request, &Response);
			if (S_OK != hrOk && Response.has_resetobjectresponse())
			{
				SvStl::MessageContainerVector errorMessageList = SvCmd::setMessageContainerFromMessagePB(Response.resetobjectresponse().messages());
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				if (0 < errorMessageList.size())
				{
					Msg.setMessage(errorMessageList[0].getMessage());
				}
				else
				{
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
				}

				m_analyzerListBox.SetCurSel(i);
				m_selectedAnalyzerID = analyzerGUID;
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


				SvPb::InspectionCmdMsgs Request, Response;
				SvPb::ResetObjectRequest* pResetObjectRequest = Request.mutable_resetobjectrequest();
				SvPb::SetGuidInProtoBytes(pResetObjectRequest->mutable_objectid(), m_TaskObjectID);
				hrOk = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &Request, &Response);
				if (S_OK != hrOk && Response.has_resetobjectresponse())
				{
					SvStl::MessageContainerVector taskResetMessageList = SvCmd::setMessageContainerFromMessagePB(Response.resetobjectresponse().messages());
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
					{	//if error found, but no name error with this analyzer, check if this analyzer have an error

						SvPb::InspectionCmdMsgs Request, Response;
						SvPb::GetMessageListRequest* pGetMessageListRequest = Request.mutable_getmessagelistrequest();
						SvPb::SetGuidInProtoBytes(pGetMessageListRequest->mutable_objectid(), m_selectedAnalyzerID);
						HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &Request, &Response);

						if (hr == S_OK && Response.has_getmessagelistresponse())
						{
							rErrorMessageList = SvCmd::setMessageContainerFromMessagePB(Response.getmessagelistresponse().messages());
						}
						if (0 < rErrorMessageList.size())
						{
							SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
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
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumnName_Empty, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
	return hrOk;
}

#pragma endregion Private Methods
} //namespace SvOg

