//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentLineStruct
//* .File Name       : $Workfile:   SVExtentLineStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:39:52  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVUtilityLibrary/SVPoint.h"
#pragma endregion Includes

struct SVExtentLineStruct  
{
	SVExtentLineStruct();
	SVExtentLineStruct( const SVExtentLineStruct &p_rsvValue );
	~SVExtentLineStruct();

	const SVExtentLineStruct &operator=( const SVExtentLineStruct &p_rsvValue );

	void Initialize();

	std::vector<SVPoint<double>> m_PointVector;
	COLORREF m_dwColor;
	
	bool m_bIsAngleValid;
	double m_dAngle;
};

typedef std::vector<SVExtentLineStruct > SVExtentLineStructVector;


