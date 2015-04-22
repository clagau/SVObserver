//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgLUTPage
//* .File Name       : $Workfile:   SVTADlgLUTPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 12:50:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgLUTPage.h"
#include "SVFormulaEditorSheet.h"
#include "SVIPDoc.h"
#include "SVLutOperator.h"
#include "SVLutEquation.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "ObjectInterfaces/SVUserMessage.h"
#pragma endregion Includes

#pragma region Declarations
//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region callback functions
//******************************************************************************
//* FUNCTION IMPLEMENTATION(S):
//******************************************************************************
BOOL CALLBACK SVLutGraphMousePointFunction( POINT Point, LPVOID PUserData )
{
	if( PUserData )
	{
		// Get Lut vector...
		SVToolAdjustmentDialogLUTPageClass* l_pDlg = reinterpret_cast <SVToolAdjustmentDialogLUTPageClass*> (PUserData);
		
		SVValueObjectReference l_svRef( l_pDlg->getLUTVector(), Point.x );

		l_pDlg->AddInputRequest( l_svRef, Point.y );

		return TRUE;
	}

	return FALSE;
}
#pragma endregion callback functions

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************
#pragma region Constructor
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
	m_isFormulaClip = TRUE;
	//}}AFX_DATA_INIT

	m_pParentDialog = Parent;
	m_pTool         = nullptr;
	m_pLUTOperator  = nullptr;
	m_pLUTEquation  = nullptr;
	m_pUseLUT       = nullptr;
	m_pContinuousRecalcLUT = nullptr;
	m_pLUTMode      = nullptr;
	m_pLUTVector    = nullptr;
	m_pLUTUpperClip = nullptr;
	m_pLUTLowerClip = nullptr;
	m_pIsLUTFormulaClipped = nullptr;
}
#pragma endregion Constructor

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogLUTPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogLUTPageClass)
	ON_BN_CLICKED(IDC_ACTIVATE_CHECK, OnActivateCheck)
	ON_CBN_SELCHANGE(IDC_LUT_MODE_COMBO, OnSelChangeLutModeCombo)
	ON_BN_CLICKED(IDC_FORMULA_BUTTON, OnLUTFormulaButton)
	ON_BN_CLICKED(IDC_CONTINUOUS_RECALC_CHECK, OnContinuousRecalcCheck)
	ON_BN_CLICKED(IDC_CLIP_MODE_CHECK, OnClipModeCheck)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_LOWER_EDIT, OnLowerEditLostFocus)
	ON_EN_KILLFOCUS(IDC_UPPER_EDIT, OnUpperEditLostFocus)
	//}}AFX_MSG_MAP
	ON_MESSAGE( SV_REFRESH_DIALOG, OnGraphRefresh )
END_MESSAGE_MAP()

#pragma region Public Methods
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
			l_hrOk = AddInputRequest( m_pIsLUTFormulaClipped, m_isFormulaClip );
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
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma region virtual
void SVToolAdjustmentDialogLUTPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogLUTPageClass)
	DDX_Control(pDX, IDC_UPPER_SLIDER, m_upperSlider);
	DDX_Control(pDX, IDC_LOWER_SLIDER, m_lowerSlider);
	DDX_Control(pDX, IDC_LUT_GRAPH, m_LUTGraph);
	DDX_Control(pDX, IDC_LUT_MODE_COMBO, m_LUTModeCombo);
	DDX_Control(pDX, IDC_YAXISLABEL, m_yAxisLabel);
	DDX_Text(pDX, IDC_UPPER_EDIT, m_strUpperClipValue);
	DDX_Text(pDX, IDC_LOWER_EDIT, m_strLowerClipValue);
	DDX_Check(pDX, IDC_ACTIVATE_CHECK, m_bUseLUT);
	DDX_Check(pDX, IDC_CONTINUOUS_RECALC_CHECK, m_bContinuousRecalcLUT);
	DDX_Check(pDX, IDC_CLIP_MODE_CHECK, m_isFormulaClip);
	//}}AFX_DATA_MAP
}

BOOL SVToolAdjustmentDialogLUTPageClass::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	if( m_pParentDialog && ( m_pTool = m_pParentDialog->GetTool() ) )
	{
		SetTaskObject( m_pTool );

		// Rotate the text for the vertical axis.
		m_yAxisLabel.SetRotationAngle( 90, true );

		// Set Normalize Mode of Graph Control...
		m_LUTGraph.SetNormalizeMode( SvOml::SVNormalize2D );

		// Set Clip Slider Data...
		m_upperSlider.SetRange( 0, 255 );
		m_lowerSlider.SetRange( 0, 255 );

		// Get LUT Operator...
		SVObjectTypeInfoStruct lutObjectInfo;
		lutObjectInfo.ObjectType = SVUnaryImageOperatorObjectType;
		lutObjectInfo.SubType	 = SVLUTOperatorObjectType;
		m_pLUTOperator = reinterpret_cast<SVLUTOperatorClass*>(::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&lutObjectInfo) ));
		if( m_pLUTOperator )
		{
			// Get Use Lut Flag...
			SVObjectTypeInfoStruct useLUTObjectInfo;
			useLUTObjectInfo.EmbeddedID = SVUseLUTObjectGuid;
			m_pUseLUT = reinterpret_cast<SVBoolValueObjectClass*>(::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&useLUTObjectInfo) ));
			if( m_pUseLUT )
			{
				m_pUseLUT->GetValue( m_bUseLUT );
			}

			// Get Continuous Recalc Lut Flag...
			SVObjectTypeInfoStruct continuousRecalcLUTObjectInfo;
			continuousRecalcLUTObjectInfo.EmbeddedID = SVContinuousRecalcLUTObjectGuid;
			m_pContinuousRecalcLUT = reinterpret_cast<SVBoolValueObjectClass*>(::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&continuousRecalcLUTObjectInfo) ));
			if( m_pContinuousRecalcLUT )
			{
				m_pContinuousRecalcLUT->GetValue( m_bContinuousRecalcLUT );
			}

			// Get Lut Equation...
			SVObjectTypeInfoStruct lutEquationObjectInfo;
			lutEquationObjectInfo.ObjectType = SVEquationObjectType;
			lutEquationObjectInfo.SubType	 = SVLUTEquationObjectType;
			m_pLUTEquation = reinterpret_cast<SVLUTEquationClass*>(::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&lutEquationObjectInfo) ));
			// Get Lut Mode...
			SVObjectTypeInfoStruct lutModeObjectInfo;
			lutModeObjectInfo.EmbeddedID = SVLUTModeObjectGuid;
			m_pLUTMode = reinterpret_cast<SVEnumerateValueObjectClass*>(::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&lutModeObjectInfo) ));
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
			                      (::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&lutVectorObjectInfo) ) ) );
			ASSERT( m_pLUTVector );

			// Get LUT Upper Clip...
			SVObjectTypeInfoStruct lutUpperClipObjectInfo;
			lutUpperClipObjectInfo.EmbeddedID = SVLUTUpperClipObjectGuid;
			m_pLUTUpperClip = dynamic_cast <SVLongValueObjectClass*>
			                       (reinterpret_cast <SVObjectClass*>
			                         (::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&lutUpperClipObjectInfo) ) ) );
			ASSERT( m_pLUTUpperClip );

			// Get LUT Lower Clip...
			SVObjectTypeInfoStruct lutLowerClipObjectInfo;
			lutLowerClipObjectInfo.EmbeddedID = SVLUTLowerClipObjectGuid;
			m_pLUTLowerClip = dynamic_cast <SVLongValueObjectClass*>
			                       (reinterpret_cast <SVObjectClass*>
			                         (::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&lutLowerClipObjectInfo) ) ) );
			ASSERT( m_pLUTLowerClip );

			// Get Formula Clip Flag...
			SVObjectTypeInfoStruct formulaClipFlagObjectInfo;
			formulaClipFlagObjectInfo.EmbeddedID = SVLUTEquationClipFlagObjectGuid;
			m_pIsLUTFormulaClipped = reinterpret_cast<SVBoolValueObjectClass*>(::SVSendMessage( m_pLUTOperator, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&formulaClipFlagObjectInfo) ));
			if( nullptr != m_pIsLUTFormulaClipped )
			{
				m_pIsLUTFormulaClipped->GetValue( m_isFormulaClip );
			}
		}

		// Check...
		if( m_pLUTOperator && 
			m_pUseLUT && 
			m_pLUTEquation && 
			m_pLUTMode && 
			m_pContinuousRecalcLUT && 
			m_pLUTVector &&
			m_pLUTUpperClip &&
			m_pLUTLowerClip &&
			m_pIsLUTFormulaClipped )
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
#pragma endregion virtual

void SVToolAdjustmentDialogLUTPageClass::OnActivateCheck()
{
	refresh();
}

void SVToolAdjustmentDialogLUTPageClass::OnSelChangeLutModeCombo()
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

void SVToolAdjustmentDialogLUTPageClass::OnClipModeCheck()
{
	refresh();
}

LRESULT SVToolAdjustmentDialogLUTPageClass::OnGraphRefresh( WPARAM mp1, LPARAM mp2 )
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

void SVToolAdjustmentDialogLUTPageClass::OnLowerEditLostFocus()
{
	int lLowerClip = 0;
	UpdateData( TRUE ); // Get data from dialog...
	int valNumbers = sscanf(m_strLowerClipValue, "%d", &lLowerClip);
	if (1 == valNumbers)
	{
		m_lowerSlider.SetPos( lLowerClip );
	}
	refresh();
}

void SVToolAdjustmentDialogLUTPageClass::OnUpperEditLostFocus()
{
	int lClip = 0;
	UpdateData( TRUE ); // Get data from dialog...
	int valNumbers = sscanf(m_strUpperClipValue, "%d", &lClip);
	if (1 == valNumbers)
	{
		m_upperSlider.SetPos( lClip );
	}
	refresh();
}

void SVToolAdjustmentDialogLUTPageClass::refresh( bool p_bSave /*= true*/ )
{
	CWnd* pWnd = NULL;
	if( nullptr != m_pTool )
	{
		if( p_bSave ) { SetInspectionData(); }

		refreshLUTGraph();
		refreshClip();

		if( nullptr != m_pUseLUT )
		{
			m_pUseLUT->GetValue( m_bUseLUT );
		}
		else
			m_bUseLUT = FALSE;

		if( nullptr != m_pLUTMode )
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
					hideAllUncommonControls();
					break;
				
				case 3: // Clip...
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( NULL, NULL );
					showClipControls();
					break;

				case 4: // Formula...
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( NULL, NULL );
					showFormulaControls();
					if( nullptr != m_pIsLUTFormulaClipped )
					{
						m_pIsLUTFormulaClipped->GetValue( m_isFormulaClip );
					}
					else
					{
						m_bUseLUT = FALSE;
					}
					break;

				case 5: // Free Form...
					// Set Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( SVLutGraphMousePointFunction, this );
					hideAllUncommonControls();
					break;

				default: // Unknown Mode...
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( NULL, NULL );
					hideAllUncommonControls();
					break;
			}
		}
		else //if( nullptr != m_pLUTMode )
		{
			// No LUT Mode...
			// Deactivate Mouse Proc Func of SVDlgGraph Control...
			m_LUTGraph.SetMousePointProcFunc( NULL, NULL );
			hideAllUncommonControls();
		}

		if( nullptr != m_pContinuousRecalcLUT )
		{
			m_pContinuousRecalcLUT->GetValue( m_bContinuousRecalcLUT );
		}
		else
			m_bUseLUT = FALSE;

		if( ! m_bUseLUT )
		{	// Deactivate Mouse Proc Func of SVDlgGraph Control...
			m_LUTGraph.SetMousePointProcFunc( NULL, NULL );
		}

		enableAllControls( m_bUseLUT );
		UpdateData( FALSE ); // set data to dialog
	}
	else //if( nullptr != m_pTool )
	{
		hideAllUncommonControls();
		enableAllControls(FALSE);
		UpdateData( FALSE ); // set data to dialog
	}
}

void SVToolAdjustmentDialogLUTPageClass::refreshLUTGraph()
{
	// Refresh LUT Graph...
	if( nullptr != m_pLUTVector )
	{
		std::vector<BYTE> byteVector;
		SvCl::SVObjectByteArrayClass byteArray;
		m_pLUTVector->GetValues( byteVector );
		int nSize = static_cast<int>(byteVector.size());
		for( int j = 0; j < nSize; j++ )
		{
			BYTE byte;
			byte = byteVector[ j ];
			byteArray.SetAtGrow( j, byte );
		}// end for
		m_LUTGraph.SetPoints( byteArray );
	}// end if
}

void SVToolAdjustmentDialogLUTPageClass::refreshClip()
{
	// Refresh upper clip...
	if(	nullptr != m_pLUTUpperClip )
	{
		long lUpperClip = 0;
		m_pLUTUpperClip->GetValue( lUpperClip );

		m_strUpperClipValue.Format( _T( "%d" ), lUpperClip );
		m_upperSlider.SetPos( ( int ) lUpperClip );
	}

	// Refresh lower clip...
	if(	nullptr != m_pLUTLowerClip )
	{
		long lLowerClip = 0;
		m_pLUTLowerClip->GetValue( lLowerClip );

		m_strLowerClipValue.Format( _T( "%d" ), lLowerClip );
		m_lowerSlider.SetPos( ( int ) lLowerClip );
	}
}

void SVToolAdjustmentDialogLUTPageClass::showFormulaControls()
{
	CWnd* pWnd = nullptr;
	// Show Formula Button...
	if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
		pWnd->ShowWindow( SW_SHOW );
	if( pWnd = GetDlgItem( IDC_CLIP_MODE_CHECK ) )
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
}

void SVToolAdjustmentDialogLUTPageClass::showClipControls()
{
	CWnd* pWnd = nullptr;
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
	if( pWnd = GetDlgItem( IDC_CLIP_MODE_CHECK ) )
		pWnd->ShowWindow( SW_HIDE );
}

void SVToolAdjustmentDialogLUTPageClass::hideAllUncommonControls()
{
	CWnd* pWnd = nullptr;
	// Hide all controls...
	if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
		pWnd->ShowWindow( SW_HIDE );
	if( pWnd = GetDlgItem( IDC_CLIP_MODE_CHECK ) )
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

void SVToolAdjustmentDialogLUTPageClass::enableAllControls(BOOL isEnable)
{
	CWnd* pWnd = nullptr;
	// Set controls activation states...
	if( pWnd = GetDlgItem( IDC_CONTINUOUS_RECALC_CHECK ) )
		pWnd->EnableWindow( isEnable );
	if( pWnd = GetDlgItem( IDC_LUT_MODE_COMBO ) )
		pWnd->EnableWindow( isEnable );
	if( pWnd = GetDlgItem( IDC_LUT_GRAPH ) )
		pWnd->EnableWindow( isEnable );
	if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
		pWnd->EnableWindow( isEnable );
	if( pWnd = GetDlgItem( IDC_CLIP_MODE_CHECK ) )
		pWnd->EnableWindow( isEnable );
	if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
		pWnd->EnableWindow( isEnable );
	if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
		pWnd->EnableWindow( isEnable );
	if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
		pWnd->EnableWindow( isEnable );
	if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
		pWnd->EnableWindow( isEnable );
	if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
		pWnd->EnableWindow( isEnable );
	if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
		pWnd->EnableWindow( isEnable );
}
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgLUTPage.cpp_v  $
 * 
 *    Rev 1.4   15 May 2014 12:50:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2014 12:14:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jan 2014 15:48:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  883
 * SCR Title:  Clip-mode for LUT formula mode
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed code to conform with guidelines.
 * Changed SVLutGraphMousePointFunction to use getLUTVector.
 * Changed OnSelchangeLutModeCombo to OnSelChangeLutModeCombo.
 * Added protected methods OnClipModeCheck, OnLowerEditLostFocus, and OnUpperEditLostFocus.
 * Changed SetInspectionData to set Clip parameters.
 * Moved code from refresh method into several methods (refreshLUTGraph, refreshClip, showFormulaControls, showClipControls, hideAllUncommonControls, enableAllControls)	
 * Changed method DoDataExchange to include m_isForumulaClip.
 * Changed method OnInitDialog to initialize m_pIsLUTFormulaClipped.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 08:17:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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