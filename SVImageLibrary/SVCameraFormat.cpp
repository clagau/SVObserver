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

#include "stdafx.h"
#include "SVCameraFormat.h"

SVCameraFormat::SVCameraFormat()
{
	init();
}

void SVCameraFormat::init()
{
	eImageType=SVImageFormatUnknown;
	lWidthMax=0;
	lHeightMax=0;
	lWidth=0;
	lHeight=0;
	lHStep=1;
	lVStep=1;
	lHPos=0;
	lVPos=0;
	lHPosStep=1;
	lVPosStep=1;
	lFrameRate=0;
	lOrder=0;
	bVariableROI = false;
	bColor = false;
	lMinTransferTime = 0;
	lMaxTransferTime = 0;
	bOptimizedTransferRate_X = false;
	bOptimizedTransferRate_Y = false;

	m_lDCamFormat = -1;
	m_lDCamMode = -1;
	m_lDCamFrameRate = -1;
	m_lDCamColorFormat = -1;

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
		strDescription = rhs.strDescription;
		eImageType = rhs.eImageType;
		lWidthMax = rhs.lWidthMax;
		lHeightMax = rhs.lHeightMax;
		lWidth = rhs.lWidth;
		lHeight = rhs.lHeight;
		lHStep = rhs.lHStep;
		lVStep = rhs.lVStep;
		lHPos = rhs.lHPos;
		lVPos = rhs.lVPos;
		lHPosStep = rhs.lHPosStep;
		lVPosStep = rhs.lVPosStep;
		lFrameRate = rhs.lFrameRate;
		bVariableROI = rhs.bVariableROI;
		lOrder = rhs.lOrder;
		bColor = rhs.bColor;
		lMinTransferTime = rhs.lMinTransferTime;
		lMaxTransferTime = rhs.lMaxTransferTime;
		bOptimizedTransferRate_X = rhs.bOptimizedTransferRate_X;
		bOptimizedTransferRate_Y = rhs.bOptimizedTransferRate_Y;

		m_lDCamFormat = rhs.m_lDCamFormat;
		m_lDCamMode = rhs.m_lDCamMode;
		m_lDCamFrameRate = rhs.m_lDCamFrameRate;
		m_lDCamColorFormat = rhs.m_lDCamColorFormat;
	}
	return *this;
}

void SVCameraFormat::AssignUserChangableValues( const SVCameraFormat& rhs )
{
	lWidth = rhs.lWidth;
	lHeight = rhs.lHeight;
	lVPos = rhs.lVPos;
	lHPos = rhs.lHPos;
}

void SVCameraFormat::AssignConstantValues( const SVCameraFormat& rhs )
{
	m_strName = rhs.m_strName;
	strDescription = rhs.strDescription;
	eImageType = rhs.eImageType;
	lWidthMax = rhs.lWidthMax;
	lHeightMax = rhs.lHeightMax;
	//lWidth = rhs.lWidth;
	//lHeight = rhs.lHeight;
	lHStep = rhs.lHStep;
	lVStep = rhs.lVStep;
	//lHPos = rhs.lHPos;
	//lVPos = rhs.lVPos;
	lHPosStep = rhs.lHPosStep;
	lVPosStep = rhs.lVPosStep;
	lFrameRate = rhs.lFrameRate;
	bVariableROI = rhs.bVariableROI;
	lOrder = rhs.lOrder;
	bColor = rhs.bColor;
	lMinTransferTime = rhs.lMinTransferTime;
	lMaxTransferTime = rhs.lMaxTransferTime;
	bOptimizedTransferRate_X = rhs.bOptimizedTransferRate_X;
	bOptimizedTransferRate_Y = rhs.bOptimizedTransferRate_Y;

	m_lDCamFormat = rhs.m_lDCamFormat;
	m_lDCamMode = rhs.m_lDCamMode;
	m_lDCamFrameRate = rhs.m_lDCamFrameRate;
	m_lDCamColorFormat = rhs.m_lDCamColorFormat;
}

HRESULT SVCameraFormat::ParseAndAssignCameraFormat( const CString& sCameraFormat )
{
	CString s = sCameraFormat;
	m_strName = s;
	bVariableROI = (s.Find( "FORMAT_7" ) != -1 );

	lFrameRate = 1;

	// parse the format string M_xXy_type@n[.n]FPS
	// or M_xXy_type_FORMAT_7_0
	// where @n[.n]FPS is optional for some camera types
	// FPS is required for 1394 cameras, optional for GIGE cameras

	s = s.Mid(2);	// strip off M_ prefix;

	if ( ! bVariableROI )
	{
		// Find @ and strip off 'FPS' at end if exists
		int iFps = s.Find('@');

		if ( iFps != -1 )
		{
			CString sFps = s.Mid( iFps + 1 );
			lFrameRate = atoi( sFps );	// for now assume integral
		}
	}

	int iV = s.Find('X');
	int iType = s.Find('_');

	CString sH = s.Left( iV );
	CString sV = s.Mid( iV + 1, iType - iV - 1 );
	CString sType = s.Mid( iType + 1, s.GetLength() - iType - 1 );
	
	lWidth = lWidthMax = atoi( sH );
	lHeight = lHeightMax = atoi( sV );

	if ( sType == _T("Y") )
	{
		eImageType = SVImageFormatMono8;
	}
	else if ( sType.Find(_T("RGB")) != -1 || sType.Find(_T("YUV")) != -1 )
	{
		eImageType = SVImageFormatRGB8888;
		bColor = true;
	}
	else if ( sType.Find(_T("Y_FORMAT_7")) != -1 )
	{
		eImageType = SVImageFormatMono8;
	}
	else	// default
	{
		eImageType = SVImageFormatMono8;
	}
	return SetDCamData( sCameraFormat );
}

enum
{ 
	DCAM_FR_1_875=0,
	DCAM_FR_3_75,
	DCAM_FR_7_5,
	DCAM_FR_15,
	DCAM_FR_30,
	DCAM_FR_60,
	DCAM_FR_120,
	DCAM_FR_240,
	DCAM_FR_LAST
};

enum
{
	DCAM_CF_Y8=0,
	DCAM_CF_YUV411,
	DCAM_CF_YUV422,
	DCAM_CF_YUV444,
	DCAM_CF_RGB8,
	DCAM_CF_Y16,
	DCAM_CF_RGB16,
	DCAM_CF_LAST
};

HRESULT SVCameraFormat::SetDCamData( const CString& sCameraFormat )
{
	HRESULT l_hrOk = S_OK;

	long l_lDCamFormat = -1;
	long l_lDCamMode = -1;
	long l_lDCamFrameRate = -1;
	long l_lDCamColorFormat = -1;

	// parse the format string M_xXy_type@n[.n]FPS
	// or M_xXy_type_FORMAT_7_0
	int l_iAt = sCameraFormat.Find( '@' );

	if( l_iAt != -1 )
	{
		CString l_csFormatMode = sCameraFormat.Left( l_iAt );

		if( l_csFormatMode.CollateNoCase( "M_160X120_YUV444" ) == 0 )
		{
			l_lDCamFormat = 0;
			l_lDCamMode = 0;
			l_lDCamColorFormat = DCAM_CF_YUV444;
		}
		else if( l_csFormatMode.CollateNoCase( "M_320X240_YUV422" ) == 0 )
		{
			l_lDCamFormat = 0;
			l_lDCamMode = 1;
			l_lDCamColorFormat = DCAM_CF_YUV422;
		}
		else if( l_csFormatMode.CollateNoCase( "M_640X480_YUV411" ) == 0 )
		{
			l_lDCamFormat = 0;
			l_lDCamMode = 2;
			l_lDCamColorFormat = DCAM_CF_YUV411;
		}
		else if( l_csFormatMode.CollateNoCase( "M_640X480_YUV422" ) == 0 )
		{
			l_lDCamFormat = 0;
			l_lDCamMode = 3;
			l_lDCamColorFormat = DCAM_CF_YUV422;
		}
		else if( l_csFormatMode.CollateNoCase( "M_640X480_RGB8" ) == 0 )
		{
			l_lDCamFormat = 0;
			l_lDCamMode = 4;
			l_lDCamColorFormat = DCAM_CF_RGB8;
		}
		else if( l_csFormatMode.CollateNoCase( "M_640X480_Y" ) == 0 )
		{
			l_lDCamFormat = 0;
			l_lDCamMode = 5;
			l_lDCamColorFormat = DCAM_CF_Y8;
		}
		else if( l_csFormatMode.CollateNoCase( "M_640X480_Y16" ) == 0 )
		{
			l_lDCamFormat = 0;
			l_lDCamMode = 6;
			l_lDCamColorFormat = DCAM_CF_Y16;
		}
		else if( l_csFormatMode.CollateNoCase( "M_800X600_YUV422" ) == 0 )
		{
			l_lDCamFormat = 1;
			l_lDCamMode = 0;
			l_lDCamColorFormat = DCAM_CF_YUV422;
		}
		else if( l_csFormatMode.CollateNoCase( "M_800X600_RGB8" ) == 0 )
		{
			l_lDCamFormat = 1;
			l_lDCamMode = 1;
			l_lDCamColorFormat = DCAM_CF_RGB8;
		}
		else if( l_csFormatMode.CollateNoCase( "M_800X600_Y" ) == 0 )
		{
			l_lDCamFormat = 1;
			l_lDCamMode = 2;
			l_lDCamColorFormat = DCAM_CF_Y8;
		}
		else if( l_csFormatMode.CollateNoCase( "M_1024X768_YUV422" ) == 0 )
		{
			l_lDCamFormat = 1;
			l_lDCamMode = 3;
			l_lDCamColorFormat = DCAM_CF_YUV422;
		}
		else if( l_csFormatMode.CollateNoCase( "M_1024X768_RGB8" ) == 0 )
		{
			l_lDCamFormat = 1;
			l_lDCamMode = 4;
			l_lDCamColorFormat = DCAM_CF_RGB8;
		}
		else if( l_csFormatMode.CollateNoCase( "M_1024X768_Y" ) == 0 )
		{
			l_lDCamFormat = 1;
			l_lDCamMode = 5;
			l_lDCamColorFormat = DCAM_CF_Y8;
		}
		else if( l_csFormatMode.CollateNoCase( "M_800X600_Y16" ) == 0 )
		{
			l_lDCamFormat = 1;
			l_lDCamMode = 6;
			l_lDCamColorFormat = DCAM_CF_Y16;
		}
		else if( l_csFormatMode.CollateNoCase( "M_1024X768_Y16" ) == 0 )
		{
			l_lDCamFormat = 1;
			l_lDCamMode = 7;
			l_lDCamColorFormat = DCAM_CF_Y16;
		}
		else if( l_csFormatMode.CollateNoCase( "M_1280X960_YUV422" ) == 0 )
		{
			l_lDCamFormat = 2;
			l_lDCamMode = 0;
			l_lDCamColorFormat = DCAM_CF_YUV422;
		}
		else if( l_csFormatMode.CollateNoCase( "M_1280X960_RGB8" ) == 0 )
		{
			l_lDCamFormat = 2;
			l_lDCamMode = 1;
			l_lDCamColorFormat = DCAM_CF_RGB8;
		}
		else if( l_csFormatMode.CollateNoCase( "M_1280X960_Y" ) == 0 )
		{
			l_lDCamFormat = 2;
			l_lDCamMode = 2;
			l_lDCamColorFormat = DCAM_CF_Y8;
		}
		else if( l_csFormatMode.CollateNoCase( "M_1600X1200_YUV422" ) == 0 )
		{
			l_lDCamFormat = 2;
			l_lDCamMode = 3;
			l_lDCamColorFormat = DCAM_CF_YUV422;
		}
		else if( l_csFormatMode.CollateNoCase( "M_1600X1200_RGB8" ) == 0 )
		{
			l_lDCamFormat = 2;
			l_lDCamMode = 4;
			l_lDCamColorFormat = DCAM_CF_RGB8;
		}
		else if( l_csFormatMode.CollateNoCase( "M_1600X1200_Y" ) == 0 )
		{
			l_lDCamFormat = 2;
			l_lDCamMode = 5;
			l_lDCamColorFormat = DCAM_CF_Y8;
		}
		else if( l_csFormatMode.CollateNoCase( "M_1280X960_Y16" ) == 0 )
		{
			l_lDCamFormat = 2;
			l_lDCamMode = 6;
			l_lDCamColorFormat = DCAM_CF_Y16;
		}
		else if( l_csFormatMode.CollateNoCase( "M_1600X1200_Y16" ) == 0 )
		{
			l_lDCamFormat = 2;
			l_lDCamMode = 7;
			l_lDCamColorFormat = DCAM_CF_Y16;
		}
		else
		{
			l_hrOk = S_FALSE;
		}

		if( l_hrOk == S_OK )
		{
			int l_iFPSCount = sCameraFormat.GetLength() - ( l_iAt + 1 );

			CString l_csFrameRate = sCameraFormat.Right( l_iFPSCount );

			if( l_csFrameRate.CollateNoCase( "1.875FPS" ) == 0 )
			{
				l_lDCamFrameRate = DCAM_FR_1_875;
			}
			else if( l_csFrameRate.CollateNoCase( "3.75FPS" ) == 0 )
			{
				l_lDCamFrameRate = DCAM_FR_3_75;
			}
			else if( l_csFrameRate.CollateNoCase( "7.5FPS" ) == 0 )
			{
				l_lDCamFrameRate = DCAM_FR_7_5;
			}
			else if( l_csFrameRate.CollateNoCase( "15FPS" ) == 0 )
			{
				l_lDCamFrameRate = DCAM_FR_15;
			}
			else if( l_csFrameRate.CollateNoCase( "30FPS" ) == 0 )
			{
				l_lDCamFrameRate = DCAM_FR_30;
			}
			else if( l_csFrameRate.CollateNoCase( "60FPS" ) == 0 )
			{
				l_lDCamFrameRate = DCAM_FR_60;
			}
			else if( l_csFrameRate.CollateNoCase( "120FPS" ) == 0 )
			{
				l_lDCamFrameRate = DCAM_FR_120;
			}
			else if( l_csFrameRate.CollateNoCase( "240FPS" ) == 0 )
			{
				l_lDCamFrameRate = DCAM_FR_240;
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	else
	{
		// parse the format string M_xXy_type@n[.n]FPS
		// or M_xXy_type_FORMAT_7_0
		int l_iFormat7 = sCameraFormat.Find( "_FORMAT_7_" );

		if( l_iFormat7 != -1 )
		{
			int l_iModeCount = sCameraFormat.GetLength() - ( sCameraFormat.ReverseFind( '_' ) + 1 );

			l_lDCamMode = atoi( sCameraFormat.Right( l_iModeCount ) );
			l_lDCamFormat = 7;

			if( sCameraFormat.Find( "_Y_FORMAT_7_" ) != -1 )
			{
				l_lDCamColorFormat = DCAM_CF_Y8;
			}
			else if( sCameraFormat.Find( "_YUV411_FORMAT_7_" ) != -1 )
			{
				l_lDCamColorFormat = DCAM_CF_YUV411;
			}
			else if( sCameraFormat.Find( "_YUV422_FORMAT_7_" ) != -1 )
			{
				l_lDCamColorFormat = DCAM_CF_YUV422;
			}
			else if( sCameraFormat.Find( "_YUV444_FORMAT_7_" ) != -1 )
			{
				l_lDCamColorFormat = DCAM_CF_YUV444;
			}
			else if( sCameraFormat.Find( "_RGB8_FORMAT_7_" ) != -1 )
			{
				l_lDCamColorFormat = DCAM_CF_RGB8;
			}
			else if( sCameraFormat.Find( "_Y16_FORMAT_7_" ) != -1 )
			{
				l_lDCamColorFormat = DCAM_CF_Y16;
			}
			else if( sCameraFormat.Find( "_RGB16_FORMAT_7_" ) != -1 )
			{
				l_lDCamColorFormat = DCAM_CF_RGB16;
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	if( l_hrOk == S_OK )
	{
		m_lDCamFormat = l_lDCamFormat;
		m_lDCamMode = l_lDCamMode;
		m_lDCamFrameRate = l_lDCamFrameRate;
		m_lDCamColorFormat = l_lDCamColorFormat;
	}

	return l_hrOk;
}

