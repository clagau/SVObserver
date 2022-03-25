//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProgressDialog
//* .File Name       : $Workfile:   SVProgressDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:37:36  $
//******************************************************************************
#pragma region Inlcudes
#include "stdafx.h"
#include "SVIProgress.h"
#include "SVProgressDialog.h"
#include "Definitions/SVUserMessage.h"
#pragma endregion Inlcudes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVProgressDialog::SVProgressDialog(LPCTSTR title, CWnd* pParent /*=nullptr*/)
: CDialog(SVProgressDialog::IDD, pParent)
, m_Title(title)
, m_TotalSize(0)
, m_AmountProcessed(0)
, m_pTask(nullptr)
{
	//{{AFX_DATA_INIT(SVProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVProgressDialog::~SVProgressDialog()
{
}

void SVProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVProgressDialog)
	DDX_Control(pDX, IDC_STATIC1, m_staticTextCtrl);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVProgressDialog, CDialog)
	//{{AFX_MSG_MAP(SVProgressDialog)
	//}}AFX_MSG_MAP
	ON_MESSAGE( SV_UPDATE_PROGRESS, OnUpdateProgress )
	ON_MESSAGE( SV_UPDATE_PROGRESS_TEXT, OnUpdateText )
	ON_MESSAGE( SV_END_PROGRESS_DIALOG, OnEndProgressDialog )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVProgressDialog message handlers

LRESULT SVProgressDialog::OnUpdateProgress( WPARAM wParam, LPARAM lParam )
{
	int cnt = static_cast<int>(wParam);
	int total = static_cast<int>(lParam);

	// to handle initial range setup
	if (total > m_TotalSize)
	{
		m_TotalSize = total;
	}
	// To handle recursive routines
	if (total && total != m_TotalSize)
	{
		m_AmountProcessed = (m_TotalSize - total) + cnt;
	}
	else
	{
		m_AmountProcessed = cnt;
	}

	if (m_AmountProcessed && m_TotalSize)
	{
		double percent = (static_cast<double>(m_AmountProcessed) / static_cast<double>(m_TotalSize)) * 100.0;

		m_progressCtrl.SetPos(static_cast<int>(percent));
	}
	return true;
}

LRESULT SVProgressDialog::OnUpdateText(WPARAM , LPARAM lParam)
{
	m_Text = reinterpret_cast<LPCTSTR>(lParam);
	m_staticTextCtrl.SetWindowText(m_Text);

	return true;
}

LRESULT SVProgressDialog::OnEndProgressDialog(WPARAM, LPARAM )
{
	m_progressCtrl.SetPos(100);
	EndDialog(IDOK);

	return true;
}

BOOL SVProgressDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (!m_Title.IsEmpty())
	{
		SetWindowText(m_Title);
	}
	if (!m_Text.IsEmpty())
	{
		m_staticTextCtrl.SetWindowText(m_Text);
	}
	m_progressCtrl.SetStep(1);
	m_progressCtrl.SetPos(0);

	if (m_pTask)
	{
		m_pTask->Start();
	}
	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVProgressDialog::OnOK()
{
}

void SVProgressDialog::OnCancel()
{
}

