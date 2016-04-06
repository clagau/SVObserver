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

#pragma once

#pragma region Includes
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "SVUtilityLibrary/SVString.h"
#include "ObjectInterfaces/SVImageEnums.h"
#pragma endregion Includes

class SVCameraFormat
{
public:
	SVString m_strName;
	SVString m_strDescription;
	SVImageFormatEnum m_eImageType;
	long m_lWidthMax;
	long m_lHeightMax;
	long m_lWidth;
	long m_lHeight;
	long m_lHStep;
	long m_lVStep;
	long m_lHPos;
	long m_lVPos;
	long m_lHPosStep;
	long m_lVPosStep;
	bool m_bVariableROI;
	long m_lOrder;
	bool m_bColor;

	SVCameraFormat();
	SVCameraFormat( const SVCameraFormat& rhs );
	const SVCameraFormat& operator = ( const SVCameraFormat& rhs );
	void AssignUserChangableValues( const SVCameraFormat& rhs );
	void AssignConstantValues( const SVCameraFormat& rhs );

	HRESULT ParseAndAssignCameraFormat( const CString& sCameraFormat );

private:
	void init();
};

