//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOSelectItemListDlg
//* .File Name       : $Workfile:   SVOSelectItemListDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 07:01:52  $
//******************************************************************************

#include "stdafx.h"
#include "SVOSelectItemListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVMFCControls
	{
		CSVOSelectItemListDlg::CSVOSelectItemListDlg(CWnd* pParent /*=NULL*/)
			: CDialog(CSVOSelectItemListDlg::IDD, pParent)
		{
			//{{AFX_DATA_INIT(CSVOSelectItemListDlg)
			m_sSelectItemListValue = _T("");
			//}}AFX_DATA_INIT
		}


		void CSVOSelectItemListDlg::DoDataExchange(CDataExchange* pDX)
		{
			CDialog::DoDataExchange(pDX);
			//{{AFX_DATA_MAP(CSVOSelectItemListDlg)
			DDX_Control(pDX, IDC_LST_SELECT_ITEM, m_ctlSelectItemList);
			DDX_LBString(pDX, IDC_LST_SELECT_ITEM, m_sSelectItemListValue);
			//}}AFX_DATA_MAP
		}


		BEGIN_MESSAGE_MAP(CSVOSelectItemListDlg, CDialog)
			//{{AFX_MSG_MAP(CSVOSelectItemListDlg)
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		/////////////////////////////////////////////////////////////////////////////
		// CSVOSelectItemListDlg message handlers

		void CSVOSelectItemListDlg::AddListBoxItem(CString sItem)
		{
			if (m_DisplayList.IsEmpty())
			{
				m_DisplayList.AddHead((CString)sItem);
			}
			else
			{
				m_DisplayList.AddTail((CString)sItem);
			}
			//m_ctlSelectItemList.InsertString(-1,sItem);
		}

		BOOL CSVOSelectItemListDlg::OnInitDialog() 
		{
			CDialog::OnInitDialog();
			SVDisplayList::iterator pos;
			CString sTxt;

			int iNum = static_cast<int>(m_DisplayList.GetCount());
			if (iNum > 0)
			{
				for (int x = 0; x < iNum; x++)
				{
					pos = m_DisplayList.FindIndex(x);
					m_ctlSelectItemList.InsertString(-1,m_DisplayList.GetAt(pos));
				}
			}
			m_ctlSelectItemList.SetCurSel(0);

			SetWindowText(m_sDisplay);

			// TODO: Add extra initialization here

			return TRUE;  // return TRUE unless you set the focus to a control
			// EXCEPTION: OCX Property Pages should return FALSE
		}

		void CSVOSelectItemListDlg::OnOK() 
		{
			// TODO: Add extra validation here
			m_iSelectedItemPosition = m_ctlSelectItemList.GetCurSel();
			CDialog::OnOK();
		}

		void CSVOSelectItemListDlg::SetDisplayName(LPCTSTR sDisplayName)
		{
			m_sDisplay = sDisplayName;
		}
	} //SVMFCControls
} //Seidenader

