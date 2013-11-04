//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraManagerDlg
//* .File Name       : $Workfile:   SV1394CameraManagerDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:08  $
//******************************************************************************

#include "stdafx.h"
#include "svobserver.h"
#include "SV1394CameraManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SV1394CameraManagerDlg dialog


SV1394CameraManagerDlg::SV1394CameraManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SV1394CameraManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SV1394CameraManagerDlg)
	m_sSelectedCamea = _T("");
	//}}AFX_DATA_INIT
}


void SV1394CameraManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SV1394CameraManagerDlg)
	DDX_Control(pDX, IDC_MOVE_UP, m_CUpButton);
	DDX_Control(pDX, IDC_MOVE_DOWN, m_CDownButton);
	DDX_Control(pDX, IDC_CAMERA_LIST, m_lbCameras);
	DDX_LBString(pDX, IDC_CAMERA_LIST, m_sSelectedCamea);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SV1394CameraManagerDlg, CDialog)
	//{{AFX_MSG_MAP(SV1394CameraManagerDlg)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SV1394CameraManagerDlg message handlers

BOOL SV1394CameraManagerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_lbCameras.SetItemHeight(0, 20);
	
	m_OriginalCamList = TheSV1394CameraManager.GetCameraOrder();

	Refresh();

	m_CDownArrowBmp.LoadBitmap (IDB_DOWNARROW);
	m_CUpArrowBmp.LoadBitmap (IDB_UPARROW);

	m_CUpButton.SetBitmap(m_CUpArrowBmp);
	m_CDownButton.SetBitmap(m_CDownArrowBmp);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void SV1394CameraManagerDlg::OnRefresh() 
{
	Refresh();

	m_lbCameras.Invalidate();
	
}

HRESULT SV1394CameraManagerDlg::Refresh()
{
	HCURSOR hCursor = NULL;
	hCursor = ::LoadCursor(NULL, IDC_WAIT);
	hCursor = ::SetCursor( hCursor );

	TheSV1394CameraManager.Refresh();
	m_CamList = TheSV1394CameraManager.GetCameraOrder();

	long l_Size = std::max< long >( m_lbCameras.GetCount(), m_CamList.GetSize() );

	for( int x = 0 ; x < l_Size ; x ++)
	{
		if( x < m_CamList.GetSize() )
		{
			if( x < m_lbCameras.GetCount() )
			{
				m_lbCameras.SetItemData(x, (DWORD) &m_CamList.ElementAt(x));
			}
			else
			{
				CString sSerial = m_CamList[x].strSerialNum;

				m_lbCameras.AddString(sSerial);
				m_lbCameras.SetItemData(x, (DWORD)&m_CamList[x]);
			}
		}
		else
		{
			m_lbCameras.SetItemData(x, NULL);
		}
	}

	::SetCursor( hCursor );
	return S_OK;


}


void SV1394CameraManagerDlg::OnOK() 
{
	TheSV1394CameraManager.UpdateCameraOrder( m_CamList );

	CDialog::OnOK();
}

void SV1394CameraManagerDlg::OnMoveDown() 
{
	int iCurSel = m_lbCameras.GetCurSel();
	if( iCurSel > -1 && iCurSel < m_lbCameras.GetCount()-1)
	{

		SV1394CameraStruct SVCamTmp;
		SVCamTmp = m_CamList[iCurSel + 1];

		m_CamList[iCurSel + 1] = m_CamList[iCurSel];
		m_CamList[iCurSel + 1].iPosition = iCurSel + 1;

		m_CamList[iCurSel] = SVCamTmp;
		m_CamList[iCurSel].iPosition = iCurSel;

		m_lbCameras.SetCurSel(iCurSel+1);

		for( int x = 0 ; x < m_CamList.GetSize() ; x ++)
			m_lbCameras.SetItemData(x, (DWORD) &m_CamList.ElementAt(x));

		m_lbCameras.Invalidate(); 

		TheSV1394CameraManager.UpdateCameraOrder( m_CamList );
	}	
}

void SV1394CameraManagerDlg::OnMoveUp() 
{
	int iCurSel = m_lbCameras.GetCurSel();
	if( iCurSel > 0 )
	{
		SV1394CameraStruct SVCamTmp;
		SVCamTmp = m_CamList[iCurSel - 1];

		m_CamList[iCurSel - 1] = m_CamList[iCurSel];
		m_CamList[iCurSel - 1].iPosition = iCurSel - 1;

		m_CamList[iCurSel] = SVCamTmp;
		m_CamList[iCurSel].iPosition = iCurSel;

		m_lbCameras.SetCurSel(iCurSel-1);

		for( int x = 0 ; x < m_CamList.GetSize() ; x ++)
			m_lbCameras.SetItemData(x, (DWORD) &m_CamList.ElementAt(x));

		m_lbCameras.Invalidate();

		TheSV1394CameraManager.UpdateCameraOrder( m_CamList );
	}
	
}

void SV1394CameraManagerDlg::OnCancel() 
{
	TheSV1394CameraManager.UpdateCameraOrder( m_OriginalCamList, true );

	CDialog::OnCancel();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SV1394CameraManagerDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:18:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Jan 2013 14:48:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  804
 * SCR Title:  Fix Camera Manager Cancel function
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added ignore handles behavior to cancel function so the serial numbers will be left as they were.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   03 Mar 2010 11:40:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera manager to use a different functionality to manage the camera updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 Feb 2010 15:59:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with not setting point after camera is removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Jul 2007 10:49:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Jan 2004 09:57:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to make the model and vendor name available in the camera manager dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Nov 2003 09:37:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added capability to reorder cameras
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
