//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraManagerDlg
//* .File Name       : $Workfile:   SVGigeCameraManagerDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 12:40:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVGigeCameraManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVGigeCameraManagerDlg::SVGigeCameraManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVGigeCameraManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVGigeCameraManagerDlg)
	//}}AFX_DATA_INIT
}

void SVGigeCameraManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVGigeCameraManagerDlg)
	DDX_Control(pDX, IDC_MOVE_UP, m_CUpButton);
	DDX_Control(pDX, IDC_MOVE_DOWN, m_CDownButton);
	DDX_Control(pDX, IDC_LST_CAMERAS, m_ctlCameraList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVGigeCameraManagerDlg, CDialog)
	//{{AFX_MSG_MAP(SVGigeCameraManagerDlg)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVGigeCameraManagerDlg message handlers

BOOL SVGigeCameraManagerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctlCameraList.InsertColumn(0, _T("#"), LVCFMT_LEFT, -1, -1);
	m_ctlCameraList.InsertColumn(1, _T("IP Address"), LVCFMT_LEFT, -1, -1);
	m_ctlCameraList.InsertColumn(2, _T("Serial #"), LVCFMT_LEFT, -1, -1);
	m_ctlCameraList.InsertColumn(3, _T("Model"), LVCFMT_LEFT, -1, -1);

	m_ctlCameraList.SetExtendedStyle( m_ctlCameraList.GetExtendedStyle() | LVS_EX_FULLROWSELECT );

	m_ctlCameraList.SetColumnWidth(0, 25);
	m_ctlCameraList.SetColumnWidth(1, 100);
	m_ctlCameraList.SetColumnWidth(2, 105);
	m_ctlCameraList.SetColumnWidth(3, 105);

	//orginal camera list from INI (not modified)
	m_OriginalIniCameraList = TheSVGigeCameraManager.GetOriginalCameraIniOrder();

	//camera list to modify
	m_CamList = TheSVGigeCameraManager.GetCameraOrder();
	
	Refresh();
	
	m_CDownArrowBmp.LoadBitmap(IDB_DOWNARROW);
	m_CUpArrowBmp.LoadBitmap(IDB_UPARROW);

	m_CUpButton.SetBitmap(m_CUpArrowBmp);
	m_CDownButton.SetBitmap(m_CDownArrowBmp);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVGigeCameraManagerDlg::OnRefresh() 
{
	Refresh();

	m_ctlCameraList.Invalidate();
}

HRESULT SVGigeCameraManagerDlg::Refresh()
{
	HCURSOR hCursor = NULL;
	hCursor = ::LoadCursor(NULL, IDC_WAIT);
	hCursor = ::SetCursor( hCursor );

	TheSVGigeCameraManager.Refresh();
	m_ctlCameraList.DeleteAllItems();

	m_CamList = TheSVGigeCameraManager.GetCameraOrder();
	
	int iCnt = m_ctlCameraList.GetItemCount();
	
	long l_Size = std::max< long >( m_ctlCameraList.GetItemCount(), m_CamList.GetSize() );

	for( int x = 0 ; x < l_Size ; x ++)
	{
		if( x < m_CamList.GetSize() )
		{
			CString sCamNum;
			sCamNum.Format("%d",x+1);
			m_ctlCameraList.InsertItem(x,sCamNum);
			CString sSerial = m_CamList[x].strSerialNum;
			
			CString sIPAddress = m_CamList[x].strIPAddress;
			CString sModelName = m_CamList[x].strModelName;

			if ( !sIPAddress.IsEmpty() )
			{
				m_ctlCameraList.SetItemText(x, 1, sIPAddress);
				m_ctlCameraList.SetItemText(x, 2, sSerial);
				m_ctlCameraList.SetItemText(x, 3, sModelName);
			}
			else
			{
				m_ctlCameraList.SetItemText(x, 1, _T("<no device>"));
			}
			m_ctlCameraList.SetItemData(x, (reinterpret_cast<DWORD_PTR>(&m_CamList[x])));
		}
	}
	::SetCursor( hCursor );
	return S_OK;
}

void SVGigeCameraManagerDlg::OnOK() 
{
	TheSVGigeCameraManager.UpdateCameraOrder( m_CamList );
	TheSVGigeCameraManager.UpdateCameraIniList();

	CDialog::OnOK();
}

void SVGigeCameraManagerDlg::OnMoveDown() 
{
	int iCurSel = 0;

	if ( m_ctlCameraList.GetItemCount() <= 1) 
	{
		//can't do anything so return
		return;
	}
	POSITION l_Pos = m_ctlCameraList.GetFirstSelectedItemPosition();
	if ( l_Pos != NULL )
	{
		iCurSel = m_ctlCameraList.GetNextSelectedItem( l_Pos );

		int iItemCount = m_ctlCameraList.GetItemCount();

		if ( iCurSel > -1 && iCurSel < (iItemCount-1) )
		{
			SVGigeCameraStruct svCamTmp;
			svCamTmp = m_CamList[iCurSel+1];
			m_CamList[iCurSel+1] = m_CamList[iCurSel];
			m_CamList[iCurSel+1].iPosition = iCurSel+1;

			m_CamList[iCurSel] = svCamTmp;
			m_CamList[iCurSel].iPosition = iCurSel;

			for( int x = 0 ; x < m_CamList.GetSize() ; x ++)
			{
				m_ctlCameraList.SetItemData(x, reinterpret_cast<DWORD_PTR>(&m_CamList.ElementAt(x)));
			}
			m_ctlCameraList.Invalidate(); 

			UpdateListCtrl();

			TheSVGigeCameraManager.UpdateCameraOrder( m_CamList );
			//Refresh();

			int iSelction = m_ctlCameraList.SetItemState( iCurSel+1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void SVGigeCameraManagerDlg::OnMoveUp() 
{
	int iCurSel = 0;

	if ( m_ctlCameraList.GetItemCount() <= 1) 
	{
		//can't do anything so return
		return;
	}

	POSITION l_Pos = m_ctlCameraList.GetFirstSelectedItemPosition();
	if ( l_Pos != NULL )
	{
		iCurSel = m_ctlCameraList.GetNextSelectedItem( l_Pos );
		if ( iCurSel > 0 )
		{
			SVGigeCameraStruct SVCamTmp;
			SVCamTmp = m_CamList[iCurSel - 1];

			m_CamList[iCurSel - 1] = m_CamList[iCurSel];
			m_CamList[iCurSel - 1].iPosition = iCurSel - 1;

			m_CamList[iCurSel] = SVCamTmp;
			m_CamList[iCurSel].iPosition = iCurSel;

			for( int x = 0 ; x < m_CamList.GetSize() ; x ++)
			{
				m_ctlCameraList.SetItemData(x, reinterpret_cast<DWORD_PTR>(&m_CamList.ElementAt(x)));
			}
			m_ctlCameraList.Invalidate(); 

			UpdateListCtrl();

			TheSVGigeCameraManager.UpdateCameraOrder( m_CamList );
			//Refresh();

			int iSelction = m_ctlCameraList.SetItemState( iCurSel-1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void SVGigeCameraManagerDlg::UpdateListCtrl()
{
	m_ctlCameraList.DeleteAllItems();

	long l_Size = m_CamList.GetSize();

	for (int x = 0; x < l_Size; x++)
	{
		CString sNumber;
		sNumber.Format("%d",x+1);
		m_ctlCameraList.InsertItem(x,sNumber);
		CString sSerial = m_CamList[x].strSerialNum;
		CString sIPAddress = m_CamList[x].strIPAddress;
		CString sModel = m_CamList[x].strModelName;

		if (!sIPAddress.IsEmpty())
		{
			m_ctlCameraList.SetItemText(x, 1, sIPAddress);
			m_ctlCameraList.SetItemText(x, 2, sSerial);
			m_ctlCameraList.SetItemText(x, 3, sModel);
		}
		else
		{
			m_ctlCameraList.SetItemText(x, 1, _T("<no device>"));
		}
		m_ctlCameraList.SetItemData(x, reinterpret_cast<DWORD_PTR>(&m_CamList[x]));
	}
}

void SVGigeCameraManagerDlg::OnCancel() 
{
	TheSVGigeCameraManager.UpdateCameraOrder( m_OriginalIniCameraList, true );

	CDialog::OnCancel();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVGigeCameraManagerDlg.cpp_v  $
 * 
 *    Rev 1.3   15 May 2014 12:40:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SetItemData to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Jul 2013 09:11:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Jun 2013 12:17:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed m_OriginalCamList and replaced it with m_OriginalIniCameraList so the cancel would work correctly and not modifiy the svim.ini.  Added method UpdateListCtrl
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jun 2013 15:19:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 May 2013 10:20:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the dialog to use a List Control instead of a List Box to show the connected cameras.  The cameras will show up in a 4 columns.  The colunms will be camera #, IPAddress, Serial Number and Model.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:42:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Jan 2013 14:48:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  804
 * SCR Title:  Fix Camera Manager Cancel function
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added ignore handles behavior to cancel function so the serial numbers will be left as they were.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Mar 2010 11:55:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera manager to fix issues with changing cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Feb 2010 16:08:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to handle camera refresh issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:47:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
