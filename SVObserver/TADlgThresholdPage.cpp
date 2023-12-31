//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogThresholdPage
//* .File Name       : $Workfile:   TADlgThresholdPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgThresholdPage.h"
#include "SVIPDoc.h"
#include "TADlgSheetClass.h"
#include "InspectionEngine/SVDataBuffer.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "Operators/SVAutoThresholdEquation.h"
#include "Operators/SVLowerThresholdEquation.h"
#include "Operators/SVThresholdClass.h"
#include "Operators/SVUpperThresholdEquation.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVMFCControls/SVHistogram.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#include "SVOGuiUtility/DataController.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static long MaxThresholdValue = 255L;
static long MaxValues = 256;
static double ScaleFactor = 10000.0;
static long SliderRange = 20000;
static long SliderTic = SliderRange / 2;

BEGIN_MESSAGE_MAP(TADlgThresholdPage, CPropertyPage)
	//{{AFX_MSG_MAP(TADlgThresholdPage)
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

TADlgThresholdPage::TADlgThresholdPage( uint32_t inspectionId, uint32_t taskObjectId, TADlgSheetClass* Parent ) 
: CPropertyPage(TADlgThresholdPage::IDD)
, m_pUseExternATM(nullptr)
, m_pUseExternLT(nullptr)
, m_pUseExternUT(nullptr)
, m_pTaskObject(nullptr)
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
, m_ImageController(inspectionId, taskObjectId)
{
	//{{AFX_DATA_INIT(TADlgThresholdPage)
	m_bUseExternATM = false;
	m_bUseExternLT = false;
	m_bUseExternUT = false;
	//}}AFX_DATA_INIT

	if (m_pParentDialog)
	{
		m_pTaskObject = m_pParentDialog->GetTaskObject();

		if (nullptr != m_pTaskObject)
		{
			SvDef::SVObjectTypeInfoStruct info(SvPb::SVUnaryImageOperatorObjectType, SvPb::SVThresholdObjectType);
			m_pCurrentThreshold = dynamic_cast<SvOp::SVThresholdClass *>(m_pTaskObject->getFirstObject(info));

			if( nullptr != m_pCurrentThreshold )
			{
				// Try to get Threshold Embeddeds...

				SvDef::SVObjectTypeInfoStruct infoATM;
				infoATM.m_ObjectType = SvPb::SVValueObjectType;
				infoATM.m_SubType = SvPb::SVBoolValueObjectType;
				infoATM.m_EmbeddedID = SvPb::UseExternalATMEId;
				m_pUseExternATM = dynamic_cast<SvVol::SVBoolValueObjectClass*>(m_pCurrentThreshold->getFirstObject(infoATM));

				SvDef::SVObjectTypeInfoStruct infoLT;
				infoLT.m_ObjectType = SvPb::SVValueObjectType;
				infoLT.m_SubType = SvPb::SVBoolValueObjectType;
				infoLT.m_EmbeddedID = SvPb::UseExternalLTEId;
				m_pUseExternLT = dynamic_cast<SvVol::SVBoolValueObjectClass*>(m_pCurrentThreshold->getFirstObject(infoLT));

				SvDef::SVObjectTypeInfoStruct infoUT;
				infoUT.m_ObjectType = SvPb::SVValueObjectType;
				infoUT.m_SubType = SvPb::SVBoolValueObjectType;
				infoUT.m_EmbeddedID = SvPb::UseExternalUTEId;
				m_pUseExternUT = dynamic_cast<SvVol::SVBoolValueObjectClass*>(m_pCurrentThreshold->getFirstObject(infoUT));
			}
		}
	}
}

TADlgThresholdPage::~TADlgThresholdPage()
{
}

HRESULT TADlgThresholdPage::SetInspectionData()
{
	HRESULT Result{ S_OK };

	UpdateData(true); // get data from dialog

	if (nullptr != m_pCurrentThreshold)
	{
		//@TODO[gra][8.00][15.01.2018]: The data controller should be used like the rest of SVOGui
		SvOgu::ValueController Values{ SvOgu::BoundValues{ m_pCurrentThreshold->GetInspection()->getObjectId(), m_pCurrentThreshold->getObjectId() } };
		Values.Init();

		Values.Set<long>(m_pCurrentThreshold->m_upperThresh.GetEmbeddedID(), m_upperThreshold.GetPos());
		Values.Set<long>(m_pCurrentThreshold->m_lowerThresh.GetEmbeddedID(), m_lowerThreshold.GetPos());
		Values.Set<bool>(m_pCurrentThreshold->m_threshActivate.GetEmbeddedID(), m_thresholdActive ? true : false);
		Values.Set<bool>(m_pCurrentThreshold->m_upperThreshActivate.GetEmbeddedID(), m_upperThresholdActive ? true : false);
		Values.Set<bool>(m_pCurrentThreshold->m_lowerThreshActivate.GetEmbeddedID(), m_lowerThresholdActive ? true : false);
		Values.Set<bool>(m_pCurrentThreshold->m_useExternalLT.GetEmbeddedID(), m_bUseExternLT ? true : false);
		Values.Set<bool>(m_pCurrentThreshold->m_useExternalUT.GetEmbeddedID(), m_bUseExternUT ? true : false);
		Values.Set<bool>(m_pCurrentThreshold->m_autoThreshold.GetEmbeddedID(), m_bUseAutoThreshold ? true : false);
		Values.Set<double>(m_pCurrentThreshold->m_dAutoThresholdMultiplier.GetEmbeddedID(), m_dAutoThreshold);
		Values.Set<bool>(m_pCurrentThreshold->m_blackBackground.GetEmbeddedID(), m_autoThresholdBlackRadio.GetCheck() ? true : false);
		Values.Set<bool>(m_pCurrentThreshold->m_useExternalATM.GetEmbeddedID(), m_bUseExternATM ? true : false);

		Result = Values.Commit();
	}


	std::vector<long> histogramValues;
	histogramValues.resize(MaxValues);
	m_histState = 0;
	if (m_thresholdActive)
	{
		m_histState = histogram::shown;
		if (!m_bUseAutoThreshold)
		{
			m_histState |= histogram::active;
		}
		m_pCurrentThreshold->saveHistogram = true;
		SvCmd::RunOnceSynchronous(m_pCurrentThreshold->GetInspection()->getObjectId());
		histogramValues = m_pCurrentThreshold->GetHistogramValues();
		m_pCurrentThreshold->saveHistogram = false;
	}

	m_histogram.SetPixelCounts(histogramValues.begin(), histogramValues.end());
	m_histogram.PostMessage(ID_BOUND_CHANGE, 1, m_lowerThreshold.GetPos());
	m_histogram.PostMessage(ID_BOUND_CHANGE, 2, m_upperThreshold.GetPos()|(m_histState << 8));

	UpdateData(false);

	return Result;
}

void TADlgThresholdPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	if (FALSE == m_isEdit)
	{
		//{{AFX_DATA_MAP(TADlgThresholdPage)
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

void TADlgThresholdPage::initThreshold()
{
	if (nullptr != m_pCurrentThreshold)
	{
		SetInspectionData();

		// Calculate And Show White Pixels...
		if (m_pCurrentThreshold->getOutputImage())
		{
			__int64 histResultID = M_NULL;
			std::vector<long> outHistValues;
			SvIe::SVDataBufferInfoClass svData;

			outHistValues.resize(MaxValues);
			svData.Length = MaxValues;
			svData.Type = SvIe::SVDataBufferInfoClass::SVHistResult;
			svData.HBuffer.milResult = histResultID;
			if (S_OK == SvIe::SVImageProcessingClass::CreateDataBuffer(&svData))
			{
				histResultID = svData.HBuffer.milResult;
			}

			SvOi::SVImageBufferHandlePtr pImageBuffer = m_pCurrentThreshold->getOutputImage()->getLastImage();

			if (nullptr != pImageBuffer && !pImageBuffer->empty())
			{
				SVMatroxImageInterface::Histogram(histResultID, pImageBuffer->GetBuffer());
				SVMatroxImageInterface::GetResult(histResultID, outHistValues);
				SVMatroxImageInterface::Destroy(histResultID);

				m_strWhitePixel = std::format("White Pixel: {:d}", outHistValues[MaxThresholdValue]).c_str();
			}
		}
		setImages();
		UpdateData(false); // set data to dialog
	}
}

BOOL TADlgThresholdPage::OnSetActive()
{
	SetInspectionData();
	setImages();
	return CPropertyPage::OnSetActive();
}

BOOL TADlgThresholdPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_ImageController.Init();
	m_resultImageID = getFirstResultImageId(m_ImageController);

	if(nullptr != m_pTaskObject)
	{
		if( m_pCurrentThreshold )
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
			CWnd* pWnd = GetDlgItem(IDC_DIALOGIMAGE);
			if(nullptr != pWnd)
				pWnd->EnableWindow( false );
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_SLIDER)))
				pWnd->EnableWindow(false);
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_SLIDER)))
				pWnd->EnableWindow(false);
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_CHECK)))
				pWnd->EnableWindow(false);
			if (nullptr != (pWnd = GetDlgItem(IDC_CHECK1)))
				pWnd->EnableWindow(false);
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_CHECK)))
				pWnd->EnableWindow(false);
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_EDIT)))
				pWnd->EnableWindow(false);
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_EDIT)))
				pWnd->EnableWindow(false);
			if (nullptr != (pWnd = GetDlgItem(IDC_WHITE_PIXEL_TEXT)))
				pWnd->EnableWindow(false);
			if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_LT_CHECK)))
				pWnd->EnableWindow(false);
			if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_UT_CHECK)))
				pWnd->EnableWindow(false);

			// Hide Formula Button
			if (nullptr != (pWnd = GetDlgItem(IDC_LT_FORMULA_BUTTON)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_UT_FORMULA_BUTTON)))
				pWnd->ShowWindow( SW_HIDE );

			// Hide auto threshold controls...
			if (nullptr != (pWnd = GetDlgItem(IDC_AUTOTHRESHOLD_EDIT)))
				pWnd->ShowWindow(SW_HIDE);
			if (nullptr != (pWnd = GetDlgItem(IDC_BLACK_BACKGROUND_RADIO)))
				pWnd->ShowWindow(SW_HIDE);
			if (nullptr != (pWnd = GetDlgItem(IDC_WHITE_BACKGROUND_RADIO)))
				pWnd->ShowWindow(SW_HIDE);
			if (nullptr != (pWnd = GetDlgItem(IDC_AUTOTHRESHOLD_SLIDER)))
				pWnd->ShowWindow(SW_HIDE);
			if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_ATM_CHECK)))
				pWnd->ShowWindow(SW_HIDE);
			if (nullptr != (pWnd = GetDlgItem(IDC_ATM_FORMULA_BUTTON)))
				pWnd->ShowWindow(SW_HIDE);
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

void TADlgThresholdPage::OnChangeUpperEdit()
{
	UpdateData(true); // get data of dialog
	//
	// Check threshold limits.
	//
	_stscanf( static_cast<LPCTSTR>(m_csUpperThres), _T("%ld"), &m_upperThres );

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

void TADlgThresholdPage::OnChangeLowerEdit()
{
	UpdateData(true); // get data of dialog
	//
	// Check threshold limits.
	//
	_stscanf(static_cast<LPCTSTR>(m_csLowerThres), "%ld", &m_lowerThres );

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

void TADlgThresholdPage::OnChangeAutothresholdEdit()
{
	UpdateData(true); // get data of dialog

	//
	// Check auto threshold limits.
	//
	_stscanf(static_cast<LPCTSTR>(m_csEditAutoThreshold), "%lf", &m_dAutoThreshold );

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

void TADlgThresholdPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
void TADlgThresholdPage::OnUpperActivateCheck()
{
	// Activate or Deactivate upperThresholding
	UpdateData(true); // get data of dialog

	CWnd* pWnd(nullptr);
	if (TRUE == m_upperThresholdActive)
	{
		if (m_bUseExternUT)
		{
			// Disable and hide slider...
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_SLIDER)))
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Enable and show formula button...
			if (nullptr != (pWnd = GetDlgItem(IDC_UT_FORMULA_BUTTON)))
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
			if (nullptr != (pWnd = GetDlgItem(IDC_UT_FORMULA_BUTTON)))
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Enable and show slider...
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_SLIDER)))
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
		if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_EDIT)))
		{
			pWnd->ShowWindow( SW_SHOW );
			pWnd->EnableWindow(m_bUseExternUT ? FALSE : TRUE);
		}

		// Enable and show use extern UT check..
		if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_UT_CHECK)))
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
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_SLIDER)))
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Show disabled formula button...
			if (nullptr != (pWnd = GetDlgItem(IDC_UT_FORMULA_BUTTON)))
			{
				pWnd->EnableWindow( FALSE );
				pWnd->ShowWindow( SW_SHOW );
			}
		}
		else
		{
			// Hide and disable formula button...
			if (nullptr != (pWnd = GetDlgItem(IDC_UT_FORMULA_BUTTON)))
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Show disabled slider...
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_SLIDER)))
			{
				pWnd->EnableWindow( FALSE );
				pWnd->ShowWindow( SW_SHOW );
			}
		}

		// Disable edit...
		if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_EDIT)))
			pWnd->EnableWindow( FALSE );

		// Disable use extern UT check..
		if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_UT_CHECK)))
			pWnd->EnableWindow( FALSE );

		m_histogram.Disable(histogram::high);
	}

	UpdateData(false); // set data to dialog
	initThreshold();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnLowerActivateCheck
////////////////////////////////////////////////////////////////////////////////
void TADlgThresholdPage::OnLowerActivateCheck()
{
	// Activate or Deactivate lowerThresholding
	UpdateData(true); // get data of dialog

	CWnd* pWnd(nullptr);
	if( TRUE == m_lowerThresholdActive )
	{
		if( m_bUseExternLT )
		{
			// Disable and hide slider...
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_SLIDER)))
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Enable and show formula button...
			if (nullptr != (pWnd = GetDlgItem(IDC_LT_FORMULA_BUTTON)))
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
			if (nullptr != (pWnd = GetDlgItem(IDC_LT_FORMULA_BUTTON)))
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Enable and show slider...
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_SLIDER)))
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
		if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_EDIT)))
		{
			pWnd->ShowWindow( SW_SHOW );
			pWnd->EnableWindow(m_bUseExternLT ? FALSE : TRUE );
		}

		// Enable and show use extern LT check..
		if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_LT_CHECK)))
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
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_SLIDER)))
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Show disabled formula button...
			if (nullptr != (pWnd = GetDlgItem(IDC_LT_FORMULA_BUTTON)))
			{
				pWnd->EnableWindow( FALSE );
				pWnd->ShowWindow( SW_SHOW );
			}
		}
		else
		{
			// Hide and disable formula button...
			if (nullptr != (pWnd = GetDlgItem(IDC_LT_FORMULA_BUTTON)))
			{
				pWnd->ShowWindow( SW_HIDE );
				pWnd->EnableWindow( FALSE );
			}

			// Show disabled slider...
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_SLIDER)))
			{
				pWnd->EnableWindow( FALSE );
				pWnd->ShowWindow( SW_SHOW );
			}
		}

		// Disable edit...
		if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_EDIT)))
		{
			pWnd->EnableWindow( FALSE );
		}
		// Disable use extern LT check..
		if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_LT_CHECK)))
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
void TADlgThresholdPage::OnCheck1()
{
	UpdateData( true ); // get data of dialog

	CWnd* pWnd(nullptr);
	if( TRUE == m_thresholdActive )
	{
		if( m_bUseAutoThreshold )
		{
			// Hide manual threshold controls...
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_CHECK)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_CHECK)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_STATIC)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_STATIC)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_EDIT)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_EDIT)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_SLIDER)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_SLIDER)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_LT_CHECK)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_UT_CHECK)))
				pWnd->ShowWindow( SW_HIDE );
			// Hide Formula Button
			if (nullptr != (pWnd = GetDlgItem(IDC_LT_FORMULA_BUTTON)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_UT_FORMULA_BUTTON)))
				pWnd->ShowWindow( SW_HIDE );

			// Show auto threshold controls...
			if (nullptr != (pWnd = GetDlgItem(IDC_AUTOTHRESHOLD_CHECK)))
				pWnd->ShowWindow(SW_SHOW);
			if (nullptr != (pWnd = GetDlgItem(IDC_AUTOTHRESHOLD_EDIT)))
				pWnd->ShowWindow( SW_SHOW );
			if (nullptr != (pWnd = GetDlgItem(IDC_BLACK_BACKGROUND_RADIO)))
				pWnd->ShowWindow( SW_SHOW );
			if (nullptr != (pWnd = GetDlgItem(IDC_WHITE_BACKGROUND_RADIO)))
				pWnd->ShowWindow( SW_SHOW );
			if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_ATM_CHECK)))
				pWnd->ShowWindow( SW_SHOW );

			if (nullptr != (pWnd = GetDlgItem(IDC_AUTOTHRESHOLD_SLIDER)))
				pWnd->ShowWindow(m_bUseExternATM ? SW_HIDE : SW_SHOW);
			if (nullptr != (pWnd = GetDlgItem(IDC_ATM_FORMULA_BUTTON)))
				pWnd->ShowWindow(m_bUseExternATM ? SW_SHOW : SW_HIDE);

			// Save state
			m_lo = static_cast<int>(m_lowerThres);
			m_up = static_cast<int>(m_upperThres);
		}
		else
		{
			// Hide auto threshold controls...
			if (nullptr != (pWnd = GetDlgItem(IDC_AUTOTHRESHOLD_EDIT)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_BLACK_BACKGROUND_RADIO)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_WHITE_BACKGROUND_RADIO)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_AUTOTHRESHOLD_SLIDER)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_ATM_CHECK)))
				pWnd->ShowWindow( SW_HIDE );
			if (nullptr != (pWnd = GetDlgItem(IDC_ATM_FORMULA_BUTTON)))
				pWnd->ShowWindow( SW_HIDE );

			// Show manual threshold controls...
			if (nullptr != (pWnd = GetDlgItem(IDC_AUTOTHRESHOLD_CHECK)))
				pWnd->ShowWindow(SW_SHOW);
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_CHECK)))
				pWnd->ShowWindow( SW_SHOW );
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_CHECK)))
				pWnd->ShowWindow( SW_SHOW );
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_STATIC)))
				pWnd->ShowWindow( SW_SHOW );
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_STATIC)))
				pWnd->ShowWindow( SW_SHOW );
			if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_EDIT)))
				pWnd->ShowWindow( SW_SHOW );
			if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_EDIT)))
				pWnd->ShowWindow( SW_SHOW );
			if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_LT_CHECK)))
				pWnd->ShowWindow( SW_SHOW );
			if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_UT_CHECK)))
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
		if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_CHECK)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_CHECK)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_STATIC)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_STATIC)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_EDIT)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_EDIT)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_SLIDER)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_SLIDER)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_LT_CHECK)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_UT_CHECK)))
			pWnd->ShowWindow( SW_HIDE );
		// Hide Formula Button
		if (nullptr != (pWnd = GetDlgItem(IDC_LT_FORMULA_BUTTON)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_UT_FORMULA_BUTTON)))
			pWnd->ShowWindow( SW_HIDE );

		// Hide auto threshold controls...
		if (nullptr != (pWnd = GetDlgItem(IDC_AUTOTHRESHOLD_CHECK)))
			pWnd->ShowWindow(SW_HIDE);
		if (nullptr != (pWnd = GetDlgItem(IDC_AUTOTHRESHOLD_EDIT)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_BLACK_BACKGROUND_RADIO)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_WHITE_BACKGROUND_RADIO)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_AUTOTHRESHOLD_SLIDER)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_USE_EXTERN_ATM_CHECK)))
			pWnd->ShowWindow( SW_HIDE );
		if (nullptr != (pWnd = GetDlgItem(IDC_ATM_FORMULA_BUTTON)))
			pWnd->ShowWindow( SW_HIDE );
	}

	initThreshold();
}

void TADlgThresholdPage::OnAutoThresholdCheck()
{
	OnCheck1();
}

void TADlgThresholdPage::OnWhiteBackgroundRadio()
{
	UpdateData(true); // get data of dialog
	CheckRadioButton(IDC_WHITE_BACKGROUND_RADIO, IDC_BLACK_BACKGROUND_RADIO, IDC_WHITE_BACKGROUND_RADIO);
	initThreshold();
}

void TADlgThresholdPage::OnBlackBackgroundRadio()
{
	UpdateData(true); // get data of dialog
	CheckRadioButton(IDC_WHITE_BACKGROUND_RADIO, IDC_BLACK_BACKGROUND_RADIO, IDC_BLACK_BACKGROUND_RADIO);
	initThreshold();
}

void TADlgThresholdPage::OnUTFormulaButton()
{
	UpdateData(true); // get data of dialog

	SvDef::SVObjectTypeInfoStruct info(SvPb::SVEquationObjectType, SvPb::SVUpperThresholdEquationObjectType);
	SvOp::SVUpperThresholdEquation* pEquation = dynamic_cast<SvOp::SVUpperThresholdEquation*>(m_pCurrentThreshold->getFirstObject(info));
	if (pEquation)
	{
		std::string Caption = pEquation->GetName();
		Caption += _T(" Formula");

		uint32_t inspectionID = m_pParentDialog->GetInspectionID();
		uint32_t objectID = m_pCurrentThreshold->getObjectId();
		SvOg::SVFormulaEditorSheetClass dlg(inspectionID, objectID, info, Caption.c_str());
		dlg.DoModal();

		long l_lResult = static_cast<long>(pEquation->GetYACCResult());
		m_csUpperThres.Format(_T("%ld") , l_lResult);

		UpdateData(false);

		initThreshold();
	}
}

void TADlgThresholdPage::UpdateLowerThresholdFromFormula()
{
	SvDef::SVObjectTypeInfoStruct info;
	info.m_ObjectType = SvPb::SVEquationObjectType;
	info.m_SubType    = SvPb::SVLowerThresholdEquationObjectType;
	SvOp::SVLowerThresholdEquation* pEquation = dynamic_cast<SvOp::SVLowerThresholdEquation*>(m_pCurrentThreshold->getFirstObject(info));
	if (pEquation)
	{
		long l_lResult = static_cast<long>(pEquation->GetYACCResult());
		m_csLowerThres.Format(_T("%ld"), l_lResult);
	}
}

void TADlgThresholdPage::UpdateUpperThresholdFromFoumula()
{
	SvDef::SVObjectTypeInfoStruct info;
	info.m_ObjectType = SvPb::SVEquationObjectType;
	info.m_SubType    = SvPb::SVUpperThresholdEquationObjectType;
	SvOp::SVUpperThresholdEquation* pEquation = dynamic_cast<SvOp::SVUpperThresholdEquation*>(m_pCurrentThreshold->getFirstObject(info));
	if (pEquation)
	{
		long l_lResult = static_cast<long>(pEquation->GetYACCResult());
		m_csUpperThres.Format(_T("%ld"), l_lResult);
	}
}

void TADlgThresholdPage::OnATMFormulaButton()
{
	UpdateData(true); // get data of dialog

	SvDef::SVObjectTypeInfoStruct info(SvPb::SVEquationObjectType, SvPb::SVAutoThresholdEquationObjectType);
	SvOp::SVAutoThresholdEquation* pEquation = dynamic_cast<SvOp::SVAutoThresholdEquation*>(m_pCurrentThreshold->getFirstObject(info));
	if (pEquation)
	{
		std::string Caption = pEquation->GetName();
		Caption += _T(" Formula");

		uint32_t inspectionID = m_pParentDialog->GetInspectionID();
		uint32_t objectID = m_pCurrentThreshold->getObjectId();
		SvOg::SVFormulaEditorSheetClass dlg(inspectionID, objectID, info, Caption.c_str());
		dlg.DoModal();

		initThreshold();
	}
}

void TADlgThresholdPage::OnLTFormulaButton()
{
	UpdateData(true); // get data of dialog

	SvDef::SVObjectTypeInfoStruct info(SvPb::SVEquationObjectType, SvPb::SVLowerThresholdEquationObjectType);
	SvOp::SVLowerThresholdEquation* pEquation = dynamic_cast<SvOp::SVLowerThresholdEquation*>(m_pCurrentThreshold->getFirstObject(info));
	if (pEquation)
	{
		std::string Caption = pEquation->GetName();
		Caption += _T(" Formula");

		uint32_t inspectionID = m_pParentDialog->GetInspectionID();
		uint32_t objectID = m_pCurrentThreshold->getObjectId();
		SvOg::SVFormulaEditorSheetClass dlg(inspectionID, objectID, info, Caption.c_str());
		dlg.DoModal();

		long l_lResult = static_cast<long>(pEquation->GetYACCResult());
		m_csLowerThres.Format(_T("%ld"), l_lResult);

		UpdateData(false);
		initThreshold();
	}
}

void TADlgThresholdPage::OnUseExternATMCheck()
{
	UpdateData(true); // get data of dialog
	OnCheck1();	
}

void TADlgThresholdPage::OnUseExternLTCheck()
{
	UpdateData(true); // get data of dialog
	OnCheck1();	
}

void TADlgThresholdPage::OnUseExternUTCheck()
{
	UpdateData(true); // get data of dialog
	OnCheck1();
}

LRESULT TADlgThresholdPage::OnBoundChange(WPARAM wParam, LPARAM lParam)
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

BOOL TADlgThresholdPage::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

void TADlgThresholdPage::setImages()
{
	IPictureDisp* pResultImage = m_ImageController.GetImage(m_resultImageID);
	m_dialogImage.setImage(pResultImage);
	m_dialogImage.Refresh();
}
