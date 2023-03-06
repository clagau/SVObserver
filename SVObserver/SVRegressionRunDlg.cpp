// ******************************************************************************
// * COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVRegressionRunDlg.h"
#include "SVIPDoc.h"
#include "Definitions/SVUserMessage.h"
#include "SVInspectionProcess.h"
#include "SVRegressionFileSelectSheet.h"
#include "SVRegressionExitDlg.h"
#include "SVMFCControls/SVFileDialog.h"
#include "SVOGui/SVFormulaEditorSheet.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVRegressionRunDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_BEGINNING, OnBtnBeginning)
	ON_BN_CLICKED(IDC_BTN_FRAME_BACK, OnBtnFrameBack)
	ON_BN_CLICKED(IDC_BTN_FRAME_UP, OnBtnFrameUp)
	ON_BN_CLICKED(IDC_BTN_MODE, OnBtnMode)
	ON_BN_CLICKED(IDC_BTN_PLAY, OnBtnPlay)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	ON_BN_CLICKED(IDC_REG_VALIDATE_MODE, OnValidateMode)
	ON_BN_CLICKED(IDC_CHECK_PLAY_COND, OnCheckPlayCond)
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
	ON_BN_CLICKED(IDC_BTN_REG_TEST_PAUSE_EQUATION, OnBnClickedFormula)
	ON_BN_CLICKED(IDC_BTN_SELECT_RESULT_FILE, OnBnClickedResultFile)
END_MESSAGE_MAP()

#pragma region Constructor
SVRegressionRunDlg::SVRegressionRunDlg(RegressionTestController& rRegessionController, uint32_t inspectionID, CWnd* pParent /*=nullptr*/)
	: CDialog(SVRegressionRunDlg::IDD, pParent)
	, m_rRegressionController(rRegessionController)
	, m_pFormulaController(rRegessionController.getPlayEquationController())
	, m_timeDelayText( _T( " 0.0" ) ) // Lead with a space for all values less than 10.
	, m_InspectionID(inspectionID)
	, m_bPlayByEquation(rRegessionController.usePlayCondition())
{
	m_sliderDelayTime.SetInvertVerticalArrowKeys(true);
}

SVRegressionRunDlg::~SVRegressionRunDlg()
{
	DestroyIcons();
	m_rRegressionController.setDoRunOnce(false);
}
#pragma endregion Constructor

#pragma region MFC Methods
void SVRegressionRunDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVRegressionRunDlg)
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
	DDX_Check(pDX, IDC_CHECK_PLAY_COND, m_bPlayByEquation);
	DDX_Text(pDX, IDC_EDIT_PAUSE_EQUATION, m_equationString);
	DDX_Check(pDX, IDC_REG_VALIDATE_MODE, m_isValidationMode);
	DDX_Text(pDX, IDC_VALIDATE_RESULT_FILE, m_validateResultFile);
	//}}AFX_DATA_MAP
}

BOOL SVRegressionRunDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
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

		m_ctlToolTip->AddTool(&m_btnSettings, IDS_REGTEST_SETTING);
		m_ctlToolTip->AddTool(&m_btnStopExit, IDS_REGTEST_STOP);
		m_ctlToolTip->AddTool(&m_btnPlayPause, IDS_REGTEST_PLAY);
		m_ctlToolTip->AddTool(&m_btnMode, IDS_REGTEST_MODE_RUNTOEND);
		m_ctlToolTip->AddTool(&m_btnFrameUp, IDS_REGTEST_FRAME_UP);
		m_ctlToolTip->AddTool(&m_btnFrameBack, IDS_REGTEST_FRAME_BACK);
		m_ctlToolTip->AddTool(&m_btnBeginning, IDS_REGTEST_BEGINNING);

		m_sliderDelayTime.SetRange( 0, 100, TRUE );
		m_sliderDelayTime.SetPageSize( 1 );
		m_sliderDelayTime.SetPos( 0 );

		//put in the window placement
		WINDOWPLACEMENT *lwp = nullptr;
		UINT nl;

		if(app->GetProfileBinary("RegressionTest", "WP", (LPBYTE*)&lwp, &nl))
		{
			WINDOWPLACEMENT lpwndpl;
			GetWindowPlacement(&lpwndpl);
			CRect rect(lpwndpl.rcNormalPosition);
			CRect oldRect(lwp->rcNormalPosition);
			//Check if size of the window is the same. If not, do not use the position from registry because it is from an old style
			if (rect.Height() == oldRect.Height() && rect.Width() == oldRect.Width())
			{
				SetWindowPlacement(lwp);
			}
		}
		delete[] lwp;

		EnableButtons(m_rRegressionController.getRegCameras().size() > 0);

		UpdateData(FALSE);

		m_btnSettings.SetFocus();
	}

	setEquationText();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVRegressionRunDlg::OnOK()
{
}

void SVRegressionRunDlg::OnCancel()
{
}

BOOL SVRegressionRunDlg::PreTranslateMessage(MSG* pMsg)
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
			GetDlgItem(IDC_BTN_FRAME_BACK)->PostMessage(BM_CLICK);
			break;
		case 'I': // fall through...
		case 'i': // fall through...
		case VK_PRIOR: // PAGE UP key
			GetDlgItem(IDC_BTN_FRAME_UP)->PostMessage(BM_CLICK);
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

void SVRegressionRunDlg::OnBtnBeginning()
{
	if ( m_enumPlayPause == Play )
	{
		m_rRegressionController.setRunMode(RegModeBackToBeginningPlay);
	}
	else
	{
		m_rRegressionController.setRunMode(RegModeBackToBeginningStop);
	}
}

void SVRegressionRunDlg::OnBtnFrameBack()
{
	m_rRegressionController.setRunMode(RegModeSingleStepBack);
}

void SVRegressionRunDlg::OnBtnFrameUp()
{
	m_rRegressionController.setRunMode(RegModeSingleStepForward);
}

void SVRegressionRunDlg::OnBtnMode()
{
	if ( m_enumMode == RunToEnd )
	{
		//set to continue
		m_enumMode = Continue;
		m_btnMode.SetIcon(m_iconModeContinue);
		m_ctlToolTip->UpdateTipText(IDS_REGTEST_MODE_CONTINUOUS,&m_btnMode);
		m_rRegressionController.setPlayMode(Continue);
	}
	else // must be continue
	{
		//set to runtoend;
		m_enumMode = RunToEnd;
		m_btnMode.SetIcon(m_iconModeRunToEnd);
		m_ctlToolTip->UpdateTipText(IDS_REGTEST_MODE_RUNTOEND,&m_btnMode);
		m_rRegressionController.setPlayMode(RunToEnd);
	}
}

void SVRegressionRunDlg::OnBtnPlay()
{
	if ( m_enumPlayPause == Play )
	{
		m_rRegressionController.setRunMode(RegModePause);
		//set button to Pause;
		m_enumPlayPause = Pause;
		m_btnPlayPause.SetIcon(m_iconPlay);
		m_ctlToolTip->UpdateTipText(IDS_REGTEST_PLAY,&m_btnPlayPause);
		m_btnFrameBack.EnableWindow(TRUE);
		m_btnFrameUp.EnableWindow(TRUE);
	}
	else // must be paused
	{
		m_rRegressionController.setRunMode(RegModePlay);
		//set button to play
		m_enumPlayPause = Play;
		m_btnPlayPause.SetIcon(m_iconPause);
		m_ctlToolTip->UpdateTipText(IDS_REGTEST_PAUSE,&m_btnPlayPause);
		m_btnFrameBack.EnableWindow(FALSE);
		m_btnFrameUp.EnableWindow(FALSE);
	}
}

void SVRegressionRunDlg::OnBtnStop()
{
	m_enumPlayPause = Pause;
	OnBtnBeginning();
}

void SVRegressionRunDlg::OnCheckPlayCond()
{
	UpdateData(true); // get data from dialog
	m_rRegressionController.usePlayCondition(m_bPlayByEquation?true:false);
}

void SVRegressionRunDlg::OnValidateMode()
{
	UpdateData(true); // get data from dialog
	m_rRegressionController.setValidationMode(m_isValidationMode ? true : false);
}

void SVRegressionRunDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData( TRUE ); // get data from dialog

	if( &m_sliderDelayTime == ( CSliderCtrl* ) pScrollBar )
	{
		int pos = m_sliderDelayTime.GetPos();
		setDelayTime( pos );
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void SVRegressionRunDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	m_rRegressionController.setTimeoutPeriod(m_timeDelayInMS);
}
void SVRegressionRunDlg::OnClose()
{
	DestroyIcons();
	CDialog::OnClose();
}

void SVRegressionRunDlg::OnBtnSettings()
{
	if ( m_enumPlayPause != Pause )
	{ //set to pause...
		OnBtnPlay();
	}
	//set regression to pause 
	m_rRegressionController.setRunMode(RegModePause);

	SVRegressionFileSelectSheet dlgRegFileSelect("Regression Test", m_InspectionID);

	dlgRegFileSelect.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	SvIe::SVVirtualCameraPtrVector cameraVector = m_pIPDocParent->GetCameras();

	dlgRegFileSelect.CreatePages(&m_rRegressionController.getRegCameras(), &m_rRegressionController.getRegImages(), cameraVector);
	if ( IDOK == dlgRegFileSelect.DoModal() )
	{
		SortSelectedFiles(m_rRegressionController.getRegCameras());
		SortSelectedFiles(m_rRegressionController.getRegImages());
		if (m_rRegressionController.getRegCameras().size() > 0 )
		{
			EnableButtons(TRUE);
		}
		m_lstCameraImages.ResetContent();
		if (FALSE == m_bFirst)
		{
			m_rRegressionController.setRunMode(RegModeResetSettings);
		}
		else
		{
			m_bFirst = FALSE;
		}
		m_rRegressionController.setDoRunOnce(false);
	}
	else
	{
		if ( 0 == m_rRegressionController.getRegCameras().size() )
		{
			EnableButtons(FALSE);
		}
	}
}

void SVRegressionRunDlg::OnBtnExit()
{
	SVRegressionExitDlg Dlg;

	Dlg.DoModal();
}

LRESULT  SVRegressionRunDlg::SetNextFiles(WPARAM wParam, LPARAM )
{
	HRESULT hRet = S_OK;

	std::vector<RegressionRunFileStruct>* pRegressionFileVec = (std::vector<RegressionRunFileStruct>*) wParam;

	int iCount = static_cast<int>(pRegressionFileVec->size());

	size_t MaxStringSize = 0;
	std::string Text;

	bool bIsListUsed = false;
	if ( m_lstCameraImages.GetCount() > 0 )
	{
		bIsListUsed = true;
	}

	for( const auto& rRegFile : *pRegressionFileVec )
	{
		std::string Name = rRegFile.ObjectName + " : "+ rRegFile.FileName;

		if ( MaxStringSize == 0 )
		{
			MaxStringSize = Name.size();
			Text = Name;
		}
		else
		{
			if ( MaxStringSize < Name.size() )
			{
				MaxStringSize = Name.size();
				Text = Name;
			}
		}

		m_lstCameraImages.AddString( Name.c_str() );
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

	Extent = dc.GetTextExtent(Text.c_str(), static_cast<int> (Text.size()));

	Extent.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);

	m_lstCameraImages.SetHorizontalExtent(Extent.cx);

	return hRet;
}

LRESULT SVRegressionRunDlg::SetPlayPauseBtn(WPARAM , LPARAM )
{
	HRESULT hRet = S_OK;

	RegressionRunModeEnum enumVal = m_rRegressionController.getRunMode();

	if ( enumVal == RegModePlay )
	{
		m_rRegressionController.setRunMode(RegModePlay);
		//set button to Play;
		m_enumPlayPause = Play;
		m_btnPlayPause.SetIcon(m_iconPause);
		m_ctlToolTip->UpdateTipText(IDS_REGTEST_PAUSE, &m_btnPlayPause);
		m_btnFrameBack.EnableWindow(FALSE);
		m_btnFrameUp.EnableWindow(FALSE);
	}
	else // must be paused
	{
		m_rRegressionController.setRunMode(RegModePause);
		//set button to Pause
		m_enumPlayPause = Pause;
		m_btnPlayPause.SetIcon(m_iconPlay);
		m_ctlToolTip->UpdateTipText(IDS_REGTEST_PLAY, &m_btnPlayPause);
		m_btnFrameBack.EnableWindow(TRUE);
		m_btnFrameUp.EnableWindow(TRUE);
	}

	return hRet;
}

LRESULT SVRegressionRunDlg::SetPreviousFrameBtn(WPARAM, LPARAM lParam)
{
	HRESULT hRet = S_OK;

	bool l_bVal = ( 0 != lParam );

	m_btnFrameBack.EnableWindow(l_bVal);

	return hRet;
}

LRESULT SVRegressionRunDlg::CloseRegressionTest(WPARAM, LPARAM)
{
	WINDOWPLACEMENT lpwndpl;

	GetWindowPlacement(&lpwndpl);
	AfxGetApp()->WriteProfileBinary("RegressionTest", "WP", (LPBYTE)&lpwndpl, sizeof(lpwndpl));

	delete m_ctlToolTip;

	m_rRegressionController.setRunMode(RegModeStopExit);

	return (LRESULT)TRUE;
}
#pragma endregion MFC Methods

#pragma region Public Methods
void SVRegressionRunDlg::SetIPDocParent(SVIPDoc* pIPDocParent)
{
	m_pIPDocParent = pIPDocParent;
}
#pragma endregion Public Methods

#pragma region Private Methods
BOOL SVRegressionRunDlg::EnableButtons(BOOL p_bValue)
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

void SVRegressionRunDlg::DestroyIcons()
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

void SVRegressionRunDlg::setDelayTime( int position )
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

	m_rRegressionController.setTimeoutPeriod(m_timeDelayInMS);
}

void SVRegressionRunDlg::OnBnClickedFormula()
{
	BOOL updateState = UpdateData(TRUE);
	if (updateState)
	{
		CString strCaption = _T("Play Condition");
		
		SvOg::SVFormulaEditorSheetClass dlg(m_pFormulaController, strCaption);
		dlg.DoModal();
		setEquationText();
	}
}

void SVRegressionRunDlg::OnBnClickedResultFile()
{
	constexpr LPCTSTR cResultFileFilter = _T("CSV Files (*.csv)|*.csv||");
	SvMc::SVFileDialog dlg(false, true, _T("csv"), nullptr, 0, cResultFileFilter, nullptr);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	m_validateResultFile.Empty();
	if (dlg.DoModal() == IDOK)
	{
		m_validateResultFile = dlg.GetPathName();
	}
	m_rRegressionController.setValidationFile(m_validateResultFile.GetString());
	UpdateData(false);
}

void SVRegressionRunDlg::setEquationText()
{
	// Get text from EquationStruct and place into Editor
	m_equationString = _T("");
	if (nullptr != m_pFormulaController)
	{
		m_equationString = m_pFormulaController->GetEquationText().c_str();
	}

	UpdateData(false);
}

void SVRegressionRunDlg::SortSelectedFiles(std::vector<RegressionTestStruct>& rRegTestList)
{
	//StrCmpLogicalW is the sorting function used by Windows Explorer
	auto FileCompare = [](const std::string& rLhs, const std::string& rRhs) { return ::StrCmpLogicalW(_bstr_t{ rLhs.c_str() }, _bstr_t{ rRhs.c_str() }) < 0; };
	for (auto& rRegTest : rRegTestList)
	{
		if (rRegTest.fileSortRange.size() > 0)
		{
			//This is for different directories so that each directory is sorted separately
			for (const auto& rSortRange : rRegTest.fileSortRange)
			{
				auto startIter = std::find(rRegTest.stdVectorFile.begin(), rRegTest.stdVectorFile.end(), rSortRange.first);
				auto endIter = std::find(rRegTest.stdVectorFile.begin(), rRegTest.stdVectorFile.end(), rSortRange.second);
				++endIter;
				std::sort(startIter, endIter, FileCompare);
			}
		}
		else
		{
			std::sort(rRegTest.stdVectorFile.begin(), rRegTest.stdVectorFile.end(), FileCompare);
		}
		
		switch (rRegTest.iFileMethod)
		{
			case RegressionFileEnum::RegSubDirectories:
			case RegressionFileEnum::RegMultDirectories:
				rRegTest.stdIteratorStart = rRegTest.stdVectorFile.begin();
				break;
			default:
				rRegTest.stdIteratorStart = std::find(rRegTest.stdVectorFile.begin(), rRegTest.stdVectorFile.end(), rRegTest.firstFilepath);
		}
		rRegTest.stdIteratorCurrent = rRegTest.stdIteratorStart;
	}
}
#pragma endregion Private Methods
