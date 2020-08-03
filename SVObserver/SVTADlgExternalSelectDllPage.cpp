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
#include "svobserver.h"
#include "SVInspectionProcess.h"
#include "SVTADlgExternalSelectDllPage.h"
#include "SVIPDoc.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "Definitions/SVUserMessage.h"
#include "Operators/SVExternalToolTask.h"
#include "SVMFCControls\SVFileDialog.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVStatusLibrary\GlobalPath.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVSystemLibrary/FileVersion.h"
#include "SVStatusLibrary/MessageTextGenerator.h"
//#include <Psapi.h>
#pragma endregion Includes
#include <tlhelp32.h> 
#include "SVSystemLibrary/ModuleInfo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr char* cCRLF(_T("\r\n"));

enum { WM_UPDATE_STATUS = WM_APP + 100 };





//@TODO [Arvid][10.00][16.6.2020] this helper function should be placed elsewhere and generalized (and not return a tool pointer)
std::pair<SvOp::SVExternalToolTask*, uint32_t> getExternalToolTaskInfo(uint32_t inspectionID, uint32_t ownerID)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectidrequest()->mutable_info();
	pRequest->set_ownerid(ownerID);
	pRequest->mutable_infostruct()->set_objecttype(SvPb::SVExternalToolTaskObjectType);

	HRESULT hr = SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectidresponse())
	{
		uint32_t taskObjectID = responseCmd.getobjectidresponse().objectid();
		auto pTask = dynamic_cast<SvOp::SVExternalToolTask*>(SVObjectManagerClass::Instance().GetObject(taskObjectID));
		return std::pair<SvOp::SVExternalToolTask*, uint32_t>(pTask, taskObjectID);
	}

	return std::pair<SvOp::SVExternalToolTask*, uint32_t>(nullptr, 0);
}

/////////////////////////////////////////////////////////////////////////////
// SVSelectExternalDllPage dialog

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

BEGIN_MESSAGE_MAP(SVSelectExternalDllPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVSelectExternalDllPage)
	ON_BN_CLICKED(IDC_DELETE_ALL, OnDeleteAll)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_STATUS, OnUpdateStatus)
END_MESSAGE_MAP()

SVSelectExternalDllPage::SVSelectExternalDllPage(uint32_t inspectionID, uint32_t toolObjectID, SVToolAdjustmentDialogSheetClass* pSheet)
	: CPropertyPage(IDD)
	, m_InspectionID(inspectionID)
	, m_ToolObjectID(toolObjectID) //attention: SVToolAdjustmentDialogSheetClass::m_TaskObjectID is passed to this value when this constructor is called by SVToolAdjustmentDialogSheetClass!
	, m_pSheet(pSheet)
{
	auto taskinfo = getExternalToolTaskInfo(inspectionID, toolObjectID);

	m_pTask = taskinfo.first;

	assert(m_pTask);

	//{{AFX_DATA_INIT(SVSelectExternalDllPage)
	m_strDLLPath = _T("");
	m_strStatus = _T("");
	//}}AFX_DATA_INIT
}


void SVSelectExternalDllPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVSelectExternalDllPage)
	DDX_Control(pDX, IDC_DLL_STATUS, m_StatusEdit);
	DDX_Control(pDX, IDC_DELETE_ALL, m_btnDeleteAll);
	DDX_Control(pDX, IDC_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_BROWSE, m_btnBrowse);
	DDX_Control(pDX, IDC_ADD, m_btnAdd);
	DDX_Check(pDX, IDC_RESET_INPUT, m_ResetInput);
	DDX_Control(pDX, IDC_DEPENDENT_LIST, m_lbDependentList);
	DDX_Text(pDX, IDC_DLL_PATH, m_strDLLPath);
	DDX_Text(pDX, IDC_DLL_STATUS, m_strStatus);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// SVSelectExternalDllPage message handlers

BOOL SVSelectExternalDllPage::OnInitDialog()
{
	std::string Value;
	// stuff the value before base OnInitDialog
	m_pTask->m_Data.m_voDllPath.GetValue(Value);
	// cppcheck-suppress danglingLifetime //m_strDLLPath is a CString and will not merely hold a copy of a pointer
	m_strDLLPath = Value.c_str();

	CPropertyPage::OnInitDialog();	// create button and list box windows

	m_strLastDllPath = AfxGetApp()->GetProfileString(_T("Settings"), _T("Last External Tool Dll Path"), SvStl::GlobalPath::Inst().GetRunPath().c_str());

	m_ToolTip.Create(this, TTS_NOPREFIX | WS_VISIBLE);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.SetDelayTime(32000, TTDT_AUTOPOP);
	m_ToolTip.SetDelayTime(10, TTDT_INITIAL);
	m_ToolTip.SetDelayTime(250, TTDT_RESHOW);

	InitializeDll(true);

	int iDependentsSize = static_cast<int>(m_pTask->m_Data.m_aDllDependencies.size());
	for (int i = 0; i < iDependentsSize; i++)
	{
		std::string Temp;
		m_pTask->m_Data.m_aDllDependencies[i].GetValue(Temp);
		if (!Temp.empty())
		{
			m_lbDependentList.AddString(Temp.c_str());
		}
	}

	PostMessage(WM_UPDATE_STATUS, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE

}

void SVSelectExternalDllPage::OnOK()
{
	UpdateData();

	// set dll path
	m_pTask->m_Data.m_voDllPath.SetValue(std::string(m_strDLLPath));


	// Copy DLL Dependents from listbox to Task Class FileNameValueObject List...
	SetDependencies();

	try
	{
		m_pTask->Initialize();
		m_pTask->resetAllObjects();
	}
	catch (const SvStl::MessageContainer&)
	{
	}

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if (nullptr != l_pIPDoc)
	{
		l_pIPDoc->UpdateAllViews(nullptr);
	}

	CPropertyPage::OnOK();
}

void SVSelectExternalDllPage::OnDeleteAll()
{
	// Clear List Box
	m_lbDependentList.ResetContent();
	testExternalDll();
}

void SVSelectExternalDllPage::OnDelete()
{
	int iCurrentPos = m_lbDependentList.GetCurSel();
	if (iCurrentPos > -1 && iCurrentPos < m_lbDependentList.GetCount())
	{
		m_lbDependentList.DeleteString(iCurrentPos);
		testExternalDll();
	}

}

void SVSelectExternalDllPage::OnAdd()
{
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog cfd(true, bFullAccess, _T("dll"), _T(""),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("All Files (*.*)|*.*|Dynamic Link Library(*.dll)|*.dll||"));

	TCHAR tszPath[MAX_PATH] = { 0 };
	_tcscpy(tszPath, m_strLastDllPath);
	cfd.m_ofn.lpstrInitialDir = tszPath;
	cfd.m_ofn.lpstrTitle = _T("Select File");

	if (cfd.DoModal() == IDOK)
	{
		// Extract File Name
		m_strLastDllPath = cfd.GetPathName();
		int iFind = m_strLastDllPath.ReverseFind(_T('\\'));
		if (iFind >= 0)
		{
			m_strLastDllPath = m_strLastDllPath.Left(iFind);
		}
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("Last External Tool Dll Path"), m_strLastDllPath);

		// Check for already existing DLL Paths
		if (LB_ERR == m_lbDependentList.FindString(0, cfd.GetPathName()))
		{
			m_lbDependentList.AddString(cfd.GetPathName());
		}
		testExternalDll();
	}
}

void SVSelectExternalDllPage::OnBrowse()
{
	UpdateData();
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog cfd(true, bFullAccess, _T("dll"), _T(""),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Dynamic Link Library(*.dll)|*.dll|All Files (*.*)|*.*||"));

	TCHAR tszPath[MAX_PATH] = { 0 };
	_tcscpy(tszPath, m_strLastDllPath);
	cfd.m_ofn.lpstrInitialDir = tszPath;
	cfd.m_ofn.lpstrTitle = _T("Select File");

	if (cfd.DoModal() == IDOK)
	{
		m_strLastDllPath = cfd.GetPathName();
		int iFind = m_strLastDllPath.ReverseFind(_T('\\'));
		if (iFind >= 0)
		{
			m_strLastDllPath = m_strLastDllPath.Left(iFind);
		}
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("Last External Tool Dll Path"), m_strLastDllPath);

		m_strDLLPath = cfd.GetPathName();
		std::string DLLname = cfd.GetFileName();
		UpdateData(FALSE);

		
		std::string dllPath(m_strDLLPath.GetString());
		m_pTask->m_Data.m_voDllPath.SetValue(std::string(m_strDLLPath));

		std::string runpath;
		m_pTask->m_Data.m_voDllPath.GetValue(runpath);
		m_preserveStatus = false;
		if (runpath != dllPath)
		{
			bool isVersionEqual = SvSyl::FileVersion::isEqual(runpath.c_str(), dllPath.c_str());
	
			std::vector<std::string> Modulnames;
			if (false == isVersionEqual)
			{
				//check if previous Version is still used 
				//Unload current dll  
				m_pTask->m_Data.m_voDllPath.SetValue(std::string());
				InitializeDll(false, false);
				m_pTask->m_Data.m_voDllPath.SetValue(std::string(m_strDLLPath));
				
				bool isUsed =  SvSyl::ModuleInfo::isProcessModuleName(GetCurrentProcessId(), DLLname);

				if (isUsed)
				{
					std::string Status = SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_CouldNotCopyDll);
					m_strStatus = Status.c_str();
					m_strStatus += cCRLF;
					m_preserveStatus = true;

					m_strDLLPath = _T("");
					UpdateData(FALSE);
					m_pTask->m_Data.m_voDllPath.SetDefaultValue(std::string(m_strDLLPath), true);
				}
			
			}
		}
		m_pTask->ClearData();

		CWnd *pParent = GetParent();
		if (nullptr != pParent)
		{
			pParent->SendMessage(SV_REMOVE_PAGES_FOR_TESTED_DLL);
		}
		testExternalDll(m_ResetInput == TRUE);
	}
}// end void SVSelectExternalDllPage::OnBrowse() 

void SVSelectExternalDllPage::testExternalDll(bool setDefaultValues)
{
	UpdateData();

	SetDependencies();

	// DLL Path
	m_pTask->m_Data.m_voDllPath.SetDefaultValue(std::string(m_strDLLPath), true);

	InitializeDll(false, setDefaultValues);
}

void SVSelectExternalDllPage::SetDependencies()
{
	int i(0);

	// Clear All File Paths
	int iDepSize = static_cast<int>(m_pTask->m_Data.m_aDllDependencies.size());
	for (i = 0; i < iDepSize; i++)
	{
		m_pTask->m_Data.m_aDllDependencies[i].SetDefaultValue(std::string(), true);
	}

	// Set all File Paths from listbox
	int ilbSize = m_lbDependentList.GetCount();
	for (i = 0; i < ilbSize; i++)
	{
		CString Temp;
		m_lbDependentList.GetText(i, Temp);
		m_pTask->m_Data.m_aDllDependencies[i].SetDefaultValue(std::string(Temp), true);
		m_pTask->m_Data.m_aDllDependencies[i].SetValue(std::string(Temp));
	}

	m_pTask->SetAllAttributes();	// update dependency attributes
}

void SVSelectExternalDllPage::InitializeDll(bool jumpToInputPage, bool setDefaultValues)
{

	try
	{
		CWnd *pParent = GetParent();
		if (nullptr == pParent)//this should never happen!
		{
			return;
		}

		pParent->SendMessage(SV_REMOVE_PAGES_FOR_TESTED_DLL);

		if (!m_preserveStatus)
		{
			m_strStatus.Empty();
		}
		UpdateData(FALSE);

		m_pTask->Initialize(boost::bind(&SVSelectExternalDllPage::NotifyProgress, this, _1));



		if (setDefaultValues)
		{
			m_pTask->SetDefaultValues();
		}
		m_pTask->resetAllObjects();

		m_strStatus += _T("DLL passes the tests.");
		m_strStatus += cCRLF;
		UpdateData(FALSE);
		m_StatusEdit.SetSel(m_strStatus.GetLength(), m_strStatus.GetLength());

		//if we arrive here, Initialization has been successful
		pParent->SendMessage(SV_ADD_PAGES_FOR_TESTED_DLL);

		if (jumpToInputPage)
		{
			pParent->PostMessage(PSM_SETCURSEL, c_indexOfInputValuePage, 0);
		}
	}
	catch (const SvStl::MessageContainer& e)
	{
		//if we arrive here, Initialization has failed
		m_pSheet->AddAdditionalPagesForExternalTool(true);
		// display all sub-errors in box
		UpdateData(TRUE);
		m_strStatus += _T("DLL did not pass.");
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
}

// Override PreTranslateMessage() so RelayEvent() can be 
// called to pass a mouse message to the 
// tooltip control for processing.
BOOL SVSelectExternalDllPage::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void SVSelectExternalDllPage::NotifyProgress(LPCTSTR Message)
{
	UpdateData();

	m_strStatus += Message;
	m_strStatus += cCRLF;

	UpdateData(FALSE);
	YieldPaintMessages(GetSafeHwnd());
}

LRESULT SVSelectExternalDllPage::OnUpdateStatus(WPARAM, LPARAM)
{
	UpdateData();
	m_StatusEdit.SetSel(m_strStatus.GetLength(), m_strStatus.GetLength());
	return 0;
}


bool SVSelectExternalDllPage::QueryAllowExit()
{
	return true;
}


SVIPDoc* SVSelectExternalDllPage::GetIPDoc() const
{
	SVIPDoc* l_pIPDoc = nullptr;

	if (nullptr != m_pSheet)
	{
		l_pIPDoc = m_pSheet->GetIPDoc();
	}

	return l_pIPDoc;
}

