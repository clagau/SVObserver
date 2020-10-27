//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/10/27,2020/10/27 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file AudidFilesDialog.h
/// Dialog for config external files in audidtrail 
//******************************************************************************

#pragma once
#include "SVUtilityLibrary/AudidFiles.h"
#include <afxdialogex.h>

namespace SvOg
{

	class AudidFilesDialog : public CDialogEx
	{
		DECLARE_DYNAMIC(AudidFilesDialog)

	public:
		enum columnType { Ename = 0, Esize, Edate, EIgnore, ECalcHash, EHashValue, EColumncount };
		enum DialogType { EDefault, WhiteList };
		AudidFilesDialog(const std::vector< SvUl::AudidFile>& rList, DialogType t, CWnd* pParent = nullptr);   // standard constructor
		virtual ~AudidFilesDialog();

		// Dialog Data
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_DLG_REPORT_FILE };
#endif

	public: 

		std::vector< SvUl::AudidFile>& GetFiles() { return m_AudidFiles.GetFiles(); };
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
		SvUl::CAudidFiles                 m_AudidFiles;
		CListCtrl m_ListCtrl;

	};
}