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

#include "SVContainerLibrary/SVVector.h"
#include "SVExtentPointStruct.h"

struct SVExtentLineStruct  
{
	SVExtentLineStruct();
	SVExtentLineStruct( const SVExtentLineStruct &p_rsvValue );
	~SVExtentLineStruct();

	const SVExtentLineStruct &operator=( const SVExtentLineStruct &p_rsvValue );

	HRESULT Initialize();

	SVExtentPointStructCArray m_svPointArray;
	COLORREF m_dwColor;
	
	bool m_bIsAngleValid;
	double m_dAngle;
};

typedef SVVector< SVExtentLineStruct > SVExtentLineStructCArray;


