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
#include "SVROIFigureEditor.h"
#include "SVTADlgColorThresholdSheet.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "Operators/SVColorThreshold.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "Tools/SVColorTool.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//This comment is to avoid that the SVColorToolClass include is marked as not required due to forward declaration from a base class

constexpr SvPb::EmbeddedIdEnum upperThresholdID[SvDef::BandEnum::BandNumber] = { SvPb::Band0UpperThresholdEId, SvPb::Band1UpperThresholdEId, SvPb::Band2UpperThresholdEId };
constexpr SvPb::EmbeddedIdEnum lowerThresholdID[SvDef::BandEnum::BandNumber] = { SvPb::Band0LowerThresholdEId, SvPb::Band1LowerThresholdEId, SvPb::Band2LowerThresholdEId };
constexpr SvPb::EmbeddedIdEnum thresholdExcludeID[SvDef::BandEnum::BandNumber] = { SvPb::Band0ThresholdExcludeEId, SvPb::Band1ThresholdExcludeEId, SvPb::Band2ThresholdExcludeEId };
constexpr SvPb::EmbeddedIdEnum thresholdEnabledID[SvDef::BandEnum::BandNumber] = { SvPb::Band0ThresholdEnabledEId, SvPb::Band1ThresholdEnabledEId, SvPb::Band2ThresholdEnabledEId };
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
//InspectionID and TaskID are set later
, m_Values {SvOg::BoundValues{ SvDef::InvalidObjectId, SvDef::InvalidObjectId }}
{
	//{{AFX_DATA_INIT(SVTADlgColorThresholdAdjustment)
	StrUpper = _T("");
	StrLower = _T("");
	m_exclude = FALSE;
	//}}AFX_DATA_INIT
}

SVTADlgColorThresholdAdjustment::~SVTADlgColorThresholdAdjustment()
{
	if(M_NULL != m_histogramResultID)
	{
		SVMatroxImageInterface::Destroy(m_histogramResultID);
	}
}

void SVTADlgColorThresholdAdjustment::SetInspectionData()
{
	m_Values.Set<long>(lowerThresholdID[m_band], static_cast<long> (m_lowerThreshold));
	m_Values.Set<long>(upperThresholdID[m_band], static_cast<long> (m_upperThreshold));
	m_Values.Set<bool>(thresholdExcludeID[m_band], m_exclude ? true : false);
	m_Values.Set<bool>(thresholdEnabledID[m_band], m_Enabled ? true : false);
	m_Values.Set<double>(SvPb::ExtentRelativeLeftPositionEId, static_cast<double> (m_pSheet->m_rectROI.left));
	m_Values.Set<double>(SvPb::ExtentRelativeTopPositionEId, static_cast<double> (m_pSheet->m_rectROI.top));
	m_Values.Set<double>(SvPb::ExtentWidthEId, static_cast<double> (m_pSheet->m_rectROI.Width()));
	m_Values.Set<double>(SvPb::ExtentHeightEId, static_cast<double> (m_pSheet->m_rectROI.Height()));
	m_Values.Commit();
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
	
	assert( m_pSheet );

	m_pTool = m_pSheet->GetTool();

	// Get the color threshold object
	SvDef::SVObjectTypeInfoStruct objectInfo;
	objectInfo.m_ObjectType = SvPb::SVOperatorObjectType;
	objectInfo.m_SubType = SvPb::SVColorThresholdObjectType;

	m_pThreshold = dynamic_cast<SvOp::SVColorThreshold*> (m_pTool->getFirstObject(objectInfo));

	if( m_pThreshold )
	{
		if(!initHistogram())
		{
			SvStl::MessageMgrStd message(SvStl::MsgType::Log | SvStl::MsgType::Display);
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateBufferFailed, SvStl::SourceFileParams(StdMessageParams));
		}

		m_graphFigure.SetDrawPen(true, PS_SOLID, 1, SvDef::DefaultSubFunctionColor1);

		m_Values.SetInspectionID(m_pThreshold->GetInspection()->getObjectId());
		m_Values.SetTaskID(m_pThreshold->getObjectId());
		m_Values.Init();
		// Get Train Color Extent Variables
		SvDef::SVObjectTypeInfoStruct extentObjectInfo;
		extentObjectInfo.m_ObjectType = SvPb::SVValueObjectType;
		extentObjectInfo.m_SubType = SvPb::SVDoubleValueObjectType;
	}

	SvIe::SVImageClass* pImage(nullptr);
	
	// Get the Image to Display
	SvOp::BandThreshold* pBandThreshold = m_pThreshold->GetBandThreshold(m_band);
	if (nullptr != pBandThreshold)
	{
		pImage = &pBandThreshold->m_OutputImage;
	}

	if( pImage )
	{
		m_svDlgImage.UpdateImageInfo( pImage->GetInspection()->getObjectId(), pImage->getObjectId() );

		SVDlgImageOverlayOptions options;
		options.sizeROI = CSize(255,255);
		options.rectMaxXY = CRect(0,0, 255,255);
		options.bFlipVertical = true;

		// Note: Do this first
		updateThresholdBars();
		m_svDlgImage.AddOverlayPoints(m_thresholdBarFigures, options);

		// Get the Graph Figure from the SVColorThreshold
		// Note: Do this last
		options.sizeROI = m_pSheet->m_rectROI.Size();
		options.bNormalizeY_ROI = true;
		options.bScaleY = false;

		updateHistogram();
		m_svDlgImage.AddOverlayPoints(m_graphFigure, options);
		m_svDlgImage.refresh();
	}

	setInitialValues();

	updateGraphDisplay();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVTADlgColorThresholdAdjustment::setThresholdParams()
{
	SetInspectionData();
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
	m_lowerThreshold = static_cast<BYTE> (m_Values.Get<long>(lowerThresholdID[m_band]));
	m_upperThreshold = static_cast<BYTE> (m_Values.Get<long>(upperThresholdID[m_band]));
	m_exclude = m_Values.Get<bool>(thresholdExcludeID[m_band]);
	m_Enabled = m_Values.Get<bool>(thresholdEnabledID[m_band]);

	// Set Slider Range to 0 - 255
	setScrollRange( &LowerSliderCtrl, 0, 255 );
	
	// Set Slider Range to 0 - 255
	setScrollRange( &UpperSliderCtrl, 0, 255 );

	updateControls();

	updateGraphDisplay();
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
		m_pTool->resetAllObjects();

		m_Values.RunOnce(m_pTool->GetInspection()->getObjectId());

		m_svDlgImage.ClearOverlayPoints();

		SVDlgImageOverlayOptions options;
		options.sizeROI = CSize(255,255);
		options.rectMaxXY = CRect(0,0, 255,255);
		options.bFlipVertical = true;

		// Note: Do this first
		updateThresholdBars();
		m_svDlgImage.AddOverlayPoints(m_thresholdBarFigures, options);

		// Get the Graph Figure from the SVColorThreshold
		// Note: Do this last
		options.sizeROI = m_pSheet->m_rectROI.Size();
		options.bNormalizeY_ROI = true;
		options.bScaleY = false;
		//! Only draw overlays when enabled
		if (m_Enabled)
		{
			updateHistogram();
			m_svDlgImage.AddOverlayPoints(m_graphFigure, options);
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

bool SVTADlgColorThresholdAdjustment::initHistogram()
{
	bool result{false};

	if (nullptr != m_pThreshold)
	{
		SvIe::SVImageClass* pImage = m_pThreshold->GetBandInputImage(m_band);

		if (nullptr != pImage)
		{
			result = true;
			SVImageInfoClass ImageInfo = pImage->GetImageInfo();

			long l_lPixelDepth = 0;
			ImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, l_lPixelDepth);
			long histogramSize = 1 << (l_lPixelDepth & SVBufferSize);

			SVMatroxImageInterface::Create(m_histogramResultID, histogramSize, SVImageHistList);
			if (M_NULL == m_histogramResultID)
			{
				return false;
			}
		}
	}
	return result;
}

void SVTADlgColorThresholdAdjustment::updateHistogram()
{
	if (nullptr != m_pThreshold && nullptr != m_pSheet)
	{
		if (m_Values.Get<bool>(thresholdEnabledID[m_band]))
		{
			SvIe::SVImageClass* pImage = m_pThreshold->GetBandInputImage(m_band);

			m_histogramImage.reset();
			m_graphFigure.SetListSize(0);
			if(nullptr != pImage)
			{
				SVImageInfoClass childImageInfo;
				//Need to normalize the ROI for the image boundaries
				CRect rectImage = m_svDlgImage.GetOutputRectFromImage();
				CRect rectRoi;
				if (m_pSheet->m_rectROI.left < rectImage.Width() && m_pSheet->m_rectROI.top < rectImage.Height())
				{
					rectRoi.left = (m_pSheet->m_rectROI.left < 0) ? 0 : m_pSheet->m_rectROI.left;
					rectRoi.top = (m_pSheet->m_rectROI.top < 0) ? 0 : m_pSheet->m_rectROI.top;
					rectRoi.right = (m_pSheet->m_rectROI.right > rectImage.Width()) ? rectImage.Width() : m_pSheet->m_rectROI.right;
					rectRoi.bottom = (m_pSheet->m_rectROI.bottom > rectImage.Height()) ? rectImage.Height() : m_pSheet->m_rectROI.bottom;
					childImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, SV8BitUnsigned);
					childImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1);
					childImageInfo.SetExtentProperty(SvPb::SVExtentPropertyPositionPointX, rectRoi.left);
					childImageInfo.SetExtentProperty(SvPb::SVExtentPropertyPositionPointY, rectRoi.top);
					childImageInfo.SetExtentProperty(SvPb::SVExtentPropertyWidth, rectRoi.Width());
					childImageInfo.SetExtentProperty(SvPb::SVExtentPropertyHeight, rectRoi.Height());
					SvIe::SVImageProcessingClass::CreateImageChildBuffer(pImage->GetImageInfo(), pImage->getLastImage(), childImageInfo, m_histogramImage);
				}
			}

			if (nullptr != m_histogramImage)
			{
				if (S_OK != SVMatroxImageInterface::Histogram(m_histogramResultID, m_histogramImage->GetBuffer()))
				{
					return;
				}
				std::vector<long> histogramResultVector;
				SVMatroxImageInterface::GetResult(m_histogramResultID, histogramResultVector);

				// Calculate Graph...
				m_graphFigure.SetListSize(static_cast<int> (histogramResultVector.size()));
		
				for (int i = 0; i < histogramResultVector.size(); i++)
				{
					// Get Pixel ...
					long Pixel = histogramResultVector[i];

					// Must be weighted by color number!
					POINT graphPoint;
					graphPoint.x = i;
					graphPoint.y = Pixel;

					m_graphFigure.SetPointAtGrow(i, graphPoint);
				}
			}
		}
	}
}


void SVTADlgColorThresholdAdjustment::updateThresholdBars()
{
	if(nullptr != m_pThreshold)
	{
		SvIe::SVImageClass* pInputImage = m_pThreshold->GetBandInputImage(m_band);

		if (nullptr != pInputImage)
		{
			SVImageInfoClass ImageInfo = pInputImage->GetImageInfo();

			m_thresholdBarFigures.Flush();


			SVDrawObject DrawObject;

			DrawObject.SetDrawPen(true, PS_DOT, 1, SvDef::DefaultGoodColor);

			POINT point;
			POINT graphPoint;

			long height = 0;

			ImageInfo.GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, point);
			ImageInfo.GetExtentProperty(SvPb::SVExtentPropertyHeight, height);

			// MaxThresholdBar
			// Max left...
			graphPoint.x = point.x + m_upperThreshold;
			graphPoint.y = point.y;

			DrawObject.SetPointAtGrow(0, graphPoint);

			// Max right...
			graphPoint.y = point.y + height;

			DrawObject.SetPointAtGrow(1, graphPoint);

			m_thresholdBarFigures.Add(DrawObject);

			// MinThresholdBar
			// Min left...
			graphPoint.x = point.x + m_lowerThreshold;
			graphPoint.y = point.y;

			DrawObject.SetPointAtGrow(0, graphPoint);

			// Min right...
			graphPoint.y = point.y + height;

			DrawObject.SetPointAtGrow(1, graphPoint);

			m_thresholdBarFigures.Add(DrawObject);
		}
	}
}
