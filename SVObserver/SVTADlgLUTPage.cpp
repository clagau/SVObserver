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
#include "SvOGui/SVFormulaEditorSheet.h"
#include "SVIPDoc.h"
#include "SVLutOperator.h"
#include "SVLutEquation.h"
#include "SVOCore/SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#pragma region Declarations

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
		
		SVObjectReference ObjectRef( l_pDlg->getLUTVector(), Point.x );

		l_pDlg->AddInputRequest( ObjectRef, Point.y );

		return TRUE;
	}

	return FALSE;
}
#pragma endregion callback functions

#pragma region Constructor

SVToolAdjustmentDialogLUTPageClass::SVToolAdjustmentDialogLUTPageClass( const GUID& rInspectionID, const GUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent )
	: CPropertyPage(SVToolAdjustmentDialogLUTPageClass::IDD)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID) 
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

SVToolAdjustmentDialogLUTPageClass::~SVToolAdjustmentDialogLUTPageClass()
{
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

		if( S_OK == l_hrOk )
		{
			long lLowerClip = ( long ) m_lowerSlider.GetPos();

			l_hrOk = AddInputRequest( m_pLUTLowerClip, lLowerClip );
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequest( m_pContinuousRecalcLUT, m_bContinuousRecalcLUT );
		}

		if( S_OK == l_hrOk )
		{
			int sel = m_LUTModeCombo.GetCurSel();
			if( sel >= 0 )
			{
				long lValue = ( long ) m_LUTModeCombo.GetItemData( sel );

				l_hrOk = AddInputRequest( m_pLUTMode, lValue );
			}
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequest( m_pUseLUT, m_bUseLUT );
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequest( m_pIsLUTFormulaClipped, m_isFormulaClip );
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = RunOnce( m_pTool->GetUniqueObjectID() );
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
		m_LUTGraph.SetNormalizeMode( SvMc::SVNormalize2D );

		// Set Clip Slider Data...
		m_upperSlider.SetRange( 0, 255 );
		m_lowerSlider.SetRange( 0, 255 );

		// Get LUT Operator...
		SVObjectTypeInfoStruct lutObjectInfo;
		lutObjectInfo.ObjectType = SVUnaryImageOperatorObjectType;
		lutObjectInfo.SubType	 = SVLUTOperatorObjectType;
		m_pLUTOperator = dynamic_cast<SVLUTOperatorClass*>(m_pTool->getFirstObject(lutObjectInfo));
		if( m_pLUTOperator )
		{
			// Get Use Lut Flag...
			SVObjectTypeInfoStruct useLUTObjectInfo;
			useLUTObjectInfo.EmbeddedID = SVUseLUTObjectGuid;
			m_pUseLUT = dynamic_cast<SVBoolValueObjectClass*>(m_pLUTOperator->getFirstObject(useLUTObjectInfo));
			if( m_pUseLUT )
			{
				m_pUseLUT->GetValue( m_bUseLUT );
			}

			// Get Continuous Recalc Lut Flag...
			SVObjectTypeInfoStruct continuousRecalcLUTObjectInfo;
			continuousRecalcLUTObjectInfo.EmbeddedID = SVContinuousRecalcLUTObjectGuid;
			m_pContinuousRecalcLUT = dynamic_cast<SVBoolValueObjectClass*>(m_pLUTOperator->getFirstObject(continuousRecalcLUTObjectInfo));
			if( m_pContinuousRecalcLUT )
			{
				m_pContinuousRecalcLUT->GetValue( m_bContinuousRecalcLUT );
			}

			// Get Lut Equation...
			SVObjectTypeInfoStruct lutEquationObjectInfo;
			lutEquationObjectInfo.ObjectType = SVEquationObjectType;
			lutEquationObjectInfo.SubType	 = SVLUTEquationObjectType;
			m_pLUTEquation = dynamic_cast<SVLUTEquationClass*>(m_pLUTOperator->getFirstObject(lutEquationObjectInfo));
			// Get Lut Mode...
			SVObjectTypeInfoStruct lutModeObjectInfo;
			lutModeObjectInfo.EmbeddedID = SVLUTModeObjectGuid;
			m_pLUTMode = dynamic_cast<SVEnumerateValueObjectClass*>(m_pLUTOperator->getFirstObject(lutModeObjectInfo));
			if( m_pLUTMode )
			{
				// Populate LUT Mode combo...
				SVString EnumList;
				m_pLUTMode->GetEnumTypes( EnumList );
				m_LUTModeCombo.SetEnumTypes( EnumList.c_str() );
			}

			// Get Lut Vector...
			SVObjectTypeInfoStruct lutVectorObjectInfo;
			lutVectorObjectInfo.EmbeddedID = SVOutputLUTVectorObjectGuid;
			m_pLUTVector = dynamic_cast <SVByteValueObjectClass*>(m_pLUTOperator->getFirstObject(lutVectorObjectInfo));
			ASSERT( m_pLUTVector );

			// Get LUT Upper Clip...
			SVObjectTypeInfoStruct lutUpperClipObjectInfo;
			lutUpperClipObjectInfo.EmbeddedID = SVLUTUpperClipObjectGuid;
			m_pLUTUpperClip = dynamic_cast <SVLongValueObjectClass*>(m_pLUTOperator->getFirstObject(lutUpperClipObjectInfo));
			ASSERT( m_pLUTUpperClip );

			// Get LUT Lower Clip...
			SVObjectTypeInfoStruct lutLowerClipObjectInfo;
			lutLowerClipObjectInfo.EmbeddedID = SVLUTLowerClipObjectGuid;
			m_pLUTLowerClip = dynamic_cast <SVLongValueObjectClass*>(m_pLUTOperator->getFirstObject(lutLowerClipObjectInfo));
			ASSERT( m_pLUTLowerClip );

			// Get Formula Clip Flag...
			SVObjectTypeInfoStruct formulaClipFlagObjectInfo;
			formulaClipFlagObjectInfo.EmbeddedID = SVLUTEquationClipFlagObjectGuid;
			m_pIsLUTFormulaClipped = dynamic_cast<SVBoolValueObjectClass*>(m_pLUTOperator->getFirstObject(formulaClipFlagObjectInfo));
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
		SVString Text = SvUl_SF::LoadSVString( IDS_FORMULA_STRING );

		SVString Caption = m_pLUTEquation->GetName();
		Caption += _T( " " )+ Text;

		const GUID& rObjectID = m_pLUTOperator->GetUniqueObjectID();
		SVObjectTypeInfoStruct info(SVEquationObjectType, SVLUTEquationObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( m_InspectionID, rObjectID, info, Caption.c_str() );

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
	CWnd* pWnd = nullptr;
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
			SVString EnumString;
			if( S_OK == m_pLUTMode->getValue( EnumString ) )
			{
				// Set cur sel in combo box...
				m_LUTModeCombo.SelectString( -1, EnumString.c_str() );
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
					m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr );
					hideAllUncommonControls();
					break;
				
				case 3: // Clip...
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr );
					showClipControls();
					break;

				case 4: // Formula...
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr );
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
					m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr );
					hideAllUncommonControls();
					break;
			}
		}
		else //if( nullptr != m_pLUTMode )
		{
			// No LUT Mode...
			// Deactivate Mouse Proc Func of SVDlgGraph Control...
			m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr);
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
			m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr );
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
		m_pLUTVector->GetArrayValues( byteVector );
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

