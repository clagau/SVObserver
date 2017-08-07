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

SVCameraFormat::SVCameraFormat( const SVCameraFormat& rRhs )
{
	*this = rRhs;
}

const SVCameraFormat& SVCameraFormat::operator = ( const SVCameraFormat& rRhs )
{
	if ( this != &rRhs )
	{
		m_strName = rRhs.m_strName;
		m_strDescription = rRhs.m_strDescription;
		m_eImageType = rRhs.m_eImageType;
		m_lWidthMax = rRhs.m_lWidthMax;
		m_lHeightMax = rRhs.m_lHeightMax;
		m_lWidth = rRhs.m_lWidth;
		m_lHeight = rRhs.m_lHeight;
		m_lHStep = rRhs.m_lHStep;
		m_lVStep = rRhs.m_lVStep;
		m_lHPos = rRhs.m_lHPos;
		m_lVPos = rRhs.m_lVPos;
		m_lHPosStep = rRhs.m_lHPosStep;
		m_lVPosStep = rRhs.m_lVPosStep;
		m_bVariableROI = rRhs.m_bVariableROI;
		m_lOrder = rRhs.m_lOrder;
		m_bColor = rRhs.m_bColor;
	}
	return *this;
}

void SVCameraFormat::AssignUserChangableValues( const SVCameraFormat& rRhs )
{
	m_lWidth = rRhs.m_lWidth;
	m_lHeight = rRhs.m_lHeight;
	m_lVPos = rRhs.m_lVPos;
	m_lHPos = rRhs.m_lHPos;
}

void SVCameraFormat::AssignConstantValues( const SVCameraFormat& rRhs )
{
	m_strName = rRhs.m_strName;
	m_strDescription = rRhs.m_strDescription;
	m_eImageType = rRhs.m_eImageType;
	m_lWidthMax = rRhs.m_lWidthMax;
	m_lHeightMax = rRhs.m_lHeightMax;
	m_lHStep = rRhs.m_lHStep;
	m_lVStep = rRhs.m_lVStep;
	m_lHPosStep = rRhs.m_lHPosStep;
	m_lVPosStep = rRhs.m_lVPosStep;
	m_bVariableROI = rRhs.m_bVariableROI;
	m_lOrder = rRhs.m_lOrder;
	m_bColor = rRhs.m_bColor;
}

HRESULT SVCameraFormat::ParseAndAssignCameraFormat( const SVString& rCameraFormat )
{
	SVString CameraFormat( rCameraFormat );
	m_strName = CameraFormat;
	// GigE is set to variableROI if the camera format string contains FORMAT_7
	m_bVariableROI = (SVString::npos != CameraFormat.find( _T("FORMAT_7") ));

	// parse the format string M_xXy_type@n[.n]FPS
	// or M_xXy_type_FORMAT_7_0
	// where @n[.n]FPS is optional for some camera types
	// FPS is optional (useless) for GIGE cameras

	CameraFormat = SvUl_SF::Mid( CameraFormat, 2 );	// strip off M_ prefix;

	size_t posV =  CameraFormat.find('X');
	size_t posType = CameraFormat.find('_');

	SVString sH = SvUl_SF::Left( CameraFormat, posV );
	SVString sV = CameraFormat.substr( posV + 1, posType - posV - 1 );
	SVString sType = CameraFormat.substr( posType + 1, CameraFormat.size() - posType - 1 );
	
	m_lWidth = m_lWidthMax = atoi( sH.c_str() );
	m_lHeight = m_lHeightMax = atoi( sV.c_str() );

	if ( sType == _T("Y") )
	{
		m_eImageType = SVImageFormatMono8;
	}
	//! Note all Bayer formats will be possible BayerRG8 or 12, BayerGB8 10 or 12
	else if ( SVString::npos != sType.find(_T("RGB")) || SVString::npos != sType.find(_T("YUV")) || SVString::npos != sType.find(_T("Bayer")) )
	{
		m_eImageType = SVImageFormatRGB8888;
		m_bColor = true;
	}
	else if ( SVString::npos != sType.find(_T("Y_FORMAT_7"))  )
	{
		m_eImageType = SVImageFormatMono8;
	}
	else	// default
	{
		m_eImageType = SVImageFormatMono8;
	}
	return S_OK;
}
