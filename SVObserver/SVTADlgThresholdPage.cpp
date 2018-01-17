//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogThresholdPage
//* .File Name       : $Workfile:   SVTADlgThresholdPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgThresholdPage.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "InspectionEngine/SVAutoThresholdEquation.h"
#include "InspectionEngine/SVDataBuffer.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVIPDoc.h"
#include "InspectionEngine/SVLowerThresholdEquation.h"
#include "SVObserver.h"
#include "InspectionEngine/SVThresholdClass.h"
#include "InspectionEngine/SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "InspectionEngine/SVUpperThresholdEquation.h"
#include "SVMFCControls\SVHistogram.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static long MaxThresholdValue = 255L;
static long MaxValues = 256;
static double ScaleFactor = 10000.0;
static long SliderRange = 20000;
static long SliderTic = SliderRange / 2;

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

SVToolAdjustmentDialogThresholdPageClass::SVToolAdjustmentDialogThresholdPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent ) 
: CPropertyPage(SVToolAdjustmentDialogThresholdPageClass::IDD)
, m_pUseExternATM(nullptr)
, m_pUseExternLT(nullptr)
, m_pUseExternUT(nullptr)
, m_pTool(nullptr)
, m_pCurrentThreshold(nullptr)
, m_pParentDialog(Parent)
, m_upperThresholdActive(false)
, m_thresholdActive(false)
, m_lowerThresholdActive(false)
, m_upperThres(0)
, m_lowerThres(0)
, m_bUseAutoThreshold(false)
, m_dAutoThreshold(1.0)
, m_histState(0)
, m_isEdit(false)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogThresholdPageClass)
	m_bUseExternATM = false;
	m_bUseExternLT = false;
	m_bUseExternUT = false;
	//}}AFX_DATA_INIT

	if (m_pParentDialog)
	{
		m_pTool = m_pParentDialog->GetTool();

		if (m_pTool)
		{
			SvDef::SVObjectTypeInfoStruct info(SvDef::SVUnaryImageOperatorObjectType, SvDef::SVThresholdObjectType);
			m_pCurrentThreshold = dynamic_cast<SVThresholdClass *>(m_pTool->getFirstObject(info));

			if( m_pCurrentThreshold )
			{
				// Try to get Threshold Embeddeds...

				SvDef::SVObjectTypeInfoStruct infoATM;
				infoATM.ObjectType = SvDef::SVValueObjectType;
				infoATM.SubType = SvDef::SVBoolValueObjectType;
				infoATM.EmbeddedID = SVUseExternalATMObjectGuid;
				m_pUseExternATM = dynamic_cast<SVBoolValueObjectClass*>(m_pCurrentThreshold->getFirstObject(infoATM));

				SvDef::SVObjectTypeInfoStruct infoLT;
				infoLT.ObjectType = SvDef::SVValueObjectType;
				infoLT.SubType = SvDef::SVBoolValueObjectType;
				infoLT.EmbeddedID = SVUseExternalLTObjectGuid;
				m_pUseExternLT = dynamic_cast<SVBoolValueObjectClass*>(m_pCurrentThreshold->getFirstObject(infoLT));

				SvDef::SVObjectTypeInfoStruct infoUT;
				infoUT.ObjectType = SvDef::SVValueObjectType;
				infoUT.SubType = SvDef::SVBoolValueObjectType;
				infoUT.EmbeddedID = SVUseExternalUTObjectGuid;
				m_pUseExternUT = dynamic_cast<SVBoolValueObjectClass*>(m_pCurrentThreshold->getFirstObject(infoUT));
			}
		}
	}
}

SVToolAdjustmentDialogThresholdPageClass::~SVToolAdjustmentDialogThresholdPageClass()
{
}

HRESULT SVToolAdjustmentDialogThresholdPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	UpdateData(true); // get data from dialog

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequest( &(m_pCurrentThreshold->m_upperThresh), m_upperThreshold.GetPos() );
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequest( &(m_pCurrentThreshold->m_lowerThresh), m_lowerThreshold.GetPos() );
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequest( &(m_pCurrentThreshold->m_threshActivate), m_thresholdActive );
	}

	if (S_OK ==  l_hrOk)
	{
		l_hrOk = AddInputRequest( &(m_pCurrentThreshold->m_upperThreshActivate), m_upperThresholdActive );
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequest( &(m_pCurrentThreshold->m_lowerThreshActivate), m_lowerThresholdActive );
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequest( &(m_pCurrentThreshold->m_useExternalLT), m_bUseExternLT );
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequest( &(m_pCurrentThreshold->m_useExternalUT), m_bUseExternUT );
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequest( &(m_pCurrentThreshold->m_autoThreshold), m_bUseAutoThreshold );
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequest( &(m_pCurrentThreshold->m_dAutoThresholdMultiplier), m_dAutoThreshold );
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequest( &(m_pCurrentThreshold->m_blackBackground), m_autoThresholdBlackRadio.GetCheck() );
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequest( &(m_pCurrentThreshold->m_useExternalATM), m_bUseExternATM );
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequestMarker();
	}

	if (S_OK == l_hrOk)
	{
		m_histState = 0;
		if (m_thresholdActive)
		{
			m_histState = histogram::shown;
			if (!m_bUseAutoThreshold)
			{
				m_histState |= histogram::active;
			}
		}
		m_pCurrentThreshold->saveHistogram = true;
		l_hrOk = RunOnce( m_pTool->GetUniqueObjectID() );
		const SVMatroxLongArray & l_val = m_pCurrentThreshold->GetHistogramValues();
		m_histogram.SetPixelCounts(l_val.begin(), l_val.end());
		m_histogram.PostMessage(ID_BOUND_CHANGE, 1, m_lowerThreshold.GetPos());
		m_histogram.PostMessage(ID_BOUND_CHANGE, 2, m_upperThreshold.GetPos()|(m_histState << 8));
		m_pCurrentThreshold->saveHistogram = false;
	}
	UpdateData(false);

	return l_hrOk;
}

void SVToolAdjustmentDialogThresholdPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	if (FALSE == m_isEdit)
	{
		//{{AFX_DATA_MAP(SVToolAdjustmentDialogThresholdPageClass)
		DDX_Control(pDX, IDC_LOWER_EDIT, m_editLowerThres);
		DDX_Control(pDX, IDC_UPPER_EDIT, m_editUpperThres);
		DDX_Control(pDX, IDC_AUTOTHRESHOLD_EDIT, m_editAutoThreshold);
		DDX_Control(pDX, IDC_WHITE_BACKGROUND_RADIO, m_autoThresholdWhiteRadio);
		DDX_Control(pDX, IDC_BLACK_BACKGROUND_RADIO, m_autoThresholdBlackRadio);
		DDX_Control(pDX, IDC_AUTOTHRESHOLD_SLIDER, m_autoThresholdCtrl);
		DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
		DDX_Control(pDX, IDC_LOWER_SLIDER, m_lowerThreshold);
		DDX_Control(pDX, IDC_UPPER_SLIDER, m_upperThreshold);
		DDX_Check(pDX, IDC_UPPER_CHECK, m_upperThresholdActive);
		DDX_Check(pDX, IDC_CHECK1, m_thresholdActive);
		DDX_Check(pDX, IDC_LOWER_CHECK, m_lowerThresholdActive);
		DDX_Text(pDX, IDC_WHITE_PIXEL_TEXT, m_strWhitePixel);
		DDX_Check(pDX, IDC_AUTOTHRESHOLD_CHECK, m_bUseAutoThreshold);
		DDX_Text(pDX, IDC_AUTOTHRESHOLD_EDIT, m_csEditAutoThreshold);
		DDX_Text(pDX, IDC_UPPER_EDIT, m_csUpperThres);
		DDX_Text(pDX, IDC_LOWER_EDIT, m_csLowerThres);
		DDX_Check(pDX, IDC_USE_EXTERN_ATM_CHECK, m_bUseExternATM);
		DDX_Check(pDX, IDC_USE_EXTERN_LT_CHECK, m_bUseExternLT);
		DDX_Check(pDX, IDC_USE_EXTERN_UT_CHECK, m_bUseExternUT);
		//}}AFX_DATA_MAP
	}
	else
	{
		DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
		DDX_Control(pDX, IDC_AUTOTHRESHOLD_SLIDER, m_autoThresholdCtrl);
		DDX_Control(pDX, IDC_LOWER_SLIDER, m_lowerThreshold);
		DDX_Control(pDX, IDC_UPPER_SLIDER, m_upperThreshold);
		DDX_Text(pDX, IDC_AUTOTHRESHOLD_EDIT, m_csEditAutoThreshold);
	}
	DDX_Control(pDX, IDC_HISTOGRAM, m_histogram);
}

void SVToolAdjustmentDialogThresholdPageClass::initThreshold()
{
	if( m_pCurrentThreshold )
	{
		SetInspectionData();

		if( m_pCurrentThreshold && m_pCurrentThreshold->getReferenceImage() )
		{
			// Calculate And Show White Pixels...
			// &&&
			SVMatroxIdentifier histResultID = M_NULL;
			SVMatroxLongArray l_alHistValues;
			SVDataBufferInfoClass svData;

			l_alHistValues.resize(MaxValues);
			svData.Length = MaxValues;
			svData.Type = SVDataBufferInfoClass::SVHistResult;
			svData.HBuffer.milResult = histResultID;
			if ( S_OK == SVImageProcessingClass::CreateDataBuffer( &svData ) )
			{
				histResultID = svData.HBuffer.milResult;
			}

			SvOi::SVImageBufferHandlePtr ImageHandle;

			if ( m_pCurrentThreshold->getReferenceImage()->GetImageHandle( ImageHandle ) && nullptr != ImageHandle)
			{
				HRESULT l_Code = SVMatroxImageInterface::Histogram( histResultID, ImageHandle->GetBuffer() );
				l_Code = SVMatroxImageInterface::GetResult( histResultID, l_alHistValues );
				l_Code = SVMatroxImageInterface::Destroy( histResultID );

				m_strWhitePixel.Format( "White Pixel: %d", l_alHistValues[ MaxThresholdValue ] );
			}
		}
		setImages();
		UpdateData(false); // set data to dialog
	}
}

BOOL SVToolAdjustmentDialogThresholdPageClass::OnSetActive()
{
	SetInspectionData();
	setImages();
	return CPropertyPage::OnSetActive();
}

SVImageClass* SVToolAdjustmentDialogThresholdPageClass::getReferenceImage(SVToolClass* pTool, SVThresholdClass* pCurrentThreshold)
{
	SVImageClass* pImage(nullptr);

	const SVImageInfoClass* pImageInfo = pTool->getFirstImageInfo();
	if (nullptr != pImageInfo)
	{
		pImageInfo->GetOwnerImage(pImage);
	}
	return pImage;
}

BOOL SVToolAdjustmentDialogThresholdPageClass::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SetTaskObject( m_pTool );

	if( m_pTool )
	{
		if( m_pCurrentThreshold && getReferenceImage(m_pTool, m_pCurrentThreshold) )
		{
			m_dialogImage.AddTab(_T("Tool Result")); 
			setImages();

			// Setup sliders...
			m_upperThreshold.SetRange( 0, MaxThresholdValue, TRUE );
			m_lowerThreshold.SetRange( 0, MaxThresholdValue, TRUE );
			m_lowerThreshold.SetTic( MaxThresholdValue / 2 );
			m_upperThreshold.SetTic( MaxThresholdValue / 2 );
			m_lowerThreshold.SetPageSize( 1 );
			m_upperThreshold.SetPageSize( 1 );

			// Get current threshold values...
			m_pCurrentThreshold->m_upperThresh.GetValue( m_up );
			m_pCurrentThreshold->m_lowerThresh.GetValue( m_lo );

			m_upperThres = m_up;
			m_lowerThres = m_lo;

			m_upperThreshold.SetPos( static_cast<int>(m_upperThres) );
			m_lowerThreshold.SetPos( static_cast<int>(m_lowerThres) );

			m_csLowerThres.Format( _T("%ld"), m_lowerThres );
			m_csUpperThres.Format( _T("%ld"), m_upperThres );

			// Get other flags...
			m_pCurrentThreshold->m_threshActivate.GetValue( m_thresholdActive );
			m_pCurrentThreshold->m_upperThreshActivate.GetValue( m_upperThresholdActive );
			m_pCurrentThreshold->m_lowerThreshActivate.GetValue( m_lowerThresholdActive );
			m_pCurrentThreshold->m_useExternalLT.GetValue( m_bUseExternLT );
			m_pCurrentThreshold->m_useExternalUT.GetValue( m_bUseExternUT );
			m_pCurrentThreshold->m_autoThreshold.GetValue( m_bUseAutoThreshold );
			m_pCurrentThreshold->m_dAutoThresholdMultiplier.GetValue( m_dAutoThreshold );
			m_pCurrentThreshold->m_useExternalATM.GetValue( m_bUseExternATM );

			// setup auto threshold slider...
			m_autoThresholdCtrl.SetRange( 0, SliderRange, TRUE );
			m_autoThresholdCtrl.SetTic( SliderTic );

			m_autoThresholdCtrl.SetPos( static_cast<int>( m_dAutoThreshold * ScaleFactor ) );
			m_csEditAutoThreshold.Format( _T("%.4f"), m_dAutoThreshold);

			BOOL bBlackBackground;
			m_pCurrentThreshold->m_blackBackground.GetValue( bBlackBackground );
			m_autoThresholdBlackRadio.SetCheck( bBlackBackground );
			m_autoThresholdWhiteRadio.SetCheck( ! bBlackBackground );

			UpdateData(false); // set data to dialog
			OnCheck1();	// check thresholdActive state
		}
		else
		{
			// Deacivate all threshold page controls...
			CWnd* pWnd(nullptr);
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
	{
		// Not valid call...
		if( GetParent() )
		{
			GetParent()->SendMessage( WM_CLOSE );
		}
		else
		{
			SendMessage( WM_CLOSE );
		}
	}
	return TRUE;
}

void SVToolAdjustmentDialogThresholdPageClass::OnChangeUpperEdit()
{
	UpdateData(true); // get data of dialog
	//
	// Check threshold limits.
	//
	_stscanf( (LPCTSTR)m_csUpperThres, _T("%ld"), &m_upperThres );

	if (m_upperThres < 0L)
	{
		m_upperThres = 0L;
		m_csUpperThres.Format( _T("%ld") , m_upperThres );
	}
	else if (m_upperThres > MaxThresholdValue)
	{
		m_upperThres = MaxThresholdValue;
		m_csUpperThres.Format( _T("%ld") , m_upperThres );
	}

	m_histogram.PostMessage(ID_BOUND_CHANGE, 2, m_upperThres|(m_histState << 8));
	m_upperThreshold.SetPos( static_cast<int>(m_upperThres) );
	m_isEdit = true;	
	UpdateData(false); // set data to dialog
	m_isEdit = false;	
	initThreshold();
}

void SVToolAdjustmentDialogThresholdPageClass::OnChangeLowerEdit()
{
	UpdateData(true); // get data of dialog
	//
	// Check threshold limits.
	//
	_stscanf( (LPCTSTR)m_csLowerThres, "%ld", &m_lowerThres );

	if (m_lowerThres < 0L)
	{
		m_lowerThres = 0L;
		m_csLowerThres.Format( _T("%ld") , m_lowerThres );	
	}
	else if (m_lowerThres > MaxThresholdValue)
	{
		m_lowerThres = MaxThresholdValue;
		m_csLowerThres.Format( _T("%ld") , m_lowerThres );
	}
	m_histogram.PostMessage(ID_BOUND_CHANGE, 1, m_lowerThres|(m_histState << 8));

	m_lowerThreshold.SetPos( static_cast<int>(m_lowerThres) );	
	m_isEdit = true;	
	UpdateData(false); // set data to dialog
	m_isEdit = false;
	initThreshold();
}

void SVToolAdjustmentDialogThresholdPageClass::OnChangeAutothresholdEdit()
{
	UpdateData(true); // get data of dialog

	//
	// Check auto threshold limits.
	//
	_stscanf( (LPCTSTR)m_csEditAutoThreshold, "%lf", &m_dAutoThreshold );

	if (m_dAutoThreshold < 0.0)
	{
		m_dAutoThreshold = 0.0;
		m_csEditAutoThreshold.Format( _T("%.4f"), m_dAutoThreshold);
	}
	else if (m_dAutoThreshold > 2.0)
	{
		m_dAutoThreshold = 2.0;
		m_csEditAutoThreshold.Format( _T("%.4f"), m_dAutoThreshold);
	}
	//
	// Update controls.
	//
	m_autoThresholdCtrl.SetPos( static_cast<int>( m_dAutoThreshold * ScaleFactor ) );
	//autoThresholdCtrl.UpdateWindow();

	m_isEdit = true;
	UpdateData(false); // set data to dialog
	m_isEdit = false;	

	//
	// Show the effects of new threshold value.
	//
	initThreshold();
}

void SVToolAdjustmentDialogThresholdPageClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData(true); // get data of dialog

	UINT id = pScrollBar->GetDlgCtrlID();
	
	if (IDC_UPPER_SLIDER == id)
	{
		m_upperThres = static_cast<long>(m_upperThreshold.GetPos());
		m_histogram.PostMessage(ID_BOUND_CHANGE, 2, m_upperThres|(m_histState << 8));
		m_csUpperThres.Format( _T("%ld") , m_upperThres );
	}
	else if (IDC_LOWER_SLIDER == id)
	{
		m_lowerThres = static_cast<long>(m_lowerThreshold.GetPos());
		m_histogram.PostMessage(ID_BOUND_CHANGE, 1, m_lowerThres|(m_histState << 8));
		m_csLowerThres.Format( _T("%ld"), m_lowerThres );
	}
	else if (IDC_AUTOTHRESHOLD_SLIDER == id)
	{
		m_dAutoThreshold = static_cast<double>(m_autoThresholdCtrl.GetPos() ) / ScaleFactor;
		m_csEditAutoThreshold.Format( _T("%.4f") , m_dAutoThreshold );
	}

	UpdateData(false); // set data to dialog
	initThreshold();

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpperActivateCheck
////////////////////////////////////////////////////////////////////////////////
void SVToolAdjustmentDialogThresholdPageClass::OnUpperActivateCheck()
{
	// Activate or Deactivate upperThresholding
	UpdateData(true); // get data of dialog

	CWnd* pWnd(nullptr);
	if (TRUE == m_upperThresholdActive)
	{
		if (m_bUseExternUT)
		{
			// Disable and hide slider...
			if (pWnd = GetDlgItem(IDC_UPPER_SLIDER))
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
			if ( pWnd = GetDlgItem( IDC_UT_FORMULA_BUTTON ) )
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
			m_upperThres = static_cast<DWORD>(m_up);
			m_upperThreshold.SetPos( static_cast<int>(m_upperThres) );
			m_histogram.Enable(histogram::high);
			m_csUpperThres.Format( _T("%ld") , m_upperThres );
		}

		// Enable and show edit...
		if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
		{
			pWnd->ShowWindow( SW_SHOW );
			pWnd->EnableWindow(m_bUseExternUT ? FALSE : TRUE);
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
		m_up = static_cast<int>(m_upperThres);
		m_upperThres = MaxThresholdValue;
		m_upperThreshold.SetPos( static_cast<int>(m_upperThres) );

		if( m_bUseExternUT )
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

	UpdateData(false); // set data to dialog
	initThreshold();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnLowerActivateCheck
////////////////////////////////////////////////////////////////////////////////
void SVToolAdjustmentDialogThresholdPageClass::OnLowerActivateCheck()
{
	// Activate or Deactivate lowerThresholding
	UpdateData(true); // get data of dialog

	CWnd* pWnd(nullptr);
	if( TRUE == m_lowerThresholdActive )
	{
		if( m_bUseExternLT )
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
			m_lowerThres = static_cast<DWORD>(m_lo);
			m_lowerThreshold.SetPos( static_cast<int>(m_lowerThres) );
			m_histogram.Enable(histogram::low);
			// Setup Edit
			m_csLowerThres.Format( _T("%ld"), m_lowerThres );
		}

		// Enable and show edit...
		if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
		{
			pWnd->ShowWindow( SW_SHOW );
			pWnd->EnableWindow(m_bUseExternLT ? FALSE : TRUE );
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
		m_lo = static_cast<int>(m_lowerThres);
		m_lowerThres = 0;
		m_lowerThreshold.SetPos( static_cast<int>(m_lowerThres) );

		if( m_bUseExternLT )
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
		{
			pWnd->EnableWindow( FALSE );
		}
		// Disable use extern LT check..
		if( pWnd = GetDlgItem( IDC_USE_EXTERN_LT_CHECK ) )
		{
			pWnd->EnableWindow( FALSE );
		}
		m_histogram.Disable(histogram::low);
	}

	UpdateData( false ); // set data to dialog
	initThreshold();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnCheck1
//	Activate or Deactivate Thresholding
////////////////////////////////////////////////////////////////////////////////
void SVToolAdjustmentDialogThresholdPageClass::OnCheck1()
{
	UpdateData( true ); // get data of dialog

	CWnd* pWnd(nullptr);
	if( TRUE == m_thresholdActive )
	{
		if( m_bUseAutoThreshold )
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
			if( m_bUseExternATM )
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
			m_lo = static_cast<int>(m_lowerThres);
			m_up = static_cast<int>(m_upperThres);
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

			m_lo = static_cast<int>(m_lowerThres);
			m_up = static_cast<int>(m_upperThres);

			OnLowerActivateCheck(); // check lowerThresholdActive state
			OnUpperActivateCheck(); // check upperThresholdActive state

			m_lowerThreshold.SetPos(m_lo);
			m_upperThreshold.SetPos(m_up);
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
	UpdateData(true); // get data of dialog
	CheckRadioButton(IDC_WHITE_BACKGROUND_RADIO, IDC_BLACK_BACKGROUND_RADIO, IDC_WHITE_BACKGROUND_RADIO);
	initThreshold();
}

void SVToolAdjustmentDialogThresholdPageClass::OnBlackBackgroundRadio()
{
	UpdateData(true); // get data of dialog
	CheckRadioButton(IDC_WHITE_BACKGROUND_RADIO, IDC_BLACK_BACKGROUND_RADIO, IDC_BLACK_BACKGROUND_RADIO);
	initThreshold();
}

void SVToolAdjustmentDialogThresholdPageClass::OnUTFormulaButton()
{
	UpdateData(true); // get data of dialog

	SvDef::SVObjectTypeInfoStruct info(SvDef::SVEquationObjectType, SvDef::SVUpperThresholdEquationObjectType);
	SVUpperThresholdEquationClass* pEquation = dynamic_cast<SVUpperThresholdEquationClass*>(m_pCurrentThreshold->getFirstObject(info));
	if (pEquation)
	{
		std::string Caption = pEquation->GetName();
		Caption += _T(" Formula");

		const GUID& rInspectionID = m_pParentDialog->GetInspectionID();
		const GUID& rObjectID = m_pCurrentThreshold->GetUniqueObjectID();
		SvOg::SVFormulaEditorSheetClass dlg(rInspectionID, rObjectID, info, Caption.c_str());
		dlg.DoModal();

		long l_lResult = static_cast<long>(pEquation->GetYACCResult());
		m_csUpperThres.Format(_T("%ld") , l_lResult);

		UpdateData(false);

		initThreshold();
	}
}

void SVToolAdjustmentDialogThresholdPageClass::UpdateLowerThresholdFromFormula()
{
	SvDef::SVObjectTypeInfoStruct info;
	info.ObjectType = SvDef::SVEquationObjectType;
	info.SubType    = SvDef::SVLowerThresholdEquationObjectType;
	SVLowerThresholdEquationClass* pEquation = dynamic_cast<SVLowerThresholdEquationClass*>(m_pCurrentThreshold->getFirstObject(info));
	if (pEquation)
	{
		long l_lResult = static_cast<long>(pEquation->GetYACCResult());
		m_csLowerThres.Format(_T("%ld"), l_lResult);
	}
}

void SVToolAdjustmentDialogThresholdPageClass::UpdateUpperThresholdFromFoumula()
{
	SvDef::SVObjectTypeInfoStruct info;
	info.ObjectType = SvDef::SVEquationObjectType;
	info.SubType    = SvDef::SVUpperThresholdEquationObjectType;
	SVUpperThresholdEquationClass* pEquation = dynamic_cast<SVUpperThresholdEquationClass*>(m_pCurrentThreshold->getFirstObject(info));
	if (pEquation)
	{
		long l_lResult = static_cast<long>(pEquation->GetYACCResult());
		m_csUpperThres.Format(_T("%ld"), l_lResult);
	}
}

void SVToolAdjustmentDialogThresholdPageClass::OnATMFormulaButton()
{
	UpdateData(true); // get data of dialog

	SvDef::SVObjectTypeInfoStruct info(SvDef::SVEquationObjectType, SvDef::SVAutoThresholdEquationObjectType);
	SVAutoThresholdEquationClass* pEquation = dynamic_cast<SVAutoThresholdEquationClass*>(m_pCurrentThreshold->getFirstObject(info));
	if (pEquation)
	{
		std::string Caption = pEquation->GetName();
		Caption += _T(" Formula");

		const GUID& rInspectionID = m_pParentDialog->GetInspectionID();
		const GUID& rObjectID = m_pCurrentThreshold->GetUniqueObjectID();
		SvOg::SVFormulaEditorSheetClass dlg(rInspectionID, rObjectID, info, Caption.c_str());
		dlg.DoModal();

		initThreshold();
	}
}

void SVToolAdjustmentDialogThresholdPageClass::OnLTFormulaButton()
{
	UpdateData(true); // get data of dialog

	SvDef::SVObjectTypeInfoStruct info(SvDef::SVEquationObjectType, SvDef::SVLowerThresholdEquationObjectType);
	SVLowerThresholdEquationClass* pEquation = dynamic_cast<SVLowerThresholdEquationClass*>(m_pCurrentThreshold->getFirstObject(info));
	if (pEquation)
	{
		std::string Caption = pEquation->GetName();
		Caption += _T(" Formula");

		const GUID& rInspectionID = m_pParentDialog->GetInspectionID();
		const GUID& rObjectID = m_pCurrentThreshold->GetUniqueObjectID();
		SvOg::SVFormulaEditorSheetClass dlg(rInspectionID, rObjectID, info, Caption.c_str());
		dlg.DoModal();

		long l_lResult = static_cast<long>(pEquation->GetYACCResult());
		m_csLowerThres.Format(_T("%ld"), l_lResult);

		UpdateData(false);
		initThreshold();
	}
}

void SVToolAdjustmentDialogThresholdPageClass::OnUseExternATMCheck()
{
	UpdateData(true); // get data of dialog
	OnCheck1();	
}

void SVToolAdjustmentDialogThresholdPageClass::OnUseExternLTCheck()
{
	UpdateData(true); // get data of dialog
	OnCheck1();	
}

void SVToolAdjustmentDialogThresholdPageClass::OnUseExternUTCheck()
{
	UpdateData(true); // get data of dialog
	OnCheck1();
}

LRESULT SVToolAdjustmentDialogThresholdPageClass::OnBoundChange(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case 1:
		m_lowerThres = static_cast<long>(wParam);
		m_csLowerThres.Format( _T("%ld"), m_lowerThres );
		m_lowerThreshold.SetPos(m_lowerThres);
		break;
	case 2:
		m_upperThres = static_cast<long>(wParam);
		m_csUpperThres.Format( _T("%ld"), m_upperThres );
		m_upperThreshold.SetPos(m_upperThres);
		break;
	default:
		break;
	}
	UpdateData(false);
	initThreshold();

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
	return true;
}

void SVToolAdjustmentDialogThresholdPageClass::setImages()
{
	m_dialogImage.setImage( m_pCurrentThreshold->getReferenceImage() );
	m_dialogImage.Refresh();
}

void AFXAPI DDX_SVThresholdText( CDataExchange* pDX, int nIDC, DWORD& Value )
{
	HWND hWndCtrl = pDX->PrepareEditCtrl( nIDC );
	if ( pDX->m_bSaveAndValidate )
	{
		if ( !GetThreshold( hWndCtrl, Value ) )
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Threshold_OutOfRange, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10202 ); 
			pDX->Fail();
		}
	}
	else
	{
		SetThreshold( hWndCtrl, Value );
	}
}

bool GetThreshold( CWnd* pWnd, DWORD& Value )
{
	ASSERT( nullptr != pWnd );
	return GetThreshold( pWnd->m_hWnd, Value );
}

bool GetThreshold( HWND hWnd, DWORD& Value )
{
	TCHAR szWindowText[20];
	long con;

	::GetWindowText( hWnd, szWindowText, 19 );

	// only numbers be allowed!
	for( TCHAR* index = szWindowText; *index != 0;  index++ )
	{
		if( ( *index < '0' ) || ( *index > '9') )
		{
			return false;
		}
	}

	if( ( con = _ttol( szWindowText ) ) > MaxThresholdValue )
	{
		return false;
	}

	Value = static_cast<DWORD>(con);

	return true;
}

void SetThreshold( CWnd* pWnd, DWORD Value )
{
	ASSERT( nullptr != pWnd );
	SetThreshold( pWnd->m_hWnd, Value );
}

void SetThreshold( HWND hWnd, DWORD Value )
{
	std::string Text = SvUl::Format(_T("%lu"), Value);
	::SetWindowText( hWnd, Text.c_str() );
}

