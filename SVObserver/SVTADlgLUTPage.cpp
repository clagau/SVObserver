//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgLUTPage
//* .File Name       : $Workfile:   SVTADlgLUTPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:15:20  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVTADlgLUTPage.h"

#include "SVFormulaEditorSheet.h"
#include "SVIPDoc.h"
#include "SVLutOperator.h"
#include "SVLutEquation.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVUserMessage.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//******************************************************************************
//* FUNCTION IMPLEMENTATION(S):
//******************************************************************************

BOOL CALLBACK SVLutGraphMousePointFunction( POINT Point, LPVOID PUserData )
{
	if( PUserData )
	{
		// Get Lut vector...
		SVToolAdjustmentDialogLUTPageClass* l_pDlg = reinterpret_cast <SVToolAdjustmentDialogLUTPageClass*> (PUserData);
		
		SVValueObjectReference l_svRef( l_pDlg->m_pLUTVector, Point.x );

		l_pDlg->AddInputRequest( l_svRef, Point.y );

		return TRUE;
	}

	return FALSE;
}


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVToolAdjustmentDialogLUTPageClass
//* Note(s)    : Property Page
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SVToolAdjustmentDialogLUTPageClass::SVToolAdjustmentDialogLUTPageClass( SVToolAdjustmentDialogSheetClass* Parent )
	: CPropertyPage(SVToolAdjustmentDialogLUTPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogLUTPageClass)
	m_strUpperClipValue = _T("");
	m_strLowerClipValue = _T("");
	m_bUseLUT = FALSE;
	m_bContinuousRecalcLUT = FALSE;
	//}}AFX_DATA_INIT

	m_pParentDialog = Parent;
	m_pTool         = NULL;

	m_pLUTOperator  = NULL;
	m_pLUTEquation  = NULL;
	m_pUseLUT       = NULL;
	m_pContinuousRecalcLUT = NULL;
	m_pLUTMode      = NULL;
	m_pLUTVector    = NULL;
	m_pLUTUpperClip = NULL;
	m_pLUTLowerClip = NULL;
}


HRESULT SVToolAdjustmentDialogLUTPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( m_pTool )
	{
		UpdateData( TRUE ); // get data from dialog

		long lUpperClip = ( long ) m_upperSlider.GetPos();
		
		l_hrOk = AddInputRequest( m_pLUTUpperClip, lUpperClip );

		if( l_hrOk == S_OK )
		{
			long lLowerClip = ( long ) m_lowerSlider.GetPos();

			l_hrOk = AddInputRequest( m_pLUTLowerClip, lLowerClip );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( m_pContinuousRecalcLUT, m_bContinuousRecalcLUT );
		}

		if( l_hrOk == S_OK )
		{
			int sel = m_LUTModeCombo.GetCurSel();
			if( sel >= 0 )
			{
				long lValue = ( long ) m_LUTModeCombo.GetItemData( sel );

				l_hrOk = AddInputRequest( m_pLUTMode, lValue );
			}
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( m_pUseLUT, m_bUseLUT );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( m_pTool );
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}

void SVToolAdjustmentDialogLUTPageClass::refresh( bool p_bSave = true )
{
	CWnd* pWnd = NULL;
	if( m_pTool )
	{
		if( p_bSave )
			SetInspectionData();

		if( m_pLUTVector )
		{
			// Refresh LUT Graph...
			std::vector<BYTE> byteVector;
			SVObjectByteArrayClass byteArray;
			m_pLUTVector->GetValues( byteVector );
			int nSize = byteVector.size();
			for( int j = 0; j < nSize; j++ )
			{
				BYTE byte;
				byte = byteVector[ j ];
				byteArray.SetAtGrow( j, byte );
			}// end for
			m_LUTGraph.SetPoints( byteArray );
		}// end if

		// Refresh upper clip...
		if(	m_pLUTUpperClip )
		{
			long lUpperClip = 0;
			m_pLUTUpperClip->GetValue( lUpperClip );

			m_strUpperClipValue.Format( _T( "%d" ), lUpperClip );
			m_upperSlider.SetPos( ( int ) lUpperClip );
		}

		// Refresh lower clip...
		if(	m_pLUTLowerClip )
		{
			long lLowerClip = 0;
			m_pLUTLowerClip->GetValue( lLowerClip );

			m_strLowerClipValue.Format( _T( "%d" ), lLowerClip );
			m_lowerSlider.SetPos( ( int ) lLowerClip );
		}

		if( m_pLUTMode )
		{
			// refresh lut mode combo settings...
			CString strEnum;
			if( m_pLUTMode->GetValue( strEnum ) == S_OK )
			{
				// Set cur sel in combo box...
				m_LUTModeCombo.SelectString( -1, strEnum );
			}

			// Show or hide Controls depending on LUT Mode...
			long lLUTMode = 0;
			m_pLUTMode->GetValue( lLUTMode );
			switch( lLUTMode )
			{
				case 0:	// Identity...
				case 1: // Inversion...
				case 2: // Sign...
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( NULL, NULL );

					// Show no special controls...
					if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
						pWnd->ShowWindow( SW_HIDE );

					break;
				
				case 3: // Clip...
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( NULL, NULL );

					// Show Clip Slider, Edit, Captions...
					if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
						pWnd->ShowWindow( SW_SHOW );
					// Hide Formula Button and others...
					if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
						pWnd->ShowWindow( SW_HIDE );

					break;

				case 4: // Formula...
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( NULL, NULL );

					// Show Formula Button...
					if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
						pWnd->ShowWindow( SW_SHOW );
					// Hide Clip Mode controls and others...
					if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
						pWnd->ShowWindow( SW_HIDE );

					break;

				case 5: // Free Form...
					// Set Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( SVLutGraphMousePointFunction, this );

					// Hide all other controls...
					if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
						pWnd->ShowWindow( SW_HIDE );

					break;
					
				default: // Unknown Mode...
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( NULL, NULL );

					// Hide all...
					if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
						pWnd->ShowWindow( SW_HIDE );
					if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
						pWnd->ShowWindow( SW_HIDE );
			}
		}
		else
		{
			// No LUT Mode...
			// Deactivate Mouse Proc Func of SVDlgGraph Control...
			m_LUTGraph.SetMousePointProcFunc( NULL, NULL );

			// Hide all...
			if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
				pWnd->ShowWindow( SW_HIDE );
		}

		if( m_pUseLUT )
		{
			// refresh LUT settings...
			m_pUseLUT->GetValue( m_bUseLUT );
		}
		else
			m_bUseLUT = FALSE;


		if( m_pContinuousRecalcLUT )
		{
			// refresh LUT settings...
			m_pContinuousRecalcLUT->GetValue( m_bContinuousRecalcLUT );
		}
		else
			m_bUseLUT = FALSE;

		if( ! m_bUseLUT )
		{
			// Deactivate Mouse Proc Func of SVDlgGraph Control...
			m_LUTGraph.SetMousePointProcFunc( NULL, NULL );
		}

		// Set controls activation states...
		if( pWnd = GetDlgItem( IDC_CONTINUOUS_RECALC_CHECK ) )
			pWnd->EnableWindow( m_bUseLUT );
		if( pWnd = GetDlgItem( IDC_LUT_MODE_COMBO ) )
			pWnd->EnableWindow( m_bUseLUT );
		if( pWnd = GetDlgItem( IDC_LUT_GRAPH ) )
			pWnd->EnableWindow( m_bUseLUT );
		if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
			pWnd->EnableWindow( m_bUseLUT );
		if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
			pWnd->EnableWindow( m_bUseLUT );
		if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
			pWnd->EnableWindow( m_bUseLUT );
		if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
			pWnd->EnableWindow( m_bUseLUT );
		if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
			pWnd->EnableWindow( m_bUseLUT );
		if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
			pWnd->EnableWindow( m_bUseLUT );
		if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
			pWnd->EnableWindow( m_bUseLUT );


		UpdateData( FALSE ); // set data to dialog

		return;
	}

	// Hide all...
	if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
		pWnd->ShowWindow( SW_HIDE );
	if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
		pWnd->ShowWindow( SW_HIDE );
	if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
		pWnd->ShowWindow( SW_HIDE );
	if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
		pWnd->ShowWindow( SW_HIDE );
	if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
		pWnd->ShowWindow( SW_HIDE );
	if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
		pWnd->ShowWindow( SW_HIDE );
	if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
		pWnd->ShowWindow( SW_HIDE );

	// Set controls activation states...
	if( pWnd = GetDlgItem( IDC_CONTINUOUS_RECALC_CHECK ) )
		pWnd->EnableWindow( FALSE );
	if( pWnd = GetDlgItem( IDC_LUT_MODE_COMBO ) )
		pWnd->EnableWindow( FALSE );
	if( pWnd = GetDlgItem( IDC_LUT_GRAPH ) )
		pWnd->EnableWindow( FALSE );

	UpdateData( FALSE ); // set data to dialog
}

void SVToolAdjustmentDialogLUTPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogLUTPageClass)
	DDX_Control(pDX, IDC_UPPER_SLIDER, m_upperSlider);
	DDX_Control(pDX, IDC_LOWER_SLIDER, m_lowerSlider);
	DDX_Control(pDX, IDC_LUT_GRAPH, m_LUTGraph);
	DDX_Control(pDX, IDC_LUT_MODE_COMBO, m_LUTModeCombo);
	DDX_Text(pDX, IDC_UPPER_EDIT, m_strUpperClipValue);
	DDX_Text(pDX, IDC_LOWER_EDIT, m_strLowerClipValue);
	DDX_Check(pDX, IDC_ACTIVATE_CHECK, m_bUseLUT);
	DDX_Check(pDX, IDC_CONTINUOUS_RECALC_CHECK, m_bContinuousRecalcLUT);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogLUTPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogLUTPageClass)
	ON_BN_CLICKED(IDC_ACTIVATE_CHECK, OnActivateCheck)
	ON_CBN_SELCHANGE(IDC_LUT_MODE_COMBO, OnSelchangeLutModeCombo)
	ON_BN_CLICKED(IDC_FORMULA_BUTTON, OnLUTFormulaButton)
	ON_BN_CLICKED(IDC_CONTINUOUS_RECALC_CHECK, OnContinuousRecalcCheck)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_MESSAGE( SV_REFRESH_DIALOG, OnGraphRefresh )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVToolAdjustmentDialogLUTPageClass message handlers

BOOL SVToolAdjustmentDialogLUTPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( m_pParentDialog && ( m_pTool = m_pParentDialog->GetTool() ) )
	{
		SetTaskObject( m_pTool );

		// Set Normalize Mode of Graph Control...
		m_LUTGraph.SetNormalizeMode( SVNormalize2D );

		// Set Clip Slider Data...
		m_upperSlider.SetRange( 0, 255 );
		m_lowerSlider.SetRange( 0, 255 );

		// Get LUT Operator...
		SVObjectTypeInfoStruct lutObjectInfo;
		lutObjectInfo.ObjectType = SVUnaryImageOperatorObjectType;
		lutObjectInfo.SubType	 = SVLUTOperatorObjectType;
		m_pLUTOperator = ( SVLUTOperatorClass* ) ::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &lutObjectInfo );
		if( m_pLUTOperator )
		{
			// Get Use Lut Flag...
			SVObjectTypeInfoStruct useLUTObjectInfo;
			useLUTObjectInfo.EmbeddedID = SVUseLUTObjectGuid;
			m_pUseLUT = ( SVBoolValueObjectClass* ) ::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &useLUTObjectInfo );
			if( m_pUseLUT )
			{
				m_pUseLUT->GetValue( m_bUseLUT );
			}

			// Get Continuous Recalc Lut Flag...
			SVObjectTypeInfoStruct continuousRecalcLUTObjectInfo;
			continuousRecalcLUTObjectInfo.EmbeddedID = SVContinuousRecalcLUTObjectGuid;
			m_pContinuousRecalcLUT = ( SVBoolValueObjectClass* ) ::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &continuousRecalcLUTObjectInfo );
			if( m_pContinuousRecalcLUT )
			{
				m_pContinuousRecalcLUT->GetValue( m_bContinuousRecalcLUT );
			}

			// Get Lut Equation...
			SVObjectTypeInfoStruct lutEquationObjectInfo;
			lutEquationObjectInfo.ObjectType = SVEquationObjectType;
			lutEquationObjectInfo.SubType	 = SVLUTEquationObjectType;
			m_pLUTEquation = ( SVLUTEquationClass* ) ::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &lutEquationObjectInfo );

			// Get Lut Mode...
			SVObjectTypeInfoStruct lutModeObjectInfo;
			lutModeObjectInfo.EmbeddedID = SVLUTModeObjectGuid;
			m_pLUTMode = ( SVEnumerateValueObjectClass* ) ::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &lutModeObjectInfo );
			if( m_pLUTMode )
			{
				// Populate LUT Mode combo...
				CString strEnumList;
				m_pLUTMode->GetEnumTypes( strEnumList );
				m_LUTModeCombo.SetEnumTypes( strEnumList );
			}

			// Get Lut Vector...
			SVObjectTypeInfoStruct lutVectorObjectInfo;
			lutVectorObjectInfo.EmbeddedID = SVOutputLUTVectorObjectGuid;
			m_pLUTVector = dynamic_cast <SVByteValueObjectClass*> 
			                    (reinterpret_cast <SVObjectClass*>
			                      (::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast <DWORD> (&lutVectorObjectInfo) ) ) );
			ASSERT( m_pLUTVector );

			// Get LUT Upper Clip...
			SVObjectTypeInfoStruct lutUpperClipObjectInfo;
			lutUpperClipObjectInfo.EmbeddedID = SVLUTUpperClipObjectGuid;
			m_pLUTUpperClip = dynamic_cast <SVLongValueObjectClass*>
			                       (reinterpret_cast <SVObjectClass*>
			                         (::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast <DWORD> (&lutUpperClipObjectInfo) ) ) );
			ASSERT( m_pLUTUpperClip );

			// Get LUT Lower Clip...
			SVObjectTypeInfoStruct lutLowerClipObjectInfo;
			lutLowerClipObjectInfo.EmbeddedID = SVLUTLowerClipObjectGuid;
			m_pLUTLowerClip = dynamic_cast <SVLongValueObjectClass*>
			                       (reinterpret_cast <SVObjectClass*>
			                         (::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast <DWORD> (&lutLowerClipObjectInfo) ) ) );
			ASSERT( m_pLUTLowerClip );
		}


		// Check...
		if( m_pLUTOperator &&	
			m_pUseLUT && 
			m_pLUTEquation && 
			m_pLUTMode && 
			m_pContinuousRecalcLUT && 
			m_pLUTVector &&
			m_pLUTUpperClip &&
			m_pLUTLowerClip
		  )
		{
			refresh( false );

			UpdateData( FALSE );

			return TRUE;
		}
	}

	// To deactivate all...
	refresh( false );

	UpdateData( FALSE );

	// Not valid call...
	if( GetParent() )
		GetParent()->SendMessage( WM_CLOSE );
	else
		SendMessage( WM_CLOSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben

}


void SVToolAdjustmentDialogLUTPageClass::OnActivateCheck() 
{
	refresh();
}

void SVToolAdjustmentDialogLUTPageClass::OnSelchangeLutModeCombo() 
{
	refresh();
}

void SVToolAdjustmentDialogLUTPageClass::OnLUTFormulaButton() 
{
	if( m_pLUTEquation )
	{
		CString l_Temp;
		l_Temp.LoadString( IDS_FORMULA_STRING );

		CString strCaption;
		strCaption = m_pLUTEquation->GetName();
		strCaption += _T( " " );
		strCaption += l_Temp;

		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( m_pLUTEquation );

		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogLUTPageClass::OnContinuousRecalcCheck() 
{
	refresh();
}

long SVToolAdjustmentDialogLUTPageClass::OnGraphRefresh( WPARAM mp1, LPARAM mp2 )
{
	refresh();

	return 0;
}

void SVToolAdjustmentDialogLUTPageClass::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd* pWnd = ChildWindowFromPoint( point );
	if( pWnd == &m_LUTGraph )
	{
		ClientToScreen( &point );
		pWnd->ScreenToClient( &point );
		pWnd->SendMessage( WM_LBUTTONDOWN, nFlags, MAKELPARAM( point.x, point.y ) );
	}
	else
		CPropertyPage::OnLButtonDown(nFlags, point);
}

void SVToolAdjustmentDialogLUTPageClass::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd* pWnd = ChildWindowFromPoint( point );
	if( pWnd == &m_LUTGraph )
	{
		ClientToScreen( &point );
		pWnd->ScreenToClient( &point );
		pWnd->SendMessage( WM_LBUTTONUP, nFlags, MAKELPARAM( point.x, point.y ) );
	}
	else
		CPropertyPage::OnLButtonUp(nFlags, point);
}

void SVToolAdjustmentDialogLUTPageClass::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd* pWnd = ChildWindowFromPoint( point );
	if( pWnd == &m_LUTGraph )
	{
		ClientToScreen( &point );
		pWnd->ScreenToClient( &point );
		pWnd->SendMessage( WM_MOUSEMOVE, nFlags, MAKELPARAM( point.x, point.y ) );
	}
	else
		CPropertyPage::OnMouseMove(nFlags, point);
}

void SVToolAdjustmentDialogLUTPageClass::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData( TRUE ); // Get data from dialog...

	CSliderCtrl* pSlider = ( CSliderCtrl* ) pScrollBar;

	if( m_pLUTUpperClip && pSlider == &m_upperSlider )
	{
		refresh();

		return;
	}
	else if( m_pLUTLowerClip && pSlider == &m_lowerSlider )
	{
		refresh();

		return;
	}
	
	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void SVToolAdjustmentDialogLUTPageClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData( TRUE ); // Get data from dialog...

	CSliderCtrl* pSlider = ( CSliderCtrl* ) pScrollBar;

	if( m_pLUTUpperClip && pSlider == &m_upperSlider )
	{
		refresh();

		return;
	}
	else if( m_pLUTLowerClip && pSlider == &m_lowerSlider )
	{
		refresh();

		return;
	}
	
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgLUTPage.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:15:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   02 Jul 2012 17:40:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   08 Dec 2010 13:39:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   07 Oct 2005 08:57:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix Bug OnInitDialog for Lut clip function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   31 Aug 2005 09:22:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed bugs with LUT Dlg Continous Reload and Formula
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   26 Aug 2005 08:45:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix Activate LUT check box
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   11 Aug 2005 13:28:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   09 Aug 2005 08:18:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Aug 2005 12:01:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   replaced SVVectorObject with SVValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   21 Jun 2005 08:32:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Feb 2005 16:04:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new reset / create methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Apr 2003 16:12:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   20 Nov 2002 13:42:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   #include "SVUserMessage.h"
 * Add ResultDataIndex  and ResultImageIndex
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Jan 2002 16:15:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  278
 * SCR Title:  Error message is displayed when removing result objects used by other tools
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified ::refresh and ::initMask and ::initThreshold to validate the toolset before blindly running it.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Jun 2001 10:17:08   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: refresh.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2001 19:28:58   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2001 15:40:32   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jun 2000 13:34:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  153
 * SCR Title:  LUT free form operation causes crash
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised declaration and implementation of OnGraphRefresh
 * to have two parameters as required.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Mar 2000 14:52:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Provides Tool Adjust Dialog Page for LUT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
