// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCameraFormat
// * .File Name       : $Workfile:   SVCameraFormat.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:37:16  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVCameraFormat.h"
#pragma endregion Includes

SVCameraFormat::SVCameraFormat()
{
	init();
}

void SVCameraFormat::init()
{
	m_eImageType = SVImageFormatUnknown;
	m_lWidthMax = 0;
	m_lHeightMax = 0;
	m_lWidth = 0;
	m_lHeight = 0;
	m_lHStep = 1;
	m_lVStep = 1;
	m_lHPos = 0;
	m_lVPos = 0;
	m_lHPosStep = 1;
	m_lVPosStep = 1;
	m_lOrder = 0;
	m_bVariableROI = false;
	m_bColor = false;
}

SVCameraFormat::SVCameraFormat( const SVCameraFormat& rhs )
{
	*this = rhs;
}

const SVCameraFormat& SVCameraFormat::operator = ( const SVCameraFormat& rhs )
{
	if ( this != &rhs )
	{
		m_strName = rhs.m_strName;
		m_strDescription = rhs.m_strDescription;
		m_eImageType = rhs.m_eImageType;
		m_lWidthMax = rhs.m_lWidthMax;
		m_lHeightMax = rhs.m_lHeightMax;
		m_lWidth = rhs.m_lWidth;
		m_lHeight = rhs.m_lHeight;
		m_lHStep = rhs.m_lHStep;
		m_lVStep = rhs.m_lVStep;
		m_lHPos = rhs.m_lHPos;
		m_lVPos = rhs.m_lVPos;
		m_lHPosStep = rhs.m_lHPosStep;
		m_lVPosStep = rhs.m_lVPosStep;
		m_bVariableROI = rhs.m_bVariableROI;
		m_lOrder = rhs.m_lOrder;
		m_bColor = rhs.m_bColor;
	}
	return *this;
}

void SVCameraFormat::AssignUserChangableValues( const SVCameraFormat& rhs )
{
	m_lWidth = rhs.m_lWidth;
	m_lHeight = rhs.m_lHeight;
	m_lVPos = rhs.m_lVPos;
	m_lHPos = rhs.m_lHPos;
}

void SVCameraFormat::AssignConstantValues( const SVCameraFormat& rhs )
{
	m_strName = rhs.m_strName;
	m_strDescription = rhs.m_strDescription;
	m_eImageType = rhs.m_eImageType;
	m_lWidthMax = rhs.m_lWidthMax;
	m_lHeightMax = rhs.m_lHeightMax;
	m_lHStep = rhs.m_lHStep;
	m_lVStep = rhs.m_lVStep;
	m_lHPosStep = rhs.m_lHPosStep;
	m_lVPosStep = rhs.m_lVPosStep;
	m_bVariableROI = rhs.m_bVariableROI;
	m_lOrder = rhs.m_lOrder;
	m_bColor = rhs.m_bColor;
}

HRESULT SVCameraFormat::ParseAndAssignCameraFormat( const CString& sCameraFormat )
{
	CString s = sCameraFormat;
	m_strName = s;
	// GigE is set to variableROI if the camera format string contains FORMAT_7
	m_bVariableROI = (-1 != s.Find( "FORMAT_7" ));

	// parse the format string M_xXy_type@n[.n]FPS
	// or M_xXy_type_FORMAT_7_0
	// where @n[.n]FPS is optional for some camera types
	// FPS is optional (useless) for GIGE cameras

	s = s.Mid(2);	// strip off M_ prefix;

	int iV = s.Find('X');
	int iType = s.Find('_');

	CString sH = s.Left( iV );
	CString sV = s.Mid( iV + 1, iType - iV - 1 );
	CString sType = s.Mid( iType + 1, s.GetLength() - iType - 1 );
	
	m_lWidth = m_lWidthMax = atoi( sH );
	m_lHeight = m_lHeightMax = atoi( sV );

	if ( sType == _T("Y") )
	{
		m_eImageType = SVImageFormatMono8;
	}
	else if ( -1 != sType.Find(_T("RGB")) || -1 != sType.Find(_T("YUV")) )
	{
		m_eImageType = SVImageFormatRGB8888;
		m_bColor = true;
	}
	else if ( -1 != sType.Find(_T("Y_FORMAT_7"))  )
	{
		m_eImageType = SVImageFormatMono8;
	}
	else	// default
	{
		m_eImageType = SVImageFormatMono8;
	}
	return S_OK;
}
