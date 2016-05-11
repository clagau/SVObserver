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

#include "stdafx.h"
#include "SVTADlgColorThresholdAdjustment.h"

#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "GuiCommands/InspectionRunOnce.h"
#include "SVColorTool.h"
#include "SVColorThreshold.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVSVIMStateClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVTADlgColorThresholdAdjustment, SVTADlgColorThresholdBasePage)

BEGIN_MESSAGE_MAP(SVTADlgColorThresholdAdjustment, SVTADlgColorThresholdBasePage)
	//{{AFX_MSG_MAP(SVTADlgColorThresholdAdjustment)
	ON_BN_CLICKED(IDC_EXCLUDE_COLOR_THRESHOLD, OnExcludeColorThreshold)
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVTADlgColorThresholdAdjustment::SVTADlgColorThresholdAdjustment() : SVTADlgColorThresholdBasePage(SVTADlgColorThresholdAdjustment::IDD)
{
	mpUpperThreshold = nullptr;
	mpLowerThreshold = nullptr;
	mpExclude = nullptr;
	mpCurrentDocument = nullptr;
	mpTool = nullptr;
	mpThreshold = nullptr;
	mBandNumber = 0;

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
	HRESULT l_hrOk = AddInputRequest( mpLowerThreshold, m_lowerThreshold );

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequest( mpUpperThreshold, m_upperThreshold );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequest( mpExclude, m_exclude );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequest( mpExtentLeft, m_pSheet->m_rectROI.left );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequest( mpExtentTop, m_pSheet->m_rectROI.top );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequest( mpExtentWidth, m_pSheet->m_rectROI.Width() );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequest( mpExtentHeight, m_pSheet->m_rectROI.Height() );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequestMarker();
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = RunOnce( mpTool );
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
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COLOR_THRESHOLD_IMAGE, m_svDlgImage);
}


/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdAdjustment message handlers

BOOL SVTADlgColorThresholdAdjustment::OnInitDialog() 
{
	SVTADlgColorThresholdBasePage::OnInitDialog();
	
	ASSERT( m_pSheet );

	mpTool = m_pSheet->GetTool();

	SetTaskObject( mpTool );

	// Get the color threshold object
	SVObjectTypeInfoStruct objectInfo;
	objectInfo.ObjectType = SVOperatorObjectType;
	objectInfo.SubType = SVColorThresholdObjectType;

	mpThreshold = reinterpret_cast <SVColorThresholdClass*> (::SVSendMessage( mpTool, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ) );

	if( mpThreshold )
	{
		// Get Train Color Extent Variables
		SVObjectTypeInfoStruct extentObjectInfo;
		extentObjectInfo.ObjectType = SVDoubleValueObjectType;

		// Get Train Color ROI Extent Left Object...
		extentObjectInfo.EmbeddedID = SVExtentRelativeLeftPositionObjectGuid;
		mpExtentLeft = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&extentObjectInfo) ));

		// Get Train Color ROI Extent Top Object...
		extentObjectInfo.EmbeddedID = SVExtentRelativeTopPositionObjectGuid;
		mpExtentTop = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&extentObjectInfo) ));

		// Get Train Color ROI Extent Width Object...
		extentObjectInfo.EmbeddedID = SVExtentWidthObjectGuid;
		mpExtentWidth = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&extentObjectInfo) ));

		// Get Train Color ROI Extent Height Object...
		extentObjectInfo.EmbeddedID = SVExtentHeightObjectGuid;
		mpExtentHeight = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&extentObjectInfo) ));
	}

	SVImageClass* pImage = nullptr;
	
	// Get the Threshold variables
	objectInfo.SubType = SVNotSetSubObjectType;

	if( mBandNumber == 0 )
	{
		// Get the Image to Display
		pImage = mpThreshold->GetBand0OutputImage();

		// Get the threshold variables
		objectInfo.ObjectType = SVLongValueObjectType;

		objectInfo.EmbeddedID = SVBand0UpperThresholdObjectGuid;
		mpUpperThreshold = reinterpret_cast<SVLongValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

		objectInfo.EmbeddedID = SVBand0LowerThresholdObjectGuid;
		mpLowerThreshold = reinterpret_cast<SVLongValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVBand0ThresholdExcludeObjectGuid;
		mpExclude = reinterpret_cast<SVBoolValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));
	}
	else if( mBandNumber == 1 )
	{
		pImage = mpThreshold->GetBand1OutputImage();
		
		// Get the threshold variables
		objectInfo.ObjectType = SVLongValueObjectType;

		objectInfo.EmbeddedID = SVBand1UpperThresholdObjectGuid;
		mpUpperThreshold = reinterpret_cast<SVLongValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

		objectInfo.EmbeddedID = SVBand1LowerThresholdObjectGuid;
		mpLowerThreshold = reinterpret_cast<SVLongValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVBand1ThresholdExcludeObjectGuid;
		mpExclude = reinterpret_cast<SVBoolValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));
	}
	else
	{
		pImage = mpThreshold->GetBand2OutputImage();
		
		// Get the threshold variables
		objectInfo.ObjectType = SVLongValueObjectType;

		objectInfo.EmbeddedID = SVBand2UpperThresholdObjectGuid;
		mpUpperThreshold = reinterpret_cast<SVLongValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

		objectInfo.EmbeddedID = SVBand2LowerThresholdObjectGuid;
		mpLowerThreshold = reinterpret_cast<SVLongValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVBand2ThresholdExcludeObjectGuid;
		mpExclude = reinterpret_cast<SVBoolValueObjectClass*>(::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));
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
		SVDrawObjectListClass* pThresholdBarsFigure = mpThreshold->GetThresholdBarsFigure( mBandNumber );
		//m_svDlgImage.AddPoints( pThresholdBarsFigure );
		m_svDlgImage.AddOverlayPoints( pThresholdBarsFigure, options );

		// Get the Graph Figure from the SVColorThresholdClass
		// Note: Do this last
		options.sizeROI = m_pSheet->m_rectROI.Size();
		options.bNormalizeY_ROI = true;
		options.bScaleY = false;
		SVDrawObjectClass* pGraphFigure = mpThreshold->GetGraphFigure( mBandNumber );
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
	if( mpUpperThreshold && mpLowerThreshold && mpExclude )
	{
		long value;
	
		mpLowerThreshold->GetValue( value );
		m_lowerThreshold = (unsigned char)value;

		mpUpperThreshold->GetValue( value );
		m_upperThreshold = (unsigned char)value;

		mpExclude->GetValue( m_exclude );

		UpdateData( FALSE );
	}
}

void SVTADlgColorThresholdAdjustment::setThresholdParams()
{
	if( mpUpperThreshold && mpLowerThreshold && mpExclude )
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
	if( mpUpperThreshold && mpLowerThreshold && mpExclude )
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
	if( nullptr != mpTool )
	{
		SVInspectionProcess* pInspection = mpTool->GetInspection();

		::SVSendMessage( mpTool, SVM_RESET_ALL_OBJECTS, 0, 0 );

		if( nullptr != pInspection )
		{
			GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID() );
			SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

			l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
		}

		m_svDlgImage.ClearOverlayPoints();

		SVDlgImageOverlayOptions options;
		options.sizeROI = CSize(255,255);
		options.rectMaxXY = CRect(0,0, 255,255);
		options.bFlipVertical = true;

		// Get the Threshold bars from the SVColorThresholdClass
		// Note: Do this first
		SVDrawObjectListClass* pThresholdBarsFigure = mpThreshold->GetThresholdBarsFigure( mBandNumber );
		m_svDlgImage.AddOverlayPoints( pThresholdBarsFigure, options );

		// Get the Graph Figure from the SVColorThresholdClass
		// Note: Do this last
		options.sizeROI = m_pSheet->m_rectROI.Size();
		options.bNormalizeY_ROI = true;
		options.bScaleY = false;
		SVDrawObjectClass* pGraphFigure = mpThreshold->GetGraphFigure( mBandNumber );
		//m_svDlgImage.AddPoints( *pGraphFigure, SVGraphScale );
		m_svDlgImage.AddOverlayPoints( *pGraphFigure, options );

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

void SVTADlgColorThresholdAdjustment::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( mpUpperThreshold && mpLowerThreshold )
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

