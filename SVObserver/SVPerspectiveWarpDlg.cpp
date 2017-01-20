//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPerspectiveWarpDlg
//* .File Name       : $Workfile:   SVPerspectiveWarpDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:31:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPerspectiveWarpDlg.h"
#include "svobserver.h"
#include "SVTool.h"
#include "SVPerspectiveTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "CameraLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVPerspectiveWarpDlg dialog


SVPerspectiveWarpDlg::SVPerspectiveWarpDlg( const SVGUID& rInspectionId, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id ) 
: CPropertyPage( id )
, pParentDialog(Parent)
{
	
	//{{AFX_DATA_INIT(SVPerspectiveWarpDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVPerspectiveWarpDlg::~SVPerspectiveWarpDlg()
{
}

HRESULT SVPerspectiveWarpDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	if( m_pTool )
	{
		bool l_bUpdate = false;

		UpdateData( TRUE ); // get data from dialog

		int sel = m_FunctionCombo.GetCurSel();
		if( sel >= 0 )
		{
			l_bUpdate = true;

			long lValue = ( long ) m_FunctionCombo.GetItemData( sel );

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pWarpType, lValue );
			}
		}

		sel = m_cbInterpolation.GetCurSel();
		if( sel >= 0 )
		{
			l_bUpdate = true;

			long lValue = ( long ) m_cbInterpolation.GetItemData( sel );

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pInterpolationMode, lValue );
			}
		}

		if( l_bUpdate )
		{
			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequestMarker();
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = RunOnce( m_pTool->GetUniqueObjectID() );
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


void SVPerspectiveWarpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPerspectiveWarpDlg)
	DDX_Control(pDX, IDC_PERSPECTIVE_UNWARP_TYPE_COMBO, m_FunctionCombo);
	DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_cbInterpolation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPerspectiveWarpDlg, CDialog)
	//{{AFX_MSG_MAP(SVPerspectiveWarpDlg)
	ON_CBN_SELCHANGE(IDC_PERSPECTIVE_UNWARP_TYPE_COMBO, OnSelchangePerspectiveUnwarpTypeCombo)
	ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnSelchangeInterpolationModeCombo)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPerspectiveWarpDlg message handlers

BOOL SVPerspectiveWarpDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( pParentDialog && ( m_pTool = pParentDialog->GetTool() ) )
	{
		SetTaskObject( m_pTool );

		SVObjectTypeInfoStruct objectInfo;
		objectInfo.EmbeddedID = SVWarpTypeObjectGuid;
		m_pWarpType = dynamic_cast<SVEnumerateValueObjectClass*>(m_pTool->getFirstObject(objectInfo));
		if( m_pWarpType )
		{
			
			SVString EnumList;

			m_pWarpType->GetEnumTypes( EnumList );
			m_FunctionCombo.SetEnumTypes( EnumList.c_str() );

			SVString EnumString;
			m_pWarpType->GetValue( EnumString );
			m_FunctionCombo.SelectString( -1, EnumString.c_str() );
		}
		m_pWarpType->GetValue( m_lLastWarpType );

		// Interpolation Mode
		objectInfo.EmbeddedID = SVOutputInterpolationModeObjectGuid;
		m_pInterpolationMode = dynamic_cast<SVEnumerateValueObjectClass*>(m_pTool->getFirstObject(objectInfo));
		if( m_pInterpolationMode )
		{
			SVString EnumList;

			m_pInterpolationMode->GetEnumTypes( EnumList );
			m_cbInterpolation.SetEnumTypes( EnumList.c_str() );

			SVString EnumString;
			m_pInterpolationMode->GetValue( EnumString );
			m_cbInterpolation.SelectString( -1, EnumString.c_str() );
		}
		m_pWarpType->GetValue( m_lLastWarpType );

	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVPerspectiveWarpDlg::OnSelchangePerspectiveUnwarpTypeCombo() 
{
	SetInspectionData();
}


void SVPerspectiveWarpDlg::OnDestroy() 
{
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
		SVPerspectiveToolClass::WarpType eWarpType = (SVPerspectiveToolClass::WarpType) lWarpType;

		if( eWarpType == SVPerspectiveToolClass::WarpTypeVertical )
		{
			l_svExtents.SetExtentProperty( SVExtentPropertyTranslationOffsetY, 10 );
			l_svExtents.SetExtentProperty( SVExtentPropertyTranslationOffsetX, 0 );
		}
		else
		if( eWarpType == SVPerspectiveToolClass::WarpTypeHorizontal  )
		{
			l_svExtents.SetExtentProperty( SVExtentPropertyTranslationOffsetX, 10 );
			l_svExtents.SetExtentProperty( SVExtentPropertyTranslationOffsetY, 0 );
		}
		m_pTool->SetImageExtent( 1, l_svExtents );
	}
	CDialog::OnDestroy();
}

void SVPerspectiveWarpDlg::OnSelchangeInterpolationModeCombo() 
{
	SetInspectionData();
}

