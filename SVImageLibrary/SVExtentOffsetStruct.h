//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentOffsetStruct
//* .File Name       : $Workfile:   SVExtentOffsetStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:40:00  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVPoint.h"
#pragma endregion Includes

namespace SvIe
{
class SVImageClass;
}

struct SVExtentOffsetStruct
{
	SVExtentOffsetStruct()
	{
		Initialize();
	};

	SVExtentOffsetStruct( const SVExtentOffsetStruct& p_rsvData )
	{
		*this = p_rsvData;
	};

	const SVExtentOffsetStruct& operator=( const SVExtentOffsetStruct& p_rsvData )
	{
		m_psvRootImage = p_rsvData.m_psvRootImage;
		m_psvImage = p_rsvData.m_psvImage;

		m_csImageName = p_rsvData.m_csImageName;

		m_bIsLinear = p_rsvData.m_bIsLinear;
		m_bAlwaysUpdate = p_rsvData.m_bAlwaysUpdate;

		m_svRotationPoint = p_rsvData.m_svRotationPoint;
		m_svOffset = p_rsvData.m_svOffset;

		m_dRotationAngle = p_rsvData.m_dRotationAngle;

		return *this;
	};

	HRESULT Initialize()
	{
		m_psvRootImage = nullptr;
		m_psvImage = nullptr;

		m_csImageName.clear();

		m_bIsLinear = true;
		m_bAlwaysUpdate = false;

		m_svRotationPoint.clear();
		m_svOffset.clear();

		m_dRotationAngle = 0.0;

		return S_OK;
	};

	SvIe::SVImageClass *m_psvRootImage;
	SvIe::SVImageClass *m_psvImage;

	std::string m_csImageName;

	bool m_bIsLinear;
	bool m_bAlwaysUpdate;

	SVPoint<double> m_svRotationPoint;
	SVPoint<double> m_svOffset;

	double m_dRotationAngle;
};

