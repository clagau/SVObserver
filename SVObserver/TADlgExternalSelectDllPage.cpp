//******************************************************************************
//* COPYRIGHT (c) 2004 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgExternalSelectDllPage.cpp
//* .File Name       : $Workfile:   TADlgExternalSelectDllPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   18 Sep 2014 13:39:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVSecurity/SVSecurityManager.h"
#include "TADlgExternalSelectDllPage.h"
#include "SVOGuiUtility/ExternalToolTaskController.h"
#include "Definitions/SVUserMessage.h"
#include "SVMFCControls/SVFileDialog.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary/MessageTextGenerator.h"
#include "SVSystemLibrary/FileVersion.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"

//#include <Psapi.h>
#pragma endregion Includes
#include <tlhelp32.h> 
#include "SVUtilityLibrary/ModuleInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr const char* cCRLF(_T("\r\n"));

enum { WM_UPDATE_STATUS = WM_APP + 100 };

/////////////////////////////////////////////////////////////////////////////
// TADlgExternalSelectDllPage dialog

namespace	// file scope
{
	void YieldPaintMessages(HWND hWnd /* = nullptr */)
	{// Let other PAINT messages through

		MSG msg;

		while (PeekMessage(&msg, hWnd, WM_PAINT, WM_PAINT, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

BEGIN_MESSAGE_MAP(TADlgExternalSelectDllPage, CPropertyPage)
	//{{AFX_MSG_MAP(TADlgExternalSelectDllPage)
	ON_BN_CLICKED(IDC_DELETE_ALL, OnDeleteAll)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_STATUS, OnUpdateStatus)
END_MESSAGE_MAP()

TADlgExternalSelectDllPage::TADlgExternalSelectDllPage(uint32_t inspectionID, uint32_t toolObjectID, CWnd& rParent, SvOgu::ExternalToolTaskController& rExternalToolTaskController)
	: CPropertyPage(IDD)
	, m_InspectionID(inspectionID)
	, m_ToolObjectID(toolObjectID) //attention: TADlgSheetClass::m_TaskObjectID is passed to this value when this constructor is called by TADlgSheetClass!
	, m_rParent(rParent)
	, m_rExternalToolTaskController(rExternalToolTaskController)
	, m_valueControllerToolTask {SvOgu::BoundValues{ inspectionID, m_rExternalToolTaskController.getExternalToolTaskObjectId() }}
{
	//{{AFX_DATA_INIT(TADlgExternalSelectDllPage)
	m_currentExternalDllFilepath = _T("");
	m_strStatus = _T("");
	//}}AFX_DATA_INIT
}


void TADlgExternalSelectDllPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADlgExternalSelectDllPage)
	DDX_Control(pDX, IDC_DLL_STATUS, m_StatusEdit);
	DDX_Control(pDX, IDC_DELETE_ALL, m_btnDeleteAll);
	DDX_Control(pDX, IDC_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_BROWSE, m_btnBrowse);
	DDX_Control(pDX, IDC_ADD, m_btnAdd);
	DDX_Check(pDX, IDC_RESET_INPUT, m_ResetInput);
	DDX_Control(pDX, IDC_DEPENDENT_LIST, m_lbDependentList);
	DDX_Text(pDX, IDC_DLL_PATH, m_currentExternalDllFilepath);
	DDX_Text(pDX, IDC_DLL_STATUS, m_strStatus);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// TADlgExternalSelectDllPage message handlers

BOOL TADlgExternalSelectDllPage::OnInitDialog()
{
	// stuff the value before base OnInitDialog

	m_valueControllerToolTask.Init();

	auto strDllPath = getStdStringFromValueController(SvPb::EmbeddedIdEnum::DllFileNameEId);
	m_currentExternalDllFilepath = strDllPath.c_str();

	CPropertyPage::OnInitDialog();	// create button and list box windows

	m_previousExternalDllDirectorypath = AfxGetApp()->GetProfileString(_T("Settings"), _T("Last External Tool Dll Path"), SvStl::GlobalPath::Inst().GetRunPath().c_str());

	m_ToolTip.Create(this, TTS_NOPREFIX | WS_VISIBLE);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.SetDelayTime(32000, TTDT_AUTOPOP);
	m_ToolTip.SetDelayTime(10, TTDT_INITIAL);
	m_ToolTip.SetDelayTime(250, TTDT_RESHOW);

	InitializeDllAndDisplayResults(true);
	
	for (auto i = 0; i < SvOgu::ExternalToolTaskController::NUM_TOOL_DEPENDENCIES; ++i)
	{
		auto tempDependency = m_valueControllerToolTask.Get<_variant_t>(SvPb::EmbeddedIdEnum::DllDependencyFileNameEId + i);
		auto dependency = SvUl::VariantToString(tempDependency);
		if (false == dependency.empty())
		{ 
			m_lbDependentList.AddString(dependency.c_str());
		}
	}

	PostMessage(WM_UPDATE_STATUS, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE

}

void TADlgExternalSelectDllPage::OnOK()
{
	UpdateData();

	// set dll path
	m_valueControllerToolTask.Set<CString>(SvPb::EmbeddedIdEnum::DllFileNameEId, m_currentExternalDllFilepath);
	
	// Copy DLL Dependents from listbox to Task Class FileNameValueObject List...
	SetDependencies();

	try
	{
		m_rExternalToolTaskController.initialize(false);

		m_rExternalToolTaskController.resetAllObjects(false); //first error will be displayed anyway when containing sheet is closed
	}
	catch (const SvStl::MessageContainer&)
	{
	}

	m_valueControllerToolTask.Commit(SvOgu::PostAction::doNothing);

	m_rParent.SendMessage(SV_UPDATE_IPDOC_VIEWS);

	CPropertyPage::OnOK();
}

void TADlgExternalSelectDllPage::OnDeleteAll()
{
	// Clear List Box
	m_lbDependentList.ResetContent();
	testExternalDll();
}

void TADlgExternalSelectDllPage::OnDelete()
{
	int iCurrentPos = m_lbDependentList.GetCurSel();
	if (iCurrentPos > -1 && iCurrentPos < m_lbDependentList.GetCount())
	{
		m_lbDependentList.DeleteString(iCurrentPos);
		testExternalDll();
	}
}

void TADlgExternalSelectDllPage::OnAdd()
{
	//@TODO [Arvid][10.20][13.7.2021] is this necessary? otherwise we could do without #include "SVObserver/svobserver.h" and move this file and its header file to SVOGui
	bool bFullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS); 
	//@TODO [Arvid][10.20][18.10.2021] maybe we could have a singleton for the security manager?
	SvMc::SVFileDialog cfd(true, bFullAccess, _T("dll"), _T(""),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("All Files (*.*)|*.*|Dynamic Link Library(*.dll)|*.dll||"));

	TCHAR tszPath[MAX_PATH] = { 0 };
	_tcscpy(tszPath, m_previousExternalDllDirectorypath);
	cfd.m_ofn.lpstrInitialDir = tszPath;
	cfd.m_ofn.lpstrTitle = _T("Select File");

	if (cfd.DoModal() == IDOK)
	{
		// Extract File Name
		m_previousExternalDllDirectorypath = cfd.GetPathName();
		int iFind = m_previousExternalDllDirectorypath.ReverseFind(_T('\\'));
		if (iFind >= 0)
		{
			m_previousExternalDllDirectorypath = m_previousExternalDllDirectorypath.Left(iFind);
		}
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("Last External Tool Dll Path"), m_previousExternalDllDirectorypath);

		// Check for already existing DLL Paths
		if (LB_ERR == m_lbDependentList.FindString(0, cfd.GetPathName()))
		{
			m_lbDependentList.AddString(cfd.GetPathName());
		}

		testExternalDll();
	}
}

void TADlgExternalSelectDllPage::OnBrowse()
{
	UpdateData();

	SvOgu::ValueController valueControllerTool {SvOgu::BoundValues{ m_InspectionID, m_ToolObjectID}};

	valueControllerTool.Init();

	if (valueControllerTool.Get<bool>(SvPb::ConditionalEditFreezeFlagEId))
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ExternalDllEditModeFreezeSetNoDllLoad, SvStl::SourceFileParams(StdMessageParams));
		return;
	}

	bool bFullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS); //@TODO [Arvid][10.20][13.7.2021] is this necessary? otherwise we could do without #include "SVObserver/svobserver.h" and move this file and its header file to SVOGui
	SvMc::SVFileDialog cfd(true, bFullAccess, _T("dll"), _T(""),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Dynamic Link Library(*.dll)|*.dll|All Files (*.*)|*.*||"));

	TCHAR tszPath[MAX_PATH] = { 0 };
	_tcscpy(tszPath, m_previousExternalDllDirectorypath);
	cfd.m_ofn.lpstrInitialDir = tszPath;
	cfd.m_ofn.lpstrTitle = _T("Select File");

	if (cfd.DoModal() == IDOK)
	{
		m_previousExternalDllDirectorypath = cfd.GetPathName();
		int iFind = m_previousExternalDllDirectorypath.ReverseFind(_T('\\'));
		if (iFind >= 0)
		{
			m_previousExternalDllDirectorypath = m_previousExternalDllDirectorypath.Left(iFind);
		}
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("Last External Tool Dll Path"), m_previousExternalDllDirectorypath);

		m_currentExternalDllFilepath = cfd.GetPathName();
		std::string DLLname = cfd.GetFileName().GetString();
		UpdateData(FALSE);

		//The dll path has to be completely processed by the underlying object,
		//thus a commit is necessary after set and an init is necessary before get in order to fetch the processed value.
		std::string dllPath(m_currentExternalDllFilepath.GetString());
		m_valueControllerToolTask.Set<CString>(SvPb::EmbeddedIdEnum::DllFileNameEId, m_currentExternalDllFilepath);
		m_valueControllerToolTask.Commit(SvOgu::PostAction::doNothing);

		m_valueControllerToolTask.Init();
		auto runpath{ getStdStringFromValueController(SvPb::EmbeddedIdEnum::DllFileNameEId) };

		m_preserveStatus = false;
		if (runpath != dllPath)
		{
			//Unload current dll  
			m_valueControllerToolTask.Set<CString>(SvPb::EmbeddedIdEnum::DllFileNameEId, CString());
			m_valueControllerToolTask.Commit(SvOgu::PostAction::doNothing);

			InitializeDllAndDisplayResults(false, false);
			m_valueControllerToolTask.Set<CString>(SvPb::EmbeddedIdEnum::DllFileNameEId, m_currentExternalDllFilepath);
				
			bool isVersionEqual = SvSyl::FileVersion::isEqual(runpath.c_str(), dllPath.c_str());

			if (false == isVersionEqual)
			{
				//check if previous version is still used
				bool isUsed =  SvUl::ModuleInfo::isProcessModuleName(GetCurrentProcessId(), DLLname);

				if (isUsed)
				{
					std::string Status = SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_CouldNotCopyDll);
					m_strStatus = Status.c_str();
					m_strStatus += cCRLF;
					m_preserveStatus = true;

					m_currentExternalDllFilepath = _T("");
					UpdateData(FALSE);
					m_valueControllerToolTask.Set<CString>(SvPb::EmbeddedIdEnum::DllFileNameEId, m_currentExternalDllFilepath);
				}
			}
		}
		m_rExternalToolTaskController.clearData();

		showDllConfigurationPages(false);
		testExternalDll(m_ResetInput == TRUE);
	}
}// end void TADlgExternalSelectDllPage::OnBrowse() 

void TADlgExternalSelectDllPage::testExternalDll(bool setDefaultValues)
{
	UpdateData();

	SetDependencies();

	// DLL Path
	m_valueControllerToolTask.SetDefault<CString>(SvPb::EmbeddedIdEnum::DllFileNameEId, m_currentExternalDllFilepath);
	// ensure that all data are available for initialize dll
	m_valueControllerToolTask.Commit(SvOgu::PostAction::doNothing);
	InitializeDllAndDisplayResults(false, setDefaultValues);
}

void TADlgExternalSelectDllPage::setDefaultValuesForInputs()
{
	
	auto inputDefinitions = m_rExternalToolTaskController.getInputValuesDefinition();
	int size = inputDefinitions.inputvaluesdefinition_size();
	for (int i = 0; i < size; i++)
	{
		auto& rInputDef = inputDefinitions.inputvaluesdefinition()[i];
		int LinkValueIndex = rInputDef.linkedvalueindex();
		auto data = m_valueControllerToolTask.Get<SvOgu::LinkedValueData>(SvPb::EmbeddedIdEnum::ExternalInputEId + LinkValueIndex);
		if (rInputDef.type() == SvPb::ExDllInterfaceType::Scalar || rInputDef.type() == SvPb::ExDllInterfaceType::Array)
		{
			_variant_t value;
			SvPb::ConvertProtobufToVariant(rInputDef.defaultvalue(), value);
			data.m_defaultValue = value;
			data.m_directValue = value;
			data.m_Value = value;
			data.m_selectedOption = SvPb::DirectValue;
		}
		else if (rInputDef.type() == SvPb::ExDllInterfaceType::TableArray || rInputDef.type() == SvPb::ExDllInterfaceType::TableNames)
		{
			data.m_defaultValue = _variant_t {};
			data.m_directValue = _variant_t {};
			data.m_Value = _variant_t {};
			data.m_selectedOption = SvPb::IndirectValue;
			data.m_indirectIdName = "VOID";
		}
		m_valueControllerToolTask.Set<SvOgu::LinkedValueData>(SvPb::EmbeddedIdEnum::ExternalInputEId + LinkValueIndex, data);
	}
}

std::string TADlgExternalSelectDllPage::getStdStringFromValueController(SvPb::EmbeddedIdEnum objectId, int index)
{
	std::string Value(m_valueControllerToolTask.Get<CString>(objectId + index));
	if (Value.empty())
	{
		_variant_t temp = m_valueControllerToolTask.Get<_variant_t>(objectId + index);

		Value = SvUl::VariantToString(temp);
	}
	return Value;
}

void TADlgExternalSelectDllPage::SetDependencies()
{
	int i(0);
	
	for (auto idxDep = 0; idxDep < SvOgu::ExternalToolTaskController::NUM_TOOL_DEPENDENCIES; ++idxDep)
	{
		//GetDefault() could be called only once if the condition "all dependencies have the same defaut value" holds TRUE always. 
		//Assumption: the existing default value is correct, so use it to "reset" the actual value with the existing default value
		m_valueControllerToolTask.Set<CString>(SvPb::EmbeddedIdEnum::DllDependencyFileNameEId + idxDep, 
		m_valueControllerToolTask.GetDefault<CString>(SvPb::EmbeddedIdEnum::DllDependencyFileNameEId + idxDep));
	}

	// Set all File Paths from listbox
	int ilbSize = m_lbDependentList.GetCount();
	for (i = 0; i < ilbSize; i++)
	{
		CString Temp;
		m_lbDependentList.GetText(i, Temp);
		m_valueControllerToolTask.Set<CString>(SvPb::EmbeddedIdEnum::DllDependencyFileNameEId + i, Temp);
	}

	m_rExternalToolTaskController.setAllAttributes();	// update dependency attributes
}

void TADlgExternalSelectDllPage::showDllConfigurationPages(bool show)
{
	m_rParent.PostMessage (show ? SV_ADAPT_TO_TESTED_DLL : SV_ADAPT_TO_UNTESTED_DLL);
}

void TADlgExternalSelectDllPage::InitializeDllAndDisplayResults(bool jumpToInputPage, bool setDefaultValues)
{
	bool dllWasInitialized = false;

	try
	{
		showDllConfigurationPages(false);

		bool problemsOccurred = InitializeDll(setDefaultValues);

		dllWasInitialized = true;

		// if we arrive here, basic initialization has been successful
		// now let's check for problems the initialized DLL might have

		auto [ok, firstError] = m_rExternalToolTaskController.resetAllObjects(true);

		if (!ok)
		{
			problemsOccurred = true;
			jumpToInputPage = false; //an error occurred here - we want to remain on the selection page and see what happened

			displayInitializationProblem(firstError);
		}

		SvDef::StringVector msgList;
		msgList.emplace_back(problemsOccurred ? SvStl::MessageData::convertId2AdditionalText(SvStl::Tid_ExternalDllProblemsOccurred) : "");
		SvStl::MessageManager Msg(SvStl::MsgType::Log | (problemsOccurred ? SvStl::MsgType::Display : SvStl::MsgType::Data));
		Msg.setMessage(problemsOccurred ? SVMSG_SVO_93_GENERAL_WARNING : SVMSG_SVO_94_GENERAL_Informational,
			SvStl::Tid_ExternalDllPassesTheTest, msgList, SvStl::SourceFileParams(StdMessageParams), 0, m_ToolObjectID);

		auto mc = Msg.getMessageContainer();
			
		m_strStatus += mc.What().c_str();
		m_strStatus += cCRLF;

		UpdateData(FALSE);

	}
	catch (const SvStl::MessageContainer& e)
	{
		// if we arrive here, initialization has failed 

		displayInitializationFailure(e);
	}
	showDllConfigurationPages(dllWasInitialized);

	if (dllWasInitialized && jumpToInputPage)
	{
		m_rParent.PostMessage(SV_SELECT_INPUT_VALUE_PAGE);
	}
}

bool TADlgExternalSelectDllPage::InitializeDll(bool setDefaultValues)
{
	bool problemsOccurred = false;

	if (!m_preserveStatus)
	{
		m_strStatus.Empty();
	}
	UpdateData(FALSE);
	//must throw
	auto [hrInitialize, statusMessagesResponse] = m_rExternalToolTaskController.initialize(setDefaultValues);

	if (S_OK != hrInitialize)
	{
		problemsOccurred = true; // descriptions are contained in statusMessagesResponse.statusmessages()
	}

	for (const auto& message : statusMessagesResponse.statusmessages())
	{
		m_strStatus += message.c_str();
		m_strStatus += cCRLF;
	}

	UpdateData(FALSE);

	if (setDefaultValues)
	{
		setDefaultValuesForInputs();
	}

	return problemsOccurred;
}

void TADlgExternalSelectDllPage::displayInitializationProblem(std::string error)
{
	error = std::regex_replace(error, std::regex("\\("), cCRLF + std::string("  ("));
	error = std::regex_replace(error, std::regex("\\)\\s?"), std::string(")") + cCRLF);

	m_strStatus += error.c_str();
	m_strStatus += cCRLF;

	SvStl::MessageContainer mc;
	mc.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorInReset, SvStl::SourceFileParams(StdMessageParams), m_ToolObjectID);

	m_strStatus += mc.What().c_str();
	m_strStatus += cCRLF;
}

void TADlgExternalSelectDllPage::displayInitializationFailure(const SvStl::MessageContainer& e)
{
	// display all sub-errors in box
	m_strStatus += _T("External DLL: test failed!");
	m_strStatus += cCRLF;

	if (!e.getMessage().getAdditionalText().empty())
	{
		m_strStatus += e.getMessage().getAdditionalText().c_str();
		m_strStatus += cCRLF;
	}
	const SvStl::Messages& AdditionalMessages(e.getAdditionalMessages());
	SvStl::Messages::const_iterator iter;
	for (iter = AdditionalMessages.begin(); iter != AdditionalMessages.end(); ++iter)
	{
		m_strStatus += iter->getAdditionalText().c_str();
		m_strStatus += cCRLF;
	}

	UpdateData(FALSE);
}

// Override PreTranslateMessage() so RelayEvent() can be 
// called to pass a mouse message to the 
// tooltip control for processing.
BOOL TADlgExternalSelectDllPage::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}


LRESULT TADlgExternalSelectDllPage::OnUpdateStatus(WPARAM, LPARAM)
{
	UpdateData();
	return 0;
}


bool TADlgExternalSelectDllPage::QueryAllowExit()
{
	return true;
}


BOOL TADlgExternalSelectDllPage::OnKillActive()
{
	// Since the ValueController used here share the same data with the one used on another page (see TADlgExternalInputSelectPage),
	// we need to avoid unwanted interactions between the two (e.g. during navigation between tabs). 
	// Therefore, ensure a Commit() w/o call for each controller by page leave.
	m_valueControllerToolTask.Commit(SvOgu::PostAction::doNothing);

	return __super::OnKillActive();
}
