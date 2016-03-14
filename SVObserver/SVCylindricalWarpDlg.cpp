//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCylindricalWarpDlg
//* .File Name       : $Workfile:   SVCylindricalWarpDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:12:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "svobserver.h"
#include "SVTool.h"
#include "SVCylindricalWarpDlg.h"
#include "SVCylindricalWarpTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVCylindricalWarpDlg::SVCylindricalWarpDlg( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id ) 
: CPropertyPage( id )
, m_pParentDialog(Parent)
{
	//{{AFX_DATA_INIT(SVCylindricalWarpDlg)
	m_dWarpAngle = 0.0;
	//}}AFX_DATA_INIT
}

SVCylindricalWarpDlg::~SVCylindricalWarpDlg() 
{
}

HRESULT SVCylindricalWarpDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	if( m_pTool )
	{
		bool l_bUpdate = false;

		UpdateData( TRUE ); // get data from dialog

		int sel = m_cbOrientation.GetCurSel();
		if( sel >= 0 )
		{
			long lValue = ( long ) m_cbOrientation.GetItemData( sel );

			l_bUpdate = true;

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pWarpType, lValue );
			}
		}

		if( m_dWarpAngle >= 10 &&	m_dWarpAngle <= 180)
		{
			l_bUpdate = true;

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pWarpAngle, m_dWarpAngle );
			}
		}

		sel = m_cbInterpolation.GetCurSel();
		if( sel >= 0 )
		{
			long lValue = ( long ) m_cbInterpolation.GetItemData( sel );

			l_bUpdate = true;

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pInterpolationMode, lValue );
			}
		}

		if( l_bUpdate )
		{
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequestMarker();
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = RunOnce( m_pTool );
			}
		}

		UpdateData( FALSE );
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}


void SVCylindricalWarpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVCylindricalWarpDlg)
	DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_cbInterpolation);
	DDX_Control(pDX, IDC_ORIENTATION, m_cbOrientation);
	DDX_Text(pDX, IDC_WARP_ANGLE_EDIT, m_dWarpAngle);
	DDV_MinMaxDouble(pDX, m_dWarpAngle, 10., 180.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVCylindricalWarpDlg, CDialog)
	//{{AFX_MSG_MAP(SVCylindricalWarpDlg)
	ON_CBN_SELCHANGE(IDC_ORIENTATION, OnSelchangeOrientationCombo)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnSelchangeInterpolationModeCombo)
	ON_EN_KILLFOCUS(IDC_WARP_ANGLE_EDIT, OnKillfocusWarpAngleEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVCylindricalWarpDlg message handlers

BOOL SVCylindricalWarpDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( m_pParentDialog && ( m_pTool = m_pParentDialog->GetTool() ) )
	{
		SetTaskObject( m_pTool );

		SVObjectTypeInfoStruct objectInfo;
		// Warp Type
		objectInfo.EmbeddedID = SVWarpTypeObjectGuid;
		m_pWarpType = reinterpret_cast<SVEnumerateValueObjectClass*>(::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo)));
		if( m_pWarpType )
		{
			CString l_strEnumList;

			m_pWarpType->GetEnumTypes( l_strEnumList );
			m_cbOrientation.SetEnumTypes( l_strEnumList );

			CString strEnum;
			m_pWarpType->GetValue( strEnum );
			m_cbOrientation.SelectString( -1, strEnum );
		}
		m_pWarpType->GetValue( m_lLastWarpType );

		// Interpolation Mode
		objectInfo.EmbeddedID = SVOutputInterpolationModeObjectGuid;
		m_pInterpolationMode = reinterpret_cast<SVEnumerateValueObjectClass*>(::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo) ));
		if( m_pInterpolationMode )
		{
			CString l_strEnumList;

			m_pInterpolationMode->GetEnumTypes( l_strEnumList );
			m_cbInterpolation.SetEnumTypes( l_strEnumList );

			CString strEnum;
			m_pInterpolationMode->GetValue( strEnum );
			m_cbInterpolation.SelectString( -1, strEnum );
		}
		m_pWarpType->GetValue( m_lLastWarpType );


		// Warp Angle
		objectInfo.EmbeddedID = SVWarpAngleObjectGuid;
		m_pWarpAngle = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo)) );
		if( m_pWarpAngle )
		{
			m_pWarpAngle->GetValue( m_dWarpAngle );
		}
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVCylindricalWarpDlg::OnSelchangeOrientationCombo()
{
	SetInspectionData();
}

void SVCylindricalWarpDlg::OnDestroy() 
{
	UpdateData();
	// Set Some defaults if warp type changed...
	long l_lType ;
	m_pWarpType->GetValue( l_lType );
	if( l_lType != m_lLastWarpType )
	{
		SVImageExtentClass l_svExtents;
		HRESULT l_hr = S_OK;
		l_hr = m_pTool->GetImageExtent( l_svExtents );
		long lWarpType;
		m_pWarpType->GetValue( lWarpType );
		SVCylindricalWarpToolClass::WarpType eWarpType = (SVCylindricalWarpToolClass::WarpType) lWarpType;

		if( eWarpType == SVCylindricalWarpToolClass::WarpTypeVertical )
		{
			l_svExtents.SetTranslation( SVExtentTranslationCylindricalWarpV );
		}
		else
		if( eWarpType == SVCylindricalWarpToolClass::WarpTypeHorizontal )
		{
			l_svExtents.SetTranslation( SVExtentTranslationCylindricalWarpH );
		}
		HRESULT hrUpdate = l_svExtents.UpdateData();
		ASSERT( hrUpdate == S_OK );
		m_pTool->SetImageExtent( 1, l_svExtents );
	}

	SetInspectionData();

	CDialog::OnDestroy();
}

void SVCylindricalWarpDlg::OnSelchangeInterpolationModeCombo() 
{
	SetInspectionData();
}


void SVCylindricalWarpDlg::OnKillfocusWarpAngleEdit() 
{
	UpdateData();
	SVCylindricalWarpToolClass *l_pWarpTool = dynamic_cast<SVCylindricalWarpToolClass*>(m_pTool);
	if( l_pWarpTool )
	{
		l_pWarpTool->ValidateAngle( m_dWarpAngle );
		UpdateData(FALSE );
	}
}

