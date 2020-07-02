//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOConfigNameEdit
//* .File Name       : $Workfile:   SVOConfigNameEdit.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:49:44  $
//******************************************************************************
#pragma once

namespace SvMc
{
	class CSVOConfigNameEdit : public CEdit
	{
		// Construction
	public:
		explicit CSVOConfigNameEdit(LPCTSTR const sExcludedChars);

		// Attributes
	public:

		// Operations
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSVOConfigNameEdit)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~CSVOConfigNameEdit();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CSVOConfigNameEdit)
		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
		//}}AFX_MSG

	private:
		CString m_sExcludedChars;

		DECLARE_MESSAGE_MAP()
	};
} //namespace SvMc
