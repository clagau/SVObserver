//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgExternalSelectDllPage.cpp
//* .File Name       : $Workfile:   SVTADlgExternalSelectDllPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   18 Sep 2014 13:39:12  $
//******************************************************************************

#pragma region Includes
//Moved to precompiled header: #include <boost/bind>
#include "stdafx.h"
#include "SVObserver/svobserver.h"
#include "SVTADlgExternalSelectDllPage.h"
#include "SVObserver/SVIPDoc.h"
#include "SVObserver/SVToolAdjustmentDialogSheetClass.h"
#include "Definitions/SVUserMessage.h"
#include "SVMFCControls\SVFileDialog.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVStatusLibrary\GlobalPath.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVStatusLibrary/MessageTextGenerator.h"
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
// SVTADlgExternalSelectDllPage dialog

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

BEGIN_MESSAGE_MAP(SVTADlgExternalSelectDllPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVTADlgExternalSelectDllPage)
	ON_BN_CLICKED(IDC_DELETE_ALL, OnDeleteAll)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_STATUS, OnUpdateStatus)
END_MESSAGE_MAP()

SVTADlgExternalSelectDllPage::SVTADlgExternalSelectDllPage(uint32_t inspectionID, uint32_t toolObjectID, SVToolAdjustmentDialogSheetClass* pSheet)
	: CPropertyPage(IDD)
	, m_InspectionID(inspectionID)
	, m_ToolObjectID(toolObjectID) //attention: SVToolAdjustmentDialogSheetClass::m_TaskObjectID is passed to this value when this constructor is called by SVToolAdjustmentDialogSheetClass!
	, m_pSheet(pSheet)
	, m_externalToolTaskController(inspectionID, toolObjectID)
	, m_valueController{ SvOg::BoundValues{ inspectionID, m_externalToolTaskController.getExternalToolTaskObjectId() } }
{
	//{{AFX_DATA_INIT(SVTADlgExternalSelectDllPage)
	m_currentExternalDllFilepath = _T("");
	m_strStatus = _T("");
	//}}AFX_DATA_INIT
}


void SVTADlgExternalSelectDllPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTADlgExternalSelectDllPage)
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
// SVTADlgExternalSelectDllPage message handlers

BOOL SVTADlgExternalSelectDllPage::OnInitDialog()
{
	//@TODO [Arvid][10.00][20.4.2021] as suggested by Marc, rather than checking this pointer here it would be better 
	//		to use a reference to the sheet	(which should be passed to the constructor of this page by the sheet), 
	//		ideally for all tool select sheets. Since we are currently close to the release of Version 10.10 I am not doing this
	//		right now but leave it for some later time.

	if(nullptr == m_pSheet)
	{
		//this should never happen!

		SvStl::MessageManager mm(SvStl::MsgType::Log | SvStl::MsgType::Display);
		SvDef::StringVector msgList;
		msgList.push_back("No Parent Sheet");
		mm.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
		mm.Process();

		return FALSE;
	}

	// stuff the value before base OnInitDialog

	m_valueController.Init();

	auto strDllPath = getStdStringFromValueController(SvPb::EmbeddedIdEnum::DllFileNameEId);
	// cppcheck-suppress danglingLifetime //m_currentExternalDllFilepath is a CString and will not merely hold a copy of a pointer
	m_currentExternalDllFilepath = strDllPath.c_str();

	CPropertyPage::OnInitDialog();	// create button and list box windows

	m_previousExternalDllDirectorypath = AfxGetApp()->GetProfileString(_T("Settings"), _T("Last External Tool Dll Path"), SvStl::GlobalPath::Inst().GetRunPath().c_str());

	m_ToolTip.Create(this, TTS_NOPREFIX | WS_VISIBLE);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.SetDelayTime(32000, TTDT_AUTOPOP);
	m_ToolTip.SetDelayTime(10, TTDT_INITIAL);
	m_ToolTip.SetDelayTime(250, TTDT_RESHOW);

	InitializeDll(true);
	
	for (auto i = 0; i < ExternalToolTaskController::NUM_TOOL_DEPENDENCIES; ++i)
	{
		auto tempDependency = m_valueController.Get<_variant_t>(SvPb::EmbeddedIdEnum::DllDependencyFileNameEId + i);
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

void SVTADlgExternalSelectDllPage::OnOK()
{
	UpdateData();

	// set dll path
	m_valueController.Set<CString>(SvPb::EmbeddedIdEnum::DllFileNameEId, m_currentExternalDllFilepath);
	
	// Copy DLL Dependents from listbox to Task Class FileNameValueObject List...
	SetDependencies();

	try
	{
		m_externalToolTaskController.initialize();

		m_externalToolTaskController.resetAllObjects(false); //first error will be displayed anyway when containing sheet is closed
	}
	catch (const SvStl::MessageContainer&)
	{
	}

	m_valueController.Commit(SvOg::PostAction::doNothing);

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if (nullptr != l_pIPDoc)
	{
		l_pIPDoc->UpdateAllViews(nullptr);
	}

	CPropertyPage::OnOK();
}

void SVTADlgExternalSelectDllPage::OnDeleteAll()
{
	// Clear List Box
	m_lbDependentList.ResetContent();
	testExternalDll();
}

void SVTADlgExternalSelectDllPage::OnDelete()
{
	int iCurrentPos = m_lbDependentList.GetCurSel();
	if (iCurrentPos > -1 && iCurrentPos < m_lbDependentList.GetCount())
	{
		m_lbDependentList.DeleteString(iCurrentPos);
		testExternalDll();
	}

}

void SVTADlgExternalSelectDllPage::OnAdd()
{
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
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

void SVTADlgExternalSelectDllPage::OnBrowse()
{
	UpdateData();
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
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
		m_valueController.Set<CString>(SvPb::EmbeddedIdEnum::DllFileNameEId, m_currentExternalDllFilepath);
		m_valueController.Commit(SvOg::PostAction::doNothing);

		m_valueController.Init();
		auto runpath{ getStdStringFromValueController(SvPb::EmbeddedIdEnum::DllFileNameEId) };

		m_preserveStatus = false;
		if (runpath != dllPath)
		{
			//Unload current dll  
			m_valueController.Set<CString>(SvPb::EmbeddedIdEnum::DllFileNameEId, CString());
			m_valueController.Commit(SvOg::PostAction::doNothing);

			InitializeDll(false, false);
			m_valueController.Set<CString>(SvPb::EmbeddedIdEnum::DllFileNameEId, m_currentExternalDllFilepath);
				
			bool isUsed =  SvUl::ModuleInfo::isProcessModuleName(GetCurrentProcessId(), DLLname);

			if (isUsed)
			{
				std::string Status = SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_CouldNotCopyDll);
				m_strStatus = Status.c_str();
				m_strStatus += cCRLF;
				m_preserveStatus = true;

				m_currentExternalDllFilepath = _T("");
				UpdateData(FALSE);
				m_valueController.Set<CString>(SvPb::EmbeddedIdEnum::DllFileNameEId, m_currentExternalDllFilepath);
			}
		}
		m_externalToolTaskController.clearData();

		showDllConfigurationPages(false);
		testExternalDll(m_ResetInput == TRUE);
	}
}// end void SVTADlgExternalSelectDllPage::OnBrowse() 

void SVTADlgExternalSelectDllPage::testExternalDll(bool setDefaultValues)
{
	UpdateData();

	SetDependencies();

	// DLL Path
	m_valueController.SetDefault<CString>(SvPb::EmbeddedIdEnum::DllFileNameEId, m_currentExternalDllFilepath);
	// ensure that all data are available for initialize dll
	m_valueController.Commit(SvOg::PostAction::doNothing);
	InitializeDll(false, setDefaultValues);
}

void SVTADlgExternalSelectDllPage::setDefaultValuesForInputs()
{
	
	auto inputDefinitions = m_externalToolTaskController.getInputValuesDefinition();
	int size = inputDefinitions.inputvaluesdefinition_size();
	for (int i = 0; i < size; i++)
	{
		auto& rInputDef = inputDefinitions.inputvaluesdefinition()[i];
		int LinkValueIndex = rInputDef.linkedvalueindex();
		if (rInputDef.type() == SvPb::ExDllInterfaceType::Scalar || rInputDef.type() == SvPb::ExDllInterfaceType::Array)
		{
			_variant_t value;
			SvPb::ConvertProtobufToVariant(rInputDef.defaultvalue(), value);

			m_valueController.SetDefault<_variant_t>(SvPb::EmbeddedIdEnum::ExternalInputEId + LinkValueIndex, value);
			m_valueController.Set<_variant_t>(SvPb::EmbeddedIdEnum::ExternalInputEId + LinkValueIndex, value);
		}
		else if (rInputDef.type() == SvPb::ExDllInterfaceType::TableArray || rInputDef.type() == SvPb::ExDllInterfaceType::TableNames)
		{
			m_valueController.SetDefault<_variant_t>(SvPb::EmbeddedIdEnum::ExternalInputEId + LinkValueIndex, _variant_t());
			m_valueController.Set<_variant_t>(SvPb::EmbeddedIdEnum::ExternalInputEId + LinkValueIndex, _variant_t());
		}
	}
	
}

std::string SVTADlgExternalSelectDllPage::getStdStringFromValueController(SvPb::EmbeddedIdEnum objectId, int index)
{
	std::string Value(m_valueController.Get<CString>(objectId + index));
	if (Value.empty())
	{
		_variant_t temp = m_valueController.Get<_variant_t>(objectId + index);

		Value = SvUl::VariantToString(temp);
	}
	return Value;
}

void SVTADlgExternalSelectDllPage::SetDependencies()
{
	int i(0);
	
	for (auto idxDep = 0; idxDep < ExternalToolTaskController::NUM_TOOL_DEPENDENCIES; ++idxDep)
	{
		//GetDefault() could be called only once if the condition "all dependencies have the same defaut value" holds TRUE always. 
		//Assumption: the existing default value is correct, so use it to "reset" the actual value with the existing default value
		m_valueController.Set<CString>(SvPb::EmbeddedIdEnum::DllDependencyFileNameEId + idxDep, 
		m_valueController.GetDefault<CString>(SvPb::EmbeddedIdEnum::DllDependencyFileNameEId + idxDep));
	}

	// Set all File Paths from listbox
	int ilbSize = m_lbDependentList.GetCount();
	for (i = 0; i < ilbSize; i++)
	{
		CString Temp;
		m_lbDependentList.GetText(i, Temp);
		m_valueController.Set<CString>(SvPb::EmbeddedIdEnum::DllDependencyFileNameEId + i, Temp);
	}

	m_externalToolTaskController.setAllAttributes();	// update dependency attributes
}


void SVTADlgExternalSelectDllPage::showDllConfigurationPages(bool show)
{
	m_pSheet->SendMessage(show ? SV_ADD_PAGES_FOR_TESTED_DLL : SV_REMOVE_PAGES_FOR_TESTED_DLL);
}


void SVTADlgExternalSelectDllPage::InitializeDll(bool jumpToInputPage, bool setDefaultValues)
{
	bool dllWasInitialized = false;

	try
	{
		showDllConfigurationPages(false);

		if (!m_preserveStatus)
		{
			m_strStatus.Empty();
		}
		UpdateData(FALSE);

		SvPb::InitializeExternalToolTaskResponse statusMessagesResponse;

		auto hrInitialize = m_externalToolTaskController.initialize(statusMessagesResponse);

		for (const auto& message : statusMessagesResponse.statusmessages())
		{
			m_strStatus += message.c_str();
			m_strStatus += cCRLF;
		}
		UpdateData(FALSE);

		if (S_OK != hrInitialize)
		{
			SvStl::MessageContainer mc;
			mc.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitExternalTaskFailed, SvStl::SourceFileParams(StdMessageParams), 0, m_ToolObjectID);
			throw mc;
		}

		if (setDefaultValues)
		{
			setDefaultValuesForInputs();
		}

		dllWasInitialized = true;

		auto [ok, firstError] = m_externalToolTaskController.resetAllObjects(true);

		if (!ok)
		{
			jumpToInputPage = false; //an error occurred here - we want to remain on the selection page and see what happened

			std::string error = firstError.c_str();
			error = std::regex_replace(error, std::regex("\\("), cCRLF+std::string("  ("));
			error = std::regex_replace(error, std::regex("\\)\\s?"), std::string(")") + cCRLF);

			m_strStatus += error.c_str();
			m_strStatus += "!";
			m_strStatus += cCRLF;

			SvStl::MessageContainer mc;
			mc.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorInReset, SvStl::SourceFileParams(StdMessageParams), 0, m_ToolObjectID);
			throw mc;
		}

		//if we arrive here, Initialization has been successful

		m_strStatus += _T("DLL tests passed.");
		m_strStatus += cCRLF;
		UpdateData(FALSE);
		m_StatusEdit.SetSel(m_strStatus.GetLength(), m_strStatus.GetLength());

	}
	catch (const SvStl::MessageContainer& e)
	{
		//if we arrive here, Initialization has failed

		// display all sub-errors in box
		m_strStatus += _T("External DLL test failed!");
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
	showDllConfigurationPages(dllWasInitialized);

	if (dllWasInitialized && jumpToInputPage)
	{
		m_pSheet->PostMessage(PSM_SETCURSEL, c_indexOfInputValuePage, 0);
	}
}

// Override PreTranslateMessage() so RelayEvent() can be 
// called to pass a mouse message to the 
// tooltip control for processing.
BOOL SVTADlgExternalSelectDllPage::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}


LRESULT SVTADlgExternalSelectDllPage::OnUpdateStatus(WPARAM, LPARAM)
{
	UpdateData();
	m_StatusEdit.SetSel(m_strStatus.GetLength(), m_strStatus.GetLength());
	return 0;
}


bool SVTADlgExternalSelectDllPage::QueryAllowExit()
{
	return true;
}


SVIPDoc* SVTADlgExternalSelectDllPage::GetIPDoc() const
{
	SVIPDoc* l_pIPDoc = nullptr;

	if (nullptr != m_pSheet)
	{
		l_pIPDoc = m_pSheet->GetIPDoc();
	}

	return l_pIPDoc;
}



BOOL SVTADlgExternalSelectDllPage::OnKillActive()
{
	// Since the ValueController used here share the same data with the one used on another page (see SVTADlgExternalInputSelectPage),
	// we need to avoid unwanted interactions between the two (e.g. during navigation between tabs). 
	// Therefore, ensure a Commit() w/o call for each controller by page leave.
	m_valueController.Commit(SvOg::PostAction::doNothing);

	return __super::OnKillActive();
}
