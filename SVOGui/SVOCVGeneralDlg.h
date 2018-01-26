//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVGeneralDlg
//* .File Name       : $Workfile:   SVOCVGeneralDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:03:10  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#pragma endregion Includes

namespace SvOg
{
	class SVOCVSheet;

	class SVOCVGeneralDlg : public CPropertyPage
	{
	public:
		SVOCVGeneralDlg(SVOCVSheet& rParent);
		virtual ~SVOCVGeneralDlg();

		HRESULT SetInspectionData();

	// Dialog Data
		//{{AFX_DATA(SVOCVGeneralDlg)
		enum { IDD = IDD_OCV_GENERAL_DIALOG };
		CString m_fontFilename;
		CString m_constraintsFilename;
		CString m_controlsFilename;
		//}}AFX_DATA

		void UpdateOCVParameters();

	protected:
		virtual BOOL OnKillActive() override;
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	protected:
		virtual BOOL OnInitDialog() override;
		afx_msg void OnFontBrowseCmd();
		afx_msg void OnConstraintsBrowseCmd();
		afx_msg void OnControlsBrowseCmd();
		DECLARE_MESSAGE_MAP()

	private:
		SVOCVSheet& m_rParent;
	};
} //namespace SvOg
