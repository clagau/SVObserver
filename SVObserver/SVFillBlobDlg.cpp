// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVFillBlobDlg.cpp
// * .File Name       : $Workfile:   SVFillBlobDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   12 Nov 2014 07:05:36  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "svobserver.h"
#include "SVFillBlobDlg.h"
#include "SVOCore/SVTaskObject.h"
#include "SVTool.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVFillBlobDlg::SVFillBlobDlg(SVToolClass* pTool, CWnd* pParent /*=nullptr*/)
: CDialog(SVFillBlobDlg::IDD, pParent)
, m_pTool(pTool)
{
	//{{AFX_DATA_INIT(SVFillBlobDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVFillBlobDlg::~SVFillBlobDlg()
{
}

HRESULT SVFillBlobDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	bool l_bRequestAdded = false;

	int iCurSel = m_ctlBlobColor.GetCurSel();
	if( iCurSel >= 0 )
	{
		long lValue = ( long ) m_ctlBlobColor.GetItemData( iCurSel );

		if( S_OK == AddInputRequest( m_pvoBlobFillColor, lValue ) )
		{
			l_bRequestAdded = true;
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	iCurSel = m_ctlBlobType.GetCurSel();
	if( iCurSel >= 0 )
	{
		long lValue = ( long ) m_ctlBlobType.GetItemData( iCurSel );

		if( S_OK == AddInputRequest( m_pvoBlobType, lValue ) )
		{
			l_bRequestAdded = true;
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	if( S_OK == l_hrOk && l_bRequestAdded )
	{
		l_hrOk = AddInputRequestMarker();

		if( S_OK == l_hrOk )
		{
			l_hrOk = RunOnce( m_pTool->GetUniqueObjectID() );
		}
	}

	return l_hrOk;
}

void SVFillBlobDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVFillBlobDlg)
	DDX_Control(pDX, IDC_COMBO_BLOB_TYPE, m_ctlBlobType);
	DDX_Control(pDX, IDC_COMBO_BLOB_COLOR, m_ctlBlobColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVFillBlobDlg, CDialog)
	//{{AFX_MSG_MAP(SVFillBlobDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_BLOB_COLOR, OnSelchangeComboBlobColor)
	ON_CBN_SELCHANGE(IDC_COMBO_BLOB_TYPE, OnSelchangeComboBlobType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVFillBlobDlg message handlers

BOOL SVFillBlobDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetTaskObject( dynamic_cast<SVTaskObjectClass *>( m_pTool ) );

	FillCombos();
	// TODO: Add extra initialization here

	//use the value object pointers to fill in the combo box and current values
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVFillBlobDlg::FillCombos()
{
	SVString BlobType;
	SVString BlobColor;

	m_pvoBlobType->GetEnumTypes( BlobType );
	m_ctlBlobType.SetEnumTypes( BlobType.c_str() );

	SVString CurrentType;
	m_pvoBlobType->GetValue( CurrentType );
	m_ctlBlobType.SelectString( -1,CurrentType.c_str() );

	SVString CurrentColor;
	m_pvoBlobFillColor->GetEnumTypes( BlobColor );
	m_ctlBlobColor.SetEnumTypes( BlobColor.c_str() );
	m_pvoBlobFillColor->GetValue( CurrentColor );
	m_ctlBlobColor.SelectString( -1,CurrentColor.c_str() );

	UpdateData( FALSE );
}

void SVFillBlobDlg::OnSelchangeComboBlobColor() 
{
	SetInspectionData();
}

void SVFillBlobDlg::OnSelchangeComboBlobType() 
{
	SetInspectionData();
}

