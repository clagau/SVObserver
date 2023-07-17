//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/10/27,2020/10/27 by Körber Pharma Inspection GmbH. All Rights Reserved 
/// \file AuditFilesDialog.h
/// Dialog for config external files in audidtrail 
//******************************************************************************

#pragma once
#include "SVUtilityLibrary/AuditFiles.h"
#include "GridCtrlLibrary/GridCtrl.h"

namespace SvOg
{

	class AuditFilesDialog : public CDialog
	{
	public:
		enum columnType { Ename = 0, Esize, Etimestamp, EIgnore, EHash, ETime,  EHashValue, EColumncount };
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
		void setResizeControls();
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		const static LPCSTR HeaderNames[EColumncount];
		const static DWORD  HeaderWith[EColumncount];
		void InitFileGrid();
		void UpdateFileGrid();

		DECLARE_MESSAGE_MAP()
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnHelp();
		afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
		afx_msg void OnBnClickedButtonAdd();
		afx_msg void OnBnClickedButtonRemove();
		afx_msg void OnBnClickedButtonHelp();
		afx_msg void OnGridBeginEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
		afx_msg void OnGridClick(NMHDR* pNotifyStruct, LRESULT* /*pResult*/);
		afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* pMinMaxInfo);
	private:
		void FileGrid2Data();
		SvGcl::GridCtrl m_FileGrid;
		DialogType m_DialogType;
		SvUl::AuditFiles m_AuditFiles;
		CSize m_minSize {0,0};
	};
}