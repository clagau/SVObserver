//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzerResultDlg
//* .File Name       : $Workfile:   SVAnalyzerResultDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:32:42  $
//******************************************************************************
#pragma once

#pragma region Inlcudes
#include "Definitions/StringTypeDef.h"
#pragma endregion Inlcudes

class SVAnalyzerResultDlg : public CDialog
{
// Construction
public:
	SVAnalyzerResultDlg(const SvDef::StringVector& rAnalyzerResults, UINT IDD, CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVAnalyzerResultDlg();

	CListBox	m_ResultList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

protected:

	virtual BOOL OnInitDialog() override;
	DECLARE_MESSAGE_MAP()

	const SvDef::StringVector& m_rAnalyzerResults;
};

