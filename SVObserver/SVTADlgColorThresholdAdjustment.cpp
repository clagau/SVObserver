//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdAdjustment
//* .File Name       : $Workfile:   SVTADlgColorThresholdAdjustment.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgColorThresholdAdjustment.h"

#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVColorTool.h"
#include "SVColorThreshold.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"

#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

IMPLEMENT_DYNCREATE(SVTADlgColorThresholdAdjustment, SVTADlgColorThresholdBasePage)

BEGIN_MESSAGE_MAP(SVTADlgColorThresholdAdjustment, SVTADlgColorThresholdBasePage)
	//{{AFX_MSG_MAP(SVTADlgColorThresholdAdjustment)
	ON_BN_CLICKED(IDC_EXCLUDE_COLOR_THRESHOLD, OnExcludeColorThreshold)
	ON_BN_CLICKED(IDC_ENABLED_COLOR_THRESHOLD, OnEnabledThreshold)
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVTADlgColorThresholdAdjustment::SVTADlgColorThresholdAdjustment() 
: SVTADlgColorThresholdBasePage(SVTADlgColorThresholdAdjustment::IDD)
, m_pUpperThreshold(nullptr)
, m_pLowerThreshold(nullptr)
, m_pExclude(nullptr)
, m_pEnabled(nullptr)
, m_pCurrentDocument(nullptr)
, m_BandNumber(0)
{
	//{{AFX_DATA_INIT(SVTADlgColorThresholdAdjustment)
	StrUpper = _T("");
	StrLower = _T("");
	m_exclude = FALSE;
	//}}AFX_DATA_INIT
}

SVTADlgColorThresholdAdjustment::~SVTADlgColorThresholdAdjustment()
{
}

HRESULT SVTADlgColorThresholdAdjustment::SetInspectionData()
{
	HRESULT l_hrOk = AddInputRequest( m_pLowerThreshold, m_lowerThreshold );

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequest( m_pUpperThreshold, m_upperThreshold );
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequest(m_pExclude, m_exclude);
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = AddInputRequest(m_pEnabled, m_Enabled);
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequest( m_pExtentLeft, m_pSheet->m_rectROI.left );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequest( m_pExtentTop, m_pSheet->m_rectROI.top );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequest( m_pExtentWidth, m_pSheet->m_rectROI.Width() );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequest( m_pExtentHeight, m_pSheet->m_rectROI.Height() );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequestMarker();
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = RunOnce( m_pTool->GetUniqueObjectID() );
	}

	return l_hrOk;
}

void SVTADlgColorThresholdAdjustment::DoDataExchange(CDataExchange* pDX)
{
	SVTADlgColorThresholdBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTADlgColorThresholdAdjustment)
	DDX_Control(pDX, IDC_UPPER_THRESHOLD_SLIDER, UpperSliderCtrl);
	DDX_Control(pDX, IDC_LOWER_THRESHOLD_SLIDER, LowerSliderCtrl);
	DDX_Text(pDX, IDC_UPPER_THRESHOLD_STATIC, StrUpper);
	DDX_Text(pDX, IDC_LOWER_THRESHOLD_STATIC, StrLower);
	DDX_Check(pDX, IDC_EXCLUDE_COLOR_THRESHOLD, m_exclude);
	DDX_Check(pDX, IDC_ENABLED_COLOR_THRESHOLD, m_Enabled);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COLOR_THRESHOLD_IMAGE, m_svDlgImage);
}


/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdAdjustment message handlers

BOOL SVTADlgColorThresholdAdjustment::OnInitDialog() 
{
	SVTADlgColorThresholdBasePage::OnInitDialog();
	
	ASSERT( m_pSheet );

	m_pTool = m_pSheet->GetTool();

	SetTaskObject( m_pTool );

	// Get the color threshold object
	SVObjectTypeInfoStruct objectInfo;
	objectInfo.ObjectType = SVOperatorObjectType;
	objectInfo.SubType = SVColorThresholdObjectType;

	m_pThreshold = dynamic_cast<SVColorThresholdClass*> (m_pTool->getFirstObject(objectInfo));

	if( m_pThreshold )
	{
		// Get Train Color Extent Variables
		SVObjectTypeInfoStruct extentObjectInfo;
		extentObjectInfo.ObjectType = SVValueObjectType;
		extentObjectInfo.SubType = SVDoubleValueObjectType;

		// Get Train Color ROI Extent Left Object...
		extentObjectInfo.EmbeddedID = SVExtentRelativeLeftPositionObjectGuid;
		m_pExtentLeft = dynamic_cast<SVDoubleValueObjectClass*> (m_pThreshold->getFirstObject(extentObjectInfo));

		// Get Train Color ROI Extent Top Object...
		extentObjectInfo.EmbeddedID = SVExtentRelativeTopPositionObjectGuid;
		m_pExtentTop = dynamic_cast<SVDoubleValueObjectClass*> (m_pThreshold->getFirstObject(extentObjectInfo));

		// Get Train Color ROI Extent Width Object...
		extentObjectInfo.EmbeddedID = SVExtentWidthObjectGuid;
		m_pExtentWidth = dynamic_cast<SVDoubleValueObjectClass*> (m_pThreshold->getFirstObject(extentObjectInfo));

		// Get Train Color ROI Extent Height Object...
		extentObjectInfo.EmbeddedID = SVExtentHeightObjectGuid;
		m_pExtentHeight = dynamic_cast<SVDoubleValueObjectClass*> (m_pThreshold->getFirstObject(extentObjectInfo));
	}

	SVImageClass* pImage(nullptr);
	
	BandEnum Band = static_cast<BandEnum> (m_BandNumber);
	// Get the Image to Display
	BandThreshold* pBandThreshold = m_pThreshold->GetBandThreshold(Band);
	if (nullptr != pBandThreshold)
	{
		m_pUpperThreshold = &pBandThreshold->m_UpperThreshold;
		m_pLowerThreshold = &pBandThreshold->m_LowerThreshold;
		m_pExclude = &pBandThreshold->m_ThresholdExclude;
		m_pEnabled = &pBandThreshold->m_ThresholdEnabled;
		pImage = &pBandThreshold->m_OutputImage;
	}

	if( pImage )
	{
		m_svDlgImage.UpdateImageInfo( pImage->GetInspection()->GetUniqueObjectID(), pImage->GetUniqueObjectID() );

		SVDlgImageOverlayOptions options;
		options.sizeROI = CSize(255,255);
		options.rectMaxXY = CRect(0,0, 255,255);
		options.bFlipVertical = true;

		// Get the Threshold bars from the SVColorThresholdClass
		// Note: Do this first
		SVDrawObjectListClass* pThresholdBarsFigure = m_pThreshold->GetThresholdBarsFigure(Band);
		//m_svDlgImage.AddPoints( pThresholdBarsFigure );
		m_svDlgImage.AddOverlayPoints( pThresholdBarsFigure, options );

		// Get the Graph Figure from the SVColorThresholdClass
		// Note: Do this last
		options.sizeROI = m_pSheet->m_rectROI.Size();
		options.bNormalizeY_ROI = true;
		options.bScaleY = false;
		SVDrawObjectClass* pGraphFigure = m_pThreshold->GetGraphFigure(Band);
		//m_svDlgImage.AddPoints( *pGraphFigure, SVGraphScale );
		m_svDlgImage.AddOverlayPoints( *pGraphFigure, options );

		m_svDlgImage.refresh();
	}

	setInitialValues();

	updateGraphDisplay();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVTADlgColorThresholdAdjustment::getThresholdParams()
{
	// Update DDX variables
	if(nullptr != m_pUpperThreshold && nullptr != m_pLowerThreshold && nullptr != m_pExclude && nullptr != m_pEnabled)
	{
		long value;
	
		m_pLowerThreshold->GetValue( value );
		m_lowerThreshold = (unsigned char)value;

		m_pUpperThreshold->GetValue( value );
		m_upperThreshold = (unsigned char)value;

		m_pExclude->GetValue(m_exclude);
		m_pEnabled->GetValue(m_Enabled);

		UpdateData( FALSE );
	}
}

void SVTADlgColorThresholdAdjustment::setThresholdParams()
{
	if( m_pUpperThreshold && m_pLowerThreshold && m_pExclude )
	{
		// Set the Values
		SetInspectionData();
	}
}

void SVTADlgColorThresholdAdjustment::updateControls()
{
	setScrollPos( &UpperSliderCtrl, m_upperThreshold );
	setScrollPos( &LowerSliderCtrl, m_lowerThreshold );
	
	StrUpper.Format( "%u", m_upperThreshold );
	StrLower.Format( "%u", m_lowerThreshold );

	UpdateData( FALSE );
}

void SVTADlgColorThresholdAdjustment::setInitialValues()
{
	if( m_pUpperThreshold && m_pLowerThreshold && m_pExclude )
	{
		getThresholdParams();

		// Set Slider Range to 0 - 255
		setScrollRange( &LowerSliderCtrl, 0, 255 );
	
		// Set Slider Range to 0 - 255
		setScrollRange( &UpperSliderCtrl, 0, 255 );

		updateControls();

		updateGraphDisplay();
	}
}

void SVTADlgColorThresholdAdjustment::setScrollRange( CSliderCtrl* pSliderCtrl, int min, int max )
{
	// Revise the scale ?
	m_Normalizer.SetRealRange( ( double )min, ( double )max );

	pSliderCtrl->SetRange( ( int ) m_Normalizer.GetNormalRangeMin(), ( int ) m_Normalizer.GetNormalRangeMax(), TRUE );
	pSliderCtrl->SetTic( ( int ) ( m_Normalizer.GetNormalRangeBaseSize() / 2 + m_Normalizer.GetNormalRangeMin() ) );
	int pageSize = ( int ) ( ( double ) ( m_Normalizer.GetNormalRangeBaseSize() / m_Normalizer.GetRealRangeBaseSize() ) );
	pSliderCtrl->SetPageSize( pageSize > 0 ? pageSize : 1 );
}

void SVTADlgColorThresholdAdjustment::setScrollPos( CSliderCtrl* pSliderCtrl, int pos )
{
	int iNormalizedValue = ( int ) m_Normalizer.CalcNormalizedValue( ( double ) pos );
	pSliderCtrl->SetPos( static_cast<int>(m_Normalizer.GetNormalRangeMax() - iNormalizedValue) );
}

void SVTADlgColorThresholdAdjustment::updateGraphDisplay()
{
	if( nullptr != m_pTool )
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(m_pTool->GetInspection());

		m_pTool->resetAllObjects();

		RunOnce(m_pTool->GetUniqueObjectID());

		m_svDlgImage.ClearOverlayPoints();

		SVDlgImageOverlayOptions options;
		options.sizeROI = CSize(255,255);
		options.rectMaxXY = CRect(0,0, 255,255);
		options.bFlipVertical = true;

		// Get the Threshold bars from the SVColorThresholdClass
		// Note: Do this first
		SVDrawObjectListClass* pThresholdBarsFigure = m_pThreshold->GetThresholdBarsFigure(static_cast<BandEnum> (m_BandNumber));
		m_svDlgImage.AddOverlayPoints( pThresholdBarsFigure, options );

		// Get the Graph Figure from the SVColorThresholdClass
		// Note: Do this last
		options.sizeROI = m_pSheet->m_rectROI.Size();
		options.bNormalizeY_ROI = true;
		options.bScaleY = false;
		//! Only draw overlays when enabled
		if (m_Enabled)
		{
			SVDrawObjectClass* pGraphFigure = m_pThreshold->GetGraphFigure(static_cast<BandEnum> (m_BandNumber));
			m_svDlgImage.AddOverlayPoints(*pGraphFigure, options);
		}

		m_svDlgImage.refresh();
	}
}

void SVTADlgColorThresholdAdjustment::OnExcludeColorThreshold()
{
	UpdateData();
	setThresholdParams();

	updateControls();

	updateGraphDisplay();
}

void SVTADlgColorThresholdAdjustment::OnEnabledThreshold()
{
	UpdateData();
	setThresholdParams();

	updateControls();

	updateGraphDisplay();
}

void SVTADlgColorThresholdAdjustment::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( m_pUpperThreshold && m_pLowerThreshold )
	{
		UpdateData( TRUE ); // get data of dialog	
	
		if( &UpperSliderCtrl == ( CSliderCtrl* ) pScrollBar )
		{
			m_upperThreshold = 255 - (unsigned char) m_Normalizer.CalcRealValue( ( double ) UpperSliderCtrl.GetPos() );
			StrUpper.Format( "%u", m_upperThreshold );
		}
		else if( &LowerSliderCtrl == ( CSliderCtrl* ) pScrollBar )
		{
			m_lowerThreshold = 255 - (unsigned char) m_Normalizer.CalcRealValue( ( double ) LowerSliderCtrl.GetPos() );
			StrLower.Format( "%u", m_lowerThreshold );
		}

		UpdateData( FALSE ); // set data to dialog

		setThresholdParams();
		updateGraphDisplay();
	}
	
	SVTADlgColorThresholdBasePage::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL SVTADlgColorThresholdAdjustment::OnSetActive() 
{
	updateGraphDisplay();	
	return SVTADlgColorThresholdBasePage::OnSetActive();
}

void SVTADlgColorThresholdAdjustment::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( point.x == 0 && point.y == 0 )
	{
		// this is a reflected message from the image
		CPoint ptMouse = m_svDlgImage.GetMouseCoordinates();

		if ( m_pSheet->m_pFigureEditor->IsCaptured() )
		{
			RefreshProperties();
			SetInspectionData();
			updateGraphDisplay();
		}
	}
	else
	{
		SVTADlgColorThresholdBasePage::OnMouseMove(nFlags, point);
	}
}

void SVTADlgColorThresholdAdjustment::RefreshProperties()
{
	m_pSheet->m_rectROI = m_pSheet->m_pFigureEditor->GetRect();
}

