//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdROI
//* .File Name       : $Workfile:   SVTADlgColorThresholdROI.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Tools/SVColorTool.h"
#include "Operators/SVColorThreshold.h"
#include "SVTADlgColorThresholdROI.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVInspectionProcess.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdROI property page

IMPLEMENT_DYNCREATE(SVTADlgColorThresholdROI, SVTADlgColorThresholdBasePage)


BEGIN_MESSAGE_MAP(SVTADlgColorThresholdROI, SVTADlgColorThresholdBasePage)
	//{{AFX_MSG_MAP(SVTADlgColorThresholdROI)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


SVTADlgColorThresholdROI::SVTADlgColorThresholdROI() : SVTADlgColorThresholdBasePage(SVTADlgColorThresholdROI::IDD)
{
	//{{AFX_DATA_INIT(SVTADlgColorThresholdROI)
	mcsROIHeight = _T("");
	mcsROIWidth = _T("");
	mcsROIY = _T("");
	mcsROIX = _T("");
	m_strMouseColor = _T("");
	m_strMouseCoordinates = _T("");
	//}}AFX_DATA_INIT
}

SVTADlgColorThresholdROI::~SVTADlgColorThresholdROI()
{
}

void SVTADlgColorThresholdROI::DoDataExchange(CDataExchange* pDX)
{
	SVTADlgColorThresholdBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTADlgColorThresholdROI)
	DDX_Text(pDX, IDC_COLOR_ROI_H_VALUE, mcsROIHeight);
	DDX_Text(pDX, IDC_COLOR_ROI_W_VALUE, mcsROIWidth);
	DDX_Text(pDX, IDC_COLOR_ROI_Y_VALUE, mcsROIY);
	DDX_Text(pDX, IDC_COLOR_ROI_X_VALUE, mcsROIX);
	DDX_Text(pDX, IDC_RGB_COLOR, m_strMouseColor);
	DDX_Text(pDX, IDC_COORDINATES, m_strMouseCoordinates);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdROI message handlers

BOOL SVTADlgColorThresholdROI::OnInitDialog() 
{
	SVTADlgColorThresholdBasePage::OnInitDialog();

	SvDef::SVObjectTypeInfoStruct objectInfo;
	objectInfo.ObjectType = SvPb::SVOperatorObjectType;
	objectInfo.SubType = SvPb::SVColorThresholdObjectType;

	// Get the color threshold object
	m_pThreshold = dynamic_cast<SvOp::SVColorThresholdClass*> (m_pTool->getFirstObject(objectInfo));

	if( m_pThreshold )
	{
		// Get Train Color Extent Variables
		SvDef::SVObjectTypeInfoStruct extentObjectInfo;
		extentObjectInfo.ObjectType = SvPb::SVValueObjectType;
		extentObjectInfo.SubType = SvPb::SVDoubleValueObjectType;

		// Get Train Color ROI Extent Left Object...
		extentObjectInfo.EmbeddedID = SVExtentRelativeLeftPositionObjectGuid;
		m_pExtentLeft = dynamic_cast<SvVol::SVDoubleValueObjectClass*> (m_pThreshold->getFirstObject(extentObjectInfo));

		// Get Train Color ROI Extent Top Object...
		extentObjectInfo.EmbeddedID = SVExtentRelativeTopPositionObjectGuid;
		m_pExtentTop = dynamic_cast<SvVol::SVDoubleValueObjectClass*> (m_pThreshold->getFirstObject(extentObjectInfo));

		// Get Train Color ROI Extent Width Object...
		extentObjectInfo.EmbeddedID = SVExtentWidthObjectGuid;
		m_pExtentWidth = dynamic_cast<SvVol::SVDoubleValueObjectClass*> (m_pThreshold->getFirstObject(extentObjectInfo));

		// Get Train Color ROI Extent Height Object...
		extentObjectInfo.EmbeddedID = SVExtentHeightObjectGuid;
		m_pExtentHeight = dynamic_cast<SvVol::SVDoubleValueObjectClass*> (m_pThreshold->getFirstObject(extentObjectInfo));

		// Initialize Extent
		double value;
		m_pExtentLeft->GetValue( value );
		m_pSheet->m_rectROI.left = (long)(value);

		m_pExtentTop->GetValue( value );
		m_pSheet->m_rectROI.top = (long)(value);

		m_pExtentWidth->GetValue( value );
		m_pSheet->m_rectROI.right = (long)(value) + m_pSheet->m_rectROI.left;

		m_pExtentHeight->GetValue( value );
		m_pSheet->m_rectROI.bottom = (long)(value) + m_pSheet->m_rectROI.top;

		SvIe::SVImageClass* pImage = m_pTool->getOutputImage();

		if( nullptr != pImage )
		{
			SVImageInfoClass l_ImageInfo = pImage->GetImageInfo();

			l_ImageInfo.GetOutputRectangle( m_pSheet->m_ImageRect );

			m_pSheet->m_pFigureEditor->SetImageInfo( l_ImageInfo );
			m_pSheet->m_pFigureEditor->SetRect( m_pSheet->m_rectROI );

			m_svDlgImage.UpdateImageInfo( pImage->GetInspection()->GetUniqueObjectID(), pImage->GetUniqueObjectID() );
			m_svDlgImage.refresh();
		}

		RefreshProperties();
		UpdateToolFigure();
	}// end if( mpThreshold )
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVTADlgColorThresholdROI::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( point.x == 0 && point.y == 0 )
	{
		// this is a reflected message from the image
		CPoint ptMouse = m_svDlgImage.GetMouseCoordinates();

		if ( m_pSheet->m_ImageRect.PtInRect( ptMouse ) )
		{
			// Mouse Color
			CPoint pt;
			::GetCursorPos(&pt);	// in screen coords
			ScreenToClient(&pt);
			HDC hDC = ::GetDC(m_hWnd);
			COLORREF rgb = ::GetPixel( hDC, pt.x, pt.y );
			::ReleaseDC( m_hWnd, hDC );
			m_strMouseColor.Format(_T("%d, %d, %d"), GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));


			// Mouse position
			m_strMouseCoordinates.Format( _T("(%d, %d)"), ptMouse.x, ptMouse.y );
			
			
			UpdateData(FALSE);
		}

		if ( m_pSheet->m_pFigureEditor->IsCaptured() )
		{
			RefreshProperties();
			UpdateToolFigure();

			m_svDlgImage.refresh();
		}
	}
	else
	{
		SVTADlgColorThresholdBasePage::OnMouseMove(nFlags, point);
	}
}

void SVTADlgColorThresholdROI::RefreshProperties()
{
	m_pSheet->m_rectROI = m_pSheet->m_pFigureEditor->GetRect();
}


void SVTADlgColorThresholdROI::UpdateToolFigure()
{
	SetInspectionData();

	// Update Tool Figure
	SVExtentFigureStruct l_svFigure = m_pSheet->m_rectROI;

	mcsROIX.Format(_T("%d"),m_pSheet->m_rectROI.left );
	mcsROIY.Format(_T("%d"),m_pSheet->m_rectROI.top );
	mcsROIHeight.Format(_T("%d"),m_pSheet->m_rectROI.Height() );
	mcsROIWidth.Format(_T("%d"),m_pSheet->m_rectROI.Width() );

	UpdateData(FALSE);
}

void SVTADlgColorThresholdROI::SetInspectionData()
{
	UpdateData(true);

	if (nullptr != m_pThreshold)
	{
		//@TODO[gra][8.00][15.01.2018]: The data controller should be used like the rest of SVOGui
		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
		Controller Values{ SvOg::BoundValues{ m_pThreshold->GetInspection()->GetUniqueObjectID(), m_pThreshold->GetUniqueObjectID() } };
		Values.Init();

		Values.Set<double>(m_pExtentLeft->GetEmbeddedID(), static_cast<double> (m_pSheet->m_rectROI.left));
		Values.Set<double>(m_pExtentTop->GetEmbeddedID(), static_cast<double> (m_pSheet->m_rectROI.top));
		Values.Set<double>(m_pExtentWidth->GetEmbeddedID(), static_cast<double> (m_pSheet->m_rectROI.Width()));
		Values.Set<double>(m_pExtentHeight->GetEmbeddedID(), static_cast<double> (m_pSheet->m_rectROI.Height()));
		Values.Commit();
	};
}

