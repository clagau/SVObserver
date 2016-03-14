//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : HLB
//* .File Name       : $Workfile:   Hlb.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 Aug 2014 15:21:14  $
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVMFCControls
	{
		class SVHorizListBox : public CListBox
		{
		public:
			SVHorizListBox();
			virtual ~SVHorizListBox();

		protected:
			int CalcHorizExtent(CDC* pDC, LPCTSTR text) const;
			void ResetHorizExtent();
			void CalcNewHorizExtent(LPCTSTR text);

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVHorizListBox)
			//}}AFX_VIRTUAL

			// Generated message map functions
		protected:
			//{{AFX_MSG(SVHorizListBox)
			//}}AFX_MSG
			afx_msg LRESULT OnAddString(WPARAM wParam, LPARAM lParam);
			afx_msg LRESULT OnInsertString(WPARAM wParam, LPARAM lParam);
			afx_msg LRESULT OnDeleteString(WPARAM wParam, LPARAM lParam);
			afx_msg LRESULT OnSetTabStops(WPARAM wParam, LPARAM lParam);
			afx_msg LRESULT OnResetContent(WPARAM wParam, LPARAM lParam);
			DECLARE_MESSAGE_MAP()
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

