// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionRunDlg
// * .File Name       : $Workfile:   SVRegressionRunDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.5  $
// * .Check In Date   : $Date:   25 Nov 2014 20:32:04  $
// ******************************************************************************
// SVRegressionRunDlg.cpp : implementation file
//
#pragma region Includes
#include "stdafx.h"
#include "svobserver.h"
#include "SVRegressionRunDlg.h"
#include "SVIPDoc.h"
#include "SVUserMessage.h"
#include "SVInspectionProcess.h"
#include "SVRegressionFileSelectDlg.h"
#include "SVRegressionFileSelectSheet.h"
#include "SVRegressionExitDlg.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma region Constructor
CSVRegressionRunDlg::CSVRegressionRunDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSVRegressionRunDlg::IDD, pParent)
	,m_iconPlay(NULL)
	,m_iconPause(NULL)
	,m_iconStop(NULL)
	,m_iconModeContinue(NULL)
	,m_iconModeRunToEnd(NULL)
	,m_iconFrameUp(NULL)
	,m_iconFrameBack(NULL)
	,m_timeDelayText( _T( " 0.0" ) ) // Lead with a space for all values less than 10.
{
	//{{AFX_DATA_INIT(CSVRegressionRunDlg)
	//}}AFX_DATA_INIT
}

CSVRegressionRunDlg::~CSVRegressionRunDlg()
{
	DestroyIcons();
}
#pragma endregion Constructor

#pragma region MFC Methods
void CSVRegressionRunDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVRegressionRunDlg)
	DDX_Control(pDX, IDC_BTN_EXIT, m_btnExit);
	DDX_Control(pDX, IDC_BTN_SETTINGS, m_btnSettings);
	DDX_Control(pDX, IDC_SPIN_DELAY_TIME, m_sliderDelayTime);
	DDX_Control(pDX, IDC_LIST_CAMERA_IMAGES, m_lstCameraImages);
	DDX_Control(pDX, IDC_BTN_STOP, m_btnStopExit);
	DDX_Control(pDX, IDC_BTN_PLAY, m_btnPlayPause);
	DDX_Control(pDX, IDC_BTN_MODE, m_btnMode);
	DDX_Control(pDX, IDC_BTN_FRAME_UP, m_btnFrameUp);
	DDX_Control(pDX, IDC_BTN_FRAME_BACK, m_btnFrameBack);
	DDX_Control(pDX, IDC_BTN_BEGINNING, m_btnBeginning);
	DDX_Text(pDX, IDC_EDT_DELAY_TIME, m_timeDelayText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSVRegressionRunDlg, CDialog)
	//{{AFX_MSG_MAP(CSVRegressionRunDlg)
	ON_BN_CLICKED(IDC_BTN_BEGINNING, OnBtnBeginning)
	ON_BN_CLICKED(IDC_BTN_FRAME_BACK, OnBtnFrameBack)
	ON_BN_CLICKED(IDC_BTN_FRAME_UP, OnBtnFrameUp)
	ON_BN_CLICKED(IDC_BTN_MODE, OnBtnMode)
	ON_BN_CLICKED(IDC_BTN_PLAY, OnBtnPlay)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_SETTINGS, OnBtnSettings)
	ON_WM_KEYDOWN()
	ON_MESSAGE(WM_REGRESSION_TEST_SET_FILENAMES, SetNextFiles)
	ON_MESSAGE(WM_REGRESSION_TEST_SET_PLAYPAUSE, SetPlayPauseBtn)
	ON_MESSAGE(WM_REGRESSION_TEST_SET_PREVIOUS, SetPreviousFrameBtn)
	ON_MESSAGE(WM_REGRESSION_TEST_CLOSE_REGRESSION, CloseRegressionTest)
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBtnExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVRegressionRunDlg message handlers

BOOL CSVRegressionRunDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	BOOL bRet;
	CWinApp* app = AfxGetApp();

	if ( nullptr != app )
	{
		//Load all BMP's
		m_iconPlay = app->LoadIcon(IDI_ICON_PLAY);
		m_iconPause = app->LoadIcon(IDI_ICON_PAUSE);
		m_iconStop = app->LoadIcon(IDI_ICON_STOP);
		m_iconModeContinue = app->LoadIcon(IDI_ICON_MODE_CONTINUE);
		m_iconModeRunToEnd = app->LoadIcon(IDI_ICON_MODE_RUNTOEND);
		m_iconFrameUp = app->LoadIcon(IDI_ICON_FRAMEUP);
		m_iconFrameBack = app->LoadIcon(IDI_ICON_FRAMEBACK);
		m_iconBeginning = app->LoadIcon(IDI_ICON_BEGINNING);

		m_bFirst = TRUE;
		//Set all BMP's to Buttons

		m_btnStopExit.SetIcon(m_iconStop);
		m_btnPlayPause.SetIcon(m_iconPlay);
		m_btnMode.SetIcon(m_iconModeRunToEnd);
		m_btnFrameUp.SetIcon(m_iconFrameUp);
		m_btnFrameBack.SetIcon(m_iconFrameBack);
		m_btnBeginning.SetIcon(m_iconBeginning);

		//set default
		m_enumPlayPause = Pause;
		m_enumMode = RunToEnd;
		m_btnPlayPause.SetIcon(m_iconPlay);
		m_timeDelayInMS = 0;

		//Add Tool Tips
		m_ctlToolTip = new CToolTipCtrl;
		m_ctlToolTip->Create(this, TTS_NOPREFIX | WS_VISIBLE);
		m_ctlToolTip->Activate(TRUE);
		m_ctlToolTip->SetDelayTime( 32000, TTDT_AUTOPOP );
		m_ctlToolTip->SetDelayTime( 10,    TTDT_INITIAL );
		m_ctlToolTip->SetDelayTime( 250,   TTDT_RESHOW );

		bRet = m_ctlToolTip->AddTool(&m_btnSettings, IDS_REGTEST_SETTING);
		bRet = m_ctlToolTip->AddTool(&m_btnStopExit, IDS_REGTEST_STOP);
		bRet = m_ctlToolTip->AddTool(&m_btnPlayPause, IDS_REGTEST_PLAY);
		bRet = m_ctlToolTip->AddTool(&m_btnMode, IDS_REGTEST_MODE_RUNTOEND);
		bRet = m_ctlToolTip->AddTool(&m_btnFrameUp, IDS_REGTEST_FRAME_UP);
		bRet = m_ctlToolTip->AddTool(&m_btnFrameBack, IDS_REGTEST_FRAME_BACK);
		bRet = m_ctlToolTip->AddTool(&m_btnBeginning, IDS_REGTEST_BEGINNING);

		m_sliderDelayTime.SetRange( 0, 100, TRUE );
		m_sliderDelayTime.SetPageSize( 1 );
		m_sliderDelayTime.SetPos( 0 );

		//put in the window placement
		WINDOWPLACEMENT *lwp;
		UINT nl;

		if(app->GetProfileBinary("RegressionTest", "WP", (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
			delete [] lwp;
		}

		EnableButtons(m_pIPDocParent->m_listRegCameras.GetCount() > 0);

		UpdateData(FALSE);

		m_btnSettings.SetFocus();
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSVRegressionRunDlg::OnOK()
{
}

void CSVRegressionRunDlg::OnCancel()
{
}

BOOL CSVRegressionRunDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message && m_ctlToolTip )
		m_ctlToolTip->RelayEvent(pMsg);
	
	BOOL retValue = FALSE;
	//send hotkey to main window if control is pressed
	if (WM_KEYDOWN == pMsg->message && HIBYTE( ::GetKeyState(VK_CONTROL)) )
	{
		retValue = AfxGetMainWnd()->PreTranslateMessage(pMsg);
	}

	if (!retValue && WM_KEYDOWN == pMsg->message)
	{
		retValue = TRUE;
		switch (pMsg->wParam)
		{
		case 'S': // fall through...
		case 's':
			GetDlgItem(IDC_BTN_SETTINGS)->PostMessage(BM_CLICK);
			break;
		case 'P': // fall through...
		case 'p':
			GetDlgItem(IDC_BTN_PLAY)->PostMessage(BM_CLICK);
			break;
		case VK_HOME: // HOME key
			GetDlgItem(IDC_BTN_STOP)->PostMessage(BM_CLICK);
			break;
		case 'O': // fall through...
		case 'o': // fall through...
		case VK_NEXT: // PAGE DOWN key
			GetDlgItem(IDC_BTN_FRAME_UP)->PostMessage(BM_CLICK);
			break;
		case 'I': // fall through...
		case 'i': // fall through...
		case VK_PRIOR: // PAGE UP key
			GetDlgItem(IDC_BTN_FRAME_BACK)->PostMessage(BM_CLICK);
			break;
		case 'J': // fall through...
		case 'j':
			GetDlgItem(IDC_BTN_BEGINNING)->PostMessage(BM_CLICK);
			break;
		case 'M': // fall through...
		case 'm':
			GetDlgItem(IDC_BTN_MODE)->PostMessage(BM_CLICK);
			break;
		default:
			retValue = FALSE;
			break;
		}
	}

	//if message was not translated, call base class
	if (!retValue)
	{
		retValue = CDialog::PreTranslateMessage(pMsg);
	}
	return retValue;
}

void CSVRegressionRunDlg::OnBtnBeginning()
{
	if ( m_enumPlayPause == Play )
	{
		m_pIPDocParent->SetRegressionTestRunMode(RegModeBackToBeginningPlay);
	}
	else
	{
		m_pIPDocParent->SetRegressionTestRunMode(RegModeBackToBeginningStop);
	}
}

void CSVRegressionRunDlg::OnBtnFrameBack()
{
	m_pIPDocParent->SetRegressionTestRunMode(RegModeSingleStepBack);
}

void CSVRegressionRunDlg::OnBtnFrameUp()
{
	m_pIPDocParent->SetRegressionTestRunMode(RegModeSingleStepForward);
}

void CSVRegressionRunDlg::OnBtnMode()
{
	if ( m_enumMode == RunToEnd )
	{
		//set to continue
		m_enumMode = Continue;
		m_btnMode.SetIcon(m_iconModeContinue);
		m_ctlToolTip->UpdateTipText(IDS_REGTEST_MODE_CONTINUOUS,&m_btnMode);
		m_pIPDocParent->SetRegressionTestPlayMode(Continue);
	}
	else // must be continue
	{
		//set to runtoend;
		m_enumMode = RunToEnd;
		m_btnMode.SetIcon(m_iconModeRunToEnd);
		m_ctlToolTip->UpdateTipText(IDS_REGTEST_MODE_RUNTOEND,&m_btnMode);
		m_pIPDocParent->SetRegressionTestPlayMode(RunToEnd);
	}
}

void CSVRegressionRunDlg::OnBtnPlay()
{
	if ( m_enumPlayPause == Play )
	{
		m_pIPDocParent->SetRegressionTestRunMode(RegModePause);
		//set button to Pause;
		m_enumPlayPause = Pause;
		m_btnPlayPause.SetIcon(m_iconPlay);
		m_ctlToolTip->UpdateTipText(IDS_REGTEST_PLAY,&m_btnPlayPause);
		m_btnFrameBack.EnableWindow(TRUE);
		m_btnFrameUp.EnableWindow(TRUE);
	}
	else // must be paused
	{
		m_pIPDocParent->SetRegressionTestRunMode(RegModePlay);
		//set button to play
		m_enumPlayPause = Play;
		m_btnPlayPause.SetIcon(m_iconPause);
		m_ctlToolTip->UpdateTipText(IDS_REGTEST_PAUSE,&m_btnPlayPause);
		m_btnFrameBack.EnableWindow(FALSE);
		m_btnFrameUp.EnableWindow(FALSE);
	}
}

void CSVRegressionRunDlg::OnBtnStop()
{
	m_enumPlayPause = Pause;
	OnBtnBeginning();
}

void CSVRegressionRunDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData( TRUE ); // get data from dialog

	if( &m_sliderDelayTime == ( CSliderCtrl* ) pScrollBar )
	{
		int pos = m_sliderDelayTime.GetPos();
		setDelayTime( pos );
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSVRegressionRunDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	m_pIPDocParent->SetRegressionTimeoutPeriod(m_timeDelayInMS);
}
void CSVRegressionRunDlg::OnClose()
{
	DestroyIcons();
	CDialog::OnClose();
}

void CSVRegressionRunDlg::OnBtnSettings()
{
	if ( m_enumPlayPause != Pause )
	{ //set to pause...
		OnBtnPlay();
	}
	//set regression to pause 
	m_pIPDocParent->SetRegressionTestRunMode(RegModePause);

	CSVRegressionFileSelectSheet dlgRegFileSelect("Regression Test");

	dlgRegFileSelect.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	SVVirtualCameraPtrSet CameraList;
	HRESULT hrOk = m_pIPDocParent->GetCameras( CameraList );

	dlgRegFileSelect.CreatePages(&m_pIPDocParent->m_listRegCameras,CameraList);
	if ( IDOK == dlgRegFileSelect.DoModal() )
	{
		if ( m_pIPDocParent->m_listRegCameras.GetCount() > 0 )
		{
			EnableButtons(TRUE);
		}
		m_lstCameraImages.ResetContent();
		if ( !m_bFirst )
		{
			m_pIPDocParent->SetRegressionTestRunMode(RegModeResetSettings);
		}
		else
		{
			m_bFirst = FALSE;
		}
	}
	else
	{
		if ( 0 == m_pIPDocParent->m_listRegCameras.GetCount() )
		{
			EnableButtons(FALSE);
		}
	}
}

void CSVRegressionRunDlg::OnBtnExit()
{
	SVRegressionExitDlg* pDlg = new SVRegressionExitDlg();

	if ( nullptr != pDlg )
	{
		pDlg->DoModal();
		delete pDlg;
	}
}

LRESULT  CSVRegressionRunDlg::SetNextFiles(WPARAM wParam, LPARAM lParam)
//(CList<RegressionRunFileStruct*,RegressionRunFileStruct*>*RegressionFileList)
{
	HRESULT hRet = S_OK;

	CList<RegressionRunFileStruct*,RegressionRunFileStruct*> *RegressionFileList =
	(CList<RegressionRunFileStruct*,RegressionRunFileStruct*>*) wParam;

	//clear contents of list control
	//m_lstCameraImages.DeleteAllItems();

	int iCount = static_cast<int>(RegressionFileList->GetCount());

	int iMaxStringSize = 0;
	CString sTmpString;

	bool bIsListUsed = false;
	if ( m_lstCameraImages.GetCount() > 0 )
	{
		bIsListUsed = true;
	}

	for ( int i = 0; i <= iCount-1; i++ )
	{
		POSITION pos = RegressionFileList->FindIndex(i);
		if ( pos )
		{
			int iVal;
			RegressionRunFileStruct *pTmpStruct = RegressionFileList->GetAt(pos);
			CString sTmp = pTmpStruct->csCameraName + " : "+ pTmpStruct->csFileName;

			if ( iMaxStringSize == 0 )
			{
				iMaxStringSize = sTmp.GetLength();
				sTmpString = sTmp;
			}
			else
			{
				if ( iMaxStringSize < sTmp.GetLength() )
				{
					iMaxStringSize = sTmp.GetLength();
					sTmpString = sTmp;
				}
			}

			iVal = m_lstCameraImages.AddString(sTmp);
		}
	}

	//delete what was previously there...
	if ( bIsListUsed )
	{
		for ( int iDel = 0; iDel <= iCount-1; iDel ++ )
		{
			m_lstCameraImages.DeleteString(0);
		}
	}

	CClientDC dc(this);
	CSize Extent;

	Extent = dc.GetTextExtent(sTmpString, static_cast<int>(_tcslen(sTmpString)));

	Extent.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);

	m_lstCameraImages.SetHorizontalExtent(Extent.cx);

	return hRet;
}

LRESULT CSVRegressionRunDlg::SetPlayPauseBtn(WPARAM wParam, LPARAM lParam)
{
	HRESULT hRet = S_OK;

	RegressionRunModeEnum enumVal = m_pIPDocParent->GetRegressionRunMode();

	if ( enumVal == RegModePlay )
	{
		m_pIPDocParent->SetRegressionTestRunMode(RegModePlay);
		//set button to Play;
		m_enumPlayPause = Play;
		m_btnPlayPause.SetIcon(m_iconPause);
		m_ctlToolTip->UpdateTipText(IDS_REGTEST_PAUSE, &m_btnPlayPause);
		m_btnFrameBack.EnableWindow(FALSE);
		m_btnFrameUp.EnableWindow(FALSE);
	}
	else // must be paused
	{
		m_pIPDocParent->SetRegressionTestRunMode(RegModePause);
		//set button to Pause
		m_enumPlayPause = Pause;
		m_btnPlayPause.SetIcon(m_iconPlay);
		m_ctlToolTip->UpdateTipText(IDS_REGTEST_PLAY, &m_btnPlayPause);
		m_btnFrameBack.EnableWindow(TRUE);
		m_btnFrameUp.EnableWindow(TRUE);
	}

	return hRet;
}

LRESULT CSVRegressionRunDlg::SetPreviousFrameBtn(WPARAM wParam, LPARAM lParam)
{
	HRESULT hRet = S_OK;

	bool l_bVal = ( 0 != lParam );

	m_btnFrameBack.EnableWindow(l_bVal);

	return hRet;
}

LRESULT CSVRegressionRunDlg::CloseRegressionTest(WPARAM, LPARAM)
{
	WINDOWPLACEMENT lpwndpl;

	GetWindowPlacement(&lpwndpl);
	AfxGetApp()->WriteProfileBinary("RegressionTest", "WP", (LPBYTE)&lpwndpl, sizeof(lpwndpl));

	delete m_ctlToolTip;

	m_pIPDocParent->SetRegressionTestRunMode(RegModeStopExit);

	return (LRESULT)TRUE;
}
#pragma endregion MFC Methods

#pragma region Public Methods
void CSVRegressionRunDlg::SetIPDocParent(SVIPDoc* pIPDocParent)
{
	m_pIPDocParent = pIPDocParent;
}
#pragma endregion Public Methods

#pragma region Private Methods
BOOL CSVRegressionRunDlg::EnableButtons(BOOL p_bValue)
{
	//if false first time, all btns enable = false, except for 'Settings'
	//if true, files have been selected
	m_btnStopExit.EnableWindow(p_bValue);
	m_btnPlayPause.EnableWindow(p_bValue);
	m_btnMode.EnableWindow(p_bValue);
	m_btnFrameUp.EnableWindow(p_bValue);
	m_btnFrameBack.EnableWindow(p_bValue);
	m_btnBeginning.EnableWindow(p_bValue);

	return TRUE;
}

void CSVRegressionRunDlg::DestroyIcons()
{
	if(m_iconPlay)
	{
		DestroyIcon(m_iconPlay);
		m_iconPlay = nullptr;
	}
	if(m_iconPause)
	{
		DestroyIcon(m_iconPause);
		m_iconPause = nullptr;
	}
	if(m_iconStop)
	{
		DestroyIcon(m_iconStop);
		m_iconStop = nullptr;
	}
	if(m_iconModeContinue)
	{
		DestroyIcon(m_iconModeContinue);
		m_iconModeContinue = nullptr;
	}
	if(m_iconModeRunToEnd)
	{
		DestroyIcon(m_iconModeRunToEnd);
		m_iconModeRunToEnd = nullptr;
	}
	if(m_iconFrameUp)
	{
		DestroyIcon(m_iconFrameUp);
		m_iconFrameUp = nullptr;
	}
	if(m_iconFrameBack)
	{
		DestroyIcon(m_iconFrameBack);
		m_iconFrameBack = nullptr;
	}
	if(m_iconBeginning)
	{
		DestroyIcon(m_iconBeginning);
		m_iconBeginning = nullptr;
	}
}

void CSVRegressionRunDlg::setDelayTime( int position )
{
	m_timeDelayInMS = position*100;

	// check to make sure value does not drop below 0
	if ( m_timeDelayInMS < 0 )
	{
		m_timeDelayInMS = 0;
	}

	// check to make sure value does not go above 10000
	if ( m_timeDelayInMS > 10000 )
	{
		m_timeDelayInMS = 10000;
	}

	m_timeDelayText.Format("%4.1f", m_timeDelayInMS/1000.0);

	//set the data in IPDoc.
	UpdateData(FALSE);

	m_pIPDocParent->SetRegressionTimeoutPeriod(m_timeDelayInMS);
}
#pragma endregion Private Methods
// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRegressionRunDlg.cpp_v  $
 * 
 *    Rev 1.5   25 Nov 2014 20:32:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  930
 * SCR Title:  Improve Regression Test Dialog (SVO-136)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Code review clean up
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Nov 2014 08:14:40   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  930
 * SCR Title:  Improve Regression Test Dialog (SVO-136)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   changes all over the RegressionRunDlg for new design (SVO-136)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Jun 2014 04:06:46   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  907
 * SCR Title:  Add hotkey Ctrl+0 for "Reset Counts All IPs" in the View Menu
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Change the method PreTranslateMessage to send hot keys with pressed Ctrl-key first to main-Wnd.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Feb 2014 09:22:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  890
 * SCR Title:  Fix SVObserver Memory Leaks
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Delete Icons. Add virtual destructor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 08:00:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:42:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   13 Mar 2012 13:55:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  745
 * SCR Title:  Fix contention issue during load process and display
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated OnBtnSettings method to consolidated functions back to IPDoc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   28 Sep 2011 13:13:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   08 Dec 2010 13:35:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   28 Oct 2010 14:11:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   09 Apr 2009 14:58:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  657
 * SCR Title:  Remove Apply button from the Tool Adjust Dialogs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the apply button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   06 Dec 2005 15:29:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  527
 * SCR Title:  Fix problems with Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with the regression test
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Sep 2005 11:47:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with changing settings. if nothing is selected the buttons will be grayed out
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Aug 2005 11:21:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new message handle to Exit the regression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Aug 2005 11:20:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Aug 2005 10:33:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with regression test.  settings button will reselect files and will reset the files back to beginning of the list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jul 2005 06:46:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   When changing settings, the listbox displaying the file names will be cleared.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   26 Jul 2005 15:25:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problem with scrolling
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Jul 2005 14:31:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   the scroll bar will not reset to the left with each file name.  Once it has been moved, it will stay in that position unitl it is moved again
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2005 11:34:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
