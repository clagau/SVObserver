//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSelectExternalDllPage.cpp
//* .File Name       : $Workfile:   SVSelectExternalDllPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   18 Sep 2014 13:39:12  $
//******************************************************************************

#pragma region Includes
//Moved to precompiled header: #include <boost/bind>
#include "stdafx.h"
#include "svobserver.h"
#include "SVInspectionProcess.h"
#include "SVExternalToolSelectDllPage.h"
#include "SVExternalToolInputSelectPage.h"
#include "SVExternalToolResultPage.h"
#include "SVIPDoc.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "Operators/SVExternalToolTask.h"
#include "SVLibrary/ISVCancel.h"
#include "SVMFCControls\SVFileDialog.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVOGui\SVExternalToolImageSelectPage.h"
#include "SVStatusLibrary\GlobalPath.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr char* cCRLF (_T("\r\n"));

enum {WM_UPDATE_STATUS = WM_APP + 100};

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
	ON_BN_CLICKED(ID_UNDO_CHANGES, OnUndoChanges)
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_UPDATE_STATUS, OnUpdateStatus)
END_MESSAGE_MAP()

SVSelectExternalDllPage::SVSelectExternalDllPage(uint32_t inspectionID, uint32_t toolObjectID, SVToolAdjustmentDialogSheetClass* pSheet )
: CPropertyPage(IDD)
, m_InspectionID(inspectionID)
, m_ToolObjectID(toolObjectID) //attention: SVToolAdjustmentDialogSheetClass::m_TaskObjectID is passed to this value when this constructor is called by SVToolAdjustmentDialogSheetClass!
{
	m_pSheet = pSheet;

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectidrequest()->mutable_info();
	pRequest->set_ownerid(m_ToolObjectID);
	pRequest->mutable_infostruct()->set_objecttype(SvPb::SVExternalToolTaskObjectType);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectidresponse())
	{
		m_TaskObjectID = responseCmd.getobjectidresponse().objectid();
	}

	m_pTask = dynamic_cast<SvOp::SVExternalToolTask*>(SVObjectManagerClass::Instance().GetObject(m_TaskObjectID));
	assert( m_pTask );

	m_pCancelData = nullptr;
	if ( m_pTask->CanCancel() )
	{
		m_pTask->GetCancelData( m_pCancelData );
	}

	//{{AFX_DATA_INIT(SVSelectExternalDllPage)
	m_strDLLPath = _T("");
	m_strStatus = _T("");
	//}}AFX_DATA_INIT
}

SVSelectExternalDllPage::~SVSelectExternalDllPage()
{
	if ( m_pCancelData )
	{
		delete m_pCancelData;
	}
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
	DDX_Control(pDX, ID_UNDO_CHANGES, m_btnUndoChanges);
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
	m_pTask->m_Data.m_voDllPath.GetValue( Value );
	// cppcheck-suppress danglingLifetime //m_strDLLPath is a CString and will not merely hold a copy of a pointer
	m_strDLLPath = Value.c_str();

	CPropertyPage::OnInitDialog();	// create button and list box windows

	m_strLastDllPath = AfxGetApp()->GetProfileString(_T("Settings"), _T("Last External Tool Dll Path"), SvStl::GlobalPath::Inst().GetRunPath().c_str());

	m_ToolTip.Create(this, TTS_NOPREFIX | WS_VISIBLE);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.SetDelayTime( 32000, TTDT_AUTOPOP );
	m_ToolTip.SetDelayTime( 10,    TTDT_INITIAL );
	m_ToolTip.SetDelayTime( 250,   TTDT_RESHOW );
	
	InitializeDll();

	int iDependentsSize = static_cast< int >( m_pTask->m_Data.m_aDllDependencies.size() );
	for( int i = 0 ; i < iDependentsSize ; i++)
	{
		std::string Temp;
		m_pTask->m_Data.m_aDllDependencies[i].GetValue( Temp );
		if( !Temp.empty() )
		{
			m_lbDependentList.AddString( Temp.c_str() );
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
	catch ( const SvStl::MessageContainer&)
	{
	}

	CleanUpOldToolInfo();

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
	if ( iCurrentPos > -1 && iCurrentPos < m_lbDependentList.GetCount() )
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

	TCHAR tszPath[MAX_PATH]={0};
	_tcscpy(tszPath, m_strLastDllPath);
	cfd.m_ofn.lpstrInitialDir = tszPath;
	cfd.m_ofn.lpstrTitle = _T("Select File");
	
	if( cfd.DoModal() == IDOK)
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
		if( LB_ERR == m_lbDependentList.FindString( 0, cfd.GetPathName() )  )
		{
			m_lbDependentList.AddString( cfd.GetPathName() );
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

	TCHAR tszPath[MAX_PATH]={0};
	_tcscpy(tszPath, m_strLastDllPath);
	cfd.m_ofn.lpstrInitialDir = tszPath;
	cfd.m_ofn.lpstrTitle = _T("Select File");
	
	if( cfd.DoModal() == IDOK)
	{
		m_strLastDllPath = cfd.GetPathName();
		int iFind = m_strLastDllPath.ReverseFind(_T('\\'));
		if (iFind >= 0)
		{
			m_strLastDllPath = m_strLastDllPath.Left(iFind);
		}
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("Last External Tool Dll Path"), m_strLastDllPath);

		m_strDLLPath = cfd.GetPathName();
		UpdateData(FALSE);

		m_pTask->m_Data.m_voDllPath.SetValue(std::string(m_strDLLPath));

		m_pTask->ClearData();
		m_pTask->SetDefaultValues();

		m_strStatus = _T("Dll needs to be tested.");
		m_strStatus += cCRLF;
		UpdateData(FALSE);

		m_pSheet->RemovePagesForTestedExternalTool();

		testExternalDll();
	}
}// end void SVSelectExternalDllPage::OnBrowse() 

void SVSelectExternalDllPage::testExternalDll() 
{
	UpdateData();

	SetDependencies();

	// DLL Path
	m_pTask->m_Data.m_voDllPath.SetDefaultValue( std::string(m_strDLLPath), true );

	InitializeDll();
}


void SVSelectExternalDllPage::OnUndoChanges()
{
	// reload previous info
	RestoreOriginalData();
}



void SVSelectExternalDllPage::SetDependencies() 
{
	int i( 0 );

	// Clear All File Paths
	int iDepSize = static_cast< int >( m_pTask->m_Data.m_aDllDependencies.size() );
	for ( i = 0 ; i < iDepSize ; i++)
	{
		m_pTask->m_Data.m_aDllDependencies[i].SetDefaultValue( std::string(), true );
	}

	// Set all File Paths from listbox
	int ilbSize = m_lbDependentList.GetCount();
	for ( i = 0 ; i < ilbSize ; i++ )
	{
		CString Temp;
		m_lbDependentList.GetText(i, Temp);
		m_pTask->m_Data.m_aDllDependencies[i].SetDefaultValue( std::string(Temp), true );
		m_pTask->m_Data.m_aDllDependencies[i].SetValue( std::string(Temp));
	}

	m_pTask->SetAllAttributes();	// update dependency attributes
}


void SVSelectExternalDllPage::AddPagesForTestedExternalTool()
{
	m_pSheet->AddPage(new SvOg::SVExternalToolImageSelectPage(m_InspectionID, m_TaskObjectID, m_pTask->m_aInputImageInformationStructs));
	m_pSheet->AddPage(new SVExternalToolInputSelectPage(_T("External Tool Inputs"), m_InspectionID, m_ToolObjectID, m_TaskObjectID));
	m_pSheet->AddPage(new SVExternalToolResultPage(_T("External Tool Results"), m_InspectionID, m_TaskObjectID));

	m_pSheet->PostMessage(PSM_SETCURSEL, c_minimumNumberOfExternalToolPages + 1, 0);
}


void SVSelectExternalDllPage::InitializeDll()
{
	try
	{
		m_pSheet->RemovePagesForTestedExternalTool();

		m_strStatus.Empty();
		UpdateData(FALSE);
		m_pTask->Initialize(boost::bind(&SVSelectExternalDllPage::NotifyProgress, this, _1));

		m_pTask->resetAllObjects();

		m_strStatus += _T("DLL passes the tests.");
		m_strStatus += cCRLF;
		UpdateData(FALSE);
		m_StatusEdit.SetSel( m_strStatus.GetLength(), m_strStatus.GetLength());

		AddPagesForTestedExternalTool();//if we arrive here, Initialization has been successful
	}
	catch ( const SvStl::MessageContainer& e)
	{
		// display all sub-errors in box
		UpdateData(TRUE);
		m_strStatus += _T("DLL did not pass.");
		m_strStatus += cCRLF;

		if ( !e.getMessage().getAdditionalText().empty() )
		{
			m_strStatus += e.getMessage().getAdditionalText().c_str();
			m_strStatus += cCRLF;
		}
		const SvStl::Messages& AdditionalMessages( e.getAdditionalMessages() );
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

void SVSelectExternalDllPage::NotifyProgress( LPCTSTR Message )
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
	m_StatusEdit.SetSel( m_strStatus.GetLength(), m_strStatus.GetLength());
	return 0;
}


bool SVSelectExternalDllPage::QueryAllowExit()
{
	return true;
}


SVIPDoc* SVSelectExternalDllPage::GetIPDoc() const
{
	SVIPDoc* l_pIPDoc = nullptr;

	if( nullptr != m_pSheet )
	{
		l_pIPDoc = m_pSheet->GetIPDoc();
	}

	return l_pIPDoc;
}

HRESULT SVSelectExternalDllPage::RestoreOriginalData()
{
	int i( 0 );

	SvOp::SVExternalToolTaskData* pOriginalData = static_cast<SvOp::SVExternalToolTaskData*> (m_pCancelData);
	// LOAD DLL NAME
	m_pTask->m_Data.m_voDllPath = pOriginalData->m_voDllPath;

	// LOAD DEPENDENCIES

	// Clear All File Paths
	int iDepSize = static_cast< int >( m_pTask->m_Data.m_aDllDependencies.size() );
	for ( i = 0 ; i < iDepSize ; i++)
	{
		m_pTask->m_Data.m_aDllDependencies[i].SetDefaultValue( _T("") , true);
	}

	// Set all File Paths from listbox
	int iSize = static_cast< int >( pOriginalData->m_aDllDependencies.size() );
	for ( i = 0 ; i < iSize ; i++ )
	{
		m_pTask->m_Data.m_aDllDependencies[i] = pOriginalData->m_aDllDependencies[i];
	}

	m_pTask->SetAllAttributes();	// update dependency attributes

	InitializeDll();

	// update display
	std::string Value;
	m_pTask->m_Data.m_voDllPath.GetValue( Value );
	// cppcheck-suppress danglingLifetime //m_strDLLPath is a CString and will not merely hold a copy of a pointer
	m_strDLLPath = Value.c_str();
	int iDependentsSize = static_cast< int >( m_pTask->m_Data.m_aDllDependencies.size() );
	for( i = 0 ; i < iDependentsSize ; i++)
	{
		std::string Temp;
		m_pTask->m_Data.m_aDllDependencies[i].GetValue(Temp);
		if( !Temp.empty() )
		{
			m_lbDependentList.AddString( Temp.c_str() );
		}
	}
	UpdateData(FALSE);

	OnUpdateStatus(0,0);

	return S_OK;
}


HRESULT SVSelectExternalDllPage::CleanUpOldToolInfo()
{
	SVObjectPtrVector list;
	m_pTask->FindInvalidatedObjects( list, m_pCancelData, SvOp::SVExternalToolTask::FindEnum::FIND_IMAGES );
	m_pTask->DisconnectInputsOutputs(list);

	list.clear();
	m_pTask->FindInvalidatedObjects( list, m_pCancelData, SvOp::SVExternalToolTask::FindEnum::FIND_VALUES );
	m_pTask->HideInputsOutputs(list);

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		l_pIPDoc->UpdateAllViews( nullptr );
	}
	return S_OK;
}

