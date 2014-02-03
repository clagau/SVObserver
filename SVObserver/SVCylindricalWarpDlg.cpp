//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCylindricalWarpDlg
//* .File Name       : $Workfile:   SVCylindricalWarpDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 10:32:24  $
//******************************************************************************

// SVCylindricalWarpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "svobserver.h"
#include "SVTool.h"
#include "SVCylindricalWarpDlg.h"
#include "SVCylindricalWarpTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVCylindricalWarpDlg dialog


SVCylindricalWarpDlg::SVCylindricalWarpDlg( SVToolAdjustmentDialogSheetClass* Parent, int id ) 
	: CPropertyPage( id )
{
	m_pParentDialog		= Parent;
	//{{AFX_DATA_INIT(SVCylindricalWarpDlg)
	m_dWarpAngle = 0.0;
	//}}AFX_DATA_INIT
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
		m_pWarpType = ( SVEnumerateValueObjectClass* ) ::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) );
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
		m_pInterpolationMode = ( SVEnumerateValueObjectClass* ) ::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) );
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
		m_pWarpAngle = ( SVDoubleValueObjectClass* ) ::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVCylindricalWarpDlg.cpp_v  $
 * 
 *    Rev 1.1   01 Feb 2014 10:32:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:05:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jul 2012 16:39:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Aug 2005 15:37:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Jul 2005 15:50:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  501
 * SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed coding cut and paste error in interpolation change combo.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jul 2005 08:50:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  501
 * SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Cylindrical Dialog Changes
 * Added Warp Angle and Interpolation Mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   09 Mar 2005 12:17:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
