//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOSelectItemListDlg
//* .File Name       : $Workfile:   SVOSelectItemListDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:12:28  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <list>
#include "SVOResource\resource.h"
#pragma endregion Includes

namespace SvMc
{
	class CSVOSelectItemListDlg : public CDialog
	{
		// Construction
	public:
		void AddListBoxItem(CString sItem);
		CSVOSelectItemListDlg(CWnd* pParent = nullptr);   // standard constructor
		virtual ~CSVOSelectItemListDlg();

		void SetDisplayName(LPCTSTR sDisplayName);
		int m_iSelectedItemPosition;
		// Dialog Data
		//{{AFX_DATA(CSVOSelectItemListDlg)
		enum { IDD = IDD_DLG_AA_SELECT_ITEM_DLG };
		CListBox	m_ctlSelectItemList;
		CString	m_sSelectItemListValue;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSVOSelectItemListDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CSVOSelectItemListDlg)
		virtual BOOL OnInitDialog() override;
		virtual void OnOK() override;
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		typedef std::list<CString> SVDisplayList;

		SVDisplayList m_DisplayList;

		CString m_sDisplay;
	};
} //namespace SvMc
