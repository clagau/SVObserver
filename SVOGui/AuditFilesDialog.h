//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/10/27,2020/10/27 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file AuditFilesDialog.h
/// Dialog for config external files in audidtrail 
//******************************************************************************

#pragma once
#include "SVUtilityLibrary/AuditFiles.h"
#include "GridCtrlLibrary/GridCtrl.h"
#include "SVMFCControls/DlgItemResizer.h"

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
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnHelp();
		afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
		afx_msg void OnBnClickedButtonAdd();
		afx_msg void OnBnClickedButtonRemove();
		afx_msg void OnBnClickedButtonHelp();
		afx_msg void OnGridBeginEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
		afx_msg void OnGridClick(NMHDR* pNotifyStruct, LRESULT* /*pResult*/);
	private:
		void FileGrid2Data();
		SvMc::DlgItemResizer m_Resizer;				//Object which handles the resizing of all controls
		SvGcl::GridCtrl m_FileGrid;						
		DialogType m_DialogType;
		SvUl::CAuditFiles                 m_AuditFiles;
		

	};
}