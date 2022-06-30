//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	class SVOSelectItemListDlg : public CDialog
	{
		// Construction
	public:
		void AddListBoxItem(CString sItem);
		SVOSelectItemListDlg(CWnd* pParent = nullptr) : CDialog(IDD, pParent) {};
		virtual ~SVOSelectItemListDlg() = default;

		void SetDisplayName(LPCTSTR sDisplayName);
		int m_iSelectedItemPosition {0};
		// Dialog Data
		//{{AFX_DATA(SVOSelectItemListDlg)
		enum { IDD = IDD_DLG_AA_SELECT_ITEM_DLG };
		CListBox	m_ctlSelectItemList;
		CString	m_sSelectItemListValue;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVOSelectItemListDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(SVOSelectItemListDlg)
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
