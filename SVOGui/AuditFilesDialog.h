//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/10/27,2020/10/27 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file AuditFilesDialog.h
/// Dialog for config external files in audidtrail 
//******************************************************************************

#pragma once
#include "SVUtilityLibrary/AuditFiles.h"


namespace SvOg
{

	class AuditFilesDialog : public CDialog
	{
		DECLARE_DYNAMIC(AuditFilesDialog)

	public:
		enum columnType { Ename = 0, Esize, Edate, EIgnore, ECalcHash, EHashValue, EColumncount };
		enum DialogType { EDefault, WhiteList };
		AuditFilesDialog(const std::vector< SvUl::AuditFile>& rList, DialogType t, CWnd* pParent = nullptr);   // standard constructor
		virtual ~AuditFilesDialog();

		// Dialog Data
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_DLG_REPORT_FILE };
#endif

	public: 

		std::vector< SvUl::AuditFile>& GetFiles() { return m_AuditFiles.GetFiles(); };
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		void UpdateListctrl(bool rebuild);
		const static LPCSTR HeaderNames[EColumncount];
		const static DWORD  HeaderWith[EColumncount];


		DECLARE_MESSAGE_MAP()
	
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
		afx_msg void OnDefaultCalctrue();
		afx_msg void OnDefaultCalcfalse();
		afx_msg void OnDefaultIgnoretrue();
		afx_msg void OnDefaultIgnorefalse();
		afx_msg void OnDefaultCalchash();
		afx_msg void OnBnClickedButtonCalc();
		afx_msg void OnDefaultAddfile();
		afx_msg void OnDefaultDeletefile();

	private:
		void IterateForSelected(columnType col, bool par); 
		void LstCtrl2Data();

		CMenu m_ContextMenu;
		DialogType m_DialogType;
		SvUl::CAuditFiles                 m_AuditFiles;
		CListCtrl m_ListCtrl;

	};
}