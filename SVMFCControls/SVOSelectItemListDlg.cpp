//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOSelectItemListDlg
//* .File Name       : $Workfile:   SVOSelectItemListDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 07:01:52  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOSelectItemListDlg.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvMc
{
	void SVOSelectItemListDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVOSelectItemListDlg)
		DDX_Control(pDX, IDC_LST_SELECT_ITEM, m_ctlSelectItemList);
		DDX_LBString(pDX, IDC_LST_SELECT_ITEM, m_sSelectItemListValue);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(SVOSelectItemListDlg, CDialog)
		//{{AFX_MSG_MAP(SVOSelectItemListDlg)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVOSelectItemListDlg message handlers

	void SVOSelectItemListDlg::AddListBoxItem(CString sItem)
	{
		m_DisplayList.insert(m_DisplayList.end(), sItem);
	}

	BOOL SVOSelectItemListDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();

		for(auto const& rItem : m_DisplayList)
		{
			m_ctlSelectItemList.InsertString(-1, rItem);
		}
		m_ctlSelectItemList.SetCurSel(0);

		SetWindowText(m_sDisplay);

		// TODO: Add extra initialization here

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVOSelectItemListDlg::OnOK() 
	{
		// TODO: Add extra validation here
		m_iSelectedItemPosition = m_ctlSelectItemList.GetCurSel();
		CDialog::OnOK();
	}

	void SVOSelectItemListDlg::SetDisplayName(LPCTSTR sDisplayName)
	{
		m_sDisplay = sDisplayName;
	}
} //namespace SvMc
