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
#include "SVColorTool.h"
#include "SVColorThreshold.h"
#include "SVTADlgColorThresholdROI.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVInspectionProcess.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVIPDoc.h"
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

	SetTaskObject( m_pTool );

	SvDef::SVObjectTypeInfoStruct objectInfo;
	objectInfo.ObjectType = SvDef::SVOperatorObjectType;
	objectInfo.SubType = SvDef::SVColorThresholdObjectType;

	// Get the color threshold object
	m_pThreshold = dynamic_cast<SVColorThresholdClass*> (m_pTool->getFirstObject(objectInfo));

	if( m_pThreshold )
	{
		// Get Train Color Extent Variables
		SvDef::SVObjectTypeInfoStruct extentObjectInfo;
		extentObjectInfo.ObjectType = SvDef::SVValueObjectType;
		extentObjectInfo.SubType = SvDef::SVDoubleValueObjectType;

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

		SVImageClass* pImage = m_pTool->getOutputImage();

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

HRESULT SVTADlgColorThresholdROI::SetInspectionData()
{
	HRESULT l_hrOk = AddInputRequest( m_pExtentLeft, m_pSheet->m_rectROI.left );

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

