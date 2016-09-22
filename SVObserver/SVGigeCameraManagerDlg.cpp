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
#include "SVOMFCLibrary/SVDeviceParams.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVGigeCameraManagerDlg::SVGigeCameraManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SVGigeCameraManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVGigeCameraManagerDlg)
	//}}AFX_DATA_INIT
}

SVGigeCameraManagerDlg::~SVGigeCameraManagerDlg()
{
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
	HCURSOR hCursor = nullptr;
	hCursor = ::LoadCursor(nullptr, IDC_WAIT);
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
	if ( nullptr != l_Pos )
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
	if ( nullptr != l_Pos )
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

