// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCameraFormat
// * .File Name       : $Workfile:   SVCameraFormat.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:37:18  $
// ******************************************************************************

#ifndef SVCAMERAFORMAT_H
#define SVCAMERAFORMAT_H

#include "SVOMFCLibrary/SVTemplate.h"
#include "SVHBitmapUtilitiesLibrary/SVHBitmapUtilities.h"
#include "SVUtilityLibrary/SVString.h"
#include "ObjectInterfaces/SVImageEnums.h"

class SVCameraFormat
{
public:
	SVString m_strName;
	SVString strDescription;
	SVImageFormatEnum eImageType;
	long lWidthMax;
	long lHeightMax;
	long lWidth;
	long lHeight;
	long lHStep;
	long lVStep;
	long lHPos;
	long lVPos;
	long lHPosStep;
	long lVPosStep;
	long lFrameRate;
	bool bVariableROI;
	long lOrder;
	bool bColor;
	long lMinTransferTime;
	long lMaxTransferTime;
	bool bOptimizedTransferRate_X;
	bool bOptimizedTransferRate_Y;

	long m_lDCamFormat;
	long m_lDCamMode;
	long m_lDCamFrameRate;
	long m_lDCamColorFormat;

	SVCameraFormat();
	SVCameraFormat( const SVCameraFormat& rhs );
	const SVCameraFormat& operator = ( const SVCameraFormat& rhs );
	void AssignUserChangableValues( const SVCameraFormat& rhs );
	void AssignConstantValues( const SVCameraFormat& rhs );

	HRESULT ParseAndAssignCameraFormat( const CString& sCameraFormat );

private:

	HRESULT SetDCamData( const CString& sCameraFormat );

	void init();
};


#endif

