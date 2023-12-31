//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraManagerDlg
//* .File Name       : $Workfile:   SVGigeCameraManagerDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 12:40:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVGigeCameraManagerDlg.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVGigeCameraManagerDlg, CDialog)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
END_MESSAGE_MAP()

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
	DDX_Control(pDX, IDC_LST_CAMERAS, m_ctlCameraList);
	//}}AFX_DATA_MAP
}

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

	//camera list to modify
	m_CamList = SVGigeCameraManager::Instance().GetCameraOrder();
	
	Refresh();
	
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

	SVGigeCameraManager::Instance().Refresh();
	m_ctlCameraList.DeleteAllItems();

	m_CamList = SVGigeCameraManager::Instance().GetCameraOrder();
	
	int Size = std::max<int> (m_ctlCameraList.GetItemCount(), static_cast<int> (m_CamList.size()));

	for( int x = 0 ; x < Size ; x ++)
	{
		if( x < static_cast<int> (m_CamList.size()))
		{
			std::string CameraNumber = std::format("{:d}",x+1);
			m_ctlCameraList.InsertItem( x, CameraNumber.c_str() );

			if( !m_CamList[x].m_IPAddress.empty() )
			{
				m_ctlCameraList.SetItemText( x, 1, m_CamList[x].m_IPAddress.c_str() );
				m_ctlCameraList.SetItemText( x, 2, m_CamList[x].m_SerialNum.c_str() );
				m_ctlCameraList.SetItemText( x, 3, m_CamList[x].m_ModelName.c_str() );
			}
			else
			{
				m_ctlCameraList.SetItemText( x, 1, _T("<no device>") );
			}
			m_ctlCameraList.SetItemData(x, (reinterpret_cast<DWORD_PTR>(&m_CamList[x])));
		}
	}
	::SetCursor( hCursor );
	return S_OK;
}
