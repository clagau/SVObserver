//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolDlg
//* .File Name       : $Workfile:   SVExternalToolDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   18 Sep 2014 13:39:12  $
//******************************************************************************

#include "stdafx.h"
#include "svobserver.h"
#include "SVExternalToolDlg.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVExternalTool.h"
#include "SVExternalToolTask.h"
#include "SVExternalToolDetailsSheet.h"
#include "SVOGui/SVShowDependentsDialog.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVMFCControls\SVFileDialog.h"
#include "SVLoki\Functor.h"
#include "SVStatusLibrary\MessageContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString CRLF (_T("\r\n"));

enum {WM_UPDATE_STATUS = WM_APP + 100};

/////////////////////////////////////////////////////////////////////////////
// SVExternalToolDlg dialog

namespace	// file scope
{
	void YieldPaintMessages(HWND hWnd /* = NULL */)
	{// Let other PAINT messages through
		
		MSG msg;
		
		while (PeekMessage(&msg, hWnd, WM_PAINT, WM_PAINT, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

BEGIN_MESSAGE_MAP(SVExternalToolDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVExternalToolDlg)
	ON_BN_CLICKED(IDC_DETAILS, OnDetails)
	ON_BN_CLICKED(IDC_DELETE_ALL, OnDeleteAll)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(ID_TEST, OnTest)
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_UPDATE_STATUS, OnUpdateStatus)
END_MESSAGE_MAP()

SVExternalToolDlg::SVExternalToolDlg( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* pSheet )
: CPropertyPage(IDD)
, m_InspectionID(rInspectionID)
, m_TaskObjectID(rTaskObjectID) 
{
	m_pSheet = pSheet;
	m_pTool = dynamic_cast<SVExternalTool*> (pSheet->GetTool());
	ASSERT( m_pTool );

	SVObjectTypeInfoStruct info;
	info.ObjectType = SVExternalToolTaskObjectType;

	m_pTask = dynamic_cast<SVExternalToolTask*> ( reinterpret_cast<SVObjectClass*>(::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info)) ));
	ASSERT( m_pTask );

	m_pCancelData = NULL;
	if ( m_pTask->CanCancel() )
	{
		m_pTask->GetCancelData( m_pCancelData );
	}

	m_bToolTipEnabled = false;
	//{{AFX_DATA_INIT(SVExternalToolDlg)
	m_strDLLPath = _T("");
	m_strStatus = _T("");
	//}}AFX_DATA_INIT
}

SVExternalToolDlg::~SVExternalToolDlg()
{
	if ( m_pCancelData )
	{
		delete m_pCancelData;
	}
}


void SVExternalToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVExternalToolDlg)
	DDX_Control(pDX, IDC_DLL_STATUS, m_StatusEdit);
	DDX_Control(pDX, IDC_DELETE_ALL, m_btnDeleteAll);
	DDX_Control(pDX, IDC_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_BROWSE, m_btnBrowse);
	DDX_Control(pDX, IDC_ADD, m_btnAdd);
	DDX_Control(pDX, ID_TEST, m_btnTest);
	DDX_Control(pDX, IDC_DETAILS, m_btnDetails);
	DDX_Control(pDX, IDC_DEPENDENT_LIST, m_lbDependentList);
	DDX_Text(pDX, IDC_DLL_PATH, m_strDLLPath);
	DDX_Text(pDX, IDC_DLL_STATUS, m_strStatus);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// SVExternalToolDlg message handlers

BOOL SVExternalToolDlg::OnInitDialog() 
{
	// stuff the value before base OnInitDialog
	m_pTask->m_Data.m_voDllPath.GetValue(m_strDLLPath);

	CPropertyPage::OnInitDialog();	// create button and list box windows

	m_strLastDllPath = AfxGetApp()->GetProfileString(_T("Settings"), _T("Last External Tool Dll Path"), _T("C:\\Run"));

	m_ToolTip.Create(this, TTS_NOPREFIX | WS_VISIBLE);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.SetDelayTime( 32000, TTDT_AUTOPOP );
	m_ToolTip.SetDelayTime( 10,    TTDT_INITIAL );
	m_ToolTip.SetDelayTime( 250,   TTDT_RESHOW );
	EnableDetailTip( true, _T("Dll needs to be tested before Details can be selected"));

	InitializeDll();

	int iDependentsSize = static_cast< int >( m_pTask->m_Data.m_aDllDependencies.size() );
	for( int i = 0 ; i < iDependentsSize ; i++)
	{
		CString strTmp;
		m_pTask->m_Data.m_aDllDependencies[i].GetValue(strTmp);
		if( !strTmp.IsEmpty() )
		{
			m_lbDependentList.AddString(strTmp);
		}
	}
	
	PostMessage(WM_UPDATE_STATUS, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}

void SVExternalToolDlg::OnOK() 
{
	UpdateData();

	//if ( ShowDependentsDlg() )	// moved to QueryAllowExit
	if ( true )
	{
		// set dll path
		m_pTask->m_Data.m_voDllPath.SetValue(1, m_strDLLPath);


		// Copy DLL Dependents from listbox to Task Class FileNameValueObject List...
		SetDependencies();

		HRESULT hr;
		try
		{
			hr = m_pTask->Initialize();
			if( ::SVSendMessage( m_pTask, SVM_RESET_ALL_OBJECTS, NULL, NULL ) != SVMR_SUCCESS )
			{
				hr = S_FALSE;
			}
		}
		catch ( const SvStl::MessageContainer& e)
		{
			hr = static_cast<HRESULT> (e.getMessage().m_MessageCode);
		}

		CleanUpOldToolInfo();

		CPropertyPage::OnOK();
	}
}

void SVExternalToolDlg::OnDetails() 
{
	// Add new property sheet to adjust Input Images, Input Value Objects..
	SVExternalToolDetailsSheet sheet(m_InspectionID, m_TaskObjectID, m_pTask->m_Data.m_lNumInputImages, _T("External Tool Details"), this, 0);

	SVCancelData* pCancelData = NULL;

	sheet.m_pTool = m_pTool;
	sheet.m_pTask = m_pTask;

	if ( sheet.CanCancel() )
		sheet.GetCancelData(pCancelData);

	sheet.CreatePages();

	if( sheet.DoModal() == IDOK )
	{	// sheet modifies tool; no need to do anything
	}
	else
	{	// Set original data back to tool 
		sheet.SetCancelData(pCancelData);
	}

	if ( pCancelData )
	{
		delete pCancelData;
	}

	
}

void SVExternalToolDlg::OnDeleteAll() 
{
	// Clear List Box
	m_lbDependentList.ResetContent();
}

void SVExternalToolDlg::OnDelete() 
{
	int iCurrentPos = m_lbDependentList.GetCurSel();
	if ( iCurrentPos > -1 && iCurrentPos < m_lbDependentList.GetCount() )
	{
		m_lbDependentList.DeleteString(iCurrentPos);
	}
	
}

void SVExternalToolDlg::OnAdd() 
{
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
		// Extract File Name
		CString strFileName = cfd.GetPathName();
		m_strLastDllPath = strFileName;
		int iFind = m_strLastDllPath.ReverseFind(_T('\\'));
		if (iFind >= 0)
		{
			m_strLastDllPath = m_strLastDllPath.Left(iFind);
		}
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("Last External Tool Dll Path"), m_strLastDllPath);
		//CString strFileName = "c:\\Run\\" + cfd.GetFileName();

		// Check for already existing DLL Paths
		if(m_lbDependentList.FindString( 0, cfd.GetPathName() ) == LB_ERR 
			&& m_lbDependentList.FindString(0, strFileName) == LB_ERR )
		{
			m_lbDependentList.AddString( cfd.GetPathName() );
		}
	}
	
}

void SVExternalToolDlg::OnBrowse() 
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
		CString strFileName = cfd.GetPathName();
		m_strLastDllPath = strFileName;
		int iFind = m_strLastDllPath.ReverseFind(_T('\\'));
		if (iFind >= 0)
		{
			m_strLastDllPath = m_strLastDllPath.Left(iFind);
		}
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("Last External Tool Dll Path"), m_strLastDllPath);

		m_strDLLPath = cfd.GetPathName();
		UpdateData(FALSE);

		m_pTask->m_Data.m_voDllPath.SetValue(1, m_strDLLPath);

		HRESULT hr;
		hr = m_pTask->ClearData();
		hr = m_pTask->SetDefaultValues();

		m_strStatus = _T("Dll needs to be tested.") + CRLF;
		UpdateData(FALSE);

		m_btnDetails.EnableWindow(FALSE);	// force the user to press Test which will call Initialize
		EnableDetailTip( true, _T("Dll needs to be tested before Details can be selected"));
	}
}// end void SVExternalToolDlg::OnBrowse() 

void SVExternalToolDlg::OnTest() 
{

	UpdateData();

	SetDependencies();

	// DLL Path
	m_pTask->m_Data.m_voDllPath.SetDefaultValue( m_strDLLPath, TRUE );

	InitializeDll();

	bool bResult = ShowDependentsDlg();

	if ( !bResult )
	{
		// reload previous info
		RestoreOriginalData();
	}
}


void SVExternalToolDlg::SetDependencies() 
{
	int i( 0 );

	// Clear All File Paths
	int iDepSize = static_cast< int >( m_pTask->m_Data.m_aDllDependencies.size() );
	for ( i = 0 ; i < iDepSize ; i++)
	{
		m_pTask->m_Data.m_aDllDependencies[i].SetDefaultValue( _T("") , TRUE);
		//m_pTask->m_Data.m_aDllDependencies[i].SetValue(m_pTask->m_Data.m_aDllDependencies[i].GetLastSetIndex(), _T("")  );
	}

	// Set all File Paths from listbox
	int ilbSize = m_lbDependentList.GetCount();
	for ( i = 0 ; i < ilbSize ; i++ )
	{
		//SVFileNameValueObjectClass svDependent;
		CString strTmp;
		m_lbDependentList.GetText(i, strTmp);
		m_pTask->m_Data.m_aDllDependencies[i].SetDefaultValue( strTmp , TRUE);
		m_pTask->m_Data.m_aDllDependencies[i].SetValue( 1, strTmp );
	}

	m_pTask->SetAllAttributes();	// update dependency attributes
}

bool SVExternalToolDlg::EnableDetailTip(  bool bEnable, const CString& rstrTip)
{
	bool bLastState = m_bToolTipEnabled;

	if( m_bToolTipEnabled && bEnable )
	{
		m_ToolTip.UpdateTipText(rstrTip, &m_btnDetails);
	}
	else if( !m_bToolTipEnabled && bEnable )
	{
		m_ToolTip.AddTool( &m_btnDetails, rstrTip ); 
	}
	else if( m_bToolTipEnabled && !bEnable )
	{
		m_ToolTip.DelTool( &m_btnDetails );
	}
	else
	{
	}
	m_bToolTipEnabled = bEnable;
	return bLastState;
}

void SVExternalToolDlg::InitializeDll()
{
	try
	{
		m_strStatus.Empty();
		UpdateData(FALSE);
		m_pTask->Initialize( SVDllLoadLibraryCallback(this, &SVExternalToolDlg::NotifyProgress) );
		::SVSendMessage( m_pTask, SVM_RESET_ALL_OBJECTS, NULL, NULL );


		m_strStatus += CString(_T("DLL passes the tests.")) + CRLF;
		UpdateData(FALSE);
		m_StatusEdit.SetSel( m_strStatus.GetLength(), m_strStatus.GetLength());

		EnableDetailTip( false );

		m_btnDetails.EnableWindow();
	}
	catch ( const SvStl::MessageContainer& e)
	{
		// display all sub-errors in box
		UpdateData(TRUE);
		m_strStatus +=  _T("DLL did not pass.") + CRLF;

		if ( !e.getMessage().m_AdditionalText.empty() )
		{
			m_strStatus += e.getMessage().m_AdditionalText.c_str();
			m_strStatus += CRLF;
		}
		const SvStl::Messages& AdditionalMessages( e.getAdditionalMessages() );
		SvStl::Messages::const_iterator iter;
		for (iter = AdditionalMessages.begin(); iter != AdditionalMessages.end(); ++iter)
		{
			m_strStatus += iter->m_AdditionalText.c_str();
			m_strStatus += CRLF;
		}
		UpdateData(FALSE);

		m_btnDetails.EnableWindow(FALSE);
		EnableDetailTip(true, _T("Dll did not pass tests so no details available."));
	}
}

// Override PreTranslateMessage() so RelayEvent() can be 
// called to pass a mouse message to the 
// tooltip control for processing.
BOOL SVExternalToolDlg::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void SVExternalToolDlg::NotifyProgress(const CString& rstrMessage)
{
	UpdateData();

	m_strStatus += rstrMessage + CRLF;

	UpdateData(FALSE);
	YieldPaintMessages(GetSafeHwnd());
}

LRESULT SVExternalToolDlg::OnUpdateStatus(WPARAM, LPARAM)
{
	UpdateData();
	m_StatusEdit.SetSel( m_strStatus.GetLength(), m_strStatus.GetLength());
	return 0;
}


bool SVExternalToolDlg::ShowDependentsDlg()
{
	bool Result( true );

	if( nullptr != m_pTask )
	{
		SVObjectVector list;
		m_pTask->FindInvalidatedObjects( list, m_pCancelData, SVExternalToolTask::FIND_ALL_OBJECTS );

		CString DisplayText;
		CString Name( m_pTask->GetName() );
		DisplayText.Format( IDS_CHANGE_DLL_EXTERNAL_TOOL, Name, Name, Name, Name);

		SVObjectPairVector DependencyList;

		m_pTask->GetDependentsList( list, DependencyList );

		SvOg::SVShowDependentsDialog Dlg( DependencyList, DisplayText );

		Result = ( IDOK == Dlg.DoModal() );
	}
	return Result;
}

bool SVExternalToolDlg::QueryAllowExit()
{
	bool bAllow = ShowDependentsDlg();
	return bAllow;
}

SVIPDoc* SVExternalToolDlg::GetIPDoc() const
{
	SVIPDoc* l_pIPDoc = NULL;

	if( m_pSheet != NULL )
	{
		l_pIPDoc = m_pSheet->GetIPDoc();
	}

	return l_pIPDoc;
}

HRESULT SVExternalToolDlg::RestoreOriginalData()
{
	int i( 0 );

	SVExternalToolTaskData* pOriginalData = static_cast<SVExternalToolTaskData*> (m_pCancelData);
	// LOAD DLL NAME
	m_pTask->m_Data.m_voDllPath = pOriginalData->m_voDllPath;

	// LOAD DEPENDENCIES

	// Clear All File Paths
	int iDepSize = static_cast< int >( m_pTask->m_Data.m_aDllDependencies.size() );
	for ( i = 0 ; i < iDepSize ; i++)
	{
		m_pTask->m_Data.m_aDllDependencies[i].SetDefaultValue( _T("") , TRUE);
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
	m_pTask->m_Data.m_voDllPath.GetValue(m_strDLLPath);
	int iDependentsSize = static_cast< int >( m_pTask->m_Data.m_aDllDependencies.size() );
	for( i = 0 ; i < iDependentsSize ; i++)
	{
		CString strTmp;
		m_pTask->m_Data.m_aDllDependencies[i].GetValue(strTmp);
		if( !strTmp.IsEmpty() )
		{
			m_lbDependentList.AddString(strTmp);
		}
	}
	UpdateData(FALSE);

	OnUpdateStatus(0,0);

	return S_OK;
}


HRESULT SVExternalToolDlg::CleanUpOldToolInfo()
{
	SVObjectVector list;
	m_pTask->FindInvalidatedObjects( list, m_pCancelData, SVExternalToolTask::FIND_IMAGES );
	m_pTask->DisconnectInputsOutputs(list);

	list.clear();
	m_pTask->FindInvalidatedObjects( list, m_pCancelData, SVExternalToolTask::FIND_VALUES );
	m_pTask->HideInputsOutputs(list);

	SVIPDoc* l_pIPDoc = NULL;

	if( m_pTask->GetInspection() != NULL )
	{
		l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( m_pTask->GetInspection()->GetUniqueObjectID() );

		if( l_pIPDoc != NULL )
		{
			l_pIPDoc->UpdateAllViews( NULL );
		}
	}

	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVExternalToolDlg.cpp_v  $
 * 
 *    Rev 1.5   18 Sep 2014 13:39:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  944
 * SCR Title:  Fix Security for File and Folder Selection Dialog for 64 Bit
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVLibrary/SVFileDialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 May 2014 12:13:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 May 2014 11:24:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2014 10:36:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Oct 2013 08:21:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added check for Bitness on External Tool DLL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 14:12:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:29:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   04 Oct 2012 10:45:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   02 Jul 2012 16:52:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Dec 2010 08:05:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Aug 2010 13:48:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to remove or fix uses of CString GetBuffer and ReleaseBuffer methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Dec 2009 10:18:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   03 Sep 2009 10:07:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jun 2007 12:46:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Aug 2005 15:58:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 14:07:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Oct 2004 12:52:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  448
 * SCR Title:  Fix External Tool Change DLL functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified to allow correct functioning when choosing a different DLL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jul 2004 09:01:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed info.SubType to info.ObjectType in constructor
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 12:22:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
