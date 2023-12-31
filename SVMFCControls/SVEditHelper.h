//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEditHelper
//* .File Name       : $Workfile:   SVEditHelper.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:23:00  $
//******************************************************************************

#pragma once

namespace SvMc
{
	#define		WM_APP_ED_EDIT_FINISHED			( WM_APP + 04101 )		

	class SVEditHelper : public CEdit
	{
		// Construction
	public:
		SVEditHelper();

		// Attributes
	public:

		// Operations
	public:

		// Implementation
	public:
		virtual ~SVEditHelper();

		// Generated message map functions
	protected:
		//{{AFX_MSG(SVEditHelper)
		afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnKillFocus(CWnd* pNewWnd);
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	};
} //namespace SvMc
