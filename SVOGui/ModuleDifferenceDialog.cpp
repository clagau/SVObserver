//*****************************************************************************
/// \copyright (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog for show the different between the old and new Module.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ModuleDifferenceDialog.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
BEGIN_MESSAGE_MAP(ModuleDifferenceDialog, CDialog)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
ModuleDifferenceDialog::ModuleDifferenceDialog(CString currentHistory, CString newHistory, const std::string& rModuleName, CWnd* pParent /*nullptr*/)
	: m_strHistory {currentHistory}
	, m_strHistoryNew {newHistory}
	, m_rModuleName {rModuleName}
	,CDialog(ModuleDifferenceDialog::IDD, pParent)
{

}

ModuleDifferenceDialog::~ModuleDifferenceDialog()
{}
#pragma endregion Constructor

#pragma region Protected Methods
void ModuleDifferenceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_HISTORY_TEXT, m_strHistory);
	DDX_Text(pDX, IDC_HISTORY_TEXT_NEW, m_strHistoryNew);
}

BOOL ModuleDifferenceDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(ID_EDIT_NAME)->SetWindowText(m_rModuleName.c_str());

	static_cast<CEdit*>(GetDlgItem(IDC_HISTORY_TEXT))->SetReadOnly(true);
	static_cast<CEdit*>(GetDlgItem(IDC_HISTORY_TEXT_NEW))->SetReadOnly(true);

	UpdateData(FALSE);
	return TRUE;
}

void ModuleDifferenceDialog::OnYes()
{
	EndDialog(IDYES);
}

void ModuleDifferenceDialog::OnNo()
{
	EndDialog(IDNO);
}

CString createHistoryText(const std::vector<std::pair<time_t, std::string>>& rHistoryList, const std::string& guid)
{
	CString strHistory;
	std::string guidForFirstLine {guid};
	for (const auto& rPair : rHistoryList)
	{
		char mbstr[100];
		std::strftime(mbstr, sizeof(mbstr), "%c", std::localtime(&rPair.first));

		strHistory = (std::string {mbstr} + ": " + rPair.second + guidForFirstLine + "\r\n").c_str() + strHistory;
		guidForFirstLine = "";
	}
	return strHistory;
}

CString createHistoryText(const ::google::protobuf::RepeatedPtrField< ::SvPb::History >& rHistoryList, const std::string& guid)
{
	CString strHistory;
	std::string guidForFirstLine {guid};
	for (const auto& rPair : rHistoryList)
	{
		char mbstr[100];
		time_t time (rPair.time());
		std::strftime(mbstr, sizeof(mbstr), "%c", std::localtime(&time));

		strHistory = (std::string {mbstr} + ": " + rPair.comment() + guidForFirstLine + "\r\n").c_str() + strHistory;
		guidForFirstLine = "";
	}
	return strHistory;
}
} //namespace SvOg