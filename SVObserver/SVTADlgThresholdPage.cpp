//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogThresholdPage
//* .File Name       : $Workfile:   SVTADlgThresholdPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 Aug 2013 10:35:06  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVTADlgThresholdPage.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"

#include "SVAutoThresholdEquation.h"
#include "SVDataBuffer.h"
#include "SVFormulaEditorSheet.h"
#include "SVImageProcessingClass.h"
#include "SVIPDoc.h"
#include "SVLowerThresholdEquation.h"
#include "SVObserver.h"
#include "SVThresholdClass.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVUpperThresholdEquation.h"
#include "SVHistogram.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : Class SVToolAdjustmentDialogThresholdPageClass 
//* Note(s)    : Property Page Class
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Message Map Entries
//******************************************************************************
BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogThresholdPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogThresholdPageClass)
	ON_EN_CHANGE(IDC_UPPER_EDIT, OnChangeUpperEdit)
	ON_EN_CHANGE(IDC_LOWER_EDIT, OnChangeLowerEdit)
	ON_BN_CLICKED(IDC_UPPER_CHECK, OnUpperActivateCheck)
	ON_BN_CLICKED(IDC_LOWER_CHECK, OnLowerActivateCheck)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_AUTOTHRESHOLD_CHECK, OnAutoThresholdCheck)
	ON_BN_CLICKED(IDC_WHITE_BACKGROUND_RADIO, OnWhiteBackgroundRadio)
	ON_BN_CLICKED(IDC_BLACK_BACKGROUND_RADIO, OnBlackBackgroundRadio)
	ON_EN_CHANGE(IDC_AUTOTHRESHOLD_EDIT, OnChangeAutothresholdEdit)
	ON_BN_CLICKED(IDC_UT_FORMULA_BUTTON, OnUTFormulaButton)
	ON_BN_CLICKED(IDC_ATM_FORMULA_BUTTON, OnATMFormulaButton)
	ON_BN_CLICKED(IDC_LT_FORMULA_BUTTON, OnLTFormulaButton)
	ON_BN_CLICKED(IDC_USE_EXTERN_ATM_CHECK, OnUseExternATMCheck)
	ON_BN_CLICKED(IDC_USE_EXTERN_LT_CHECK, OnUseExternLTCheck)
	ON_BN_CLICKED(IDC_USE_EXTERN_UT_CHECK, OnUseExternUTCheck)
	ON_MESSAGE(ID_BOUND_CHANGE, OnBoundChange)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
// Constructor(s):
//******************************************************************************

SVToolAdjustmentDialogThresholdPageClass::SVToolAdjustmentDialogThresholdPageClass( SVToolAdjustmentDialogSheetClass* Parent ) : CPropertyPage(SVToolAdjustmentDialogThresholdPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogThresholdPageClass)
	bUseExternATM = FALSE;
	bUseExternLT = FALSE;
	bUseExternUT = FALSE;
	//}}AFX_DATA_INIT

	pUseExternATM = NULL;
	pUseExternLT  = NULL;
	pUseExternUT  = NULL;

	upperThresholdActive = FALSE;
	thresholdActive = FALSE;
	lowerThresholdActive = FALSE;
	upperThres = 0;
	lowerThres = 0;
	bUseAutoThreshold = FALSE;
	dAutoThreshold = 1.0;         // 22 Sep 1999 - frb.
	histState = 0;

	pTool = NULL;
	pUnaryImageOperatorList = NULL;
	pCurrentThreshold = NULL;
	pParentDialog = Parent;
	isEdit = FALSE;

	if( pParentDialog )
	{
		pTool = pParentDialog->GetTool();

		if( pTool )
		{
			SVObjectTypeInfoStruct info;
			info.ObjectType = SVUnaryImageOperatorListObjectType;

			pUnaryImageOperatorList = ( SVUnaryImageOperatorListClass* )SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD )&info );
		
			info.ObjectType = SVUnaryImageOperatorObjectType;
			info.SubType = SVThresholdObjectType;

			pCurrentThreshold = ( SVThresholdClass * )SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &info );

			if( pCurrentThreshold )
			{
				// Try to get Threshold Embeddeds...

				SVObjectTypeInfoStruct infoATM;
				infoATM.ObjectType = SVBoolValueObjectType;
				infoATM.EmbeddedID = SVUseExternalATMObjectGuid;
				pUseExternATM = ( SVBoolValueObjectClass* ) SVSendMessage( pCurrentThreshold, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &infoATM );

				SVObjectTypeInfoStruct infoLT;
				infoLT.ObjectType = SVBoolValueObjectType;
				infoLT.EmbeddedID = SVUseExternalLTObjectGuid;
				pUseExternLT = ( SVBoolValueObjectClass* ) SVSendMessage( pCurrentThreshold, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &infoLT );

				SVObjectTypeInfoStruct infoUT;
				infoUT.ObjectType = SVBoolValueObjectType;
				infoUT.EmbeddedID = SVUseExternalUTObjectGuid;
				pUseExternUT = ( SVBoolValueObjectClass* ) SVSendMessage( pCurrentThreshold, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &infoUT );
			}

		}
	}
}


//******************************************************************************
// Destructor(s):
//******************************************************************************

SVToolAdjustmentDialogThresholdPageClass::~SVToolAdjustmentDialogThresholdPageClass()
{
}

HRESULT SVToolAdjustmentDialogThresholdPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	UpdateData( TRUE ); // get data from dialog

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &(pCurrentThreshold->m_upperThresh), upperThreshold.GetPos() );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &(pCurrentThreshold->m_lowerThresh), lowerThreshold.GetPos() );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &(pCurrentThreshold->m_threshActivate), thresholdActive );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &(pCurrentThreshold->m_upperThreshActivate), upperThresholdActive );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &(pCurrentThreshold->m_lowerThreshActivate), lowerThresholdActive );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &(pCurrentThreshold->m_useExternalLT), bUseExternLT );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &(pCurrentThreshold->m_useExternalUT), bUseExternUT );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &(pCurrentThreshold->m_autoThreshold), bUseAutoThreshold );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &(pCurrentThreshold->m_dAutoThresholdMultiplier), dAutoThreshold );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &(pCurrentThreshold->m_blackBackground), autoThresholdBlackRadio.GetCheck() );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &(pCurrentThreshold->m_useExternalATM), bUseExternATM );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequestMarker();
	}

	if( l_hrOk == S_OK )
	{
		histState = 0;
		if (thresholdActive)
		{
			histState = histogram::shown;
			if (!bUseAutoThreshold)
			{
				histState |= histogram::active;
			}
		}
		pCurrentThreshold->saveHistogram = true;
		l_hrOk = RunOnce( pTool );
		const SVMatroxLongArray & l_val = pCurrentThreshold->GetHistogramValues();
		m_histogram.SetPixelCounts(l_val.begin(), l_val.end());
		m_histogram.PostMessage(ID_BOUND_CHANGE, 1, lowerThreshold.GetPos());
		m_histogram.PostMessage(ID_BOUND_CHANGE, 2, upperThreshold.GetPos()|(histState << 8));
		pCurrentThreshold->saveHistogram = false;
	}

	UpdateData( FALSE );

	return l_hrOk;
}

void SVToolAdjustmentDialogThresholdPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	if( isEdit == FALSE )
	{
		//{{AFX_DATA_MAP(SVToolAdjustmentDialogThresholdPageClass)
	DDX_Control(pDX, IDC_LOWER_EDIT, editLowerThres);
	DDX_Control(pDX, IDC_UPPER_EDIT, editUpperThres);
	DDX_Control(pDX, IDC_AUTOTHRESHOLD_EDIT, editAutoThreshold);
		DDX_Control(pDX, IDC_WHITE_BACKGROUND_RADIO, autoThresholdWhiteRadio);
		DDX_Control(pDX, IDC_BLACK_BACKGROUND_RADIO, autoThresholdBlackRadio);
		DDX_Control(pDX, IDC_AUTOTHRESHOLD_SLIDER, autoThresholdCtrl);
		DDX_Control(pDX, IDC_DIALOGIMAGE, dialogImage);
		DDX_Control(pDX, IDC_LOWER_SLIDER, lowerThreshold);
		DDX_Control(pDX, IDC_UPPER_SLIDER, upperThreshold);
		DDX_Check(pDX, IDC_UPPER_CHECK, upperThresholdActive);
		DDX_Check(pDX, IDC_CHECK1, thresholdActive);
		DDX_Check(pDX, IDC_LOWER_CHECK, lowerThresholdActive);
		DDX_Text(pDX, IDC_WHITE_PIXEL_TEXT, strWhitePixel);
		DDX_Check(pDX, IDC_AUTOTHRESHOLD_CHECK, bUseAutoThreshold);
		DDX_Text(pDX, IDC_AUTOTHRESHOLD_EDIT, csEditAutoThreshold);
	DDX_Text(pDX, IDC_UPPER_EDIT, csUpperThres);
	DDX_Text(pDX, IDC_LOWER_EDIT, csLowerThres);
	DDX_Check(pDX, IDC_USE_EXTERN_ATM_CHECK, bUseExternATM);
	DDX_Check(pDX, IDC_USE_EXTERN_LT_CHECK, bUseExternLT);
	DDX_Check(pDX, IDC_USE_EXTERN_UT_CHECK, bUseExternUT);
	//}}AFX_DATA_MAP
	}
	else
	{
		DDX_Control(pDX, IDC_DIALOGIMAGE, dialogImage);
		DDX_Control(pDX, IDC_AUTOTHRESHOLD_SLIDER, autoThresholdCtrl);
		DDX_Control(pDX, IDC_LOWER_SLIDER, lowerThreshold);
		DDX_Control(pDX, IDC_UPPER_SLIDER, upperThreshold);
		DDX_Text(pDX, IDC_AUTOTHRESHOLD_EDIT, csEditAutoThreshold);
	}
	DDX_Control(pDX, IDC_HISTOGRAM, m_histogram);
}


void SVToolAdjustmentDialogThresholdPageClass::initThreshold()
{
	if( pCurrentThreshold )
	{
		SetInspectionData();

		if( pCurrentThreshold && pCurrentThreshold->getReferenceImage() )
		{
			// Calculate And Show White Pixels...
			// &&&
			SVMatroxImageResult histResultID;
			SVMatroxLongArray l_alHistValues;
			SVDataBufferInfoClass svData;
			
			l_alHistValues.resize(256);
			svData.Length = 256;
			svData.Type = SVDataBufferInfoClass::SVHistResult;
			svData.HBuffer.milResult = histResultID;
			if ( S_OK == SVImageProcessingClass::Instance().CreateDataBuffer( &svData ) )
			{
				histResultID = svData.HBuffer.milResult;
			}
			
			SVSmartHandlePointer ImageHandle;
			
			if ( pCurrentThreshold->getReferenceImage()->GetImageHandle( ImageHandle ) && !( ImageHandle.empty() ) )
			{
				SVImageBufferHandleImage l_MilHandle;
				ImageHandle->GetData( l_MilHandle );
				
				SVMatroxImageInterface::SVStatusCode l_Code;
				l_Code = SVMatroxImageInterface::Histogram( histResultID, l_MilHandle.GetBuffer() );
				l_Code = SVMatroxImageInterface::GetResult( histResultID, l_alHistValues );
				l_Code = SVMatroxImageInterface::Destroy( histResultID );
				
				strWhitePixel.Format( "White Pixel: %d", l_alHistValues[ 255 ] );
			}
		}

		dialogImage.refresh();

		UpdateData( FALSE ); // set data to dialog
	}
}



BOOL SVToolAdjustmentDialogThresholdPageClass::OnSetActive() 
{
	SetInspectionData();

	return CPropertyPage::OnSetActive();
}


BOOL SVToolAdjustmentDialogThresholdPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	SetTaskObject( pTool );

	if( pTool )
	{
		if( pCurrentThreshold && pCurrentThreshold->getReferenceImage() )
		{
			dialogImage.UpdateImageInfo( pCurrentThreshold->getReferenceImage() );
			dialogImage.refresh();

			// Setup sliders...
			upperThreshold.SetRange( 0, 255, TRUE );
			lowerThreshold.SetRange( 0, 255, TRUE );
			lowerThreshold.SetTic( 127 );
			upperThreshold.SetTic( 127 );
			lowerThreshold.SetPageSize( 1 );
			upperThreshold.SetPageSize( 1 );

			// Get current threshold values...
			pCurrentThreshold->m_upperThresh.GetValue( up );
			pCurrentThreshold->m_lowerThresh.GetValue( lo );

			upperThres = up;
			lowerThres = lo;

			upperThreshold.SetPos( ( int ) upperThres );
			lowerThreshold.SetPos( ( int ) lowerThres );
			
			csLowerThres.Format( _T("%ld"), lowerThres );
			csUpperThres.Format( _T("%ld"), upperThres );
			
			// Get other flags...
			pCurrentThreshold->m_threshActivate.GetValue( thresholdActive );
			pCurrentThreshold->m_upperThreshActivate.GetValue( upperThresholdActive );
			pCurrentThreshold->m_lowerThreshActivate.GetValue( lowerThresholdActive );
			pCurrentThreshold->m_useExternalLT.GetValue( bUseExternLT );
			pCurrentThreshold->m_useExternalUT.GetValue( bUseExternUT );
			pCurrentThreshold->m_autoThreshold.GetValue( bUseAutoThreshold );
			pCurrentThreshold->m_dAutoThresholdMultiplier.GetValue( dAutoThreshold );
			pCurrentThreshold->m_useExternalATM.GetValue( bUseExternATM );

			// setup auto threshold slider...
			autoThresholdCtrl.SetRange( 0, 20000, TRUE );
			autoThresholdCtrl.SetTic( 10000 );

			autoThresholdCtrl.SetPos( ( int ) ( dAutoThreshold * 10000.0 ) );
			csEditAutoThreshold.Format( _T("%.4f") ,dAutoThreshold);

			BOOL bBlackBackground;
			pCurrentThreshold->m_blackBackground.GetValue( bBlackBackground );
			autoThresholdBlackRadio.SetCheck( bBlackBackground );
			autoThresholdWhiteRadio.SetCheck( ! bBlackBackground );

			UpdateData( FALSE ); // set data to dialog
			OnCheck1();	// check thresholdActive state
		}
		else
		{
			// Deacivate all threshold page controls...
			CWnd* pWnd;
			if( pWnd = GetDlgItem( IDC_DIALOGIMAGE ) )
				pWnd->EnableWindow( FALSE );
			if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
				pWnd->EnableWindow( FALSE );
			if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
				pWnd->EnableWindow( FALSE );
			if( pWnd = GetDlgItem( IDC_UPPER_CHECK ) )
				pWnd->EnableWindow( FALSE );
			if( pWnd = GetDlgItem( IDC_CHECK1 ) )
				pWnd->EnableWindow( FALSE );
			if( pWnd = GetDlgItem( IDC_LOWER_CHECK ) )
				pWnd->EnableWindow( FALSE );
			if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
				pWnd->EnableWindow( FALSE );
			if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
				pWnd->EnableWindow( FALSE );
			if( pWnd = GetDlgItem( IDC_WHITE_PIXEL_TEXT ) )
				pWnd->EnableWindow( FALSE );
			if( pWnd = GetDlgItem( IDC_USE_EXTERN_LT_CHECK ) )
				pWnd->EnableWindow( FALSE );
			if( pWnd = GetDlgItem( IDC_USE_EXTERN_UT_CHECK ) )
				pWnd->EnableWindow( FALSE );
			
			// Hide Formula Button
			if( pWnd = GetDlgItem( IDC_LT_FORMULA_BUTTON ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_UT_FORMULA_BUTTON ) )
				pWnd->ShowWindow( SW_HIDE );

			// Hide auto threshold controls...
			if( pWnd = GetDlgItem( IDC_AUTOTHRESHOLD_EDIT ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_BLACK_BACKGROUND_RADIO ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_WHITE_BACKGROUND_RADIO ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_AUTOTHRESHOLD_SLIDER ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_USE_EXTERN_ATM_CHECK ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_ATM_FORMULA_BUTTON ) )
				pWnd->ShowWindow( SW_HIDE );

		}
	}
	else
		// Not valid call...
		if( GetParent() )
			GetParent()->SendMessage( WM_CLOSE );
		else
			SendMessage( WM_CLOSE );

	return TRUE;
}



void SVToolAdjustmentDialogThresholdPageClass::OnChangeUpperEdit() 
{
	UpdateData( TRUE ); // get data of dialog
	//
	// Check threshold limits.
	//
	_stscanf( (LPCTSTR)csUpperThres, _T("%ld"), &upperThres );

	if(upperThres < 0L)
	{
		upperThres = 0L;
		csUpperThres.Format( _T("%ld") , upperThres );
	}
	else if(upperThres > 255L)
	{
		upperThres = 255L;
		csUpperThres.Format( _T("%ld") , upperThres );
	}

	m_histogram.PostMessage(ID_BOUND_CHANGE, 2, upperThres|(histState << 8));
	upperThreshold.SetPos( ( int ) upperThres );
	isEdit = TRUE;	
	UpdateData( FALSE ); // set data to dialog
	isEdit = FALSE;	
	initThreshold();
}


void SVToolAdjustmentDialogThresholdPageClass::OnChangeLowerEdit() 
{
	UpdateData( TRUE ); // get data of dialog
	//
	// Check threshold limits.
	//
	_stscanf( (LPCTSTR)csLowerThres, "%ld", &lowerThres );

	if(lowerThres < 0L)
	{
		lowerThres = 0L;
		csLowerThres.Format( _T("%ld") , lowerThres );	
	}
	else if(lowerThres > 255L)
	{
		lowerThres = 255L;
		csLowerThres.Format( _T("%ld") , lowerThres );	
	}
	m_histogram.PostMessage(ID_BOUND_CHANGE, 1, lowerThres|(histState << 8));

	lowerThreshold.SetPos( ( int ) lowerThres );	
	isEdit = TRUE;	
	UpdateData( FALSE ); // set data to dialog
	isEdit = FALSE;
	initThreshold();
}


void SVToolAdjustmentDialogThresholdPageClass::OnChangeAutothresholdEdit() 
{
	UpdateData( TRUE ); // get data of dialog

	//
	// Check auto threshold limits.
	//
	_stscanf( (LPCTSTR)csEditAutoThreshold, "%lf", &dAutoThreshold );

	if(dAutoThreshold < 0.0)
	{
		dAutoThreshold = 0.0;
		csEditAutoThreshold.Format( _T("%.4f") ,dAutoThreshold);
	}
	else if(dAutoThreshold > 2.0)
	{
		dAutoThreshold = 2.0;
		csEditAutoThreshold.Format( _T("%.4f") ,dAutoThreshold);
	}
    //
	// Update controls.
	//
	autoThresholdCtrl.SetPos( ( int ) ( dAutoThreshold * 10000.0 ) );
	//autoThresholdCtrl.UpdateWindow();

	isEdit = TRUE;	
	UpdateData( FALSE ); // set data to dialog
	isEdit = FALSE;	

	//
	// Show the effects of new threshold value.
	//
	initThreshold();
}



void SVToolAdjustmentDialogThresholdPageClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData( TRUE ); // get data of dialog	
	
	if( &upperThreshold == ( CSliderCtrl* ) pScrollBar )
	{
		upperThres = ( long ) upperThreshold.GetPos();		
		m_histogram.PostMessage(ID_BOUND_CHANGE, 2, upperThres|(histState << 8));
		csUpperThres.Format( _T("%ld") , upperThres );
	}

	if( &lowerThreshold == ( CSliderCtrl* ) pScrollBar )
	{
		lowerThres = ( long ) lowerThreshold.GetPos();	
		m_histogram.PostMessage(ID_BOUND_CHANGE, 1, lowerThres|(histState << 8));
		csLowerThres.Format( _T("%ld") , lowerThres );	
	}

	if( &autoThresholdCtrl == ( CSliderCtrl* ) pScrollBar )
	{
		dAutoThreshold = ( ( double ) autoThresholdCtrl.GetPos() ) / 10000;		
		csEditAutoThreshold.Format( _T("%.4f") , dAutoThreshold );
	}

	UpdateData( FALSE ); // set data to dialog
	initThreshold();
	
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpperActivateCheck
////////////////////////////////////////////////////////////////////////////////
void SVToolAdjustmentDialogThresholdPageClass::OnUpperActivateCheck() 
{
	// Activate or Deactivate upperThresholding
	UpdateData( TRUE ); // get data of dialog
	
	CWnd* pWnd;
	if( upperThresholdActive == TRUE )
	{
		if( bUseExternUT )
		{
			// Disable and hide slider...
			if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Enable and show formula button...
			if( pWnd = GetDlgItem( IDC_UT_FORMULA_BUTTON ) )
			{
				pWnd->ShowWindow( SW_SHOW );
				pWnd->EnableWindow();
			}
			m_histogram.Disable(histogram::high);
			// Update Edit with formula
			UpdateUpperThresholdFromFoumula();
		}
		else
		{
			// Disable and hide formula button...
			if( pWnd = GetDlgItem( IDC_UT_FORMULA_BUTTON ) )
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Enable and show slider...
			if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
			{
				pWnd->ShowWindow( SW_SHOW );
				pWnd->EnableWindow();
			}

			// Setup slider...
			upperThres = ( DWORD ) up;
			upperThreshold.SetPos( ( int ) upperThres );
			m_histogram.Enable(histogram::high);
			csUpperThres.Format( _T("%ld") , upperThres );
		}

		// Enable and show edit...
		if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
		{
			pWnd->ShowWindow( SW_SHOW );
			pWnd->EnableWindow(bUseExternUT ? FALSE : TRUE);
		}

		// Enable and show use extern UT check..
		if( pWnd = GetDlgItem( IDC_USE_EXTERN_UT_CHECK ) )
		{
			pWnd->ShowWindow( SW_SHOW );
			pWnd->EnableWindow();
		}

	}
	else
	{
		up = ( int ) upperThres;
		upperThres = 255;
		upperThreshold.SetPos( ( int ) upperThres );

		if( bUseExternUT )
		{
			// Hide and disable slider...
			if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Show disabled formula button...
			if( pWnd = GetDlgItem( IDC_UT_FORMULA_BUTTON ) )
			{
				pWnd->EnableWindow( FALSE );
				pWnd->ShowWindow( SW_SHOW );
			}
		}
		else
		{
			// Hide and disable formula button...
			if( pWnd = GetDlgItem( IDC_UT_FORMULA_BUTTON ) )
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Show disabled slider...
			if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
			{
				pWnd->EnableWindow( FALSE );
				pWnd->ShowWindow( SW_SHOW );
			}
		}

		// Disable edit...
		if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
			pWnd->EnableWindow( FALSE );

		// Disable use extern UT check..
		if( pWnd = GetDlgItem( IDC_USE_EXTERN_UT_CHECK ) )
			pWnd->EnableWindow( FALSE );

		m_histogram.Disable(histogram::high);
	}

	UpdateData( FALSE ); // set data to dialog
	initThreshold();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnLowerActivateCheck
////////////////////////////////////////////////////////////////////////////////
void SVToolAdjustmentDialogThresholdPageClass::OnLowerActivateCheck() 
{
	// Activate or Deactivate lowerThresholding
	UpdateData( TRUE ); // get data of dialog
	
	CWnd* pWnd;
	if( lowerThresholdActive == TRUE )
	{
		if( bUseExternLT )
		{
			// Disable and hide slider...
			if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Enable and show formula button...
			if( pWnd = GetDlgItem( IDC_LT_FORMULA_BUTTON ) )
			{
				pWnd->ShowWindow( SW_SHOW );
				pWnd->EnableWindow();
			}
			m_histogram.Disable(histogram::low);
			// Update lower Threshold Edit
			UpdateLowerThresholdFromFormula();
		}
		else
		{
			// Disable and hide formula button...
			if( pWnd = GetDlgItem( IDC_LT_FORMULA_BUTTON ) )
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Enable and show slider...
			if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
			{
				pWnd->ShowWindow( SW_SHOW );
				pWnd->EnableWindow();
			}

			// Setup slider...
			lowerThres = ( DWORD ) lo;
			lowerThreshold.SetPos( ( int ) lowerThres );
			m_histogram.Enable(histogram::low);
			// Setup Edit
			csLowerThres.Format( _T("%ld") , lowerThres );
		}

		// Enable and show edit...
		if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
		{
			pWnd->ShowWindow( SW_SHOW );
			pWnd->EnableWindow(bUseExternLT ? FALSE : TRUE );
		}

		// Enable and show use extern LT check..
		if( pWnd = GetDlgItem( IDC_USE_EXTERN_LT_CHECK ) )
		{
			pWnd->ShowWindow( SW_SHOW );
			pWnd->EnableWindow();
		}
	}
	else
	{
		lo = ( int ) lowerThres;
		lowerThres = 0;
		lowerThreshold.SetPos( ( int ) lowerThres );

		if( bUseExternLT )
		{
			// Hide and disable slider...
			if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Show disabled formula button...
			if( pWnd = GetDlgItem( IDC_LT_FORMULA_BUTTON ) )
			{
				pWnd->EnableWindow( FALSE );
				pWnd->ShowWindow( SW_SHOW );
			}
		}
		else
		{
			// Hide and disable formula button...
			if( pWnd = GetDlgItem( IDC_LT_FORMULA_BUTTON ) )
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Show disabled slider...
			if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
			{
				pWnd->EnableWindow( FALSE );
				pWnd->ShowWindow( SW_SHOW );
			}
		}

		// Disable edit...
		if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
			pWnd->EnableWindow( FALSE );

		// Disable use extern LT check..
		if( pWnd = GetDlgItem( IDC_USE_EXTERN_LT_CHECK ) )
			pWnd->EnableWindow( FALSE );
		m_histogram.Disable(histogram::low);
	}

	UpdateData( FALSE ); // set data to dialog
	initThreshold();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnCheck1
//	Activate or Deactivate Thresholding
////////////////////////////////////////////////////////////////////////////////
void SVToolAdjustmentDialogThresholdPageClass::OnCheck1() 
{
	UpdateData( TRUE ); // get data of dialog

	CWnd* pWnd;
	if( thresholdActive == TRUE )
	{
		if( bUseAutoThreshold )
		{
			// Hide manual threshold controls...
			if( pWnd = GetDlgItem( IDC_UPPER_CHECK ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_LOWER_CHECK ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_USE_EXTERN_LT_CHECK ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_USE_EXTERN_UT_CHECK ) )
				pWnd->ShowWindow( SW_HIDE );
			// Hide Formula Button
			if( pWnd = GetDlgItem( IDC_LT_FORMULA_BUTTON ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_UT_FORMULA_BUTTON ) )
				pWnd->ShowWindow( SW_HIDE );

			// Show auto threshold controls...
			if( pWnd = GetDlgItem( IDC_AUTOTHRESHOLD_EDIT ) )
				pWnd->ShowWindow( SW_SHOW );
			if( pWnd = GetDlgItem( IDC_BLACK_BACKGROUND_RADIO ) )
				pWnd->ShowWindow( SW_SHOW );
			if( pWnd = GetDlgItem( IDC_WHITE_BACKGROUND_RADIO ) )
				pWnd->ShowWindow( SW_SHOW );
			if( pWnd = GetDlgItem( IDC_USE_EXTERN_ATM_CHECK ) )
				pWnd->ShowWindow( SW_SHOW );
			if( bUseExternATM )
			{
				if( pWnd = GetDlgItem( IDC_AUTOTHRESHOLD_SLIDER ) )
					pWnd->ShowWindow( SW_HIDE );

				if( pWnd = GetDlgItem( IDC_ATM_FORMULA_BUTTON ) )
					pWnd->ShowWindow( SW_SHOW );
			}
			else
			{
				if( pWnd = GetDlgItem( IDC_ATM_FORMULA_BUTTON ) )
					pWnd->ShowWindow( SW_HIDE );

				if( pWnd = GetDlgItem( IDC_AUTOTHRESHOLD_SLIDER ) )
					pWnd->ShowWindow( SW_SHOW );
			}

			// Save state
			lo = ( int ) lowerThres;
			up = ( int ) upperThres;

		}
		else
		{
			// Hide auto threshold controls...
			if( pWnd = GetDlgItem( IDC_AUTOTHRESHOLD_EDIT ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_BLACK_BACKGROUND_RADIO ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_WHITE_BACKGROUND_RADIO ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_AUTOTHRESHOLD_SLIDER ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_USE_EXTERN_ATM_CHECK ) )
				pWnd->ShowWindow( SW_HIDE );
			if( pWnd = GetDlgItem( IDC_ATM_FORMULA_BUTTON ) )
				pWnd->ShowWindow( SW_HIDE );


			// Show manual threshold controls...
			if( pWnd = GetDlgItem( IDC_UPPER_CHECK ) )
				pWnd->ShowWindow( SW_SHOW );
			if( pWnd = GetDlgItem( IDC_LOWER_CHECK ) )
				pWnd->ShowWindow( SW_SHOW );
			if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
				pWnd->ShowWindow( SW_SHOW );
			if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
				pWnd->ShowWindow( SW_SHOW );
			if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
				pWnd->ShowWindow( SW_SHOW );
			if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
				pWnd->ShowWindow( SW_SHOW );
			if( pWnd = GetDlgItem( IDC_USE_EXTERN_LT_CHECK ) )
				pWnd->ShowWindow( SW_SHOW );
			if( pWnd = GetDlgItem( IDC_USE_EXTERN_UT_CHECK ) )
				pWnd->ShowWindow( SW_SHOW );


			lo = ( int ) lowerThres;
			up = ( int ) upperThres;

			OnLowerActivateCheck();	// check lowerThresholdActive state
			OnUpperActivateCheck(); // check upperThresholdActive state

			lowerThreshold.SetPos(lo);
			upperThreshold.SetPos(up);

		}
	}
	else
	{
		// Hide manual threshold controls...
		if( pWnd = GetDlgItem( IDC_UPPER_CHECK ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_LOWER_CHECK ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_USE_EXTERN_LT_CHECK ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_USE_EXTERN_UT_CHECK ) )
			pWnd->ShowWindow( SW_HIDE );
		// Hide Formula Button
		if( pWnd = GetDlgItem( IDC_LT_FORMULA_BUTTON ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_UT_FORMULA_BUTTON ) )
			pWnd->ShowWindow( SW_HIDE );


		// Hide auto threshold controls...
		if( pWnd = GetDlgItem( IDC_AUTOTHRESHOLD_EDIT ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_BLACK_BACKGROUND_RADIO ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_WHITE_BACKGROUND_RADIO ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_AUTOTHRESHOLD_SLIDER ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_USE_EXTERN_ATM_CHECK ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_ATM_FORMULA_BUTTON ) )
			pWnd->ShowWindow( SW_HIDE );

	}

	initThreshold();
}

void SVToolAdjustmentDialogThresholdPageClass::OnAutoThresholdCheck() 
{
	OnCheck1();
}

void SVToolAdjustmentDialogThresholdPageClass::OnWhiteBackgroundRadio() 
{
	UpdateData( TRUE ); // get data of dialog
	CheckRadioButton( IDC_WHITE_BACKGROUND_RADIO, IDC_BLACK_BACKGROUND_RADIO, IDC_WHITE_BACKGROUND_RADIO );
	initThreshold();
}

void SVToolAdjustmentDialogThresholdPageClass::OnBlackBackgroundRadio() 
{
	UpdateData( TRUE ); // get data of dialog
	CheckRadioButton( IDC_WHITE_BACKGROUND_RADIO, IDC_BLACK_BACKGROUND_RADIO, IDC_BLACK_BACKGROUND_RADIO );
	initThreshold();
}

void SVToolAdjustmentDialogThresholdPageClass::OnUTFormulaButton() 
{
	UpdateData( TRUE ); // get data of dialog

	SVObjectTypeInfoStruct info;
	info.ObjectType = SVEquationObjectType;
	info.SubType    = SVUpperThresholdEquationObjectType;
	SVUpperThresholdEquationClass* pEquation = ( SVUpperThresholdEquationClass* ) SVSendMessage( pCurrentThreshold, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &info );
	if( pEquation )
	{
		CString strCaption = pEquation->GetName();
		strCaption += _T( " Formula" );
		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEquation );

		dlg.DoModal();

		long l_lResult = (long)pEquation->GetYACCResult();
		csUpperThres.Format( _T("%ld") , l_lResult );

		UpdateData(FALSE);

		initThreshold();
	}
}

void SVToolAdjustmentDialogThresholdPageClass::UpdateLowerThresholdFromFormula()
{
	SVObjectTypeInfoStruct info;
	info.ObjectType = SVEquationObjectType;
	info.SubType    = SVLowerThresholdEquationObjectType;
	SVLowerThresholdEquationClass* pEquation = ( SVLowerThresholdEquationClass* ) SVSendMessage( pCurrentThreshold, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &info );
	if( pEquation )
	{
		long l_lResult = (long)pEquation->GetYACCResult();
		csLowerThres.Format( _T("%ld") , l_lResult );
	}
}

void SVToolAdjustmentDialogThresholdPageClass::UpdateUpperThresholdFromFoumula()
{
	SVObjectTypeInfoStruct info;
	info.ObjectType = SVEquationObjectType;
	info.SubType    = SVUpperThresholdEquationObjectType;
	SVUpperThresholdEquationClass* pEquation = ( SVUpperThresholdEquationClass* ) SVSendMessage( pCurrentThreshold, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &info );
	if( pEquation )
	{
		long l_lResult = (long)pEquation->GetYACCResult();
		csUpperThres.Format( _T("%ld") , l_lResult );
	}
}

void SVToolAdjustmentDialogThresholdPageClass::OnATMFormulaButton() 
{
	UpdateData( TRUE ); // get data of dialog

	SVObjectTypeInfoStruct info;
	info.ObjectType = SVEquationObjectType;
	info.SubType    = SVAutoThresholdEquationObjectType;
	SVAutoThresholdEquationClass* pEquation = ( SVAutoThresholdEquationClass* ) SVSendMessage( pCurrentThreshold, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &info );
	if( pEquation )
	{
		CString strCaption = pEquation->GetName();
		strCaption += _T( " Formula" );
		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEquation );

		dlg.DoModal();

		initThreshold();
	}
}

void SVToolAdjustmentDialogThresholdPageClass::OnLTFormulaButton() 
{
	UpdateData( TRUE ); // get data of dialog

	SVObjectTypeInfoStruct info;
	info.ObjectType = SVEquationObjectType;
	info.SubType    = SVLowerThresholdEquationObjectType;
	SVLowerThresholdEquationClass* pEquation = ( SVLowerThresholdEquationClass* ) SVSendMessage( pCurrentThreshold, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &info );
	if( pEquation )
	{
		CString strCaption = pEquation->GetName();
		strCaption += _T( " Formula" );
		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEquation );

		dlg.DoModal();

		long l_lResult = (long)pEquation->GetYACCResult();
		csLowerThres.Format( _T("%ld") , l_lResult );

		UpdateData(FALSE);
		initThreshold();
	}
}

void SVToolAdjustmentDialogThresholdPageClass::OnUseExternATMCheck() 
{
	UpdateData( TRUE ); // get data of dialog
	OnCheck1();	
}

void SVToolAdjustmentDialogThresholdPageClass::OnUseExternLTCheck() 
{
	UpdateData( TRUE ); // get data of dialog
	OnCheck1();	
}

void SVToolAdjustmentDialogThresholdPageClass::OnUseExternUTCheck() 
{
	UpdateData( TRUE ); // get data of dialog	
	OnCheck1();
}

LRESULT SVToolAdjustmentDialogThresholdPageClass::OnBoundChange(WPARAM wParam, LPARAM lParam)
{
	//UpdateData(TRUE);
	switch ( lParam )
	{
	case 1:
		lowerThres = wParam;
		csLowerThres.Format( _T("%ld") , lowerThres );
		lowerThreshold.SetPos(lowerThres);
		break;
	case 2:
		upperThres = wParam;
		csUpperThres.Format( _T("%ld") , upperThres );
		upperThreshold.SetPos(upperThres);
		break;
	}
	UpdateData(FALSE);
	initThreshold();
	//OnCheck1();
	return TRUE;
}


BOOL SVToolAdjustmentDialogThresholdPageClass::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CRect rect;
	m_histogram.GetWindowRect(&rect);
	if (rect.PtInRect(pt))
	{
		m_histogram.SetFocus();
	}
	else
	{
		return CPropertyPage::OnMouseWheel(nFlags, zDelta, pt);
	}
	return TRUE;
}




//******************************************************************************
//* FUNCTION IMPLEMENTATION(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : DDX_SVThresholdText
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void AFXAPI DDX_SVThresholdText( CDataExchange* pDX, int nIDC, DWORD& Value )
{
	HWND hWndCtrl = pDX->PrepareEditCtrl( nIDC );
	if ( pDX->m_bSaveAndValidate )
	{
		if ( !GetThreshold( hWndCtrl, Value ) )
		{
			AfxMessageBox( "Out of Range!\n(0 ... 255)" );
			pDX->Fail();
		}
	}
	else
	{
		SetThreshold( hWndCtrl, Value );
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetThreshold
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL GetThreshold( CWnd* pWnd, DWORD& Value )
{
	ASSERT( pWnd != NULL );
	return GetThreshold( pWnd->m_hWnd, Value );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetThreshold
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL GetThreshold( HWND hWnd, DWORD& Value )
{
	TCHAR szWindowText[20];
	DWORD con;

	::GetWindowText( hWnd, szWindowText, 19 );
	
	// only numbers be allowed!
	for( TCHAR* index = szWindowText; *index != 0;  index++ )
		if( ( *index < '0' ) || ( *index > '9') )
			return FALSE;
	
	if( ( con = _ttol( szWindowText ) ) > 255 )
		return FALSE;

	Value = con;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SetThreshold
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SetThreshold( CWnd* pWnd, DWORD Value )
{
	ASSERT( pWnd != NULL );
	SetThreshold( pWnd->m_hWnd, Value );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SetThreshold
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SetThreshold( HWND hWnd, DWORD Value )
{
	CString str;
	str.Format(_T("%lu"), Value);
	::SetWindowText( hWnd, str.GetBufferSetLength(20) );
}











//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgThresholdPage.cpp_v  $
 * 
 *    Rev 1.2   13 Aug 2013 10:35:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVThresholdClass.h instead of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Jul 2013 14:01:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  850
 * SCR Title:  Fix Bug in Threshold where edit box is not updated or grayed when appropriate
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Upper and Lower Threshold Edit boxs so they have updated information from the formula when formula is  used.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:23:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   02 Jul 2012 17:42:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   24 Jan 2012 10:35:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  750
 * SCR Title:  Fix issue with the threshold showing the wrong value
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issue with the threshold value not matching the edit box when activating/deactivating	the threshold.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   20 May 2011 09:54:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  716
 * SCR Title:  Fix the Histogram control interaction with threshold tab.
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Fixed issues with switching tabs to and from threshold page with histogram.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   07 Apr 2011 16:39:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   10 Feb 2011 15:15:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   27 Jan 2011 12:02:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   08 Dec 2010 13:39:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   05 Nov 2010 09:32:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   04 Nov 2010 14:15:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   04 Feb 2009 11:15:20   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  625
 * SCR Title:  Functional Requirement for Tool Adjustment Dialog - Histogram Control
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added enabling/disabling bar dragging on the histogram control depending on the "active" and "use formula" selections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   21 Oct 2008 15:00:02   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  625
 * SCR Title:  Functional Requirement for Tool Adjustment Dialog - Histogram Control
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added Histogram Control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   25 Jul 2007 07:48:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   16 Aug 2005 09:45:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   12 Aug 2005 15:13:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed casting problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   09 Aug 2005 08:25:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   19 May 2005 14:52:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added m_ prefix to member variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   17 Feb 2005 16:04:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new reset / create methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   08 Jul 2003 09:25:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated initThreshold method to include changes in SVImageProcessingClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   22 Apr 2003 16:21:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   30 Jan 2003 16:45:58   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated initThreshold method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   20 Nov 2002 13:48:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed #include "SVSystem.h"
 *  Removed #include "SVEvent.h"
 * Added #include "SVDataBuffer.h"
 *  Add ResultDataIndex and ResultImageIndex
 * changes to initThreshold
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   30 Jan 2002 16:15:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  278
 * SCR Title:  Error message is displayed when removing result objects used by other tools
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified ::refresh and ::initMask and ::initThreshold to validate the toolset before blindly running it.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   23 Mar 2000 16:45:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to work with new In Place Image Operator List
 * Revised to Run complete ToolSet istead of just the Operator List.
 * Revised to do a fast image and result view update, so 
 * changes can be seen immediately.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:43:14   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Oct 1999 17:30:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  48
 * SCR Title:  Threshold Page Formula
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Check in. Revised Threshold adjustment page class.
 * (Moved from SVToolAdjustmentDialog)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
//** EOF **
