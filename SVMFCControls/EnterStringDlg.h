//*****************************************************************************
/// \copyright (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file StringEnterDlg.h
/// All Rights Reserved 
//*****************************************************************************
/// A Dialog to enter a string in a CEdit-control.
//******************************************************************************
#pragma once

#pragma region Inlcudes
#include "SVOResource/resource.h"
//#include "Definitions/StringTypeDef.h"
#pragma endregion Inlcudes

class EnterStringDlg : public CDialog
{
// Construction
public:
	EnterStringDlg(const std::string& rInitString, std::function<void(const std::string&)> checkFunc, LPCTSTR title = nullptr, CWnd* pParent = nullptr);   // standard constructor
	virtual ~EnterStringDlg();

	// Dialog Data
	//{{AFX_DATA(StringEnterDlg)
	enum { IDD = IDD_STRING_ENTER_DLG };
	//}}AFX_DATA

	std::string getString() const { return std::string{m_String}; };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void OnOK() override;

protected:

	virtual BOOL OnInitDialog() override;
	DECLARE_MESSAGE_MAP()

	//const SvDef::StringVector& m_rAnalyzerResults;
	std::function<void(const std::string&)> m_checkFunc;
	CString	m_String;
	LPCTSTR m_title;
};

