//*****************************************************************************
/// \copyright (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file StringEnterDlg.cpp
/// All Rights Reserved 
//*****************************************************************************
/// A Dialog to enter a string in a CEdit-control.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "EnterStringDlg.h"
#include "SVOResource/resource.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

EnterStringDlg::EnterStringDlg(const std::string& rInitString, std::function<void(const std::string&)> checkFunc, LPCTSTR title, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD, pParent)
	, m_checkFunc{checkFunc}
	, m_String {rInitString.c_str()}
	, m_title {title}
{
}

EnterStringDlg::~EnterStringDlg()
{
}

void EnterStringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StringEnterDlg)
	DDX_Text(pDX, IDC_EDIT1, m_String);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EnterStringDlg, CDialog)
	//{{AFX_MSG_MAP(StringEnterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StringEnterDlg message handlers

BOOL EnterStringDlg::OnInitDialog()
{
	constexpr int cMaxCharacters = 100;

	CDialog::OnInitDialog();

	SetWindowText(m_title);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void EnterStringDlg::OnOK()
{
	UpdateData(TRUE);

	try
	{
		if (m_checkFunc)
		{
			m_checkFunc(std::string {m_String});
		}
		CDialog::OnOK();
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Display);
		Exception.setMessage(rExp.getMessage());
	}
}