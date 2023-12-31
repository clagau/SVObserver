// ******************************************************************************
// * COPYRIGHT (c) 2002 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions/SVImageFormatEnum.h"
#pragma endregion Includes

class SVCameraFormat
{
public:
	std::string m_strName;
	std::string m_strDescription;
	SvDef::SVImageFormatEnum m_eImageType;
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
	SVCameraFormat( const SVCameraFormat& rRhs );
	const SVCameraFormat& operator = ( const SVCameraFormat& rRhs );
	void AssignUserChangableValues( const SVCameraFormat& rRhs );
	void AssignConstantValues( const SVCameraFormat& rRhs );

	HRESULT ParseAndAssignCameraFormat( const std::string& rCameraFormat );

private:
	void init();
};

