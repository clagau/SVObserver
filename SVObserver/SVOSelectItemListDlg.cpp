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

/////////////////////////////////////////////////////////////////////////////
// CSVOSelectItemListDlg dialog


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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOSelectItemListDlg.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 07:01:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:12:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Nov 2010 16:25:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:27:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
