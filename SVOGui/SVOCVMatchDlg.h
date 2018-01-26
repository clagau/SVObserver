//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVMatchDlg
//* .File Name       : $Workfile:   SVOCVMatchDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:03:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#pragma endregion Includes

namespace SvOg
{
	class SVOCVSheet;

	class SVOCVMatchDlg : public CPropertyPage
	{
	public:
		SVOCVMatchDlg(SVOCVSheet& rParent);
		virtual ~SVOCVMatchDlg();

		HRESULT SetInspectionData();

		enum { IDD = IDD_OCV_MATCH_DIALOG };
		CButton	m_buttonBrowse;
		CEdit	m_editMatchString;
		CEdit	m_editMatchFilename;
		CString	m_MatchFilename;
		CString	m_MatchString;
		int		m_useMatchFile;
		int		m_nRadioOperation;

	public:
		virtual BOOL OnKillActive() override;
		protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	public:

		void UpdateOCVParameters();

	protected:
		virtual BOOL OnInitDialog() override;
		afx_msg void OnFontBrowseCmd();
		afx_msg void OnUseMatchString();
		afx_msg void OnUseMatchFile();
		DECLARE_MESSAGE_MAP()

	private:
		SVOCVSheet& m_rParent;
	};
} //namespace SvOg
