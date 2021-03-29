//******************************************************************************
//* COPYRIGHT (c) 2012 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PropTreeTestDlg
//* .File Name       : $Workfile:   PropTreeTestDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   03 Oct 2013 14:43:30  $
//******************************************************************************

#include "stdafx.h"
#include "PropTreeTestDlg.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "SVRPropertyTree/SVRPropTreeItemCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPropTreeTestDlg dialog

namespace
{
	const static int ID1 = 1001;
	const static int ID2 = 1002;
	const static int ID3 = 1003;
	const static int PROP_TRG_TYPE = 102;
}

CPropTreeTestDlg::CPropTreeTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CPropTreeTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPropTreeTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPropTreeTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY( PTN_ITEMCHANGED, IDC_RESULT_LIST, OnItemChanged )
END_MESSAGE_MAP()

// CPropTreeTestDlg message handlers

BOOL CPropTreeTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	DWORD dwStyle;
	CRect rc;

	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;
	// Init the control's size to cover the entire client area
    GetDlgItem(IDC_RESULT_LIST)->GetWindowRect( rc );
    ScreenToClient( rc );
	GetDlgItem( IDC_RESULT_LIST )->DestroyWindow();
	m_Tree.Create( dwStyle, rc, this, IDC_RESULT_LIST );
	m_Tree.SetColumn( rc.Width() / 3 );
	m_Tree.ShowInfoText( false );

	AddItemEdit( CString( "Name" ), CString( "Value" ), ID1 );
	AddItemEdit( CString( "Name2" ), CString( "Value2" ), ID2 );
	AddItemCombo( CString( "Name3" ), CString( "Value3" ), ID3 );
	SetupTrigger();

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPropTreeTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPropTreeTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPropTreeTestDlg::AddItemEdit( CString p_Name, CString p_Value, UINT p_ID )
{
    SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	assert( pRoot );
	pRoot->SetCanShrink(false);
    pRoot->SetInfoText(_T(""));
	pRoot->HideItem();
	pRoot->SetHeight(2);

	SVRPropertyItemEdit* pEdit = dynamic_cast< SVRPropertyItemEdit* >( m_Tree.InsertItem( new SVRPropertyItemEdit(), pRoot ) );

	pEdit->SetCtrlID( p_ID );
	pEdit->SetLabelText( p_Name );
	pEdit->SetBold( false );
	pEdit->SetHeight( 16 );
	pEdit->SetItemValue( p_Value );
	pEdit->OnRefresh();

	SVRPropertyItem* pChild = pRoot->GetChild();
	while ( pChild )
	{
		pChild->Expand( TRUE );
		pChild = pChild->GetSibling();
	}
	pRoot->Expand( true );	// needed for redrawing
}

void CPropTreeTestDlg::AddItemCombo( CString p_Name, CString p_Value, UINT p_ID )
{
    SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	assert( pRoot );
	pRoot->SetCanShrink(false);
	pRoot->SetLabelText( CString( "Label Text" ) );
    pRoot->SetInfoText( CString( "Info Text" ) );
	//pRoot->HideItem();
	//pRoot->SetHeight(2);

	SVRPropertyItemCombo* pCombo = dynamic_cast< SVRPropertyItemCombo* >( m_Tree.InsertItem( new SVRPropertyItemCombo(), pRoot ) );

	pCombo->SetCtrlID( /*ID_BASE + (int) iter->first*/ p_ID );
	pCombo->SetLabelText( p_Name );
	pCombo->SetInfoText( p_Value );
	pCombo->CreateComboBox();
	int l_Position = pCombo->AddString( CString( "Happy" ) );
	pCombo->SetItemData( l_Position, 0 );
	l_Position = pCombo->AddString( CString( "Birthday" ) );
	pCombo->SetItemData( l_Position, 1 );

	SVRPropertyItem* pChild = pRoot->GetChild();
	while ( pChild )
	{
		pChild->Expand( TRUE );
		pChild = pChild->GetSibling();
	}
	pRoot->Select( true );
	pRoot->Expand( true );	// needed for redrawing
}

void CPropTreeTestDlg::SetupTrigger()
{
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	if (pRoot)
	{
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("Trigger"));
		pRoot->SetInfoText(_T("Define a Trigger"));

		SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);

		if (pCombo)
		{
			pCombo->SetCtrlID(PROP_TRG_TYPE);
			pCombo->SetLabelText(_T("Trigger Type"));
			pCombo->SetInfoText(_T("This item indicates the type of trigger.")
				_T(" Hardware, Software."));
			pCombo->CreateComboBox();

			int iPos = pCombo->AddString(_T("Hardware"));
			pCombo->SetItemData(iPos, 0);

			iPos = pCombo->AddString(_T("Software"));
			pCombo->SetItemData(iPos, 1);
			
			pCombo->SetItemValue( 0 );
		}
		pRoot->Select( true );
		pRoot->Expand();
	}
}

afx_msg void CPropTreeTestDlg::OnItemChanged( NMHDR* p_NotifyStruct, LRESULT* pResult )
{
	LPNMPROPTREE l_pNMPropTree = reinterpret_cast< LPNMPROPTREE >( p_NotifyStruct );
	*pResult = S_OK;

	if ( l_pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = l_pNMPropTree->pItem;

		UINT l_ID = pItem->GetCtrlID();

		switch( l_ID )
		{
			case PROP_TRG_TYPE:
			{
				// SVRPropertyItemCombo

				long l_Long = 0;
				SVRPropertyItem* l_Item = m_Tree.FindItem( l_ID );
				l_Item->GetItemValue( l_Long );

				_variant_t Value;
				l_Item->GetItemValue( Value );

				break;
			}
			case ID1:
			{
				::MessageBoxA( 0, "test", "ID1", MB_OK );
				break;
			}
			case ID2:
			{
				::MessageBoxA( 0, "test", "ID2", MB_OK );
				break;
			}
			case ID3:
			{
				::MessageBoxA( 0, "test", "ID3", MB_OK );
				break;
			}
		}
	}
}

